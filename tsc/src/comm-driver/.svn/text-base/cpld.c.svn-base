/**
 * cpld.c
 * 
 * Linux drivers for PC104 to communicate with CPLD
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-06-01 10:57:37 中国标准时间
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
#include <linux/proc_fs.h>
#include <linux/interrupt.h>	// for tasklet
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>

#include "cpld.h"
#include "cpld_fifo.h"

static int debug = 0;
MODULE_PARM(debug,"i");
MODULE_PARM_DESC(debug,"debug switch");

// CPLD_MAJOR = 10 , misc char device
#define CPLD_MINOR 240	// 240 -255 is for local use
static unsigned char CPLD_PORT1_WRITABLE_MASK = 0x10;
static unsigned char CPLD_PORT0_WRITABLE_MASK = 0x08;
static unsigned char CPLD_PORT1_READABLE_MASK = 0x04;
static unsigned char CPLD_PORT0_READABLE_MASK = 0x02;
static unsigned char CPLD_INTERRUPT_PENDING_MASK = 0x01;

// 写地址
static int cpld_write_port_addr0 = 0x220;
static int cpld_write_port_addr1 = 0x221;

// 读地址
static int cpld_read_port_addr0 = 0x222;
static int cpld_read_port_addr1 = 0x223;

// 状态地址
static int cpld_status_port_addr = 0x224;
#define PORT_RANGE 5

// 中断号
static int cpld_irq =  6;


// cpld 设备实例的私有数据结构
typedef struct {
	struct fasync_struct* async_queue;
	int is_need_notify_reader;	// 需要给读进程发送SIGIO信号
	wait_queue_head_t inq;
	wait_queue_head_t outq;

	cpld_fifo_t in_queue;
	spinlock_t in_spinlock;
	cpld_fifo_t out_queue;
	spinlock_t out_spinlock;
	
	u32 tx_count;
	u32 rx_count;
	u16 last_tx_count;


	int read_port_addr;
	int write_port_addr;
	int status_port_addr;
	int readable_mask;
	int writable_mask;

	/* tasklet */
	struct tasklet_struct cpld_tasklet;
} cpld_dev_t;

static cpld_dev_t cpld_dev[2];

/**
 * 测试状态口，看是否有数据可读
 */
static int cpld_dev_can_read(cpld_dev_t* self)
{
	unsigned char status = inb(self->status_port_addr);
	return status & self->readable_mask;
}

/**
 * 测试状态口，看是否可以写数据
 */
static int cpld_dev_can_write(cpld_dev_t* self)
{
	unsigned char status = inb(self->status_port_addr);
	return status & self->writable_mask;
}

/**
 * 测试看现在是否有中断未处理
 */
static int cpld_dev_interrupt_pending(cpld_dev_t* self)
{
	unsigned char status = inb(self->status_port_addr);
	return status & CPLD_INTERRUPT_PENDING_MASK;
}

/**
 * 读取一个字节
 */
static unsigned char cpld_dev_read_char(cpld_dev_t* self)
{
	unsigned char ch = inb(self->read_port_addr);
	return ch;
}

/**
 * 写一个字节
 */
static void cpld_dev_write_char(cpld_dev_t* self,unsigned char ch)
{
	if(debug){
		printk(KERN_ERR "cpld_write_char: %d\n",ch);
	}
	outb(ch,self->write_port_addr);
}

static void cpld_tasklet_fn(unsigned long data)
{
	unsigned long flags;
	cpld_dev_t* dev;
	int count;
	count = 0;
	dev = (cpld_dev_t*)data;
	// transfer data in outqueue
	spin_lock_irqsave(&dev->out_spinlock,flags);
	while(!cpld_fifo_is_empty(&dev->out_queue)){

		if(cpld_dev_can_write(dev)){
			cpld_dev_write_char(dev,cpld_fifo_getchar(&dev->out_queue));
			dev->tx_count++;
			count++;
		}
		else{
			break;
		}

	}
	dev->last_tx_count = count;
	if(!cpld_fifo_is_empty(&dev->out_queue))
	// reschedule ourself
		tasklet_schedule(&dev->cpld_tasklet);
	spin_unlock_irqrestore(&dev->out_spinlock,flags);
}
static void cpld_dev_init(cpld_dev_t* self)
{
	self->async_queue = NULL;
	self->is_need_notify_reader = 1;
	init_waitqueue_head(&self->inq);
	init_waitqueue_head(&self->outq);

	cpld_fifo_init(&self->in_queue);
	spin_lock_init(&self->in_spinlock);
	cpld_fifo_init(&self->out_queue);
	spin_lock_init(&self->out_spinlock);


	self->rx_count = 0;
	self->tx_count = 0;

	tasklet_init(&self->cpld_tasklet,cpld_tasklet_fn,(unsigned long)self);
}


static ssize_t cpld_read(struct file* file, char* buf,
				size_t count, loff_t* pos)
{
	cpld_dev_t* dev;
#define IN_BUFFER_LEN 1024

	unsigned char tmp_buf[IN_BUFFER_LEN];
	unsigned long len,read_size;
	unsigned long flags;

	dev = (cpld_dev_t*)file->private_data;
	read_size = count > IN_BUFFER_LEN ? IN_BUFFER_LEN : count;

	if(debug){
		printk(KERN_ERR "cpld_read: %d\n",count);
	}
	// 把队列里面的数据返回给进程
	len=0;

	// 上锁
	spin_lock_irqsave(&dev->in_spinlock,flags);
	len = cpld_fifo_read(&dev->in_queue,tmp_buf,read_size);
	dev->is_need_notify_reader = 0;	// 不用异步通知进程
	// 开锁
	spin_unlock_irqrestore(&dev->spin_lock,flags);

	if(copy_to_user(buf,tmp_buf,len) == 0)
		return len;
	else
		return -EFAULT;
}
static ssize_t cpld_write(struct file* file, const char* buf,
				size_t count, loff_t* pos)
{
	unsigned long flags;
	cpld_dev_t* dev;
	size_t len,done;
	unsigned char buffer[1024];

	dev = (cpld_dev_t*)file->private_data;
	len = 1024<count?1024:count;
	if(copy_from_user(buffer,buf,len) != 0)
		return -EFAULT;
	if(debug){
		printk(KERN_ERR "cpld_write: %d\n",count);
	}
	done=0;
#if 1
	// put to outqueue and use tasklet to tansfer 
	spin_lock_irqsave(&dev->out_spinlock,flags);
	done = cpld_fifo_write(&dev->out_queue,buffer,len);
	// schedule tasklet to run 
	if(!cpld_fifo_is_empty(&dev->out_queue))
		tasklet_schedule(&dev->cpld_tasklet);
	spin_unlock_irqrestore(&dev->out_spinlock,flags);

#else
	// transfer directly
	while(done < len)
	{
		if(cpld_dev_can_write(dev))
		{
			cpld_dev_write_char(dev,buffer[done]);
			dev->tx_count++;
			done++;
		}
		else
		{
			break;
		}
	}
#endif
	if(debug){
		printk(KERN_ERR "cpld_write: %d bytes written.\n",done);
	}
	return done;
}
static int cpld_fasync(int fd,struct file* file,int mode)
{
	cpld_dev_t* dev = (cpld_dev_t*)file->private_data;
	if(debug){
		printk(KERN_ERR "cpld_fasync: fd=%d mode=%d\n",fd,mode);
	}
	return fasync_helper(fd,file,mode,&dev->async_queue);
}

static int cpld_open(struct inode *inode, struct file *file)
{
	int i  = 0;
	int fd = -1;
	if(debug){
		printk(KERN_ERR "cpld_open: %s\n", file->f_dentry->d_iname);
	}
	MOD_INC_USE_COUNT;
	// 根据用户打开的设备文件名决定使用哪个端口
	if(strstr(file->f_dentry->d_iname,"cpld1") != NULL)
		file->private_data = &cpld_dev[1];
	else
		file->private_data = &cpld_dev[0];


	if(file->f_flags & FASYNC)
	{
		// 查找文件的描述符
		for(i=0; i<current->files->max_fds; i++)
			if(file == current->files->fd[i])
			{
				fd = i;
				break;
			}
	
		cpld_fasync(fd,file,1);
	}
	return 0;
}
static int cpld_ioctl(struct inode * inode, struct file * file, unsigned int cmd, unsigned long arg)
{
	if(cmd != CPLD_IOC_PORT_SELECT)
		return -ENOTTY;

	if(arg == 0)
		file->private_data = &cpld_dev[0];
	else
		file->private_data = &cpld_dev[1];
	if(debug){
		printk(KERN_ERR "cpld_ioctl: port select %ld\n",arg);
	}
	return 0;
}
static int cpld_release(struct inode *inode, struct file *file)
{
	cpld_dev_t* dev;
	if(debug){
		printk(KERN_ERR "cpld_release\n");
	}
	dev = (cpld_dev_t*)file->private_data;
	tasklet_kill(&dev->cpld_tasklet);

	MOD_DEC_USE_COUNT;
	file->private_data = NULL;

	if(file->f_flags & FASYNC)
		cpld_fasync(-1,file,0);
	return 0;
}

static void cpld_interrupt(int irq, void* dev_id, struct pt_regs* regs)
{
	unsigned char ch;
	int port;
	cpld_dev_t* dev;
	long flags;

	if(debug){
		printk(KERN_ERR "cpld_interrupt: %d\n",irq);	
	}
	for(port = 1; port >= 0; port--){
	
		// 读取状态
		if(!cpld_dev_can_read(&cpld_dev[port]))
			continue ;

		dev = &cpld_dev[port];

		// 上锁
		spin_lock_irqsave(&dev->in_spinlock,flags);

		if(cpld_fifo_is_will_full(&dev->in_queue)){
			// 需要通知异步读进程
			dev->is_need_notify_reader = 1;
		}

		// 必要的话,通知异步读进程
		if(dev->is_need_notify_reader)
		{
			if(debug){
				printk(KERN_ERR "cpld_interrupt: notify async reader.\n");
			}
			kill_fasync(&dev->async_queue,SIGIO,POLL_IN);
		}

		// 读取一个字节,放到队列里面
		ch = cpld_dev_read_char(dev);
		dev->rx_count++;

		if(!cpld_fifo_is_full(&dev->in_queue)){
			cpld_fifo_putchar(&dev->in_queue,ch);
		}

		spin_unlock_irqrestore(&dev->in_spinlock,flags);

		dev->is_need_notify_reader = 1;

		if(debug){
			printk(KERN_CRIT "cpld_interrupt: read %d\n",ch);
		}
	}
	
}
static unsigned int cpld_poll(struct file* file, poll_table* wait)
{
	cpld_dev_t* data;
	unsigned int mask = 0;
	unsigned long flags = 0;
	if(debug){
		printk(KERN_CRIT "cpld_poll\n");
	}
	data = (cpld_dev_t*)file->private_data;

	poll_wait(file,&data->inq,wait);
	poll_wait(file,&data->outq,wait);

	spin_lock_irqsave(&data->in_spinlock,flags);
	if(!cpld_fifo_is_empty(&data->in_queue))
		mask |= POLLIN | POLLRDNORM;
	spin_unlock_irqrestore(&data->in_spinlock,flags);

	if(!cpld_fifo_is_full(&data->out_queue))
		mask |= POLLOUT | POLLWRNORM; // always writable
	return mask;
}
static struct file_operations cpld_fops = {
        owner:          THIS_MODULE,
        llseek:         no_llseek,
        read:           cpld_read,
	write:		cpld_write,
        open:           cpld_open,
	ioctl:		cpld_ioctl,
	fasync:		cpld_fasync,
        release:        cpld_release,
	poll:		cpld_poll,
};

static struct miscdevice cpld_device =
{
	CPLD_MINOR,
	"cpld",
	&cpld_fops,
};

/**
 * proc read operation
 */
static int cpld_read_proc(char *page, char **start, off_t off, int count,
			int *eof, void *data)
{
	int len;
	int i;
	unsigned char ch;
	len = 0;
	for(i=0; i<2; i++){
		len += sprintf(page+len,"/dev/cpld%d: rx_count: %d  tx_count: %d  last_tx_count: %d\n",
			       i,cpld_dev[i].rx_count,cpld_dev[i].tx_count,cpld_dev[i].last_tx_count);
		len += sprintf(page+len,"/dev/cpld%d: in_queue:   read_p = %04d write_p = %04d\n",
				i,cpld_dev[i].in_queue.m_read_p,cpld_dev[i].in_queue.m_write_p);
		len += sprintf(page+len,"/dev/cpld%d: out_queue:  read_p = %04d write_p = %04d\n",
				i,cpld_dev[i].out_queue.m_read_p,cpld_dev[i].out_queue.m_write_p);
	}
	ch = inb(cpld_status_port_addr);
	len += sprintf(page+len,"/dev/cpld : interrupt pending: %d\n",
				ch & CPLD_INTERRUPT_PENDING_MASK ? 1:0);
	len += sprintf(page+len,"/dev/cpld0: readable: %d writable: %d\n",
				ch & CPLD_PORT0_READABLE_MASK ? 1:0,
				ch & CPLD_PORT0_WRITABLE_MASK ? 1:0);
	len += sprintf(page+len,"/dev/cpld1: readable: %d writable: %d\n",
				ch & CPLD_PORT1_READABLE_MASK ? 1:0,
				ch & CPLD_PORT1_WRITABLE_MASK ? 1:0);
	*eof = 1;
	return len;
}

static int cpld_init()
{
	int err;
	if(debug){
		printk(KERN_ERR "cpld_init\n");
	}
	cpld_dev_init(&cpld_dev[0]);
	cpld_dev[0].read_port_addr   = cpld_read_port_addr0;
	cpld_dev[0].write_port_addr  = cpld_write_port_addr0;
	cpld_dev[0].status_port_addr = cpld_status_port_addr;
	cpld_dev[0].readable_mask    = CPLD_PORT0_READABLE_MASK;
	cpld_dev[0].writable_mask    = CPLD_PORT0_WRITABLE_MASK;

	cpld_dev_init(&cpld_dev[1]);
	cpld_dev[1].read_port_addr   = cpld_read_port_addr1;
	cpld_dev[1].write_port_addr  = cpld_write_port_addr1;
	cpld_dev[1].status_port_addr = cpld_status_port_addr;
	cpld_dev[1].readable_mask    = CPLD_PORT1_READABLE_MASK;
	cpld_dev[1].writable_mask    = CPLD_PORT1_WRITABLE_MASK;

	inb(cpld_read_port_addr0);
	inb(cpld_read_port_addr1);

	// 注册驱动程序
	err =  misc_register(&cpld_device);
	if(err < 0)
		return err;
	err = check_region(cpld_write_port_addr0,PORT_RANGE);
	if(err < 0)
		return err;
	request_region(cpld_write_port_addr0,PORT_RANGE,"cpld");

	// 注册中断处理程序
	request_irq(cpld_irq,cpld_interrupt,SA_INTERRUPT,"cpld",NULL);

	if(create_proc_read_entry("cpld",S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH|S_IFREG,
				NULL, cpld_read_proc,NULL) == NULL){
		printk(KERN_ERR "can not create proc entry /proc/cpld\n");
	};

	// 测试看是否有未处理的中断
	if(cpld_dev_interrupt_pending(&cpld_dev[0]))
	{
		if(cpld_dev_can_read(&cpld_dev[0])){
					cpld_dev_read_char(&cpld_dev[0]);
		}

		if(cpld_dev_can_read(&cpld_dev[1])){
					cpld_dev_read_char(&cpld_dev[0]);
		}
	}

	return 0;
}
static void cpld_fini()
{
	if(debug){
		printk(KERN_ERR "cpld_fini\n");
	}
	remove_proc_entry("cpld",NULL);
	// 取消中断处理程序
	free_irq(cpld_irq,NULL);
	// 释放IO端口
	release_region(cpld_write_port_addr0,PORT_RANGE);
	// 取消驱动程序
	misc_deregister(&cpld_device);
}
#ifdef MODULE
int init_module(void)
{
	return cpld_init();
}
void cleanup_module(void)
{
	cpld_fini();
}
#endif
