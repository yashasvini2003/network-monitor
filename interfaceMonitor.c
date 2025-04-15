#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/network_monitor.sock"
#define BUFFER_SIZE 128

int socket_fd;

void handle_signal(int sig) {
    close(socket_fd);
    printf("Shutting down interface monitor...\n");
    exit(0);
}


void read_stat(const char *interface, const char *stat_name, char *buffer) {
    char path[BUFFER_SIZE];
    if (strcmp(stat_name, "operstate") == 0) {
        snprintf(path, sizeof(path), "/sys/class/net/%s/%s", interface, stat_name);
    } else {
        snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", interface, stat_name);
    }
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Error opening stat file");
        strcpy(buffer, "N/A");
    } else {
        read(fd, buffer, BUFFER_SIZE - 1);
        buffer[strlen(buffer) - 1] = '\0'; 
        close(fd);
    }
}


void display_stats(const char *interface) {
    char state[BUFFER_SIZE], rx_bytes[BUFFER_SIZE], rx_dropped[BUFFER_SIZE], 
         rx_errors[BUFFER_SIZE], rx_packets[BUFFER_SIZE], 
         tx_bytes[BUFFER_SIZE], tx_dropped[BUFFER_SIZE], 
         tx_errors[BUFFER_SIZE], tx_packets[BUFFER_SIZE];
    

    read_stat(interface, "operstate", state);
    read_stat(interface, "rx_bytes", rx_bytes);
    read_stat(interface, "rx_dropped", rx_dropped);
    read_stat(interface, "rx_errors", rx_errors);
    read_stat(interface, "rx_packets", rx_packets);
    read_stat(interface, "tx_bytes", tx_bytes);
    read_stat(interface, "tx_dropped", tx_dropped);
    read_stat(interface, "tx_errors", tx_errors);
    read_stat(interface, "tx_packets", tx_packets);
    
    printf("Interface: %s state:%s\n", interface, state);
    printf("  rx_bytes:%s rx_dropped:%s rx_errors:%s rx_packets:%s\n", 
           rx_bytes, rx_dropped, rx_errors, rx_packets);
    printf("  tx_bytes:%s tx_dropped:%s tx_errors:%s tx_packets:%s\n", 
           tx_bytes, tx_dropped, tx_errors, tx_packets);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char *interface = argv[1];
    struct sockaddr_un addr;
    
    signal(SIGINT, handle_signal);
    
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    
    if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Connection error");
        return -1;
    }
    
    write(socket_fd, "Ready", strlen("Ready"));
    
    while (1) {
        display_stats(interface);
        
        sleep(1);
    }
    
    handle_signal(SIGINT);  
    return 0;
}

