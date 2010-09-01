#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#else
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif
#include <iostream>
#include <udt.h>
//#include "cc.h"

using namespace std;

void DeleteBuf(char* buf, int) {delete [] buf;}

#ifndef WIN32
void* monitor(void*);
#else
DWORD WINAPI monitor(LPVOID);
#endif

int main(int argc, char* argv[])
{
   if ((3 != argc) || (0 == atoi(argv[2])))
   {
      cout << "usage: appclient server_ip server_port" << endl;
      return 0;
   }

   struct addrinfo hints, *res;

   memset(&hints, 0, sizeof(struct addrinfo));

   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   //hints.ai_socktype = SOCK_DGRAM;

   if (0 != getaddrinfo(argv[1], argv[2], &hints, &res))
   {
      cout << "incorrect network address.\n" << endl;
      return 0;
   }

   UDTSOCKET client = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   // UDT Options
   //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
   //UDT::setsockopt(client, 0, UDT_MSS, new int(7500), sizeof(int));
   //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(200000000), sizeof(int));
   //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(100000000), sizeof(int));

#ifdef WIN32
   UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
#endif

   // connect to the server, implict bind
   if (UDT::ERROR == UDT::connect(client, res->ai_addr, res->ai_addrlen))
   {
      cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
   }

   freeaddrinfo(res);

   // using CC method
   //CUDPBlast* cchandle = NULL;
   //int temp;
   //UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
   //if (NULL != cchandle)
   //   cchandle->setRate(500);

   int size = 10000000;
   int handle = 0;
   char* data = new char[size];

#ifndef WIN32
   pthread_create(new pthread_t, NULL, monitor, &client);
#else
   CreateThread(NULL, 0, monitor, &client, 0, NULL);
#endif

   for (int i = 0; i < 1000; i ++)
   {
      if (UDT::ERROR == UDT::send(client, data, size, 0, &handle))
      //if (UDT::ERROR == UDT::sendmsg(client, data, size))
      {
         cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
         return 0;
      }
   }

   UDT::close(client);

   delete [] data;

   return 1;
}

#ifndef WIN32
void* monitor(void* s)
#else
DWORD WINAPI monitor(LPVOID s)
#endif
{
   UDTSOCKET u = *(UDTSOCKET*)s;

   UDT::TRACEINFO perf;

   cout << "SendRate(Mb/s) RTT(ms) FlowWindow PktSndPeriod(us) RecvACK RecvNAK" << endl;

   while (true)
   {
#ifndef WIN32
      sleep(1);
#else
      Sleep(1000);
#endif
      if (UDT::ERROR == UDT::perfmon(u, &perf))
      {
         cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << endl;
         break;
      }

      cout << perf.mbpsSendRate << "\t" 
           << perf.msRTT << "\t" 
           << perf.pktFlowWindow << "\t" 
           << perf.usPktSndPeriod << "\t" 
           << perf.pktRecvACK << "\t" 
           << perf.pktRecvNAK << endl;
   }

#ifndef WIN32
   return NULL;
#else
   return 0;
#endif
}
