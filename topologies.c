#include "graph.h"

extern void network_start_packet_thread(graph *topo);

graph *buildTopo(){
    graph* graph_t=createNewGraph("1st Graph");

    node_t *n1=createGraphNode(graph_t,"Node1");
    node_t *n2=createGraphNode(graph_t,"Node2");
    node_t *n3=createGraphNode(graph_t,"Node3");

    createLink(n1,n2,"eth:0/1","eth:1/0",1);
    createLink(n2,n3,"eth:1/1","eth:2/1",1);
    createLink(n3,n1,"eth:2/0","eth:0/0",1);

    node_set_device_type(n1, L3_ROUTER);
    node_set_loopback_address(n1, "122.1.1.0");
    node_set_intf_ip_address(n1, "eth:0/1", "40.1.1.1", 24);
    node_set_intf_ip_address(n1, "eth:0/0", "20.1.1.1", 24);

    node_set_device_type(n2, L3_ROUTER);
    node_set_loopback_address(n2, "122.1.1.1");
    node_set_intf_ip_address(n2, "eth:1/0", "40.1.1.2", 24);
    node_set_intf_ip_address(n2, "eth:1/1", "30.1.1.1", 24);

    node_set_device_type(n3, L3_ROUTER);
    node_set_loopback_address(n3, "122.1.1.2");
    node_set_intf_ip_address(n3, "eth:2/0", "20.1.1.2", 24);
    node_set_intf_ip_address(n3, "eth:2/1", "30.1.1.2", 24);
    
    network_start_packet_thread(graph_t);
    return graph_t;
}