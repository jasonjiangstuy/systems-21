#include "pipe_networking.h"


static void sighandler(int signo) {
  if ( signo == SIGINT ){
    remove("parent");
    exit(0);
  }
}
/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  signal(SIGINT, sighandler);
  int from_client = 0;
  // Server creates a well know pip (wkp) and waitfor a connection.
  mkfifo("parent", 0644);
  from_client = open("parent", O_RDONLY);
  printf("%s\n", "Created parent input pipe");

  // Server gets client message, Remove the well known pipe
  char * clientName = calloc(50, sizeof(char));
  read(from_client,clientName, 50);
  printf("%s %s\n", "Client message recieved", clientName);
  remove("parent");
  printf("%s\n", "Removed named pipe");

  *to_client = open(clientName, O_WRONLY);
  write(*to_client, "0", sizeof(char));
  printf("%s\n", "final response to client");

  // Server waits for response
  char * hold = calloc(1, sizeof(char));
  read(from_client,hold, sizeof(char));
  printf("%s\n", "recieved final message from client");

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int pid = getpid();
  char * clientName = calloc(50, sizeof(char));
  sprintf(clientName, "%d", pid);
  mkfifo(clientName, 0644);
  //   Client creates a secret pipe with pid
  printf("%s\n", "Created secret pipe");

  printf("%s %s\n", "Opened parent input pipe", clientName);
  *to_server = open("parent", O_WRONLY);
  // Client sends the secret pipe name to the server
  printf("%s\n", "sent to parent");
  write(*to_server, clientName, 50);

  int from_server = open(clientName, O_RDONLY);


// Client waits for response
  char * hold = calloc(1, sizeof(char));
  read(from_server,hold, sizeof(char));
  printf("%s\n", "recieved final message from parent");

  // Client gets response, removes named pipe
  remove(clientName);
  printf("%s\n", "removed named pipe");

  // Client sends final response
  write(*to_server, "0", sizeof(char));
  printf("%s\n", "final write to parent");

  return from_server;
}
