using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using EGL;
using EGL.Communication;
using System.Diagnostics;

namespace RemoteMonitor
{
	public interface ICmdline
	{
	}

	public class program
	{
		public static STASynchronizationContext mainContext;

		public static void RunCmd(string[] cmds)
		{
			ICmdline cmd = ServiceHodler<RemoteMonitorService>.Service;
			var type = typeof(ICmdline);
			var method = type.GetMethod(cmds[0]);
			if (method == null)
			{
				Console.Error.WriteLine("Unknown command.");
				return;
			}
			var paramInfos = method.GetParameters();
			if (paramInfos.Length != cmds.Length - 1)
			{
				Console.Error.WriteLine("Error argument number.");
				Console.Error.Write("Uasge: {0} ", cmds[0]);
				foreach (var p in paramInfos)
					Console.Error.Write("{0} ", p.Name);
				Console.Error.WriteLine();
				return;
			}
			object[] paramters = new object[paramInfos.Length];
			for (var i = 0; i < paramInfos.Length; ++i)
				paramters[i] = Convert.ChangeType(cmds[i + 1], paramInfos[i].ParameterType);
			try
			{
				mainContext.Send(obj => { method.Invoke(cmd, paramters); }, null);
			}
			catch (Exception e)
			{
				while (e.InnerException != null)
					e = e.InnerException;
				Console.Error.WriteLine("error: {0}", e.Message);
				Console.Error.WriteLine("call stack:{0}", e.StackTrace);
			}
		}

		public static void InputThread()
		{
			var thread = new Thread(obj =>
			{
				Thread.CurrentThread.Name = "Input";
				while (true)
				{
					string cmd = Console.In.ReadLine();
					if (cmd == null)
					{
						ServiceHodler<RemoteMonitorService>.Close();
						mainContext.Exit();
						break;
					}
					if (cmd == "")
						continue;
					var cmdArgs = cmd.SplitArgs();
					if (cmdArgs[0].ToLower() == "exit")
					{
						ServiceHodler<RemoteMonitorService>.Close();
						mainContext.Exit();
						break;
					}
					switch (cmdArgs[0])
					{
						case "Debug":
							System.Diagnostics.Debugger.Launch();
							break;
						case "LogFlush":
							Log.Flush();
							break;
						case "LogEnable":
							if (cmdArgs.Length != 3)
								Console.Error.WriteLine("Error argument number.");
							Log.LogEnable((LogCategory)Enum.Parse(typeof(LogCategory), cmdArgs[1]), bool.Parse(cmdArgs[2]));
							break;
						default:
							RunCmd(cmdArgs);
							break;
					}
					Log.Flush();
				}
			});
			thread.Start();
		}

		static void Main(string[] args)
		{
			try
			{
				//Console.InputEncoding = Encoding.Default;
				//Console.OutputEncoding = Encoding.UTF8;
				//Log.OnLogEvent += new Log.OnLog(AppendLog);
				Thread.CurrentThread.Name = "Main";
				mainContext = new STASynchronizationContext();
				SynchronizationContext.SetSynchronizationContext(mainContext);
				ServiceHodler<RemoteMonitorService>.Start(14000);
				ServiceHodler<RemoteMonitorService>.SetUpdateInterval(200);
				InputThread();
				mainContext.Run();
				Log.Info("service exit");
			}
			catch (Exception e)
			{
				Log.Fatal("program crash. Error message: {0}.\n	CallStack:{1}", e.Message, e.StackTrace);
			}
			Log.Flush();
		}
	}
}