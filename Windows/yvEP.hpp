#ifndef YVEP
#define YVEP

#include <cstring>
#include "yvRSA.hpp"
#include "yvWindowsUDP.hpp"

namespace yvEP {
	yvUDP *udp;
	yvRSA rsa,rsa2;
	char thost[100]={};
	in_port_t tport;
	void (*recvfunc)(const char*,in_port_t,size_t,const char*);
	void (*keyreq)(const char*,in_port_t);
	void (*keyrecv)(const char*,in_port_t);
    static void RecvE(const char* RemoteIP,in_port_t RemotePort,size_t,const char* Content) {
		if (*Content=='k') {
			char msg[300]={'a'};
			memcpy(msg+1,rsa.GetPubKey(),270);
			udp->Send(RemoteIP,RemotePort,272,msg);
			if (keyreq)
				keyreq(RemoteIP,RemotePort);
		}
		else if (*Content=='a') {
			rsa2.SetPubKey((unsigned char*)(Content+1));
			strcpy(thost,RemoteIP);
			tport=RemotePort;
			if (keyrecv)
				keyrecv(RemoteIP,RemotePort);
		}
		else if (*Content=='m'&&recvfunc) {
			char *msg=rsa.Decrypt((const unsigned char*)(Content+1));
			recvfunc(RemoteIP,RemotePort,strlen(msg),msg);
		}
	}
	void Init(in_port_t Port,void (*ReceiveFunction)(const char* RemoteIP,in_port_t RemotePort,size_t ContentLength,const char* Content),void (*KeyRequest)(const char* RemoteIP,in_port_t RemotePort),void (*KeyReceive)(const char* RemoteIP,in_port_t RemotePort)) {
		udp=new yvUDP(Port,RecvE);
		rsa.GenKey();
		recvfunc=ReceiveFunction;
		keyreq=KeyRequest;
		keyrecv=KeyReceive;
	}
	bool Turn(const char* TargetHost,in_port_t TargetPort) {
		hostent *h=gethostbyname(TargetHost);
		char TargetIP[20]={};
		in_addr loc;
		memcpy(&loc,*(h->h_addr_list),sizeof(in_addr));
		strcpy(TargetIP,inet_ntoa(loc));
		tport=0;
		udp->Send(TargetIP,TargetPort,2,"k");
		clock_t c=clock();
		while (((clock()-c)/double(CLOCKS_PER_SEC)<=1)&&(tport!=TargetPort||strcmp(TargetIP,thost)));
		return tport==TargetPort&&(!strcmp(TargetIP,thost));
	}
	void Send(const char* Content) {
		char msg[10000]={'m'};
		memcpy(msg+1,(const char*)rsa2.Encrypt(Content),rsa2.GetKeySize());
		udp->Send(thost,tport,rsa2.GetKeySize()+2,msg);
	}
	void Close() {
		udp->Close();
		rsa.Close();
		rsa2.Close();
	}
};

#endif

