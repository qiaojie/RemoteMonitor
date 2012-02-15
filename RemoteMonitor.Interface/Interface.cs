using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using EGL;
using EGL.Communication;

namespace RemoteMonitor
{
	[Serializable()]
	public class ClientData
	{
		public string Name;
		public int CPU;
		public long IdleTime;
		public long KernelTime;
		public long UserTime;
		public long CommitTotal;
		public long CommitLimit;
		public long CommitPeak;
		public long PhysicalTotal;
		public long PhysicalAvailable;
		public long SystemCache;
		public long KernelTotal;
		public long KernelPaged;
		public long KernelNonpaged;

		public int PageSize;
		public int HandleCount;
		public int ProcessCount;
		public int ThreadCount;
	}

	[Protocol(ID = 0x489ABE4F, Version = 1)]
	public interface IRemoteMonitor
	{
		[DispId(1)]
		void UpdateData(ClientData data);
	}
}
