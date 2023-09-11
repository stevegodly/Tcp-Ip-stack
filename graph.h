#include "gluethread/glthread.h"
#include <stdlib.h>
#include <string.h>
#include "net.h"

#define NODE_NAME_SIZE 16
#define NAME_SIZE 16
#define MAX_INTF_PER_NODE 10

typedef struct node node_t;
typedef struct link link_t;

typedef struct interface_{
    char name[NAME_SIZE];
    struct node *att_node;
    intf_nw_props_t intf_nw_props;
    link_t *link;
} interface;

struct link{
    interface intf1;
    interface intf2;
    unsigned int cost;
};

struct node{
    char node_name[NODE_NAME_SIZE];
    interface *intf[MAX_INTF_PER_NODE];
    unsigned int uPort;
    int sockFd;
    node_nw_prop_t node_nw_prop;
    glthread_t graph_glue;
};

GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);

typedef struct graph_t{
    char topology_name[32];
    glthread_t node_list;
}graph;

graph *createNewGraph(char *buffer);

node_t *createGraphNode(graph *graphT,char *nodeName);

void addNextNode(glthread_t *curr_glthread,glthread_t *new_glthread);

void createLink(node_t *node1,node_t *node2,char *from_name,char *to_name,unsigned int cost);

static inline node_t *getNbrNode(interface *interface_t){
    interface * intf=(&(interface_t->link->intf1)==interface_t?&(interface_t->link->intf2):&(interface_t->link->intf1));
    return intf->att_node;
}

static inline int nodeIntfAvail(node_t *node){
    int i ;
    for( i = 0 ; i < MAX_INTF_PER_NODE; i++){
        if(node->intf[i])
            continue;
        return i;
    }
    return -1;
}

static inline interface *getIntfByName(node_t *node, char *if_name){
    int i=0;
    for(;i<MAX_INTF_PER_NODE;i++){
        if(!node->intf[i]) return NULL;
        if(!strcmp(node->intf[i]->name,if_name)) return node->intf[i];
    }
    return NULL;
}

static inline node_t *getNodeByName(graph *topo, const char *node_name){
    if (topo == NULL || node_name == NULL) {
        return NULL;
    }

    node_t *node=NULL;
    glthread_t *curr=NULL;

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){

        node = graph_glue_to_node(curr);
        if(strncmp(node->node_name, node_name, strlen(node_name)) == 0)
            return node;
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);
    return NULL;

}
void dumpGraph(graph *graph_t);

void dumpNode(node_t *node);

void dumpInterface(interface *interface_t);