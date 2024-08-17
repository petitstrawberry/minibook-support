#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "debug.h"
#include "server.h"
#include "vdevice.h"

#define VERSION "tabletmoded 1.0.0"

#define KEYBOARDD_SOCK "/var/run/keyboardd.sock"
#define TRACKPOINTERD_SOCK "/var/run/trackpointerd.sock"

server_t *server_addr = NULL;

int output = -1;
int is_enabled_tabletmode = 0;

// Recovery the device
void recovery_device() {
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

    // Enable the synchronization events
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    // Enable the switch events
    ioctl(fd, UI_SET_EVBIT, EV_SW);

    // Enable the tabletmode switch
    ioctl(fd, UI_SET_SWBIT, SW_TABLET_MODE);

    // Setup the device
    struct uinput_setup uisetup = {0};
    memset(&uisetup, 0, sizeof(uisetup));
    strcpy(uisetup.name, "MiniBookSupport Virtual Switch");
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
    printf("Usage: ./tabletmoded [-d] [-h] [--version]\n");
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

// Send command to the other devices
int send_command(const char *path, uint8_t cmd, uint8_t data) {
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Cannot create the socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Cannot connect to the server");
        close(sockfd);
        return -1;
    }

    uint8_t buffer[2] = {cmd, data};
    if (send(sockfd, buffer, sizeof(buffer), 0) == -1) {
        perror("Cannot send the command");
        return -1;
    }

    uint8_t res;
    if (recv(sockfd, &res, sizeof(res), 0) == -1) {
        perror("Cannot receive the response");
        return -1;
    }

    close(sockfd);
    return (int)res;
}

void set_tabletmode(int value) {
    if (send_command(KEYBOARDD_SOCK, 0, !value) == -1) {
        perror("Cannot send the command to the keyboardd");
    }
    if (send_command(TRACKPOINTERD_SOCK, 0, !value) == -1) {
        perror("Cannot send the command to the trackpointerd");
    }

    emit(output, EV_SW, SW_TABLET_MODE, value);
    emit(output, EV_SYN, SYN_REPORT, 0);
}

// Server callback
uint8_t server_callback(uint8_t type, uint8_t data) {
    debug_printf("Server callback: %d %d\n", type, data);
    switch (type) {
    case 0:
        debug_printf("Set tabletmode: %d\n", data);
        is_enabled_tabletmode = data;
        set_tabletmode(data);
        return 0;
    case 1:
        return (uint8_t)is_enabled_tabletmode;
    default:
        break;
    }
    return 0;
}

// Main
int main(int argc, char *argv[]) {
    // Parse the command line arguments
    parse_args(argc, argv);

    output = new_device();
    if (output == -1) {
        perror("Cannot create the output device");
        recovery_device();
        return (EXIT_FAILURE);
    }

    // Register the signal handler
    signal(SIGINT, sigint_handler);

    // Create the server
    server_t server;
    server_addr = &server;
    // Setup the server
    setup_server(&server, "/var/run/tabletmoded.sock", server_callback);

    // Start the server
    if (start_server(&server) == 1) {
        perror("Cannot start the server");
        recovery_device();
        return (EXIT_FAILURE);
    }

    // Main loop
    while (1) {
        sleep(1);
    }

    // Never reach here...
    ioctl(output, UI_DEV_DESTROY);
    close(output);
    return (EXIT_SUCCESS);
}