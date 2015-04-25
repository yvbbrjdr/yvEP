#include <iostream>
#include "yvEP.hpp"
using namespace yvEP;
using namespace std;

void Recv(const char*,in_port_t,size_t,const char* Content) {cout<<Content<<endl;}

void KeyReq(const char *IP,in_port_t Port) {Turn(IP,Port);}

int main(int argc,char **argv) {
	if (argc>2)
		return 0;
	else if (argc==2) {
		Init(12345,Recv,NULL,NULL);
		if (!Turn(argv[1],12345))
			return 0;
	} else
		Init(12345,Recv,KeyReq,NULL);
	while (1) {
		char msg[256];
		cin.getline(msg,246);
		Send(msg);
	}
}

