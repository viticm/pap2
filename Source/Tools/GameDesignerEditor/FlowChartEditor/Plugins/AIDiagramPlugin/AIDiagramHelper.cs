using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using luaEditor;

namespace AIDiagramPlugin
{
    public class AIDiagramHelper
    {               
        /// <summary>
        /// 查询对象
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        public static void QueryObjects(object sender, luaEditor.QueryEventsArgs e)
        {            
            // 参数规定：
            // szCode: 所有的代码
            // nPos: 光标位置（汉字按2个字符算）
            // nIsDelete: 0为正常输入，1为删除，
            // 11-22为F1-F12,
            // 1111代表Ctrl+Shift+F1,
            // 1011代表Ctrl+F1
            // 111代表Shift+F1
            int nIsDelete = e.nPos > 0 ? 0 : 1;

            if (e.keyword != "")
            {
                nIsDelete = 10 + int.Parse(e.keyword);
            }

            IntPtr ls_out = IntPtr.Zero;
            LuaRuntime.Init();
            bool bret = LuaRuntime.LuaRun("OnCodeSense", e.szCode, Math.Abs(e.nPos), nIsDelete, "true,1024,显示所有,1,false,语法检查", ref ls_out);
            string strOut = Marshal.PtrToStringAnsi(ls_out);

            if (string.IsNullOrEmpty(strOut))
            {
                return;
            }

            string[] as_data = strOut.Split(new char[] { '|' });

            if (as_data.Length < 3)
            {
                MessageBox.Show("返回值不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 读取overlen
            e.nOverLen = int.Parse((as_data[0]));

            // 初始化游标
            int nPosition = 3;

            // 读取list
            int nListCount = int.Parse((as_data[1]));

            if (as_data.Length < 3 + nListCount)
            {
                MessageBox.Show("List返回值个数不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            for (int i = 0; i < nListCount; i++)
            {
                string s_name = as_data[nPosition++];
                string s_word = as_data[nPosition++];
                string s_type = as_data[nPosition++];
                string s_info = as_data[nPosition++];
                e.leTable.Add(s_name, new luaEditor.LuaEditorTableItem(s_name, s_type, s_word, s_info));                
            }

            // 读取info
            int nInfoCount = Int32.Parse((as_data[2]));

            if (as_data.Length < 3 + nListCount + nInfoCount)
            {
                MessageBox.Show("Info返回值个数不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            for (int i = 0; i < nInfoCount; i++)
            {
                string s_text = as_data[nPosition++] as string;
                e.parms_list.Add(s_text.Replace("\n", "<BR>"));
            }
        }

        /// <summary>
        /// 检查脚本
        /// </summary>
        /// <param name="codeEditBox">脚本编辑控件</param>
        /// <param name="content">脚本内容</param>
        public static bool CheckScript(luaEditorControl codeEditBox, string content)
        {
            string folderName = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck");
            string scriptFilePath = Path.Combine(folderName, "tmp.lua");
            bool passCheck = true;

            AITableExport.WriteStringToFile(content, scriptFilePath);
            // string checkFilePath = Path.Combine(folderName, "ExecLua.exe");
            string checkFilePath = Path.Combine(folderName, "luac5.1.exe");

            //执行检查工具
            if (File.Exists(checkFilePath))
            {
                Process p = new Process();
                p.StartInfo.FileName = checkFilePath;
                p.StartInfo.Arguments = string.Format("\"{0}\"", scriptFilePath);
                p.StartInfo.WorkingDirectory = folderName;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardError = true;
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.CreateNoWindow = true;
                p.Start();

                StreamReader stream = p.StandardOutput;
                StreamReader streamErr = p.StandardError;

                StringBuilder checkString = new StringBuilder();

                while (!p.HasExited)
                {
                    checkString.AppendLine(stream.ReadLine());
                    checkString.AppendLine(streamErr.ReadLine());
                }

                checkString.AppendLine(stream.ReadLine());
                checkString.AppendLine(streamErr.ReadLine());
                string checkResult = checkString.ToString().Trim(new char[] { '\r', '\n' });
                checkResult = checkResult.Replace(string.Format("{0}:", checkFilePath), "");

                p.WaitForExit();
                File.Delete(scriptFilePath);

                if (!string.IsNullOrEmpty(checkResult)) // 脚本通过检查
                {
                    StringBuilder checkInfo = new StringBuilder();
                    string[] dataArray = checkResult.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);

                    if (dataArray.Length > 1)
                    {
                        for (int i = 1; i < dataArray.Length; i++)
                        {
                            checkInfo.Append(string.Format("{0}:", dataArray[i]));
                        }

                        checkInfo.Remove(checkInfo.Length - 1, 1);
                        MessageBox.Show(checkInfo.ToString(), "lua脚本检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        passCheck = false;

                        //尝试跳转到出错行
                        if (codeEditBox != null)
                        {
                            int lineIndex = -1;

                            if (int.TryParse(dataArray[1], out lineIndex))
                            {
                                codeEditBox.MoveToLine(lineIndex);
                            }
                        }                        
                    }
                }
            }

            return passCheck;
        }
    }
}
