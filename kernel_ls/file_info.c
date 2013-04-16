#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");

static char *pathname = "no path!";
module_param(pathname, charp, 0644);
MODULE_PARM_DESC(pathname, "file pathname\n");

static int __init lkp_init( void )
{
	mm_segment_t old_fs;
	struct file 	*file_open;
	 file_open = filp_open(pathname, O_RDWR | O_APPEND | O_CREAT, 0644);
	if (IS_ERR(file_open)) {
		file_open = filp_open(pathname, O_DIRECTORY, 0644);
		if (IS_ERR(file_open)) {
			printk("invalid pathname!\n");
			return -1;
		} 
		printk("it's directory!\n");
		printk("count: %d\n", file_open->f_dentry->d_count);
		printk("flags: %d\n", file_open->f_dentry->d_flags);
		return 0;
	}
	else {
		printk("it's file!\n");
		printk("inode: %ld\n", file_open->f_dentry->d_inode->i_ino);
		printk("uid: %ld\n", file_open->f_dentry->d_inode->i_uid);
	}

//	old_fs = get_fs();
//	set_fs(KERNEL_DS);

//	set_fs(old_fs);

	return 0;
}

static int __exit lkp_cleanup( void )
{
	return 0;
}

module_init( lkp_init );
module_exit( lkp_cleanup );
