#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define MEMORY_THRESHOLD 80 // Threshold in percentage
#define CONFIG_BUFFER_SIZE 1024

static char kill_processes[CONFIG_BUFFER_SIZE] = "";
static char avoid_pids[CONFIG_BUFFER_SIZE] = "";
static struct proc_dir_entry *config_entry;
static struct proc_dir_entry *config_pids;
static struct timer_list my_timer; 



static int get_memory_usage(void) {
    struct sysinfo si;
    si_meminfo(&si);
    unsigned long total_mem = si.totalram >> 10;
    unsigned long free_mem = si.freeram >> 10;   
    unsigned long used_mem = total_mem - free_mem;

    return (used_mem * 100) / total_mem;
}



static bool is_kill_process(const char *proc_name) {
    return strstr(kill_processes, proc_name) != NULL;
}



static bool is_avoid_pid(const int *proc_pid) {
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", proc_pid);

    return strstr(avoid_pids, pid_str) != NULL;
}



static void monitor_memory(struct timer_list *timer) {
    struct task_struct *task;

        if (get_memory_usage() > MEMORY_THRESHOLD) {
            for_each_process(task) {
                if (task->mm && is_kill_process(task->comm) && !is_avoid_pid(task->pid)) {
                    pr_info("Terminating process: %s [PID: %d]\n", task->comm, task_pid_nr(task));
                    send_sig(SIGKILL, task, 1);
                }
            }
        }
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
}



static ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *offset) {
    return simple_read_from_buffer(buffer, count, offset, kill_processes, strlen(kill_processes));
}



static ssize_t proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset) {
    if (count > CONFIG_BUFFER_SIZE - 1)
        return -EINVAL;

    if (copy_from_user(kill_processes, buffer, count))
        return -EFAULT;

    kill_processes[count] = '\0'; 
    return count;
}



static struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};



static ssize_t pid_read(struct file *file, char __user *buffer, size_t count, loff_t *offset) {
    return simple_read_from_buffer(buffer, count, offset, avoid_pids, strlen(avoid_pids));
}



static ssize_t pid_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset) {
    if (count > CONFIG_BUFFER_SIZE - 1)
        return -EINVAL;

    if (copy_from_user(avoid_pids, buffer, count))
        return -EFAULT;

    avoid_pids[count] = '\0';
    return count;
}



static struct proc_ops pid_fops = {
    .proc_read = pid_read,
    .proc_write = pid_write,
};



static int __init ram_monitor_init(void) {
    pr_info("RAM Monitor Kernel Module Loaded.\n");

    config_entry = proc_create("ram_monitor_config", 0666, NULL, &proc_fops);
    config_pids = proc_create("ram_monitor_pids", 0666, NULL, &pid_fops);

    if (!config_entry) {
        pr_err("Failed to create /proc entry.\n");
        return -ENOMEM;
    }

    if (!config_pids) {
        pr_err("Failed to create /proc entry.\n");
        return -ENOMEM;
    }

    timer_setup(&my_timer, monitor_memory, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}



static void __exit ram_monitor_exit(void) {
    pr_info("RAM Monitor Kernel Module Unloaded.\n");
    proc_remove(config_entry);
    proc_remove(config_pids);
    del_timer(&my_timer);
}

module_init(ram_monitor_init);
module_exit(ram_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nicholas Dinis");
MODULE_DESCRIPTION("A Linux kernel module to monitor RAM and terminate processes.");
