#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"     
#include "userprog/pagedir.h"  
#include "devices/shutdown.h"  
#include "userprog/process.h"
#include "threads/palloc.h"

static void syscall_handler (struct intr_frame *);
void halt(void);
void exit(int status);
static tid_t exec(const char *user_cmd_line);
int wait(tid_t pid);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
check_address(const void *addr)
{
    if (addr == NULL || !is_user_vaddr(addr) || 
        pagedir_get_page(thread_current()->pagedir, addr) == NULL)
    {
        exit(-1);
    }
}

static void
check_string(const char *str)
{
    check_address(str);
    while (*str != '\0')
    {
        str++;
        check_address(str);
    }
}

static void
syscall_handler (struct intr_frame *f) 
{
  check_address(f->esp);

  int syscall_number = *(int *)(f->esp);
  switch (syscall_number) {
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      {
        check_address(f->esp + 4);
        exit(*(int *)(f->esp + 4));
        break;
      }
    case SYS_WRITE:
      {
        check_address(f->esp + 12); 
        int fd = *(int *)(f->esp + 4);
        const void *buffer = *(const void **)(f->esp + 8); 
        unsigned size = *(unsigned *)(f->esp + 12);

        check_address(buffer);
        for (unsigned i = 0; i < size; i++)
        {
            check_address((const char *)buffer + i);
        }

        if (fd == 1) { // STDOUT
            putbuf(buffer, size);
            f->eax = size;
        } 
        else { //file descriptor
            f->eax = -1;
        }
        break;      
      }
    case SYS_EXEC:
      {
        check_address(f->esp + 4);
        char *cmd_line = *(char **)(f->esp + 4);
        f->eax = exec(cmd_line);
        break;
      }
    case SYS_WAIT:
      {
        check_address(f->esp + 4);
        int pid = *(int *)(f->esp + 4);
        f->eax = wait(pid);
        break;
      }
    case SYS_FIBONACCI:
      {
        check_address(f->esp + 4);
        int n = *(int *)(f->esp + 4);
        f->eax = fibonacci(n);
        break;
      }
    case SYS_MAX_OF_FOUR_INT:
      {
        check_address(f->esp + 16);
        int a = *(int *)(f->esp + 4);
        int b = *(int *)(f->esp + 8);
        int c = *(int *)(f->esp + 12);
        int d = *(int *)(f->esp + 16);
        f->eax = max_of_four_int(a, b, c, d);
        break;
      }
    default:
      exit(-1);
      break;
  }

}

void halt(void) {
  shutdown_power_off();
}

void exit(int status) {
    struct thread *cur = thread_current();
    cur->exit_status = status;
    //printf("%s: exit(%d)\n", cur->name, status);
    
    thread_exit();
}

static tid_t exec(const char *cmd_line) {
    check_string(cmd_line);
    return process_execute(cmd_line);
}

int wait(tid_t pid) {
  return process_wait(pid);
}

int fibonacci(int n) {
  int result = 0;
  int temp1 = 0, temp2 = 1;
  for (int i = 0; i < n; i++) {
    result = temp1 + temp2;
    temp1 = temp2;
    temp2 = result;
  }
  return result;
}
int max_of_four_int(int a, int b, int c, int d) {
  int max = a;
  if (b > max) max = b;
  if (c > max) max = c;
  if (d > max) max = d;
  return max;
}