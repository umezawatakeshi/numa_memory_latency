numa_memory_latency
===================

特に NUMA 環境でメモリのレイテンシを計測する、とても簡単なツール。

(see [README.md](./README.md) in English)

## 必要環境

- C++11 (GCC 4.4.7 の C++0x もサポートします)
- libnuma

## ビルド方法

単に make するだけです。

## 使い方

```
numa_memory_latency [bufsize] [nloop]
numa_memory_latency -h
```

_bufsize_ は、メモリアクセスするバッファのサイズ (KiB) です。
 計測するメモリのサイズより小さく、
 そのメモリの1つ低いレベルのサイズより十分大きい必要があります。
 例えば、L3 キャッシュのレイテンシを計測したい場合、
 _bufsize_ は L3 キャッシュのサイズより小さく、
 L2 キャッシュのサイズより十分大きい必要があります。

_nloop_ はループ回数です。
正確な結果を得るために、ある程度大きな値を指定する必要があります。

### 例

[Xeon E5 2630L v2](https://ark.intel.com/ja/products/75791/Intel-Xeon-Processor-E5-2630L-v2-15M-Cache-2_40-GHz) は

- 32KiB L1 data cache /core
- 256KiB L2 cache /core
- 15MiB L3 cache /socket

を持ちますが、このデュアルプロセッサ環境において、

- `numa_memory_latency 16 8192` で L1 データキャッシュ、
- `numa_memory_latency 200 2048` で L2 キャッシュ、
- `numa_memory_latency 1200 1024` で L3 キャッシュ、
- `numa_memory_latency 256000 256` でメインメモリ（ローカルおよびリモートノード）、

のレイテンシを計測することができます。

## ライセンス

[GNU GPL v2](./LICENSE)

## 作者

[梅澤威志](https://github.com/umezawatakeshi)
