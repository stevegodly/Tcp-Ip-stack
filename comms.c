#include "graph.h"
#include "comms.h"
#include "gluethread/glthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


static unsigned int udpPort =4000;

static unsigned int nextPortNo(){
    return udpPort++;
}

void init_udp_socket(node_t *node){
    node->uPort=nextPortNo();
    int fd;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(node->uPort);
	servaddr.sin_addr.s_addr = INADDR_ANY;
    node->sockFd=fd;
    if (bind(node->sockFd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 ){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

}

extern void layer2FrameRecv(node_t *node, interface *interf,char *pkt, unsigned int pkt_size);

int recvPckt(node_t *node,char *buff,int size){
    char *intfName=buff;
    interface *intf=getIntfByName(node,intfName);
    if(!intf){
        printf("Error : Pkt recvd on unknown interface %s on node %s\n",intf->name, node->node_name);
        return 0;
    }
    layer2FrameRecv(node,intf,buff+NAME_SIZE,size-NAME_SIZE );
    return 1;
}

static void *nw_start_packet(void *arg){
    graph *topo=arg;
    node_t *node;
    glthread_t *curr;
    fd_set bckpfds;
    char *buffer=calloc(1,2048);
    int maxSockfd=0,recvBytes=0,len,x;
    FD_ZERO(&bckpfds);
    ITERATE_GLTHREAD_BEGIN(&topo->node_list,curr){
        node=graph_glue_to_node(curr);
        if(!node->sockFd) continue;
        if(node->sockFd>maxSockfd) maxSockfd=node->sockFd;
        FD_SET(node->sockFd,&bckpfds);
    }
    ITERATE_GLTHREAD_END(&topo->node_list,curr);
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    len = sizeof(cliaddr);
    fd_set cpyfds;
    FD_ZERO(&cpyfds);
    while(1){
        memcpy(&cpyfds, &bckpfds, sizeof(fd_set));
        select(maxSockfd+1,&cpyfds, NULL, NULL, NULL);
        ITERATE_GLTHREAD_BEGIN(&topo->node_list,curr){
            node=graph_glue_to_node(curr);
            if(FD_ISSET(node->sockFd,&cpyfds)){
                recvBytes=recvfrom(node->sockFd,(char *)buffer,1000,0,( struct sockaddr *) &cliaddr,&len);
                x=recvPckt(node,buffer,recvBytes);
            }
        }    
        ITERATE_GLTHREAD_END(&topo->node_list,curr);    
    }
}

void network_start_packet_thread(graph *topo){
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t id;
    pthread_create(&id, &attr,(void *)nw_start_packet, (void*)topo);
} 

void sendPacket(char *pckt,int pcktSize,interface *interface_t){
    node_t *node=getNbrNode(interface_t);
    int dstPort = node->uPort,fd;
    interface *dstIntf=(interface_t==&interface_t->link->intf1?&interface_t->link->intf2:&interface_t->link->intf1);
    
    int len=pcktSize+NAME_SIZE;
    char *auxPckt=calloc(1,len);
    strncpy(auxPckt, dstIntf->name,NAME_SIZE);
    auxPckt[NAME_SIZE]='\0';
    memcpy(auxPckt+NAME_SIZE,pckt,pcktSize);
    auxPckt[len-1]='\0';
    if ( (fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in cliaddr;
    struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1");
    cliaddr.sin_family=AF_INET;
    cliaddr.sin_port=htons(dstPort);
    cliaddr.sin_addr=*((struct in_addr *)host->h_addr_list[0]);

    int rc;
    if((rc=sendto(fd,auxPckt,len,MSG_CONFIRM, (const struct sockaddr *) &cliaddr,sizeof(cliaddr)))< 0 ){
        perror("packet delivery failed");
		exit(EXIT_FAILURE);
    }
    close(fd);
    free(auxPckt);
} 

int send_pkt_flood(node_t *node, char *pkt, unsigned int len){
    unsigned int i = 0;
    interface *intf;

    for( ; i < MAX_INTF_PER_NODE; i++){
        intf = node->intf[i];
        if(!intf) return 0;
        sendPacket(pkt, len, intf);
    }
    return 0;
}

