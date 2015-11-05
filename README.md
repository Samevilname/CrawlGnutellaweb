CrawlGnutellaweb
================
1. Introduction
  only 1 cpp and 1 head file are contained in this program for find seed files and their peers
head file contents class winsock, class Peer, class Parameters, UINT Thread_strategy and function dnslookup
main.cpp file create each class and send initial data to each class to crawl each seed. after parsing each string receiving from each node, send the same command to each new IP address from queue and receive new ultrapeers and peers.
2. head file implement
  a) class Winsock 
    using for building socket connection with perticular ip and port and send perticular string to receive string contenting neighbor peers and ultrapeers
    1)constructor:
      1. initialing WSADATA to start up a winsocket and to judge its return whether is 0 or other, if it is 0 initialing failed, not 0 means success.
    2)OpenTcpSock(DWORD &IP, int port, char *stringsend)
      1. build a socket using SOCKET to connect to server, if it is INVALID_SOCKET, it is failed to connection.
      2. judge whether input string is IP address or domain name, if it is IP address, just give it to stucture server, if it is domain name use gethostbyname() to do a DNS look up.
      3. build a structure server to connect including IP address, port information and protocol type
      4. use connect() sentence to connect to server and send information from structure server to have a connection with server.
      5. use send() sentence to send string which is reference arguments stringsend, may one of sending string is like what i wrote in the program: "GET /?client=udayton55&hostfile=1 HTTP/1.0\nHost: cache.w3-hidden.cc\n\n" to ask for request from server and wait for seed files receive.
    3)Receive(string &recv_string)
      1. use recv() to receive request after handshaking. use same socket and receive data store in recvbuf and state its size.
      2. if the result of recv() equal to 0, it means it has no contents to give you and the result great than 0, it means recvbuf has string inside.
      3. save string data which we receive from recv(), because recv() has its data limitation for its receiving, make a loop to receive data until recv() result equal to 0
      4. give the final string to recv_string to reference arguments to let the next step to parse this string.
    4)CloseTCPSocket(void)
      1. use function closesocket() to close using socket. if return value is SOCKET_ERROR, it means close socket failed and give the error number to print out on screen.
  b) class Peer
    declare Peer structure including IP and port using for store receiving ip and port, mostly using for saving data in queue and set structures.
  c) class Parameters
    declare sharing memory for multiple threads including Handle BFSmutex, Handle EventQuit, Handle SemaQ, Handle finished, set s, queue Q, int num_tasks and int active_threads.
    1. BFSmutex is using for locking queue to give threads to author to crawl.
    2. EventQuit is using for giving signal to semaphore to lock the queue.
    3. SemaQ is using for lock queue until signal from EventQuit.
    4. finished is using for finishing active threads.
    5. set s using for storge all peer saving ip address and port and check size() to make sure no duplicate.
    6. queue Q using for storge peer and X-Try-Ultrapeer to crawl.
  d) UNIT Thread_Strategy    
    strategy of threads saved in class Thread_strategy, use semaphore and mutex to lock the whole critical section and queue to make sure only one thread get data from sharing space in the same time.
    the whole thread strategy is to lock queue using author to give one thread using queue author at the same time. when the queue is empty and no active threads is waiting for queue, signaled eventquit and let whole project out. receive string from sending GNUTELLA to each peer to get peers ip and port saving in queue and use set to check duplicate and storage all ip and port in it.
  e) function dnslookup
    using winsock function getnameinfo to save host name in string name and save it as return value to output.
the whole project running for 3239 sec and receive 759 nodes. the detail information of implementing in Crawler Graphs.xlsx.
