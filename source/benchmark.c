#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <threads.h>
#include <switch.h>

#include "benchmark.h"
#include "menu.h"

#include "nx/fs.h"
#include "nx/gc.h"

#include "util/file.h"
#include "util/log.h"


typedef struct
{
    FILE *fp;
    void *data;
    size_t chunk_size;
    size_t data_processed;
    size_t total_size;
    char path[0x400];
} thread_t;


int read_benchmark(void *in)
{
    thread_t *t = (thread_t *)in;
    if (!t)
        return -1;

    void *buf = malloc(t->chunk_size);
    if (!buf)
        return -1;
    
    while (t->data_processed < t->total_size)
    {
        if (t->data_processed + t->chunk_size > t->total_size)
            t->chunk_size = t->total_size - t->data_processed;

        read_file(buf, t->chunk_size, 0, t->fp);
        t->data_processed += t->chunk_size;
    }

    free(buf);
    return 0;
}

int write_benchmark(void *in)
{
    thread_t *t = (thread_t *)in;
    if (!t)
        return -1;

    // the buffer we will write from.
    void *buf = malloc(t->chunk_size);
    if (!buf)
        return -1;
    
    // fill the buffer with 1's.
    memset(buf, 1, t->chunk_size);

    while (t->data_processed < t->total_size)
    {
        rewind(t->fp);

        if (t->data_processed + t->chunk_size > t->total_size)
            t->chunk_size = t->total_size - t->data_processed;

        fwrite(buf, 1, t->chunk_size, t->fp);
        t->data_processed += t->chunk_size;
    }

    return 0;
}

bool setup_benchmark(thread_t *t, BenchmarkMode mode)
{
    switch (mode)
    {
        case BenchmarkMode_GC_Read:
        {
            if (!find_file_of_size_at_least("@GcApp:/", t->path, t->chunk_size))
            {
                return false;
            }
            t->fp = fopen(t->path, "rb");
            return t->fp != NULL;
        }
        case BenchmarkMode_NAND_Read:
        {
            create_filled_file("@User:/tempfile", t->chunk_size, true);
            t->fp = fopen(strcpy(t->path, "@User:/tempfile"), "rb");
            return t->fp != NULL;
        }
        case BenchmarkMode_SD_Read:
        {
            create_filled_file("sdmc:/tempfile", t->chunk_size, true);
            t->fp = fopen(strcpy(t->path, "sdmc:/tempfile"), "rb");
            return t->fp != NULL;
        }

        case BenchmarkMode_NAND_Write:
        {
            t->fp = fopen(strcpy(t->path, "@User:/tempfile"), "wb");
            return t->fp != NULL;
        }
        case BenchmarkMode_SD_Write:
        {
            t->fp = fopen(strcpy(t->path, "sdmc:/tempfile"), "wb");
            return t->fp != NULL;
        }

        default:
        {
            return false;
        }
    }
}

bool benchmark(BenchmarkMode mode, size_t chunk_size, size_t full_size)
{
    print_display("Starting benchmark...\n\n");

    // create struct
    thread_t t = {0};

    // setup the sizes.
    t.chunk_size = chunk_size;
    t.total_size = full_size;

    if (!setup_benchmark(&t, mode))
    {
        return false;
    }

    thrd_t thrd;
    thrd_create(&thrd, mode < BenchmarkMode_NAND_Write ? read_benchmark : write_benchmark, &t);
    
    // save the starting time.
    time_t start_time = time(NULL);

    while (t.data_processed < t.total_size)
    {
        uint16_t bar_length = 40;
        double fraction_downloaded = (double)t.data_processed / (double)t.total_size;

        uint16_t hashes = round(fraction_downloaded * bar_length);

        uint16_t i = 0;
        printf("%3.0f%% [",fraction_downloaded * 100);

        for (; i < hashes; i++)
        {
            printf("#");
        }

        for (; i < bar_length; i++)
        {
            printf(" ");
        }
        
        printf("]\r");
        consoleUpdate(NULL);
    }
    printf("\n\n\n");

    // wait for the thread to exit.
    thrd_join(thrd, NULL);

    // save the ending time.
    time_t end_time = time(NULL);

    fclose(t.fp);
    if (mode != BenchmarkMode_GC_Read)
        delete_file(t.path);

    // get the diff time.
    double execution_time = difftime(end_time, start_time);

    // print the results.
    printf("min: %lu seconds: %.f\n\n", (uint64_t)execution_time / 60, fmod(execution_time, 60));
    printf("%.2f MB/s \n\n", (full_size / execution_time) / 0x100000);
    print_lock("Done.\n");

    return true; 
}