int read_lock(int fd, off_t offset, int whence, off_t len);
int readw_lock(int fd, off_t offset, int whence, off_t len);
int write_lock(int fd, off_t offset, int whence, off_t len);
int readw_lock(int fd, off_t offset, int whence, off_t len);
int un_lock(int fd, off_t offset, int whence, off_t len);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len);
void set_fl(int fd, int flag);
void clr_fl(int fd, int flag);
