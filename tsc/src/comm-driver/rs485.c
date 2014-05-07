/**
 * rs485.c
 * 
 * linux driver for RS-485
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-10 11:03:39 中国标准时间
 * 
 * $log$
 *
 */
#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/sysctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>

// use as a misc device

#define TX 0
#define RX 0
#define DLL 0
#define DLH 1
#define FCR 2
#define LCR 3
#define LSR 5
#define MCR 4
#define MSR 6
 
typedef struct {
	int base_addr;
	int irq;

} rs_485_dev_t;

void rs_485_dev_init(rs_485_dev_t* self,int base_addr,int irq)
{
	self->base_addr = base_addr;
	self->irq = irq;
}
int rs_485_dev_send(rs_485_dev_t* self,unsigned char ch)
{

	return 0;
}
int rs_485_dev_recv(rs_485_dev_t* self,unsigned char ch)
{
	return 0;
}
int rs_485_dev_get_status(rs_485_dev_t* self)
{
	return 0;
}
int rs_485_dev_set_baud()
{
	return 0;
}

// driver read
static ssize_t rs485_read(struct file* file, char* buf,
				size_t count, loff_t* pos)
{
	return 0;
}
static ssize_t rs485_write(struct file* file, const char* buf,
				size_t count, loff_t* pos)
{
	return 0;
}
static int rs485_open(struct inode *inode, struct file *file)
{

	return 0;
}
static int rs485_release(struct inode *inode, struct file *file)
{
	return 0;
}
static int rs485_ioctl(struct inode * inode, struct file * file, unsigned int cmd, unsigned long arg)
{

	return 0;
}
static int rs485_fasync(int fd,struct file* file,int mode)
{
	return 0;
}
static unsigned int rs485_poll(struct file* file, poll_table* wait)
{
	return 0;
}
static void rs485_interrupt(int irq, void* dev_id, struct pt_regs* regs)
{

#ifndef NDEBUG
	printk(KERN_ERR "rs485_interrupt %d \n",irq);
#endif

	
}
static struct file_operations rs485_fops = {
        owner:          THIS_MODULE,
        llseek:         no_llseek,
        read:           rs485_read,
	write:		rs485_write,
        open:           rs485_open,
	ioctl:		rs485_ioctl,
	fasync:		rs485_fasync,
        release:        rs485_release,
	poll:		rs485_poll,
};

#define RS485_MINOR 241
static struct miscdevice rs485_device =
{
	RS485_MINOR,
	"rs485",
	&rs485_fops,
};

static int rs485_init()
{
	int err;
#ifndef NDEBUG
	printk(KERN_ERR "rs485_init\n");
#endif

	rs485_dev_init(&rs485_dev[0]);
	rs485_dev[0].read_port_addr   = rs485_read_port_addr0;
	rs485_dev[0].write_port_addr  = rs485_write_port_addr0;
	rs485_dev[0].status_port_addr = rs485_status_port_addr;
	rs485_dev[0].readable_mask    = RS485_PORT0_READABLE_MASK;
	rs485_dev[0].writable_mask    = RS485_PORT0_WRITABLE_MASK;

	rs485_dev_init(&rs485_dev[1]);
	rs485_dev[1].read_port_addr   = rs485_read_port_addr1;
	rs485_dev[1].write_port_addr  = rs485_write_port_addr1;
	rs485_dev[1].status_port_addr = rs485_status_port_addr;
	rs485_dev[1].readable_mask    = RS485_PORT1_READABLE_MASK;
	rs485_dev[1].writable_mask    = RS485_PORT1_WRITABLE_MASK;

	inb(rs485_read_port_addr0);
	inb(rs485_read_port_addr1);

	// 注册驱动程序
	err =  misc_register(&rs485_device);
	if(err < 0)
		return err;
	err = check_region(rs485_write_port_addr0,PORT_RANGE);
	if(err < 0)
		return err;
	request_region(rs485_write_port_addr0,PORT_RANGE,"rs485");

	// 注册中断处理程序
	request_irq(rs485_irq,rs485_interrupt,SA_INTERRUPT,"rs485",NULL);

	return 0;
}
#ifdef MODULE
int init_module(void)
{
	return rs485_init();
}
void cleanup_module(void)
{
	rs485_fini();
}
#endif
