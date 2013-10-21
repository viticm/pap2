using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Data.SqlClient;
using System.Diagnostics;

namespace IconSelector
{
    public partial class FormSelector : Form
    {
        private object[] m_iconsinfo;
        private int m_nSelectedID = -1;
        private ArrayList m_allicons;
        private string m_strUndefined = "<未定义>";
        private TreeNode OldDragOverNode = null;

        private bool m_editMode = false;
        Hashtable m_htNameValue = null;
        ListViewItem[] m_hidenItems = null;

        private bool m_bFindID = true;
        private int m_iFindID = 0;
        private string m_strFindUitex = string.Empty;

        public FormSelector()
        {
            InitializeComponent();
            imageIcons.Images.Clear();
            listview.Items.Clear();
            MyInitControls();
            m_htNameValue = new Hashtable();
            m_htNameValue.Add(0,  "[ 0] 饰品");
            m_htNameValue.Add(1,  "[ 1] 防具（衣裤类）");
            m_htNameValue.Add(2,  "[ 2] 陶瓷");
            m_htNameValue.Add(3,  "[ 3] 布匹");
            m_htNameValue.Add(4,  "[ 4] 药草");
            m_htNameValue.Add(5,  "[ 5] 铁器");
            m_htNameValue.Add(6,  "[ 6] 金钱");
            m_htNameValue.Add(7,  "[ 7] 纸张");
            m_htNameValue.Add(8,  "[ 8] 戒指");
            m_htNameValue.Add(9,  "[ 9] 石器");
            m_htNameValue.Add(10, "[10] 金属武器（枪,剑,双兵）");
            m_htNameValue.Add(11, "[11] 棍（木质武器）");
            m_htNameValue.Add(12, "[12] 笔、暗器、木材（木质短武器）");
            m_htNameValue.Add(14, "[14] 有容器的液体（饮料类）");
            m_htNameValue.Add(15, "[15] 肉");
            m_htNameValue.Add(16, "[16] 食品");
            m_htNameValue.Add(17, "[17] 药丸");
            foreach (DictionaryEntry de in m_htNameValue)
            {
                this.txtSoundID.Items.Add(de.Value);
            }
        }

        private void MyInitControls()
        {
            // 导入 按钮的初始化
            DevComponents.DotNetBar.ButtonItem itemIniOnly = new DevComponents.DotNetBar.ButtonItem("ImportSingle", "单帧");
            itemIniOnly.Click += new EventHandler(this.buttonX1_Click);
            this.buttonX1.SubItems.Add(itemIniOnly);

            DevComponents.DotNetBar.ButtonItem itemLogicAlso = new DevComponents.DotNetBar.ButtonItem("ImportMulti", "多帧");
            itemLogicAlso.Click += new EventHandler(this.buttonX1_Multi_Click);
            this.buttonX1.SubItems.Add(itemLogicAlso);
        }

        public void EnterEditMode()
        {
            m_editMode = true;
            label1.Visible = true;
            txtTag1.Visible = true;
            label2.Visible = true;
            txtTag2.Visible = true;
            buttonX1.Visible = true;
            label3.Visible = true;
            txtSoundID.Visible = true;
            checkSoundIdOnly.Visible = true;
        }

        public object[] IconsInfo
        {
            set { m_iconsinfo = value; }
        }
        public int SelectedID
        {
            get { return m_nSelectedID; }
            set { m_nSelectedID = value; }
        }

        private void FormSelector_Load(object sender, EventArgs e)
        {
            m_allicons = new ArrayList();
            listview.Items.Clear();
            treeView.Nodes.Clear();
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (listview.SelectedItems.Count > 0)
            {
                Info info = (Info)listview.SelectedItems[0].Tag;
                m_nSelectedID = info.id;
            }
            else
                m_nSelectedID = -1;
        }

        private void FormSelector_Shown(object sender, EventArgs e)
        {
            Info SelectedInfo = null;
            foreach (object obj in m_iconsinfo)
            {
                Info info = (Info)obj;
                //ListViewItem item = listview.Items.Add(info.fielname, info.ImageIndex);
                //item.Tag = info;

                TreeNode node;
                if (!treeView.Nodes.ContainsKey(info.kind))
                {
                    node = treeView.Nodes.Add(info.kind, info.kind);
                }
                else
                    node = treeView.Nodes[info.kind];
                
                if (node.Tag == null)
                    node.Tag = new ArrayList();
                ArrayList arr = (ArrayList)node.Tag;

                arr.Add(info);
                m_allicons.Add(info);

                // ahpho
                // 在这里加入第二层的处理
                TreeNode _2ndNode = null;
                TreeNode _undefineNode = null;
                if (!node.Nodes.ContainsKey(m_strUndefined))
                {
                    _undefineNode = node.Nodes.Add(m_strUndefined, m_strUndefined);
                }
                else
                {
                    _undefineNode = node.Nodes[m_strUndefined];
                }

                if (info.subKind == string.Empty)
                {
                    _2ndNode = _undefineNode;
                }
                else
                {
                    if (!node.Nodes.ContainsKey(info.subKind))
                        _2ndNode = node.Nodes.Add(info.subKind, info.subKind);
                    else
                        _2ndNode = node.Nodes[info.subKind];
                }

                if (_2ndNode.Tag == null)
                    _2ndNode.Tag = new ArrayList();
                ArrayList _2ndArr = _2ndNode.Tag as ArrayList;
                _2ndArr.Add(info);

                if (SelectedInfo == null && info.id == SelectedID)
                {
                    SelectedInfo = info;
                }
            }
            if (SelectedInfo != null)
            {
                TreeNode _node = FindTreeNodeWithInfo(SelectedInfo);
                treeView.SelectedNode = _node;


//                 TreeNode[] nodes = treeView.Nodes.Find(SelectedInfo.kind, true);
//                 if (nodes != null && nodes.Length > 0)
//                     treeView.SelectedNode = nodes[0];
            }
            listview.Select();

            DataTable tbl = FormScan.GetDataTable("select Tag1 from icon group by Tag1");
            foreach (DataRow row in tbl.Rows)
            {
                if (row[0].ToString() != string.Empty)
                    txtTag1.Items.Add(row[0].ToString());
            }
            tbl = FormScan.GetDataTable("select Tag2 from icon group by Tag2");
            foreach (DataRow row in tbl.Rows)
            {
                if (row[0].ToString() != string.Empty)
                    txtTag2.Items.Add(row[0].ToString());
            }

        }

        private TreeNode FindTreeNodeWithInfo(Info info)
        {
            TreeNode[] nodes = treeView.Nodes.Find(info.kind, false);
            if (nodes != null && nodes.Length == 1)
            {
                TreeNode[] subNodes = nodes[0].Nodes.Find(info.subKind, false);
                if (subNodes != null && subNodes.Length == 1)
                    return subNodes[0];
            }
            return null;
        }

        private void ReloadUsedCount()
        {
            DataTable tbl = FormScan.GetDataTable("select *,(select count(*) from tbl_icon_item where icon.id=tbl_icon_item.iconid) as c from icon");
            for (int i = 0; i < tbl.Rows.Count; i++)
            {
                DataRow row = tbl.Rows[i];
                int id = Convert.ToInt32(row["ID"]);
                int usedcount = Convert.ToInt32(row["c"]);
                foreach (object o in m_iconsinfo)
                {
                    if (o is Info)
                    {
                        Info inf = o as Info;
                        if(inf.id == id)
                        {
                            inf.usedcount = usedcount;
                        }
                    }
                }
            }
        }

        private void treeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            listview.Items.Clear();
            ReloadUsedCount();
            if (e.Node.Tag != null && e.Node.Tag is ArrayList)
            {
                if (e.Node.Level == 0) // 点在第一层
                {
                    ArrayList arr = (ArrayList)e.Node.Tag;
                    string strKind = e.Node.Text;
                    foreach (object obj in arr)
                    {
                        Info info = (Info)obj;
                        if (info.kind == strKind)
                        {
                            // ListViewItem item = listview.Items.Add("[" + info.usedcount.ToString() + "]" + info.fielname.Split(new char[] { '.' })[0], info.ImageIndex);
                            ListViewItem item = listview.Items.Add(string.Format("[{0}]{1}\r\n{2}", info.usedcount.ToString(), info.fielname.Split(new char[] { '.' })[0], info.id.ToString())
                                                                   , info.ImageIndex);
                            item.Tag = info;
                        }
                    }
                }
                else if (e.Node.Level == 1) // 第二层
                {
                    ArrayList arr = (ArrayList)e.Node.Tag;
                    string strSubKind = e.Node.Text;
                    foreach (object obj in arr)
                    {
                        Info info = (Info)obj;
                        if (info.subKind == strSubKind || info.subKind == string.Empty && strSubKind == m_strUndefined)
                        {
                            // ListViewItem item = listview.Items.Add("[" + info.usedcount.ToString() + "]" + info.fielname.Split(new char[] { '.' })[0], info.ImageIndex);
                            ListViewItem item = listview.Items.Add(string.Format("[{0}]{1}\r\nID: {2}", info.usedcount.ToString(), info.fielname.Split(new char[] { '.' })[0], info.id.ToString())
                                                                   , info.ImageIndex);
                            item.Tag = info;
                        }
                    }
                }
            }

            if (SelectedID > -1)
            {
                foreach (ListViewItem item in listview.Items)
                {
                    Info info = (Info)item.Tag;
                    if (info.id == SelectedID)
                    {
//                         TreeNode[] nodes = treeView.Nodes.Find(info.kind, true);
//                         if (nodes.Length > 0)
//                             treeView.SelectedNode = nodes[0];
                        item.Selected = true;
                        item.Focused = true;
                        //item.Position = new Point(30, 30);
                        item.EnsureVisible();
                        break;
                    }
                }
            }
            else
            {
                //if (listview.TopItem != null)
                //    listview.TopItem.Selected = true;
            }
        }

        private void listview_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listview.SelectedItems != null && listview.SelectedItems.Count == 1)
            {
                Info inf = (Info)listview.SelectedItems[0].Tag;
                SelectedID = inf.id;
                txtTag1.Text = inf.tag1;
                txtTag2.Text = inf.tag2;
                if (inf.soundID != -1)
                {
                    if (m_htNameValue[inf.soundID] != null)
                        txtSoundID.Text = m_htNameValue[inf.soundID].ToString();
                    else
                        txtSoundID.Text = inf.soundID.ToString();
                }
                btnXOK.Enabled = true;
            }
            else
            {
                txtTag1.Text = string.Empty;
                txtTag2.Text = string.Empty;
                txtSoundID.Text = string.Empty;
                btnXOK.Enabled = false;
            }
        }

        private void panelEx1_Click(object sender, EventArgs e)
        {

        }

        private static int addon = 0;
        private static int addon_2 = 0;
        private void popNewNode_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode.Level == 0) // 新建第一层
            {
                addon++;
                TreeNode tn = null;
                while (treeView.Nodes.Find("新分类" + addon.ToString(), false).Length > 0)
                {
                    addon++;
                }
                tn = treeView.Nodes.Add("新分类" + addon.ToString(), "新分类" + addon.ToString());
                tn.Tag = new ArrayList();
            }
            else if (treeView.SelectedNode.Level == 1) // 第二层 
            {
                addon_2++;
                TreeNode tn = null;
                while (treeView.SelectedNode.Parent.Nodes.Find("新分类" + addon_2.ToString(), false).Length > 0)
                {
                    addon_2++;
                }
                tn = treeView.SelectedNode.Parent.Nodes.Add("新分类" + addon_2.ToString(), "新分类" + addon_2.ToString());
                tn.Tag = new ArrayList();
            }
        }

        private void popDelNode_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode == null) return;
            ArrayList ilist = treeView.SelectedNode.Tag as ArrayList;
            if (ilist.Count > 0)
            {
                MessageBox.Show("请先删除分类中的图标");
                return;
            }
            treeView.Nodes.Remove(treeView.SelectedNode);
        }

        private ListView.SelectedListViewItemCollection m_selecteditems = null;
        private TreeNode m_selectednode = null;

        private void listview_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if (!m_editMode)
                return;

            if (treeView.SelectedNode.Level == 0) // 选中第一层不允许拖
                return;

            m_selecteditems = this.listview.SelectedItems;
            this.DoDragDrop(this.listview.SelectedItems, DragDropEffects.Move);
        }

        private void treeView_DragDrop(object sender, DragEventArgs e)
        {
            if (!m_editMode)
                return;

            ListView.SelectedListViewItemCollection lvc = m_selecteditems;
            m_selecteditems = null;
            foreach (TreeNode tn in treeView.Nodes)
            {
                tn.BackColor = Color.White;
                tn.ForeColor = Color.Black;
            }
            if (lvc.Count == 0) return;
            if (m_selectednode == null) return;
            if (treeView.SelectedNode == null) return;
            if (OldDragOverNode != null)
            {
                OldDragOverNode.BackColor = Color.White;
                OldDragOverNode.ForeColor = Color.Black;
            }

            // m_selectednode: 拖动的目的
            // treeview.selectedNode: 源

            if (m_selectednode.Level == 0) // 不允许拖到第一层
                return;

            //从内存中移动图标
            ArrayList afrom = treeView.SelectedNode.Tag as ArrayList;
            ArrayList ato = m_selectednode.Tag as ArrayList;
            ArrayList aPreDel1 = new ArrayList();
            ArrayList aPreDel2 = new ArrayList();

            ArrayList afromParent = treeView.SelectedNode.Parent.Tag as ArrayList;
            ArrayList atoParent = m_selectednode.Parent.Tag as ArrayList;
            ArrayList aPreDel1Parent = new ArrayList();
            //ArrayList aPreDel2Parent = new ArrayList();

            foreach (ListViewItem lvi in lvc)
            {
                foreach (Info info in afrom)
                {
                    if (info.fielname == (lvi.Tag as Info).fielname && info.ImageIndex == lvi.ImageIndex)
                    {
                        //修改数据库
                        tgaIO.ResetAllKind(info.id.ToString(), m_selectednode.Parent.Text, m_selectednode.Text); // ahpho这个函数要改
                        //修改数据库完毕
                        aPreDel1.Add(info);
                        aPreDel1Parent.Add(info);

                        info.kind = m_selectednode.Parent.Text;
                        info.subKind = m_selectednode.Text;
                        ato.Add(info);
                        atoParent.Add(info);
                    }
                }
                aPreDel2.Add(lvi);
            }

            foreach (object obj in aPreDel1)
            {
                afrom.Remove(obj);
            }
            foreach (object obj in aPreDel1Parent)
            {
                afromParent.Remove(obj);
            }
            foreach (object obj in aPreDel2)
            {
                listview.Items.Remove(obj as ListViewItem);
            }

            listview.Refresh();
        }
        private void treeView_DragLeave(object sender, EventArgs e)
        {
            if (!m_editMode)
                return;

            if (OldDragOverNode != null)
            {
                OldDragOverNode.BackColor = Color.White;
                OldDragOverNode.ForeColor = Color.Black;
            }
        }
        private void treeView_DragOver(object sender, DragEventArgs e)
        {
            Point ptScreen = new Point(e.X, e.Y);
            Point pt = treeView.PointToClient(ptScreen);
            TreeNode item =  treeView.GetNodeAt(pt.X, pt.Y);
            if (item == OldDragOverNode)
            {
                return;
            }

            item.BackColor = Color.Blue;
            item.ForeColor = Color.White;

            if (OldDragOverNode != null)
            {
                OldDragOverNode.BackColor = Color.White;
                OldDragOverNode.ForeColor = Color.Black;
            }

            OldDragOverNode = item;
            m_selectednode = item;
        }

        private void treeView_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }

        private void treeView_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            string oldValue = e.Node.Text;
            string newValue = e.Label;
            if (oldValue == newValue) return;
            if (e.Label == null) return;
            if (newValue.Trim() == "")
            {
                e.CancelEdit = true;
                return;
            }

            if (e.Node.Level == 0) // 改大类
            {
                foreach (TreeNode tn in treeView.Nodes)
                {
                    if (tn.Text == newValue)
                    {
                        e.CancelEdit = true;
                        MessageBox.Show("名称重复");
                        return;
                    }
                }
                //修改数据库和node上的内存
                foreach (Info info in e.Node.Tag as ArrayList)
                {
                    info.kind = newValue;
                    tgaIO.ResetKind(info.id.ToString(), newValue);
                }
            }
            else if (e.Node.Level == 1) // 改小类
            {
                foreach (TreeNode tn in e.Node.Parent.Nodes)
                {
                    if (tn.Text == newValue)
                    {
                        e.CancelEdit = true;
                        MessageBox.Show("名称重复");
                        return;
                    }
                }
                //修改数据库和node上的内存
                foreach (Info info in e.Node.Tag as ArrayList)
                {
                    info.subKind = newValue;
                    tgaIO.ResetSubKind(info.id.ToString(), newValue);
                }
            }

            //修改m_iconsinfo（貌似这种举动没有任何必要，因为node上得info和m_iconsinfo中的info指向的地址一样）
            foreach (Info info in m_iconsinfo)
            {
                if (info.kind == oldValue)
                {
                    info.kind = newValue;
                }
            }
        }

        private void treeView_BeforeLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            
        }

        private void popRename_Click(object sender, EventArgs e)
        {
            if (treeView.SelectedNode == null) return;
            treeView.SelectedNode.BeginEdit();
        }

        private void txtTag_Leave(object sender, EventArgs e)
        {
            int selectedCount = listview.SelectedItems.Count;
            if (selectedCount == 0) return;

            ComboBox txtTag = null;
            string fieldname = string.Empty;
            bool changed = true;
            if ((sender as ComboBox).Name == "txtTag1")
            {
                txtTag = txtTag1;
                fieldname = "Tag1";
            }
            else
            {
                txtTag = txtTag2;
                fieldname = "Tag2";
            }

            int[] ids = new int[selectedCount];
            string newText = txtTag.Text;

            if (selectedCount == 1)
            {
                Info inf = listview.SelectedItems[0].Tag as Info;
                if (inf == null) return;

                ids[0] = inf.id;
                tgaIO.UpdateTag12(ids, fieldname, newText);
                if ((sender as ComboBox).Name == "txtTag1")
                    inf.tag1 = newText;
                else
                    inf.tag2 = newText;
            }
            else if (selectedCount > 1)
            {
                int i = 0;
                foreach (ListViewItem lvitem in listview.SelectedItems)
                {
                    ids[i++] = (lvitem.Tag as Info).id;
                    if ((sender as ComboBox).Name == "txtTag1")
                        (lvitem.Tag as Info).tag1 = newText;
                    else
                        (lvitem.Tag as Info).tag2 = newText;
                }
                tgaIO.UpdateTag12(ids, fieldname, newText);
            }
            if (!txtTag2.Items.Contains(newText))
                txtTag.Items.Add(newText);
        }

        private void txtSoundID_Leave(object sender, EventArgs e)
        {
            int selectedCount = listview.SelectedItems.Count;
            if (selectedCount == 0) return;

            string fieldname = "SoundID";
            int[] ids = new int[selectedCount];
            string newText = txtSoundID.Text;
            if (newText.IndexOf('[') != -1)
                newText = newText.Substring(1, 2); // 转换
            
            if (newText.Length == 0)
                return;

            if (selectedCount == 1)
            {
                Info inf = listview.SelectedItems[0].Tag as Info;
                if (inf == null) return;

                ids[0] = inf.id;
                tgaIO.UpdateTag12(ids, fieldname, newText);
                inf.soundID = Convert.ToInt32(newText);
            }
            else if (selectedCount > 1)
            {
                int i = 0;
                foreach (ListViewItem lvitem in listview.SelectedItems)
                {
                    ids[i++] = (lvitem.Tag as Info).id;
                    (lvitem.Tag as Info).soundID = Convert.ToInt32(newText);
                }
                tgaIO.UpdateTag12(ids, fieldname, newText);
            }
        }


        [DllImport("DllMergeTga.dll")]
        private static extern int TgaToUitex(string strFileTga, string strOutputFolder);
        private void buttonX1_Click(object sender, EventArgs e) // 导入
        {
            CD1.Multiselect = true;
            CD1.Filter = "TGA (*.tga)|*.tga";
            DialogResult dr =  CD1.ShowDialog();
            if (dr == DialogResult.OK)
            {
                bool bYesToAll = false;
                bool bNoToAll = false;
                bool bCancel = false;

                foreach (string strSelectedFileName in CD1.FileNames)
                {
                    if (bCancel)
                        break;

                    bool bNeedDoThisTime = false;

                    if (!File.Exists(strSelectedFileName)) continue;
                    FileInfo tgafile = new FileInfo(strSelectedFileName);

                    DirectoryInfo dirIcon = new DirectoryInfo(tgaIO.StrIconFolder);
                    if (dirIcon.GetFiles(tgafile.Name).Length > 0)
                    {
                        //包含重复文件

                        if (bNoToAll)
                        {
                            continue;
                        }
                        else if (bYesToAll)
                        {
                            bNeedDoThisTime = true;
                        }
                        else
                        {

                            respond form = new respond("已经包含同名文件[" + tgafile.Name + "]，是否覆盖？");
                            if (form == null)
                            {
                                MessageBox.Show("G。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return;
                            }
                            form.ShowDialog();
                            switch (form.m_respond)
                            {
                                case "yestoall":
                                    bYesToAll = true;
                                    bNeedDoThisTime = true;
                                    break;
                                case "yes":
                                    bNeedDoThisTime = true;
                                    break;
                                case "no":
                                    continue;
                                case "notoall":
                                    bNoToAll = true;
                                    continue;
                                case "cancel":
                                    bCancel = true;
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    else
                    {
                        bNeedDoThisTime = true;
                    }

                    if (bNeedDoThisTime)
                    {
                        //生成文件
                        //File.Copy(strSelectedFileName, Path.Combine(tgaIO.StrIconFolder, tgafile.Name));
                        TgaToUitex(strSelectedFileName, tgaIO.StrIconFolder);

                        //导入数据
                        tgaIO.InsertNewRecord(tgafile.Name.Split(new char[] { '.' })[0] + ".UITex", "", "");
                    }
                }

                this.Visible = false;
                this.Refresh();
                this.Close();

                Implement parent = this.Tag as Implement;
                parent.Init(tgaIO.sqlConn, tgaIO.StrIconFolder);
                parent.Show();
            }
        }

        private void buttonX1_Multi_Click(object sender, EventArgs e)
        {
            string strWholeExePath = Application.StartupPath + "/MergeTga.exe";
            if (!File.Exists(strWholeExePath))
            {
                MessageBox.Show("找不到MergeTga工具。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            Process p = Process.Start(strWholeExePath, "start");
            p.WaitForExit();
            MessageBox.Show("工具退出。");
        }

        private void checkSoundIdOnly_CheckedChanged(object sender, EventArgs e)
        {
            if (this.checkSoundIdOnly.Checked)
            {
                m_hidenItems = new ListViewItem[listview.Items.Count];
                int i = 0;
                foreach (ListViewItem item in listview.Items)
                {
                    Info inf = item.Tag as Info;
                    if (inf.soundID != -1)
                    {
                        m_hidenItems[i++] = item;
                        item.Remove();
                    }
                }
            }
            else
            {
                foreach (ListViewItem item in m_hidenItems)
                {
                    if (item != null)
                        listview.Items.Add(item);
                }
                m_hidenItems = null;
            }
        }

        private void textBoxFindID_TextChanged(object sender, EventArgs e)
        {
            if (this.textBoxFindID.Text.Length == 0)
                return;

            // 查找依据是什么
            try
            {
                m_iFindID = Convert.ToInt32(this.textBoxFindID.Text);
                m_bFindID = true;
            }
            catch /*(System.Exception ex)*/
            {
                m_strFindUitex = this.textBoxFindID.Text + ".UITex";
                m_bFindID = false;
            }

            // 找到它的kind subKind
            string kind = string.Empty;
            string subkind = string.Empty;

            foreach (Info info in m_allicons)
            {
                if (m_bFindID)
                {
                    if (info.id == m_iFindID)
                    {
                        kind = info.kind;
                        subkind = info.subKind;
                        break;
                    }
                }
                else
                {
                    if (info.fielname == m_strFindUitex)
                    {
                        kind = info.kind;
                        subkind = info.subKind;
                        break;
                    }
                }
            }

            // 选择这个结点
            foreach (TreeNode lv1node in this.treeView.Nodes)
            {
                if (lv1node.Text == kind)
                {
                    foreach (TreeNode lv2node in lv1node.Nodes)
                    {
                        if (lv2node.Text == subkind /*|| (lv2node.Text == "<未定义>" && subkind == "未分类")*/)
                        {
                            this.treeView.SelectedNode = lv2node;
                            break;
                        }
                    }
                    break;
                }
            }


        }

        private void buttonX2_Click(object sender, EventArgs e)//查找
        {
            foreach (ListViewItem item in this.listview.Items)
            {
                item.Selected = false;
            }

            foreach (ListViewItem item in this.listview.Items)
            {

                Info info = (Info)item.Tag;
                if (m_bFindID)
                {
                    if (info.id == m_iFindID)
                    {
                        item.Selected = true;
                        break;
                    }
                }
                else
                {
                    if (info.fielname == m_strFindUitex)
                    {
                        item.Selected = true;
                        break;
                    }
                }
            }

            this.listview.Focus();
        }

    }



}