#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getMaskValue(char mask){
    int x = 0xFFFFFFFF;
    char trailBits=32-mask;
    for (int i=0;i<trailBits;i++){
        UNSET_BIT(x,i);
    }
    return x;
}

void get_abcd_macFormat(long long int x,char *ip){
    ip[0]='\0';
    int j=6;
    long long int num;
    char ch[4];
    while(j>0){
        num=(x >> 40);
        sprintf(ch,"%lld",num);
        strcat(ip,ch);
        x=x<<8;
        if(j>1) strcat(ip, ".");
        j-=1;
    }
}
void get_abcd_ipFormat(unsigned int x,char *ip){
    int num,j=4;
    char ch[4];
    while(j>0){
        num=(x>>24);
        sprintf(ch,"%d",num);
        strcat(ip,ch);
        x<<=8;
        if(j>1) strcat(ip, ".");
        j-=1;
    }
    ip[15]='\0';
}

void apply_mask(char *prefix, char mask, char *str_prefix){
    char *token;
    unsigned int octet,buffer=0,j=24;
    char *duplicate = strdup(prefix);
    token=strtok(duplicate,".");
    while(token!=NULL){
        sscanf(token,"%d",&octet);
        buffer|=octet<<j;
        token=strtok(NULL,".");
        j-=8;
    }
    int m=getMaskValue(mask);
    buffer&=m;
    str_prefix[15] = '\0';
    get_abcd_ipFormat(buffer,str_prefix);
}

void layer2_fill_with_broadcast_mac(char *mac_array){
    mac_array[0] = 0xFF;
    mac_array[1] = 0xFF;
    mac_array[2] = 0xFF;
    mac_array[3] = 0xFF;
    mac_array[4] = 0xFF;
    mac_array[5] = 0xFF;
}

