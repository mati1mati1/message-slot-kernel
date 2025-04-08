// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h> 
#include <linux/slab.h>  /* for memset. NOTE - not string.h!*/
#include "message_slot.h"


struct message_channel {
    int channel_id;
    char message[BUFFER_SIZE];
    int length;
    struct message_channel *next;
};

struct message_slots{
    int minor;
    struct message_channel *channels; 
    struct message_slots *next; 
};
struct message_slots *head = NULL;


struct message_channel *get_or_create_message_channel(int channel_id,struct message_channel **head );
struct message_slots *find_or_create_slot(int minor);
void clean_slot_message_channel(void);
void print_message_slots_info(void) {
    struct message_slots *current_slot = head; 
    struct message_channel *current_channel;
    printk(KERN_INFO "The address stored in 'head' is: %p\n", (void *)head);
    printk(KERN_INFO "Printing message slots info:\n");
    while (current_slot != NULL) {
        printk(KERN_INFO "Slot Minor: %d\n", current_slot->minor);

        current_channel = current_slot->channels;
        while (current_channel != NULL) {
            printk(KERN_INFO "\tChannel ID: %d\n message : %s", current_channel->channel_id,current_channel->message);
            current_channel = current_channel->next; 
        }

        current_slot = current_slot->next; 
    }
    printk( "Done printing message slots info:\n");

}
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  int i;
  struct message_channel *channel ;
  channel = (struct message_channel *)file->private_data;
  printk("device_read start");
  print_message_slots_info();
  if (!channel) {
      printk(KERN_ERR  "No channel set yet.\n");
      return -EINVAL;
  }
  printk(KERN_INFO "device_read: Reading from channel ID %d, buffer size %zu\n", channel->channel_id, length);
  if (strlen(channel->message) == 0) {
      printk(KERN_ERR "No message in the channel");
      return -EWOULDBLOCK;
  }
  if (length < strlen(channel->message)) {
      printk(KERN_ERR "Buffer too small");
      return -ENOSPC;
  }
    for (i = 0; i < length && channel->message[i] != '\0'; ++i) {
        if (put_user(channel->message[i], &buffer[i])) {
            printk(KERN_ERR "Failed to copy message to user.\n");
            return -EFAULT; // Error copying to user space
        }
    }
  printk(KERN_INFO "read message to channel ID: %d\n", channel->channel_id);
  return channel->length;
}
static int device_open( struct inode* inode,
                        struct file*  file )
{
  
  int minor;
  struct message_slots *slot;
  printk("device_open");
  minor = iminor(file->f_inode);
  slot = find_or_create_slot(minor);
  return SUCCESS;
}
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  
  struct message_channel *channel;
  struct message_slots *slot;
  int minor;
  printk("device_ioctl start");
  if (ioctl_command_id != MSG_SLOT_CHANNEL  ) {
    printk(KERN_WARNING "ioctl command not valid.\n");
    return -EINVAL;
  }

  if (ioctl_param == 0) {
    printk(KERN_WARNING "Channel ID is 0 .\n");
    return -EINVAL;
  }
  minor = iminor(file->f_inode);
  slot = find_or_create_slot(minor);
  channel = get_or_create_message_channel(ioctl_param,&(slot->channels));
  if (!channel) 
    return -1;
  file->private_data = channel;
  printk("device_ioctl end");
  return SUCCESS;
}
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  ssize_t i;
  struct message_channel *channel = (struct message_channel *)file->private_data;
  printk("device_write start");
  if (!channel) {
      printk(KERN_ERR "No channel set yet.\n");
      return -EINVAL;
  }
  printk(KERN_INFO "device_write: Writing to channel ID %d, length %zu\n", channel->channel_id, length);
  if (length == 0 || length > BUFFER_SIZE) {
    printk(KERN_ERR "Invalid message length.\n");
    return  -EMSGSIZE;
  }
  // empty last the message
  memset(channel->message, 0, sizeof(channel->message));
  for(i = 0; i < length && i < BUFFER_SIZE; ++i ) {
    get_user(channel->message[i], &buffer[i]);
  }
  channel->length = i;
  printk(KERN_INFO "Writing message to channel ID: %d\n, mesagge : %s", channel->channel_id,channel->message);
  return i;
 }

struct file_operations Fops = {
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
};
static int __init simple_init(void) {
    int rc = register_chrdev(MAJOR_NUM, DEVICE_FILE_NAME, &Fops);
    if (rc < 0) {
        printk(KERN_ALERT "%s registration failed for %d\n", DEVICE_FILE_NAME, MAJOR_NUM);
        return rc;
    }

    printk(KERN_INFO "Registration is successful.\n");
    printk(KERN_INFO "If you want to talk to the device driver,\n");
    printk(KERN_INFO "you'll need to create a device file:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
    printk(KERN_INFO "Don't forget to rm the device file and rmmod when you're done.\n");

    return 0;
}

static void __exit simple_cleanup(void) {
    clean_slot_message_channel();
    unregister_chrdev(MAJOR_NUM, DEVICE_FILE_NAME);
    printk(KERN_INFO "Message slots module unloaded.\n");
}

void clean_message_channel(struct message_channel *curr){
  while (curr != NULL){
    struct message_channel *next = curr->next; 
    kfree(curr);
    curr = next; 
  }
}
void clean_slot_message_channel(void){
  struct message_slots *next;
  struct message_slots *curr = head;
  printk(KERN_INFO "start clean_slot_message_channel.\n");
  while (curr != NULL){
    clean_message_channel(curr->channels);
    next = curr->next; 
    kfree(curr);
    curr = next; 
  }
  printk(KERN_INFO "End clean_slot_message_channel.\n");
}

struct message_channel *get_or_create_message_channel(int channel_id, struct message_channel **head) {
    struct message_channel *curr = *head;
    struct message_channel *prev = NULL;
    struct message_channel *new_channel;
    printk(KERN_INFO "start get_or_create_message_channel.\n");

    while (curr) {
        if (curr->channel_id == channel_id) {
            printk(KERN_INFO "get_or_create_message_channel return message channel %d\n", channel_id);
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }

    printk(KERN_INFO "get_or_create_message_channel create new message channel %d\n", channel_id);
    new_channel = kmalloc(sizeof(struct message_channel), GFP_KERNEL);
    if (!new_channel) {
        printk(KERN_ERR "kmalloc new channel failed.\n");
        return NULL;
    }
    memset(new_channel->message, 0, sizeof(new_channel->message));
    new_channel->channel_id = channel_id;
    new_channel->next = NULL; 

    if (prev) {
        prev->next = new_channel; 
    } else {
        *head = new_channel; 
    }

    printk(KERN_INFO "End get_or_create_message_channel.\n");
    return new_channel;
}
struct message_slots *find_or_create_slot(int minor) {
    struct message_slots *curr = head;
    struct message_slots *new;
    struct message_slots *prev = NULL;
    printk(KERN_INFO "message_slots start \n");
    while (curr) {
        if (curr->minor == minor) {
            printk(KERN_INFO "find_or_create_slot return message_slots %d\n",minor);
            return curr; 
        }
        prev = curr;
        curr = curr->next;
    }
    new = kmalloc(sizeof(struct message_slots), GFP_KERNEL);
    if (!new) {
        printk(KERN_ERR "kmalloc  new message slot Failed.\n");
        return NULL; 
    }
    memset(new, 0, sizeof(struct message_slots));
    new->minor = minor;
    new->next = NULL;

    if (prev == NULL) {
        head = new; 
    } else {
        prev->next = new; 
    }

    printk(KERN_INFO "message_slots created and returned new slot for minor %d\n", minor);
    return new;
}
MODULE_LICENSE("GPL");
module_init(simple_init);
module_exit(simple_cleanup);
//========================= END OF FILE =========================
