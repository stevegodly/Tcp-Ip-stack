#ifndef LAYER2_H
#define LAYER2_H

#include "../utils.h"
#include "../graph.h"
#include "../net.h"
#include "../comms.h"
#include "../gluethread/glthread.h"

#define ARP_BROAD_REQ   1
#define ARP_REPLY       2
#define ARP_MSG         806
#define BROADCAST_MAC   0xFFFFFFFFFFFF

#define ETH_FCS(eth_hdr_ptr, payload_size) (eth_hdr_ptr)->FCS;

#pragma pack (push,1)
typedef struct arp_hdr_{

    short hw_type;          /*1 for ethernet cable*/
    short proto_type;       /*0x0800 for IPV4*/
    char hw_addr_len;       /*6 for MAC*/
    char proto_addr_len;    /*4 for IPV4*/
    short op_code;          /*req or reply*/
    mac_add_t src_mac;      /*MAC of OIF interface*/
    unsigned int src_ip;    /*IP of OIF*/
    mac_add_t dst_mac;      /*?*/
    unsigned int dst_ip;        /*IP for which ARP is being resolved*/
} arp_hdr_t;

typedef struct ethernet_hdr_{
    mac_add_t dst_mac;
    mac_add_t src_mac;
    unsigned short type;
    char payload[248];  /*Max allowed 1500*/
    unsigned int FCS;
} ethernet_hdr_t;

#pragma pack(pop)

typedef struct arp_table_{
    glthread_t arp_entries;
} arp_table_t;

typedef struct arp_entry_{
    ip_add_t ip_addr;   
    mac_add_t  mac_addr;
    char oif_name[NAME_SIZE];
    glthread_t arp_glue;
} arp_entry_t;

/*VLAN support*/

#pragma pack (push,1)
/*Vlan 802.1q 4 byte hdr*/
typedef struct vlan_8021q_hdr_{

    unsigned short tpid; /* = 0x8100*/
    short tci_pcp : 3 ;  /* inital 4 bits not used in this course*/
    short tci_dei : 1;   /*Not used*/
    short tci_vid : 12 ; /*Tagged vlan id*/
} vlan_8021q_hdr_t;

typedef struct vlan_ethernet_hdr_{

    mac_add_t dst_mac;
    mac_add_t src_mac;
    vlan_8021q_hdr_t vlan_8021q_hdr;
    unsigned short type;
    char payload[248];  /*Max allowed 1500*/
    unsigned int FCS;
} vlan_ethernet_hdr_t;
#pragma pack(pop)

GLTHREAD_TO_STRUCT(getArpEntry,arp_entry_t,arp_glue);

static inline ethernet_hdr_t *ALLOC_ETH_HDR_WITH_PAYLOAD(char *pkt, unsigned int pkt_size);

void init_arp_table(arp_table_t **arpTable);

arp_entry_t *arpTableLookup(arp_table_t *arpTable,char *ip);

bool_t addArpEntry(arp_table_t *arpTable,arp_entry_t *entry);

void deleteArpEntry(arp_table_t *arpTable,char *ip);

void arpTableUpdate(arp_table_t *arpTable,arp_hdr_t *arpHeader,interface *intf);

void arpReplyTableUpdate(arp_table_t *arpTable,arp_hdr_t *arpHeader,interface *intf);

void arpSendBroadcast(node_t * node,interface *intf,char *ip);

static void processArpBroadcastRequest(node_t *node, interface *iif, ethernet_hdr_t *ethernet_hdr);

void layer2FrameRecv(node_t *node, interface *intf,char *pkt, unsigned int pkt_size);

void promoteToL3();

void dumpArpTable(arp_table_t *arp_table);

static inline bool_t l2_frame_recv_qualify_on_interface(interface *intf, ethernet_hdr_t *ethernet_hdr){
    if(!intf->intf_nw_props.is_ipadd_config) return FALSE;

    if(memcmp(IF_MAC(intf),ethernet_hdr->dst_mac.mac,sizeof(mac_add_t)) == 0){
        return TRUE;
    }
    if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac.mac)){
        return TRUE;
    }
    return FALSE;
}



#endif