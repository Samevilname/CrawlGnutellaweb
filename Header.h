/*
 using for all cpp
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN for historical reason winsocket 2.0 need to define

#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <mmsystem.h>
#include <queue>
#include <map>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <ctime>


using namespace std;
#define MAX_COUNT 10000000
#define THREAD_COUNT 50
fstream strfile("LeafpeerIPstorage.txt", fstream::in | fstream::out);
fstream outputfile("ultrapeerIPstorage.txt", fstream::in | fstream::out);
fstream useragent("useragent.txt", fstream::out);
class Peer{
public:
	DWORD IP;
	unsigned short port;
	bool operator()(Peer x, Peer y){
		return memcmp(&x, &y, sizeof(Peer)) < 0;
	}
};
class Parameters{
public:
	HANDLE BFSmutex;
	HANDLE EventQuite;
	HANDLE SemaQ;
	HANDLE finished;
	set<Peer, Peer> s;
	set<Peer, Peer>::iterator it;
	queue<Peer> Q;
	int num_tasks;
	int active_threads;
};
class Winsock
{
public:
	Winsock(); //constructor
	~Winsock();//destructor
	bool OpenTcpSocket(DWORD &IP, int port, char *stringsend);// open a socket to build TCP connection with given IP address
	bool Receive(string &);// receive buf from server by sending given string
	bool CloseTcpSocket(void); // close socket by WSAcleanup()
	//void Getstring(char x[],char y[]);
private:
#define DEFAULT_BUFLEN 150 // size of receiving array
	int iResult;// save warning code
	//char *str = "cache.w3-hidden.cc";// IP address by domain name
	//char *buf;//receive storing space
	//string recev_string = "";
	int bytes = 0;// return of sending also bytes of sending
	//char *stringsend = "GET /?client=udayton55&hostfile=1 HTTP/1.0\nHost: cache.w3-hidden.cc\n\n";//to get the ultrapeer node
	int recvbuflen = DEFAULT_BUFLEN;// size of receiving strong spacing
	char recvbuf[DEFAULT_BUFLEN];// peer node receive storing spacing
	SOCKET ConnectSocket = INVALID_SOCKET;
	//int i, j;
};
bool Winsock::OpenTcpSocket(DWORD &IP, int port, char *stringsend){
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		//itoa(WSAGetLastError(), Errorstrnum, 10);
		//Errorstr = "socket() generated error" + Errorstrnum + "\n";
		//fputs( Errorstr, fp);
		printf("socket() generated error: %d", WSAGetLastError());
		return 1;
	}
	// structure for connecting to server
	struct sockaddr_in server;

	// structure used in DNS lookups
	struct hostent *remote;
	// if a valid IP, directly drop its binary version into sin_addr
	server.sin_addr.S_un.S_addr = IP;
	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons(port);		// host-to-network flips the byte order

	// connect to the server on port 80
	if (connect(ConnectSocket, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Connection error: %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("successfully connect to %s on port:%d\n", inet_ntoa(server.sin_addr), htons(server.sin_port));

	//printf("Successfully connected to %s (%s) on port %d\n", str, inet_ntoa(server.sin_addr), htons(server.sin_port));

	int bytesSent = send(ConnectSocket, stringsend, 120, 0);
	if (bytesSent < 0){
		printf("Error sending request \n");
		return 1;
	}
	else{
		printf("Bytes sent: %d\n", bytesSent);
		return 0;
	}


	// Resolve the server address and port
}
bool Winsock::Receive(string &recv_string){
	FD_SET Reader;
	FD_ZERO(&Reader);
	FD_SET(ConnectSocket, &Reader); // add your sock to the set Reader 
	struct timeval timeout; // set timeout, used for select()
	timeout.tv_sec = 30; // must include <time.h>
	timeout.tv_usec = 0;
	recv_string = ""; // empty string
	do{
		if (select(0, &Reader, NULL, NULL, &timeout) > 0){
			// has something in sock, so call function recv
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0); // save msg in buf
			// if bytes > 0 
			if (iResult > 0){
				for (int i = 0; i < iResult; i++){
					printf("%c", recvbuf[i]);
					recv_string += recvbuf[i];
				}
			}
			else if (iResult == 0)
				printf("Connection closed\n");
			else{
				printf("recv failed with error: %d\n", WSAGetLastError());
				return 1;
			}
		}
		else {
			printf(" timeout \n");
			return 1;
		}
	} while (iResult > 0);
	// finish receiving response
	return 0;
};
bool Winsock::CloseTcpSocket()
{
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) {
		printf("close failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	else
		return 0;
};
/*void Winsock::Getstring(char (&x)[], char &y){
for (int i = 0; i < sizeof(x); i++){
str[i] = x[i];
}
for (int i = 0; i < sizeof(y); i++){
stringsend[i] = y[i];
}
}*/
Winsock::Winsock()
{
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		printf("WSAstartup failed %d\n", iResult);
	}

}


Winsock::~Winsock()
{
	WSACleanup();
}
UINT Thread_Strategy(LPVOID pParam){
	Parameters *p = (Parameters*)pParam;
	Winsock winsock;
	Peer p1, p2;
	int num1, num2;
	char *peerstrsend = "GNUTELLA CONNECT/0.6\r\nUser-Agent: udayton_cs_crawler/1.0\r\nX-Ultrapeer: False\r\nCrawler: 0.1\r\n\r\n";//to get the peer node
	string recv_string = "";
	string sub_recv_string;
	string line, strIP, strPort;
	queue<Peer> LeafQ;
	char *linepch;
	char *next_token1 = NULL;
	char *next_token2 = NULL;

	// wait for mutex, then print and sleep inside the critical section
	WaitForSingleObject(p->BFSmutex, INFINITE);					// lock mutex
	printf("Thread %d started\n", GetCurrentThreadId());		// always print inside critical section to avoid screen garbage
	ReleaseMutex(p->BFSmutex);										// release critical section
	HANDLE arr[] = { p->EventQuite, p->SemaQ };
	while (true){
		if (WaitForMultipleObjects(2, arr, false, INFINITY) == WAIT_OBJECT_0 + 1){
			WaitForSingleObject(p->BFSmutex, INFINITE);
			Sleep(1000);
			p->active_threads++;
			if (!p->Q.empty()){
				p1 = p->Q.front();
				p->Q.pop();
			}
			ReleaseMutex(p->BFSmutex);
			if (!winsock.OpenTcpSocket(p1.IP, p1.port, peerstrsend)){
				if (!winsock.Receive(recv_string)){
					//crawl, parse string to receive structure Peer and insert to Leafpeer_set
					strfile << recv_string << endl;
					if (recv_string != ""){
						size_t found_begin = recv_string.find("User-Agent");
						if (found_begin != -1){
							size_t found_end = recv_string.find("\n", found_begin + 1);
							sub_recv_string = recv_string.substr(found_begin + 12, found_end - found_begin - 11);
							useragent << sub_recv_string << endl;
						}
					}
					if (recv_string != ""){
						size_t found_begin = recv_string.find("Peers");
						if (found_begin != -1){
							size_t found_end = recv_string.find("\n", found_begin + 1, 1);
							sub_recv_string = recv_string.substr(found_begin + 7, found_end - found_begin - 6);

							linepch = new char[sub_recv_string.length() + 1];
							strcpy_s(linepch, sub_recv_string.length() + 1, sub_recv_string.c_str());
							linepch = strtok_s(linepch, ",", &next_token1);
							while (linepch != NULL){
								linepch = strtok_s(linepch, ":", &next_token2);
								strIP = linepch;
								linepch = strtok_s(NULL, ":", &next_token2);
								strPort = linepch;
								linepch = strtok_s(NULL, ",", &next_token1);
								Peer p1 = { inet_addr(strIP.c_str()), atoi(strPort.c_str()) };
								//p->Q.push(p1);
								LeafQ.push(p1);
							}
						}
					}
					if (recv_string != ""){
						size_t found_begin = recv_string.find("X-Try-Ultrapeers");
						if (found_begin != -1){
							size_t found_end = recv_string.find("\n", found_begin + 1, 1);
							sub_recv_string = recv_string.substr(found_begin + 18, found_end - found_begin - 17);

							linepch = new char[sub_recv_string.length() + 1];
							strcpy_s(linepch, sub_recv_string.length() + 1, sub_recv_string.c_str());
							linepch = strtok_s(linepch, ",", &next_token1);
							while (linepch != NULL){
								linepch = strtok_s(linepch, ":", &next_token2);
								strIP = linepch;
								linepch = strtok_s(NULL, ":", &next_token2);
								strPort = linepch;
								linepch = strtok_s(NULL, ",", &next_token1);
								Peer p1 = { inet_addr(strIP.c_str()), atoi(strPort.c_str()) };
								//p->Q.push(p1);
								LeafQ.push(p1);
							}
						}
					}
				}
			}
			Sleep(5000);
			WaitForSingleObject(p->BFSmutex, INFINITE);
			while (!LeafQ.empty()){
				p2 = { LeafQ.front().IP, LeafQ.front().port };
				num1 = p->s.size();
				p->s.insert(p2);
				num2 = p->s.size();
				if (num1 < num2){
					p->Q.push(p2);
					ReleaseSemaphore(p->SemaQ, 1, NULL);
				}
				LeafQ.pop();
			}
			p->active_threads--;
			if (p->active_threads == 0){
				SetEvent(p->EventQuite);
			}
			Sleep(1000);
			ReleaseMutex(p->BFSmutex);
		}
		else
			break;
	}
	ReleaseSemaphore(p->finished, 1, NULL);
	return 0;
}
string dnslookup(DWORD ip)
{
	string name = "";  // empty name
	DWORD dwRetval;

	struct sockaddr_in saGNI; 	// containing the add & port # of the socket
	char hostname[NI_MAXHOST]; 	// NI_MAXSERV=32, defined in <ws2tcpip.h>
	char servInfo[NI_MAXSERV]; 	// NI_MAXHOST=1025
	u_short port = 27015;      	// port number
	saGNI.sin_family = AF_INET;
	saGNI.sin_addr.s_addr = ip; // if use string, inet_addr(strIP.c_str());
	saGNI.sin_port = htons(port);

	dwRetval = getnameinfo((struct sockaddr *) &saGNI,
		sizeof(struct sockaddr),
		hostname,
		NI_MAXHOST, servInfo,
		NI_MAXSERV, NI_NUMERICSERV);
	if (dwRetval != 0)
		printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
	else
		name = hostname;
	return name;
}
