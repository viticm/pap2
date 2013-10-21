using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace ScriptViewer
{
    public partial class scriptTree : UserControl
    {
        private SqlConnection m_conn = null;
        private SqlDb sqlDB = new SqlDb();

        private ArrayList m_AllScriptTreeNode = new ArrayList();

        public delegate void treeEventHandler(object sender, treeEventArgs e);
        public event treeEventHandler SelectedScriptNode;


        public scriptTree()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 初始化
        /// </summary>
        /// <param name="conn"></param>
        public void init(SqlConnection conn)
        {
            this.m_conn = conn;
            
            sqlDB.SetConnection(conn);
            TreeNode tn = tree.Nodes.Add("表");
            tn.Tag = new tagPlus(1, "表");
            loadData();
        }

        /// <summary>
        /// 根据条件，自动选中树上的某个结点
        /// </summary>
        /// <param name="fieldid"></param>
        /// <param name="tabid"></param>
        public void selectedByIds(string fieldid, string tabid, object tag)
        {
            foreach( TreeNode tn in this.m_AllScriptTreeNode)
            {
                tagPlus tp = tn.Tag as tagPlus;
                if(tp.fieldid == fieldid && tp.tabid == tabid)
                {
                    //让自己被展开
                    expendParent(tn);
                    raiseEvent(tn, tag);
                    tree.SelectedNode = tn;
                    break;
                }
            }
        }

        public void selectedByIdsNoEvent(string fieldid, string tabid)
        {
            foreach( TreeNode tn in this.m_AllScriptTreeNode)
            {
                tagPlus tp = tn.Tag as tagPlus;
                if(tp.fieldid == fieldid && tp.tabid == tabid)
                {
                    //让自己被展开
                    expendParent(tn);
                    tree.SelectedNode = tn;
                    break;
                }
            }
        }

        //让某个结点被展开
        private void expendParent(TreeNode tn)
        {
            if(tn.Parent != null)
            {
                tn.Parent.Expand();
                expendParent(tn.Parent);
            }
        }

        /// <summary>
        /// 加载数据到树
        /// </summary>
        private void loadData()
        {
            //模块层
            string sql = @"select distinct modelname_cn from sys_modl_def";
            DataSet ds = sqlDB.ExeSqlDs(sql, "sys_modl_def");
            foreach (DataRow row in ds.Tables[0].Rows)
            {
                TreeNodeCollection t = tree.Nodes[0].Nodes;
                string text = row["modelname_cn"].ToString();
                TreeNode tn = t.Add(text, text);
                sql = string.Format(@"SELECT modtabid
                    FROM (SELECT DISTINCT 
                    sys_modl_def.modelname_cn, sys_modl_tab_def.name, 
                    sys_modl_tab_def.modtabid
                    FROM sys_modl_def INNER JOIN
                    sys_modl_tab_def ON sys_modl_tab_def.modid = sys_modl_def.id) 
                    DERIVEDTBL
                    WHERE (modelname_cn = '{0}')", text);
                ds = sqlDB.ExeSqlDs(sql, "BiteMe");

                string first_modtabid = string.Empty;
                if (ds.Tables[0].Rows.Count > 0)
                {
                    first_modtabid = ds.Tables[0].Rows[0][0].ToString();
                }
                tn.Tag = new tagPlus(2, text, "-1", first_modtabid);
            }

            //Tab页层
            sql = @"select distinct modelname_cn,name,modtabid 
                    from sys_modl_def 
                    inner join sys_modl_tab_def 
                    on sys_modl_tab_def.modid=sys_modl_def.id     
                    ";
            ds = sqlDB.ExeSqlDs(sql, "scriptview");
            foreach (DataRow row in ds.Tables[0].Rows)
            {
                string text = row["modelname_cn"].ToString();
                TreeNodeCollection t = tree.Nodes[0].Nodes;
                t = t[text].Nodes;
                string name = row["name"].ToString();
                TreeNode tn  = t.Add(text + "->" + name, name);
                tn.Tag = new tagPlus(3, tn.FullPath.ToString(), "0", row["modtabid"].ToString());
                this.m_AllScriptTreeNode.Add(tn);
                sql = @"exec queryFieldInfo " + row["modtabid"].ToString();
                DataTable tabtable = sqlDB.ExeSqlDs(sql, "scriptview").Tables[0];
                fillNode(tabtable, tn, 0);
            }

        }

        /// <summary>
        /// 填充结点以及子内容
        /// </summary>
        /// <param name="dt">缓存表</param>
        /// <param name="tn">结点</param>
        /// <param name="p_fieldid">parentfield列的过滤条件</param>
        private void fillNode(DataTable dt, TreeNode tn, int p_fieldid)
        {
            Font ft = new Font("宋体", 9, FontStyle.Bold);
            tagPlus tp = tn.Tag as tagPlus;
            string tiaojian = "parentfield=" + p_fieldid.ToString();
            if (p_fieldid == 0)
                tiaojian += " or parentfield is NULL";
            foreach (DataRow row in dt.Select(tiaojian))
            {
                string text = row["modelname_cn"].ToString().Trim();
                string name = row["name"].ToString().Trim();
                string fieldname = row["fieldname"].ToString().Trim();
                string fieldcnname = row["fieldcnname"].ToString().Trim();
                int fieldid = int.Parse(row["fieldid"].ToString().Trim());
                int parentfield = int.Parse(row["parentfield"].ToString().Trim() == "" ? "0" : row["parentfield"].ToString().Trim());
                int hasChild = int.Parse(row["hasChild"].ToString().Trim());
                int hasScript = int.Parse(row["hasScript"].ToString().Trim());
                TreeNodeCollection t = tn.Nodes;
                TreeNode tn1 = t.Add(fieldcnname + "(" + fieldname + ")");
                this.m_AllScriptTreeNode.Add(tn1);
                tn1.Tag = new tagPlus(tp.nDeep + 1, tn1.FullPath.ToString(), fieldid.ToString(), tp.tabid);
                if (hasScript > 0)
                {
                    tn1.NodeFont = ft;
                }
                if (hasChild > 0)
                {
                    fillNode(dt, tn1, fieldid);
                }
            }
        }

        private void scriptTree_DoubleClick(object sender, EventArgs e)
        {

        }

        private void tree_DoubleClick(object sender, EventArgs e)
        {
            //return;
            
        }


        public void BoldNode(string fieldid, string tabid, string strcode)
        {
            //让结点变黑
            foreach(TreeNode tn in this.m_AllScriptTreeNode)
            {
                tagPlus tp = tn.Tag as tagPlus;
                if(fieldid == tp.fieldid && tabid == tp.tabid)
                {
                    tn.Text = tn.Text + " ";
                    tn.NodeFont = new Font("宋体", 9, FontStyle.Bold);
                    tn.Text = tn.Text.Substring(0, tn.Text.Length - 1);
                }
            }
        }

        private void tree_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void set_style_Selected(TreeNode tn)
        {
            tn.BackColor = Color.Blue;
            tn.ForeColor = Color.White;
        }
        private void set_style_UnSelected(TreeNode tn)
        {
            tn.BackColor = Color.White;
            tn.ForeColor = Color.Black;
        }

        public void tree_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            if (tree.SelectedNode != null)
                set_style_UnSelected(tree.SelectedNode);
            set_style_Selected(e.Node);
            raiseEvent(e.Node, null);
        }

        private void raiseEvent(TreeNode tn, object tag)
        {
            treeEventArgs tea = new treeEventArgs();
            tagPlus tp = tn.Tag as tagPlus;
            string strCode = "";
            if (tp.nDeep >= 2)
            {
                //模块级代码
                string sql = "select * from sys_scriptstore where fieldid=" + tp.fieldid + " and modtabid=" + tp.tabid;
                DataTable dt = sqlDB.ExeSqlDs(sql, "script").Tables[0];
                if (dt.Rows.Count > 1)
                {
                    MessageBox.Show("脚本不唯一！请检查数据库");
                    return;
                }
                if (dt.Rows.Count == 0)
                {
                    strCode = "";
                }
                else
                {
                    Byte[] ab = dt.Rows[0]["scriptcode"] as Byte[];
                    if (ab != null)
                        strCode = Encoding.UTF8.GetString(ab);
                }
            }
            else
            {
                return;
            }
            tea.scriptCode = strCode;
            tea.scriptPath = tp.stag;
            tea.fieldid = tp.fieldid;
            tea.tabid = tp.tabid;
            tea.tag = tag;
            if (SelectedScriptNode != null)
                SelectedScriptNode(this, tea);
        }

        public ArrayList AllScriptTreeNode
        {
            get { return m_AllScriptTreeNode; }
        }
    }



    public class treeEventArgs : EventArgs
    {
        public string scriptPath = "";
        public string scriptCode = "";
        public string fieldid = "";
        public string tabid = "";
        public object tag = null;
    };

    /// <summary>
    /// 树结点的附加数据
    /// </summary>
    public class tagPlus
    {
        public tagPlus(int nDeep, string stag)
        {
            this.nDeep = nDeep;
            this.stag = stag;
        }
        public tagPlus(int nDeep, string stag, string fieldid, string tabid)
        {
            this.nDeep = nDeep;
            this.stag = stag;
            this.fieldid = fieldid;
            this.tabid = tabid;
        }
        public int nDeep = 0;
        public string fieldid = "";
        public string tabid = "";
        public string stag = "";     //scriptpath
    };
}
