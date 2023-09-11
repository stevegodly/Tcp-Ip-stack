#ifndef __NET__
#define __NET__

#include "utils.h"
#include <memory.h>

/* Device IDS */
#define L3_ROUTER   (1 << 0)
#define L2_SWITCH   (1 << 1)
#define HUB         (1 << 2)

#define ARP_TABLE(node)   ((node)->node_nw_prop.arp_table)

typedef struct graph_t graph;
typedef struct interface_ interface;
typedef struct node node_t;

typedef struct ip_add_ {
    char ip_addr[16];
} ip_add_t;

typedef struct mac_add_ {
    unsigned char mac[6];
} mac_add_t;

typedef struct arp_table_ arp_table_t;
typedef struct arp_entry_ arp_entry_t;

typedef struct node_nw_prop_{
    unsigned int flags;
    bool_t is_lb_addr_config;
    mac_table_t *mac_table; 
    arp_table_t *arp_table;
    ip_add_t lb_addr; /*loopback address of node*/
} node_nw_prop_t;

typedef enum{

    ACCESS,
    TRUNK,
    L2_MODE_UNKNOWN
} intf_l2_mode_t;

extern void init_arp_table(arp_table_t **arpTable);
extern void init_mac_table(mac_table_t **mac_table);

static inline void init_node_nw_prop(node_nw_prop_t *node_nw_prop){
    node_nw_prop->flags = 0;
    node_nw_prop->is_lb_addr_config = FALSE;
    memset(node_nw_prop->lb_addr.ip_addr, 0, 16);
    init_arp_table(&(node_nw_prop->arp_table));
    nit_mac_table(&(node_nw_prop->mac_table));
}

typedef struct intf_nw_props_ {
    mac_add_t mac_add;   
    intf_l2_mode_t  intf_l2_mode;   /*if IP-address is configured on this interface, then this should be set to UNKNOWN*/
    unsigned int vlans[MAX_VLAN_MEMBERSHIP];    /*If the interface is operating in Trunk mode, it can be a member of these many vlans*/
    bool_t is_ipadd_config_backup;
   
    /*L3 properties*/
    bool_t is_ipadd_config; /*Set to TRUE if ip add is configured, intf operates in L3 mode if ip address is configured on it*/
    ip_add_t ip_add;
    char mask;
} intf_nw_props_t;


static inline void init_intf_nw_prop(intf_nw_props_t *intf_nw_props) {

    memset(intf_nw_props->mac_add.mac,0,sizeof(intf_nw_props->mac_add.mac));
    intf_nw_props->intf_l2_mode = L2_MODE_UNKNOWN;
    memset(intf_nw_props->vlans, 0, sizeof(intf_nw_props->vlans));

    intf_nw_props->is_ipadd_config = FALSE;
    memset(intf_nw_props->ip_add.ip_addr, 0, 16);
    intf_nw_props->mask = 0;
}

void interface_assign_mac_address(interface* interface_t);

/*GET shorthand Macros*/
#define IF_MAC(intf_ptr)   ((intf_ptr)->intf_nw_props.mac_add.mac)
#define IF_IP(intf_ptr)    ((intf_ptr)->intf_nw_props.ip_add.ip_addr)

#define NODE_LO_ADDR(node_ptr) ((node_ptr)->node_nw_prop.lb_addr.ip_addr)
#define NODE_ARP_TABLE(node_ptr)    (node_ptr->node_nw_prop.arp_table)
#define NODE_MAC_TABLE(node_ptr)    (node_ptr->node_nw_prop.mac_table)
#define IF_L2_MODE(intf_ptr)    (intf_ptr->intf_nw_props.intf_l2_mode)
#define IS_INTF_L3_MODE(intf_ptr)   (intf_ptr->intf_nw_props.is_ipadd_config == TRUE)

/*APIs to set Network Node properties*/
bool_t node_set_device_type(node_t *node, unsigned int F);
bool_t node_set_loopback_address(node_t *node, char *ip_addr);
bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask);
bool_t node_unset_intf_ip_address(node_t *node, char *local_if);


/*Dumping Functions to dump network information
 * on nodes and interfaces*/
void dump_nw_graph(graph *graph_t);
void dump_node_nw_props(node_t *node);
void dump_intf_props(interface *interface_t);

interface *node_get_matching_subnet_interface(node_t *node, char *ip_addr);

unsigned int convert_ip_from_str_to_int(char *ip_addr);

void convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer);

interface *getSubnetInterface(node_t *node, char *ip_addr);

#endif /* __NET__ */
