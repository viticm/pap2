using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace MakeIDL
{
    public partial class GetClassForm : Form
    {
        private string m_strClassFile = "";
        private string m_strClassDefine = "";
        private string m_strInheritClassName = "";
        private bool m_bIsInherited = false;
        public GetClassForm()
        {
            InitializeComponent();
        }
        public string GetClassName()
        {
            return tbxClass.Text;
        }
        public string GetFilePath()
        {
            return tbxFileName.Text;
        }
        public string GetClassDefineDescription()
        {
            return m_strClassDefine;
        }
        public string GetInheritedClassName()
        {
            return m_strInheritClassName;
        }
        public bool IsInherited()
        {
            return m_bIsInherited;
        }
        public void ReInit(string strClassName)
        {
            m_strInheritClassName = "";
            m_strClassFile = "";
            m_strClassDefine = "";
            m_bIsInherited = false;
            tbxFileName.Text = "";
            tbxClass.Text = strClassName;
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.ShowDialog();
            tbxFileName.Text = dlg.FileName;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (!CheckClass())
            {
                return;
            } 
            this.DialogResult = DialogResult.OK;
        }

        protected bool CheckClass()
        {
            if (tbxFileName.Text == "" || tbxClass.Text == "")
            {
                MessageBox.Show("请填写类名或者文件名", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            try
            {
                FileStream fs = new FileStream(tbxFileName.Text, FileMode.Open, FileAccess.Read);
                StreamReader sr = new StreamReader(fs);
                m_strClassFile = sr.ReadToEnd();
                sr.Close();
                fs.Close();
            }
            catch (System.Exception exp)
            {
                MessageBox.Show(exp.ToString(), "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            RemoveNote();
            int nClassBeginIndex = GetClassDefineBeginIndex();
            if (nClassBeginIndex == -1)
            {
                string str = string.Format("指定文件中没有类{0}的定义", tbxClass.Text);
                MessageBox.Show(str, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            GetClassDefine(nClassBeginIndex);
            return true;
        }
        /// <summary>
        /// 删除注释
        /// </summary>
        protected void RemoveNote()
        {
            int nIndex = 0;//表示/*的注释索引
            int nIndex2 = 0;//表示//的注释索引
            int nMinIndex = 0;
            int nEndIndex = 0;
            int nLen = 0;
            while (true)
            {
                nIndex = m_strClassFile.IndexOf(@"/*", nMinIndex);
                nIndex2 = m_strClassFile.IndexOf(@"//", nMinIndex);
                if (nIndex2 != -1 && (nIndex2 < nIndex || nIndex == -1))// 注释//在前
                {
                    nEndIndex = m_strClassFile.IndexOf("\r\n", nIndex2);
                    if (nEndIndex == -1)
                    {
                        nLen = m_strClassFile.Length - nIndex2;
                    }
                    else
                    {
                        nLen = nEndIndex - nIndex2;
                    }
                    m_strClassFile = m_strClassFile.Remove(nIndex2, nLen);
                    nMinIndex = nIndex2;
                }
                else if (nIndex != -1 && (nIndex < nIndex2 || nIndex2 == -1))//注释/*在前
                {
                    nEndIndex = m_strClassFile.IndexOf(@"*/", nIndex);
                    nLen = nEndIndex - nIndex + 2;
                    m_strClassFile = m_strClassFile.Remove(nIndex, nLen);
                    nMinIndex = nIndex;
                }
                else                                                       //没有注释了
                {
                    break;
                }
            }
        }

        /// <summary>
        /// 获取类定义文件中指定类的开始定义索引
        /// </summary>
        /// <returns>-1表示没有该类的定义</returns>
        protected int GetClassDefineBeginIndex()
        {
            int nResultIndex = -1;
            int nTempIndex = -1;
            string str1 = string.Format("interface {0}", tbxClass.Text);
            string str2 = string.Format("class {0}", tbxClass.Text);

            nResultIndex = m_strClassFile.IndexOf(str1, 0);
            if (nResultIndex != -1)
            {
                nTempIndex = m_strClassFile.IndexOf(str1, nResultIndex);
                if (nTempIndex != -1)
                {
                    nResultIndex = nTempIndex;
                }
            }
            else
            {
                nResultIndex = m_strClassFile.IndexOf(str2, 0);
                if (nResultIndex != -1)
                {
                    nTempIndex = m_strClassFile.IndexOf(str2, nResultIndex);
                    if (nTempIndex != -1)
                    {
                        nResultIndex = nTempIndex;
                    }
                }
            }
            return nResultIndex;
        }
        /// <summary>
        /// 获取类的定义添加到列表m_lstCls
        /// </summary>
        /// <param name="nBeginIndex"></param>
        /// <returns>返回继承的类名</returns>
        protected void GetClassDefine(int nBeginIndex)
        {
            int nIndex = -1;
            int nDefineEndIndex = -1;
            int nDefineBeginIndex = -1;           
            string strPublic = "public";            

            nDefineBeginIndex = m_strClassFile.IndexOf('{', nBeginIndex);
            nDefineEndIndex = m_strClassFile.IndexOf("};", nBeginIndex);
            nIndex = m_strClassFile.IndexOf(strPublic, nBeginIndex, nDefineBeginIndex - nBeginIndex);
            if (nIndex != -1)
            {
                nIndex += strPublic.Length;
                string strTrim = " \r\n";
                m_strInheritClassName = m_strClassFile.Substring(nIndex, nDefineBeginIndex - nIndex);
                m_strInheritClassName = m_strInheritClassName.Trim(strTrim.ToCharArray());
                m_bIsInherited = true;
            }
            m_strClassDefine = m_strClassFile.Substring(nDefineBeginIndex + 1, nDefineEndIndex - nDefineBeginIndex - 1); 
            
        }

        private void btnCancle_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            m_strInheritClassName = "";
            m_strClassFile = "";
            m_strClassDefine = "";
            m_bIsInherited = false;
            tbxFileName.Text = "";
            tbxClass.Text = "";
        }
    }
}