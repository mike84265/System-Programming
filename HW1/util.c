#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include "util.h"
static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return fcntl(fd,cmd,&lock);
}
int read_lock(int fd, off_t offset, int whence, off_t len)
{ return lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len); }

int readw_lock(int fd, off_t offset, int whence, off_t len)
{ return lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len); }

int write_lock(int fd, off_t offset, int whence, off_t len)
{ return lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len); }

int writew_lock(int fd, off_t offset, int whence, off_t len)
{ return lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len); }

int un_lock(int fd, off_t offset, int whence, off_t len)
{ return lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len); }


pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    if (fcntl(fd,F_GETLK,&lock) < 0)
        fprintf(stderr,"fcntl error");
    if (lock.l_type == F_UNLCK)
        return 0;
    else
        return lock.l_pid;
}

void set_fl(int fd, int flag)
{
    int val;
    if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
        fprintf(stderr, "fcntl F_GETFL error\n");
    if ( fcntl(fd, F_SETFL, val|flag) < 0)
        fprintf(stderr, "fcntl F_SETFL error\n");
}

void clr_fl(int fd, int flag)
{
    int val;
    if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
        fprintf(stderr, "fcntl F_GETFL error\n");
    if ( fcntl(fd, F_SETFL, val & ~flag) < 0)
        fprintf(stderr, "fcntl F_SETFL error\n");
}

void tv_reset(struct timeval* tv, int sec, int usec)
{
    tv->tv_sec = sec;
    tv->tv_usec = usec;
}

void init(str_arr* strArr, int capacity, int length)
{
    if (capacity <= 0)
        return;
    strArr->arr = (char**)malloc(sizeof(char*) * capacity);
    strArr->capacity = capacity;
    for (int i=0;i<capacity;++i) {
        strArr->arr[i] = (char*)malloc(sizeof(char) * length);
        strArr->arr[i][0] = '\0';
    }
    strArr->size = 0;
}

int query(const char* str, const str_arr* strArr)
{
    for (int i=0;i<strArr->capacity;++i) {
        if (strcmp(strArr->arr[i], str) == 0)
            return i;
    }
    return -1;
}

void push_back(const char* element, str_arr* strArr)
{
    for (int i=0;i<strArr->capacity;++i){
        if (strArr->arr[i][0] == '\0'){
            strcpy(strArr->arr[i], element);
            ++strArr->size;
            return;
        }
    }
}

int clear(const char* element, str_arr* strArr)
{
    for (int i=0;i<strArr->capacity;++i) {
        if (strcmp(element, strArr->arr[i]) == 0) {
            strArr->arr[i][0] = '\0';
            --strArr->size;
            return i;
        }
    }
    return -1;
}
