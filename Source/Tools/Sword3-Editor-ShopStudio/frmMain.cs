///===================================
///Purpose:
///
///Created By: WilsonWei
///Created On: 2007/02/07
///
///(c)Kingsoft Corporation
///Comments:
///
///
///
///
///====================================
///
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using XSJ.Sword3.Editor.Studio.Component;
using XSJ.Sword3.Editor.Studio.Data;
using XSJ.Sword3.Editor.Studio.AppLogic;
using XSJ.Sword3.Editor.Studio;

namespace XSJ.Sword3.Editor.Studio
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 全局修改标志位
        /// </summary>
        private static bool m_isDirty = false;
        private ShopItemCollection m_shopitems = null;
        private ItemCategoryCollection m_itemcategories = null;

        private void frmMain_Load(object sender, EventArgs e)
        {
            tableLayoutPanel1.Dock = DockStyle.Fill;
            string errmsg = string.Empty;
            string stackinfo = string.Empty;
            App.AppVariables.g_dataprovider.Init(
                    App.AppVariables.g_connstring.Replace("%s1", "sa").Replace("%s2", "ddd12345678"));
            bool bret = App.AppVariables.g_dataprovider.TryConnect(out errmsg, out stackinfo);
            if (true == bret)
            {
                App.AppVariables.g_logon = true;
            }
            if (XSJ.Sword3.Editor.Studio.App.AppVariables.g_logon == true)
            {
                _initControlEvents();

                _initShopList();
                _initTVItemCategory();
                ItemCollection items = ItemLogic.GetAllItems();
                _initItems(ref items);
            }

            //Other Initializations
            lblShopID.Text = string.Empty;
            tvItemCategory.ExpandAll();
            tvShopList.ExpandAll();
        }

        private void frmMain_Disposed(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void _initControlEvents()
        {
            tvShopList.BeforeSelect += new TreeViewCancelEventHandler(tvShopList_BeforeSelect);
            tvShopList.AfterSelect += new TreeViewEventHandler(tvShopList_AfterSelect);
            tvShopList.MouseDown += new MouseEventHandler(tvShopList_MouseDown);
            tvShopList.AfterLabelEdit += new NodeLabelEditEventHandler(tvShopList_AfterLabelEdit);
            tvShopList.KeyDown += new KeyEventHandler(tvShopList_KeyDown);
            dgvShopItems.RowEnter += new DataGridViewCellEventHandler(dgvShopItems_RowEnter);
            dgvShopItems.DragEnter += new DragEventHandler(dgvShopItems_DragEnter);
            dgvShopItems.DragDrop += new DragEventHandler(dgvShopItems_DragDrop);
            dgvShopItems.UserDeletingRow += new DataGridViewRowCancelEventHandler(dgvShopItems_UserDeletingRow);
            dgvShopItems.CellEndEdit += new DataGridViewCellEventHandler(dgvShopItems_CellEndEdit);
            dgvItems.RowEnter += new DataGridViewCellEventHandler(dgvItems_RowEnter);
            dgvItems.MouseDown += new MouseEventHandler(dgvItems_MouseDown);
            dgvItems.RowHeaderMouseDoubleClick += new DataGridViewCellMouseEventHandler(dgvItems_RowHeaderMouseDoubleClick);
            tvItemCategory.AfterSelect += new TreeViewEventHandler(tvItemCategory_AfterSelect);
        }

        void tvShopList_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                _doRemoveShopList();
            }

        }

        private void _doRemoveShopList()
        {
            bool bRet = false;
            DialogResult dr = MessageBox.Show("确认删除选择的商店吗?", "警告!", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
            if (dr == DialogResult.Yes)
            {
                if ((int)tvShopList.SelectedNode.Tag == -1) //如果是新创建的商店,则直接删除
                {
                    this.txtShopName.Text = string.Empty;
                    this.txtShopConfigFile.Text = string.Empty;
                    this.tvShopList.SelectedNode.Remove();
                }
                else
                {
                    ShopItemLogic.DeleteShopItemsByShopTemplateID(Int32.Parse(this.lblShopID.Text));
                    bRet = ShopListLogic.DeleteShopList((int)tvShopList.SelectedNode.Tag);
                    if (bRet)
                    {
                        this.tvShopList.SelectedNode.Remove();
                    }
                    else
                    {
                        dr = MessageBox.Show("删除操作失败,原因可能是数据库操作出错.", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        void dgvItems_RowHeaderMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (e.RowIndex >= 0)
                {
                    DataRowView view = (DataRowView)dgvItems.Rows[e.RowIndex].DataBoundItem;
                    if (null != view)
                    {
                        dgvItems.DoDragDrop(view, DragDropEffects.Copy);
                        this.dgvShopItems.Select();
                        _doDrop(view);
                    }
                }
            }
        }


        void tvShopList_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if ((int)e.Node.Tag == App.AppVariables.INVALID_TREENODE) return;//如果是根节点,则退出
            if (e.Label == null) return;
            m_isDirty = true;
            this.txtShopName.Text = e.Label;
        }

        void tvShopList_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            _notifySave(true);
        }

        void dgvShopItems_UserDeletingRow(object sender, DataGridViewRowCancelEventArgs e)
        {
            if (dgvShopItems.Rows.Count <= 0) return;
            foreach (ShopItem shop in m_shopitems)
            {
                if (shop.ItemIndex.ToString() == e.Row.Cells["物件标识"].Value.ToString())
                {
                    DialogResult dr = MessageBox.Show("该物件将被从商店数据库中删除,确认删除吗?", "警告!", MessageBoxButtons.OKCancel,MessageBoxIcon.Question);
                    if (dr == DialogResult.OK)
                    {
                        m_shopitems.Remove(shop);
                        App.AppVariables.g_dataprovider.DeleteShopItem(shop.ShopItemID);
                    }
                    else
                    {
                        e.Cancel = true;
                    }
                    break;
                }
            }
        }

        void dgvShopItems_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            ///非新记录
            if (dgvShopItems.Rows[e.RowIndex].Cells["物件标识"].Value != null)
            {
                ///值不能为空
                if (dgvShopItems.Rows[e.RowIndex].Cells[e.ColumnIndex].Value == null)
                {
                    dgvShopItems.CancelEdit();
                }
                ShopItem shop = _buildShopItemFromView(e.RowIndex);
                m_shopitems.UpdateItem(shop);
                m_isDirty = true;
            }
        }
        void dgvShopItems_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Copy;
        }

        void dgvItems_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                DataGridView.HitTestInfo info = dgvItems.HitTest(e.X, e.Y);
                if (info.RowIndex >= 0)
                {
                    DataRowView view = (DataRowView)dgvItems.Rows[info.RowIndex].DataBoundItem;
                    if (null != view)
                    {
                        dgvItems.DoDragDrop(view, DragDropEffects.Copy);
                    }
                }
            }
        }
       
        void dgvShopItems_DragDrop(object sender, DragEventArgs e)
        {
            if(e.Data.GetDataPresent(typeof(DataRowView)))
            {
                DataRowView view = (DataRowView)e.Data.GetData(typeof(DataRowView));
                _doDrop(view);
            }

        }
        void tvItemCategory_AfterSelect(object sender, TreeViewEventArgs e)
        {
            ItemCollection items = null;
            if (null == tvItemCategory.SelectedNode) return;
            if (e.Action == TreeViewAction.ByMouse)
            {
                int equipid = -1;
                equipid = (int)e.Node.Tag;
                if (equipid > -1)//only valid node can be process.
                {
                    Equipment equip = EquipmentLogic.GetOneEquipment(equipid);
                    items = ItemLogic.GetItemsByCategory(equip.Gener, equip.SubType, equip.DetailType);
                    _initItems(ref items);
                }
                else if (equipid == -1)
                {
                    items = ItemLogic.GetAllItems();
                    _initItems(ref items);
                }
            }
        }

        void dgvItems_RowEnter(object sender, DataGridViewCellEventArgs e)
        {
            int selectedVal = -1;
            object tmpval = null;

            if (null == dgvItems["物件标识", e.RowIndex].Value ||
                dgvShopItems.Rows.Count <= 0)
            {
                return;
            }
            selectedVal = (int)dgvItems["物件标识", e.RowIndex].Value;
            foreach (DataGridViewRow dgvr in this.dgvShopItems.Rows)
            {
                tmpval = dgvr.Cells["物件标识"].Value ;
                if (null != tmpval)
                {
                    if ((int)tmpval == selectedVal)
                    {
                        dgvr.Selected = true;
                        try
                        {
                            dgvShopItems.CurrentCell = dgvr.Cells[0];
                        }
                        catch { }
                        break;
                    }
                }
            }
        }

        void dgvShopItems_RowEnter(object sender, DataGridViewCellEventArgs e)
        {
            int selectedVal = -1;
            object tmpval = null;
            //如果Items数据表不可见,则直接返回.
            //如果"物件标识"字段值为null,则直接返回.
            if (this.splitCTopBottom.Visible == false || 
                dgvShopItems.Rows.Count <= 0 ||
                null == dgvShopItems["物件标识",e.RowIndex].Value)
            {
                return;
            }
            selectedVal = (int)dgvShopItems["物件标识",e.RowIndex].Value;
            foreach (DataGridViewRow dgvr in this.dgvItems.Rows)
            {
                tmpval = dgvr.Cells["物件标识"].Value ;
                if (null != tmpval)
                {
                    if ((int)tmpval == selectedVal)
                    {
                        dgvr.Selected = true;
                        this.dgvItems.CurrentCell = dgvr.Cells[0];
                        break;
                    }
                }
            }
        }

        void tvShopList_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                this.contextMenuStrip_Treeview.Show(this.tvShopList, e.Location, ToolStripDropDownDirection.BelowRight);
            }
        }

        void tvShopList_AfterSelect(object sender, TreeViewEventArgs e)
        {
            int shoplistid = (int)e.Node.Tag;
            if (shoplistid == App.AppVariables.INVALID_TREENODE) 
                return;//如果是根节点,则退出
            if (shoplistid < 0)
            {
                //清理控件值
                dgvShopItems.Rows.Clear();
                txtShopConfigFile.Text = "";
                txtShopName.Text = e.Node.Text;
                lblShopID.Text = "";
            }
            ShopList shop = ShopListLogic.GetOneShopList(shoplistid);
            if (null != shop)
            {
                this.txtShopConfigFile.Text = shop.ConfigFile;
                this.chkShopCanFix.Checked = shop.CanRepair;
                this.txtShopName.Text = shop.ShopName;
                lblShopID.Text = shop.ShopTemplateID.ToString();
                _initShopItems(shop.ShopTemplateID, true);
            }
        }
        

        private void 环境设置CToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frmSettings frmsettings = new frmSettings();
            frmsettings.ShowDialog();
        }

        private void 使用说明NToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frmPreview frmprev = new frmPreview();
            frmprev.Show();
        }

        private void btnHideDGVItems_Click(object sender, EventArgs e)
        {
            this.splitCTopBottom.Panel2Collapsed = !this.splitCTopBottom.Panel2Collapsed;
        }

        private void tsbSave_Click(object sender, EventArgs e)
        {
            _notifySave(false);
        }
        /// <summary>
        /// 保存商店列表的设置
        /// </summary>
        private void _saveShopListSettings()
        {
            if (null == m_shopitems || m_shopitems.Count <= 0)
                return;
            int shopid = -1;
            Int32.TryParse(lblShopID.Text, out shopid);
            if (shopid >= 0)
            {
                ShopList shoplist = ShopListLogic.GetOneShopList(shopid);
                if (null != shoplist)
                {
                    shoplist.ShopName = this.tvShopList.SelectedNode.Text;
                    shoplist.CanRepair = this.chkShopCanFix.Checked;
                    shoplist.ConfigFile = this.txtShopConfigFile.Text;
                    ShopListLogic.UpdateShopList(shoplist);
                }
            }
        }
        /// <summary>
        /// 保存商店售卖的物件列表
        /// </summary>
        private void _saveShopItemList()
        {
            foreach (ShopItem shop in m_shopitems)
            {
                if (false == App.AppVariables.g_dataprovider.IsItemExistsInShop(shop.ItemIndex, shop.ShopTemplateID))
                {
                    App.AppVariables.g_dataprovider.AddShopItem(shop);
                }
                else
                {
                    App.AppVariables.g_dataprovider.UpdateShopItem(shop);
                }
            }
        }
        /// <summary>
        /// 保存商店列表(全部情况判断)
        /// </summary>
        private void _saveShopSettings()
        {
            if ((int)this.tvShopList.SelectedNode.Tag < 0)
            {
                if (this.txtShopName.Text == "" || this.txtShopConfigFile.Text == "")
                {
                    MessageBox.Show("不能保存未完全设置完成的商店!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                _saveNewShopList();
            }
            else
            {
                _saveShopListSettings();
            }
            _saveShopItemList();
            MessageBox.Show("保存成功!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        /// <summary>
        /// 保存新商店列表
        /// </summary>
        private void _saveNewShopList()
        {
            if (txtShopName.Text != "" &&
                txtShopConfigFile.Text != "")
            {
                ShopList shoplist = new ShopList();
                shoplist.ShopName = this.tvShopList.SelectedNode.Text;
                shoplist.CanRepair = this.chkShopCanFix.Checked;
                shoplist.ConfigFile = this.txtShopConfigFile.Text;
                ShopList retobj = ShopListLogic.AddShopList(shoplist);
                if (null != retobj)
                {
                    MessageBox.Show("新商店创建成功!", "提示", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
                _initShopList();
            }
        }
        /// <summary>
        /// 将显示的数据记录转换为对象
        /// </summary>
        /// <param name="rowidx"></param>
        /// <returns></returns>
        private ShopItem _buildShopItemFromView(int rowidx)
        {
            ShopItem shop = new ShopItem();
            shop.ShopItemID = Convert.ToInt32(dgvShopItems.Rows[rowidx].Cells["ShopItemID"].Value);
            shop.CountLimit = dgvShopItems.Rows[rowidx].Cells["最大数目"].Value.ToString();
            shop.ItemIndex = Convert.ToInt32(dgvShopItems.Rows[rowidx].Cells["物件标识"].Value);
            shop.Price = Convert.ToInt32(dgvShopItems.Rows[rowidx].Cells["价格"].Value);
            shop.RandSeed = dgvShopItems.Rows[rowidx].Cells["随机种子"].Value.ToString();
            shop.ReputeLevel = dgvShopItems.Rows[rowidx].Cells["声望等级"].Value.ToString();
            shop.ShopTemplateID = Convert.ToInt32(dgvShopItems.Rows[rowidx].Cells["商店模板标识"].Value);
            shop.TabType = dgvShopItems.Rows[rowidx].Cells["物件类型"].Value.ToString();
            shop.SortIndex = Convert.ToInt32(dgvShopItems.Rows[rowidx].Cells["排列序号"].Value);
            return shop;
        }

        private void 退出编辑器XToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _notifySave(true);
            Application.Exit();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            _exportTabFile();
        }

        /// <summary>
        /// 提示保存修改
        /// </summary>
        private void _notifySave(bool notify)
        {
            if (m_isDirty == true)
            {
                if (notify)
                {
                    DialogResult dr = MessageBox.Show("你已对商店列表进行了修改,是否保存?", "提示!", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    if (dr == DialogResult.Yes)
                    {
                        goto DoSave;
                    }
                    else if (dr == DialogResult.No)
                    {
                        m_isDirty = false;
                    }
                }
                else
                {
                    DialogResult dr = MessageBox.Show("请确认是否保存?", "提示!", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    if (dr == DialogResult.Yes)
                    {
                        goto DoSave;
                    }
                    else if (dr == DialogResult.No)
                    {
                        m_isDirty = false;
                    }
                }
            }
            return;
            DoSave:
                this._saveShopSettings();
                m_isDirty = false;
        }

        private void 导出TAB文件EToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _exportTabFile();
        }

        private void _exportTabFile()
        {
            string filename = App.AppVariables.OutputDir + this.txtShopConfigFile.Text;
            if(filename == "" || null == m_shopitems)
            {
                MessageBox.Show("文件名为空或没有选择商店,导出中止!", "导出", MessageBoxButtons.OK, MessageBoxIcon.Stop);
            }
            else
            {
                Export.ExportShopToTabFile.Export(filename, ref m_shopitems);
                MessageBox.Show("导出文件成功,文件为:" +filename, "导出", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void 修改名称RToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.tvShopList.SelectedNode.BeginEdit();
        }

        private void _doDrop(DataRowView view)
        {
            if (null == m_shopitems) return;
            //不能加入相同的记录
            foreach (ShopItem item in m_shopitems)
            {
                if (item.ItemIndex == (int)view["物件标识"])
                {
                    return;
                }
            }

            ShopItem shop = new ShopItem();
            shop.ShopItemID = -1;
            shop.CountLimit = "-1";
            shop.ItemIndex = (int)view["物件标识"];
            shop.Price = 0;
            shop.RandSeed = "1";
            shop.RECID = -1;
            shop.ReputeLevel = "1";
            shop.ShopTemplateID = Int32.Parse(lblShopID.Text);
            shop.SortIndex = dgvShopItems.Rows.Count;//排序号
            if (null != m_itemcategories && m_itemcategories.Count > 0)
            {
                ItemCategory itemcategory = (ItemCategory)m_itemcategories[0];
                shop.TabType = itemcategory.CategoryName;
            }
            m_shopitems.Add(shop);
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells[2].Selected = true;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells[1].Value = lblShopID.Text;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["物件标识"].Value = view["物件标识"];
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["物件名称"].Value = view["物件名称"];
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["随机种子"].Value = shop.RandSeed;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["最大数目"].Value = shop.CountLimit;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["声望等级"].Value = shop.ReputeLevel;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["价格"].Value = shop.Price;
            dgvShopItems.Rows[dgvShopItems.Rows.Count - 1].Cells["排列序号"].Value = shop.SortIndex;

            this._initShopItems(Int32.Parse(lblShopID.Text), false);
            try
            {
                dgvShopItems.Rows[dgvShopItems.Rows.Count - 2].Cells[2].Selected = true;
            }
            catch { }
            m_isDirty = true;//设置全局修改标志位
        }

        private void 新建商店NToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool bFound = false;
            foreach (TreeNode node in tvShopList.Nodes[0].Nodes)
            {
                if ((int)node.Tag == -1)
                {
                    bFound = true;
                    break;
                }
            }
            if (bFound == false)
            {
                TreeNode newnode = new TreeNode();
                newnode.Text = "新商店";
                newnode.Tag = (int)-1;
                tvShopList.Nodes[0].Nodes.Add(newnode);
            }
            else
            {
                MessageBox.Show("请先完成新创建的商店的设置!", "提示",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void 删除商店DToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _doRemoveShopList();
        }
    }
}