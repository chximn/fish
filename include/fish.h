#ifndef H_FISH
#define H_FISH

#include <unistd.h>

struct fish_packet {
    uint8_t  channel;
    uint32_t data_size;
    uint8_t* data;
};

struct fish_packets {
	struct fish_packet * packet;
	struct fish_packets * next;
};

struct fish {
	struct mg_connection * connection;
	uint8_t buffer[64 * 1024];
	uint32_t len = 0;
};

void fish_send(struct fish * f, uint8_t channel, void * data, uint32_t size) {
    uint32_t buffer_size = sizeof(uint8_t) + sizeof(uint32_t) + size;
	uint8_t buffer[buffer_size];

	// TODO: channel selectable
	*((uint8_t *) buffer) = channel;
	*((uint32_t *) (buffer + sizeof(uint8_t))) = size;
	memcpy(buffer + sizeof(uint8_t) + sizeof(uint32_t), data, size);
    mg_send(f->connection, buffer, buffer_size);
}

void __fish_buffer(struct fish * f, void * data, uint32_t size) {
	memcpy(f->buffer + f->len, data, size);
	f->len += size;
}

void fish_packet_free(struct fish_packet *) {
// TODO:
}

void fish_packets_free(struct fish_packets *) {
// TODO:
}

// TODO: add if to check if malloc worked
struct fish_packet * __fish_consume(struct fish * f) {
	// min size
	if (f->len < sizeof(uint32_t) + 2 * sizeof(uint8_t)) {
		return NULL;
	}

	uint8_t channel = *((uint8_t *) f->buffer);
	uint32_t data_size = *((uint32_t *) (f->buffer + sizeof(uint8_t)));
	
	// not complete packet
	uint32_t packet_size = sizeof(uint8_t) + sizeof(uint32_t) + data_size;
	if (packet_size > f->len) {
		return NULL;
	}

	void * data = (void*)(f->buffer + sizeof(uint8_t) + sizeof(uint32_t));

	// create packet
	struct fish_packet * packet = (struct fish_packet *) malloc(sizeof(struct fish_packet));
	packet->channel = channel;
	packet->data_size = data_size;
	packet->data = (uint8_t *) malloc(sizeof(uint8_t) * data_size);
	memcpy(packet->data, data, data_size);

	// free used data
	for (int i = 0; i < f->len - packet_size; i++) {
		f->buffer[i] = f->buffer[i + packet_size];
	}

	f->len -= packet_size;

	return packet;
}

struct fish_packets * fish_recv(struct fish * f, void * data, uint32_t size) {
	__fish_buffer(f, data, size);

	struct fish_packets * packets = NULL;
	struct fish_packets * current = NULL;

	while(true) {
		struct fish_packet * packet = __fish_consume(f);
        
		
		if (packet == NULL) {
			break;
		}

		struct fish_packets * item = (struct fish_packets *) malloc(sizeof(struct fish_packets));
		item->packet = packet;
		item->next = NULL;

		// first item in list, so... make it the first?.. duh
		if (packets == NULL) {
			packets = item;
			current = item;
		}

		// append
		else {
			current->next = item;
			current = item;
		}

	}

	return packets;
}

#endif