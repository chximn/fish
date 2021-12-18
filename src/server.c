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

#include <constants.h>

bool done = false;
struct fish_t f;

void * __stdin2conn_thread(void * args) {
	char buf[2048];
	while(!done) {

		paper_size_t sz = 0;
		if (!paper_read(0, buf, sizeof(buf), &sz)) {
			continue;
		}

		else if (sz > 0) {
			fish_send(&f, CHANNEL_SHELL, buf, sz);
		}
	}

	return NULL;
}

void terminal_set_size(uint32_t cols, uint32_t rows) {
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
	terminal_set_size(ws.ws_col, ws.ws_row);
}

void handle_packet(struct fish_packet_t * packet) {
	if (packet->channel == CHANNEL_SHELL) {
		int sz = write(1, packet->data, packet->data_size);

		if (sz < packet->data_size) {
			printf("failed to write to stdout\n");
		}
	}
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_ACCEPT) {
		// ignore new connection if we already have one
		if (f.connection != NULL) {
			c->is_closing = 1;
		}

		else {
			// connected
			f.connection = c;
			terminal_size_handler(SIGWINCH);
			system("stty raw -echo");
			pthread_t stdin2conn_thread;
			pthread_create(&stdin2conn_thread, NULL, __stdin2conn_thread, NULL);
		}
	}
	
	else if (ev == MG_EV_READ) {
		struct fish_packets_t * packets = fish_recv(&f, c->recv.buf, c->recv.len);

		for (struct fish_packets_t * current = packets; current != NULL; current = current->next) {
			struct fish_packet_t * packet = current->packet;

			handle_packet(packet);
		}

		fish_packets_free(packets);

		mg_iobuf_del(&c->recv, 0, c->recv.len); 
	}

	else if (ev == MG_EV_ERROR) {
		printf("error\n");
	}

	else if (ev == MG_EV_CLOSE) {
		if (c == f.connection) {
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
	mg_listen(&mgr, endpoint, fn, &mgr);              // Setup listener
	while (!done) mg_mgr_poll(&mgr, 10);              // Event loop
	mg_mgr_free(&mgr);                                // Cleanup

	return 0;
}