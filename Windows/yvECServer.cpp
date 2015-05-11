#include <iostream>
#include <cstdio>
#include <map>
#include <queue>
#include <string>
#include "yvEP.hpp"
#include <windows.h>
using namespace std;
using namespace yvEP;

#define in_port_t u_short
typedef pair<string,in_port_t>endpoint;
typedef pair<endpoint,string>pack;
typedef map<string,endpoint>listmap;
queue<pack>que;
listmap userlist;
char tstr[300];

void *processqueue(void*) {
	while (1)
		if (que.empty())
			Sleep(10);
		else {
			pack tt=que.front();
			que.pop();
			const char *IP=tt.first.first.c_str(),*Content=tt.second.c_str();
			in_port_t Port=tt.first.second;
			if (*Content=='r') {
				if (Content[1]=='0') {
					Turn(IP,Port);
					if (userlist.count(Content+2))
						Send("r2");
					else {
						userlist[Content+2]=endpoint(IP,Port);
						Send("r1");
					}
				} else if (Content[1]=='3')
					userlist.erase(Content+2);
			} else if (*Content=='l') {
				string t="l1";
				for (map<string,endpoint>::iterator i=userlist.begin();i!=userlist.end();++i)
					t+=i->first+'\n';
				Turn(IP,Port);
				Send(t.c_str());
			} else if (*Content=='c')
				if (userlist.count(Content+2)) {
					Turn(userlist[Content+2].first.c_str(),userlist[Content+2].second);
					sprintf(tstr,"c1%s:%d",IP,Port);
					Send(tstr);
					Turn(IP,Port);
					sprintf(tstr,"c2%s:%d",userlist[Content+2].first.c_str(),userlist[Content+2].second);
					Send(tstr);
				} else {
					Turn(IP,Port);
					Send("c3");
				}
		}
}

void Recv(const char *IP,in_port_t Port,size_t,const char *Content) {que.push(pack(endpoint(IP,Port),Content));}

int main() {
	yvUDPInit();
	in_port_t port=2428;
	pthread_t tid;
	pthread_create(&tid,0,processqueue,0);
	printf("Welcome to yvEncryptedChatServer\n\nLoading config file \"Port.yvECS\". . . ");
	fflush(stdout);
	FILE *sf=fopen("Port.yvECS","r");
	if (sf) {
		fgets(tstr,10,sf);
		fclose(sf);
		port=atoi(tstr);
		printf("Done! \nLoaded port %d\n",port);
	} else {
		printf("Failed!\nWriting default config with %d. . . ",port);
		fflush(stdout);
		sf=fopen("Port.yvECS","w");
		fprintf(sf,"%d\n",port);
		fclose(sf);
		puts("Done! ");
	}
	printf("Initializing yvEncryptedProtocol. . . ");
	fflush(stdout);
	Init(port,Recv,NULL,NULL);
	printf("Done! \nPort %d Locked\nStart Listening. . . \n",port);
	while (1) {
		cin.clear();
		cin.getline(tstr,100);
		userlist.erase(tstr);
	}
	return 0;
}
#undef in_port_t

