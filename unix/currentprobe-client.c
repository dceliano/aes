//
// A small RPC client that askes for the running energy total(s) from a PRAZOR power probe.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "currentprobe-client.h"

static struct sockaddr_in sin;
static struct sockaddr_in pin;
static int sd = -1;

int currentprobe_open(const char *hostname)
{
  int rc = 0;

  /* fill in the socket structure with host information */
  struct hostent *hp = gethostbyname(hostname);
  if (!hp)
  {
  	perror("Unable to find that host. Code:");
  	exit(1);
  }
  memset(&pin, 0, sizeof(pin));
  pin.sin_family = AF_INET;
  pin.sin_addr.s_addr = ((struct in_addr *)((hp->h_addr_list)[0]))->s_addr;
  pin.sin_port = htons(PORT);

  /* grab an Internet domain socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("currentprobe socket");
    return -1;
  }

  /* connect to PORT on HOST */
  if (connect(sd,(struct sockaddr *) &pin, sizeof(pin)) == -1)
    {
      printf("Failed to connect to %s\n", hostname);
      perror("currentprobe connect");
      return -1;
    }
  else{
	printf("Successfully connected to %s\n", hostname);
  }
  return rc;
}


int currentprobe_operate(int *energies, const char *hostname)
{
  if (sd < 0 && hostname)
    {
      currentprobe_open(hostname);
    }

  int rc = -1;
  const char *cmd = "cmd\n";
	//printf("Reached here.\n");
  /* send a message to the server PORT on machine HOST */
  if (sd < 0 || send(sd, cmd, strlen(cmd), 0) == -1) {
    perror("send");
    return -1;
  }
  else{
	printf("sent a cmd to the server.\n");
  }

  char dir[MESSAGESIZE];

printf("about to receive.\n");
  /* wait for a message to come back from the server */
  if (recv(sd, dir, MESSAGESIZE*sizeof(char), 0) == -1) {
    perror("recv");
    return -1;
  }
else{
	printf("Received a msg from the server: %s\n", dir);
}

printf("about to parse.\n");
  // parse result to millijoules - total and core energies in use.
  int c = sscanf(dir, "ENERGY=%i!%i|", &energies[0], &energies[1]);
  printf("'%s'  c=%i  e=%i mJ (core %i mJ)\n", dir, c, energies[0], energies[1]);
  if (c == 2) rc = 0;
  return rc;
}


// eof
 
