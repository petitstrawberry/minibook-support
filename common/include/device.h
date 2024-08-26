#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdio.h>

void get_device_model(char *device_model, size_t size);
void get_event_path_by_name(const char *name, char *path, size_t size);

#endif
