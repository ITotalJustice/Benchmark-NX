#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


typedef enum
{
    BenchmarkMode_NAND_Read,
    BenchmarkMode_SD_Read,
    BenchmarkMode_GC_Read,

    BenchmarkMode_NAND_Write,
    BenchmarkMode_SD_Write,
} BenchmarkMode;

//
bool benchmark(BenchmarkMode mode, size_t chunk_size, size_t full_size);

#endif