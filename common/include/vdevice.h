// Emit the event
void emit(int fd, int type, int code, int value);

// Clone the enabled event types and codes of the device
void clone_enabled_event_types_and_codes(int fd, int fd_clone);