#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

char* logFilePath;

int logToFile(const char* logFilePath, const char* text) {
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
  FILE* file = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (file == NULL) {
    return -1;
  }

  int batteryLavel;
  int readStatus = fscanf(file, "%d", &batteryLavel);

  if (readStatus) {
    logToFile(logFilePath, "--| error: can't get battery level");
  }

  fclose(file);

  return batteryLavel;
}

int sendNotification() {
  const char* command = "notify-send 'Plug charger!'";

  FILE* pipe = popen(command, "r");
  if (pipe == NULL) {
    return 1;
  }

  return 0;
}

void daemonStart() {
  std::cout << "My daemon is running!\n";

  while (true) {
    sleep(15);

    int batteryLevel = getBatteryLevel();
    char* logText;

    sprintf(logText, "%d", batteryLevel);
    logToFile(logFilePath, logText);

    if (batteryLevel < 20) {
      sendNotification();
    }
  }
}

int main(int argc, char** argv) {
  logFilePath = argv[1];
  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "Failed to fork the process.\n";
    return 1;
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
