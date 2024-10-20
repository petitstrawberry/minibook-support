#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>


void get_device_model(char *device_model, size_t size) {
    FILE *fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if (fp == NULL) {
        return;
    }
    fgets(device_model, size, fp);
    fclose(fp);
}

void get_event_path_by_name(const char *name, char *path, size_t size) {
    struct dirent **entry;
    int ndevice = 0;

    path[0] = '\0';

    ndevice = scandir("/dev/input", &entry, NULL, versionsort);
    if (ndevice < 0) {
        return;
    }

    for (int i = 0; i < ndevice; i++) {
        // Get the event device name
        char filename[256] = {0};
        char device_name[256] = {0};
        sprintf(filename, "/dev/input/%s", entry[i]->d_name);
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            continue;
        }
        ioctl(fd, EVIOCGNAME(sizeof(device_name)), device_name);
        close(fd);

        // Compare the device name
        if (strcmp(device_name, name) == 0) {
            // snprintf(path, size, "%s", filename);
            strncpy(path, filename, size);
            break;
        }
    }
}