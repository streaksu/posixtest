#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>

const char *server_bind = "serverbind.sock";
const char *client_bind = "clientbind.sock";
const char *tmp_file    = "randomfiletrustme";

void test_unix_dgram() {
    // Create the socket and bind.
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, server_bind);
    unlink(server_bind);

    int server = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server < 0) {
        perror("Could not create socket: ");
        return;
    }
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Could not bind socket: ");
        return;
    }

    // Fork and send the test string.
    const char *test_message = "123456789";
    if (fork() == 0) {
        // Create our own socket and connect to the server.
        struct sockaddr_un addr2;
        addr2.sun_family = AF_UNIX;
        strcpy(addr2.sun_path, client_bind);
        unlink(client_bind);

        int client = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (client < 0) {
            perror("Could not create socket: ");
        }
        if (bind(client, (struct sockaddr *)&addr2, sizeof(addr2)) < 0) {
            perror("Could not bind socket: ");
        }

        struct sockaddr_un addr3;
        addr3.sun_family = AF_UNIX;
        strcpy(addr3.sun_path, server_bind);
        int len = strlen(addr3.sun_path) + sizeof(addr3.sun_family);
        if (connect(client, (struct sockaddr *)&addr3, len) == -1) {
            perror("connect");
            exit(1);
        }

        char buffer[10];
        strcpy(buffer, test_message);
        if (send(client, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("Could not send to server:");
            exit(1);
        }

        // Send thhe stdout file descriptor as example.
#ifdef S_IRWXU
        int example_fd = open(tmp_file, O_CREAT | O_RDWR, S_IRWXU);
#else
        int example_fd = open(tmp_file, O_CREAT | O_RDWR, 0);
#endif
        if (example_fd == -1) {
            perror("Could not open tmpfile");
        }
        lseek(example_fd, 69, SEEK_SET);

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        struct cmsghdr *cmsg;
        char cmsgbuf[CMSG_SPACE(sizeof(example_fd))];
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf); // necessary for CMSG_FIRSTHDR to return the correct value.
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(example_fd));
        memcpy(CMSG_DATA(cmsg), &example_fd, sizeof(example_fd));
        msg.msg_controllen = cmsg->cmsg_len; // total size of all control blocks

        if (sendmsg(client, &msg, 0) == -1) {
            perror("Could not send message to server:");
        }

        close(client);
        unlink(client_bind);
        exit(0);
    } else {
        char buffer[1024];
        struct sockaddr_un client;
        socklen_t clientlen = sizeof(client);

        int len = recvfrom(server, buffer, 1024, 0, (struct sockaddr *)&client, &clientlen);
        if (len <= 0) {
            perror("Could not receive: ");
            return;
        }
        if (strcmp (buffer, test_message) != 0) {
            printf("'%s'(%d) vs '%s'(%d)", buffer, len, test_message, strlen(test_message));
            return;
        }

        // Receive message.
        struct msghdr child_msg;
        memset(&child_msg,   0, sizeof(child_msg));
        char cmsgbuf[CMSG_SPACE(sizeof(int))];
        child_msg.msg_control = cmsgbuf; // make place for the ancillary message to be received.
        child_msg.msg_controllen = sizeof(cmsgbuf);

        int passed_fd;
        int rc = recvmsg(server, &child_msg, 0);
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&child_msg);
        if (cmsg == NULL || cmsg -> cmsg_type != SCM_RIGHTS) {
            puts("The first control structure contains no file descriptor");
            return;
        }
        memcpy(&passed_fd, CMSG_DATA(cmsg), sizeof(passed_fd));
        if (lseek(passed_fd, 0, SEEK_CUR) == 69) {
            puts("Success");
        } else {
            printf("Failure on recvmsg: %d vs %d\n", lseek(passed_fd, 0, SEEK_CUR), 69);
        }

        unlink(tmp_file);
        close(server);
        unlink(server_bind);
    }
}

void test_unix_stream() {
    // Create the socket and bind.
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, server_bind);
    unlink(server_bind);

    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server < 0) {
        perror("Could not create socket: ");
        return;
    }
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Could not bind socket: ");
        return;
    }

    // Fork and send the test string.
    const char *test_message = "123456789";
    if (fork() == 0) {
        // Create our own socket and connect to the server.
        struct sockaddr_un addr2;
        addr2.sun_family = AF_UNIX;
        strcpy(addr2.sun_path, client_bind);
        unlink(client_bind);

        int client = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client < 0) {
            perror("Could not create socket: ");
        }
        if (bind(client, (struct sockaddr *)&addr2, sizeof(addr2)) < 0) {
            perror("Could not bind socket: ");
        }

        struct sockaddr_un addr3;
        addr3.sun_family = AF_UNIX;
        strcpy(addr3.sun_path, server_bind);
        int len = strlen(addr3.sun_path) + sizeof(addr3.sun_family);
        if (connect(client, (struct sockaddr *)&addr3, len) == -1) {
            perror("connect");
            exit(1);
        }

        char buffer[10];
        strcpy(buffer, test_message);
        if (send(client, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("Could not send to server:");
            exit(1);
        }

        // Send thhe stdout file descriptor as example.
#ifdef S_IRWXU
        int example_fd = open(tmp_file, O_CREAT | O_RDWR, S_IRWXU);
#else
        int example_fd = open(tmp_file, O_CREAT | O_RDWR, 0);
#endif
        if (example_fd == -1) {
            perror("Could not open tmpfile");
        }
        lseek(example_fd, 69, SEEK_SET);

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        struct cmsghdr *cmsg;
        char cmsgbuf[CMSG_SPACE(sizeof(example_fd))];
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf); // necessary for CMSG_FIRSTHDR to return the correct value.
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(example_fd));
        memcpy(CMSG_DATA(cmsg), &example_fd, sizeof(example_fd));
        msg.msg_controllen = cmsg->cmsg_len; // total size of all control blocks

        if (sendmsg(client, &msg, 0) == -1) {
            perror("Could not send message to server:");
        }

        close(client);
        unlink(client_bind);
        exit(0);
    } else {
        char buffer[1024];
        struct sockaddr_un client;
        socklen_t clientlen = sizeof(client);
        struct sockaddr_storage their_addr;
        socklen_t addr_size;

        listen(server, 1);
        int child = accept(server, (struct sockaddr *)&their_addr, &addr_size);
        int len = recv(child, buffer, 1024, 0);
        if (len <= 0) {
            perror("Could not receive: ");
            return;
        }
        if (strcmp (buffer, test_message) != 0) {
            printf("'%s'(%d) vs '%s'(%d)", buffer, len, test_message, strlen(test_message));
            return;
        }

        // Receive message.
        struct msghdr child_msg;
        memset(&child_msg,   0, sizeof(child_msg));
        char cmsgbuf[CMSG_SPACE(sizeof(int))];
        child_msg.msg_control = cmsgbuf; // make place for the ancillary message to be received.
        child_msg.msg_controllen = sizeof(cmsgbuf);

        int passed_fd;
        int rc = recvmsg(server, &child_msg, 0);
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&child_msg);
        if (cmsg == NULL || cmsg -> cmsg_type != SCM_RIGHTS) {
            puts("The first control structure contains no file descriptor");
            return;
        }
        memcpy(&passed_fd, CMSG_DATA(cmsg), sizeof(passed_fd));
        if (lseek(passed_fd, 0, SEEK_CUR) == 69) {
            puts("Success");
        } else {
            printf("Failure on recvmsg: %d vs %d\n", lseek(passed_fd, 0, SEEK_CUR), 69);
        }

        unlink(tmp_file);
        close(server);
        unlink(server_bind);
    }
}

int main() {
    puts("Testing AF_UNIX / SOCK_DGRAM");
    test_unix_dgram();

    puts("Testing AF_UNIX / SOCK_STREAM");
    test_unix_stream();
}
