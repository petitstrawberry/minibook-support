#include "server.h"

#include <bits/pthreadtypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

// prepare socket
int prepare_socket(const char *path) {
  // ソケットの作成とパーミッションの設定例
  int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);
  unlink(addr.sun_path);
  int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
  if (ret == 0) {
    chmod(addr.sun_path, 0777);
  } else {
    perror("bind");
    return -1;
  }
  return sockfd;
}

// Server thread
void *thread_server(void *arg) {
  server_t *server = (server_t *)arg;
  int sockfd = server->sockfd;
  uint8_t (*callback)(uint8_t, uint8_t) = server->callback;

  // Listen
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return NULL;
  }

  // 一度の接続で1回のみ処理を行う
  while (server->is_running) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    // クライアントからの接続を待つ
    server->connfd = accept(sockfd, NULL, NULL);
    if (server->connfd == -1) {
      perror("accept");
      break;
    }
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    // データの受信
    uint8_t buf[2];
    ssize_t size = recv(server->connfd, buf, sizeof(buf), 0);
    if (size == -1) {
      perror("recv");
      break;
    } else if (size == 0) {
      break;
    }

    // データの処理
    uint8_t res = callback(buf[0], buf[1]);
    // レスポンスの送信
    if (send(server->connfd, &res, sizeof(res), 0) == -1) {
      perror("send");
      break;
    }

    // ソケットのクローズ
    close(server->connfd);
  }
  close(sockfd);
  return NULL;
}

// Setup server
void setup_server(server_t *server, const char *path,
                  uint8_t (*callback)(uint8_t, uint8_t)) {
  server->path = path;
  server->sockfd = prepare_socket(path);
  server->callback = callback;
}

// start server
pthread_t start_server(server_t *server) {
  server->is_running = 1;
  pthread_create(&server->thread, NULL, thread_server, server);
  return server->thread;
}

// stop server
int stop_server(server_t *server) {
  server->is_running = 0;
  pthread_cancel(server->thread);

  if (pthread_join(server->thread, NULL) != 0) {
    perror("pthread_join");
    return -1;
  }

  if (close(server->sockfd) == -1) {
    perror("close");
    return -1;
  }

  if (unlink(server->path) == -1) {
    perror("unlink");
    return -1;
  }

  return 0;
}
