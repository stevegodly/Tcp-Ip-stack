#ifndef COMMS_H
#define COMMS_H

#define MAX_BUFFER  15

static unsigned int nextPortNo();

void init_udp_socket(node_t *node);

void network_start_packet_thread(graph *topo);

void sendPacket(char *pckt,int pcktSize,interface *interface_t);

#endif