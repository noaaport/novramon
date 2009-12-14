// StatusListner.h: interface for the CStatusListner class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LINUX
	#ifndef  _WindowsIncluded_
	#define	 _WindowsIncluded_
	#include <windows.h>
	#endif
	#ifndef  _WinSock2Included_
	#define	 _WinSock2Included_
	#include <winsock2.h>
	#endif
#else
	#include <netinet/in.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
/*    #include <iostream.h> */
    #include "linux_windows_types.h"	
#endif


#include "s75.h"

class CStatusListner  
{
public:
	void GetLastPacketSource(LPBYTE Src);
	int ReceiveSinglePacket(int TimeOut);
	bool FlushBuffer(void);
	bool IsListening(void);
	void EndListening();
	int StartListening(WORD Port);
	bool Initialize(bool* FilterOnIP, LPBYTE pFilterIP, LPBYTE  pBuffer, DWORD* TimeStamp, UINT MaxRcv);
	CStatusListner();
	virtual ~CStatusListner();

private:
	bool m_IsListening;
	bool * m_pFilterOnIP;
protected:
	BYTE m_LastPacketSource[4];
	SOCKET s;
	UINT m_MaxRcvSize;
	BYTE m_RcvBuffer[1500];
	LPBYTE m_pFilterIP;
	LPBYTE m_pDestinationBuffer;
	DWORD * m_pTimeStamp;
};


