#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>


struct ethheader{
	u_char ether_dhost[6];
	u_char ether_shost[6];
	u_short ether_type;
};

struct ipheader{
	unsigned char iph_ihl:4,
		      iph_ver:4;
	unsigned char iph_tos;
	unsigned short int iph_len;
	unsigned short int iph_ident;
	unsigned short int iph_flag:3,
		       iph_offset:13;
	unsigned char iph_ttl;
	unsigned char iph_protocol;
	unsigned short int iph_chksum;
	struct in_addr iph_sourceip;
	struct in_addr iph_destip;
};

struct tcpheader{
	u_short tcp_sport;
	u_short tcp_dport;
	u_int tcp_seq;
	u_int tcp_ack;
	u_char tcp_offx2;
#define TH_OFF(th)	(((th)->tcp_offx2 & 0xf0) >> 4)
	u_char tcp_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS	(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short tcp_win;
	u_short tcp_sum;
	u_short tcp_urp;
};

void got_pocket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
  struct ethheader *eth = (struct ethheader *)packet;
  if (ntohs(eth->ether_type) == 0x0800) {
	printf("Ethernet header: \n");
	printf("       From: %02x:%02x:%02x:%02x:%02x:%02x\n",
		   eth->ether_shost[0],
		   eth->ether_shost[1],
		   eth->ether_shost[2],
		   eth->ether_shost[3],
		   eth->ether_shost[4],
		   eth->ether_shost[5]);
	printf("         To: %02x:%02x:%02x:%02x:%02x:%02x\n",
		   eth->ether_dhost[0],
		   eth->ether_dhost[1],
		   eth->ether_dhost[2],
		   eth->ether_dhost[3],
		   eth->ether_dhost[4],
		   eth->ether_dhost[5]);
	

	struct ipheader * ip = (struct ipheader *) (packet + sizeof(struct ethheader)); 
	printf("IP Header: \n");
    printf("       From: %s\n", inet_ntoa(ip->iph_sourceip));   
    printf("         To: %s\n", inet_ntoa(ip->iph_destip));

	printf("TCP Header: \n");
	printf("       Source Port: %d\n", ntohs(tcp->tcp_sport));
	printf("         Dest Port: %d\n", ntohs(tcp->tcp_dport));
  }
	
	
}
int main()
{
	pcap_t *handle;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;
	char filter_exp[] = "tcp";
	bpf_u_int32 net;

	handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

	pcap_compile(handle, &fp, filter_exp, 0, net);

	if (pcap_setfilter(handle, &fp) != 0) {
		pcap_perror(handle, "Error setting filter");
		exit(EXIT_FAILURE);
	}

	pcap_loop(handle, -1, got_pocket, NULL);

	pcap_close(handle);

	return 0;
}
