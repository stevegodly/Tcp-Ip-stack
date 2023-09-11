#include "graph.h"
#include "net.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include<assert.h>
#include <stdio.h>


static unsigned int hash_code(void *ptr, unsigned int size){
    unsigned int value=0, i =0;
    char *str = (char*)ptr;
    while(i < size)
    {
        value += *str;
        value*=97;
        str++;
        i++;
    }
    return value;
}

/*Heuristics, Assign a unique mac address to interface_t*/
void interface_assign_mac_address(interface *interface_t){
    unsigned int hash_code_val = hash_code(interface_t, sizeof(interface_t));
    memset(IF_MAC(interface_t), 0, sizeof(IF_MAC(interface_t)));
    memcpy(IF_MAC(interface_t), (char *)&hash_code_val, sizeof(unsigned int));
}

bool_t node_set_device_type(node_t *node, unsigned int F){

    SET_BIT(node->node_nw_prop.flags, F);
    return TRUE;
}

bool_t node_set_loopback_address(node_t *node, char *ip_addr){

    assert(ip_addr);

    if(IS_BIT_SET(node->node_nw_prop.flags, HUB))
        assert(0); /*Wrong Config : A HUB do not have any IP addresses*/
    if(!IS_BIT_SET(node->node_nw_prop.flags, L3_ROUTER))
        assert(0); /*You must enable L3 routing on device first*/

    node->node_nw_prop.is_lb_addr_config = TRUE;
    strncpy(NODE_LO_ADDR(node), ip_addr, 16);
    NODE_LO_ADDR(node)[15] = '\0';

    return TRUE;
}

bool_t node_set_intf_ip_address(node_t *node, char *local_if,char *ip_addr, char mask) {

    interface *interface_t = getIntfByName(node, local_if);
    if(!interface_t) assert(0);

    strncpy(IF_IP(interface_t), ip_addr, 16);
    IF_IP(interface_t)[16] = '\0';
    interface_t->intf_nw_props.mask = mask;
    interface_t->intf_nw_props.is_ipadd_config = TRUE;
    return TRUE;
}

bool_t node_unset_intf_ip_address(node_t *node, char *local_if){

    return TRUE;
}

void dump_node_nw_props(node_t *node){

    printf("\nNode Name = %s\n", node->node_name);
    printf("\t node flags : %u", node->node_nw_prop.flags);
    if(node->node_nw_prop.is_lb_addr_config){
        printf("\t  lo addr : %s/32\n", NODE_LO_ADDR(node));
    }
}

void dump_intf_props(interface *interface_t){

    dumpInterface(interface_t);

    if(interface_t->intf_nw_props.is_ipadd_config){
        printf("\t IP Addr = %s/%u", IF_IP(interface_t), interface_t->intf_nw_props.mask);
    }
    else{
        printf("\t IP Addr = %s/%u", "Nil", 0);
    }
    printf("\t MAC : %s:\n",IF_MAC(interface_t));
}

void dump_nw_graph(graph *graph_t){

    node_t *node;
    glthread_t *curr;
    interface *interface_t;
    unsigned int i;

    printf("Topology Name = %s\n", graph_t->topology_name);

    ITERATE_GLTHREAD_BEGIN(&graph_t->node_list, curr){

        node = graph_glue_to_node(curr);
        dump_node_nw_props(node);
        for( i = 0; i < MAX_INTF_PER_NODE; i++){
            interface_t = node->intf[i];
            if(!interface_t) break;
            dump_intf_props(interface_t);
        }
    } ITERATE_GLTHREAD_END(&graph_t->node_list, curr);
}       
interface *node_get_matching_subnet_interface(node_t *node, char *ip_addr){
    interface *interface_t;
    char *ipConfigured=NULL;
    char *ipMasked=NULL;
    for(int i = 0; i < MAX_INTF_PER_NODE; i++){
        interface_t = node->intf[i];
        char *ip=(interface_t->intf_nw_props.ip_add.ip_addr);
        char mask=interface_t->intf_nw_props.mask;
        apply_mask(ip_addr,mask,ipMasked);
        apply_mask(ip,mask,ipConfigured);
        if(!strcmp(ipConfigured,ipMasked)){
            return interface_t;
        }
    }
    return NULL;
}               

unsigned int convert_ip_from_str_to_int(char *ip_addr){
    char *token;
    unsigned int octet,buffer=0,j=24;
    token=strtok(ip_addr,".");
    while(token!=NULL){
        sscanf(token,"%d",&octet);
        buffer|=octet<<j;
        token=strtok(NULL,".");
        j-=8;
    }
    return buffer;
}

void convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer){
    char ch[4]="\0";
    int j=24;
    while(!ip_addr){
        sprintf(ch,"%d",ip_addr>>j);
        strcat(output_buffer,ch);
        strcat(output_buffer,".");
        ip_addr=ip_addr<<8;
    }
}

char *pcktShiftRight(char *pckt,unsigned int pcktSize,int bufferSize){
    memcpy(pckt + (bufferSize - pcktSize), pckt, pcktSize);
    memset(pckt, 0, pcktSize);
    return pckt + (bufferSize - pcktSize);
}

interface *getSubnetInterface(node_t *node, char *ip_addr){
    char ip[16];
    char *ipIntf=NULL;
    char intfIp[16];
    memset(ip,0,16);
    memset(intfIp,0,16);
    for(int i=0;i<MAX_INTF_PER_NODE;i++){
        ipIntf=node->intf[i]->intf_nw_props.ip_add.ip_addr;
        apply_mask(ip_addr,node->intf[i]->intf_nw_props.mask,ip);
        apply_mask(ipIntf,node->intf[i]->intf_nw_props.mask,intfIp);
        if(!strcmp(ip,intfIp)) return node->intf[i];
    }
    return NULL;
}