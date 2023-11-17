#include "fileUtils.h"
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *logFilePath;

int logToFile(const char *logFilePath, const char *text) {
  if (!logFilePath) {
    return -1;
  }

  FILE *file = fopen(logFilePath, "a");
  if (file == NULL) {
    return 1;
  }

  fprintf(file, text);
  fprintf(file, "\n");

  fclose(file);

  return 0;
}

int getBatteryLevel() {
  int batteryLevel =
      std::atoi(readFile("/sys/class/power_supply/BAT0/capacity"));

  return batteryLevel;
}

int sendNotification(char *batteryLevel) {
  char textStart[] = "notify-send 'battery level: ";
  char *command = strcat(strcat(textStart, batteryLevel), "'");

  FILE *pipe = popen(command, "r");
  if (pipe == NULL) {
    return 1;
  }

  return 0;
}

void daemonStart() {
  while (true) {
    sleep(60);

    int batteryLevel = getBatteryLevel();
    char sbatteryLevel[10];

    sprintf(sbatteryLevel, "%d", batteryLevel);
    logToFile(logFilePath, sbatteryLevel);

    if (batteryLevel < 20) {
      sendNotification(sbatteryLevel);
    }
  }
}

int main(int argc, char **argv) {
  logFilePath = argv[1];
  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "Failed to fork the process.\n";
    return 1;
  }

  if (pid > 0) {
    return 0;
  }

  pid_t sid = setsid();
  if (sid < 0) {
    std::cerr << "Failed to create a new session ID.\n";
    return 1;
  }

  if ((chdir("/")) < 0) {
    std::cerr << "Failed to change the working directory.\n";
    return 1;
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);

  daemonStart();

  return 0;
}
