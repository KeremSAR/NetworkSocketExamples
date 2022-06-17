/*Kerem SARI Tiny implemantation of ARP request and ARP REPLY*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <unistd.h>
struct arp_request_packet {
	unsigned short hardware_type ; // hardware type
	unsigned short protocol_type ; // protocol type
	unsigned char hardware_address_length ; // hardware address length
	unsigned char protocol_address_length ; // Protocol adress length
	unsigned short operation ; // Operation
	unsigned char source_mac [6]; // Source hardware address
	unsigned char source_ip [4]; // Source IP address
	unsigned char destination_mac [6]; // Target hardware address
	unsigned char destination_ip [4]; // Target IP address
	char padding [18]; // Padding , ARP - Requests are small
};

int main(int argc, char *argv[])
{
    unsigned char buffer[42];
  
    struct arp_request_packet *arp_pkt= (struct arp_request_packet *)(buffer + 14); // starting struct after 14 byte
    struct in_addr in;
    struct in_addr dest;
    struct sockaddr addr;
    const unsigned char *src_ip ;   // source ip address
    const unsigned char *dest_ip;   // destination ip address
     unsigned char source_mac[6];
    //unsigned char source_mac[6] = {0x2c,0xf0,0x5d,0xb8,0x63,0x07}; //2C-F0-5D-B8-63-07
    if(strcmp(argv[1],"-s")==0){
        src_ip = argv[2];    				
     }
    if(strcmp(argv[3],"-d")==0){				
	 dest_ip = argv[4];	
	}
    if(strcmp(argv[5],"-m")==0){
	sscanf(argv[6],"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&source_mac[0],&source_mac[1]	//interpret as mac
					    ,&source_mac[2],&source_mac[3]
					    ,&source_mac[4],&source_mac[5]);
				    	
		}
    unsigned char dest_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};  
    char recvarp[42] = {0};
    char sendarp[42] = {0};
    int sock_fd,i;
    char type[2] = {0x08, 0x06};
    
    /*filling first 3 section for arp request*/
    memcpy(buffer,&dest_mac,sizeof(dest_mac));  // dest mac adress
    memcpy(buffer+6,&source_mac,sizeof(source_mac)); // source mac address
    memcpy(buffer+12, &type, sizeof(type)); // 0x0806
    
    /*setting arp packet format to send*/
   
    arp_pkt->hardware_type = htons(0x1);
    arp_pkt->protocol_type = htons(0x800);
    arp_pkt->hardware_address_length =6;  // eth mac
    arp_pkt->protocol_address_length =4;   // ip4
    arp_pkt->operation = htons(0x0001);      //0x0001 for ARP Request
    /*setting source mac address*/
    memcpy(arp_pkt->source_mac,&source_mac,6);
   /*setting ip4 source address*/
    in.s_addr = inet_addr(src_ip);  // converts ip to network format
    memcpy(arp_pkt->source_ip,&in.s_addr,4);
    /*destination mac address*/
    memcpy(arp_pkt->destination_mac,&dest_mac,6);
    /*ip4 destination address*/
    dest.s_addr = inet_addr(dest_ip);		
    memcpy(arp_pkt->destination_ip,&dest.s_addr,4);	
   /*fill the packet with 0 to complete 60 byte*/
   bzero(arp_pkt->padding,18);
    
    if ((sock_fd = socket(AF_INET, SOCK_PACKET, htons(0x0003))) < 0) {
        printf("Could not create socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    strncpy(addr.sa_data, "ens33", sizeof(addr.sa_data));  // setting the interface ,my wm's inteface name is ens33
    socklen_t len = sizeof(addr);


   if (sendto(sock_fd, buffer, 42, 0, &addr, len) == 42) {
    	printf("Source ARP address: ");
    	for(i = 0; i < 42;i++)
      	{
        	printf("%02x ",buffer[i]);
      	}
    	printf("\n\n");
   	} 
   	else {
    		printf("could not send the arp packet");
    		exit(1);
  	 }

   if (recvfrom(sock_fd, recvarp, 42, 0, &addr, &len) == 42) {
        memcpy(dest_mac, recvarp+6 , 6); // destination mac address
	printf("Destination ARP address: ");
	for(i = 0; i < sizeof(recvarp);i++)
	      {
	        printf("%02x ",(unsigned char)recvarp[i]);
	      }
	printf("\n\n");
	printf("Destination MAC address: ");
	for(i = 0; i < sizeof(dest_mac);i++)
	      {
	        printf("%02x ",(unsigned char)dest_mac[i]);
	      }
	printf("\n");
     
   }
    close(sock_fd);
    return 0;
}
