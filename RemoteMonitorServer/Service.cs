using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using EGL;
using EGL.Communication;

namespace RemoteMonitor
{
	class RemoteMonitorService : ServiceBase, IRemoteMonitorImpl, ICmdline
    {

		public override void OnConnection(Session client)
		{
		}

		#region IRemoteMonitorImpl Members

		public void UpdateData(Session session, ClientData data)
		{
			Log.Info("收到客户端数据\n客户端名：{0}\n内存使用情况: \n", data.Name);
			int MB = 1024 * 1024 / data.PageSize;

			Log.Append("	提交: {0} MB\n", data.CommitTotal / MB);
			Log.Append("	最大可提交: {0} MB\n", data.CommitLimit / MB);
			Log.Append("	提交峰值: {0} MB\n", data.CommitPeak / MB);
			Log.Append("	物理内存总量：{0} MB\n", data.PhysicalTotal / MB);
			Log.Append("	物理内存可用：{0} MB\n", data.PhysicalAvailable / MB);
			Log.Append("	系统缓存：{0} MB\n", data.SystemCache / MB);
			Log.Append("	核心占用：{0} MB\n", data.KernelTotal / MB);
			Log.Append("	核心分页：{0} MB\n", data.KernelPaged / MB);
			Log.Append("	核心非分页：{0} MB\n", data.KernelNonpaged / MB);
			Log.Append("	句柄数：{0} \n", data.HandleCount);
			Log.Append("	进程数：{0} \n", data.ProcessCount);
			Log.Append("	线程数：{0} \n", data.ThreadCount);
			Log.Append("CPU使用情况\n");
			Log.Append("	空闲时间：{0}秒\n", data.IdleTime / 10000000);
			Log.Append("	内核时间：{0}秒\n", data.KernelTime / 10000000);
			Log.Append("	用户时间：{0}秒\n", data.UserTime / 10000000);
			
			//throw new NotImplementedException();
		}

		#endregion
	}
}
