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

static int logfd;
char* validChar = "ABCDEFGHIJKLIMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.";
int numDied = 0;
int exit_status;
static void sig_child(int signo);
static void sig_usr(int signo);

int main( int argc, char** argv ) {
   http_server server;              // http server
   http_request* requestP = NULL;   // pointer to http requests from client

   int maxfd;                  // size of open file descriptor table

   struct sockaddr_in cliaddr; // used by accept()
   int clilen;

   int conn_fd;        // fd for a new connection with client
   int err;            // used by read_header_and_file()
   int i, ret, nwritten;
   
   fd_set rset;
   List pList;
   init(&pList);

   // Parse args. 
   if ( argc != 3 ) {
      (void) fprintf( stderr, "usage:  %s port# logfile\n", argv[0] );
      exit(1);
   }

   logfd = open(argv[2],O_CREAT | O_WRONLY | O_TRUNC, 0644);
   signal(SIGCHLD,sig_child);
   signal(SIGUSR1,sig_usr);
   int timefd = open("time_info", O_CREAT | O_RDWR | O_TRUNC, 0644);
   TimeInfo* info = (TimeInfo*)mmap(0, sizeof(TimeInfo), PROT_READ, MAP_SHARED, timefd, 0);

   // Initialize http server
   init_http_server( &server, (unsigned short) atoi( argv[1] ) );

   maxfd = getdtablesize();
   requestP = ( http_request* ) malloc( sizeof( http_request ) * maxfd );
   if ( requestP == (http_request*) 0 ) {
      fprintf( stderr, "out of memory allocating all http requests\n" );
      exit(1);
   }
   for (i=0;i<maxfd;i++)
      init_request( &requestP[i] );
   requestP[ server.listen_fd ].conn_fd = server.listen_fd;
   requestP[ server.listen_fd ].status = READING;

   fprintf( stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d, logfile %s...\n", 
      server.hostname, server.port, server.listen_fd, maxfd, argv[2] );


   // Main loop. 
   while (1) {
      FD_ZERO(&rset);
      FD_SET(server.listen_fd, &rset);
      for (i=6;i<1024;++i) {
         if (requestP[i].status != 0) {
            FD_SET(requestP[i].fd_c2p[0], &rset);
         }
      }
      FD_CLR(0,&rset);
      #ifdef DEBUG
      fprintf(stderr,"select;\n");
      #endif
      int n = select(1024,&rset,NULL,NULL,NULL);
      #ifdef DEBUG
      fprintf(stderr,"select = %d\n", n);
      #endif
      if (n <= 0) {
         if (errno == EINTR)
            continue;
         else
            exit(1);
      }
      if (FD_ISSET(server.listen_fd, &rset) == 0) {
         for (i=6;i<1024;++i) {
            if (requestP[i].status != 0 && FD_ISSET(requestP[i].fd_c2p[0],&rset)) {
               http_request* reqP = &requestP[i];
               int buflen;
               char buf[1024];
               buflen = read(reqP->fd_c2p[0],buf,sizeof(buf));
               if (buflen == 0) {
                  // EOF or empty file
                  int pid = wait(&exit_status);
                  exit_status = WEXITSTATUS(exit_status);
                  erase(&pList,pid);
                  fprintf(stderr,"status = %d\n",exit_status);
                  if (exit_status != 0) {
                     strcpy(reqP->cntbuf,"File not found!\n");
                     write_header(reqP, 404);
                     write(reqP->conn_fd, reqP->buf, reqP->buf_len);
                     write(logfd, reqP->buf, reqP->buf_len );
                  }
                  else {
                     #ifdef DEBUG
                     printf("exit_time = %s\n",info->time_string);
                     printf("numDied = %d\n", numDied);
                     #endif
                     if (reqP->header_written == 0) {
                        write_header(reqP, 200);
                        write(reqP->conn_fd, reqP->buf, reqP->buf_len);
                        write(logfd, reqP->buf, reqP->buf_len );
                     }
                  }
                  // Clear.
                  close(reqP->conn_fd);
                  free_request(reqP);
                  break;
               } else {
                  if (reqP->header_written == 0) {
                     write_header(reqP,200);
                     write(reqP->conn_fd, reqP->buf, reqP->buf_len);
                     write(logfd, reqP->buf, reqP->buf_len );
                     reqP->header_written = 1;
                  }
                  write(reqP->conn_fd, buf, buflen);
                  write(logfd, buf, buflen);
                  break;
               }
            }
         }
         continue;
      }

      // Wait for a connection.
      clilen = sizeof(cliaddr);
      conn_fd = accept( server.listen_fd, (struct sockaddr *) &cliaddr, (socklen_t *) &clilen );
      if ( conn_fd < 0 ) {
         if ( errno == EINTR || errno == EAGAIN ) continue; // try again 
         if ( errno == ENFILE ) {
            (void) fprintf( stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd );
            continue;
         }   
         ERR_EXIT( "accept" )
      }
      requestP[conn_fd].conn_fd = conn_fd;
      requestP[conn_fd].status = READING;      
      strcpy( requestP[conn_fd].host, inet_ntoa( cliaddr.sin_addr ) );
      set_ndelay( conn_fd );

      fprintf( stderr, "getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host );

      nwritten = 0;
      while(1) {
         ret = read_header_and_file( &requestP[conn_fd], &err );
         if (ret > 0) continue;
         else if ( ret < 0 ) {
            // error for reading http header or requested file
            fprintf( stderr, "error on fd %d, code %d\n", 
            requestP[conn_fd].conn_fd, err );
            requestP[conn_fd].status = ERROR;
            close( requestP[conn_fd].conn_fd );
            free_request( &requestP[conn_fd] );
            break;
         } else if ( ret == 0 ) {
            // ready for writing
            fprintf( stderr, "writing (buf %p, idx %d) %d bytes to request fd %d\n", 
               requestP[conn_fd].buf, (int) requestP[conn_fd].buf_idx,
               (int) requestP[conn_fd].buf_len, requestP[conn_fd].conn_fd );

            #ifdef DEBUG
            fprintf(stderr, "file = %s\nquery = %s\n", requestP[conn_fd].file, requestP[conn_fd].query);
            #endif

            http_request* reqP = &requestP[conn_fd];

            #define BADREQ(x)   {\
               char* str = (x); \
               strcpy(reqP->cntbuf,(x)); \
               write_header(reqP,400); \
               write(reqP->conn_fd, reqP->buf, reqP->buf_len ); \
               write(logfd, reqP->buf, reqP->buf_len ); \
               nwritten = reqP->buf_len; \
               nwritten += write(reqP->conn_fd, str, strlen(str)); \
               write(logfd, str, strlen(str)); \
               break; \
            }

            if (strcmp(reqP->file,"info") == 0) {
               if (fork() == 0) {
                  raise(SIGUSR1);
                  exit(0);
               } else {
                  pause();
                  wait(NULL);
                  char buf[1024];
                  sprintf(buf,"%d processes died previously.\n", numDied);
                  strcat(buf,"PIDs of Running Processes: ");
                  if (empty(&pList) == 0) {
                     struct Node* p = pList.head->next;
                     sprintf(buf,"%s%d",buf,p->val);
                     for (p=p->next; p!=pList.head; p=p->next)
                        sprintf(buf, "%s, %d",buf,p->val);
                  }
                  #ifdef DEBUG
                  fprintf(stderr, "size = %d, empty = %d\n",pList.size, empty(&pList));
                  #endif
                  sprintf(buf,"%s\nLast EXit CGI: %s\n",buf,info->time_string);
                  strcpy(reqP->cntbuf,buf);
                  write_header(reqP, 200);
                  write(reqP->conn_fd, reqP->buf, reqP->buf_len);
                  write(logfd, reqP->buf, reqP->buf_len);
                  write(reqP->conn_fd,buf,strlen(buf));
                  write(logfd,buf,strlen(buf));
                  close(reqP->conn_fd);
                  free_request(reqP);
                  break;
               }
            }

            if (strcmp(reqP->file,"favicon.ico") != 0) {
               if (strspn(reqP->file, validChar) != strlen(reqP->file)) 
                  BADREQ("Invalid file name!!\n")
               char* c = strstr(reqP->query,"filename=");
               if (c == NULL)
                  BADREQ("Invalid query parameter!!\n")
               else 
                  if (strspn(c+9,validChar) != strlen(c+9))
                  BADREQ("Query file contains invalid character!!\n")
            } 
            int pid;
            pipe(reqP->fd_p2c);
            pipe(reqP->fd_c2p);
            if ((pid = fork()) == 0) {
               close(reqP->fd_p2c[1]);
               close(reqP->fd_c2p[0]);
               if (dup2(reqP->fd_p2c[0],0) != 0)
                  fprintf(stderr,"dup2 STDIN error\n");
               if (dup2(reqP->fd_c2p[1],1) != 1)
                  fprintf(stderr,"dup2 STDOUT error\n");
               if (execlp(requestP[conn_fd].file,requestP[conn_fd].file,(char*)0) == -1) {
                  fprintf(stderr,"exec Error\n");
                  exit(3);
               }
            } else if (pid > 0){
               close(reqP->fd_p2c[0]);
               close(reqP->fd_c2p[1]);
               push(&pList,pid);
               write(reqP->fd_p2c[1],reqP->query,sizeof(reqP->query));
            }
            break;
         }
      }
   }
   free( requestP );
   return 0;
}

void sig_child(int signo) {
   ++numDied;
   #ifdef DEBUG
   fprintf(stderr,"SIGCHLD is caught!\n");
   #endif
}

void sig_usr(int signo) {
   #ifdef DEBUG
   fprintf(stderr,"SIGUSR1 is caught!\n");
   #endif
}
