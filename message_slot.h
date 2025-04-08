#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

// The major device number.
// We don't rely on dynamic registration
// any more. We want ioctls to know this
// number at compile time.
#define MAJOR_NUM 235

// Set the message of the device driver
#define IOCTL_SET_ENC _IOW(MAJOR_NUM, 0, unsigned long)

#define DEVICE_FILE_NAME "message_slot"
#define SUCCESS 0
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)
#define BUFFER_SIZE 128
#endif