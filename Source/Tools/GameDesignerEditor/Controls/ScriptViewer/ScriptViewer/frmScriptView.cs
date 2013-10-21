using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;


namespace ScriptViewer
{
    public delegate void svEventsHandle(object sender, svEventsArgs e); //给OnNewWindow用的
    public delegate void OnWindowSaveEventsHandle(object sender, OnSaveEventArgs e); //某个窗口保存用的

    public partial class frmScriptView : Form
    {
        public event svEventsHandle OnNewWindow = null;
        public event OnWindowSaveEventsHandle OnWindowSave = null;
        
        private SqlConnection m_conn = null;
        ScriptFindForm theFindForm;

        private Font mScriptFont;

        public SqlConnection Conn
        {
            get { return m_conn; }
        }

        private FindResultPosition curPosition = null;

        public frmScriptView(SqlConnection conn)
        {
            InitFont();
            InitializeComponent();
            
            this.tabStrip1.MdiForm = this;
            this.scriptTree1.init(conn);
            this.m_conn = conn;
            ScriptLock slLock = new ScriptLock(conn, 0, 0, "", "");
            slLock.DeleteAllLocalLock();
        }

        private void InitFont()
        {
            Font font = new Font("宋体", 12f);
            if (File.Exists("./scriptfont.xml"))
            {
                font = DeserializeFont();
            }
            mScriptFont = font;
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is frmScriptEdit)
                {
                    (frm as frmScriptEdit).luaEditorControl1.ScriptFont = mScriptFont;
                }
            }
        }

        private void ResetScriptFont()
        {
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is frmScriptEdit)
                {
                    (frm as frmScriptEdit).luaEditorControl1.ScriptFont = mScriptFont;
                }
            }
        }

        public void SerializeFont(Font font)
        {
            Stream stream = File.Open(".//scriptfont.xml", FileMode.Create);
            BinaryFormatter formatter = new BinaryFormatter();
            formatter.Serialize(stream, font);
            stream.Close();
        }

        public Font DeserializeFont()
        {
            Font font = null;
            Stream stream = File.Open(".//scriptfont.xml", FileMode.Open);
            BinaryFormatter formatter = new BinaryFormatter();
            font = formatter.Deserialize(stream) as Font;
            stream.Close();
            return font;
        }

        public void OpenWindow(int fieldid, int modtabid, object tag)
        {
            this.scriptTree1.selectedByIds(fieldid.ToString().Trim(), modtabid.ToString().Trim(), tag);
            foreach (Form frm in this.MdiChildren)
            {
                if (frm is frmScriptEdit)
                {
                    (frm as frmScriptEdit).luaEditorControl1.ScriptFont = mScriptFont;
                }
            }
            ResetScriptFont();
        }

        public void OpenFileWindow(string filename, object tag)
        {
            frmScriptEdit fseshow = null;
            foreach (frmScriptEdit fse in this.MdiChildren)
            {
                if(filename == fse.Tag.ToString())
                {
                    fseshow = fse;
                    break;
                }
            }

            if (fseshow == null)
            {
                fseshow = new frmScriptEdit();
                fseshow.luaEditorControl1.ForWho = "GameLuaEditor";

                fseshow.Tag = filename;
                fseshow.filename = filename;
                fseshow.Text = filename;
                fseshow.fieldid = "";
                fseshow.tabid = "";
                fseshow.strCode = FileToString(filename);
                fseshow.MdiParent = this;
                if (this.OnNewWindow != null)
                {
                    //this.OnNewWindow(this, new svEventsArgs(fseshow.luaEditorControl1));
                }
                fseshow.WindowState = FormWindowState.Maximized;

                
                fseshow.Show();
            }

            foreach (DevComponents.DotNetBar.TabItem ti in this.tabStrip1.Tabs)
            {
                if (ti.AttachedControl == fseshow)
                {
                    tabStrip1.SelectedTab = ti;
                }
            }

            ResetScriptFont();
        }

        public void fseshow_OnSave(object sender, OnSaveEventArgs e)
        {
            if(this.OnWindowSave != null)
            {
                this.OnWindowSave(this, new OnSaveEventArgs(e.fieldid, e.tabid, e.strcode, e.tag));
            }
        }

        private void scriptTree1_SelectedScriptNode(object sender, treeEventArgs e)
        {
            //MessageBox.Show(e.scriptPath);
            frmScriptEdit fseshow = null;
            foreach (frmScriptEdit fse in this.MdiChildren)
            {
                if (e.scriptPath == fse.Tag.ToString())
                {
                    fseshow = fse;
                    break;
                }
            }
            if (fseshow == null)
            {
                fseshow = new frmScriptEdit();
                fseshow.Tag = e.scriptPath;
                fseshow.Text = e.scriptPath;
                fseshow.fieldid = e.fieldid;
                fseshow.tabid = e.tabid;
                fseshow.strCode = e.scriptCode;
                fseshow.tag = e.tag;
                fseshow.MdiParent = this;
                fseshow.OnSave += new OnSaveEventsHandle(fseshow_OnSave);
                if (this.OnNewWindow != null)
                {
                    this.OnNewWindow(this, new svEventsArgs(fseshow.luaEditorControl1));
                }
                fseshow.WindowState = FormWindowState.Maximized;
                fseshow.Show();

            }

            //激活窗口到前台
            foreach (DevComponents.DotNetBar.TabItem ti in this.tabStrip1.Tabs)
            {
                if (ti.AttachedControl == fseshow)
                {
                    tabStrip1.SelectedTab = ti;
                }
            }

            fseshow.GoToPosition(curPosition);
            curPosition = null;

            if (mScriptFont != null)
            {
                fseshow.luaEditorControl1.ScriptFont = mScriptFont;
            }
        }

        private void dotNetBarManager1_ItemClick(object sender, EventArgs e)
        {

        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            if( this.ActiveMdiChild != null )
            {
                frmScriptEdit fse = this.ActiveMdiChild as frmScriptEdit;
                fse.Save();
            }
        }

        private void btnSaveAll_Click(object sender, EventArgs e)
        {
            foreach (frmScriptEdit fse in this.MdiChildren)
            {
                fse.Save();
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            if( this.ActiveMdiChild != null )
            {
                frmScriptEdit fse = this.ActiveMdiChild as frmScriptEdit;
                fse.Close();
            }
        }

        private void btnCloseAll_Click(object sender, EventArgs e)
        {
            foreach (frmScriptEdit fse in this.MdiChildren)
            {
                fse.Close();
            }
        }

        private void frmScriptView_FormClosing(object sender, FormClosingEventArgs e)
        {
            foreach (frmScriptEdit fse in this.MdiChildren)
            {
                fse.Close();
            }
            this.Visible = false;
            e.Cancel = true;
        }

        private void tabStrip1_SelectedTabChanged(object sender, DevComponents.DotNetBar.TabStripTabChangedEventArgs e)
        {
            if (this.ActiveMdiChild == null) return;
            frmScriptEdit fse = this.ActiveMdiChild as frmScriptEdit;
            scriptTree1.selectedByIdsNoEvent(fse.fieldid, fse.tabid);
        }

        // 读取文件到string
        static public string FileToString(string strFileName)
        {
            return FileToString(strFileName, Encoding.GetEncoding("gb2312"));
        }
        static public string FileToString(string strFileName, Encoding encoding)
        {
            StreamReader reader;
            try
            {
                reader = new StreamReader(strFileName, encoding);
            }
            catch //(Exception e)
            {
                //MessageBox.Show(e.Message);
                return "";
            }

            //StreamReader reader = new StreamReader(strFileName, encoding);
            string strContent = reader.ReadToEnd();
            reader.Close();
            return strContent;
        }

        private void btnHistory_Click(object sender, EventArgs e)
        {
            if (this.ActiveMdiChild == null)
                return;
            frmHistory frmh = new frmHistory();
            frmh.listBox1.Items.Clear();
            frmScriptEdit fse = this.ActiveMdiChild as frmScriptEdit;
            if (fse.filename != "")
            {
                MessageBox.Show("没有历史纪录");
                return;
            }
            fillListBox(frmh.listBox1, fse.fieldid, fse.tabid, fse.strCode);
            DialogResult ret = frmh.ShowDialog();
            if(ret == DialogResult.OK)
            {
                fse.setCode(frmh.strRetCode);
            }
        }

        private void fillListBox(ListBox lb, string fieldid, string tabid, string curCode)
        {
            if (this.m_conn == null) return;
            SqlDb sdb = new SqlDb();
            sdb.SetConnection(this.m_conn);
            string sql = "";
            if(fieldid == "0")
            {
                sql = string.Format("select logid,scriptcode,logtime,reason,author from sys_scriptstore_log where (fieldid is NULL or fieldid=0) and modtabid={0} order by logtime", tabid);
            }
            else
            {
                sql = string.Format("select logid,scriptcode,logtime,reason,author from sys_scriptstore_log where fieldid={0} and modtabid={1} order by logtime", fieldid, tabid);
            }
            DataSet ds = sdb.ExeSqlDs(sql, "script");
            foreach (DataRow r in ds.Tables[0].Rows)
            {
                int id = int.Parse(r["logid"].ToString().Trim());
                string logtime = r["logtime"].ToString().Trim();
                string reason = r["reason"].ToString().Trim();
                string author = r["author"].ToString().Trim();
                Byte[] ab = r["scriptcode"] as Byte[];
                string strCode = ab == null? "" : Encoding.UTF8.GetString(ab);
                hisItem hi = new hisItem(id, logtime, strCode, reason, author);
                lb.Items.Add(hi);
            }
            hisItem hi1 = new hisItem(curCode);
            lb.Items.Add(hi1);
        }

        private void btnFind_Click(object sender, EventArgs e)
        {           
            if (theFindForm == null)
            {
                theFindForm = new ScriptFindForm();
                theFindForm.Init(m_conn, this.scriptTree1.AllScriptTreeNode);
                theFindForm.ListBoxDoubleClick += new doubleclickEventsHandle(OnListBoxDoubleClick);
            }           
            theFindForm.Show();

        } 
        private void OnListBoxDoubleClick(object sender, FindResultPosition position)
        {
            TreeNode node = position.TheTreeNode;
            if (node == null)
            {
                return;
            }
            curPosition = position;


            TreeViewCancelEventArgs tvcea = new TreeViewCancelEventArgs(node, false, TreeViewAction.Unknown);            
            this.scriptTree1.tree_BeforeSelect(sender, tvcea);            
        }

        private void buttonItem1_Click(object sender, EventArgs e)
        {

        }

        private void fontButton_Click(object sender, EventArgs e)
        {
            FontDialog fontDlg = new FontDialog();
            fontDlg.Font = mScriptFont;
            if (fontDlg.ShowDialog(this) == DialogResult.OK)
            {
                
                mScriptFont = fontDlg.Font;
                foreach (Form frm in this.MdiChildren)
                {
                    if (frm is frmScriptEdit)
                    {
                        (frm as frmScriptEdit).luaEditorControl1.ScriptFont = mScriptFont;
                    }
                }

                SerializeFont(mScriptFont);
            }
           
        }
    }
    public class svEventsArgs : EventArgs
    {
        public svEventsArgs(luaEditor.luaEditorControl luace)
        {
            this.luace = luace;
        }
        public luaEditor.luaEditorControl luace = null;
    }

    public class FindResultPosition
    {
        private int lineNumber;
        private int colNumber;
        private int IndexinCode;
        private string PreviewStr;
        //add by wangying1 2009-7-23
        private string TextToFind; 
        //end of add
        private TreeNode theNode;

        public FindResultPosition(TreeNode node)
        {
            theNode = node;
        }
        //add by wangying1 2009-7-23
        public string Text
        {
            get { return TextToFind; }
            set { TextToFind = value; }
        }
        //end of add
        public int LineNumber
        {
            get { return lineNumber; }
            set { lineNumber = value; }
        }
        public int Index
        {
            get { return IndexinCode; }
            set { IndexinCode = value; }
        }
        public string LinePreview
        {
            get { return PreviewStr; }
            set { PreviewStr = value; }
        }
        public int ColNumber
        {
            get { return colNumber; }
            set { colNumber = value; }
        }
        public TreeNode TheTreeNode
        {
            get { return theNode; }
        }
      
        public override string ToString()
        {
            if (theNode != null)
            {
                return string.Format("{0},Ln:{1} Col:{2} {3}", theNode.FullPath, lineNumber, colNumber, PreviewStr);
            }
            else
            {
                return base.ToString();
            }
        }

        
    };
}