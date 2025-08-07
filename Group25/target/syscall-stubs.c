#include <errno.h>

/**
 * We are linking with libnosys, which declares implementations of the OS-level
 * syscalls for reading/writing files that return error values. Newer versions
 * of the toolchain emit linker warnings when these functions are linked.
 * Manually define them here to silence these warnings.
 */

#define DECLARE_SYSCALL_NOT_IMPL(name, ...) \
    int name(__VA_ARGS__) {                 \
        errno = ENOSYS;                     \
        return -1;                          \
    }

DECLARE_SYSCALL_NOT_IMPL(_close, int fd);
DECLARE_SYSCALL_NOT_IMPL(_read, int fd, char *buf, int len);
DECLARE_SYSCALL_NOT_IMPL(_write, int fd, char *buf, int len);
DECLARE_SYSCALL_NOT_IMPL(_lseek, int fd, int ptr, int dir)
