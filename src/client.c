#include <mongoose.h>
#include <paper.h>
#include <terminal.h>

#include <pthread.h>
#include <stdbool.h>

#include <fish.h>

#include <constants.h>


struct fish_t f;
bool done = false;
bool init = false;
struct terminal_t terminal;


void * __term_out2conn_thread(void * args) {

	char buf[2048];
	while(true) {
		paper_size_t sz = 0;
		if (!paper_read(terminal.out, buf, sizeof(buf), &sz)) {
			done = true;
			printf("cant read from pty's stdout!\n");
			break;
		}
		else if (sz > 0) {
			fish_send(&f, CHANNEL_SHELL, buf, sz);
		}
	}

	return NULL;
}

void start_shell(struct mg_connection *c) {
	// open terminal
	if (!terminal_open(&terminal)) {
		printf("Failed to open terminal!\n");
	}

	init = true;

	// start thread to read from terminal's out
	pthread_t thread;
	pthread_create(&thread, NULL, __term_out2conn_thread, c);
}

void handle_packet(struct fish_packet_t * packet) {
	if (packet->channel == CHANNEL_SHELL) {

		paper_size_t sz;
		if (!paper_write(terminal.in, packet->data, packet->data_size, &sz) || sz < packet->data_size) {
			printf("failed to write to terminal's in\n");
		}
	}

	else if (packet->channel == CHANNEL_COMMAND) {
		uint8_t command = *((uint8_t*)(packet->data));
		
		if (command == COMMAND_TERMINAL_SIZE) {

			uint32_t data_size = sizeof(uint8_t) + 2 * sizeof(uint32_t);
			if (packet->data_size != data_size) {
				printf("invalid command packet size\n");
				return;
			}

			uint32_t cols = *((uint32_t*)(packet->data + sizeof(uint8_t)));
			uint32_t rows = *((uint32_t*)(packet->data + sizeof(uint8_t) + sizeof(uint32_t)));

			// printf("got terminal size command w/ cols=%d rows=%d\n", cols, rows);
			terminal_set_size(&terminal, cols, rows);
		}

		else {
			printf("unkown command!\n");
		}

	}

	else {
		printf("Unkown channel!\n");
	}
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_CONNECT) {
		printf("connected!\n");
		f.connection = c;

		start_shell(c);
	}
	
	else if (ev == MG_EV_READ) {
		if (!init) printf("fd not initialized!\n");
		else {

			struct fish_packets_t * packets = fish_recv(&f, c->recv.buf, c->recv.len);

			for (struct fish_packets_t * current = packets; current != NULL; current = current->next) {
				struct fish_packet_t * packet = current->packet;

				handle_packet(packet);
			}

			fish_packets_free(packets);
			mg_iobuf_del(&c->recv, 0, c->recv.len);
		}
	}
	
	else if (ev == MG_EV_ERROR) {
		printf("error\n");
	}

	else if (ev == MG_EV_CLOSE) {
		done = true;
		printf("disconnected!\n");
	}
}

int main(int argc, char *argv[]) {
#if MG_ARCH == MG_ARCH_WIN32
	if (handle_agent_request(argc, argv)) {
		return 0;
	}
#endif

	if (argc != 3) {
		printf("bababoy\n");
		return -1;
	}

	char endpoint[1024];
	sprintf(endpoint, "tcp://%s:%s", argv[1], argv[2]);

	struct mg_mgr mgr;                                       // Event manager
	mg_log_set("0"); 
	mg_mgr_init(&mgr);                                       // Initialise event manager
	mg_connect(&mgr, endpoint, fn, &mgr); // Create client connection
	
	while(!done) mg_mgr_poll(&mgr, 10);
	mg_mgr_free(&mgr);                                       // Free resources
	return 0;
}