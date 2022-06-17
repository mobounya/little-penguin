#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int fd;

    fd = open("/sys/kernel/debug/fortytwo/jiffies", O_RDONLY);
    if (fd == -1) {
        printf("%s\n", strerror(errno));
        return fd;    
    } else {
        char buffer[100] = { '\0' };
       	ssize_t ret = read(fd, buffer, 100);
        printf("%s\n", buffer);
    }
}
