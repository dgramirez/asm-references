#ifndef ASMREF_INCLUDE_ASMREF_SYSCALL_LINUX_X86_64_H
#define ASMREF_INCLUDE_ASMREF_SYSCALL_LINUX_X86_64_H

extern isz
sys_write(i32 fd,
          const void *buf,
          usz count);

extern void *
sys_mmap(void *addr,
         usz length,
         i32 prot,
         i32 flags,
         i32 fd,
         i32 offset);

extern i32
sys_munmap(void *addr,
           usz length);

extern void
sys_exit(i32 status);

#endif // ASMREF_INCLUDE_ASMREF_SYSCALL_LINUX_X86_64_H

