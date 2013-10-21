using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Xml;

namespace Replace
{
    static class Program
    {
        private static bool forPublic = false; // 是否开启public版功能

        [DllImport("kernel32.dll")]
        public static extern void Sleep(int miliSec);

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());
            try
            {
                if (args.Length < 1)
                {
                    Application.Exit();
                }

                Sleep(200);

                //int processId = Int32.Parse(args[0]);

                //System.Diagnostics.Process process = System.Diagnostics.Process.GetProcessById(processId);
                //// System.Windows.Forms.MessageBox.Show(processId.ToString());
                //process.Kill();

                //process.WaitForExit(2000);

                //MessageBox.Show("aaa");

                string path = System.IO.Directory.GetCurrentDirectory();

                if (forPublic)
                {
                    if (System.IO.File.Exists(path + "\\UpdateProgram_Public.exe.temp"))
                    {
                        File.Delete(path + "\\UpdateProgram_Public.exe");
                        File.Move(path + "\\UpdateProgram_Public.exe.temp", path + "\\UpdateProgram_Public.exe");
                    }

                    if (File.Exists(path + "\\update_public.xml"))
                    {
                        if (File.Exists(path + "\\local.xml"))
                            File.Delete(path + "\\local.xml");
                        File.Move(path + "\\update_public.xml", path + "\\local.xml");
                    }
                }
                else
                {
                    if (System.IO.File.Exists(path + "\\UpdateProgram.exe.temp"))
                    {
                        File.Delete(path + "\\UpdateProgram.exe");
                        File.Move(path + "\\UpdateProgram.exe.temp", path + "\\UpdateProgram.exe");
                    }

                    if (File.Exists(path + "\\update.xml"))
                    {
                        if (File.Exists(path + "\\local.xml"))
                            File.Delete(path + "\\local.xml");
                        File.Move(path + "\\update.xml", path + "\\local.xml");
                    }
                }
                
                //add by cuiwei 07.8.21
                XmlDocument docRemote = new XmlDocument();
                docRemote.Load(Application.StartupPath + "\\local.xml");
                XmlElement rootRemote = docRemote.DocumentElement;
                XmlNode updateNode = rootRemote.SelectSingleNode("PostUpdate");
                //int count = int.Parse(updateNode.Attributes["count"].Value);
                int count = updateNode.ChildNodes.Count;
                for (int i = 0; i < count; i++)
                {
                    XmlNode itemNode = updateNode.ChildNodes[i];
                    string filename = itemNode.Attributes["name"].Value;
                    string arg = "", sync = "";
                    try
                    {
                        arg = itemNode.Attributes["arg"].Value;
                        sync = itemNode.Attributes["async"].Value;
                    }
                    catch
                    {
                        sync = "false";
                    }
                    Command.Execute(filename, arg, sync);
                }
                //add end

                //Process.Start("GameDesingerTools.exe");
                //Process.Start("GameDesingerTools.exe", "-startup");
            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
            }
        }

        //add by cuiwei 07.8.21
        class Command
        {
            ///<summary>   
            ///运行指定命令行   
            ///</summary>   
            ///<param name="cmd">命令</param>   
            ///<param name="arg">命令行参数</param>    
            ///<returns></returns>   
            public static void Execute(string cmd, string arg, string sync)
            {
                Process p = new Process();
                p.StartInfo.FileName = cmd;
                p.StartInfo.Arguments = arg;
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.RedirectStandardInput = true;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardError = true;
                p.StartInfo.CreateNoWindow = true;
                p.Start();
                if("false" == sync)
                {
                    p.WaitForExit();
                    p.Close();
                }
            }
        } 
        //add end

    }
}