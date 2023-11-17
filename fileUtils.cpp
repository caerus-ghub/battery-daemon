#include "fileUtils.h"
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

char *readFile(char *filePath) {
  int readBytes = 1;
  char *buffer = (char *)malloc(1024);
  int bufferLen = 1024;
  int bufferDataAmount = 0;

  int fileDescriptor = open(filePath, O_RDONLY);
  if (fileDescriptor < 0) {
    return {""};
  }

  while (readBytes) {
    readBytes = read(fileDescriptor, buffer + bufferDataAmount, 1024);
    buffer = (char *)realloc(buffer, bufferLen + readBytes);

    if (buffer != nullptr) {
      bufferLen += readBytes;
      bufferDataAmount += readBytes;
    } else {
      readBytes = 0;
    }
  }

  buffer = (char *)realloc(buffer, bufferLen + 1);
  buffer[bufferLen - 1] = '\0';

  close(fileDescriptor);

  return buffer;
}
