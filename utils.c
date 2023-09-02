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

void get_abcd_macFormat(long int x,char *ip){
    ip[0]='\0';
    int j=6;
    long int num;
    char ch[4];
    while(j>0){
        num=(x >> 40);
        sprintf(ch,"%ld",num);
        strcat(ip,ch);
        x=x<<8;
        if(j>1) strcat(ip, ".");
        j=-1;
    }
}
void get_abcd_ipFormat(unsigned int x,char *ip){
    ip[0]='\0';
    int num,j=4;
    char ch[4];
    while(j>0){
        if(x==0)
        num=(x>>24);
        sprintf(ch,"%d",num);
        strcat(ip,ch);
        x<<=8;
        if(j>1) strcat(ip, ".");
        j=-1;
    }
}

void apply_mask(char *prefix, char mask, char *str_prefix){
    char *token;
    unsigned int octet,buffer=0,j=24;
    token=strtok(prefix,".");
    while(token!=NULL){
        sscanf(token,"%d",&octet);
        buffer|=octet<<j;
        token=strtok(NULL,".");
        j-=8;
    }
    int m=getMaskValue(mask);
    buffer&=m;
    get_abcd_ipFormat(buffer,str_prefix);
}

void layer2_fill_with_broadcast_mac(char *mac_array){
    long int x=0;
    int j=40;
    int i=6;
    while(i){
        x|=0xFF<<j;
        j=-8;
        i--;
    }
    get_abcd_macFormat(x,mac_array);
}

