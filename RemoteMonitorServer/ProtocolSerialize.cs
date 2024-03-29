/* This file is automatically generated from RemoteMonitor.Interface, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null. Don't modify it. */
using System;
using System.Collections.Generic;
using EGL;
using EGL.Communication;

namespace RemoteMonitor
{
	public static class ProtocolSerializeExtension
	{
		public static void Write(this BinaryStreamWriter stream, ClientData val)
		{
			if(val == null)
			{
				stream.Write(false);
				return;
			}

			stream.Write(true);
			stream.Write(val.Name);
			stream.Write(val.CPU);
			stream.Write(val.IdleTime);
			stream.Write(val.KernelTime);
			stream.Write(val.UserTime);
			stream.Write(val.CommitTotal);
			stream.Write(val.CommitLimit);
			stream.Write(val.CommitPeak);
			stream.Write(val.PhysicalTotal);
			stream.Write(val.PhysicalAvailable);
			stream.Write(val.SystemCache);
			stream.Write(val.KernelTotal);
			stream.Write(val.KernelPaged);
			stream.Write(val.KernelNonpaged);
			stream.Write(val.PageSize);
			stream.Write(val.HandleCount);
			stream.Write(val.ProcessCount);
			stream.Write(val.ThreadCount);
		}
		public static void Read(this BinaryStreamReader stream, out ClientData val)
		{
			bool hasValue;
			stream.Read(out hasValue);
			if(!hasValue)
			{
				val = null;
				return;
			}

			val = new ClientData();
			stream.Read(out val.Name);
			stream.Read(out val.CPU);
			stream.Read(out val.IdleTime);
			stream.Read(out val.KernelTime);
			stream.Read(out val.UserTime);
			stream.Read(out val.CommitTotal);
			stream.Read(out val.CommitLimit);
			stream.Read(out val.CommitPeak);
			stream.Read(out val.PhysicalTotal);
			stream.Read(out val.PhysicalAvailable);
			stream.Read(out val.SystemCache);
			stream.Read(out val.KernelTotal);
			stream.Read(out val.KernelPaged);
			stream.Read(out val.KernelNonpaged);
			stream.Read(out val.PageSize);
			stream.Read(out val.HandleCount);
			stream.Read(out val.ProcessCount);
			stream.Read(out val.ThreadCount);
		}
		public static void Write(this BinaryStreamWriter stream, ClientData[] arr)
		{
			if(arr == null)
			{
				stream.Write(0);
				return;
			}
			stream.Write(arr.Length);
			foreach(var e in arr)
				stream.Write(e);
		}
		public static void Read(this BinaryStreamReader stream, out ClientData[] arr)
		{
			int len;
			stream.Read(out len);
			arr = new ClientData[len];
			for(int i = 0; i < len; ++i)
				stream.Read(out arr[i]);
		}

	}
	public class IRemoteMonitorProxyImpl : ProxyImplBase, IRemoteMonitor
	{
		public IRemoteMonitorProxyImpl(Connection conn) : base(conn)
		{
		}
		void IRemoteMonitor.UpdateData(RemoteMonitor.ClientData data)
		{
			BinaryStreamWriter __stream = new BinaryStreamWriter();
			__stream.Write(1);
			__stream.Write(data);
			Connection.Write(__stream.BuildSendBuffer());
			BinaryStreamReader __reader = new BinaryStreamReader(Connection.Read());
			int __errCode;
			__reader.Read(out __errCode);
			if(__errCode == 0)
			{
				return;
			}
			else
			{
				string __errMsg;
				__reader.Read(out __errMsg);
				throw new ServiceException(__errCode, __errMsg);
			}
		}
	}
}
