#include <linux/uinput.h>
#include <unistd.h>

// Emit the event
void emit(int fd, int type, int code, int value) {
    struct input_event event = {.type = type, .code = code, .value = value};
    // Set the timestamp (dummy)
    event.time.tv_sec = 0;
    event.time.tv_usec = 0;
    write(fd, &event, sizeof(event));
}
