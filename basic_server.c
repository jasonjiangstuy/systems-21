#include "pipe_networking.h"




int main() {

  int to_client;
  int from_client;

  while (1) {
      from_client = server_handshake( &to_client );
      printf("\n%s\n\n", "HandShake Established");
      char *array = calloc(50, sizeof(char));
      while (read(from_client,array, 50)) {
        // rot13
        int i = 0;
        int boundleft;
        int boundright;
        for (; i < strlen(array); i++){
          // used to exceed the char size limit of 127
          int target = array[i];
          int charater = 0;
          if (65 <= target && target <= 90){
            // cap charaters
            boundleft = 65;
            boundright = 90;
            charater = 1;

          }else if(97 <= target && target <= 122){
            // lower charaters
            boundleft = 97;
            boundright = 122;
            charater = 1;
          }
          target = target + 13;
          if (charater){
            if (boundright - target < 0){
              // pass the alphabet
              // wrap around
              target = boundleft + target - boundright - 1;
            }
          }

          array[i] = target;
        }
        write(to_client, array, 50);
      }
      printf("\n%s\n\n", "Client Exited");
      // read returns 0
      free(array);

  }
}
