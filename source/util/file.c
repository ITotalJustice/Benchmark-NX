#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

#include "util/file.h"
#include "util/dir.h"


FILE *open_file2(const char *mode, const char *file, ...)
{
    char full_path[0x400] = {0};
    va_list v;
    va_start(v, file);
    vsnprintf(full_path, 0x400, file, v);
    va_end(v);

    //printf("tried file %s\n", full_path);
    return fopen(full_path, mode);
}

FILE *open_temp_file(const char *mode)
{
    return fopen("sdmc:/temp", mode);
}

bool create_file(const char *file)
{
    if (check_if_file_exists(file))
        return true;
    FILE *f = open_file2(file, "wb");
    if (!f)
        return false;
    fclose (f);
    return true;
}

bool create_filled_file(const char *file, size_t size, bool truncate)
{
    if (!file || !size)
        return false;

    if (check_if_file_exists(file) && !truncate)
        return false;

    FILE *fp = open_file2("wb", file);
    if (!fp)
        return false;

    void *buf = malloc(size);
    if (!buf)
    {
        fclose(fp);
        return false;
    }

    memset(buf, 1, size);
    fwrite(buf, 1, size, fp);
    fclose(fp);
    return true;
}

bool create_temp_filled_file(size_t size)
{
    return create_filled_file("sdmc:/temp", size, true);
}

bool check_if_file_exists(const char *file)
{
    FILE *f = fopen(file, "r");
    if (!f)
        return false;
    fclose(f);
    return true;
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    return !dot || dot == filename ? "" : dot + 1;
}

bool check_file_ext(const char *file_name, const char *ext)
{
    return strcmp(get_filename_ext(file_name), ext) == 0 ? true : false; 
}

size_t get_file_size(const char *file)
{
    size_t size = 0;
    FILE *f = open_file2("r", file);
    if (!f)
        return size;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);
    return size;
}

size_t get_file_size_stat(const char *file)
{
    struct stat st;
    stat(file, &st);
    return st.st_size;
}

bool find_file_of_size_at_least(const char *path, char *out, size_t size)
{
    struct dirent *d = {0};
    DIR *dir = open_dir(path);
    if (!dir)
        return false;

    printf("opened dir\n");
    char full_path_buf[0x400] = {0};
    bool found = false;

    while ((d = readdir(dir)))
    {
        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") || d->d_type != DT_REG)
            continue;

        snprintf(full_path_buf, 0x400, "%s%s", path, d->d_name);
        if (get_file_size_stat(full_path_buf) >= size)
        {
            strcpy(out, full_path_buf);
            found = true;
            break;
        }
    }
    closedir(dir);
    return found;
}

bool delete_file(const char *file)
{
    return remove(file) == 0 ? true : false;
}

bool delete_temp_file(void)
{
    return remove("sdmc:/temp") == 0 ? true : false;
}

void copy_file(const char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        void *buf = malloc(0x800000);
        size_t bytes; // size of the file to write (8MiB or filesize max)

        while (0 < (bytes = fread(buf, 1, 0x800000, srcfile)))
            fwrite(buf, bytes, 1, newfile);
        free(buf);
    }
    fclose(srcfile);
    fclose(newfile);
}

void move_file(const char *src, char *dest)
{
    rename(src, dest);
}

void read_file(void *out, size_t size, uint64_t offset, FILE *f)
{
    fseek(f, offset, SEEK_SET);
    fread(out, 1, size, f);
}

void write_file(const void *in, size_t size, uint64_t offset, FILE *f)
{
    fseek(f, offset, SEEK_SET);
    fwrite(in, 1, size, f);
}