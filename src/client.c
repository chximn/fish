#include <mongoose.h>
#include <paper.h>
#include <terminal.h>

#include <pthread.h>
#include <stdbool.h>

#include <fish.h>

bool done = false;
bool init = false;
bool welcomed = false;
paper_t terminal_in, terminal_out;

void * __term_out2conn_thread(void * args) {
	struct mg_connection * c = (struct mg_connection *) args;

	char buf[2048];
	while(true) {
		paper_size_t sz;
		if (!paper_read(terminal_out, buf, sizeof(buf), &sz)) {
			// disconnected
			done = true;
			// perror("ret < 0\n");
			printf("disconnected!\n");
			break;
		}
		else if (sz > 0) {
			mg_send(c, buf, sz);
		}
	}

	return NULL;
}

void start_shell(struct mg_connection *c) {
	// open terminal
	if (!open_terminal(&terminal_in, &terminal_out)) {
		printf("Failed to open terminal!\n");
	}
	init = true;

	// start thread to read from terminal's out
	pthread_t thread;
	pthread_create(&thread, NULL, __term_out2conn_thread, c);
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_CONNECT) {
		printf("connected!\n");
	}
	
	else if (ev == MG_EV_READ) {
		if (!welcomed) {
			char * welcome = WELCOME_BYTES;
			int l = strlen(welcome);
			if (c->recv.len != l + 1) {
				// not welcome :(
				done = true;
			}

			else {
				for (int i = 0; i < l; i++) {
					if (c->recv.buf[i] != welcome[i]) {
						// not welcome :(
						done = true;
						break;
					}
				}
			}

			if (!done) {
				welcomed = true;
				printf("welcomed!\n");
				mg_iobuf_del(&c->recv, 0, c->recv.len);
				start_shell(c);
			}

			else {
				printf("not welcome :(\n");
			}
			
			return;
		}

		if (welcomed) {
			paper_size_t sz;
			if (!init) printf("fd not initialized!\n");
			else if (!paper_write(terminal_in, c->recv.buf, c->recv.len, &sz)) {
				printf("failed to write to terminal's in\n");
			}
			else mg_iobuf_del(&c->recv, 0, c->recv.len);
		}
	}
	
	else if (ev == MG_EV_ERROR) {
		printf("error\n");
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
	
	int time_elapsed = 0;
	while(!done) {
		if (!welcomed) {
			if (time_elapsed > 10000) {
				printf("welcome timeout!\n");
				break;
			}
			else time_elapsed += 10;
		}
		mg_mgr_poll(&mgr, 10);
	}                     
	mg_mgr_free(&mgr);                                       // Free resources
	return 0;
}