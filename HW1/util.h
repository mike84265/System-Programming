#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef UTIL_H
#define UTIL_H
int read_lock(int fd, off_t offset, int whence, off_t len);
int readw_lock(int fd, off_t offset, int whence, off_t len);
int write_lock(int fd, off_t offset, int whence, off_t len);
int writew_lock(int fd, off_t offset, int whence, off_t len);
int un_lock(int fd, off_t offset, int whence, off_t len);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len);
void set_fl(int fd, int flag);
void clr_fl(int fd, int flag);
void tv_reset(struct timeval* tv, int sec, int usec);


typedef struct {
    char** arr;
    int size;
    int length;
    int capacity;
} str_arr;


void init(str_arr* strArr, int capacity, int length);
void push_back(const char* element, str_arr* strArr);
int clear(const char* element, str_arr* strArr);
int query(const char* str, const str_arr* strArr);
#endif
