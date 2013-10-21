using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace HelperEditor
{
    public partial class ConditionDefineEditor : Form
    {
        public ConditionDefineEditor()
        {
            InitializeComponent();
        }

        private void ConditionDefineEditor_Load(object sender, EventArgs e)
        {
            //tabControl1.SelectedIndex = 1;
        }

        private DataGridViewRow lastSelectedRow;

        private void dataGridView3_MouseClick(object sender, MouseEventArgs e)
        {
            EventSelector formEventSelector = new EventSelector();

            // 首先通过当前树的内容, 修改事件选择器的选定内容
            formEventSelector.SetListAllItemChecked(false);
            foreach (DataGridViewRow row in dataGridView3.Rows)
            {
                if (row != null && row.Cells[0].Value != null)
                {
                    formEventSelector.SetListItemChecked(row.Cells[0].Value.ToString(), true);
                }
            }

            if (formEventSelector.ShowDialog() == DialogResult.OK)
            {
                string[] EventKeyList = formEventSelector.GetSelectedEventList();
                dataGridView3.Rows.Clear();
                foreach (string szKey in EventKeyList)
                {
                    dataGridView3.Rows.Add(szKey, TabFile.EventData.GetTabCell(szKey, "ShowName"));
                }
            }
        }

        private void dataGridView4_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            DataGridView triggeredGrid = (DataGridView)sender;
            // 必须修改了名字才有效
            if (triggeredGrid.Rows[e.RowIndex].Cells[0].Value == null && triggeredGrid.Rows[e.RowIndex].Cells[2].Value != null)
            {
                string[, ] tContent = TabFile.ConditionData.GetTabContent();
                for (int i = 1; i < tContent.GetLength(0); i++)
                {
                    if (tContent[i, 0] == "")
                    {
                        triggeredGrid.Rows[e.RowIndex].Cells[0].Value = i.ToString();
                        TabFile.ConditionData.SetTabCell(i.ToString(), "ConditionID", i.ToString());
                        TabFile.ConditionData.SetTabCell(i.ToString(), "ShowName", triggeredGrid.Rows[e.RowIndex].Cells[2].Value.ToString());
                        break;
                    }
                }
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (dataGridView4.SelectedRows.Count > 0)
                SaveTabData(dataGridView4.SelectedRows[0]);
            TabFile.ConditionData.ExportSettingsFile(Directory.GetCurrentDirectory() + @"\Settings\ConditionDefine.tab");
        }

        // ************ 方法 ************
        public void ChangeSelectedRow(string szKey)
        {
            foreach (DataGridViewRow row in dataGridView4.Rows)
                if (row != null && row.Cells[0].Value != null && row.Cells[0].Value.ToString() == szKey)
                {
                    row.Selected = true;
                }
        }

        public void ChangeSelectedTab(int nIndex)
        {
            tabControl1.SelectedIndex = nIndex;

            richTextBox1.Text = "";
            dataGridView4.Rows.Clear();
            dataGridView3.Rows.Clear();
            dataGridView2.Rows.Clear();
            dataGridView1.Rows.Clear();

            string[,] tConditionTable = TabFile.ConditionData.GetTabContent();
            for (int i = 0; i < tConditionTable.GetLength(0); i++)
            {
                string[] tCommonData = new string[] { 
                    TabFile.ConditionData.GetTabCell(i, "ConditionID"),
                    TabFile.ConditionData.GetTabCell(i, "Class"),
                    TabFile.ConditionData.GetTabCell(i, "ShowName")
                };

                if (tCommonData[0] != "")
                    dataGridView4.Rows.Add(tCommonData);
            }
        }

        public void SaveTabData(DataGridViewRow lastSelectedRow)
        {
            string szLastMainKey = lastSelectedRow.Cells[0].Value.ToString();
            // 保存普通信息
            if (lastSelectedRow.Cells[1].Value == null)
                TabFile.ConditionData.SetTabCell(szLastMainKey, "Class", "");
            else
                TabFile.ConditionData.SetTabCell(szLastMainKey, "Class", lastSelectedRow.Cells[1].Value.ToString());

            if (lastSelectedRow.Cells[2].Value == null)
                TabFile.ConditionData.SetTabCell(szLastMainKey, "ShowName", "");
            else
                TabFile.ConditionData.SetTabCell(szLastMainKey, "ShowName", lastSelectedRow.Cells[2].Value.ToString());

            TabFile.ConditionData.SetTabCell(szLastMainKey, "CompareSource", richTextBox1.Text.ToString());
            // 保存允许事件组
            string szAllowableEvent = "";
            foreach (DataGridViewRow row in dataGridView3.Rows)
                if (row != null && row.Cells[0].Value != null)
                {
                    if (szAllowableEvent == "")
                    {
                        szAllowableEvent += row.Cells[0].Value.ToString();
                    }
                    else
                    {
                        szAllowableEvent += "," + row.Cells[0].Value.ToString();
                    }
                }
            TabFile.ConditionData.SetTabCell(szLastMainKey, "AllowableEvent", szAllowableEvent);
            // 保存逻辑运算符
            string szCompareOperator = "";
            foreach (DataGridViewRow row in dataGridView2.Rows)
                if (row != null && row.Cells[0].Value != null)
                {
                    string szRealValue = "";
                    if (row.Cells[1].Value != null)
                        szRealValue = row.Cells[1].Value.ToString();

                    if (szCompareOperator == "")
                        szCompareOperator += row.Cells[0].Value.ToString() + ";" + szRealValue;
                    else
                        szCompareOperator += "," + row.Cells[0].Value.ToString() + ";" + szRealValue;
                }
            TabFile.ConditionData.SetTabCell(szLastMainKey, "CompareOperator", szCompareOperator);
            // 保存比较列表
            string szTargetOperator = "";
            foreach (DataGridViewRow row in dataGridView1.Rows)
                if (row != null && row.Cells[0].Value != null)
                {
                    string szRealValue = "";
                    if (row.Cells[1].Value != null)
                        szRealValue = row.Cells[1].Value.ToString();

                    if (szTargetOperator == "")
                        szTargetOperator += row.Cells[0].Value.ToString() + ";" + szRealValue;
                    else
                        szTargetOperator += "," + row.Cells[0].Value.ToString() + ";" + szRealValue;
                }
            TabFile.ConditionData.SetTabCell(szLastMainKey, "CompareTarget", szTargetOperator);
        }

        private void dataGridView4_SelectionChanged(object sender, EventArgs e)
        {

        }
    }
}
