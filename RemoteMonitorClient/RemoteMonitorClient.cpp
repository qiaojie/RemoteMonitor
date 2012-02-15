// RemoteMonitorClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Util.h"
#include "TCPSocket.h"
#include "Protocol.h"
#include "Psapi.h"
#include <comdef.h>
#include <Wbemidl.h>

#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>

#pragma comment(lib, "wbemuuid.lib")

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib,"Setupapi.lib")



#define CALL_COM(x, msg) {HRESULT hr = x; if(FAILED(hr)){printf(#msg" Error code = 0x%x\n", hr); return hr;}}

class QuerySink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;
	CRITICAL_SECTION threadLock; // for thread safety

public:
    QuerySink() { m_lRef = 0; bDone = false; 
	    InitializeCriticalSection(&threadLock); }
    ~QuerySink() { bDone = true;
        DeleteCriticalSection(&threadLock); }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();        
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate( 
            LONG lObjectCount,
            IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
            );
        
    virtual HRESULT STDMETHODCALLTYPE SetStatus( 
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
            );

	bool IsDone();
};


ULONG QuerySink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG QuerySink::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT QuerySink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT QuerySink::Indicate(long lObjectCount,
    IWbemClassObject **apObjArray)
{
	HRESULT hres = S_OK;

    for (int i = 0; i < lObjectCount; i++)
    {
        VARIANT varName;
		CALL_COM(
			apObjArray[i]->Get(_bstr_t(L"Name"), 0, &varName, 0, 0),
			"Failed to get the data from the query");

        printf("Name: %ls\n", V_BSTR(&varName));
    }

    return WBEM_S_NO_ERROR;
}

HRESULT QuerySink::SetStatus(
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        )
{
	if(lFlags == WBEM_STATUS_COMPLETE)
	{
		printf("Call complete.\n");

		EnterCriticalSection(&threadLock);
		bDone = true;
		LeaveCriticalSection(&threadLock);
	}
	else if(lFlags == WBEM_STATUS_PROGRESS)
	{
		printf("Call in progress.\n");
	}
    
    return WBEM_S_NO_ERROR;
}


bool QuerySink::IsDone()
{
    bool done = true;

	EnterCriticalSection(&threadLock);
	done = bDone;
	LeaveCriticalSection(&threadLock);

    return done;
}    // end of QuerySink.cpp


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


	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED); 


	SEC_WINNT_AUTH_IDENTITY_W        authidentity;
	SecureZeroMemory(&authidentity, sizeof(authidentity));

	authidentity.User = (unsigned short*)L"QiaoJie";
	authidentity.UserLength = wcslen((WCHAR*)authidentity.User);
	authidentity.Domain = (unsigned short*)L"QiaoJie-PC2";
	authidentity.DomainLength = wcslen((WCHAR*)authidentity.Domain);
	authidentity.Password = (unsigned short*)L"pp1209";
	authidentity.PasswordLength = wcslen((WCHAR*)authidentity.Password);
	authidentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

	SOLE_AUTHENTICATION_INFO authninfo[2];

	SecureZeroMemory(authninfo, sizeof(SOLE_AUTHENTICATION_INFO) * 2);

	// NTLM Settings
	authninfo[0].dwAuthnSvc = RPC_C_AUTHN_WINNT;
	authninfo[0].dwAuthzSvc = RPC_C_AUTHZ_NONE;
	authninfo[0].pAuthInfo = &authidentity;

	// Kerberos Settings
	authninfo[1].dwAuthnSvc = RPC_C_AUTHN_GSS_KERBEROS ;
	authninfo[1].dwAuthzSvc = RPC_C_AUTHZ_NONE;
	authninfo[1].pAuthInfo = &authidentity;

	SOLE_AUTHENTICATION_LIST authentlist;

	authentlist.cAuthInfo = 2;
	authentlist.aAuthInfo = authninfo;

	printf("initialize security\n");
	CALL_COM(
		CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,&authentlist, EOAC_NONE, NULL),
		"Failed to initialize security.");

	CComPtr<IWbemLocator> wbemLocator;
	
	printf("create IWbemLocator object\n");
	CALL_COM(
		wbemLocator.CoCreateInstance(CLSID_WbemLocator),
		"Failed to create IWbemLocator object.");

	CComPtr<IWbemServices> wbemServices;

	printf("Connect to the root\\CIMV2 namespace with the current user.\n");
	CALL_COM(
		wbemLocator->ConnectServer(BSTR(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &wbemServices),
		"Could not connect. ");

	printf("SetProxyBlanket.\n");
	CALL_COM(
		CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, 
			RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE),
		"Could not set proxy blanket.");
	
	CComPtr<IEnumWbemClassObject>  objects;
    CALL_COM(
		wbemServices->ExecQuery(L"WQL", L"SELECT * FROM Win32_PerfFormattedData_PerfOS_Processor", WBEM_FLAG_FORWARD_ONLY, 0, &objects),
		"ExecQuery Failed");

	while(true)
	{
		CComPtr<IWbemClassObject> objs;
		ULONG count;
		objects->Next(WBEM_INFINITE, 1, &objs, &count);
		if(count == 0)
			break;
		BSTR str;
		objs->GetObjectText(0, &str);
		wprintf(L"%s\n", str);
		::SysFreeString(str);
	}

	/*
	QuerySink* querySink = new QuerySink();
    CALL_COM(
		wbemServices->ExecQueryAsync(
			bstr_t("WQL"), 
			bstr_t("SELECT * FROM Win32_OperatingSystem"),
			WBEM_FLAG_BIDIRECTIONAL, 
			NULL,
			querySink),
		"ExecQueryAsync failed!");

	Sleep(1000);
	*/

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;

    // Create a HDEVINFO with all present devices.
    hDevInfo = SetupDiGetClassDevs(NULL,
        0, // Enumerator
        0,
        DIGCF_PRESENT | DIGCF_ALLCLASSES );
    
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        printf("SetupDiGetClassDevs Failed!");
        return 1;
    }
    
    // Enumerate through all devices in Set.
    wprintf(L"½á¹û\n");
	//for(int i = 0; count; i++)
	int index = 0;
	while(true)
    {
	    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		if(!SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData))
			break;
        DWORD DataT;
        LPTSTR buffer = 0;
        DWORD size = 0;
        index++;

		while (!SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_DEVICEDESC,
            &DataT,
            (PBYTE)buffer,
            size,
            &size))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // Change the buffer size.
                if (buffer) LocalFree(buffer);
                buffer = (LPTSTR)LocalAlloc(LPTR, size);
            }
            else
            {
                // Insert error handling here.
                break;
            }
        }

		printf("result:[%S]\n", buffer);
        
        if (buffer)LocalFree(buffer);
    }

	//  Cleanup
    SetupDiDestroyDeviceInfoList(hDevInfo);



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
		char recvbuf[4096];
		int recvsize;
		sock.Recv(recvbuf, 4096, &recvsize).Check();

		LogAppend("ok");
		LogInfo("close connection. add sleep 1 second...");
		//::shutdown(sock.GetSocket(), SD_BOTH);
		sock.Close();
		::Sleep(1000);
	}
	return 0;
}

