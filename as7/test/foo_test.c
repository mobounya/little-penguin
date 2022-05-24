#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int fd;

    fd = open("/sys/kernel/debug/fortytwo/foo", O_RDWR);
    if (fd == -1) {
        printf("%s\n", strerror(errno));
        return fd;    
    } else {
        char buffer[50] = { '\0' };
	size_t ret = 0;
        printf("<------------- WRITING DATA ----------->\n");
        ret = write(fd, "THE_UNIVERSE_IS_EXPANDING", 25);
        printf("%d\n", ret);
	printf("<------------ READING DATA ------------>\n");
	ret = read(fd, buffer, 25);
	printf("%d || %s\n", ret, buffer);
    }
}
