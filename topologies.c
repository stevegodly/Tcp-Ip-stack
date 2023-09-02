#include "graph.h"

extern void network_start_packet_thread(graph *topo);

graph *buildTopo(){
    graph* graph_t=createNewGraph("1st Graph");

    node_t *n1=createGraphNode(graph_t,"Node1");
    node_t *n2=createGraphNode(graph_t,"Node2");
    node_t *n3=createGraphNode(graph_t,"Node3");

    createLink(n1,n2,"eth:1","eth:2",1);
    createLink(n1,n3,"eth:1","eth:3",1);
    createLink(n2,n3,"eth:2","eth:3",1);

    
    network_start_packet_thread(graph_t);
    return graph_t;
}