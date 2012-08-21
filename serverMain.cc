#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "downloadServer.h"
#include "debug.h"

char* map(char* filename, int& size)
{
    int fd;
    if ((fd = open(filename, O_RDONLY)) == -1) {
        perror("map(): open(): \n");
        exit(1);
    }    

    struct stat buf;

    if (fstat(fd, &buf)) {
        perror("map(): fstat(): \n");
        exit(1);
    }

    size = buf.st_size;
    
    void* map;
    map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("map(): mmap(): \n");
        exit(1);
    }

    return (char*) map;
}


/* usage: server filename */
int main (int arc, char* argv[])
{
  ech_init_module();

  DownloadServer::Document document;
  int size;

  document.data = map(argv[1], size);
  document.size = size;

  DownloadServer server(0x1234, &document, 1);
  server.start();
  DEBUGOUT("download server up and running\n");

  getchar();

  printf("shutting down download server\n");
  server.join();

  return 0;
}

