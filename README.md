# Linux Character Device Driver

## Description
This project implements a Linux character device driver with the following features:

- Verifies kernel version before loading the driver
- Supports read and write operations
- Ensures the read operation is completed before write
- Stores the username provided as input
- Enforces completion of operations within a specified time limit

---

## Steps to Run the Code

### 1. Compile the code
```bash
make
```

### 2. Insert the module
```bash
sudo insmod mymodule.ko kernel_version=<major_no>,<minor_no> time_limit=<time_limit_in_sec>
```

### 3. Check logs to obtain the major number
```bash
sudo dmesg | tail
```

### 4. Create the device file
```bash
sudo mknod /dev/mychardev c <major_no> 0
```

### 5. Perform read operation
```bash
cat /dev/mychardev
```

### 6. Perform write operation
```bash
echo "<username>" | sudo tee /dev/mychardev
```

### 7. Remove the module
```bash
sudo rmmod mymodule
```

### 8. View output logs
```bash
sudo dmesg
```

---

## Notes

- The read operation must be completed before the write operation.
- All operations must finish within the specified time limit.
