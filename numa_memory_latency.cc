/*
 * numa_memory_latency
 * Copyright (c) 2017 UMEZAWA Takeshi
 * This software is licensed under GNU GPL version 2 or later.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <vector>
#include <random>
#include <algorithm>

#include <numa.h>

#define cachelinesize 64 // XXX
union CACHELINE {
	char cacheline[cachelinesize];
	volatile CACHELINE* next;
};

#define REPT4(x)    do { x; x; x; x; } while(0)
#define REPT16(x)   do { REPT4(x);   REPT4(x);   REPT4(x);   REPT4(x);   } while(0);
#define REPT64(x)   do { REPT16(x);  REPT16(x);  REPT16(x);  REPT16(x);  } while(0);
#define REPT256(x)  do { REPT64(x);  REPT64(x);  REPT64(x);  REPT64(x);  } while(0);
#define REPT1024(x) do { REPT256(x); REPT256(x); REPT256(x); REPT256(x); } while(0);

size_t bufsize = 256 * 1024 * 1024;
size_t nloop = 256 * 1024;

std::vector<size_t> offsets;

volatile CACHELINE* walk(volatile CACHELINE* start)
{
	volatile CACHELINE* p = start;
	for (size_t i = 0; i < nloop; ++i) {
		REPT1024(p = p->next);
	}
	return p;
}

void bench(int tasknode, int memnode)
{
	struct timespec ts_begin, ts_end, ts_elapsed;

	printf("bench(task=%d, mem=%d)\n", tasknode, memnode);

	if (numa_run_on_node(tasknode) != 0) {
		printf("failed to run on node: %s\n", strerror(errno));
		return;
	}

	CACHELINE* const buf = (CACHELINE*)numa_alloc_onnode(bufsize, memnode);
	if (buf == NULL) {
		printf("failed to allocate memory\n");
		return;
	}

	for (size_t i = 0; i < offsets.size() - 1; ++i) {
		buf[offsets[i]].next = buf + offsets[i+1];
	}
	buf[offsets[offsets.size() - 1]].next = buf;

	clock_gettime(CLOCK_MONOTONIC, &ts_begin);
	walk(buf);
	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	ts_elapsed.tv_nsec = ts_end.tv_nsec - ts_begin.tv_nsec;
	ts_elapsed.tv_sec = ts_end.tv_sec - ts_begin.tv_sec;
	if (ts_elapsed.tv_nsec < 0) {
		--ts_elapsed.tv_sec;
		ts_elapsed.tv_nsec += 1000*1000*1000;
	}
	double elapsed = ts_elapsed.tv_sec + 0.000000001 * ts_elapsed.tv_nsec;
	printf("took %fsec. %fns/load\n", elapsed, elapsed/(1024*nloop)*(1000*1000*1000));
	
	numa_free(buf, bufsize);
}

struct RND {
	std::mt19937 mt;
	RND() : mt(time(NULL)) {}
	std::mt19937::result_type operator()(std::mt19937::result_type n) { return mt() % n; }
} r;

void usage(const char* prog)
{
	printf("usage: %s [-h] [bufsize] [nloop]\n", prog);
}

int main(int argc, char* argv[])
{
	int ch;

	while ((ch = getopt(argc, argv, "h")) != -1) {
		switch (ch) {
		case 'h':
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0)
		bufsize = atoi(argv[2]) * 1024;
	if (argc > 1)
		nloop = atoi(argv[1]) * 1024;

	offsets.resize(bufsize / cachelinesize);
	for (size_t i = 0; i < offsets.size(); ++i)
		offsets[i] = i;
	std::random_shuffle(offsets.begin() + 1, offsets.end(), r);

	printf("benchmark bufsize=%zuKiB, nloop=%zuKi\n", bufsize/1024, nloop/1024);

	int numnodes = numa_max_node() + 1;
	for (int tasknode = 0; tasknode < numnodes; ++tasknode) {
		for (int memnode = 0; memnode < numnodes; ++memnode) {
			bench(tasknode, memnode);
		}
	}

	return 0;
}
