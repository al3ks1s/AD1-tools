# AD1-tools
CLI Tools to open AccessData AD1 forensic images on linux.

Features :
- File extraction
- Digest verification
- Segmented files handling
- Filesystem mounting of images through FUSE

Todo : 
- Image decryption

See https://al3ks1s.fr/posts/adventures-part-1/ for explanations about the AD1 Format

### Installation

```
$ git clone git@github.com:al3ks1s/AD1-tools.git
$ cd AD1-tools
$ ./autogen.sh
$ cd build
$ ../configure
$ make install
```

## Acknowledgements 

- TMairi for their AD1 format dissecting : https://tmairi.github.io/posts/dissecting-the-ad1-file-format/ (Use the wayback machine)
- Pyad1 tool by pcbje : https://github.com/pcbje/pyad1
