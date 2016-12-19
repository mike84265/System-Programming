/* mmap_write.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    char c_time_string[100];
    int numRunning;
    int numDied;
    pid_t runningPID[128];
    pid_t diedPID[128];
} TimeInfo;

int main(int argc, char** argv) 
{
    int fd,i;
    time_t current_time;
    char c_time_string[100];
    TimeInfo *p_map;
    
    const char  *file ="time_test";

    fd = open(file, O_RDWR);
    p_map = (TimeInfo*)mmap(0, sizeof(TimeInfo),  PROT_READ,  MAP_SHARED, fd, 0);
    
    while(1) {
        sleep(2);
        printf("%s\n", p_map->c_time_string);
        printf("Running process: ");
        for (int i=0;i<p_map->numRunning;++i)
            printf("%d, ", p_map->runningPID[i]);
        printf("\nDied process: ");
        for (int i=0;i<p_map->numDied;++i)
            printf("%d, ", p_map->diedPID[i]);
        printf("\n---\n");
    }

    return 0;
}
