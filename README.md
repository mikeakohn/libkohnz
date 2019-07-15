# libkohnz

Library for making gzip files.

Introduction
============

While working on a project that creates large output files that has
a lot of redundant data.. redundant data that could easily be identified
and tracked.. I had this idea.  Instead of running gzip, which has to
search for redundacies on its own to compress a file, I could create
a library that creates gzip files with single function calls that either
writes data out, or give a distance / length to where redundant data is
needed to be repeated.

I created libkohnz to be able to compress these files with low CPU
and memory overhead (since redundancies don't have to be searched for).

About gzip
==========

Inside gzip files there can be 3 types of data:

* Uncompressed
* Fixed huffman data
* Dynamic huffman data

The fixed huffman data uses huffman codes that are hardcoded in
the compressor / decompressor executable.  The codes are listed
in RFC1951.

Dynamic huffman includes the huffman tables inside the .gz file.
Because of his, dynamic huffman can get higher compression since
the huffman codes can be optimized for the data.  If the file
being compressed is a text file and the letter "a" appears often,
the bit length of this code can be really small while a letter
like "z" that almost never appears can have a longer length.

libkohnz
========

Currently libkohnz only supports fixed huffman, but I plan on
adding dynamic huffman sometime.

The /sample directory in the repository has some exmples how
to use libkohnz.  A simple example here would be sample_01a.c.
This program basically creates a file with the text "MIKEMIKE"
in it:

    kohnz_start_fixed_block(kohnz, 1);
    kohnz_write_fixed(kohnz, (const uint8_t *)"MIKE", 4);
    kohnz_write_fixed_lz77(kohnz, 4, 4);
    kohnz_end_fixed_block(kohnz);

The final flag tells libkohnz that this will be the last block
of compressed data.  It's possible to have multiple blocks compressed
in different ways (fixed, uncompressed, dynamic with a different table).
The second line adds "MIKE" to the file.  The third line tells libkohnz
to go backwards in the file by 4 bytes and copy 4 bytes from there and
add it to the start of the output when uncompressing.  Every gz/deflate
compressed block must have a CRC so there needs to be a call to
kohnz_build_crc32():

    kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEMIKE", 8);

Note that, although it gets better CPU performance making this a single
call, it's possible to build the CRC with multiple calls:

    kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEM", 5);
    kohnz_build_crc32(kohnz, (const uint8_t *)"IKE", 3);

There is another example called build_json.c which builds a text
JSON file using libkohnz.  Because the output JSON file always has
the same keys with the same indentation, the sample program keeps
track of the offset in what would be the output file of where the
names are and when creating the output file will simply give a
distance length to each tag.  The output JSON file could look
like this:

    [
      {
        "airspeed": 300,
        "heading": 181,
        "altitude": 35057,
        "count": 0,
        "errors": 0
      },
      {
    <length=21 distance=105>
    <length=15 distance=105>182,
    <length=16 distance=105>35195,

I also added a program called parse_gz which can be used to debug
gzip files, printing out the contents of the files including
dynamic hufffman tables.

