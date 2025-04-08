#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/ioctl.h> 
#include "message_slot.h" 

int main(int argc, char *argv[]) {
    int fd;
    unsigned long channel_id;
    if (argc != 4) {
        perror("not the right num of arg");
        exit(1);
    }
    channel_id = strtoul(argv[2], NULL, 10);

    fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device file");
        exit(1);
    }

    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0) {
        perror("Failed to set the channel ID");
        close(fd);
        exit(1);
    }

    if (write(fd, argv[3], strlen(argv[3])) < 0) {
        perror("Failed to write the message");
        close(fd);
        exit(1);
    }
    close(fd);

    exit(0);
}
