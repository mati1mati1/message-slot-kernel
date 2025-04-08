#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include "message_slot.h" 


int main(int argc, char *argv[]) {
    int fd;
    unsigned long channel_id;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;
    if (argc != 3) {
        perror("not the right num of arg");
        exit(1);
    }

    // Convert channel id from string to unsigned long
    channel_id = strtoul(argv[2], NULL, 10);

    // Open the specified message slot device file
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device file");
        exit(1);
    }

    // Set the channel id to the id specified on the command line
    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0) {
        perror("Failed to set the channel ID");
        close(fd);
        exit(1);
    }

    // Read a message from the message slot file to a buffer
    bytes_read = read(fd, buffer, sizeof(buffer) - 1); // Leave space for null terminator
    if (bytes_read < 0) {
        perror("Failed to read the message");
        close(fd);
        exit(1);
    }

    // Close the device
    close(fd);

    // Print the message to standard output
    if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
        perror("Failed to write the message to stdout");
        exit(1);
    }

    // Exit the program with exit value 0
    exit(0);
}
