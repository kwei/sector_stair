#ifndef __TRANSMIT_HEADER
#define __TRANSMIT_HEADER

typedef struct header{
	unsigned char WiGig_id;
	unsigned char sector;
	// int ack;
}WiGig_header;
WiGig_header* WiGig_create_header();
void WiGig_free_header(WiGig_header* ptr);
unsigned char WiGig_get_ID(WiGig_header* ptr);
void WiGig_set_sector(WiGig_header* ptr, unsigned char sector);
unsigned char WiGig_get_sector(WiGig_header* ptr);
void WiGig_free_header(WiGig_header* ptr);
// #ifndef ACK
// #define ACK 1
// #endif

// #ifndef NAK
// #define NAK 1
// #endif

#endif