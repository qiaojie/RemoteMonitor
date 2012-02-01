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
		public long Memory;
	}

	[Protocol(ID = 0x489ABE4F, Version = 1)]
	public interface IRemoteMonitor
	{
		[DispId(1)]
		void UpdateData(ClientData data);
	}
}
