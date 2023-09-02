#include "graph.h"
#include "comms.h"
#include "string.h"
#include "CommandParser/libcli.h"


extern graph *buildTopo();
graph *topo=NULL;
extern void nw_init_cli();

int main(int argc, char **argv){
    nw_init_cli();
    topo = buildTopo();
    sleep(3);
    node_t *node=getNodeByName(topo,"Node1");
    interface *intf=getIntfByName(node,"eth:1");
    char mssg[]="hello\0";
    sendPacket(mssg,strlen(mssg),intf);
    start_shell();
    return 0;
}