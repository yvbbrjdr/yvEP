#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <queue>
#include <vector>
#include <cstdlib>
#include "yvEP.hpp"
#include <windows.h>
using namespace yvEP;
using namespace std;

queue<string>que;
unsigned char loggedin,chatting;
char tstr[300],ip2[20];
string server="yvec.yvbb.tk",nickname;
in_port_t port=2428,port2;

void Recv(const char*,in_port_t,size_t,const char *Content) {que.push(Content);}

void *processqueue(void*) {
	while (1)
		if (que.empty())
			Sleep(10);
		else {
			const char *Content=que.front().c_str();
			que.pop();
			if (*Content=='r')
				loggedin=Content[1]-'0';
			else if (*Content=='l') {
				printf("Online Users: \n%syvEC>",Content+2);
				fflush(stdout);
			} else if (*Content=='c') {
				if (Content[1]=='1') {
					strcpy(tstr,Content+2);
					*strchr(tstr,':')=0;
					udp->Send(tstr,atoi(&tstr[strlen(tstr)+1]),5,"test");
				} else if (Content[1]=='2') {
					strcpy(tstr,Content+2);
					*strchr(tstr,':')=0;
					strcpy(ip2,tstr);
					port2=atoi(&tstr[strlen(tstr)+1]);
					udp->Send(ip2,port2,5,"test");
					puts("Connected!\nEnter \"exit()\" to return");
					chatting=1;
				} else if (Content[1]=='3') {
					printf("User not found!\nyvEC>");
					fflush(stdout);
				}
			} else if (*Content=='m') {
				strcpy(tstr,Content+1);
				*strchr(tstr,char(-35))=0;
				time_t t=time(0);
				tm *current_time = localtime(&t);
				printf("%d:%d:%d %s\n%s\n\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,tstr,&tstr[strlen(tstr)+1]);
				if (!chatting) {
					printf("yvEC>");
					fflush(stdout);
				}
			}
		}
}

void processcommand(vector<string>command) {
	Turn(server.c_str(),port);
	if (command.size())
		if (command[0]=="help") {
			puts("connect: Connect To Others");
			puts("exit: Unregister And Exit");
			puts("help: Show This Help");
			puts("list: Show User List");
		} else if (command[0]=="list")
			Send("l0");
		else if (command[0]=="exit") {
			Send(("r3"+nickname).c_str());
			Close();
			yvUDPClose();
			exit(0);
		} else if (command[0]=="connect")
			if (command.size()!=2)
				puts("Usage: connect [nickname]");
			else if (command[1]!=nickname)
				Send(("c0"+command[1]).c_str());
			else
				puts("You cannot connect to yourself!");
		else
			puts("No such command. Try type \"help\"");
}

int main() {
	yvUDPInit();
	pthread_t tid;
	pthread_create(&tid,0,processqueue,0);
	printf("Welcome to yvEncryptedChat\n\nLoading config file \"Server.yvEC\". . . ");
	fflush(stdout);
	FILE *sf=fopen("Server.yvEC","r");
	if (sf) {
		fgets(tstr,1000,sf);
		tstr[strlen(tstr)-1]=0;
		fclose(sf);
		*strchr(tstr,':')=0;
		server=tstr;
		port=atoi(&tstr[strlen(tstr)+1]);
		printf("Done! \nLoaded server %s:%d\n",server.c_str(),port);
	} else {
		printf("Failed!\nWriting default config with %s:%d. . . ",server.c_str(),port);
		fflush(stdout);
		sf=fopen("Server.yvEC","w");
		fprintf(sf,"%s:%d\n",server.c_str(),port);
		fclose(sf);
		puts("Done! ");
	}
	printf("Initializing yvEncryptedProtocol. . . ");
	fflush(stdout);
	Init(2429,Recv,NULL,NULL);
	puts("Done! \nPort 2429 Locked");
	printf("Trying to connect to %s:%d. . . ",server.c_str(),port);
	fflush(stdout);
	while (!Turn(server.c_str(),port)) {
		printf("Failed! \nDo you want to retry? (y/n) ");
		cin.getline(tstr,10);
		if (*tstr!='y'||tstr[1]) {
			yvUDPClose();
			puts("Exiting. . . ");
			return 0;
		}
		printf("Reconnecting to %s:%d. . . ",server.c_str(),port);
		fflush(stdout);
	}
	puts("Done! ");
	while (1) {
		printf("Please enter your nickname: ");
		cin.getline(tstr,100);
		nickname=tstr;
		Send(("r0"+nickname).c_str());
		while (!loggedin);
		if (loggedin==1)
			break;
		else {
			loggedin=0;
			printf("This nickname \"%s\" has been taken. Please use another. \n",nickname.c_str());
		}
	}
	printf("You have successfully logged in with the nickname \"%s\". \n",nickname.c_str());
	Send("l0");
	while (1) {
		if (!chatting)
			printf("yvEC>");
		vector<string>vec;
		cin.clear();
		cin.getline(tstr,246);
		if (!chatting) {
			int len=strlen(tstr);
			for (int i=0;i<len;++i)
				if (tstr[i]!=' ') {
					string s;
					if (tstr[i]!='\"')
						for (;i<len&&tstr[i]!=' ';++i)
							s+=tstr[i];
					else {
						for (++i;i<len&&tstr[i]!='\"';++i)
							s+=tstr[i];
						++i;
					}
					vec.push_back(s);
				}
			processcommand(vec);
		} else
			if (string(tstr)=="exit()")
				chatting=0;
			else {
				if (chatting==1&&Turn(ip2,port2))
					chatting=2;
				Send(('m'+nickname+char(-35)+tstr).c_str());
				time_t t=time(0);
				tm *current_time=localtime(&t);
				printf("%d:%d:%d %s\n%s\n\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,nickname.c_str(),tstr);
			}
	}
}

