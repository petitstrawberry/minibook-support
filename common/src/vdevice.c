#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"

// Emit the event
void emit(int fd, int type, int code, int value) {
    struct input_event event = {.type = type, .code = code, .value = value};
    // Set the timestamp (dummy)
    event.time.tv_sec = 0;
    event.time.tv_usec = 0;
    write(fd, &event, sizeof(event));
}

// Check if the bit is enabled
int is_enabled_bit(u_int8_t *bits, int bit) {
    return (bits[bit / 8] & (1 << (bit % 8))) > 0;
}

// Clone the enabled event types and codes of the device
void clone_enabled_event_types_and_codes(int fd, int fd_clone) {
    debug_printf("Making the clone of the device\n");
    // Get the event types
    u_int8_t event_types[EV_MAX / 8 + 1];
    memset(event_types, 0, sizeof(event_types));
    ioctl(fd, EVIOCGBIT(0, sizeof(event_types)), event_types);

    for (int event_type = 1; event_type < EV_MAX; event_type++) {
        if (is_enabled_bit(event_types, event_type)) {
            debug_printf("event_type: %d\n", event_type);
            ioctl(fd_clone, UI_SET_EVBIT, event_type);

            // Get the event codes
            u_int8_t event_codes[KEY_MAX / 8 + 1];
            memset(event_codes, 0, sizeof(event_codes));
            ioctl(fd, EVIOCGBIT(event_type, sizeof(event_codes)), event_codes);

            for (int event_code = 0; event_code < KEY_MAX; event_code++) {
                if (is_enabled_bit(event_codes, event_code)) {
                    debug_printf("  code: %d\n", event_code);
                    ioctl(fd_clone,
                          _IOW(UINPUT_IOCTL_BASE, 100 + event_type, int),
                          event_code);
                    // Setup the abs values
                    if (event_type == EV_ABS) {
                        // Get the abs info
                        struct input_absinfo absinfo;
                        memset(&absinfo, 0, sizeof(absinfo));
                        ioctl(fd, EVIOCGABS(event_code), absinfo);
                        // Set the abs info
                        ioctl(fd_clone, EVIOCSABS(event_code), absinfo);

                        // Print the abs info
                        debug_printf("    absinfo: \n");
                        debug_printf("      value: %d\n", absinfo.value);
                        debug_printf("      minimum: %d\n", absinfo.minimum);
                        debug_printf("      maximum: %d\n", absinfo.maximum);
                        debug_printf("      fuzz: %d\n", absinfo.fuzz);
                        debug_printf("      flat: %d\n", absinfo.flat);
                        debug_printf("      resolution: %d\n", absinfo.resolution);
                    }
                }
            }
        }
    }
}