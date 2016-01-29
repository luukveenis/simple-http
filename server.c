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

#include "util.h"

#define MAX_STR_LEN 120         /* maximum string length */
#define MAX_REQUEST_LEN 2048
#define SERVER_PORT_ID 9898     /* server port number */

/*--------------------- Function prototypes --------------------------*/
void cleanExit();
void set_dir(char *);
int init_server(int, struct sockaddr_in*, int);
int perform_http(int);
int process_request(int, char*);
void safe_write(int, char*, int);

/*---------------------main() routine--------------------------*
 * tasks for main
 * generate socket and get socket id,
 * max number of connection is 3
 * (maximum length the queue of pending connections may grow to)
 * Accept request from client and generate new socket
 * Communicate with client and close new socket after done
 *---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
  char *dir;
  unsigned int clilen;
  int sockfd, newsockfd, port; /* return value of the accept() call */
  struct sockaddr_in server, client;

  /* Exit nicely on ^C so sockets are closed */
  signal(SIGINT, cleanExit);

  /* User must provide port number and directory when starting the server
   * If a port is not specified, port 80 is used.
   * If neither are provide, the program displays an error and terminates */
  if (argc < 2 || argc > 3)
  {
    printf("ERROR: invalid program invocation.\n");
    printf("USAGE: SimpServer <port-number> <directory-name>\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    if (argc == 3)
    {
      set_dir(argv[2]);
      port = atoi(argv[1]);
    }
    else
    {
      set_dir(argv[1]);
      port = 80;
    }
  }

  /* Create and bind the socket, then listen on it */
  sockfd = init_server(port, &server, sizeof(server));
  listen(sockfd, 5);
  clilen = sizeof(client);

  /* Loop and wait for requests to come in from clients */
  for (;;)
  {
    /* Accept connection from client */
    newsockfd = accept(sockfd, (struct sockaddr *)&client, &clilen);
    if (newsockfd < 0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    /* Handle the client's request */
    perform_http(newsockfd);

    close(newsockfd);
  }

  return 0;
}

/*---------------------------------------------------------------------------*
 * Changes the current working directory to that specified by path
 * If the operation fails, an error is printed and the program is
 * terminated.
 *---------------------------------------------------------------------------*/
void set_dir(char *path)
{
  if (chdir(path) < 0)
  {
    perror("chdir");
    exit(EXIT_FAILURE);
  }
}

/*---------------------------------------------------------------------------*
 * Creates a new socket for the server and binds it.
 * Returns the socket file descriptor for use by the program.
 *---------------------------------------------------------------------------*/
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
 * cleans up opened sockets when killed by a signal.
 *---------------------------------------------------------------------------*/
void cleanExit()
{
  printf("\nInterrupt detected, exiting...\n");
  exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*
 * Accepts a request from "sockid" and sends a response to "sockid".
 *---------------------------------------------------------------------------*/

int perform_http(int sockid)
{
  int req_len, resp_len;
  char buf[MAX_REQUEST_LEN];
  char *part, *url;

  /* Read the client's message */
  memset(buf, 0, MAX_REQUEST_LEN);
  req_len = read(sockid, buf, MAX_REQUEST_LEN);
  if (req_len < 0)
  {
    /* Failed to read request */
    perror("read");
    exit(EXIT_FAILURE);
  }

  /* Verify the request type is GET */
  part = strtok(buf, " ");
  if (!strcmp("GET", part))
  {
    /* Store the URI in part and process the request */
    url = strtok(NULL, " ");
    part = strtok(NULL, " ");
    trim(part); // Removes the trailing \r\n\r\n

    /* Add 500 error for unsupported HTTP version */
    if (!part || strcmp(part, "HTTP/1.0"))
    {
      safe_write(sockid, "HTTP/1.0 500 Internal Server Error\r\n\r\n", 38);
    }
    else
    {
      process_request(sockid, url);
    }
  }
  else
  {
    /* If request type is not GET, return status code 501 */
    safe_write(sockid, "HTTP/1.0 501 Not Implemented\r\n\r\n", 32);
  }

  return 0;
}

/*---------------------------------------------------------------------------*
 * Parses the request from the client for the requested resource, tries to
 * open the resource, and sends back the content if successful. Otherwise
 * returns the appropriate HTTP status code.
 *---------------------------------------------------------------------------*/
int process_request(int sockid, char *request)
{
  FILE *fp;
  char buf[MAX_STR_LEN];
  char *cursor = request + 7;       // begin reading after http://
  cursor = strchr(cursor, '/') + 1; // the resource follows the '/'

  /* Try to open the requested resource in the current directory */
  if ((fp = fopen(cursor, "r")))
  {
    /* If the file opened succesfully, report 200 and send contents of file */
    safe_write(sockid, "HTTP/1.0 200 Okay\r\n\r\n", 21);
    while (fgets(buf, MAX_STR_LEN, fp))
    {
      safe_write(sockid, buf, strlen(buf));
    }
  }
  else
  {
    /* If the file failed to open, return status code 404 */
    safe_write(sockid, "HTTP/1.0 404 Not Found\r\n\r\n", 26);
  }

  return 0;
}

/*---------------------------------------------------------------------------*
 * This function just wraps write with error checking. It is used so that we
 * don't have to use `if (write(...) < 0) { ... }` everywhere in the code.
 *---------------------------------------------------------------------------*/
void safe_write(int sockid, char *buf, int size)
{
  if (write(sockid, buf, size) < 0)
  {
    perror("write");
    exit(EXIT_FAILURE);
  }
}
