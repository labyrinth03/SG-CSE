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
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *);
void halt(void);
void exit(int status);
static tid_t exec(const char *user_cmd_line);
int wait(tid_t pid);

static struct lock filesys_lock; // 파일 시스템 동기화를 위한 lock

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock); // lock init
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
    case SYS_READ:
    {
      check_address(f->esp + 12);
      int fd = *(int *)(f->esp + 4);
      void *buffer = *(void **)(f->esp + 8);
      unsigned size = *(unsigned *)(f->esp + 12);

      check_address(buffer); // 버퍼 유효성 검사

      lock_acquire(&filesys_lock);
      if (fd == 0) { // STDIN
          for (unsigned i = 0; i < size; i++) {
              ((uint8_t *)buffer)[i] = input_getc();
          }
          f->eax = size;
      } else {
          struct thread *cur = thread_current();
          if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
              f->eax = file_read(cur->fd_table[fd], buffer, size);
          } else {
              f->eax = -1;
          }
      }
      lock_release(&filesys_lock);
      break;      
    }
    case SYS_WRITE:
      {
        check_address(f->esp + 12);
        int fd = *(int *)(f->esp + 4);
        const void *buffer = *(const void **)(f->esp + 8);
        unsigned size = *(unsigned *)(f->esp + 12);

        check_address(buffer); // 버퍼 유효성 검사

        lock_acquire(&filesys_lock);
        if (fd == 1) { // STDOUT
            putbuf(buffer, size);
            f->eax = size;
        } else {
            struct thread *cur = thread_current();
            if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
                f->eax = file_write(cur->fd_table[fd], buffer, size);
            } else {
                f->eax = -1;
            }
        }
        lock_release(&filesys_lock);
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
    case SYS_CREATE:
    {
      check_address(f->esp + 4);
      check_address(f->esp + 8);
      const char *file = *(const char **)(f->esp + 4);
      unsigned initial_size = *(unsigned *)(f->esp + 8);

      check_string(file);

      lock_acquire(&filesys_lock);
      f->eax = filesys_create(file, initial_size);
      lock_release(&filesys_lock);
      break;      
    }
    case SYS_REMOVE:
    {
      check_address(f->esp + 4);
      const char *file = *(const char **)(f->esp + 4);

      check_string(file);

      lock_acquire(&filesys_lock);
      f->eax = filesys_remove(file);
      lock_release(&filesys_lock);
      break;      
    }
    case SYS_OPEN:
    {
      check_address(f->esp + 4);
      const char *file = *(const char **)(f->esp + 4);
      check_string(file);

      lock_acquire(&filesys_lock);
      struct file *file_obj = filesys_open(file);
      if (file_obj == NULL) {
          f->eax = -1;
      } else {
          struct thread *cur = thread_current();
          int fd = cur->next_fd++;
          cur->fd_table[fd] = file_obj;
          f->eax = fd;
      }
      lock_release(&filesys_lock);
      break;      
    }
    case SYS_CLOSE:
    {
      check_address(f->esp + 4);
      int fd = *(int *)(f->esp + 4);
      struct thread *cur = thread_current();

      if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
          lock_acquire(&filesys_lock);
          file_close(cur->fd_table[fd]);
          cur->fd_table[fd] = NULL;
          lock_release(&filesys_lock);
      }
      break;   
    }
    case SYS_FILESIZE:
    {
      check_address(f->esp + 4);
      int fd = *(int *)(f->esp + 4);
      struct thread *cur = thread_current();

      lock_acquire(&filesys_lock);
      if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
          f->eax = file_length(cur->fd_table[fd]);
      } else {
          f->eax = -1;
      }
      lock_release(&filesys_lock);
      break;  
    }
    case SYS_SEEK:
    {
      check_address(f->esp + 8);
      int fd = *(int *)(f->esp + 4);
      unsigned position = *(unsigned *)(f->esp + 8);
      struct thread *cur = thread_current();

      lock_acquire(&filesys_lock);
      if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
          file_seek(cur->fd_table[fd], position);
      }
      lock_release(&filesys_lock);
      break; 
    }
    case SYS_TELL:
    {
      check_address(f->esp + 4);
      int fd = *(int *)(f->esp + 4);
      struct thread *cur = thread_current();

      lock_acquire(&filesys_lock);
      if (fd >= 2 && fd < cur->next_fd && cur->fd_table[fd] != NULL) {
          f->eax = file_tell(cur->fd_table[fd]);
      } else {
          f->eax = -1;
      }
      lock_release(&filesys_lock);
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