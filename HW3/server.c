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
#include "util.h"
#include "server.h"
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>

void init_request( http_request* reqP ) {
   reqP->conn_fd = -1;
   reqP->status = 0;        // not used
   reqP->file[0] = (char) 0;
   reqP->query[0] = (char) 0;
   reqP->host[0] = (char) 0;
   reqP->buf = NULL;
   reqP->buf_size = 0;
   reqP->buf_len = 0;
   reqP->buf_idx = 0;
   reqP->fd_p2c[0] = 0;
   reqP->fd_p2c[1] = 0;
   reqP->fd_c2p[0] = 0;
   reqP->fd_c2p[1] = 0;
   reqP->header_written = 0;
}

void free_request( http_request* reqP ) {
   if ( reqP->buf != NULL ) {
      free( reqP->buf );
      reqP->buf = NULL;
   }
   close(reqP->fd_p2c[1]);
   close(reqP->fd_c2p[0]);
   init_request( reqP );
}


#define ERR_RET( error ) { *errP = error; return -1; }
// return 0: success, file is buffered in retP->buf with retP->buf_len bytes
// return -1: error, check error code (*errP)
// return 1: read more, continue until return -1 or 0
// error code: 
// 1: client connection error 
// 2: bad request, cannot parse request
// 3: method not implemented 
// 4: illegal filename
// 5: illegal query
// 6: file not found
// 7: file is protected
//
int read_header_and_file( http_request* reqP, int *errP ) {
   // Request variables
   char* file = (char *) 0;
   char* path = (char *) 0;
   char* query = (char *) 0;
   char* protocol = (char *) 0;
   char* method_str = (char *) 0;
   int r, fd;
   // struct stat sb;
   // char timebuf[100];
   // int buflen;
   char buf[10000];
   // time_t now;
   // void *ptr;

   // Read in request from client
   while (1) {
      r = read( reqP->conn_fd, buf, sizeof(buf) );
      if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) ) return 1;
      if ( r <= 0 ) ERR_RET( 1 )
      add_to_buf( reqP, buf, r );
      if ( strstr( reqP->buf, "\015\012\015\012" ) != (char*) 0 ||
          strstr( reqP->buf, "\012\012" ) != (char*) 0 ) break;
   }
   fprintf( stderr, "header: %s\n", reqP->buf );

   // Parse the first line of the request.
   method_str = get_request_line( reqP );
   if ( method_str == (char*) 0 ) ERR_RET( 2 )
   path = strpbrk( method_str, " \t\012\015" );
   if ( path == (char*) 0 ) ERR_RET( 2 )
   *path++ = '\0';
   path += strspn( path, " \t\012\015" );
   protocol = strpbrk( path, " \t\012\015" );
   if ( protocol == (char*) 0 ) ERR_RET( 2 )
   *protocol++ = '\0';
   protocol += strspn( protocol, " \t\012\015" );
   query = strchr( path, '?' );
   if ( query == (char*) 0 )
      query = "";
   else
      *query++ = '\0';

   if ( strcasecmp( method_str, "GET" ) != 0 ) ERR_RET( 3 )
   else {
      strdecode( path, path );
      if ( path[0] != '/' ) ERR_RET( 4 )
      else file = &(path[1]);
   }

   if ( strlen( file ) >= MAXBUFSIZE-1 ) ERR_RET( 4 )
   if ( strlen( query ) >= MAXBUFSIZE-1 ) ERR_RET( 5 )
     
   strcpy( reqP->file, file );
   strcpy( reqP->query, query );
   return 0;
}


void add_to_buf( http_request *reqP, char* str, size_t len ) { 
   char** bufP = &(reqP->buf);
   size_t* bufsizeP = &(reqP->buf_size);
   size_t* buflenP = &(reqP->buf_len);

   if ( *bufsizeP == 0 ) {
      *bufsizeP = len + 500;
      *buflenP = 0;
      *bufP = (char*) e_malloc( *bufsizeP );
   } else if ( *buflenP + len >= *bufsizeP ) {
      *bufsizeP = *buflenP + len + 500;
      *bufP = (char*) e_realloc( (void*) *bufP, *bufsizeP );
   }
   (void) memmove( &((*bufP)[*buflenP]), str, len );
   *buflenP += len;
   (*bufP)[*buflenP] = '\0';
}

char* get_request_line( http_request *reqP ) { 
   int begin;
   char c;

   char *bufP = reqP->buf;
   int buf_len = reqP->buf_len;

   for ( begin = reqP->buf_idx ; reqP->buf_idx < buf_len; ++reqP->buf_idx ) {
      c = bufP[ reqP->buf_idx ];
      if ( c == '\012' || c == '\015' ) {
         bufP[reqP->buf_idx] = '\0';
         ++reqP->buf_idx;
         if ( c == '\015' && reqP->buf_idx < buf_len && 
            bufP[reqP->buf_idx] == '\012' ) {
            bufP[reqP->buf_idx] = '\0';
            ++reqP->buf_idx;
         }
         return &(bufP[begin]);
      }
   }
   fprintf( stderr, "http request format error\n" );
   exit( 1 );
}



void init_http_server( http_server *svrP, unsigned short port ) {
   struct sockaddr_in servaddr;
   int tmp;

   gethostname( svrP->hostname, sizeof( svrP->hostname) );
   svrP->port = port;
   
   svrP->listen_fd = socket( AF_INET, SOCK_STREAM, 0 );
   if ( svrP->listen_fd < 0 ) ERR_EXIT( "socket" )

   bzero( &servaddr, sizeof(servaddr) );
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
   servaddr.sin_port = htons( port );
   tmp = 1;
   if ( setsockopt( svrP->listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &tmp, sizeof(tmp) ) < 0 ) 
   ERR_EXIT ( "setsockopt " )
   if ( bind( svrP->listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) ERR_EXIT( "bind" )

   if ( listen( svrP->listen_fd, 1024 ) < 0 ) ERR_EXIT( "listen" )
}

void set_ndelay( int fd ) {
   int flags, newflags;

   flags = fcntl( fd, F_GETFL, 0 );
   if ( flags != -1 ) {
      newflags = flags | (int) O_NDELAY; // nonblocking mode
      if ( newflags != flags )
         (void) fcntl( fd, F_SETFL, newflags );
   }
}   

void strdecode( char* to, char* from ) {
   for ( ; *from != '\0'; ++to, ++from ) {
      if ( from[0] == '%' && isxdigit( from[1] ) && isxdigit( from[2] ) ) {
         *to = hexit( from[1] ) * 16 + hexit( from[2] );
         from += 2;
      } else
         *to = *from;
   }
   *to = '\0';
}

void write_header(http_request* reqP, int status){
   int buflen;
   time_t now;
   char timebuf[100];
   char buf[1024];
   size_t contLen;
   switch(status) {
    case 200:
      buflen = snprintf( buf, sizeof(buf), "HTTP/1.1 200 OK\015\012Server: Mike Tsai\015\012" );
      if (strcmp(reqP->file, "favicon.ico") == 0)
         contLen = 77;
      else if (strcmp(reqP->file, "info") == 0)
         contLen = strlen(reqP->cntbuf);
      else {
         struct stat sb;
         char* c = strstr(reqP->query,"filename=");
         stat(c+9, &sb);
         contLen = sb.st_size;
      }
      break;
    case 400:
      buflen = snprintf( buf, sizeof(buf), "HTTP/1.1 400 Bad Request\015\012Server: Mike Tsai\015\012" );
      contLen = strlen(reqP->cntbuf);
      break;
    case 404:
      buflen = snprintf( buf, sizeof(buf), "HTTP/1.1 404 Not Found\015\012Server: Mike Tsai\015\012" );
      contLen = strlen(reqP->cntbuf);
      break;
   }
   strcpy(reqP->buf,"\0");
   reqP->buf_len = 0;
   add_to_buf( reqP, buf, buflen );
   now = time( (time_t*) 0 );
   (void) strftime( timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", gmtime( &now ) );
   buflen = snprintf( buf, sizeof(buf), "Date: %s\015\012", timebuf );
   add_to_buf( reqP, buf, buflen );
   buflen = snprintf(
      buf, sizeof(buf), "Content-Length: %ld\015\012", contLen );
   add_to_buf( reqP, buf, buflen );
   buflen = snprintf( buf, sizeof(buf), "Connection: close\015\012\015\012" );
   add_to_buf( reqP, buf, buflen );
   if (status == 400 || status == 404)
      add_to_buf(reqP, reqP->cntbuf, contLen);

}

int hexit( char c ) {
   if ( c >= '0' && c <= '9' )
   return c - '0';
   if ( c >= 'a' && c <= 'f' )
   return c - 'a' + 10;
   if ( c >= 'A' && c <= 'F' )
   return c - 'A' + 10;
   return 0;         // shouldn't happen
}


void* e_malloc( size_t size ) {
   void* ptr;

   ptr = malloc( size );
   if ( ptr == (void*) 0 ) {
      (void) fprintf( stderr, "out of memory\n" );
      exit(1);
   }
   return ptr;
}


void* e_realloc( void* optr, size_t size ) {
   void* ptr;

   ptr = realloc( optr, size );
   if ( ptr == (void*) 0 ) {
      (void) fprintf( stderr, "out of memory\n" );
      exit(1);
   }
   return ptr;
}
