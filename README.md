numa_memory_latency
===================

Very simple tool to measure memory latency, especially in NUMA environment.

(see [README.ja.md](./README.ja.md) in Japanese)

## Requirement

- C++11 (C++0x on GCC 4.4.7 is also supported)
- libnuma

## How to Build

Just make!

## Usage

```
numa_memory_latency [bufsize] [nloop]
numa_memory_latency -h
```

_bufsize_ is the buffer size (in KiB) that walk memory access.
 This must be less than the size of memory that you want to measure
 and should be much greater than the size of the one lower level memory;
 for example, if you want to measure latency of L3 cache,
 _bufsize_ must be less than the size of L3 cache
 and should be much greater than the size of L2 cache.

_nloop_ is the number of walk loop.
 You should specify enough large number to get accurate result.

### Example

On Dual [Xeon E5 2630L v2](https://ark.intel.com/ja/products/75791/Intel-Xeon-Processor-E5-2630L-v2-15M-Cache-2_40-GHz) system, which have

- 32KiB L1 data cache /core
- 256KiB L2 cache /core
- 15MiB L3 cache /socket

you can:

- `numa_memory_latency 16 8192` to measure L1 data cache latency.
- `numa_memory_latency 200 2048` to measure L2 cache latency.
- `numa_memory_latency 1200 1024` to measure L3 cache latency.
- `numa_memory_latency 256000 256` to measure main memory latency (on both local and remote node).

## Licence

[GNU GPL v2](./LICENSE)

## Author

[UMEZAWA Takeshi](https://github.com/umezawatakeshi)
