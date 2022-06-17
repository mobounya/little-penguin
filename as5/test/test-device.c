#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int fd1;
    int fd2;
    int fd3;

    fd1 = open("/dev/fortytwo", O_RDWR);
    fd2 = open("/dev/fortytwo", O_RDWR);
    fd3 = open("/dev/fortytwo", O_RDWR);

    if (fd1 == -1 || fd1 == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    } else {
        char buffer[50] = { '\0' };
        int ret;
        printf("<------------- READING ----------->\n");
        ret = read(fd1, buffer, 9);
        printf("%d || %s\n", ret, buffer);
        printf("<------------- WRITING COORECT LOGIN ----------->\n");
        ret = write(fd2, "mobounya", 9);
        printf("%d\n", ret);
        printf("<------------- WRITING UN-COORECT LOGIN ----------->\n");
        ret = write(fd3, "blop", 4);
        printf("%d\n", ret);
    }
}
