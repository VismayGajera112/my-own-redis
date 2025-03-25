#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

void do_something(int connfd){
    char rbuf[64] = {};

    ssize_t n = read(connfd, rbuf, sizeof(rbuf)-1);
    if (n < 0){
        msg("read error");
        return;
    }

    fprintf(stderr, "client says: %s\n", rbuf);

    char wbuf[64] = "World";
    write(connfd, wbuf, strlen(wbuf));
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0){
        die("socket");
    }

     // this is needed for most server applications
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    int rv = bind(fd, (const struct sockaddr *) &addr, sizeof(addr));
    
    if (rv){
        die("bind");
    }

    // listen

    rv = listen(fd, SOMAXCONN);
    if (rv){
        die("listen");
    }

    while (true){
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t client_addr_len = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (connfd < 0){
            continue; // error
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}
