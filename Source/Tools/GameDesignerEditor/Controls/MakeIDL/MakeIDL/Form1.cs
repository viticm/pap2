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
    public partial class MakeIDLForm : Form
    {
       
        private int m_nPart1EndIndex = 0;//idl上部分结束字符的索引
        private int m_nPart3BeginIndex = 0;//idl下部分开始字符的索引
        
        private string m_strWholeFile;
        private string m_strPart2;
        private string m_strClassFile;
        private string m_strOldIdl;

        private List<KTransferOneFunction> listTransferFuncs = new List<KTransferOneFunction>();

        private string m_strIdlPath = "";
        private List<string> m_lstCls = new List<string>();
        OpenFileDialog m_dlg = new OpenFileDialog();
        SaveFileDialog saveFileDialogIDL = new SaveFileDialog();
       
        public MakeIDLForm()
        {
            InitializeComponent();            
        }
      

        /// <summary>
        /// 读取idl文件到string里
        /// </summary>
        protected void ReadidlFile2String()
        {
            try
            {
                FileStream fsIDL = new FileStream(m_strIdlPath, FileMode.Open, FileAccess.ReadWrite);
                StreamReader srIDL = new StreamReader(fsIDL);
                m_strWholeFile = srIDL.ReadToEnd();
                srIDL.Close();
                fsIDL.Close();
            }
            catch (System.Exception e)
            {
                System.Diagnostics.Trace.Write(e.ToString());
            }
            
        }
        
        /// <summary>
        /// 分析idl文件string，获取接口名称, 初始化接口列表
        /// </summary>
        protected void InitLbxInterfaces()
        {
            lbxInterfaces.Items.Clear();
            ReadidlFile2String();
            string strInterfaces = "";
            int nBegin = 0;
            int nEnd = 0;
            nBegin = m_strWholeFile.IndexOf("interface", 0);
            if (nBegin == -1)
            {
                MessageBox.Show("文件不含接口idl的定义", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            nEnd = m_strWholeFile.IndexOf('[', nBegin);
            strInterfaces = m_strWholeFile.Substring(nBegin, nEnd - nBegin);

            int nIBegin = 0;
            int nIEnd = 0;
            string strOneInterface = "";//保存一个接口名称
            while (nBegin != -1)
            {
                nIBegin = strInterfaces.IndexOf('I', nIEnd);
                nIEnd = strInterfaces.IndexOf(';', nIBegin);
                strOneInterface = strInterfaces.Substring(nIBegin, nIEnd - nIBegin);
                lbxInterfaces.Items.Add(strOneInterface);
                nBegin = strInterfaces.IndexOf("interface", nIEnd);
            }
            if (lbxInterfaces.Items.Count > 0)
            {
                lbxInterfaces.SelectedIndex = 0;
            }

        }

        /// <summary> 
        /// 根据列表里选择的接口名称设置两个索引
        /// </summary>
        protected void SetIndexs()
        {
            if (lbxInterfaces.SelectedIndex == -1)
            {
                return;
            }
            string strInterfaceName = lbxInterfaces.SelectedItem.ToString();
            string str = "interface " + strInterfaceName + " : IUnknown{";
            m_nPart1EndIndex = m_strWholeFile.IndexOf(str, 0) + str.Length - 1;
            m_nPart3BeginIndex = m_strWholeFile.IndexOf("};", m_nPart1EndIndex);
            m_strOldIdl = m_strWholeFile.Substring(m_nPart1EndIndex + 1, m_nPart3BeginIndex - m_nPart1EndIndex - 1);
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            m_dlg.Filter = "类定义文件 (*.h)|*.h";           
            DialogResult dr = DialogResult.Cancel;
            try
            {
                dr = m_dlg.ShowDialog(this);
            }
            catch(SystemException exp)
            {
                System.Diagnostics.Trace.Write(exp.ToString());
                return;
            }
            
            if (dr == DialogResult.OK)
            {
                tbxFileName.Text = m_dlg.FileName;
            }
            
        }      
               
        private void btnFinish_Click(object sender, EventArgs e)
        {
            if (!ReadClassFile2String())
            {
                return;
            }            
            RemoveNote();
            int nClassBeginIndex = GetClassDefineBeginIndex();
            if (nClassBeginIndex == -1)
            {
                string str = string.Format("指定文件中没有类{0}的定义", tbxClass.Text);
                MessageBox.Show(str, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            string strInheritClass = GetClassDefine(nClassBeginIndex);
            lbMakeInfo.Text += tbxClass.Text;
            if (strInheritClass != "")
            {
                lbMakeInfo.Text += " + ";
                string strTip = string.Format("{0}继承于:",tbxClass.Text);
                lbTip.Text = lbTip.Text + strTip;
                tbxClass.Text = strInheritClass;
                tbxFileName.Text = "";
            }
            else
            {
                btnFinish.Enabled = false;
                btnMake.Enabled = true;
            }

        }
        /// <summary>
        /// 读取类文件
        /// </summary>
        protected bool ReadClassFile2String()
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
            }
            catch (System.Exception exp)
            {
                MessageBox.Show(exp.ToString(), "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
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
        /// 获取类的定义添加到列表m_lstCls
        /// </summary>
        /// <param name="nBeginIndex"></param>
        /// <returns>返回继承的类名</returns>
        protected string GetClassDefine(int nBeginIndex)
        {
            int nIndex = -1;
            int nDefineEndIndex = -1;
            int nDefineBeginIndex = -1;
            string strInheritClass = "";
            string strPublic = "public";
            string strClassDefine = "";

            nDefineBeginIndex = m_strClassFile.IndexOf('{', nBeginIndex);
            nDefineEndIndex = m_strClassFile.IndexOf("};", nBeginIndex);
            nIndex = m_strClassFile.IndexOf(strPublic, nBeginIndex, nDefineBeginIndex - nBeginIndex);
            if (nIndex != -1)
            {
                nIndex += strPublic.Length;
                string strTrim = " \t\r\n";
                strInheritClass = m_strClassFile.Substring(nIndex, nDefineBeginIndex - nIndex);
                strInheritClass = strInheritClass.Trim(strTrim.ToCharArray());
            }
            strClassDefine = m_strClassFile.Substring(nDefineBeginIndex + 1, nDefineEndIndex - nDefineBeginIndex - 1);
            if (!m_lstCls.Contains(strClassDefine))
            {
                m_lstCls.Add(strClassDefine);
            }            

            return strInheritClass;

        }

        /// <summary>
        /// 获取类定义文件中指定类的开始定义索引
        /// </summary>
        /// <returns>-1表示没有该类的定义</returns>
        protected int GetClassDefineBeginIndex()
        {
            int nResultIndex = -1;            
            int nSemicomonIndex = 0;
            int nBracketIndex = -1;
            string str1 = string.Format("interface {0}", tbxClass.Text);
            string str2 = string.Format("class {0}", tbxClass.Text);

            do{
                nResultIndex = m_strClassFile.IndexOf(str1, nSemicomonIndex);
                if (nResultIndex == -1)
                {
                    nResultIndex = m_strClassFile.IndexOf(str2, nSemicomonIndex);                   
                }
                if (nResultIndex != -1)
                {
                    nSemicomonIndex = m_strClassFile.IndexOf(';', nResultIndex);
                    nBracketIndex = m_strClassFile.IndexOf('{', nResultIndex);
                }
                               
            }while(nResultIndex != -1 && nSemicomonIndex < nBracketIndex);            
            return nResultIndex; 
        }

        /// <summary>
        /// 生成idl
        /// </summary>
        protected void MakingIdl()
        {
            string strOneClassDefine = "";
            for (int i = m_lstCls.Count - 1; i >= 0; i--)
            {
                strOneClassDefine = m_lstCls[i];
                AnalyseOneClass(strOneClassDefine);
                
            }
            ModifyListTransfers();
            foreach (KTransferOneFunction oneTransfer in listTransferFuncs)
            {
                m_strPart2 += "\r\n\t" + oneTransfer.GetIdlResult();
            }
        
        }

        /// <summary>
        /// 扫描转换列表，修改同名函数名
        /// </summary>
        protected void ModifyListTransfers()
        {
            string strFuncNamei = "";
            string strParasi = "";
            string strParasj = "";
            string strFuncNamej = "";
            for (int i = 0; i < listTransferFuncs.Count; i++)
            {
                strFuncNamei = listTransferFuncs[i].GetFuncName();
                strParasi = listTransferFuncs[i].GetParas();
                for (int j = i + 1; j < listTransferFuncs.Count; j++)
                {
                    strFuncNamej = listTransferFuncs[j].GetFuncName();
                    strParasj = listTransferFuncs[j].GetParas();
                    if (strFuncNamei == strFuncNamej && strParasj == strParasi)
                    {
                        listTransferFuncs.RemoveAt(j);
                        j--;
                        continue;
                    }
                    if (strFuncNamei == strFuncNamej && strParasj != strParasi)
                    {
                        int nSameNameCnt = listTransferFuncs[i].IncreaseSameNameCnt();
                        listTransferFuncs[j].ModifyFuncName(nSameNameCnt);
                    }

                }
            }
        }

        

        protected bool WriteNewIDL(string strIdlFileName)
        {
            string strPart1 = m_strWholeFile.Substring(0, m_nPart1EndIndex + 1);
            string strPart3 = m_strWholeFile.Substring(m_nPart3BeginIndex);
            try
            {
                FileStream fsIDL = new FileStream(strIdlFileName, FileMode.Create, FileAccess.ReadWrite);
                StreamWriter swIDL = new StreamWriter(fsIDL);

                swIDL.Write(strPart1);
                swIDL.Write(m_strPart2);
                swIDL.Write(strPart3);

                swIDL.Close();
                fsIDL.Close();
            }
            catch (System.Exception e)
            {
                System.Diagnostics.Trace.Write(e.ToString());
                return false;
            }
            return true;

        }

        private void btnMake_Click(object sender, EventArgs e)
        {
            string strTip = lbTip.Text + tbxClass.Text + "生成：\r\n" + lbxInterfaces.SelectedItem.ToString();

            DialogResult dlgr = MessageBox.Show(strTip, "Are You Sure?", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (dlgr == DialogResult.OK)
            {
                SetIndexs();
                MakingIdl(); 
                tbxRecords.Text += lbMakeInfo.Text + "\r\n";
                btnMake.Enabled = false;
                btnSaveIdl.Visible = true;
               // btnSeeIdl.Visible = true; 
                MessageBox.Show("生成idl成功，可以保存了", "成功", MessageBoxButtons.OK);
                saveFileDialogIDL.Filter = "idl (*.idl)|*.idl|All|*.*";
                if (saveFileDialogIDL.ShowDialog() == DialogResult.OK)
                {
                    string strIdlFilePath = saveFileDialogIDL.FileName;
                    WriteNewIDL(strIdlFilePath);
                }
            }           
        }

        private void lbxInterfaces_SelectedIndexChanged(object sender, EventArgs e)
        {                      
            ReSet();
        }

        protected void AnalyseOneClass(string strClassDefine)
        {
            MacroProcess(ref strClassDefine);
            string strResult = "";
            string strOneFunc = "";
            int nBegin = 0;
            int nEnd = -1;
            
            nBegin = strClassDefine.IndexOf("virtual", 0); 
            
            while (nBegin != -1)
            {
                KTransferOneFunction myTransfer = new KTransferOneFunction();
                nEnd = strClassDefine.IndexOf(';', nBegin);
                strOneFunc = strClassDefine.Substring(nBegin, nEnd - nBegin);
                myTransfer.ReInit(strOneFunc);
                string strFuncName = myTransfer.GetFuncName();
                string strOldParas = GetOldParas(strFuncName);
                myTransfer.SetOldParas(strOldParas);
                listTransferFuncs.Add(myTransfer);                
                nBegin = strClassDefine.IndexOf("virtual", nEnd);
            }
        }
        /// <summary>
        /// 获取函数的旧参数
        /// </summary>
        /// <param name="strFuncName"></param>
        /// <returns></returns>
        protected string GetOldParas(string strFuncName)
        {
            string strSearch = strFuncName;
            string strResult = "";

            int nBegin = m_strOldIdl.IndexOf(strSearch);

            
            while (nBegin != -1)
            {
                int nBehind = nBegin + strSearch.Length;
                if (IsOneWordEnd(m_strOldIdl[nBehind]))
                {
                    nBegin += strSearch.Length + 1;
                    int nEnd = m_strOldIdl.IndexOf(')', nBegin);
                    System.Diagnostics.Debug.Assert(nEnd != -1);
                    strResult = m_strOldIdl.Substring(nBegin, nEnd - nBegin);
                    break;
                }
                nBegin = m_strOldIdl.IndexOf(strSearch, nBegin + 1);
            }
            return strResult;
        }
        /// <summary>
        /// 添加一条idl结果
        /// </summary>
        /// <param name="strOneResult"></param>
        protected void Part2Add(string strOneResult)
        {
            if (!m_strPart2.Contains(strOneResult))
            {
                m_strPart2 += "\r\n\t" + strOneResult;
            }
        }

        private void btnChooseIDL_Click(object sender, EventArgs e)
        {
            m_dlg.Filter = "Idl 文件 (*.idl)|*.idl";            
            DialogResult dr = DialogResult.Cancel;
            try
            {
                dr = m_dlg.ShowDialog(this);
            }
            catch (SystemException exp)
            {
                System.Diagnostics.Trace.Write(exp.ToString());
            }
            if (dr == DialogResult.OK)
            {
                m_strIdlPath = m_dlg.FileName;
                InitLbxInterfaces();
            }
            
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            ReSet();         
        }
        protected void ReSet()
        {
            string strClass = "";
            string strLabelMakeInfo = "";
            if (lbxInterfaces.SelectedIndex != -1)
            {
                strLabelMakeInfo = lbxInterfaces.SelectedItem.ToString();
                if (strLabelMakeInfo.Length > 4)
                {
                    strClass = strLabelMakeInfo.Substring(4);
                }                
            }
            lbMakeInfo.Text = strLabelMakeInfo + " = ";
            tbxClass.Text = strClass;
            lbTip.Text = "";
            m_strClassFile = "";
            btnMake.Enabled = false;
            btnFinish.Enabled = true;
            m_lstCls.Clear();
            listTransferFuncs.Clear();
            tbxFileName.Text = "";
            m_strPart2 = "";

            btnSaveIdl.Visible = false;
        }

        private void btnSaveIdl_Click(object sender, EventArgs e)
        {
            saveFileDialogIDL.Filter = "idl (*.idl)|*.idl|All|*.*";
            if (saveFileDialogIDL.ShowDialog() == DialogResult.OK)
            {
                string strIdlFilePath = saveFileDialogIDL.FileName;
                WriteNewIDL(strIdlFilePath);
            }
        }

        private void btnSeeIdl_Click(object sender, EventArgs e)
        {

        }
        protected void MacroProcess(ref string strToBeProcessed)
        {
            string[][] strMacrosDefine = 
                {
                    new string[]{
                        "STDMETHOD(method)", "virtual HRESULT STDMETHODCALLTYPE method" 
                    },
                    new string[]{ 
                        "STDMETHOD_(type,method)", "virtual type STDMETHODCALLTYPE method" 
                    }
                };
            foreach (string[] OneMacroDefine in strMacrosDefine)
            {
                ProcessOneMacro(ref strToBeProcessed, OneMacroDefine);
            }
            
        }

        protected void ProcessOneMacro(ref string strToBeProcessed, string[] OneMacroDefine)
        {
            bool bHasPara = false;
            int nIndex = -1;
            string strMacroName;

            nIndex = OneMacroDefine[0].IndexOf('(');
            if (nIndex != -1)
            {
                bHasPara = true;
                strMacroName = OneMacroDefine[0].Substring(0, nIndex);
            }
            else
            {
                strMacroName = OneMacroDefine[0];
            }
            int nBegin = strToBeProcessed.IndexOf(strMacroName, 0);
            int nEnd = -1;
            while (nBegin != -1)
            {
                nEnd = nBegin + strMacroName.Length;
                System.Diagnostics.Debug.Assert(strToBeProcessed.Length > nEnd);
                if (IsOneWordEnd(strToBeProcessed[nEnd]))
                {
                    string strOneMacroInstance;
                    string strReplaceInstance;
                    if (bHasPara)
                    {
                        nEnd = strToBeProcessed.IndexOf(')', nBegin);
                        System.Diagnostics.Debug.Assert(nEnd != -1);
                        strOneMacroInstance = strToBeProcessed.Substring(nBegin, nEnd - nBegin +1);
                        
                    }
                    else
                    {
                        strOneMacroInstance = strMacroName;
                    }
                    strReplaceInstance = GetReplaceMacro(OneMacroDefine, strOneMacroInstance);
                    strToBeProcessed = strToBeProcessed.Replace(strOneMacroInstance, strReplaceInstance);                    
                }
                nBegin = strToBeProcessed.IndexOf(strMacroName, nEnd);

            }           

        }
        protected bool IsOneWordEnd(char chAfterWord)
        {
            switch (chAfterWord)
            {
                case ' ':
                case '\t':
                case '(':
                case '\r':
                case '\n':
                    return true;
                default:
                    return false;
            }
        }
        protected string GetReplaceMacro(string[] OneMacroDefine, string strMacroInstance)
        {
            string[] strParas;
            string[] strInstanceParas;
            string strReplaceResult = OneMacroDefine[1];
            
            int nBegin = 0;
            int nEnd = -1;

            nBegin = OneMacroDefine[0].IndexOf('(');
            nEnd = OneMacroDefine[0].IndexOf(')');
            if (nBegin != -1)
            {
                System.Diagnostics.Debug.Assert(nEnd != -1);
                string strParamall = OneMacroDefine[0].Substring(nBegin + 1, nEnd - nBegin - 1);
                char[] strSeparator = {','};
                char[] strTrim = {' ', '\t'};
                strParamall = strParamall.Trim(strTrim);
                strParas = strParamall.Split(strSeparator);
                nBegin = strMacroInstance.IndexOf('(');
                nEnd = strMacroInstance.IndexOf(')');
                System.Diagnostics.Debug.Assert(nEnd != -1);
                System.Diagnostics.Debug.Assert(nBegin != -1);
                strParamall = strMacroInstance.Substring(nBegin + 1, nEnd - nBegin - 1);
                strParamall = strParamall.Trim(strTrim);
                strInstanceParas = strParamall.Split(strSeparator);
                for (int i = 0; i < strParas.Length; i++)
                {
                    strReplaceResult = strReplaceResult.Replace(strParas[i], strInstanceParas[i]);
                }                
            }
            return strReplaceResult;
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
       
    }
}