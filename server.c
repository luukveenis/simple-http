/*------------------------------
* server.c
* Description: HTTP server program
* CSC 361
* Instructor: Kui Wu
-------------------------------*/

#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX_STR_LEN 120         /* maximum string length */
#define SERVER_PORT_ID 9898     /* server port number */

/*--------------------- Function prototypes --------------------------*/
void cleanExit();
int init_server(int, struct sockaddr_in*, int);

/*---------------------main() routine--------------------------*
 * tasks for main
 * generate socket and get socket id,
 * max number of connection is 3 (maximum length the queue of pending connections may grow to)
 * Accept request from client and generate new socket
 * Communicate with client and close new socket after done
 *---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
  char *dir, buf[MAX_STR_LEN];
  unsigned int clilen;
  int sockfd, newsockfd, port, resp_len; /* return value of the accept() call */
  struct sockaddr_in server, client;

  signal(SIGINT, cleanExit);

  /* User must provide port number and directory when starting the server */
  if (!argv[1] || !argv[2] || argv[3])
  {
    printf("ERROR: invalid program invocation.\n");
    printf("USAGE: SimpServer <port-number> <directory-name>\n");
    exit(EXIT_FAILURE);
  }
  port = atoi(argv[1]);
  dir = argv[2];

  sockfd = init_server(port, &server, sizeof(server));
  listen(sockfd, 5);
  clilen = sizeof(client);

  while (1)
  {
    /* Accept connection from client */
    newsockfd = accept(sockfd, (struct sockaddr *)&client, &clilen);
    if (newsockfd < 0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    /* Read the client's message */
    memset(buf, 0, MAX_STR_LEN);
    resp_len = read(newsockfd, buf, MAX_STR_LEN);
    if (resp_len < 0)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }
    printf("Message: %s\n", buf);

    /* Send a response */
    resp_len = write(newsockfd, "Done!", 5);
    if (resp_len < 0)
    {
      perror("write");
      exit(EXIT_FAILURE);
    }

    close(newsockfd);
  }

  return 0;
}

/* Creates a new socket for the server and binds it.
 * Returns the socket file descriptor for use by the program. */
int init_server(int port, struct sockaddr_in *server, int serverlen)
{
  int sockfd;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(server, 0, serverlen);
  server->sin_family = AF_INET;
  server->sin_addr.s_addr = INADDR_ANY;
  server->sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) server, serverlen) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  return sockfd;
}

/*---------------------------------------------------------------------------*
 *
 * cleans up opened sockets when killed by a signal.
 *
 *---------------------------------------------------------------------------*/
void cleanExit()
{
  printf("\nInterrupt detected, exiting...\n");
  exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*
 *
 * Accepts a request from "sockid" and sends a response to "sockid".
 *
 *---------------------------------------------------------------------------*/

int perform_http(int sockid)
{
  return 0;
}

