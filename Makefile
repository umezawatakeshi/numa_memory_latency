all: numa_memory_latency

clean:
	rm -f numa_memory_latency

numa_memory_latency: numa_memory_latency.cc
	@# not -std=c++11 for compatibility with GCC 4.4.7
	g++ -o numa_memory_latency -std=c++0x -O2 -g numa_memory_latency.cc -lnuma
