// Keyboard daemon

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "debug.h"
#include "server.h"
#include "vdevice.h"

#define INPUT_DEVICE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define VERSION "keyboardd 1.0.0"

server_t *server_addr = NULL;

int input = -1, output = -1;
static int is_enabled_passthrough = 1;
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

    // Enable the events of the keyboard
    // Enable the synchronization events
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    // Enable the miscellaneous events
    ioctl(fd, UI_SET_EVBIT, EV_MSC);
    ioctl(fd, UI_SET_MSCBIT, MSC_SCAN);

    // Enable the keys of the keyboard
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for (int i = 0; i < 256; i++) {
        ioctl(fd, UI_SET_KEYBIT, i);
    }

    // Setup the device
    struct uinput_setup uisetup = {0};
    memset(&uisetup, 0, sizeof(uisetup));
    strcpy(uisetup.name, "MiniBookSupport Virtual Keyboard");

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
    printf("Usage: ./keyboardd [-d] [-h] [--version]\n");
    printf("Options:\n");
    printf("  -d: Enable debug mode\n");
    printf("  -h: Print this help message\n");
    printf("  --version: Print the version\n");
}

// Parse the command line arguments
void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            enable_debug();
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

// Server callback
uint8_t server_callback(uint8_t type, uint8_t data) {
    debug_printf("Server callback: %d %d\n", type, data);
    switch (type) {
    case 0:
        debug_printf("Set passthrough: %d\n", data);
        is_enabled_passthrough = data;
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

    sleep(1);

    // Disable the input device
    ioctl(input, EVIOCGRAB, 1);
    // Register the signal handler
    signal(SIGINT, sigint_handler);

    // Create the server
    server_t server;
    server_addr = &server;
    // Setup the server
    setup_server(&server, "/var/run/keyboardd.sock", server_callback);

    // Start the server
    if (start_server(&server) == 1) {
        perror("Cannot start the server");
        recovery_device();
        return (EXIT_FAILURE);
    }

    printf("Keyboard daemon started\n");

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
            }
            break;
        case EV_SYN:
            debug_printf("EV_SYN: %d %d\n", event.code, event.value);

            // passthrough
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
            }
            break;
        case EV_MSC:
            debug_printf("EV_MSC: %d %d\n", event.code, event.value);

            // passthrough
            if (is_enabled_passthrough) {
                emit(output, event.type, event.code, event.value);
            }
            break;
        default:
            break;
        }
    }
    // Never reach here...

    // Stop the server
    stop_server(&server);

    ioctl(output, UI_DEV_DESTROY);
    // Enable the pointer
    ioctl(input, EVIOCGRAB, 0);
    close(input);
    close(output);
    return (EXIT_SUCCESS);
}
