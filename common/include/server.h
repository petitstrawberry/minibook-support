#ifndef __SERVER_H__
#define __SERVER_H__

#include <pthread.h>
#include <stdint.h>

// Server struct
typedef struct {
  pthread_t thread;
  int is_running;
  const char *path;
  int sockfd;
  int connfd;
  uint8_t (*callback)(uint8_t, uint8_t);
} server_t;

// Setup server
void setup_server(server_t *server, const char *path,
                  uint8_t (*callback)(uint8_t, uint8_t));

// Start server
pthread_t start_server(server_t *server);
// Stop server
int stop_server(server_t *server);

#endif
