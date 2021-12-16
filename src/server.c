#include <mongoose.h>

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <paper.h>

#include <fish.h>


#define CHANNEL_COMMAND 0x00
#define CHANNEL_SHELL 0x01

#define COMMAND_TERMINAL_SIZE 0x00

bool done = false;
struct mg_connection * connection = NULL;
struct fish_t f;

void * __stdin2conn_thread(void * args) {
	struct mg_connection * c = (struct mg_connection *) args;

	char buf[2048];
	while(!done) {
		paper_size_t sz;
		if (!paper_read(0, buf, sizeof(buf), &sz)) {
			continue;
		}
		else if (sz > 0) {

			fish_send(&f, CHANNEL_SHELL, buf, sz); 
			// fish_send(c, buf, sz);
			// mg_send(c, buf, sz);
		}
	}

	return NULL;
}

void set_terminal_size(uint32_t cols, uint32_t rows) {
	uint32_t buffer_size = sizeof(uint8_t) + 2 * sizeof(uint32_t);
	uint8_t buffer[buffer_size];

	*((uint8_t*)(buffer))= COMMAND_TERMINAL_SIZE;
	*((uint32_t*)(buffer + sizeof(uint8_t))) = cols;
	*((uint32_t*)(buffer + sizeof(uint8_t) + sizeof(uint32_t))) = rows;

	fish_send(&f, CHANNEL_COMMAND, buffer, buffer_size);
}

void terminal_size_handler(int sig) {
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

	set_terminal_size(ws.ws_col, ws.ws_row);
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_ACCEPT) {
		// ignore new connection if we already have one
		if (connection != NULL) {
			c->is_closing = 1;
		}

		else {
			// connected
			connection = c;
			f.connection = c;
			terminal_size_handler(SIGWINCH);
			system("stty raw -echo");
			pthread_t thread;
			pthread_create(&thread, NULL, __stdin2conn_thread, c);
		}
	}
	
	else if (ev == MG_EV_READ) {
		struct fish_packets_t * packets = fish_recv(&f, c->recv.buf, c->recv.len);

		for (struct fish_packets_t * current = packets; current != NULL; current = current->next) {
			struct fish_packet_t * packet = current->packet;

			int sz = write(1, packet->data, packet->data_size);

			if (sz < packet->data_size) {
				printf("failed to write to stdout\n");
			}
		}

		fish_packets_free(packets);

		mg_iobuf_del(&c->recv, 0, c->recv.len); 
	}

	else if (ev == MG_EV_ERROR) {
		printf("error\n");
	}

	else if (ev == MG_EV_CLOSE) {
		if (c == connection) {
			done = true;
			system("stty sane");
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: server <ip> <port>\n");
		return -1;
	}

	signal(SIGWINCH, terminal_size_handler);

	char endpoint[1024];
	sprintf(endpoint, "tcp://%s:%s", argv[1], argv[2]);

	struct mg_mgr mgr;
	mg_log_set("0"); 
	mg_mgr_init(&mgr);                                // Init manager
	mg_listen(&mgr, endpoint, fn, &mgr);  // Setup listener
	while (!done) mg_mgr_poll(&mgr, 10);              // Event loop
	mg_mgr_free(&mgr);                                // Cleanup

	return 0;
}