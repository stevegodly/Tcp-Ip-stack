#include "graph.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

extern void init_udp_socket(node_t *node);

graph *createNewGraph(char *buffer){
    graph *topo=malloc(sizeof(graph));
    strcpy(topo->topology_name, buffer);
    topo->topology_name[31]='\0';
    initGlthread(&topo->node_list);
    return topo;
}

node_t *createGraphNode(graph *graphT,char *nodeName){
    node_t *node=malloc(sizeof(node_t));
    strcpy(node->node_name, nodeName);
    node->node_name[NODE_NAME_SIZE-1]='\0';
    init_udp_socket(node);
    initGlthread(&node->graph_glue);
    addNextNode(&graphT->node_list,&node->graph_glue);
    return node;
}

void addNextNode(glthread_t *curr_glthread,glthread_t *new_glthread){
    if (curr_glthread == NULL) return;
    if(!curr_glthread->right){
        curr_glthread->right= new_glthread;
        new_glthread->left = curr_glthread;
        return;
    }
    glthread_t *temp=curr_glthread->right;
    curr_glthread->right=new_glthread;
    new_glthread->left=curr_glthread;
    new_glthread->right=temp;
    temp->left=new_glthread;
}

void createLink(node_t *node1,node_t *node2,char *from_name,char *to_name,unsigned int cost){
    link_t *link=malloc(sizeof(link_t));
    strncpy(link->intf1.name,from_name,NAME_SIZE);
    link->intf1.name[NAME_SIZE-1]='\0';
    strncpy(link->intf2.name,to_name,NAME_SIZE);
    link->intf2.name[NAME_SIZE-1]='\0';
    
    link->intf1.link=link;
    link->intf2.link=link;
    
    link->intf1.att_node=node1;
    link->intf2.att_node=node2;

    link->cost = cost;

    int empty_slot=nodeIntfAvail(node1);
    node1->intf[empty_slot]=&link->intf1;
    empty_slot=nodeIntfAvail(node2);
    node2->intf[empty_slot]=&link->intf2;
}

void dumpGraph(graph *graph_t){
    node_t *node;
    glthread_t *curr;
    printf("Topology Name = %s\n", graph_t->topology_name);

    ITERATE_GLTHREAD_BEGIN(&graph_t->node_list,curr){
        
        node = graph_glue_to_node(curr);
        dumpNode(node);    
    } 
    ITERATE_GLTHREAD_END(&graph_t->node_list,curr)
}

void dumpNode(node_t *node){
    unsigned int i = 0;
    interface *intf;
    printf("Node Name = %s : \n", node->node_name);
    for( ; i < MAX_INTF_PER_NODE; i++){
        intf = node->intf[i];
        if(!intf) break;
        dumpInterface(intf);
    }
}

void dumpInterface(interface *interface_t){
   link_t *link = interface_t->link;
   node_t *nbr_node = getNbrNode(interface_t);
   printf(" Local Node : %s, Interface Name = %s, Nbr Node %s, cost = %u\n",interface_t->att_node->node_name,interface_t ->name, nbr_node->node_name, link->cost);
}            
