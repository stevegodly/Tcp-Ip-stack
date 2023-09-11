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

    start_shell();

    return 0;
}