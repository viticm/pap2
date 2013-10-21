using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Data.SqlClient;
using atlaxLib;
using DevComponents.DotNetBar;

namespace sprSelector
{
    public partial class frmSelect : Form
    {
        private sprViewer.doit sprViewObject = null;
        private string m_strDir = "";
        private frmLoading fl = new frmLoading();
        private List<TreeNode> tn_list = new List<TreeNode>();
        private Queue<tree_tag> tag_que = new Queue<tree_tag>();
        public  int nQueMax = 10;
        private Test3DEngine m_engine;
        public SqlConnection Conn = null;

        //4 ff
        public bool showFolder = false;
        public string strFilter = ";.spr;"; // ".mdl;.mtl;.ani;.mesh";
        public string strFolderPreviewFilter = "*.mdl";

        public string strSelectedFileName = "";

        public bool thisMdlDescExistDB = false;
        private Font boldFont = new Font("SimSun", 9F, FontStyle.Bold);
        private Font normalFont = new Font("SimSun", 9F, FontStyle.Regular);

        public ComboBox ComboBox1 { get { return comboBox1; } set { comboBox1 = value; } }
        public Label Label3 { get { return label3; } set { label3 = value; } }
        public ButtonX ButtonX5 { get { return buttonX5; } set { buttonX5 = value; } }

        // 搜索相关
        private ArrayList OldLVC = new ArrayList();

        private bool multi_load = true;
        System.Threading.Thread m_thread = null;
        bool m_threadExit = false;

        public frmSelect()
        {
            Init();
        }
        private void Init()
        {
            InitializeComponent();
            m_threadExit = false;
            //OldLVC = new ListView.ListViewItemCollection(lv1);
            sprViewObject = new sprViewer.doit();
            Control ct = sprViewObject.Init();
            this.splitContainer4.Panel2.Controls.Add(ct);

            sprViewer.Helper.GetHelper().CurrentSqlConnection = Conn;
            sprViewer.Helper.GetHelper().InitCacheTable();
        }
        public frmSelect(Test3DEngine engine, SqlConnection conn)
        {
            m_engine = engine;
            Conn = conn;
            Init();
        }

        private void splitContainer3_SplitterMoved(object sender, SplitterEventArgs e)
        {

        }

        private void frmSelect_Load(object sender, EventArgs e)
        {
            //lv1.Select();
        }

        private void cmdOK_Click(object sender, EventArgs e)
        {
            if (lv1.SelectedItems.Count != 1)
            {
                MessageBox.Show("请选择文件");
                return;
            }

            strSelectedFileName = ((tree_tag)tv1.SelectedNode.Tag).strPath + "\\" + lv1.SelectedItems[0].Text;
            
            try
            {
                // 清除oldlvc
                textBox1.Text = string.Empty;

                foreach (ListViewItem item in OldLVC)
                {
                    lv1.Items.Add(item);
                }

                OldLVC.Clear(); 
            }
            catch (System.Exception )
            {
            }
            finally
            {
                StopThread();
            }                  
            
            this.DialogResult = DialogResult.OK;
            this.Close();            
        }

        public void LoadFolder(string strPath)
        {
            if (File.Exists(strSelectedFileName))
            {
                FileInfo fi = new FileInfo(strSelectedFileName);
                this.m_strDir = fi.DirectoryName;
            }

            if(tv1.Nodes.Count == 0)
            {
                tv1.Nodes.Add("\\");
                tv1.Nodes[0].Tag = new tree_tag();
                ((tree_tag)tv1.Nodes[0].Tag).strPath = strPath;
                LoadTree(tv1.Nodes[0].Nodes, strPath);
            }

            foreach (TreeNode t in tn_list)
            {
                if(((tree_tag)t.Tag).strPath == m_strDir)
                {
                    if(tv1.SelectedNode == t)
                    {
                        tv1_AfterSelect(this, new TreeViewEventArgs(tv1.SelectedNode));
                    }
                    else
                    {
                        tv1.SelectedNode = t;
                    }

                }
            }

        }

        private bool AlreadyContainsNode(string name, TreeNodeCollection tnc)
        {
            bool bContain = false;
            foreach (TreeNode n in tnc)
            {
                if (n.Text == name)
                {
                    bContain = true;
                    break;
                }
            }
            return bContain;
        }

        private void AddSubNode(TreeNodeCollection tnc, string strPath)
        {
            if(!Directory.Exists(strPath))
            {
                return;
            }
            DirectoryInfo di = new DirectoryInfo(strPath);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                if (d.Name == ".svn" || d.Name == "Texture") continue;
                TreeNode tn = new TreeNode(d.Name);

                tn.Tag = new tree_tag();
                ((tree_tag)tn.Tag).strPath = d.FullName;
                //LoadTree(tn.Nodes, d.FullName);

                if (!AlreadyContainsNode(d.Name, tnc))
                {
                    tnc.Add(tn);
                    tn_list.Add(tn);
                }
            }
        }

        private void LoadTree(TreeNodeCollection tnc, string strPath)
        {
            AddSubNode(tnc, strPath);

            foreach (TreeNode node in tnc)
            {
                AddSubNode(node.Nodes, (node.Tag as tree_tag).strPath);
            }
        }
        private void tv1_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            TreeNode node = e.Node;
            if (node == null || node.Tag as tree_tag == null)
                return;

            foreach (TreeNode n in node.Nodes)
                AddSubNode(n.Nodes, (n.Tag as tree_tag).strPath);
        }

        private void LoadNodeDir(TreeNode node)
        {
            if (!Directory.Exists(((tree_tag)node.Tag).strPath))
            {
                return;
            }

            Control ctrl_parent = lv1.Parent;
            ctrl_parent.Controls.Remove(lv1);

            if (((tree_tag)node.Tag).listv != null)
            {
                lv1 = ((tree_tag)node.Tag).listv;
                ctrl_parent.Controls.Add(lv1);
                lv1.Enabled = true;
                lv1.Visible = true;
                lv1.Select();
                autoSelect(lv1);
                return;
            }

            fl.Show();
            fl.Enabled = false;

            tv1.Enabled = false;

            lv1 = new ListView();

            lv1.SelectedIndexChanged    += new System.EventHandler(lv1_SelectedIndexChanged);
            lv1.DoubleClick             += new EventHandler(lv1_DoubleClick);
            lv1.LargeImageList          = imageList1;
            lv1.Dock                    = DockStyle.Fill;
            lv1.MultiSelect             = false;

            ctrl_parent.Controls.Add(lv1);
            //lv1.Clear();

            DirectoryInfo di = new DirectoryInfo(((tree_tag)node.Tag).strPath);
            fl.pbar.Maximum = di.GetFiles().Length;
            fl.pbar.Value = 0;
            ListView.ListViewItemCollection lvc = new ListView.ListViewItemCollection(lv1);
            lv1.Visible = false;
            lv2.Visible = true;

            if(this.showFolder == true)
            {
                string stroutout = string.Empty;
                foreach (DirectoryInfo d in di.GetDirectories())
                {
                    if (d.Name == ".svn" || d.Name == "Texture") continue;

                    Application.DoEvents();
                    fl.label1.Text = d.FullName;
                    if (imageList1.Images.ContainsKey(d.FullName))
                    {
                        lvc.Add(d.Name, d.FullName).Tag = "folder";
                        continue;
                    }
                    Image imgFolder = imageList3.Images["folder.bmp"];
                    FileInfo[] files = d.GetFiles(this.strFolderPreviewFilter);
                    if(files.Length == 0)
                    {
                        DirectoryInfo[] d111 = d.GetDirectories("模型");
                        if (d111.Length > 0)
                            files = d111[0].GetFiles(this.strFolderPreviewFilter);
                    }
                    //    
                    if(files.Length == 0)   //普通目录
                    {
                        imageList1.Images.Add(d.FullName, imgFolder);
                        lvc.Add(d.Name, d.FullName).Tag = "folder";
                        continue;
                    }
                    Debug.WriteLine(files[0].FullName);
                    stroutout += files[0].FullName + "\r\n";
                    sprViewer.SPRItem si = new sprViewer.SPRItem(files[0].FullName, sprViewer.SPRItem.PIC_TYPE.MDL, m_engine);

                    //check cache
                    if (File.Exists(si.tgaFileName + "!.BMP")
                        && d.LastWriteTime.ToString() == sprViewer.IniIO.IniReadValue("cache", "time", si.tgaFileName + "_tb.ini"))
                    {
                        Image img = new Bitmap(si.tgaFileName + "!.BMP");
                        imageList1.Images.Add(d.FullName, img);
                    }
                    else
                    {
                        int ind = si.TGAList.Count / 2;
                        ind = ind == 0 ? 1 : ind;
                        string strFile = si.TGAList[ind.ToString()].ToString();
                        if (File.Exists(strFile))
                        {
                            Image img = Image.FromFile(strFile);
                            Image imgsmall = img.GetThumbnailImage(80, 80, null, IntPtr.Zero);
                            //imgsmall.Save(@"C:\Documents and Settings\kuangsihao\桌面\newtools\_cache\mdl\folder.bmp");
                            Image imgCombo = new Bitmap(imgFolder);
                            Graphics g = Graphics.FromImage(imgCombo);
                            //g.DrawImage(imgsmall, new Point(0, 0));
                            g.DrawImage(imgsmall, new Rectangle(6, 8, imgsmall.Width, imgsmall.Height));
                            //g.DrawImage(img, new Rectangle(0, 0, img.Width, img.Height));
                            imgCombo.Save(si.tgaFileName + "!.BMP");
                            imageList1.Images.Add(d.FullName, imgCombo);
                            sprViewer.IniIO.IniWriteValue("cache", "time", d.LastWriteTime.ToString(), si.tgaFileName + "_tb.ini");
                        }
                    }
                    ListViewItem lvi = lvc.Add(d.Name, d.FullName);
                    lvi.Tag = "folder";
                }
            }

            foreach (FileInfo f in di.GetFiles())
            {
                Application.DoEvents();
                fl.pbar.Value++;
                fl.label1.Text = f.FullName;

                if (strFilter.IndexOf(";" + f.Extension.ToLower() + ";") == -1)
                {
                    continue;
                }

                if (imageList1.Images.ContainsKey(f.FullName))
                {
                    lvc.Add(f.Name, f.FullName).Tag = "file";
                    continue;
                }

                if (!multi_load)
                {
                    sprViewer.SPRItem.PIC_TYPE pt = sprViewer.SPRItem.PIC_TYPE.NULL;

                    switch (f.Extension.ToLower())
                    {
                        case ".mdl":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.MDL;
                                break;
                            }                            
                        case ".mtl":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.MTL;
                                break;
                            }                            
                        case ".mesh":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.MESH;
                                break;
                            }                            
                        case ".ani":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.ANI;
                                break;
                            }                            
                        case ".tani":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.ANI;
                                break;
                            }                            
                        case ".spr":
                            {
                                pt = sprViewer.SPRItem.PIC_TYPE.NULL;
                                break;
                            }                            
                    }

                    sprViewer.SPRItem si;

                    if (pt != sprViewer.SPRItem.PIC_TYPE.NULL)
                    {
                        si = new sprViewer.SPRItem(f.FullName, pt, m_engine, true);
                    }
                    else
                    {
                        si = new sprViewer.SPRItem(f.FullName);
                    }

                    //check cache
                    if (File.Exists(si.tgaFileName + ".BMP")
                     && f.LastWriteTime.ToString() == sprViewer.IniIO.IniReadValue("cache", "time", si.tgaFileName + "_tb.ini"))
                    {
                        Image img = new Bitmap(si.tgaFileName + ".BMP");
                        imageList1.Images.Add(f.FullName, img);
                    }
                    else
                    {
                        int ind = si.TGAList.Count / 2;
                        ind = ind == 0 ? 1 : ind;

                        if (ind <= si.TGAList.Count)
                        {
                            string file = si.TGAList[ind.ToString()].ToString();
                            if (File.Exists(file))
                            {
                                Image img = Image.FromFile(file);
                                Image imgsmall = img.GetThumbnailImage(48, 48, null, IntPtr.Zero);
                                imgsmall.Save(si.tgaFileName + ".BMP");
                                imageList1.Images.Add(f.FullName, imgsmall);
                                sprViewer.IniIO.IniWriteValue("cache", "time", f.LastWriteTime.ToString(), si.tgaFileName + "_tb.ini");
                            }
                        }
                    }
                }

                ListViewItem lvi = lvc.Add(f.Name, f.FullName);
                lvi.Tag = "file";
            }

            if (multi_load)
            {
                m_thread = new System.Threading.Thread(new System.Threading.ThreadStart(ThreadProc));
                m_thread.Start();
            }

            ((tree_tag)node.Tag).listv = lv1;

            //清多于nQueMax个ListView的缓存
            tag_que.Enqueue((tree_tag)node.Tag);
            while (tag_que.Count > nQueMax)
            {
                tree_tag tt = tag_que.Dequeue();
                tt.listv = null;
            }

            fl.Hide();
            tv1.Enabled = true;
            lv1.Visible = true;
            lv2.Visible = false;
            lv1.Select();
            autoSelect(lv1);

            OldLVC.Clear();
        }

        private void tv1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            LoadNodeDir(e.Node);
        }

        public void ThreadProc()
        {
            if (m_threadExit)
            {
                return;
            }

            TreeView.CheckForIllegalCrossThreadCalls = false;
            DirectoryInfo di = new DirectoryInfo(((tree_tag)this.tv1.SelectedNode.Tag).strPath);
            ListView.ListViewItemCollection lvc = new ListView.ListViewItemCollection(this.lv1);

            foreach (FileInfo f in di.GetFiles())
            {
                if (m_threadExit)
                {
                    return;
                }

                Application.DoEvents();
                fl.pbar.Value++;
                fl.label1.Text = f.FullName;

                if (strFilter.IndexOf(";" + f.Extension.ToLower() + ";") == -1)
                {
                    continue;
                }

                if (imageList1.Images.ContainsKey(f.FullName))
                {
                    continue;
                }

                sprViewer.SPRItem.PIC_TYPE pt = sprViewer.SPRItem.PIC_TYPE.NULL;

                switch (f.Extension.ToLower())
                {
                    case ".mdl":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.MDL;
                            break;
                        }                        
                    case ".mtl":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.MTL;
                            break;
                        }                        
                    case ".mesh":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.MESH;
                            break;
                        }                        
                    case ".ani":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.ANI;
                            break;
                        }                        
                    case ".tani":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.ANI;
                            break;
                        }                        
                    case ".spr":
                        {
                            pt = sprViewer.SPRItem.PIC_TYPE.NULL;
                            break;
                        }                        
                }

                sprViewer.SPRItem si;

                if (pt != sprViewer.SPRItem.PIC_TYPE.NULL)
                {
                    si = new sprViewer.SPRItem(f.FullName, pt, m_engine);
                }
                else
                {
                    si = new sprViewer.SPRItem(f.FullName);
                }
                
                //check cache
                if(File.Exists(si.tgaFileName + ".BMP") 
                    && f.LastWriteTime.ToString() == sprViewer.IniIO.IniReadValue("cache","time",si.tgaFileName + "_tb.ini"))
                {
                    Image img = new Bitmap(si.tgaFileName + ".BMP");
                    imageList1.Images.Add(f.FullName, img);
                }
                else
                {
                    int ind = si.TGAList.Count / 2 ;
                    ind = ind == 0 ? 1 : ind;
                    if (ind <= si.TGAList.Count)
                    {
                        string file = si.TGAList[ind.ToString()].ToString();
                        if (File.Exists(file))
                        {
                            try
                            {
                                Image img = Image.FromFile(file);
                                Image imgsmall = img.GetThumbnailImage(48, 48, null, IntPtr.Zero);
                                imgsmall.Save(si.tgaFileName + ".BMP");
                                imageList1.Images.Add(f.FullName, imgsmall);
                                sprViewer.IniIO.IniWriteValue("cache", "time", f.LastWriteTime.ToString(), si.tgaFileName + "_tb.ini");
                            }
                            catch (System.Exception )
                            {
                            }
                        }
                    }
                }
            }
        }

        //自动选择文件
        private void autoSelect(ListView lv)
        {
            foreach (ListViewItem lvi in lv.Items)
            {
                if (lvi.ImageKey == strSelectedFileName)
                {
                    lvi.Focused = true;
                    lvi.Selected = true;
                    lvi.EnsureVisible();
                    lvi.Focused = true;
                    lvi.Selected = true;
                    break;
                }
                else
                {
                    lvi.Selected = false;
                    lvi.Focused = false;
                }
            }
        }

        /// <summary>
        /// 停止线程
        /// </summary>
        private void StopThread()
        {
            m_threadExit = true;
        }

        private void lv1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lv1.SelectedItems.Count != 1)
            {
                return;
            }

            this.textCat.Text = string.Empty;
            this.textDesc.Text = string.Empty;
            thisMdlDescExistDB = false;

            if (lv1.SelectedItems[0].Tag.ToString() == "folder")
            {
                string strFolderName = ((tree_tag)tv1.SelectedNode.Tag).strPath + "\\" + lv1.SelectedItems[0].Text;
                if (!Directory.Exists(strFolderName)) return;
                FileInfo[] files = (new DirectoryInfo(strFolderName)).GetFiles(this.strFolderPreviewFilter);
                if (files.Length == 0) return;
                sprViewObject.SetFileName(files[0].FullName, m_engine);
            }
            else
            {
                string filename = ((tree_tag)tv1.SelectedNode.Tag).strPath + "\\" + lv1.SelectedItems[0].Text;
                sprViewObject.SetFileName(filename, m_engine);

                if (filename.ToLower().EndsWith(".mdl"))
                {
                    string keyFileName = filename.Substring(sprViewControl.m_strBase.Length);
                    string sql = string.Format("SELECT * FROM dic_mdl_desc WHERE keyFileName = '{0}'", keyFileName);
                    DataTable tbl = GetDataTable(sql, Conn);

                    if (tbl.Rows.Count > 0)
                    {
                        thisMdlDescExistDB = true;
                        this.textCat.Text = tbl.Rows[0]["textCat"].ToString().Trim();
                        string description = tbl.Rows[0]["textDesc"].ToString().Trim();
                        this.textDesc.Text = description;
                        this.textDesc.Font = normalFont;
                        this.textCat.Font = normalFont;

                        if (description != "")
                        {
                            MessageBox.Show("该模型有特殊用途，请参照描述使用！", "模型信息",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                    }
                }
            }
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            StopThread();
        }

        private void frmSelect_FormClosing(object sender, FormClosingEventArgs e)
        {
            tv1.SelectedNode = tv1.Nodes[0];
        }

        private void lv1_DoubleClick(object sender, EventArgs e)
        {
            if (lv1.SelectedItems.Count != 1)
            {
                return;
            }
            if (lv1.SelectedItems[0].Tag.ToString() == "folder")
            {
                foreach (TreeNode tn in tv1.SelectedNode.Nodes)
                {
                    if(tn.Text == lv1.SelectedItems[0].Text)
                    {
                        tv1.SelectedNode = tn;
                        break;
                    }
                }
            }
        }

        private void buttonX2_Click(object sender, EventArgs e) // 向上
        {
            if (tv1.SelectedNode.Parent != null)
            {
                tv1.SelectedNode = tv1.SelectedNode.Parent;
            }
        }

        private void textDesc_TextChanged(object sender, EventArgs e)
        {
            this.textDesc.Font = boldFont;
        }

        private void textCat_TextChanged(object sender, EventArgs e)
        {
            this.textCat.Font = boldFont;
        }

        private void cmdSaveContent_Click(object sender, EventArgs e) // 保存注释
        {
            if (this.lv1.SelectedItems.Count != 1)
                return;

            string strTextCat = this.textCat.Text;
            strTextCat = strTextCat.Replace("'", "\"");
            strTextCat = strTextCat.Replace("--", "~~");
            string strTextDesc = this.textDesc.Text;
            strTextDesc = strTextDesc.Replace("'", "\"");
            strTextDesc = strTextDesc.Replace("--", "~~");

            string filename = ((tree_tag)tv1.SelectedNode.Tag).strPath + "\\" + lv1.SelectedItems[0].Text;
            string keyFileName = filename.Substring(sprViewControl.m_strBase.Length);
            if (filename.ToLower().EndsWith(".mdl"))
            {
                string sqlAdd = string.Format("INSERT INTO dic_mdl_desc (keyFileName, textCat, textDesc) VALUES ('{0}', '{1}', '{2}')", keyFileName, strTextCat, strTextDesc);
                string sqlModify = string.Format("UPDATE dic_mdl_desc SET textCat = '{0}', textDesc = '{1}' WHERE keyFileName = '{2}'", strTextCat, strTextDesc, keyFileName);
                string sql = thisMdlDescExistDB ? sqlModify : sqlAdd;
                SqlExec(sql, Conn);

                this.textDesc.Font = normalFont;
                this.textCat.Font = normalFont;
            }
        }

        private DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        private void SqlExec(string sql, SqlConnection conn)
        {
            conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            conn.Close();
        }

        private void cmdFindMdlRef_Click(object sender, EventArgs e)
        {
            if (lv1.SelectedItems.Count != 1)
            {
                MessageBox.Show("必须选定且只能选择一个文件。");
                return;
            }

            this.listBox1.Items.Clear();

            string filename = lv1.SelectedItems[0].ImageKey.Substring(sprViewControl.m_strBase.Length);
            string sql = string.Format("SELECT RepresentID FROM npc WHERE MainModelFile = '{0}'", filename);
            DataTable tbl_npc = GetDataTable(sql, Conn);
            if (tbl_npc.Rows.Count == 0)
            {
                this.listBox1.Items.Add("(找不到存在的引用。)");
                return;
            }

            sql = string.Format("SELECT * FROM NpcTemplate");
            DataTable tbl_npc_temp = GetDataTable(sql, Conn);
            foreach (DataRow row in tbl_npc_temp.Rows)
            {
                for (int i = 1; i <= 10; i++)
                {
                    if (tbl_npc.Rows.Find(row["RepresentID" + i.ToString()]) != null)
                    {
                        string strOut = string.Format("NPC模板[{0}, {1}]], NPC表现{2}, 表现ID[{3}]", row["ID"], row["Name"], i, row["RepresentID" + i.ToString()]);
                        this.listBox1.Items.Add(strOut);
                    }
                }
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            string stext = (sender as TextBox).Text.ToLower(); // 获取文本框中的内容
            if (stext.Length != 0 && stext != "关键字过滤...") // 关键字不为空
            {
                foreach (ListViewItem item in lv1.Items)
                {
                    string thisitemtext = item.Text.ToLower();
                    if (!thisitemtext.Contains(stext))
                    {
                        item.Remove();
                        OldLVC.Add(item);
                    }
                }

                ArrayList todel = new ArrayList();
                foreach (ListViewItem item in OldLVC)
                {
                    string thisitemtext = item.Text.ToLower();
                    if (thisitemtext.Contains(stext))
                    {
                        lv1.Items.Add(item);
                        todel.Add(item);
                    }
                }
                foreach (ListViewItem item in todel)
                {
                    OldLVC.Remove(item);
                }
            }
            else
            {
                foreach (ListViewItem item in OldLVC)
                    lv1.Items.Add(item);
                OldLVC.Clear();
            }
        }

        private void textBox1_MouseClick(object sender, MouseEventArgs e)
        {
            if ((sender as TextBox).Text == "关键字过滤...")
            {
                (sender as TextBox).Text = string.Empty; // 清空输入框
            }
        }

        private void cmdRefresh_Click(object sender, EventArgs e)
        {
            m_engine.ToggleEngine();
            multi_load = false;
            ((tree_tag)(this.tv1.SelectedNode.Tag)).listv = null;
            LoadNodeDir(this.tv1.SelectedNode);
            multi_load = true;
            m_engine.ToggleEngine();
        }

        private void cmdSocketSelect_Click(object sender, EventArgs e)
        {
            strSelectedFileName = this.comboBox1.Text;
            this.Close();
            this.DialogResult = DialogResult.OK;
        }

    }
    public class tree_tag
    {
        public string strPath;
        public ListView listv;
        public tree_tag()
        {
            listv = null;
            strPath = null;
        }
    };
}