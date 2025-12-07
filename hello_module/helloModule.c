#include<linux/module.h>
#include<linux/sched/signal.h>
#include<linux/pid_namespace.h>
#include<asm/io.h>
#include <linux/pgtable.h>

const int MIN_PID = 650;

int proc_count(void);

int proc_init (void) {
  printk(KERN_INFO "PROCESS REPORT:\n");
  printk(KERN_INFO "proc_id,proc_name,total_pages\n");
  proc_count();

  return 0;
}

void proc_cleanup(void) {
  printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

int proc_count(void) {
    struct task_struct *thechild;
    struct vm_area_struct *vma;
    unsigned long vpage, physical_page_addr, total_pages, prev_page, contig, noncontig;
    unsigned long total_pages_all = 0, total_contig = 0, total_noncontig = 0;

    for_each_process(thechild) { 
        if (thechild->pid > MIN_PID) {
            total_pages = prev_page = contig = noncontig = 0;
            if (thechild->mm && thechild->mm->mmap) {
                for (vma = thechild->mm->mmap; vma; vma = vma->vm_next) {
                    for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
                        physical_page_addr = virt_to_phys((void *) vpage);
                        if (physical_page_addr != 0) {
                            total_pages++;
                            if (prev_page != 0 && physical_page_addr == prev_page + PAGE_SIZE) {
                                contig++;
                            } else {
                                noncontig++;
                            }
                            prev_page = physical_page_addr;
                        }
                    }
                }
            }

            total_pages_all += total_pages;
            total_contig += contig;
            total_noncontig += noncontig; 

            printk(KERN_INFO "%d,%s,%lu,%lu,%lu\n", thechild->pid, thechild->comm, contig, noncontig, total_pages);
        }

        printk(KERN_INFO "TOTALS,,%lu,%lu,%lu\n", total_pages_all, total_contig, total_noncontig);
    }

    return 0;
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
