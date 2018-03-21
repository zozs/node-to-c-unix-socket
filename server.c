#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/node-to-c-unix-socket.sock"

void do_stuff_with_data(unsigned char *data, uint8_t len, unsigned char **response_data, uint8_t *response_len)
{
	// Simply respond with same data that was sent.
	*response_data = malloc(len);
	memcpy(*response_data, data, len);

	*response_len = len;
}

int main(int argc, char **argv)
{
	int fd;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("Failed to create socket.");
		return 1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	if (strlcpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)) >= sizeof(addr.sun_path)) {
		fprintf(stderr, "SOCKET_PATH too long!\n");
		return 1;
	}
	
	unlink(SOCKET_PATH);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Failed to bind to socket");
		return 1;
	}

	if (listen(fd, 10) == -1) {
		perror("Failed to listen on socket");
		return 1;
	}
	
	fprintf(stderr, "Ready to accept connections.\n");
	while (1) {
		int client;
		if ((client = accept(fd, NULL, NULL)) == -1) {
			perror("Failed to accept incoming connection");
			return 1;
		}

		// First read length.
		uint8_t len;
		int res = read(client, &len, sizeof(len));
		if (res == 0) {
			// EOF before any data.
			fprintf(stderr, "Unexpected EOF\n");
			continue;
		} else if (res == -1) {
			// error when reading.
			perror("Failed to read data");
			return 1;
		}

		// Then read remaining data.
		unsigned char buf[256];
		int bufread = 0;
		while ((res = read(client, buf + bufread, (int)len - bufread)) > 0) {
			bufread += res;
		}

		if (res == -1) {
			perror("Unexpected error when reading data\n");
			return 1;
		}
		
		fprintf(stderr, "Read %d bytes from socket. Now handling it.\n", bufread);
		
		unsigned char *response;
		uint8_t response_len;
		do_stuff_with_data(buf, bufread, &response, &response_len);
		if (response_len > 0) {
			fprintf(stderr, "Writing back response of %d bytes.\n", (int)response_len);
			res = write(client, &response_len, sizeof(response_len));
			if (res != 1) {
				fprintf(stderr, "Failed to write all of response!\n");
				return 1;
			}

			res = write(client, response, response_len);
			if (res != response_len) {
				fprintf(stderr, "Failed to write all of response!\n");
				return 1;
			}
			free(response);
		}
		close(client);
	}

	return 0;
}
