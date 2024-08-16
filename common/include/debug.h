#ifndef __DEBUG_H__
#define __DEBUG_H__

// Enable debug
void enable_debug();
// Disable debug
void disable_debug();
// Get Debug status
int get_debug_status();
// Debug printf
void debug_printf(const char *fmt, ...);

#endif // __DEBUG_H__
