#ifndef H_FISH
#define H_FISH

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <mongoose.h>

struct fish_packet_t {
    uint8_t  channel;
    uint32_t data_size;
    uint8_t* data;
};

struct fish_packets_t {
	struct fish_packet_t * packet;
	struct fish_packets_t * next;
};

struct fish_t {
	struct mg_connection * connection;
	uint8_t buffer[64 * 1024];
	uint32_t len = 0;
};

void fish_send(struct fish_t * f, uint8_t channel, void * data, uint32_t size);
struct fish_packets_t * fish_recv(struct fish_t * f, void * data, uint32_t size);
void fish_packet_free(struct fish_packet_t * packet);
void fish_packets_free(struct fish_packets_t * packets);

#endif