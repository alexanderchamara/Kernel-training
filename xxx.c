// Comments
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/err.h>


#define MODULE_TAG      "xxx_module: "

#define LEN_MSG 160
static char *buf_msg;

static int create_buffer(void)
{
	buf_msg = kmalloc(LEN_MSG, GFP_KERNEL);
    if (NULL == buf_msg)
        return -ENOMEM;
    return 0;
}


static void cleanup_buffer(void)
{
    if (buf_msg) {
        kfree(buf_msg);
        buf_msg = NULL;
    }
}

/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t xxx_show( struct class *class, struct class_attribute *attr, char *buf ) {
   strcpy( buf, buf_msg );
   printk( MODULE_TAG "read %ld\n", (long)strlen( buf ) );
   return strlen( buf );
}

/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t xxx_store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {
   printk( MODULE_TAG "write %ld\n", (long)count );
   strncpy( buf_msg, buf, count );
   buf_msg[ count ] = '\0';
   return count;
}

CLASS_ATTR_RW( xxx );

static struct class *x_class;

int __init x_init(void) {
   int err;

   x_class = class_create( THIS_MODULE, "x-class" );
   if( IS_ERR( x_class ) ){
	   err = PTR_ERR( x_class );
	   printk( MODULE_TAG "Error during file creation\n");
	   goto class_create_err;
   }

   err = class_create_file( x_class, &class_attr_xxx );
   if( err ){
	   printk( MODULE_TAG "Error during class creation\n");
	   goto file_create_err;
   }

    err = create_buffer();
	if( err ){
		printk( MODULE_TAG "Memory allocation error\n" );
		goto mem_aloc_err;
	}

   printk( MODULE_TAG "Module initialized successfully\n" );
   return 0;

mem_aloc_err:
	// do nothing here

file_create_err:
	class_remove_file( x_class, &class_attr_xxx );

class_create_err:
	class_destroy( x_class );
    return err;
}

void x_cleanup(void) {
	cleanup_buffer();
	class_remove_file( x_class, &class_attr_xxx );
	class_destroy( x_class );
	return;
}

module_init( x_init );
module_exit( x_cleanup );
MODULE_LICENSE( "GPL" );
