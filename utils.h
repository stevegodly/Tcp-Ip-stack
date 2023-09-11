#ifndef __UTILS__
#define __UTILS__

typedef enum{

  FALSE,
  TRUE
} bool_t;


#define IS_BIT_SET(n, pos)      ((n & (1 << (pos))) != 0)
#define TOGGLE_BIT(n, pos)      (n = n ^ (1 << (pos)))
#define COMPLEMENT(num)         (num = num ^ 0xFFFFFFFF)
#define UNSET_BIT(n, pos)       (n = n & ((1 << pos) ^ 0xFFFFFFFF))
#define SET_BIT(n, pos)     (n = n | 1 << pos)

#define IS_MAC_BROADCAST_ADDR(mac) (strcmp((mac),"255.255.255.255.255.255")==0)

int getMaskValue(char mask);

void apply_mask(char *prefix, char mask, char *str_prefix);

void layer2_fill_with_broadcast_mac(char *mac_array);

void get_abcd_macFormat(long long  int x,char *ip);

void get_abcd_ipFormat(unsigned int x,char *ip);

#endif /* __UTILS__ */
