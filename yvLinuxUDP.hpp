#ifndef YVUDP
#define YVUDP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

class yvUDP {
private:
	int sockfd;
	void (*recvfunc)(const char*,in_port_t,size_t,const char*);
	pthread_t tid;
	static void* looprecv(void* param) {
		yvUDP *th=(yvUDP*)param;
		if (th->recvfunc)
			while (1) {
				char buf[10000];
				sockaddr_in sai;
				unsigned int sail=sizeof(sai);
				size_t len=recvfrom(th->sockfd,buf,9990,0,(sockaddr*)&sai,&sail);
				th->recvfunc(inet_ntoa(sai.sin_addr),ntohs(sai.sin_port),len,buf);
			}
        return NULL;
	}
public:
	yvUDP(in_port_t Port,void (*ReceiveFunction)(const char* RemoteIP,in_port_t RemotePort,size_t ContentLength,const char* Content)) {
		recvfunc=ReceiveFunction;
		sockfd=socket(AF_INET,SOCK_DGRAM,0);
		sockaddr_in sai;
		memset(&sai,0,sizeof(sai));
		sai.sin_addr.s_addr=htons(INADDR_ANY);
		sai.sin_port=htons(Port);
		sai.sin_family=AF_INET;
		bind(sockfd,(sockaddr*)&sai,sizeof(sai));
		pthread_create(&tid,0,looprecv,this);
	}
	void Send(const char* TargetHost,in_port_t TargetPort,size_t ContentLength,const char* Content) {
		sockaddr_in sai;
		memset(&sai,0,sizeof(sai));
		hostent *h=gethostbyname(TargetHost);
		char TargetIP[20]={};
		inet_ntop(h->h_addrtype,h->h_addr,TargetIP,sizeof(TargetIP));
		inet_aton(TargetIP,&sai.sin_addr);
		sai.sin_port=htons(TargetPort);
		sai.sin_family=AF_INET;
		sendto(sockfd,Content,ContentLength,0,(sockaddr*)&sai,sizeof(sai));
	}
	void Close() {
		pthread_cancel(tid);
		close(sockfd);
	}
};

#endif

