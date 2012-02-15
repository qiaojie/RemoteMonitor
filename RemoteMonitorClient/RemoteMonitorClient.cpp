// RemoteMonitorClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Util.h"
#include "TCPSocket.h"
#include "Protocol.h"
#include "Psapi.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Psapi.lib")



int _tmain(int argc, _TCHAR* argv[])
{
	if(argc != 3)
	{
		printf("Usage: RemoteMonitorClient ip port\n");
		return 0;
	}
	const TCHAR* ip = argv[1];
	int port = atoi(CStringA(argv[2]));

	WSADATA wsadata;
	int wsaversion = WSAStartup(MAKEWORD(2, 2), &wsadata);
	TCPSocket sock;

	while(true)
	{
		Error e = sock.Connect(CStringA(ip), port);
		if(e.Check())
		{
			LogError2("error: %s. ", ::FormatAPIError(sock.CheckError()).c_str());
			return 0;
		}
		BinaryStreamIn stream;
		LogInfo("Connect to server...");
		IRemoteMonitorMarshaling::InitProtocol(&stream);
		int sent;
		e = sock.Send((const char*)stream.GetBuffer(), stream.GetSize(), &sent);
		if(e.Check())
		{
			LogError2("error: %s. ", ::FormatAPIError(sock.CheckError()).c_str());
			return 0;
		}
		LogAppend("ok");
		stream.Reset();

		LogInfo("Upload data...");

		PERFORMANCE_INFORMATION info;
		info.cb = sizeof(info);
		GetPerformanceInfo(&info, sizeof(info));

		TCHAR buf[1024];
		DWORD size = 1024;
		GetComputerName(buf, &size);
		ClientData data;
		data.Name = ::ToUtf8(buf);

		Int64 IdleTime;
		Int64 KernelTime;
		Int64 UserTime;
		GetSystemTimes((FILETIME*)&IdleTime, (FILETIME*)&KernelTime, (FILETIME*)&UserTime);
		
		data.CommitTotal			= info.CommitTotal; 
		data.CommitLimit			= info.CommitLimit;
		data.CommitPeak				= info.CommitPeak;
		data.PhysicalTotal			= info.PhysicalTotal;
		data.PhysicalAvailable		= info.PhysicalAvailable;
		data.SystemCache			= info.SystemCache;
		data.KernelTotal			= info.KernelTotal;
		data.KernelPaged			= info.KernelPaged;
		data.KernelNonpaged			= info.KernelNonpaged;

		data.PageSize				= info.PageSize;
		data.HandleCount			= info.HandleCount;
		data.ProcessCount			= info.ProcessCount;
		data.ThreadCount			= info.ThreadCount;

		data.IdleTime	= IdleTime;
		data.KernelTime	= KernelTime;
		data.UserTime	= UserTime;

		//data.CPU = Button2Click(0);
		IRemoteMonitorMarshaling::UpdateData(&stream, &data);
		e = sock.Send((const char*)stream.GetBuffer(), stream.GetSize(), &sent);
		if(e.Check())
		{
			LogError2("error: %s. ", ::FormatAPIError(sock.CheckError()).c_str());
			return 0;
		}
		LogAppend("ok");
		LogInfo("close connection. add sleep 1 second...");
		sock.Close();
		::Sleep(1000);
	}
	return 0;
}

