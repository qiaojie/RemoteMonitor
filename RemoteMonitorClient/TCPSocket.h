#pragma once

//#include <WinSock2.h>

class TCPSocket
{
public:
	TCPSocket()
	{
		_lastError = WSAENOTCONN;
		_socket = 0;
	}

	TCPSocket(const TCPSocket& sock)
	{
		_lastError = sock._lastError;
		_socket = sock._socket;
		const_cast<TCPSocket&>(sock)._socket = 0;
		const_cast<TCPSocket&>(sock)._lastError = WSAENOTCONN;		
	}

	TCPSocket(SOCKET s)
	{
		_lastError = 0;
		_socket = s;
	}

	void Attach(SOCKET s)
	{
		_lastError = 0;
		_socket = s;
	}

	void SetSendBufferSize(int size)
	{
		setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
	}

	void SetRecvBufferSize(int size)
	{
		setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
	}

	void SetNoDelay(bool value)
	{
		BOOL val = value ? TRUE : FALSE;
		setsockopt(_socket, SOL_SOCKET, TCP_NODELAY, (const char*)&val, sizeof(val));
	}	

	Error Connect(const char* ip, WORD port)
	{
		if(_socket)
			THROW(E_SocketError);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(_socket < 0)
		{
			_lastError = ::WSAGetLastError();
			_socket = 0;
			::LogError2("TCPSocket::Connect failed. error: %s", ::FormatAPIError(_lastError).c_str());
			THROW(E_SocketError);
		}

		if(::connect(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::closesocket(_socket);
			_socket = 0;
			::LogError2("TCPSocket::Connect failed. error: %s", ::FormatAPIError(_lastError).c_str());
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error Listen(WORD port)
	{
		if(_socket)
			THROW(E_SocketError);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(_socket < 0)
		{
			_socket = 0;
			_lastError = ::WSAGetLastError();
			::LogError2("TCPSocket::Listen failed. error: %s", ::FormatAPIError(_lastError).c_str());
			THROW(E_SocketError);
		}
		if(::bind(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::closesocket(_socket);
			_socket = 0;
			::LogError2("TCPSocket::Listen port[%d] failed. error: %s", port, ::FormatAPIError(_lastError).c_str());
			THROW(E_SocketError);
		}
		if(::listen(_socket, 32) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::LogError2("TCPSocket::Listen port[%d] failed. error: %s", port, ::FormatAPIError(_lastError).c_str());
			::closesocket(_socket);
			_socket = 0;
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error Listen(const char* ip, WORD port)
	{
		if(_socket)
			THROW(E_SocketError);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(_socket < 0)
		{
			_socket = 0;
			_lastError = ::WSAGetLastError();
			THROW(E_SocketError);
		}
		if(::bind(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::LogError2("TCPSocket::Listen port[%d] failed. error: %s", port, ::FormatAPIError(_lastError).c_str());
			::closesocket(_socket);
			_socket = 0;
			THROW(E_SocketError);
		}
		if(::listen(_socket, 32) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::LogError2("TCPSocket::Listen port[%d] failed. error: %s", port, ::FormatAPIError(_lastError).c_str());
			::closesocket(_socket);
			_socket = 0;
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error Accept(TCPSocket* socket, sockaddr* address)
	{
		if(!_socket)
			THROW(E_SocketError);

		int len = sizeof(*address);
		SOCKET sock = ::accept(_socket, address, &len);
		if(sock == INVALID_SOCKET)
		{
			_lastError = ::WSAGetLastError();
			if(_lastError == WSAEWOULDBLOCK)
				return E_OK;
			else
				THROW(E_SocketError);
		}
		socket->_lastError = 0;
		socket->_socket = sock;
		return E_OK;
	}

	Error Accept(TCPSocket* socket)
	{
		if(!_socket)
			THROW(E_SocketError);

		SOCKET sock = ::accept(_socket, 0, 0);
		if(sock == INVALID_SOCKET)
		{
			_lastError = ::WSAGetLastError();
			if(_lastError == WSAEWOULDBLOCK)
				return E_OK;
			else
				THROW(E_SocketError);
		}
		socket->_lastError = 0;
		socket->_socket = sock;
		return E_OK;
	}

	Error SetNonBlocking(bool enable = true)
	{
		u_long arg = enable ? 1 : 0;
		if(::ioctlsocket(_socket, FIONBIO, &arg) == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error Send(const char* buf, int size, int* sentLen)
	{
		if(!_socket)
			THROW(E_SocketError);

		*sentLen = ::send(_socket, buf, size, 0);
		if(*sentLen < 0)
		{
			_lastError = ::WSAGetLastError();
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error Recv(char* buf, int size, int* recvLen)
	{
		if(!_socket)
			THROW(E_SocketError);

		*recvLen = ::recv(_socket, buf, size, 0);
		if(*recvLen < 0)
		{
			_lastError = ::WSAGetLastError();
			THROW(E_SocketError);
		}
		if(*recvLen == 0)
		{
			Close();
		}
		return E_OK;
	}

	Error AsyncSend(char* data, int size, WSAOVERLAPPED* overlap, DWORD* sent)
	{
		if(!_socket)
			THROW(E_SocketError);

		WSABUF buf;
		buf.buf = data;
		buf.len = size;
		int ret = ::WSASend(_socket, &buf, 1, sent, 0, overlap, 0);
		if(ret == 0)
			return E_OK;

		int err = WSAGetLastError();
		if(err != WSA_IO_PENDING)
		{
			_lastError = err;
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error AsyncSend(char* data, int size, WSAOVERLAPPED* overlap)
	{
		if(!_socket)
			THROW(E_SocketError);

		WSABUF buf;
		buf.buf = data;
		buf.len = size;
		int ret = ::WSASend(_socket, &buf, 1, 0, 0, overlap, 0);
		if(ret == 0)
			return E_OK;

		int err = WSAGetLastError();
		if(err != WSA_IO_PENDING)
		{
			_lastError = err;
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error AsyncRecv(char* data, int size, WSAOVERLAPPED* overlap)
	{
		if(!_socket)
			THROW(E_SocketError);

		WSABUF buf;
		buf.buf = data;
		buf.len = size;
		DWORD flags = 0;
		DWORD len;
		
		int ret = ::WSARecv(_socket, &buf, 1, &len, &flags, overlap, 0);
		if(ret == 0)
		{
			//LogWarning2("asyncrecv ok len=%d", len);
			return E_OK;
		}

		int err = WSAGetLastError();
		if(err != WSA_IO_PENDING)
		{
			_lastError = err;
			THROW(E_SocketError);
		}
		return E_OK;
	}

	Error CheckRead(bool* canRead)
	{
		if(!_socket)
			THROW(E_SocketError);

		timeval tvLeft = {0, 0};
		fd_set fdRecv;
		FD_ZERO(&fdRecv);
		FD_SET(_socket, &fdRecv);
		int ret = ::select(1, &fdRecv, 0, 0, &tvLeft);
		if(ret == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::closesocket(_socket);
			_socket = 0;
		}

		*canRead = (ret == 1);
		return E_OK;
	}

	Error CheckWrite(bool* canWrite)
	{
		if(!_socket)
			THROW(E_SocketError);

		timeval tvLeft = {0, 0};
		fd_set fdRecv;
		FD_ZERO(&fdRecv);
		FD_SET(_socket, &fdRecv);
		int ret = ::select(1, 0, &fdRecv, 0, &tvLeft);
		if(ret == SOCKET_ERROR)
		{
			_lastError = ::WSAGetLastError();
			::closesocket(_socket);
			_socket = 0;
		}

		*canWrite = (ret == 1);
		return E_OK;
	}

	SOCKET GetSocket()
	{
		return _socket;
	}

	DWORD CheckError()
	{
		return _lastError;
	}

	bool IsConnected()
	{
		return (_socket != 0);
	}

	void Close()
	{
		if(_socket == 0)
			return;
		::closesocket(_socket);
		_socket = 0;	
	}

private:
	DWORD _lastError;
	SOCKET _socket;
};