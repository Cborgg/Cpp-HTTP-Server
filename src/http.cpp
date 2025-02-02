#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <thread>

#include "http.h"
#include "response.h"
#include "request.h"

using namespace HTTP;

Server::Server(void) {
  
}

Server::Server(bool while_loop) {
  this->while_loop = while_loop;
}

void Server::listen(int port) const {
  int socket;
  struct sockaddr_in address;
  int opt = 1;

  if ((socket = ::socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(1);
  }

  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(1);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(0);
  }

  if (::listen(socket, 3) < 0) {
    perror("listen");
    exit(0);
  }

  do {
    int new_socket = 0;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);

    if ((new_socket = accept(socket, (struct sockaddr *) &client_addr, (socklen_t*) &addrlen)) < 0) {
      perror("accept");
      exit(0);
    }

    auto connection_handler = [this](int socket) {
      // while (true) {
        char buffer[1024];
        bzero(buffer, 1024);

        if (read(socket, buffer, 1024) <= 0) {
          close(socket);
          // break;
        }

        handle(Request(buffer), Response(socket));
      // }
    };

    // std::thread th(connection_thread, new_socket); // this is not multithreading @todo multithreading
    // th.join();
    connection_handler(new_socket);
  } while (this->while_loop);
}
