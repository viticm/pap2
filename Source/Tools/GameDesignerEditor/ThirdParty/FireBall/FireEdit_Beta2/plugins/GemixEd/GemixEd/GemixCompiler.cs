#region License

//    Copyright (C) 2004  Sebastian Faltoni sebastian(at)dotnetfireball(dot)net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Diagnostics;
using System.ComponentModel;

namespace GemixEd
{
    public class GemixCompiler
    {
        private Thread _Thread = null;

        public event EventHandler OutputReceived;

        private SendOrPostCallback _OutputCallBack = null;
        private AsyncOperation _AsyncOperation = null;

        public GemixCompiler()
        {
            _Thread = new Thread(new ParameterizedThreadStart(gemix_compiler));
            _Thread.Priority = ThreadPriority.Normal;
            _Thread.IsBackground = true;

            _OutputCallBack = new SendOrPostCallback(OutputCallBack);

            _AsyncOperation = AsyncOperationManager.CreateOperation(null);
        }

        void OutputCallBack(object obj)
        {
            if (OutputReceived != null)
                OutputReceived(obj, EventArgs.Empty);
        }

        public void Start(string gemixCompiler, string prgFile)
        {
            _Thread.Start(new string[] { gemixCompiler, prgFile });
        }

        void gemix_compiler(object arg)
        {
            string gemixCompiler = ((object[])arg)[0].ToString();
            string prgFile = ((object[])arg)[1].ToString();

            ProcessStartInfo procInfo = new ProcessStartInfo("\"" + gemixCompiler + "\"", "\"" + prgFile +"\"");

            procInfo.CreateNoWindow = true;
            procInfo.UseShellExecute = false;
            procInfo.WindowStyle = ProcessWindowStyle.Hidden;
            procInfo.RedirectStandardOutput = true;

            Process process = new Process();

            process.StartInfo = procInfo;

            process.Start();

            process.WaitForExit();

            _AsyncOperation.Post(_OutputCallBack, process.StandardOutput.ReadToEnd());
        }

        void process_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            SynchronizationContext.Current.Post(_OutputCallBack, e.Data);
        }
    }
}
