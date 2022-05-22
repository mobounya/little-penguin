#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int fd;

    fd = open("/sys/kernel/debug/fortytwo/id", O_RDWR);
    if (fd == -1) {
        printf("%s\n", strerror(errno));
        return fd;    
    } else {
        char buffer[50] = { '\0' };
        int ret;
        printf("<------------- READING ----------->\n");
        ret = read(fd, buffer, 9);
        printf("%d || %s\n", ret, buffer);
        printf("<------------- WRITING COORECT LOGIN ----------->\n");
        ret = write(fd, "mobounya", 9);
        printf("%d\n", ret);
        printf("<------------- WRITING UN-COORECT LOGIN ----------->\n");
        ret = write(fd, "blop", 4);
        printf("%d\n", ret);
    }
}
