#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/network_monitor.sock"
#define MAX_INTERFACES 10
#define BUFFER_SIZE 128

int socket_fd;
fd_set fds;

void handle_signal(int sig) {
    close(socket_fd);
    unlink(SOCKET_PATH);
    printf("Shutting down network monitor...\n");
    exit(0);
}


void start_interface_monitor(const char *interface) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("./interfaceMonitor", "interfaceMonitor", interface, NULL);
        perror("Failed to start interface monitor");
        exit(EXIT_FAILURE);
    }
}

int main() {
    struct sockaddr_un addr;
    char interfaces[MAX_INTERFACES][BUFFER_SIZE];
    int num_interfaces;
    
    signal(SIGINT, handle_signal);

    printf("Enter number of interfaces to monitor: ");
    scanf("%d", &num_interfaces);
    
    for (int i = 0; i < num_interfaces; ++i) {
        printf("Enter interface name #%d: ", i + 1);
        scanf("%s", interfaces[i]);
    }
    
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);
    
    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Socket bind error");
        return -1;
    }
    
    listen(socket_fd, MAX_INTERFACES);
    
    for (int i = 0; i < num_interfaces; ++i) {
        start_interface_monitor(interfaces[i]);
    }
    
    FD_ZERO(&fds);
    FD_SET(socket_fd, &fds);
    
    printf("Network Monitor running. Press Ctrl+C to shut down.\n");
    
    while (1) {
        fd_set temp_fds = fds;
        if (select(socket_fd + 1, &temp_fds, NULL, NULL, NULL) == -1) {
            perror("Select error");
            break;
        }
        
        if (FD_ISSET(socket_fd, &temp_fds)) {
            int conn_fd = accept(socket_fd, NULL, NULL);
            if (conn_fd == -1) {
                perror("Accept error");
                break;
            }
            printf("Interface monitor connected.\n");
            FD_SET(conn_fd, &fds);
        }
    }
    
    handle_signal(SIGINT);
    return 0;
}
