#define _XOPEN_SOURCE 700
#include <openssl/md5.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "libad1_file_reader.h"

ad1_cache_entry ad1_file_cache[CACHE_SIZE];

pthread_mutex_t cache_lock;

void
free_cache() {

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (ad1_file_cache[i].data != 0) {
            free(ad1_file_cache[i].data);
        }
        ad1_file_cache[i].counter = 0;
        ad1_file_cache[i].data = 0;
        ad1_file_cache[i].cached_item = 0;
    }
}

// This ass is *not* a good cache algorithm, feel free to make one better
ad1_cache_entry*
search_cache(ad1_session* session, ad1_item_header* ad1_item) {

    pthread_mutex_lock(&cache_lock);

    for (int i = 0; i < CACHE_SIZE; i++) {

        if (ad1_file_cache[i].cached_item == ad1_item) {

            ad1_file_cache[i].counter++;

            pthread_mutex_unlock(&cache_lock);
            return &ad1_file_cache[i];
        } else if (ad1_file_cache[i].counter > 0) {
            ad1_file_cache[i].counter--;
        }

        if (ad1_file_cache[i].counter == 0 && ad1_file_cache[i].cached_item != 0) {
            free(ad1_file_cache[i].data);
            ad1_file_cache[i].data = 0;
            ad1_file_cache[i].cached_item = 0;
            ad1_file_cache[i].counter = 0;
        }
    }

    pthread_mutex_unlock(&cache_lock);
    return 0;
}

void
cache_data(ad1_item_header* ad1_item, unsigned char* data) {

    pthread_mutex_lock(&cache_lock);

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (ad1_file_cache[i].counter == 0) {
            ad1_file_cache[i].counter = 1;
            ad1_file_cache[i].cached_item = ad1_item;
            ad1_file_cache[i].data = data;

            pthread_mutex_unlock(&cache_lock);
            return;
        }
    }

    pthread_mutex_unlock(&cache_lock);
}

unsigned char*
read_file_data(ad1_session* session, ad1_item_header* ad1_item) {

    unsigned long chunk_numbers;
    unsigned char* file_data = NULL;
    unsigned long data_index = 0;

    if (ad1_item->decompressed_size == 0) {
        file_data = calloc(1, sizeof(unsigned char));
        return file_data;
    }

    ad1_cache_entry* cached_item = search_cache(session, ad1_item);

    if (cached_item != 0) {
        return cached_item->data;
    }

    file_data = calloc(ad1_item->decompressed_size, sizeof(unsigned char));

    chunk_numbers = arbitrary_read_long_little_endian(session, ad1_item->zlib_metadata_addr);

    /* 
     * The last address in the zlib metadata points at the end of the last chunk, 
     * as it is usually smaller than the max chunk size and points to the same address as the metadata start
     */
    unsigned long addresses[chunk_numbers + 1];

    for (int i = 0; i < chunk_numbers + 1; i++) {
        addresses[i] = arbitrary_read_long_little_endian(session, ad1_item->zlib_metadata_addr + ((i + 1) * 0x08));
    }

    for (int i = 0; i < chunk_numbers; i++) {
        data_index += read_zlib_chunk(session, file_data + data_index, addresses[i], addresses[i + 1] - addresses[i],
                                      ad1_item->decompressed_size);
    }

    cache_data(ad1_item, file_data);

    return file_data;
}

long
read_zlib_chunk(ad1_session* session, unsigned char* output_data_ptr, unsigned long offset,
                unsigned int zlib_chunk_size, unsigned long decompressed_size) {

    unsigned char* compressed_data;
    unsigned long chunk_decompressed_size = 0;

    compressed_data = calloc(zlib_chunk_size, sizeof(char));

    arbitrary_read(session, compressed_data, zlib_chunk_size, offset);

    chunk_decompressed_size = zlib_inflate(compressed_data, zlib_chunk_size, output_data_ptr, decompressed_size);

    free(compressed_data);
    compressed_data = NULL;

    return chunk_decompressed_size;
}

long
zlib_inflate(unsigned char* compressed_data, unsigned int compressed_data_size, unsigned char* output,
             unsigned long decompressed_size) {

    int ret;
    unsigned long have;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);

    strm.avail_in = compressed_data_size;
    strm.next_in = compressed_data;

    strm.avail_out = decompressed_size;
    strm.next_out = output;

    ret = inflate(&strm, Z_NO_FLUSH);

    assert(ret != Z_STREAM_ERROR); /* state not clobbered */
    switch (ret) {
        case Z_NEED_DICT: ret = Z_DATA_ERROR; /* and fall through */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR: (void)inflateEnd(&strm); return ret;
    }

    have = decompressed_size - strm.avail_out;

    (void)inflateEnd(&strm);

    return have;
}

void
stat_ad1_file(ad1_session* session, ad1_item_header* ad1_item, struct stat* stbuf) {

    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();

    stbuf->st_nlink = 1;

    stbuf->st_size = ad1_item->decompressed_size;

    if (ad1_item->item_type == AD1_FOLDER_SIGNATURE) {
        stbuf->st_mode = S_IFDIR | 0555;
    } else {
        stbuf->st_mode = S_IFREG | 0440;
    }

    stbuf->st_blksize = 4096;
    stbuf->st_blocks = 1;

    ad1_metadata* metadata = ad1_item->first_metadata;

    while (metadata != NULL) {

        switch (metadata->category) {
            case HASH_INFO: break;
            case ITEM_TYPE: break;
            case ITEM_SIZE: break;
            case WINDOWS_FLAGS: break;
            case TIMESTAMP:
                switch (metadata->key) {
                    case ACCESS: parse_timestamp(&stbuf->st_atim, metadata->data); break;
                    case CHANGE: parse_timestamp(&stbuf->st_ctim, metadata->data); break;
                    case MODIFIED: parse_timestamp(&stbuf->st_mtim, metadata->data); break;
                };
        }

        metadata = metadata->next_metadata;
    };
}

void
parse_timestamp(struct timespec* time, const char* time_s) {

    struct tm time_tm;

    strptime(time_s, "%Y%m%dT%H%M%S", &time_tm);

    time->tv_sec = mktime(&time_tm);
}