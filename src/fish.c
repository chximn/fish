#include <fish.h>

void fish_send(struct fish_t * f, uint8_t channel, void * data, uint32_t size) {
    uint32_t buffer_size = sizeof(uint8_t) + sizeof(uint32_t) + size;
	uint8_t buffer[buffer_size];

	*((uint8_t *) buffer) = channel;
	*((uint32_t *) (buffer + sizeof(uint8_t))) = size;
	memcpy(buffer + sizeof(uint8_t) + sizeof(uint32_t), data, size);
    mg_send(f->connection, buffer, buffer_size);
}

void __fish_buffer(struct fish_t * f, void * data, uint32_t size) {
	memcpy(f->buffer + f->len, data, size);
	f->len += size;
}

struct fish_packet_t * __fish_consume(struct fish_t * f) {
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
	struct fish_packet_t * packet = (struct fish_packet_t *) malloc(sizeof(struct fish_packet_t));
	if (packet == NULL) {
		printf("1malloc\n");
		return NULL;
	}
	packet->channel = channel;
	packet->data_size = data_size;
	packet->data = (uint8_t *) malloc(sizeof(uint8_t) * data_size);
	if (packet->data == NULL) {
		printf("2malloc\n");
		return NULL;
	}
	memcpy(packet->data, data, data_size);

	// free used data
	for (int i = 0; i < f->len - packet_size; i++) {
		f->buffer[i] = f->buffer[i + packet_size];
	}

	f->len -= packet_size;

	return packet;
}

struct fish_packets_t * fish_recv(struct fish_t * f, void * data, uint32_t size) {
	__fish_buffer(f, data, size);

	struct fish_packets_t * packets = NULL;
	struct fish_packets_t * current = NULL;

	while(true) {
		struct fish_packet_t * packet = __fish_consume(f);
        
		
		if (packet == NULL) {
			break;
		}

		struct fish_packets_t * item = (struct fish_packets_t *) malloc(sizeof(struct fish_packets_t));
		if (item == NULL) {
			printf("3malloc\n");
			return NULL;
		}
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

void fish_packet_free(struct fish_packet_t * packet) {
	free(packet->data);
	free(packet);
}

void fish_packets_free(struct fish_packets_t * packets) {
	for (struct fish_packets_t * current = packets; current != NULL;) {
		fish_packet_free(current->packet);
		struct fish_packets_t * next = current->next;
		free(current);
		current = next;
	}
}
