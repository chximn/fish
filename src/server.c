#include <mongoose.h>

#include <unistd.h>
#include <termios.h>

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <paper.h>
#include <fish.h>

bool done = false;
bool connected = false;
struct termios term_saved;
struct mg_connection * connection = NULL;

void accept(struct mg_connection * c) {
	char * w = WELCOME_BYTES;
	mg_send(c, w, strlen(w) + 1);
}

void * __stdin2conn_thread(void * args) {
	struct mg_connection * c = (struct mg_connection *) args;

	char buf[2048];
	while(!done) {
		paper_size_t sz;
		if (!paper_read(0, buf, sizeof(buf), &sz)) {
			continue;
		}
		else if (sz > 0) {
			mg_send(c, buf, sz);
		}
	}

	return NULL;
}

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_ACCEPT) {
		// ignore new connection if we already have one
		if (!connected) {
			// connected
			connected = true;
			connection = c;
			accept(c);
			system("stty raw -echo");
			pthread_t thread;
			pthread_create(&thread, NULL, __stdin2conn_thread, c);
		}
	}
	
	else if (ev == MG_EV_READ) {
		write(1, c->recv.buf, c->recv.len);
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