using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using GameDesingerTools;

namespace CustomLoadTree
{
    public partial class FindFrm : Form
    {
        private TreeView Tree;
        private Form1 Frm;
        private ArrayList AllFoundNodes;
        private RadioButton OldCheckButton = null;

        public FindFrm(Form1 frm)
        {
            InitializeComponent();
            this.radioID.Checked = true;
            Tree = frm.theTree;
            Frm = frm;
            AllFoundNodes = new ArrayList();
            this.ActiveControl = this.textBox;
        }

        private void textBox_KeyDown(object sender, KeyEventArgs e)
        {
            if ((sender as TextBox == this.textBox || sender as DevComponents.DotNetBar.Controls.TextBoxX == this.textBoxX1) && e.KeyCode == Keys.Enter)
            {
                buttonFind_Click(this.buttonFind, null);
            }
            else if (sender as DevComponents.DotNetBar.Controls.TextBoxX == this.textBoxX1 && e.KeyCode == Keys.F4)
            {
                buttonX1_Click(this.buttonX1, null);
            }
            else if (sender as DevComponents.DotNetBar.Controls.TextBoxX == this.textBoxX1 && e.KeyCode == Keys.Escape)
            {
                this.Dispose();
            }
        }

        private void FindQuest() // 查找任务
        {
            string whatToFind = this.textBox.Text;

            if (whatToFind == string.Empty)
            {
                MessageBox.Show("您还什么都没填。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (this.radioID.Checked) // 找ID
            {
                int id = -1;
                try
                {
                    id = Convert.ToInt32(whatToFind);
                }
                catch
                {
                    id = -1;
                    MessageBox.Show("你输入的并不是正确的ID。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                if (AllFoundNodes.Count == 0)
                    AllFoundNodes = Helper.FindAllActualNodeByID(Tree, id);
                if (AllFoundNodes.Count == 0)
                {
                    MessageBox.Show("没有找到符合条件的任务。", "结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
                int new_selected_index = AllFoundNodes.IndexOf(Tree.SelectedNode) + 1;
                if (new_selected_index >= AllFoundNodes.Count)
                    new_selected_index = 0;

                Tree.SelectedNode = (TreeNode)(AllFoundNodes[new_selected_index]);
            }
            else if (this.radioName.Checked) // 找name
            {
                if (AllFoundNodes.Count == 0)
                    AllFoundNodes = Helper.FindAllActualNodeByName(Tree, whatToFind);
                if (AllFoundNodes.Count == 0)
                {
                    MessageBox.Show("没有找到符合条件的任务。", "结果", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
                int new_selected_index = AllFoundNodes.IndexOf(Tree.SelectedNode) + 1;
                if (new_selected_index >= AllFoundNodes.Count)
                    new_selected_index = 0;

                Tree.SelectedNode = (TreeNode)(AllFoundNodes[new_selected_index]);
            }

            Tree.Focus();
        }

        private void FindRef() // 反向查找引用
        {
            int findID;
            try
            {
                string whatToFind = this.textBoxX1.Text; // [1] npc1
                int iStart = whatToFind.IndexOf('[');
                int iEnd = whatToFind.LastIndexOf(']');
                whatToFind = whatToFind.Substring(iStart + 1, iEnd - iStart - 1);
                findID = Convert.ToInt32(whatToFind);
            }
            catch
            {
                MessageBox.Show("输入不正确。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string npc_cols = @"StartNpcTemplateID,
                                EndNpcTemplateID,
                                DropNpcTemplateID1,
                                DropNpcTemplateID2,
                                DropNpcTemplateID3,
                                DropNpcTemplateID4,
                                KillNpcTemplateID1,
                                KillNpcTemplateID2,
                                KillNpcTemplateID3,
                                KillNpcTemplateID4";

            string doodad_cols = @"StartDoodadTemplateID,
                                EndDoodadTemplateID,
                                DropDoodadTemplateID1,
                                DropDoodadTemplateID2,
                                DropDoodadTemplateID3,
                                DropDoodadTemplateID4,
                                RelationDoodadID1,
                                RelationDoodadID2,
                                RelationDoodadID3,
                                RelationDoodadID4,
                                RelationDoodadID5,
                                RelationDoodadID6,
                                RelationDoodadID7,
                                RelationDoodadID8";

            string item_cols = @"StartItemIndex,
                                StartRequireItemIndex1,
                                StartRequireItemIndex2,
                                StartRequireItemIndex3,
                                StartRequireItemIndex4,
                                OfferItemIndex1,
                                OfferItemIndex2,
                                OfferItemIndex3,
                                OfferItemIndex4,
                                EndItemIndex,
                                EndRequireItemIndex1,
                                EndRequireItemIndex2,
                                EndRequireItemIndex3,
                                EndRequireItemIndex4,
                                PresentItemIndex1,
                                PresentItemIndex2,
                                PresentItemIndex3,
                                PresentItemIndex4,
                                PresentItemIndex5,
                                PresentItemIndex6,
                                PresentItemIndex7,
                                PresentItemIndex8";

            string strReplace = string.Empty;

            int nItemType = -1;
            if (this.radioNPC.Checked)
            {
                strReplace = npc_cols;
            }
            else if (this.radioDoodad.Checked)
            {
                strReplace = doodad_cols;
            }
            else if (this.radioItem.Checked || radioArmor.Checked || radioTrinket.Checked || radioWeapon.Checked)
            {
                strReplace = item_cols;
                nItemType = 5;
            }
            else if (radioWeapon.Checked)
            {
                    strReplace = item_cols;
                    nItemType = 6;
            }
            else if (radioArmor.Checked)
            {
                strReplace = item_cols;
                nItemType = 7;
            }
            else if (radioTrinket.Checked)
            {
                strReplace = item_cols;
                nItemType = 8;
            }
            strReplace = strReplace.Replace(" ", string.Empty);
            strReplace = strReplace.Replace("\t", string.Empty);
            strReplace = strReplace.Replace("\r\n", string.Empty);

            string[] cols = strReplace.Trim().Split(new char[] {','});
            string sql = "SELECT _Cat,QuestID,QuestName,";
            foreach (string col in cols)
            {
                sql += col;
                sql += ",";
            }
            sql = sql.Substring(0, sql.Length - 1);
            sql += " FROM tbl_quests WHERE ";
            
            foreach (string col in cols)
            {
                string strTypeCondition = string.Empty;
                if (nItemType == 5 || nItemType == 6 || nItemType == 7 || nItemType == 8)
                {
                    strTypeCondition = string.Format(" AND {0} = '{1}'", col.Replace("Index", "Type"), nItemType);
                }
                string strAdd = string.Format("({0}={1}{2}) OR ", col, findID, strTypeCondition);
                sql += strAdd;
            }
            sql = sql.Substring(0, sql.Length - 4);

            DataTable tbl = Helper.GetDataTable(sql, Helper.Conn);
            this.dataGridView1.DataSource = tbl;

            // 生成报表
            this.listBox1.Items.Clear();
            if (tbl.Rows.Count == 0)
            {
                this.listBox1.Items.Add("没有找到引用。");
                return;
            }
            foreach (DataRow row in tbl.Rows)
            {
                string report = string.Format("[{0}, {1}, {2}]\t", row["_Cat"], row["QuestID"], row["QuestName"]);
                foreach (string col in cols)
                {
                    if (row[col].ToString().Trim() == findID.ToString()) // 是该列
                    {
                        string __sql = string.Format("SELECT fieldcnname FROM sys_meta_info WHERE tablename = 'tbl_quests' AND fieldname = '{0}'", col);
                        string cnName = Helper.GetDataTable(__sql, Helper.Conn).Rows[0][0].ToString().Trim();
                        string strAdd = string.Format("{0}, ", cnName);
                        report += strAdd;
                    }
                }
                report = report.Substring(0, report.Length - 2);
                this.listBox1.Items.Add(report);
            }
        }

        // 定位到反向查找到的任务
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            buttonGotoQuest_Click(this.buttonGotoQuest, null);
        }
        private void buttonGotoQuest_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem == null)
            {
                MessageBox.Show("请先选择一行结果。");
                return;
            }
            string selectedRpt = this.listBox1.SelectedItem.ToString();
            int first = selectedRpt.IndexOf(",");
            selectedRpt = selectedRpt.Substring(first + 2);
            int second = selectedRpt.IndexOf(",");
            string strID = selectedRpt.Substring(0, second);
            this.radioID.Checked = true;
            this.textBox.Text = strID;
            buttonFind_Click(this.buttonFind, null);
            OldCheckButton.Checked = true;
        }


        private void buttonFind_Click(object sender, EventArgs e) // 查找
        {
            if (radioID.Checked || radioName.Checked)
            {
                FindQuest();
            }
            else if (radioDoodad.Checked || radioItem.Checked || radioNPC.Checked || radioArmor.Checked || radioTrinket.Checked || radioWeapon.Checked) 
            {
                FindRef();
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e) // 取消
        {
            AllFoundNodes.Clear();
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void textBox_TextChanged(object sender, EventArgs e)
        {
            if (AllFoundNodes != null)
                AllFoundNodes.Clear();
        }

        private void FindFrm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Frm.FindingForm = null;
        }

        private void radio_CheckedChanged(object sender, EventArgs e)
        {
            if (AllFoundNodes != null)
                AllFoundNodes.Clear();

            RadioButton radio = sender as RadioButton;
            if (radio == null)
                return;

            // 保证只有一个 radioButton 被选中
            if (radio.Checked)
            {
                if (this.radioDoodad != radio)
                    this.radioDoodad.Checked = false;
                if (this.radioID != radio)
                    this.radioID.Checked = false;
                if (this.radioName != radio)
                    this.radioName.Checked = false;
                if (this.radioItem != radio)
                    this.radioItem.Checked = false;
                if (this.radioNPC != radio)
                    this.radioNPC.Checked = false;
                if (this.radioTrinket != radio)
                    this.radioTrinket.Checked = false;
                if (this.radioWeapon != radio)
                    this.radioWeapon.Checked = false;
                if (this.radioArmor != radio)
                    this.radioArmor.Checked = false;
            }
            else
            {
                OldCheckButton = radio;
            }

            // 处理是否一直 topMost
            if (radio == radioID || radio == radioName)
            {
                this.TopMost = true;
                this.ActiveControl = this.textBox;
            }
            else if (radio == radioNPC || radio == radioDoodad || radio == radioItem || radio == radioArmor || radio == radioWeapon || radio == radioTrinket)
            {
                this.TopMost = false;
                this.ActiveControl = this.textBoxX1;
            }
        }

        private void buttonX1_Click(object sender, EventArgs e) // 输入查找的id
        {
            if (!this.radioDoodad.Checked && !this.radioItem.Checked && !this.radioNPC.Checked && !this.radioArmor.Checked && !this.radioTrinket.Checked && !this.radioWeapon.Checked)
            {
                MessageBox.Show("请先选择一种查找依据。");
                return;
            }

            string strCnName = string.Empty;
            if (this.radioNPC.Checked)
                strCnName = "NPC编辑器";
            else if (this.radioDoodad.Checked)
                strCnName = "Doodad编辑器";
            else if (this.radioItem.Checked)
                strCnName = "道具(Other.tab)";
            else if (this.radioArmor.Checked)
                strCnName = "装备-防具";
            else if (this.radioTrinket.Checked)
                strCnName = "装备-饰品";
            else if (this.radioWeapon.Checked)
                strCnName = "装备-武器";
            else
                return;
            this.WindowState = FormWindowState.Minimized;
            BaseDialog dlg = BaseForm.ShowModelInDialog_Static(strCnName);
            this.WindowState = FormWindowState.Normal;
            string strID = string.Empty;
            string strName = string.Empty;
            if (dlg == null)
                return;

            strID = dlg.GetPropertyValue("id").ToString();
            strName = dlg.GetPropertyValue("name").ToString();

            this.textBoxX1.Text = string.Format("[{0}] {1}", strID, strName);
            this.ActiveControl = this.textBoxX1;
        }

    }
}