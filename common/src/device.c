#include <stdio.h>

void get_device_model(char *device_model, size_t size) {
    FILE *fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if (fp == NULL) {
        return;
    }
    fgets(device_model, size, fp);
    fclose(fp);
}
