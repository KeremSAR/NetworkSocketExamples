#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull+36000  // 60*60*10  GMT-6
#define NTP_IP  "132.163.96.1" // https://tf.nist.gov/tf-cgi/servers.cgi NIST, Boulder, Colorado NTP server
typedef struct
{
  uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.

  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.

  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

}ntp_packet;    

int main()
{
   int PORT = 123;
   int sock_fd,conn;
   struct sockaddr_in addr;
   ntp_packet my_time_packet ;
   memset(&my_time_packet , 0 , sizeof ( ntp_packet )); // set all the fields to zero
   *(( char *)&my_time_packet + 0 ) = 0x1b ; 
   

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        printf("Could not create socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET; 
    inet_pton(AF_INET,NTP_IP, &(addr.sin_addr.s_addr));
    addr.sin_port=htons(PORT);
   
  if (connect(sock_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0 ){
  	printf( "ERROR connecting" );
  	exit(1);
  	}

  conn = write( sock_fd, (char *)&my_time_packet, sizeof(ntp_packet) );

  if ( conn < 0 ){
  	printf( "ERROR writing" );
  	exit(1);
    }	
   conn = read( sock_fd, ( char* )&my_time_packet, sizeof(ntp_packet));

  if ( conn < 0 ){
  	printf( "ERROR reading" );
  	exit(1);
    }	
    my_time_packet.txTm_s = ntohl (my_time_packet.txTm_s );
    my_time_packet.txTm_f = ntohl (my_time_packet.txTm_f);
    time_t txTm = ( time_t ) (my_time_packet.txTm_s - NTP_TIMESTAMP_DELTA);
	
   printf( "Time: %s", ctime( ( const time_t* ) &txTm ) ); // convert to human readable type
  
    close(sock_fd); // close socket
    return 0;
}

