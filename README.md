# 📬 Message Slot Kernel Module

This repository contains a Linux kernel module that implements a **message slot** character device. Each device supports multiple communication **channels**, selected dynamically using an `ioctl()` command. Userspace programs can write and read messages on specific channels.

---

## 🧠 Project Overview

- 💡 **Kernel Module (`message_slot.c`)**: Implements device registration, IOCTL interface, and per-channel message storage.
- 🧰 **User Tools**:
  - `message_sender`: CLI tool to send messages to specific channels.
  - `message_reader`: CLI tool to read messages from specific channels.
- 🧾 **Makefile**: Builds kernel module and compiles user-space tools.

---

## ⚙️ Features

- 🔧 Supports dynamic channel selection using IOCTL.
- 🗂️ Multiple independent message slots based on minor device numbers.
- 🛡️ Proper memory management and safety checks for user-space interaction.

---

## 📂 File Structure

```
message-slot-kernel/
├── message_slot.c       # Kernel module implementation
├── message_slot.h       # Common constants and IOCTL definitions
├── message_sender.c     # User-space message writer
├── message_reader.c     # User-space message reader
├── Makefile             # Build script for kernel and tools
```

---

## 🏁 Getting Started

### 1. Build Kernel Module and Tools

```bash
make
```

### 2. Load the Module

```bash
sudo insmod message_slot.ko
```

### 3. Create a Device File

```bash
sudo mknod /dev/message_slot c 235 0
sudo chmod 666 /dev/message_slot
```

---

## 📨 Usage

### Write a Message

```bash
./message_sender /dev/message_slot 1 "Hello, kernel!"
```

### Read a Message

```bash
./message_reader /dev/message_slot 1
```

- The number `1` is the **channel ID**.

---

## 🧹 Cleanup

```bash
sudo rmmod message_slot
make clean
sudo rm /dev/message_slot
```

---

## 🪪 License

This project is licensed under the **GPL License** as it interfaces with the Linux kernel.

---

## 👨‍💻 Author

Created by [mati1mati1](https://github.com/mati1mati1) 