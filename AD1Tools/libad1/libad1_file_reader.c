#include "libad1_file_reader.h"
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

unsigned char*
read_file_data(FILE* ad1_file, ad1_item_header* ad1_item) {

    unsigned long chunk_numbers;
    unsigned char* file_data = NULL;
    unsigned long data_index = 0;

    if (ad1_item->decompressed_size == 0) {
        file_data = (unsigned char*)calloc(1, sizeof(unsigned char));
        return file_data;
    }

    file_data = (unsigned char*)calloc(ad1_item->decompressed_size, sizeof(unsigned char));

    chunk_numbers = read_long_little_endian(ad1_file, ad1_item->zlib_metadata_addr + AD1_LOGICAL_MARGIN);

    /* 
     * The last address in the zlib metadata points at the end of the last chunk, 
     * as it is usually smaller than the max chunk size and points to the same address as the metadata start
     */
    unsigned long addresses[chunk_numbers + 1];

    for (int i = 0; i < chunk_numbers + 1; i++) {
        addresses[i] = read_long_little_endian(ad1_file,
                                               ad1_item->zlib_metadata_addr + AD1_LOGICAL_MARGIN + ((i + 1) * 0x08));
    }

    for (int i = 0; i < chunk_numbers; i++) {
        data_index += read_zlib_chunk(ad1_file, file_data + data_index, addresses[i], addresses[i + 1] - addresses[i],
                                      ad1_item->decompressed_size);
    }

    return file_data;
}

long
read_zlib_chunk(FILE* ad1_file, unsigned char* output_data_ptr, unsigned long offset, unsigned int zlib_chunk_size,
                unsigned long decompressed_size) {

    unsigned char* compressed_data;
    unsigned long chunk_decompressed_size = 0;

    compressed_data = (unsigned char*)calloc(zlib_chunk_size, sizeof(char));

    fseek(ad1_file, offset + AD1_LOGICAL_MARGIN, SEEK_SET);
    fread(compressed_data, 1, zlib_chunk_size, ad1_file);

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
