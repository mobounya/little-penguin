#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int fd_read;
    int fd_write;
    
    fd_read = open("/sys/kernel/debug/fortytwo/foo", O_RDONLY);
    fd_write = open("/sys/kernel/debug/fortytwo/foo", O_WRONLY);
    if (fd_read == -1 || fd_write == -1) {
        printf("%s\n", strerror(errno));
        return -1;    
    } else {
        char buffer[50] = { '\0' };
	size_t ret = 0;
        printf("<------------- WRITING DATA ----------->\n");
        ret = write(fd_write, "THE_UNIVERSE_IS_EXPANDING", 25);
        printf("%d\n", ret);
	printf("<------------ READING DATA ------------>\n");
	ret = read(fd_read, buffer, 25);
	printf("%d || %s\n", ret, buffer);
    }
}
