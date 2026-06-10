# HuffmanArchiver
An archiver utility based on Huffman coding, which supports compression and decompression of any byte sequence with fixed symbol size ranging from 1 to 4 bytes.

## Features

* Lossless file compression and decompression
* Canonical Huffman codes
* Configurable symbol size (1–4 bytes)
* Support for arbitrary binary files
* Compression metadata stored in file header
* Modular unit tests
* Integration and benchmarking scripts

## Building

### Requirements

* GCC
* GNU Make

### Compile

```bash
make
```

The executable will be generated as:

```bash
./huffArchiver
```

## Usage

### Compression

```bash
./huffArchiver compress <input> <output> [symbol_size]
```

Parameters:

* `input` – source file
* `output` – compressed file
* `symbol_size` – symbol size in bytes (1–4)

Example:

```bash
./huffArchiver compress input.txt output.huff 1
```

Note: If the symbol size is not specified, the program runs with symbol size 1.

### Decompression

```bash
./huffArchiver decompress <input> <output>
```

Example:

```bash
./huffArchiver decompress output.huff restored.txt
```

Note: During decompression, the symbol size specified at compression time is read from the archive header. Any symbol size passed on the command line is ignored.


## Archive Format

The archive contains:

* Original file size
* Symbol size
* Number of unique symbols
* Symbol table
* Huffman code lengths
* Compressed data stream

The decompressor reconstructs canonical Huffman codes from the stored code lengths.


## Project Structure

```text
.
├── src/
│   ├── main.c
│   ├── compress.c
│   ├── decompress.c
│   ├── huffman.c
│   └── huffman.h
├── test/
│   ├── modular/
│   └── test_data/
├── scripts/
├── Makefile
├── LICENSE
└── README.md
```

## Testing

### Modular Tests

Build and run the modular tests:

```bash
make test
```

The test suite covers:

* Priority queue operations
* Hash table operations
* Huffman tree construction
* Code length generation
* Canonical code generation

### Round-Trip Tests

Compression/decompression correctness is verified by:

1. Compressing a file
2. Decompressing the archive
3. Comparing the restored file against the original

## Performance Evaluation

Benchmark scripts are provided for measuring:

* Compression time
* Decompression time
* Compression ratio

For each test case:

* Multiple runs are performed
* Average execution time is calculated
* Standard deviation is calculated

Results are exported to:

```text
test/results.csv
```

CSV columns:

```text
file;
symbol_size;
original_size;
compressed_size;
avg_compress_s;
stddev_compress;
avg_decompress_s;
stddev_decompress;
ratio
```

## Supported Symbol Sizes

| Symbol Size | Description                          |
| ----------- | ------------------------------------ |
| 1 byte      | Standard byte-wise Huffman coding    |
| 2 bytes     | Pairs of bytes treated as symbols    |
| 3 bytes     | Triplets of bytes treated as symbols |
| 4 bytes     | Four-byte symbols                    |

Larger symbol sizes may improve compression on some file types at the cost of increased memory usage and longer processing times.

## License

This project is released under the MIT License.
