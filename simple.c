#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <asm/vmx.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Simple module");

struct vmcs{
	uint32_t revision_id;
	uint32_t abort;
	char data[0];
};

static char *buffer = NULL;
static char *buffer_area = NULL;
static const struct file_operations simple_fops;
static struct miscdevice simple_miscdev;

static unsigned int vmcs_writel(unsigned long field, unsigned long value)
{
	uint8_t error;

	// vmwrite rax,rdx
	asm volatile(
		".byte 0x0f, 0x79, 0xd0\n\t"
		"setna %0"
		: "=q"(error) 
		: "d"(field), "a"(value)
		: "cc"
	);

	if (error){
		printk("vmwrite error\n");
		return error;
	}
	return 0;
}

static unsigned long vmcs_readl(unsigned long field)
{
	unsigned long value;

	// vmread rax,rdx
	asm volatile(
		".byte 0x0f, 0x78, 0xd0\n\t"
		:"=a"(value): "d"(field): "cc"
	);
	
	return value;
}

//内核IOCTL接口
static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	uint32_t revision;
	
	revision = *((uint32_t *)buffer_area);
	switch (cmd) {
	case 0x11: 
		put_user(vmcs_readl(HOST_RIP), (uint *)arg);
		break;
	case 0x12:
		put_user(vmcs_readl(EXCEPTION_BITMAP),(uint *)arg);
		break;
	case 0x13:
		put_user(vmcs_writel(EXCEPTION_BITMAP,0x00ff00ff),(uint *)arg);
		break;
	case 0x14:
		put_user(vmcs_readl(PIN_BASED_VM_EXEC_CONTROL),(uint *)arg);
		break;
	case 0x15:
		put_user(vmcs_readl(CPU_BASED_VM_EXEC_CONTROL),(uint *)arg);
		break;
	case 0x16:
		put_user(vmcs_readl(SECONDARY_VM_EXEC_CONTROL),(uint *)arg);
		break;
	case 0x17:
		put_user(vmcs_readl(CR0_GUEST_HOST_MASK),(uint *)arg);
		break;
	case 0x18:
		put_user(vmcs_readl(CR0_READ_SHADOW),(uint *)arg);
		break;
	default:
		return -EFAULT;
		break;
	}
	return 0;
}

static int simple_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;
	uint64_t phys_addr;
	
	phys_addr = __pa(buffer_area);

	asm volatile (
		"vmptrst %0"
		:"=m"(phys_addr)
		::
	);

	printk("vmcs.revision: %x\n", ((struct vmcs *)buffer_area)->revision_id);

	// 映射到用户态
	ret = remap_pfn_range(vma, 
			vma->vm_start,
		    phys_addr >> PAGE_SHIFT,
		    vma->vm_end - vma->vm_start, 
			vma->vm_page_prot);

	if (ret != 0) {
		printk("%s: remap_pfn_range failed at [0x%lx  0x%lx]\n", 
				__func__, vma->vm_start, vma->vm_end);
		return -EAGAIN;
	}

	return 0;
}

static int simple_init(void)
{
	int err;

	err = misc_register(&simple_miscdev);
	if (err < 0) {
		pr_err("Cannot register simple device\n");
		return err;
	}

	// 创建传递给用户态的内存
	buffer = kmalloc(4096, GFP_KERNEL);
	buffer_area = (char *)(((unsigned long)buffer + PAGE_SIZE - 1) & PAGE_MASK);
	memset(buffer, 0, 4096);

	return 0;
}

static void simple_exit(void)
{
	misc_deregister(&simple_miscdev);
	pr_info("exit\n");
}

static const struct file_operations simple_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = simple_ioctl,
	.mmap = simple_mmap,
	.llseek = noop_llseek
};

static struct miscdevice simple_miscdev = {
	MISC_DYNAMIC_MINOR, 
	"simple",
	&simple_fops
};

module_init(simple_init);
module_exit(simple_exit);
