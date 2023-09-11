#include "layer2.h"
#include <stdlib.h>
#include <stdio.h>

void init_arp_table(arp_table_t **arpTable){
    *arpTable=calloc(1,sizeof(arp_table_t));
    initGlthread(&((*arpTable)->arp_entries));
}

arp_entry_t *arpTableLookup(arp_table_t *arpTable,char *ip){
    glthread_t *curr;
    arp_entry_t *entry;
    ITERATE_GLTHREAD_BEGIN(&(arpTable->arp_entries),curr){
        entry=getArpEntry(curr);
        if(!strcmp(entry->ip_addr.ip_addr,ip)) return entry;
    }ITERATE_GLTHREAD_END(&(arpTable->arp_entries),curr)

    return NULL;
}

void deleteArpEntry(arp_table_t *arpTable,char *ip){
    glthread_t *curr;
    arp_entry_t *entry;
    ITERATE_GLTHREAD_BEGIN(&(arpTable->arp_entries),curr){
        entry=getArpEntry(curr);
        if(!strcmp(entry->ip_addr.ip_addr,ip)) removeGlthread(curr);
    }ITERATE_GLTHREAD_END(&(arpTable->arp_entries),curr)
}

bool_t addArpEntry(arp_table_t *arpTable,arp_entry_t *entry){
    glthreadAddNext(&(arpTable->arp_entries),&(entry->arp_glue));
    return TRUE;
}

void arpTableUpdate(arp_table_t *arpTable,arp_hdr_t *arpHeader,interface *intf){
    glthread_t *curr;
    arp_entry_t *entry;
    ITERATE_GLTHREAD_BEGIN(&(arpTable->arp_entries),curr){
        entry=getArpEntry(curr);
        if(!strcmp(entry->oif_name,intf->name)){
            entry->mac_addr=arpHeader->src_mac;
            char *ip=NULL;
            get_abcd_ipFormat(arpHeader->src_ip,ip);
            memcpy(entry->ip_addr.ip_addr,ip,sizeof(ip_add_t));
            return;
        }
    }ITERATE_GLTHREAD_END(&(arpTable->arp_entries),curr)
}

void arpReplyTableUpdate(arp_table_t *arpTable,arp_hdr_t *arpHeader,interface *intf){
    arp_entry_t *entry=calloc(1,sizeof(arp_entry_t));
    get_abcd_ipFormat(htonl(arpHeader->src_ip),entry->ip_addr.ip_addr);
    memcpy(entry->mac_addr.mac,arpHeader->src_mac.mac,sizeof(mac_add_t));
    strcpy(entry->oif_name, intf->name);
    if(addArpEntry(arpTable,entry)) return;
}

void dumpArpTable(arp_table_t *arp_table){

    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&(arp_table->arp_entries),curr){
        arp_entry = getArpEntry(curr);
        printf("IP : %s, MAC : %u:%u:%u:%u:%u:%u, OIF = %s\n",arp_entry->ip_addr.ip_addr, 
            arp_entry->mac_addr.mac[0], 
            arp_entry->mac_addr.mac[1], 
            arp_entry->mac_addr.mac[2], 
            arp_entry->mac_addr.mac[3], 
            arp_entry->mac_addr.mac[4], 
            arp_entry->mac_addr.mac[5], arp_entry->oif_name);
    } ITERATE_GLTHREAD_END(&(arp_table->arp_entries), curr);
}

void arpSendBroadcast(node_t * node,interface *intf,char *ip){
    ethernet_hdr_t *ethernet_hdr = calloc(1, sizeof(ethernet_hdr_t));
    if(!intf){
        intf = getSubnetInterface(node, ip);
        if(!intf){
            printf("Error : %s : No eligible subnet for ARP resolution for Ip-address : %s",node->node_name, ip);
            exit(EXIT_FAILURE);
        }
    }
    layer2_fill_with_broadcast_mac(ethernet_hdr->dst_mac.mac);
    memcpy(ethernet_hdr->src_mac.mac, IF_MAC(intf), sizeof(mac_add_t));
    ethernet_hdr->type = ARP_MSG;
    arp_hdr_t *arp_hdr=(arp_hdr_t*)(ethernet_hdr->payload);
    arp_hdr->hw_type = 1;
    arp_hdr->proto_type = 0x0800;
    arp_hdr->hw_addr_len = sizeof(mac_add_t);
    arp_hdr->proto_addr_len = 4;

    arp_hdr->op_code = ARP_BROAD_REQ;

    memcpy(arp_hdr->src_mac.mac, IF_MAC(intf), sizeof(mac_add_t));

    inet_pton(AF_INET, IF_IP(intf), &arp_hdr->src_ip);
    arp_hdr->src_ip = htonl(arp_hdr->src_ip);

    memset(arp_hdr->dst_mac.mac, 0,  sizeof(mac_add_t));

    inet_pton(AF_INET, ip, &arp_hdr->dst_ip);
    arp_hdr->dst_ip = htonl(arp_hdr->dst_ip);
    
    ethernet_hdr->FCS = 0;

    sendPacket((char *)ethernet_hdr, sizeof(ethernet_hdr_t),intf);
    free(ethernet_hdr);
}

static void
send_arp_reply_msg(ethernet_hdr_t *ethernet_hdr_in,ethernet_hdr_t *ethernet_hdr_reply, interface *oif){

    arp_hdr_t *arp_hdr_in = (arp_hdr_t *)(ethernet_hdr_in->payload);

    memcpy(ethernet_hdr_reply->dst_mac.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    memcpy(ethernet_hdr_reply->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));
    
    ethernet_hdr_reply->type = ARP_MSG;
    
    arp_hdr_t *arp_hdr_reply = (arp_hdr_t *)(ethernet_hdr_reply->payload);
    
    arp_hdr_reply->hw_type = 1;
    arp_hdr_reply->proto_type = 0x0800;
    arp_hdr_reply->hw_addr_len = sizeof(mac_add_t);
    arp_hdr_reply->proto_addr_len = 4;
    
    arp_hdr_reply->op_code = ARP_REPLY;
    memcpy(arp_hdr_reply->src_mac.mac, IF_MAC(oif), sizeof(mac_add_t));

    inet_pton(AF_INET, IF_IP(oif), &arp_hdr_reply->src_ip);
    arp_hdr_reply->src_ip =  htonl(arp_hdr_reply->src_ip);

    memcpy(arp_hdr_reply->dst_mac.mac, arp_hdr_in->src_mac.mac, sizeof(mac_add_t));
    arp_hdr_reply->dst_ip = arp_hdr_in->src_ip;
  
    sendPacket((char *)ethernet_hdr_reply, sizeof(ethernet_hdr_t) + sizeof(arp_hdr_t),oif);

    ethernet_hdr_reply->FCS = 0; /*Not used*/
    free(ethernet_hdr_reply);  
}

static void processArpBroadcastRequest(node_t *node, interface *iif, ethernet_hdr_t *ethernet_hdr){
    printf("ARP Broadcast msg recvd on interface %s of node %s\n",iif->name , iif->att_node->node_name);
    arp_hdr_t *arp_hdr=(arp_hdr_t*)(ethernet_hdr->payload);
    char ip[16];
    unsigned int ip_addr=htonl(arp_hdr->dst_ip);
    inet_ntop(AF_INET,&ip_addr,ip,16);
    ip[15]='\0';
    if(strncmp(IF_IP(iif), ip, 16)){
        printf("%s : ARP Broadcast req msg dropped, Dst IP address did not match",node->node_name );
        return;
    }

    ethernet_hdr_t *ethernet_hdr_reply = calloc(1, sizeof(ethernet_hdr_t) + sizeof(arp_hdr_t));
    send_arp_reply_msg(ethernet_hdr,ethernet_hdr_reply, iif);
}

static void processArpReply(node_t *node, interface *iif,ethernet_hdr_t *ethernet_hdr){
    printf("ARP reply msg recvd on interface %s of node %s\n", iif->name , iif->att_node->node_name);
    arpReplyTableUpdate(ARP_TABLE(node),(arp_hdr_t *)(ethernet_hdr->payload), iif);
}

void layer2FrameRecv(node_t *node, interface *intf,char *pkt, unsigned int pkt_size){
    ethernet_hdr_t *eth_hdr=(ethernet_hdr_t*)(pkt);
    if(eth_hdr->type==ARP_MSG ){
        arp_hdr_t *arp_hdr = (arp_hdr_t *)(eth_hdr->payload);
        if(arp_hdr->op_code==ARP_BROAD_REQ) processArpBroadcastRequest(node, intf, eth_hdr);

        else if(arp_hdr->op_code==ARP_REPLY) processArpReply(node, intf, eth_hdr);
    }
}
                        
