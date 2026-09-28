# AD1-tools
CLI Tools to open, extract and mount AccessData AD1 images on linux. 

Features :
- File extraction
- Digest verification
- Segmented files handling
- Filesystem mounting of images through FUSE

Todo : 
- Image decryption

See https://al3ks1s.fr/posts/adventures-part-1/ for explanations about the AD1 Format


### Installation
 
#### Install from deb

Grab the [latest release](https://github.com/al3ks1s/AD1-tools/releases)
```
dpkg --install ad1tools_1.0.1-1_amd64.deb
```

#### Install from Git

```
$ git clone https://github.com/al3ks1s/AD1-tools.git
$ cd AD1-tools
$ ./autogen.sh
$ cd build
$ ../configure
$ make install
```

### Usage

#### ad1info

```
Usage: ad1info [OPTION...] ad1info [OPTIONS] -i FILENAME
Print important information of an AccessData AD1 Logical Image.

  -i, --input=FILE           Input AD1 file.
  -q, --quiet                Produce a quiet output.
  -t, --tree                 Produce a tree of the file hierarchy.
  -v, --verbose              Blurt a lotta text.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
``` 

#### ad1verify

```Usage: ad1verify [OPTION...] ad1verify [OPTIONS] -i FILENAME
Checks the integrity of an AccessData AD1 Logical Image.

  -f, --full-ckeck           Check all files hashes against their saved hash.
  -i, --input=FILE           Input AD1 file.
  -q, --quiet                Produce a quiet output.
  -s, --sha1                 Compare sha1 hashes instead of md5.
  -v, --verbose              Blurt a lotta text.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
  ```

#### ad1extract

```Usage: ad1extract [OPTION...] ad1extract [OPTIONS] -i FILENAME
Extract the content of an AccessData AD1 Logical Image.

  -d, --output-dir=DIR       Directory to extract the files to.
  -i, --input=FILE           Input AD1 file.
  -m, --metadata             Apply metadatas to extracted files (eg:
                             timestamps).
  -q, --quiet                Produce a quiet output.
  -s, --skip-hash            Skip the integrity check of a file on extraction.
  -v, --verbose              Blurt a lotta text.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
  ```

#### ad1mount
```Usage: ad1mount [OPTION...] ad1verify [OPTIONS] -i FILENAME
Mounts an AccessData AD1 Logical Image as a read only filesystem.

  -i, --input=FILE           Input AD1 file.
  -m, --mnt=DIR              Mount target.
  -q, --quiet                Produce a quiet output.
  -v, --verbose              Blurt a lotta text.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
  ```

Unmount using fusermount : `fusermount -u [mountpoint]`

#### ad1decrypt
Not yet supported

## Acknowledgements 

- TMairi for their AD1 format dissecting : https://tmairi.github.io/posts/dissecting-the-ad1-file-format/ (Use the wayback machine)
- Pyad1 tool by pcbje : https://github.com/pcbje/pyad1
