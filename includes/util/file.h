#ifndef _FILE_H_
#define _FILE_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*
*   GENERAL FILE FUNCTIONS.
*/

// open file. return NULL if error.
FILE *open_file2(const char *mode, const char *file, ...);

//
FILE *open_temp_file(const char *mode);

// return the ext of a file.
const char *get_file_name_ext(const char *filename);

//
bool check_file_ext(const char *file_name, const char *ext);

// check if file exists.
bool check_if_file_exists(const char *file);

// get the size of a file.
size_t get_file_size(const char *file);

//
bool find_file_of_size_at_least(const char *path, char *out, size_t size);

// create an empty file.
// returns 1 on success or if the file already existed.
bool create_file(const char *file);

// creates file filled with 1's of size size.
bool create_filled_file(const char *file, size_t size, bool truncate);

// wrapper around create_filled_file that creates a temp file with truncate set to true.
bool create_temp_filled_file(size_t size);

// remove file.
// returns 1 on success or if the file didn't exist.
bool delete_file(const char *file);

// wrapper around delete_file that deletes the temp file created with create_temp_filled_file.
bool delete_temp_file(void);

// copy file.
void copy_file(const char *src, char *dest);

// move file (rename).
void move_file(const char *src, char *dest);


/*
*   DATA READ / DATA WRITE.
*/

// read from file.
// out: data to be written to.
// size: size of data tp be read.
// offset: starting point of the data.
// f: the file to read the data from.
void read_file(void *out, size_t size, uint64_t offset, FILE *f);

void write_file(const void *in, size_t size, uint64_t offset, FILE *f);

#endif