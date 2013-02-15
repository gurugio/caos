#ifndef __CHAR_DEVICE_H__
#define __CHAR_DEVICE_H__




typedef struct char_device_struct {
	//struct char_device_struct *next;
	unsigned int major;
	//unsigned int minor;
	char *name;
	struct file_operations *fops;
} char_device_struct_t;



typedef struct file_operations {
	int (*read)(char *buf, int size, int offset);
	int (*write)(const char *buf, int size, int offset);
	int (*open)(const char *name);
	int (*close)(const char *name);
	int (*ioctl)(unsigned int cmd);
} file_operations_t;


int register_chrdev(char *name, char_device_struct_t *cdev);
int unregister_chrdev(int major_num, char *);
void init_char_dev(void);


#endif
