#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRVNAME "ilnchrdev"

struct iln_char_device {
	dev_t dev;
	struct class *cl;
	struct cdev c_dev;
}iln_dev;

struct file_operations fops = {
	.owner = THIS_MODULE,
};

static int __init iln_chrdrv_init(void)
{
	if (alloc_chrdev_region(&iln_dev.dev, 0, 1, DRVNAME) < 0) {
		pr_err("%s : Major Number allocation Failed\n", DRVNAME);
		return -1;
	}

	if ((iln_dev.cl = class_create("ilnchrdevclass")) == NULL) {
		pr_err("%s : Class creation Failed\n", DRVNAME);
		goto out1;
	}

	if (device_create(iln_dev.cl, NULL, iln_dev.dev, NULL, DRVNAME) == NULL) {
		pr_err("%s : Device creation Failed\n", DRVNAME);
		goto out2;
	}

	cdev_init(&iln_dev.c_dev, &fops);
	if (cdev_add(&iln_dev.c_dev, iln_dev.dev, 1) == -1) {
		pr_err("%s : cdev creation Failed\n", DRVNAME);
		goto out3;
	}

	pr_info("%s : module registered successfully\n", DRVNAME);

	return 0;

out3:
	device_destroy(iln_dev.cl, iln_dev.dev);
out2:
	class_destroy(iln_dev.cl);
out1:
	unregister_chrdev_region(iln_dev.dev, 1);

	return -1;

}

static void __exit iln_chrdrv_exit(void)
{
	cdev_del(&iln_dev.c_dev);
	device_destroy(iln_dev.cl, iln_dev.dev);
	class_destroy(iln_dev.cl);
	unregister_chrdev_region(iln_dev.dev, 1);

	pr_info("%s : module unregistered successfully\n", DRVNAME);

}

module_init(iln_chrdrv_init);
module_exit(iln_chrdrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("iLearn");
MODULE_DESCRIPTION("Character driver demonstration");
