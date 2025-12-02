#include<linux/module.h>
#include<linux/pid_namespace.h>
#include<asm/io.h>


const MIN_PID = 650;

proc_count(void);

int proc_init (void) {
  printk(KERN_INFO "helloModule: kernel module initialized\n");
  return 0;
}

void proc_cleanup(void) {
  printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

int proc_count(void) {
    int i = 0;
    struct task_struct *thechild;
    for_each_process(thechild) { 
        if (thechild->pid > MID_PID) {
            printk(KERN_INFO "%d\n", thechild->pid);
            i++;
        }
    }
    return i;
}
