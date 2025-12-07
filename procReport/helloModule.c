#include<linux/module.h>
#include<linux/sched/signal.h>
#include<linux/pid_namespace.h>
#include<asm/io.h>
#include <linux/pgtable.h>

const int MIN_PID = 650;

int proc_count(void);

int proc_init (void) {
  // print out process report
    printk(KERN_INFO "PROCESS REPORT:\n");
    printk(KERN_INFO "proc_id,proc_name,contig_pages,noncontigu_pages,total_pages\n");
    proc_count();

    return 0;
}

void proc_cleanup(void) {
    printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

int proc_count(void) {
    // task_struct
    struct task_struct *thechild;
    // vm_area
    struct vm_area_struct *vma;
    // declarations 
    unsigned long vpage, physical_page_addr, total_pages, prev_page, contig, noncontig;
    // initializations of totals
    unsigned long total_pages_all = 0, total_contig = 0, total_noncontig = 0;

    // run it for each process
    for_each_process(thechild) { 
        // check if PID > 650
        if (thechild->pid > MIN_PID) {
            // initializations
            total_pages = prev_page = contig = noncontig = 0;
            // check for memory map and vm_area
            if (thechild->mm && thechild->mm->mmap) {
                // loop over virtual memory areas
                for (vma = thechild->mm->mmap; vma; vma = vma->vm_next) {
                    // loop over pages of each virtual memory area
                    for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
                        // get physical page address
                        physical_page_addr = virt_to_phys((void *) vpage);
                        // check if the physical page address isn't 0
                        if (physical_page_addr != 0) {
                            // increment
                            total_pages++;
                            // check if contiguous or noncontiguous
                            if (prev_page != 0 && physical_page_addr == prev_page + PAGE_SIZE) {
                                contig++;
                            } else {
                                noncontig++;
                            }
                            // set prev page to current
                            prev_page = physical_page_addr;
                        }
                    }
                }
            }

            // increment totals
            total_pages_all += total_pages;
            total_contig += contig;
            total_noncontig += noncontig; 

            // print out info
            printk(KERN_INFO "%d,%s,%lu,%lu,%lu\n", thechild->pid, thechild->comm, contig, noncontig, total_pages);
        }
    }

    // print out totals
    printk(KERN_INFO "TOTALS,,%lu,%lu,%lu\n", total_contig, total_noncontig, total_pages_all);

    return 0;
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
