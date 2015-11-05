#include "Header.h"

int main(){
	time_t start_time;
	time(&start_time);
	time_t end_time;
	string recv_string = "";
	char *stringsend = "GET /?client=udayton55&hostfile=1 HTTP/1.0\nHost: cache.w3-hidden.cc\n\n";
	char *str = "94.242.222.21";
	DWORD IP = inet_addr(str);
	int iResult;
	string line;
	char *linepch;
	char *next_token1 = NULL;
	char *next_token2 = NULL;
	string strIP, strPort;

	Winsock winsock;
	Parameters P;
	
	//winsock.Getstring(str, stringsend);
	if (!winsock.OpenTcpSocket(IP, 80, stringsend) && !winsock.Receive(recv_string)){
		outputfile << recv_string.data();
		outputfile.close();
		outputfile.open("ultrapeerIPstorage.txt", fstream::in);
		
		for (int i = 0; i < 7; i++){
			getline(outputfile, line);
		}
		while (getline(outputfile, line)){
			linepch = new char[line.length() + 1];
			strcpy_s(linepch, line.length() + 1, line.c_str());
			linepch = strtok_s(linepch, ":", &next_token1);
			strIP = linepch;
			linepch = strtok_s(NULL, ":", &next_token1);
			strPort = linepch;
			Peer p1 = { inet_addr(strIP.c_str()), atoi(strPort.c_str()) };
			P.Q.push(p1);
			P.s.insert(p1);
		}
		P.num_tasks = P.Q.size();
		P.active_threads = 0;
		P.BFSmutex = CreateMutex(NULL, false, NULL);
		P.EventQuite = CreateEvent(NULL, true, false, NULL);
		P.SemaQ = CreateSemaphore(NULL, P.num_tasks, MAX_COUNT, NULL);
		P.finished = CreateSemaphore(NULL, 0, THREAD_COUNT, NULL);
		// thread handles are stored here; they can be used to check status of threads, or kill them
		HANDLE *ptrs = new HANDLE[THREAD_COUNT];
		for (int i = 0; i < THREAD_COUNT; ++i)
		{
			// structure p is the shared space between the threads		
			ptrs[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)Thread_Strategy, &P, 0, NULL);
		}
		printf("-----------created %d threads-----------\n", THREAD_COUNT);

		// make sure this main thread hangs here until the other two quit; otherwise, the program will terminate prematurely
		for (int i = 1; i <= THREAD_COUNT; ++i)
		{
			WaitForSingleObject(P.finished, INFINITE);
			printf("%d thread finished. main() function there--------------\n", i);
		}
		Sleep(1000);
		
	}
	cout << P.s.size() << endl;
	P.it = P.s.begin();
	//for (P.it; P.it != P.s.end(); P.it++){
		//useragent << dnslookup(P.it->IP) << endl;
	//}
	if (winsock.CloseTcpSocket()){
		return 1;
	}

	/*while (!P.Q.empty()){
	strfile << P.Q.front().IP << ":" << P.Q.front().port << endl;
	P.Q.pop();
	}*/
	time(&end_time);
	cout << end_time - start_time << endl;
	system("PAUSE");
	return 0;
}
