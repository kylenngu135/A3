#include<linux/module.h>
#include<linux/sched/signal.h>
#include<linux/pid_namespace.h>
#include<asm/io.h>
#include <linux/pgtable.h>

const int MIN_PID = 650;

int proc_count(void);

unsigned long virt2phys(struct mm_struct *mm, unsigned long vpage);

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
    unsigned long vpage, physical_page_addr;
    unsigned long total_pages = 0;
    for_each_process(thechild) { 
        if (thechild->pid > MIN_PID) {
            total_pages = 0;
            if (thechild->mm && thechild->mm->mmap) {
                for (vma = thechild->mm->mmap; vma; vma->vm_next) {
                    for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE) {
                        // physical_page_addr = virt2phys(thechild->mm, vpage);
                        physical_page_addr = virt_to_phys((void *) vpage);
                        if (physical_page_addr != 0) {
                            total_pages++;
                        }
                    }
                }
            }
        }
        printk(KERN_INFO "%d,%s,%lu\n", thechild->pid, thechild->comm, total_pages);
    }

    return 0;
}

unsigned long virt2phys(struct mm_struct *mm, unsigned long vpage) {
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page;
    unsigned long physical_page_addr;
    pgd = pgd_offset(mm, vpage);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
        return 0;
    p4d = p4d_offset(pgd, vpage);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
        return 0;
    pud = pud_offset(p4d, vpage);
    if (pud_none(*pud) || pud_bad(*pud)) return 0;
    pmd = pmd_offset(pud, vpage);
    if (pmd_none(*pmd) || pmd_bad(*pmd))
    if (!(pte = pte_offset_map(pmd, vpage)))
        return 0;
    if (!(page = pte_page(*pte)))
        return 0;
    physical_page_addr = page_to_phys(page);
    pte_unmap(pte);
    // handle unmapped page
    if (physical_page_addr == 70368644173568)
        return 0;

    return physical_page_addr;
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
