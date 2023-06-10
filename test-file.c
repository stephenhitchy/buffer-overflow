#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
                
int main(int argc, char *argv[])          
{               
	int rc, fd = -1;
	ssize_t size;
	char buf[1024];
    for(int i = 0; i < 11; i++){
        buf[i] = 'a';
    }
    buf[11] = '\n';
	struct addrinfo hints = { .ai_socktype = SOCK_STREAM };
	struct addrinfo *info = NULL, *p;
	int exit_code = EXIT_FAILURE;

	/* Lookup host. */
	const char *domain = argv[1];
	const char *port = argv[2];
	rc = getaddrinfo(domain , port, &hints, &info);
	if (0 != rc) {
		fprintf(stderr, "error looking up towel.blinkenlights.nl: %s\n", gai_strerror(rc));
		goto done;
	}

	/* Connect to host; first option that works wins. */
	for (p = info; p; p = p->ai_next) {
		fd = socket(p->ai_family, p->ai_socktype, 0);
		if (-1 == fd) {
			continue;
		}

		rc = connect(fd, p->ai_addr, p->ai_addrlen);
		if (rc > -1) {
			break;
		}
	}

	if (-1 == fd || -1 == rc) {
		perror("could not connect");
		goto done;
	}

	/* Read all bytes from host, and write them to stdout. */
        for (;;) {
                size = recv(fd, buf, sizeof(buf), 0);
                if (0 == size) {
                        break;
                } else if (size > 0) {
                        write(STDOUT_FILENO, buf, size);
                } else {
			perror("error reading from connection");
                        goto done;
                }
        }

	exit_code = EXIT_SUCCESS;

done:
	if (fd != -1) {
		close(fd);
	}

	exit(exit_code);
}