#include <fcntl.h>
#include <stdio.h>
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
