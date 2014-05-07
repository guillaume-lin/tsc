/**
 * io_mfc.c
 * 
 * 实现底层通信
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-02 11:10:24 中国标准时间
 * 
 * $log$
 *
 */
#include <windows.h>

static SOCKET s_sd;
static char s_ip[255];
static int s_port;

int init_sock(char* ip,int port)
{
	s_sd = INVALID_SOCKET;
	strncpy(s_ip,ip,strlen(ip) > 255 ? 255 : strlen(ip));
	s_port = port;
	return 0;
}
int close_sock(SOCKET s);

SOCKET open_sock(char* ip, int port)
{
	WSADATA wsaData;
	WORD version;
	SOCKET s;
	struct sockaddr_in addr_in;
	u_long arg;

	version = MAKEWORD(2,2);

	if(WSAStartup(version,&wsaData) != 0){
		return INVALID_SOCKET;
	}

	s = socket(AF_INET,SOCK_STREAM,0);
	if(s == INVALID_SOCKET){
		close_sock(s);
		return s;
	}

	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr(ip);
	addr_in.sin_port = htons(port);

	if(connect(s,(struct sockaddr*)&addr_in,sizeof(struct sockaddr_in)) != 0){
		
		close_sock(s);
		return INVALID_SOCKET;
	}

	// make socket nonblocking
	arg = 1;
	if(ioctlsocket(s,FIONBIO,&arg) != 0){
		perror("ioctlsocket");
		close_sock(s);
		return INVALID_SOCKET;
	}

	return s;
}
int close_sock(SOCKET s)
{
	closesocket(s);
	WSACleanup();
	return 0;
}
static int Init(HANDLE handle)
{
	BOOL b1,b2,b3;
	DCB dcb;
	COMMTIMEOUTS ct;

	b1 = GetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);
	if(!b1)
		return -1;
	dcb.DCBlength = sizeof(DCB);
	dcb.fParity  = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX  = FALSE;
	dcb.fNull = FALSE;
	dcb.fAbortOnError = FALSE;

	dcb.ByteSize = 8;		// 字节为8位
	dcb.Parity   = NOPARITY;	// 没有奇偶校验
	dcb.StopBits = ONESTOPBIT;	// 一个停止位
	dcb.BaudRate = 9600;		// 9600 波特率

	b2 = SetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);

	if(!b2)
		return -1;


//	typedef struct _COMMTIMEOUTS {  
//		DWORD ReadIntervalTimeout;  
//		DWORD ReadTotalTimeoutMultiplier;  
//		DWORD ReadTotalTimeoutConstant;  
//		DWORD WriteTotalTimeoutMultiplier;  
//		DWORD WriteTotalTimeoutConstant;
//	} COMMTIMEOUTS, *LPCOMMTIMEOUTS;

//	设定串口立即返回
	ct.ReadIntervalTimeout = MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;

	b3 = SetCommTimeouts(
			handle, // HANDLE hFile,
			&ct	// LPCOMMTIMEOUTS lpCommTimeouts
			);

	if(!b3)
		return -1;

	return 0;
}

HANDLE s_handle1;
HANDLE s_handle2;
/**
 *  打开指定的通信端口
 */
int open_comm(int port)
{
	HANDLE handle;
	char* name;
	if(port == 1 )
		name = "com1";
	else if(port == 2)
		name = "com2";
	else if(port == 255){
		s_sd = open_sock(s_ip,s_port);
		if(s_sd != INVALID_SOCKET)
			return port;
		else
			return -1;
	}
	else
		return -1;

	handle = CreateFile(
			name,	// LPCTSTR lpFileName,
			GENERIC_READ | GENERIC_WRITE ,// DWORD dwDesiredAccess,
			0,	// DWORD dwShareMode,
			NULL,	// LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			OPEN_EXISTING, // DWORD dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,// DWORD dwFlagsAndAttributes,
			NULL	// HANDLE hTemplateFile
			);

	if(handle != INVALID_HANDLE_VALUE)
	{
		if(Init(handle)!= 0)
			return -1;
	}
	else
		return -1;

	if(port == 1){
		s_handle1 = handle;
		return 1;
	}
	else {
		s_handle2 = handle;
		return 2;
	}
}

int read_comm(int fd, byte* buf, int len)
{
	HANDLE handle;
	int bytesRead = 0;
	BOOL b;
	if(fd == 1)
		handle = s_handle1;
	else if(fd == 2)
		handle = s_handle2;
	else if(fd == 255){
		char tmp[255];
		bytesRead = recv(s_sd,buf,len,0);
		sprintf(tmp,"TickCount:%d bytesRead: %d  lastError: %d\n",GetTickCount(),bytesRead,GetLastError());
		if(bytesRead > 0)
			OutputDebugString(tmp);
		return bytesRead;
	}
	else
		return -1;
	
	b = ReadFile(
			handle, // HANDLE hFile,
			buf,		// LPVOID lpBuffer,
			len,	// DWORD nNumberOfBytesToRead,
			&bytesRead, // LPDWORD lpNumberOfBytesRead,
			NULL	// LPOVERLAPPED lpOverlapped
			);

	if(!b)
		return -1;
	else
		return bytesRead;
}

int write_comm(int fd, byte* buf, int len)
{
	HANDLE handle;
	int bytesWrite = 0;
	BOOL b;
	
	if(fd == 1)
		handle = s_handle1;
	else if(fd == 2)
		handle = s_handle2;
	else if(fd == 255){
		char tmp[255];
		bytesWrite = send(s_sd,buf,len,0);
		sprintf(tmp,"TickCount: %d bytesWrite: %d         %d\n",GetTickCount(),bytesWrite,GetLastError());
		if(bytesWrite > 0)
		OutputDebugString(tmp);
		return bytesWrite;
	}
	else{
		return -1;
	}


	b = WriteFile(
			handle, // HANDLE hFile,
			buf, 	// LPCVOID lpBuffer,
			len,	// DWORD nNumberOfBytesToWrite,
			&bytesWrite,	// LPDWORD lpNumberOfBytesWritten,
			NULL	// LPOVERLAPPED lpOverlapped
			);

	if(!b)
		return -1;
	else
		return bytesWrite;
}
int set_comm_baud(int fd, int baud)
{
	HANDLE handle;
	DCB dcb;
	BOOL b1;

	if(fd == 1)
		handle = s_handle1;
	else if(fd == 2)
		handle = s_handle2;
	else
		return 0;

	b1 = GetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);
	if(!b1)
		return -1;

	dcb.ByteSize = 8;		// 字节为8位
	dcb.Parity   = NOPARITY;	// 没有奇偶校验
	dcb.StopBits = ONESTOPBIT;	// 一个停止位
	dcb.BaudRate = baud;		//  波特率

	b1 = SetCommState(
			handle, // HANDLE hFile,
			&dcb	// LPDCB lpDCB
			);

	if(!b1)
		return -1;
	
	return 0;
}
int close_comm(int fd)
{
	if(fd == 1)
		CloseHandle(s_handle1);
	else if(fd == 2)
		CloseHandle(s_handle2);
	else if(fd == 255)
		close_sock(s_sd);

	return 0;
}

int wait_for_100ms()
{
	Sleep(100);
	return 0;
}
