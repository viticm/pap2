using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace ScriptViewer
{
    public delegate void OnSaveEventsHandle(object sender, OnSaveEventArgs e);

    public partial class frmScriptEdit : Form
    {
        private LuaRuntime luaRuntime = LuaRuntime.GetLuaRunTime();
        private ScriptLock m_slLock;
        private bool m_bLocked = false;//是否有加锁记录
        private bool m_bLocalLocked = false;//本机加锁
        private string m_strCode = "";
        private frmScriptView m_fsv;
        public string fieldid = "";
        public string tabid = "";
        public string filename = "";
        public object tag = null;

        public event OnSaveEventsHandle OnSave = null;

        public frmScriptEdit()
        {
            InitializeComponent();
        }

        //改变显示文本和真实文本
        public string strCode
        {
            get { return luaEditorControl1.Text; }
            set
            {
                m_strCode = value;
                luaEditorControl1.Text = m_strCode;
            }
        }

        //仅仅改变显示文本
        public void setCode(string strcode)
        {
            luaEditorControl1.Text = strcode;
        }

        /// <summary>
        /// 是否被改变
        /// </summary>
        public bool isChanged
        {
            get { return luaEditorControl1.Text != m_strCode; }
        }

        private void luaEditorControl1_OnChange(object sender, EventArgs e)
        {
            if (this.fieldid == null || this.tabid == null)
                return;

            m_fsv = this.MdiParent as frmScriptView;
            m_slLock = new ScriptLock(m_fsv.Conn, Convert.ToInt32(this.fieldid), Convert.ToInt32(this.tabid), "", "");
            if (!m_bLocalLocked && !m_bLocked)
            {
                //本机未加锁,没有加锁记录  尝试加锁
                if (m_slLock.Lock())
                {
                    //本机已加锁，加锁成功
                    m_bLocalLocked = true;
                    m_bLocked = false;//以前无加锁记录
                }
                else
                {
                    //已有加锁记录，加锁失败
                    m_bLocked = true;
                    m_bLocalLocked = false;
                    MessageBox.Show("该内容已被" + m_slLock.GetServerLastLockUser() + "锁定，当前的修改不可以被保存！");
                }
            }
            if (isChanged && !this.Text.EndsWith("*"))
            {
                this.Text += "*";
            }
        }

        private void frmScriptEdit_FormClosing(object sender, FormClosingEventArgs e)
        {
            //关闭前先确认存否
            if(isChanged && m_bLocalLocked)
            {
                if (MessageBox.Show("内容已改变是否保存？", "是否保存", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    //保存
                    bool r = this.Save();
                    if (!r)
                    {
                        MessageBox.Show("保存失败");
                        e.Cancel = true;
                        return;
                    }
                }
                else
                {
                    //不保存，也要解锁
                    if (m_bLocalLocked)
                    {
                        m_slLock.UnLock();
                        m_bLocked = false;
                        m_bLocalLocked = false;
                    }
                }
            }
        }

        /// <summary>
        /// 保存文档到数据库
        /// </summary>
        /// <returns></returns>
        public bool Save()
        {
            m_fsv = this.MdiParent as frmScriptView;
            if(this.filename != "")
            {
                //保存文件到磁盘
                StringToFile(luaEditorControl1.Text, filename);
            }
            else
            {
                //保存脚本到dbclass
                if(this.OnSave != null && m_bLocalLocked)
                {
                    OnSaveEventArgs osea = new OnSaveEventArgs(this.fieldid, this.tabid, luaEditorControl1.Text, this.tag);
                    this.OnSave(this, osea);
                    //m_slLock = new ScriptLock(m_fsv.Conn, Convert.ToInt32(this.fieldid), Convert.ToInt32(this.tabid), "", "");
                    m_slLock.UnLock();
                    m_bLocalLocked = false;
                    m_bLocked = false;

                    if( osea.fieldid == "NG")
                    {
                        return false;
                    }
                }
                else if (!m_bLocalLocked && m_bLocked)
                {
                    MessageBox.Show("内容被锁定，无法保存到数据库！");
                }
            }
            
            this.strCode = luaEditorControl1.Text;
            if (this.Text.EndsWith("*")) this.Text = this.Text.Substring(0, this.Text.Length - 1);

            return true;
        }

        // 保存string到文件
        static public void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }
        static public void WriteStringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName); // 解决乱码关键在于Encoding
        }
        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                Directory.CreateDirectory(Path.GetFullPath(strFileName));
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }

        private void luaEditorControl1_queryObjects(object sender, luaEditor.QueryEventsArgs e)
        {
            string strOut = "";
            IntPtr ls_out = IntPtr.Zero;

            //参数规定：
            //szCode:   所有的代码
            //nPos:     光标位置（汉字按2个字符算）
            //nIsDelete 0为正常输入，1为删除，
            //  11-22为F1-F12,
            //  1111代表Ctrl+Shift+F1,
            //  1011代表Ctrl+F1
            //   111代表Shift+F1
            int nIsDelete = e.nPos > 0 ? 0 : 1;

            if (e.keyword != "")
            {
                nIsDelete = 10 + Int32.Parse(e.keyword);
            }

            bool bret = LuaRuntime.LuaRun("OnCodeSense", e.szCode, Math.Abs(e.nPos), nIsDelete, "true,1024,显示所有,1,false,语法检查", ref ls_out);
            strOut = Marshal.PtrToStringAnsi(ls_out);

            if (bret == false)
            {
                MessageBox.Show(strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (strOut == null)
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
            e.nOverLen = Int32.Parse((as_data[0]));

            // 初始化游标
            int nPosition = 3;

            // 读取list
            int nListCount = Int32.Parse((as_data[1]));
            if (as_data.Length < 3 + nListCount)
            {
                MessageBox.Show("List返回值个数不正确，返回值为：" + strOut, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            for (int i = 0; i < nListCount; i++)
            {
                string s_name = as_data[nPosition++] as string;
                string s_word = as_data[nPosition++] as string;
                string s_type = as_data[nPosition++] as string;
                string s_info = as_data[nPosition++] as string;
                e.leTable.Add(s_name, new luaEditor.leTableItem(s_name, s_type, s_word, s_info));

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

        public void GoToPosition(FindResultPosition position)
        {
            if (position == null)
            {
                return;
            }
            int LineNumber = position.LineNumber;
            int ColNumber = position.ColNumber;
            int Length = position.Text.Length;
            //this.luaEditorControl1.MoveToLine(LineNumber);
            //add by wangying1  2009-7-23
            this.luaEditorControl1.SelectText(LineNumber - 1, ColNumber, Length);
          
            //end of add 
        }

        private void luaEditorControl1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F3 && e.Control)
            {
                this.luaEditorControl1.SetupFindText();
                this.luaEditorControl1.FindNextSelectText();
            }
            else if (e.KeyCode == Keys.F3 && e.Shift)
            {
                this.luaEditorControl1.FindPreviousSelectText();
            }
            else if (e.KeyCode == Keys.F3)
            {
                this.luaEditorControl1.FindNextSelectText();
            }

        }

    }

    public class OnSaveEventArgs : EventArgs
    {
        public OnSaveEventArgs(string fieldid, string tabid, string strcode, object tag)
        {
            this.strcode = strcode;
            this.fieldid = fieldid;
            this.tabid = tabid;
            this.tag = tag;
        }
        public string strcode = null;
        public string fieldid = "";
        public string tabid = "";
        public object tag = null;
    };


}