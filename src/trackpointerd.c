#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define INPUT_DEVICE "/dev/input/event11"
#define OUT_EVENT KEY_V

int input = -1, output = -1;
int is_enabled_debug = 0;
int is_enabled_passthrough = 1;
int is_enabled_calibration = 0;

void recovery_device() {
    // Enable the pointer
    if (input != -1) {
        ioctl(input, EVIOCGRAB, 0);
        close(input);
    }
    if (output != -1) {
        ioctl(output, UI_DEV_DESTROY);
        close(output);
    }
}

// Signal handler
void sigint_handler(int signum) {
    recovery_device();
    exit(EXIT_SUCCESS);
}

// Debug printf
void debug_printf(const char *str, ...) {
    if (is_enabled_debug) {
        va_list args;
        va_start(args, str);
        vprintf(str, args);
        va_end(args);
    }
}

int new_device() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Cannot open the output device");
        recovery_device();
        exit(EXIT_FAILURE);
    }
    // Enable the buttons of the pointer
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

    // Enable the pointer
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    // Setup the device
    struct uinput_setup uisetup = {0};
    memset(&uisetup, 0, sizeof(uisetup));
    strcpy(uisetup.name, "Trsckpointerd Virtual Pointer");
    uisetup.id.bustype = BUS_USB;
    uisetup.id.vendor = 0x1234;
    uisetup.id.product = 0x5678;
    uisetup.id.version = 1;

    if (ioctl(fd, UI_DEV_SETUP, &uisetup) < 0) {
        perror("Failed to setup the virtusl device");
        recovery_device();
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        perror("Failed to create the virtual device");
        recovery_device();
        exit(EXIT_FAILURE);
    }
    return fd;
}

// Emit the event
void emit(int output, int type, int code, int value) {
    struct input_event event = {.type = type, .code = code, .value = value};
    // Set the timestamp (dummy)
    event.time.tv_sec = 0;
    event.time.tv_usec = 0;
    write(output, &event, sizeof(event));
}

// Print the help message
void print_help() {
    printf("Usage: ./pointer [-d] [-c]\n");
    printf("Options:\n");
    printf("  -d: Enable debug mode\n");
    printf("  -c: Enable calibration mode\n");
}

// Parse the command line arguments
void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            is_enabled_debug = 1;
        } else if (strcmp(argv[i], "-c") == 0) {
            is_enabled_calibration = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help();
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_help();
            exit(EXIT_FAILURE);
        }
    }
}

// Main
int main(int argc, char *argv[]) {
    // Parse the command line arguments
    parse_args(argc, argv);

    if (!is_enabled_calibration) {
        // Warning, recommend to enable calibration mode
        fprintf(stderr, "Warning: Calibration mode is disabled\n");
    }

    input = open(INPUT_DEVICE, O_RDWR);
    if (input == -1) {
        perror("Cannot open the input device");
        return (EXIT_FAILURE);
    }
    output = new_device();
    if (output == -1) {
        perror("Cannot create the output device");
        recovery_device();
        return (EXIT_FAILURE);
    }
    // Disable the input device
    ioctl(input, EVIOCGRAB, 1);

    // Register the signal handler
    signal(SIGINT, sigint_handler);

    // Initialize the relative values
    int rel_x = 0, rel_y = 0;
    int rel_x_bool = 0, rel_y_bool = 0;

    // Main loop
    while (1) {
        struct input_event event;

        ssize_t result = read(input, &event, sizeof(event));
        if (result == -1) {
            perror("Stopping");
            recovery_device();
            exit(EXIT_FAILURE);
        } else if (result != sizeof(event)) {
            perror("Invalid event size");
            recovery_device();
            exit(EXIT_FAILURE);
        }
        switch (event.type) {
            case EV_KEY:
                debug_printf("EV_KEY: %d %d\n", event.code, event.value);

                // passthrough
                if (is_enabled_passthrough) {
                    emit(output, event.type, event.code, event.value);
                    emit(output, EV_SYN, SYN_REPORT, 0);
                }
                break;
            case EV_REL:
                // Calibrate the pointer
                if (event.code == REL_X) {
                    debug_printf("REL_X: %d\n", event.value);

                    if (is_enabled_calibration) {
                        if (event.value == -1) {
                            rel_x = 0;
                        } else {
                            rel_x = event.value + 1;
                        }
                        rel_x_bool = 1;
                    } else {
                        rel_x = event.value;
                        rel_x_bool = 1;
                    }
                } else if (event.code == REL_Y) {
                    debug_printf("REL_Y: %d\n", event.value);

                    if (is_enabled_calibration) {
                        if (event.value == -1) {
                            rel_y = 0;
                        } else {
                            rel_y = event.value + 1;
                        }
                        rel_y_bool = 1;
                    } else {
                        rel_y = event.value;
                        rel_y_bool = 1;
                    }
                }

                if (is_enabled_passthrough && rel_x_bool && rel_y_bool) {
                    if (is_enabled_debug && is_enabled_calibration) {
                        debug_printf("REL_X (calibrated): %d\n", rel_x);
                        debug_printf("REL_Y (calibrated): %d\n", rel_y);
                    }

                    emit(output, EV_REL, REL_X, rel_x);
                    emit(output, EV_REL, REL_Y, rel_y);
                    // emit(output, EV_REL, REL_X, 0);
                    emit(output, EV_SYN, SYN_REPORT, 0);
                    rel_x_bool = 0;
                    rel_y_bool = 0;
                }
                break;
        }
    }

    // Never reach here...
    ioctl(output, UI_DEV_DESTROY);
    // Enable the pointer
    ioctl(input, EVIOCGRAB, 0);
    close(input);
    close(output);
    return (EXIT_SUCCESS);
}
