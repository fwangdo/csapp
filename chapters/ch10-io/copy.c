#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    char buf[4096];

    for (;;) {
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n == 0) {
            break;
        }
        if (n < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            return 1;
        }

        char *p = buf;
        ssize_t left = n;
        while (left > 0) {
            ssize_t written = write(STDOUT_FILENO, p, (size_t)left);
            if (written < 0) {
                fprintf(stderr, "write: %s\n", strerror(errno));
                return 1;
            }
            p += written;
            left -= written;
        }
    }

    return 0;
}
