#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "debug.h"

static int is_enabled_debug = 0;

// Enable debug
void enable_debug() { is_enabled_debug = 1; }

// Disable debug
void disable_debug() { is_enabled_debug = 0; }

// Get Debug status
int get_debug_status() { return is_enabled_debug; }

// Debug printf
void debug_printf(const char *fmt, ...) {
  if (is_enabled_debug) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}
