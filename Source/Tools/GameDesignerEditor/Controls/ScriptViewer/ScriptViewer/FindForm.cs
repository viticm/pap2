 using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;

namespace ScriptViewer
{      
    public delegate void doubleclickEventsHandle(object sender, FindResultPosition e); //给OnNewWindow用的
    
    public partial class ScriptFindForm : Form
    {
        const int NodeMaxCapcity = 511;
        public Hashtable htNode2Codes;//key:treenode value:code

        private SqlDb sqlDB;
        private SqlConnection Conn;

        private ArrayList m_AllScriptTreeNode;

        public event doubleclickEventsHandle ListBoxDoubleClick  = null;

      
        public ScriptFindForm()
        {
            InitializeComponent();
        }
       
        public void Init(SqlConnection conn, ArrayList allTreeNode)
        {
            m_AllScriptTreeNode = allTreeNode;
            Conn = conn;
            sqlDB = new SqlDb();
            sqlDB.SetConnection(Conn);           
        }
        private void btnFindAll_Click(object sender, EventArgs e)
        {
            lboxFindResult.Items.Clear();
            if (htNode2Codes == null)
            {
                InithtNode2Codes();
            }

            string findWhat = tbxFindWhat.Text;

            if (htNode2Codes ==  null || findWhat.Trim().Length == 0)
            {
                return;
            }

            Find(findWhat);

            lboxFindResult.Items.Add(string.Format("共找到{0}个结果", lboxFindResult.Items.Count));
        }
        private void Find(string findWhat)
        {
            string code;
            TreeNode node;

            foreach (DictionaryEntry de in htNode2Codes)
            {
                node = de.Key as TreeNode;
                code = de.Value.ToString();
                FindInCode(code, findWhat, node);
            }
        }
        private void FindInCode(string Code, string strFind, TreeNode node)
        {
            if (node == null)
            {
                return;
            }

            int LastLineEndIndex = -1;
            int CurLineEndIndex = -1;

            string OneLine = string.Empty;
            int curLineNumber = 1;      //当前行号
            int ColIndex = -1;          //在当前行的列号
            int IndexInAll = -1;        //在全文的索引号

            FindResultPosition position;

            while (FetchNextLine(Code, ref LastLineEndIndex, ref CurLineEndIndex, ref OneLine))
            {
                ColIndex = OneLine.IndexOf(strFind, this.ckbCaseSensitive.Checked ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase);
                if (ColIndex != -1)
                {
                    IndexInAll = LastLineEndIndex + ColIndex;
                    position = new FindResultPosition(node);
                    position.Index = IndexInAll;
                    position.LineNumber = curLineNumber;
                    position.LinePreview = OneLine;
                    position.ColNumber = ColIndex;
                    //add by wangying1  2009-7-23
                    position.Text = strFind;
                    //end of add 
                    lboxFindResult.Items.Insert(lboxFindResult.Items.Count, position);
                }
                ++curLineNumber;
            }
        }

        private bool FetchNextLine(string str, ref int beginIndex, ref int endIndex, ref string line)
        {
            beginIndex = endIndex + 1;

            if (beginIndex >= str.Length - 1 || beginIndex < 0)
            {
                return false;
            }

            endIndex = str.IndexOf('\n', beginIndex);
            if (endIndex == -1)
            {
                endIndex = str.Length - 1;
            }

            line = str.Substring(beginIndex, endIndex - beginIndex);

            return true; 
        }

        private void lboxFindResult_DoubleClick(object sender, EventArgs e)
        {
            FindResultPosition position = lboxFindResult.SelectedItem as FindResultPosition;
            if (position == null)
            {
                return;
            }
            ListBoxDoubleClick(sender, position);
        }

        private void InithtNode2Codes()
        {
            htNode2Codes = new Hashtable(NodeMaxCapcity);

            foreach (TreeNode node in this.m_AllScriptTreeNode)
            {
                AddNode2Hashtable(node);
            }

        }
        private bool GetNodeCode(TreeNode tn, ref string strCodeRet)
        {
            bool bResult = false;

            tagPlus tp = tn.Tag as tagPlus;

            if (tp.nDeep >= 2)
            {
                //模块级代码
                string sql = "select * from sys_scriptstore where fieldid=" + tp.fieldid + " and modtabid=" + tp.tabid;
                DataTable dt = sqlDB.ExeSqlDs(sql, "script").Tables[0];

                if (dt.Rows.Count == 1)
                {
                    Byte[] ab = dt.Rows[0]["scriptcode"] as Byte[];
                    if (ab != null)
                        strCodeRet = Encoding.UTF8.GetString(ab);
                    else
                        strCodeRet = string.Empty;
                    bResult = true;
                }
                else if (dt.Rows.Count > 1)
                {
                    MessageBox.Show("脚本不唯一！请检查数据库");
                }
            }

            return bResult;
        }
        private void AddNode2Hashtable(TreeNode tn)
        {
            string Code = string.Empty;
            if (GetNodeCode(tn, ref Code))
            {
                htNode2Codes[tn] = Code;
            }
        }

        private void ScriptFindForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Visible = false;
            e.Cancel = true;
        }
    }
}