/*------------------------------
* client.c
* Description: HTTP client program
* CSC 361
* Instructor: Kui Wu
-------------------------------*/

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

int main(int argc, char **argv)
{
  char uri[MAX_STR_LEN];
  char hostname[MAX_STR_LEN];
  char identifier[MAX_STR_LEN];
  int sockid, port = 0;

  printf("Open URI:  ");
  fgets(uri, sizeof(uri), stdin);
  trim(uri);

  /* Terminate on parsing errors */
  if (parse_URI(uri, hostname, &port, identifier) == -1)
  {
    printf("Aborting...\n");
    exit(EXIT_FAILURE);
  }
  /* sockid = open_connection(hostname, port);
  perform_http(sockid, identifier);*/
  exit(EXIT_SUCCESS);
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
    printf("Invalid: No protocol specified\n");
    return -1;
  }

  /* Begin reading after the 'http://' */
  for (cursor = start = (uri + 7); *cursor; cursor++)
  {
    if (*cursor == ':')
    {
      port_provided = 1;
      strncpy(hostname, start, (cursor - start));
      break;
    }
    if (*cursor == '/')
    {
      strncpy(hostname, start, (cursor - start));
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

/*------------------------------------*
* connect to a HTTP server using hostname and port,
* and get the resource specified by identifier
*--------------------------------------*/
int perform_http(int sockid, char *identifier)
{
  /* connect to server and retrieve response */

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

  int sockfd;
  /* generate socket
   * connect socket to the host address
   */
  return sockfd;
}
