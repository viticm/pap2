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
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using FireEdit;
using Microsoft.Win32;
using System.Diagnostics;
using System.IO; 
using System.Runtime.InteropServices;
using System.Threading;
using Fireball.Syntax;

namespace FirePHP
{
    [StructLayout(LayoutKind.Sequential)]
    public struct SECURITY_ATTRIBUTES
    {
        [MarshalAs(UnmanagedType.Bool)]
        public bool bInheritHandle;
        public IntPtr lpSecurityDescriptor;
        public int nLength;
    }

    public class PHPToolBar : ToolStrip
    {
        private FireEditApplication App = null;

        private ToolStripButton btnCheckSyntax = null;
        private ToolStripButton btnRunScript = null;

        public PHPToolBar()
        {
            App = (FireEditApplication)FireEditApplication.Istance;

            btnCheckSyntax = new ToolStripButton(Properties.Resources.document_check);
            btnCheckSyntax.Click += new EventHandler(_CheckSyntax_Click);
            btnCheckSyntax.Size = new System.Drawing.Size(22, 22);
            btnCheckSyntax.ToolTipText = "Check Syntax Only";

            this.Items.Add(btnCheckSyntax);
            this.Items.Add("-");

            btnRunScript = new ToolStripButton(Properties.Resources.RunScript);
            btnRunScript.ToolTipText = "Run Script";

            this.Items.Add(btnRunScript);

            btnRunScript.Click += new EventHandler(btnRunScript_Click);


            this.ImageScalingSize = App.MainForm.MainToolStrip.ImageScalingSize;

            this.Size = new Size(10, 25);


            this.AutoSize = true;
        }

        void btnRunScript_Click(object sender, EventArgs e)
        {
            if (!App.MainForm.CurrentEditor.Saved)
            {
                MessageBox.Show("Save File for continue syntax check!");
                return;
            }

            RegistryKey key = Config.GetRegistryKey("PHP");

            string compilerPath = (string)key.GetValue("Compiler");

            if (compilerPath == null)
            {
                OpenFileDialog opf = new OpenFileDialog();

                opf.Title = "PHP Compiler Executable - php.exe";

                opf.Filter = "PHP.exe|php.exe";

                if (opf.ShowDialog() != DialogResult.OK)
                    return;

                compilerPath = opf.FileName;

                key.SetValue("Compiler", compilerPath);
            }

            ProcessStartInfo info = new ProcessStartInfo(compilerPath, " \"" + App.MainForm.CurrentEditor.FileName + "\"");
            info.RedirectStandardOutput = true;
            info.UseShellExecute = false;
            info.CreateNoWindow = true;

            Process process = Process.Start(info);

            process.WaitForExit();

            string output = process.StandardOutput.ReadToEnd();

            App.MainForm.OutputWindow.Clear();
            App.MainForm.OutputWindow.AppendOutput(output);

            App.MainForm.OutputWindow.Show(App.MainForm.DockContainer, Fireball.Docking.DockState.DockBottom);
        }


        Row highlitedLine = null;

        void ProcessError(string errorOutput)
        {
            try
            {
                int index = errorOutput.IndexOf("on line");

                if (index == -1)
                    return;

                string line = errorOutput.Substring(index + "on line".Length + 1);

                line = line.Substring(0, line.IndexOf("\n"));

                int lineNumber = 0;

                if (int.TryParse(line, out lineNumber))
                {
                    if (highlitedLine != null)
                        highlitedLine.BackColor = Color.White;

                    lineNumber--;
                    ((FormMain)App.MainForm).CurrentEditor.GotoLine(lineNumber);

                    ((FormMain)App.MainForm).CurrentEditor.Document[lineNumber].BackColor = Color.HotPink;
                    highlitedLine = ((FormMain)App.MainForm).CurrentEditor.Document[lineNumber];
                }
            }
            catch
            {

            }
        }

        void _CheckSyntax_Click(object sender, EventArgs e)
        {

            if (!App.MainForm.CurrentEditor.Saved)
            {
                MessageBox.Show("Save File for continue syntax check!");
                return;
            }

            RegistryKey key = Config.GetRegistryKey("PHP");

            string compilerPath = (string)key.GetValue("Compiler");

            if (compilerPath == null)
            {
                OpenFileDialog opf = new OpenFileDialog();

                opf.Title = "PHP Compiler Executable - php.exe";

                opf.Filter = "PHP.exe|php.exe";

                if (opf.ShowDialog() != DialogResult.OK)
                    return;

                compilerPath = opf.FileName;

                key.SetValue("Compiler", compilerPath);
            }

            ProcessStartInfo info = new ProcessStartInfo(compilerPath, " -l \"" + App.MainForm.CurrentEditor.FileName + "\"");
            info.RedirectStandardOutput = true;
            info.UseShellExecute = false;
            info.CreateNoWindow = true;

            Process process = Process.Start(info);

            process.WaitForExit();

            string output = process.StandardOutput.ReadToEnd();

            App.MainForm.OutputWindow.Clear();
            App.MainForm.OutputWindow.AppendOutput(output);

            App.MainForm.OutputWindow.Show(App.MainForm.DockContainer, Fireball.Docking.DockState.DockBottom);

            if (process.ExitCode != 0)
                ProcessError(output);
            else
            {

                if (highlitedLine != null)
                    highlitedLine.BackColor = Color.White;
            }
        }
    }
}
