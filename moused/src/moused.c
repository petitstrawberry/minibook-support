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

#include "debug.h"
#include "device.h"
#include "server.h"
#include "vdevice.h"

#define MINIBOOK_INPUT_DEVICE "/dev/input/by-id/usb-0603_0003-event-mouse"
#define MINIBOOKX_INPUT_DEVICE                                                 \
    "/dev/input/by-path/"                                                      \
    "pci-0000:00:15.3-platform-i2c_designware.3-event-mouse"

#define VERSION "moused 1.1.1"

server_t *server_addr = NULL;

int input = -1, output = -1;
// int is_enabled_debug = 0;
int is_enabled_passthrough = 1;
int is_enabled_calibration = 0;

// Recovery the device
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
    if (server_addr != NULL) {
        stop_server(server_addr);
    }
    recovery_device();
    exit(EXIT_SUCCESS);
}

int new_device() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Cannot open the output device");
        recovery_device();
        exit(EXIT_FAILURE);
    }

    // Clone the enabled event types and codes of the device
    clone_enabled_event_types_and_codes(input, fd);

    // Setup the device
    struct uinput_setup uisetup = {0};
    memset(&uisetup, 0, sizeof(uisetup));
    strcpy(uisetup.name, "MiniBookSupport Virtual Mouse");
    uisetup.id.bustype = BUS_USB;
    uisetup.id.vendor = 0x1234;
    uisetup.id.product = 0x5678;
    uisetup.id.version = 1;

    if (ioctl(fd, UI_DEV_SETUP, &uisetup) < 0) {
        perror("Failed to setup the virtual device");
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

// Print the help message
void print_help() {
    printf("Usage: ./moused [-d] [-c] [-h] [--version]\n");
    printf("Options:\n");
    printf("  -d: Enable debug mode\n");
    printf("  -c: Enable calibration mode\n");
    printf("  -h: Print this help message\n");
    printf("  --version: Print the version\n");
}

// Parse the command line arguments
void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            enable_debug();
        } else if (strcmp(argv[i], "-c") == 0) {
            is_enabled_calibration = 1;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("%s\n", VERSION);
            exit(EXIT_SUCCESS);
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

// Manage the press / release of the key
int pressing_keys[KEY_MAX] = {0};
void press_key(int key) {
    debug_printf("Press key: %d\n", key);
    pressing_keys[key] = 1;
}
void release_key(int key) {
    debug_printf("Release key: %d\n", key);
    pressing_keys[key] = 0;
}
void release_unreleased_keys() {
    for (int i = 0; i < KEY_MAX; i++) {
        if (pressing_keys[i]) {
            release_key(i);
            // Release the key
            emit(output, EV_KEY, i, 0);
            emit(output, EV_SYN, SYN_REPORT, 0);
        }
    }
}

// Server callback
uint8_t server_callback(uint8_t type, uint8_t data) {
    debug_printf("Server callback: %d %d\n", type, data);
    switch (type) {
    case 0:
        debug_printf("Set passthrough: %d\n", data);
        is_enabled_passthrough = data;
        if (!is_enabled_passthrough) {
            release_unreleased_keys();
        }
        return 0;
    case 1:
        return (uint8_t)is_enabled_passthrough;
    default:
        break;
    }
    return 0;
}

// Main
int main(int argc, char *argv[]) {
    // Parse the command line arguments
    parse_args(argc, argv);

    // Check the device model
    char device_model[256] = {0};
    get_device_model(device_model, sizeof(device_model));
    debug_printf("Device model: %s\n", device_model);
    if (strstr(device_model, "MiniBook") == NULL) {
        fprintf(stderr, "This device is not supported\n");
        recovery_device();
        return (EXIT_FAILURE);
    }
    if (strncmp(device_model, "MiniBook X", 10) == 0) {
        input = open(MINIBOOKX_INPUT_DEVICE, O_RDWR);
        is_enabled_calibration = 0;
    } else {
        input = open(MINIBOOK_INPUT_DEVICE, O_RDWR);
    }
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

    // Wait until the all keys are released
    debug_printf("Wait until all keys are released\n");
    // Set the input device to non-blocking mode
    fcntl(input, F_SETFL, O_NONBLOCK);
    int pressing_count = 0;
    int count = 0;
    while (pressing_count != 0 || count < 10) {
        struct input_event event;
        ssize_t result = read(input, &event, sizeof(event));
        if (result == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                count++;
                usleep(100000); // 100ms
                continue;
            }
            perror("read");
            recovery_device();
            exit(EXIT_FAILURE);
        } else if (result == sizeof(event)) {
            if (event.type == EV_KEY) {
                if (event.value > 0) {
                    if (pressing_keys[event.code] == 0) {
                        press_key(event.code);
                        pressing_count++;
                    }
                } else if (event.value == 0) {
                    if (pressing_keys[event.code] == 1) {
                        release_key(event.code);
                        pressing_count--;
                    }
                }
            }
        }
        count++;
        usleep(100000); // 100ms
    }
    debug_printf("All keys are released\n");
    // Set the input device to blocking mode
    fcntl(input, F_SETFL, 0);

    // Disable the input device
    ioctl(input, EVIOCGRAB, 1);
    // Register the signal handler
    signal(SIGINT, sigint_handler);

    // Create the server
    server_t server;
    server_addr = &server;
    // Setup the server
    setup_server(&server, "/var/run/moused.sock", server_callback);

    // Start the server
    if (start_server(&server) == 1) {
        perror("Cannot start the server");
        recovery_device();
        return (EXIT_FAILURE);
    }

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
        case EV_SYN:
            debug_printf("EV_SYN: %d %d\n", event.code, event.value);
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
            }
            break;
        case EV_MSC:
            debug_printf("EV_MSC: %d %d\n", event.code, event.value);
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
            }
            break;
        case EV_KEY:
            debug_printf("EV_KEY: %d %d\n", event.code, event.value);
            // passthrough
            if (is_enabled_passthrough) {
                if (event.value > 0) {
                    press_key(event.code);
                } else if (event.value == 0) {
                    release_key(event.code);
                }
                emit(output, event.type, event.code, event.value);
            }
            break;
        case EV_REL:
            // Calibrate the pointer
            if (event.code == REL_X) {
                int rel_x = event.value;
                debug_printf("REL_X: %d\n", event.value);
                if (is_enabled_calibration) {
                    if (event.value == -1) {
                        rel_x = 0;
                    } else {
                        rel_x = event.value + 1;
                    }
                    debug_printf("REL_X (calibrated): %d\n", rel_x);
                } else {
                    rel_x = event.value;
                }
                if (is_enabled_passthrough) {
                    emit(output, event.type, event.code, rel_x);
                }
            } else if (event.code == REL_Y) {
                int rel_y = event.value;
                debug_printf("REL_Y: %d\n", event.value);

                if (is_enabled_calibration) {
                    if (event.value == -1) {
                        rel_y = 0;
                    } else {
                        rel_y = event.value + 1;
                    }
                    debug_printf("REL_Y (calibrated): %d\n", rel_y);
                } else {
                    rel_y = event.value;
                }
                if (is_enabled_passthrough) {
                    emit(output, event.type, event.code, rel_y);
                }
            }
            break;
        case EV_ABS:
            debug_printf("EV_ABS: %d %d\n", event.code, event.value);
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
            }
            break;
        default:
            debug_printf("Other: %d %d\n", event.code, event.value);
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
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
