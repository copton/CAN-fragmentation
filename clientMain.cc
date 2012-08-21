#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "downloadClient.h"
#include "debug.h"

char* filename;

void callback(char* data, int size) 
{
    printf("transmission finished. received %d bytes\n", size);
    
    int fd;
    if ((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        perror("");
        exit(-1);	
    }

    write(fd, data, size);

    close(fd);
    
    exit(0);
}


/* usage: client filename max_size */
int main (int arc, char* argv[])
{
  ech_init_module();

  int size = atoi(argv[2]);
  filename = argv[1];
  char* buffer = (char*) malloc(size);

  DownloadClient client(0x20000000);

  DEBUGOUT("starting download...\n");
  client.start(0, buffer, size, callback);

  getchar();

  return 0;
}

