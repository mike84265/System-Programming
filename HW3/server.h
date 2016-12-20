/*  B03901078  蔡承佑  */
#ifndef SERVER_H
#define SERVER_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "util.h"

#define TIMEOUT_SEC 5        // timeout in seconds for wait for a connection 
#define MAXBUFSIZE  1024     // timeout in seconds for wait for a connection 
#define NO_USE      0        // status of a http request
#define ERROR       -1    
#define READING     1        
#define WRITING     2        
#define ERR_EXIT(a) { perror(a); exit(1); }

typedef struct {
   char hostname[512];        // hostname
   unsigned short port;       // port to listen
   int listen_fd;             // fd to wait for a new connection
} http_server;

typedef struct {
   int conn_fd;            // fd to talk with client
   int status;             // not used, error, reading (from client)
                           // writing (to client)
   char file[MAXBUFSIZE];  // requested file
   char query[MAXBUFSIZE]; // requested query
   char host[MAXBUFSIZE];  // client host
   char cntbuf[1024];       // Error message to show to the client.
   char* buf;              // data sent by/to client
   size_t buf_len;         // bytes used by buf
   size_t buf_size;        // bytes allocated for buf
   size_t buf_idx;         // offset for reading and writing
   int fd_p2c[2];
   int fd_c2p[2];
   int header_written;
} http_request;


// Forwards
//
void init_http_server( http_server *svrP,  unsigned short port );
// initailize a http_request instance, exit for error

void init_request( http_request* reqP );
// initailize a http_request instance

void free_request( http_request* reqP );
// free resources used by a http_request instance

void write_header(http_request* reqP, int status);

int read_header_and_file( http_request* reqP, int *errP );
// return 0: success, file is buffered in retP->buf with retP->buf_len bytes
// return -1: error, check error code (*errP)
// return 1: continue to it until return -1 or 0
// error code: 
// 1: client connection error 
// 2: bad request, cannot parse request
// 3: method not implemented 
// 4: illegal filename
// 5: illegal query
// 6: file not found
// 7: file is protected

void set_ndelay( int fd );
// Set NDELAY mode on a socket.


void add_to_buf( http_request *reqP, char* str, size_t len );
void strdecode( char* to, char* from );
int hexit( char c );
char* get_request_line( http_request *reqP );
void* e_malloc( size_t size );
void* e_realloc( void* optr, size_t size );
#endif
