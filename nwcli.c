#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"
#include "cmdcodes.h"
#include "graph.h"
#include <stdio.h>

extern graph *topo;

typedef struct arp_table_ arp_table_t;



static int nodeHandler(param_t *param,ser_buff_t *buff,op_mode enableOrDisable){
    char *node_name=NULL;   
    char *ip=NULL;
    int CMDCODE =EXTRACT_CMD_CODE(buff);
    tlv_struct_t *tlv = NULL;
    TLV_LOOP_BEGIN(buff, tlv){ 
        if(strncmp(tlv->leaf_id, "nodeName", strlen("nodeName")) == 0){
            node_name = tlv->value;                                        
        }
    } TLV_LOOP_END;
    node_t *node = getNodeByName(topo, node_name);

    if(CMDCODE==CMDODE_SHOW_NODE) dumpNode(node); 

    else if(CMDCODE==CMDODE_SHOW_ARP) dumpTable(node->node_nw_prop.arp_table);
    return 0;
}

static int validateNodeName(char *value){

    printf("%s() is called with value = %s\n", __FUNCTION__, value);
    return VALIDATION_SUCCESS;
}

static int arpHandler(param_t *param,ser_buff_t *buff,op_mode enableOrDisable){
    char *ip;
    tlv_struct_t *tlv = NULL;
    TLV_LOOP_BEGIN(buff, tlv){ 
        if(strncmp(tlv->leaf_id, "ip_address", strlen("ip_address")) == 0){
            ip = tlv->value;                                        
        }
    } TLV_LOOP_END;
    
}

static int show_nw_topology_handler(param_t *param,ser_buff_t *buff,op_mode enableOrDisable){
    int CMDCODE=-1;
    CMDCODE =EXTRACT_CMD_CODE(buff);

    if(CMDCODE==CMDCODE_SHOW_NW_TOPOLOGY){
        dump_nw_graph(topo);        
    }
    return 0;
}

void nw_init_cli(){

    init_libcli();

    param_t *show =libcli_get_show_hook();
    param_t *debug=libcli_get_debug_hook();
    param_t *config=libcli_get_config_hook();
    param_t *run =libcli_get_run_hook();
    param_t *debug_show =libcli_get_debug_show_hook();
    param_t *root=libcli_get_root();

    {
        static param_t topology;
        init_param(&topology,CMD, "topology",show_nw_topology_handler,0,INVALID,0,"Dump Complete Network Topology");
        libcli_register_param(show,&topology);
        set_param_cmd_code(&topology,CMDCODE_SHOW_NW_TOPOLOGY);
    }
    {
        static param_t node;
        init_param(&node,CMD,"Node",0,0,INVALID,0,"NODE:HELP");
        libcli_register_param(show,&node);
        {
            static param_t node_name;
            init_param(&node_name,LEAF,0, nodeHandler,validateNodeName, STRING,"nodeName", "Help : Node name");
            libcli_register_param(&node,&node_name);
            set_param_cmd_code(&node_name,CMDODE_SHOW_NODE);
            {
                static param_t arp;
                init_param(&arp,CMD,"Arp",nodeHandler,0,INVALID,0,"ARP:HELP");
                libcli_register_param(&node_name,&arp);
                set_param_cmd_code(&node_name,CMDODE_SHOW_ARP);
            }
        } 

    }
    {
        static param_t node;
        init_param(&node,CMD,"Node",0,0,INVALID,0,"NODE:HELP");
        libcli_register_param(run,&node);
        {
            static param_t resolve_arp;
            init_param(&resolve_arp,CMD,"resolve-arp",0,0,INVALID,0,"ARP:HELP");
            libcli_register_param(&node,&resolve_arp);
            {
                static param_t ip;
                init_param(&ip,LEAF,0, arpHandler,0, STRING,"ip_address", "Help : Node name");
                 libcli_register_param(&resolve_arp,&ip);
            }
        }
        
    }
    support_cmd_negation(config);
}