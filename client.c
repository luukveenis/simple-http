/*------------------------------
* client.c
* Description: HTTP client program
* CSC 361
* Instructor: Kui Wu
-------------------------------*/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

/* define maximal string and reply length, this is just an example.*/
/* MAX_RES_LEN should be defined larger (e.g. 4096) in real testing. */
#define MAX_STR_LEN 120
#define MAX_RES_LEN 120

/* --------- Main() routine ------------
 * three main task will be excuted:
 * accept the input URI and parse it into fragments for further operation
 * open socket connection with specified sockid ID
 * use the socket id to connect sopecified server
 * don't forget to handle errors
 */

/* -------- Function Definitions --------- */
int parse_URI(char*, char*, int*, char*);
int process_hostname(char*);
int perform_http(int, char*, char*);
int open_connection(char *hostname, int port);

int main(int argc, char **argv)
{
  char hostname[MAX_STR_LEN];
  char identifier[MAX_STR_LEN];
  int sockid, port = 0;

  if (!argv[1])
  {
    printf("Invalid program invocation\n");
    printf("Format: SimpClient <request-url>\n");
    exit(EXIT_FAILURE);
  }

  /* Terminate on parsing errors */
  if (parse_URI(argv[1], hostname, &port, identifier) == -1)
  {
    printf("Aborting...\n");
    exit(EXIT_FAILURE);
  }
  sockid = open_connection(hostname, port);
  perform_http(sockid, hostname, identifier);
  exit(EXIT_SUCCESS);
}

/*------------------------------------*
* connect to a HTTP server using hostname and port,
* and get the resource specified by identifier
*--------------------------------------*/
int perform_http(int sockid, char *hostname, char *identifier)
{
  /* connect to server and retrieve response */
  int resp_len = 0, result_len = 0, buffsize = MAX_STR_LEN;
  char request[MAX_STR_LEN];
  char resp[MAX_STR_LEN];
  char *result = (char *)malloc(buffsize);
  char *begin, *end;

  sprintf(request, "GET http://%s/%s HTTP/1.0\r\n\r\n", hostname, identifier);
  printf("\n---Request Begin---\n");
  printf("%s", request);

  /* send the request */
  if (write(sockid, request, strlen(request)) < 0)
  {
    perror("couldn't send request");
    exit(EXIT_FAILURE);
  }

  printf("---Request end---\n");
  printf("HTTP request sent, awaiting response...\n\n");

  /* Receive the stuff */
  while ((resp_len = recv(sockid, resp, MAX_STR_LEN, 0)) > 0)
  {
    /* If the result buffer is full, double the size */
    if (result_len + resp_len >= buffsize)
    {
      buffsize *= 2;
      result = (char *)realloc(result, buffsize);
    }
    memcpy(result + result_len, resp, resp_len);
    result_len += resp_len;
  }
  if (resp_len == -1)
  {
    perror("recv failed");
    exit(EXIT_FAILURE);
  }

  /* Extract the header and print it */
  printf("---Response header---\n");
  begin = result;
  end = strstr(result, "\r\n\r\n");
  printf("%.*s\n\n", (int)(end - begin), begin);

  /* Extract the body and print it */
  printf("---Response body---\n");
  begin = end + 4; // Move cursor after the first \r\n\r\n
  end = strstr(end + 1, "\r\n\r\n");
  printf("%.*s\n", (int)(end - begin), begin);

  free(result);
  close(sockid);
  return 0;
}

/*---------------------------------------------------------------------------*
 *
 * open_conn() routine. It connects to a remote server on a specified port.
 *
 *---------------------------------------------------------------------------*/

int open_connection(char *hostname, int port)
{

  struct sockaddr_in server_addr;
  struct hostent *server_ent;

  server_ent = gethostbyname(hostname);
  if (server_ent == NULL)
  {
    perror("gethostbyname");
    printf("Hostname: %s\n", hostname);
    exit(EXIT_FAILURE);
  }

  memcpy(&server_addr.sin_addr, server_ent->h_addr, server_ent->h_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == sockfd)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  if (-1 == connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
  {
    perror("connect failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  return sockfd;
}

/* Parse a "uri" into "hostname" and resource "identifier"
 * Returns:
 * -  0 on success
 * - -1 on failure
 */
int parse_URI(char *uri, char *hostname, int *port, char *identifier)
{
  char *start = 0;
  char *cursor = 0;
  int port_provided = 0;

  /* Report error if the protocol is not specified */
  if (strncmp(uri, "http://", 7))
  {
    start = cursor = uri;
  }
  else
  {
    start = cursor = uri + 7;
  }

  /* Begin reading after the 'http://' */
  for (; *cursor; cursor++)
  {
    if (*cursor == ':')
    {
      port_provided = 1;
      strncpy(hostname, start, (cursor - start));
      hostname[(cursor - start)] = '\0';
      break;
    }
    if (*cursor == '/')
    {
      strncpy(hostname, start, (cursor - start));
      hostname[(cursor - start)] = '\0';
      break;
    }
  }

  /* If we reach the end of the string, we're missing the identifier */
  if (*cursor == '\0'){
    printf("Invalid: No resource identifier provided\n");
    return -1;
  }

  /* Read the port if one was provided */
  if (port_provided)
  {
    for (start = ++cursor; *cursor; cursor++)
    {
      if (*cursor == '/')
      {
        char *strport = strndup(start, (cursor - start));
        *port = atoi(strport);
        break;
      }
    }
    /* If end of string, we're missing the identifier */
    if (*cursor == '\0')
    {
      printf("Invalid: No resource identifier provided\n");
      return -1;
    }
  }
  /* The rest is the identifier */
  strcpy(identifier, ++cursor);

  /* Default port is 80 if none provided */
  if (*port == 0)
    *port = 80;

  return 0;
}
