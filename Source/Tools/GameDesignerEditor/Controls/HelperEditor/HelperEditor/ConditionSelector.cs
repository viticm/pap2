using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace HelperEditor
{
    public partial class ConditionSelector : Form
    {
        public ConditionSelector()
        {
            InitializeComponent();

            // 下拉列表格式
            comboBox1.DisplayMember = "key";
            comboBox1.ValueMember = "value";
            comboBox2.DisplayMember = "key";
            comboBox2.ValueMember = "value";
            comboBox3.DisplayMember = "key";
            comboBox3.ValueMember = "value";
        }

        private void ConditionSelector_Load(object sender, EventArgs e)
        {
            comboBox3.Items.Clear();
            TreeNode SourceNode = (TreeNode)this.Tag;
            // 下面通过判断 SourceNode 的 Tag 来判断是新建还是修改
            bool bIsNewNode = SourceNode.Tag == null;
            string szEventKey = "";

            // 这里计算出当前选中的节点的事件 ID
            if (SourceNode != null)
            {
                Form1 formSource = (Form1)SourceNode.TreeView.FindForm();
                TreeNode selectedEventNode = formSource.GetSelectedEventNode();
                if (selectedEventNode.Level == 1)
                    szEventKey = selectedEventNode.Parent.Name;
            }

            // 初始化条件下拉列表, 根据不同事件决定可用的判断条件
            string[,] tTabContent = TabFile.ConditionData.GetTabContent();
            for (int i = 1; i < TabFile.ConditionData.RecordLengthMax; i++)
            {
                int nMainKeyValue = TabFile.ConditionData.GetColIndexByFieldName(TabFile.ConditionData.MainKeyName);
                if (tTabContent[i, nMainKeyValue] != "")
                {
                    string szAllowableEventList = TabFile.ConditionData.GetTabCell(i, "AllowableEvent");
                    bool bAllow = true;
                    if (szAllowableEventList != "")
                    {
                        bAllow = false;
                        foreach (string szAllowableEventID in szAllowableEventList.Split(','))
                        {
                            if (szAllowableEventID == szEventKey)
                            {
                                bAllow = true;
                                break;
                            }
                        }
                    }
                    if (bAllow == true)
                    {
                        string szShowName = TabFile.ConditionData.GetTabCell(i, "Class") + " - " + TabFile.ConditionData.GetTabCell(i, "ShowName");
                        DorpDownListItem dorpDownListItem = new DorpDownListItem(szShowName, tTabContent[i, nMainKeyValue], 0);
                        dorpDownListItem.Index = comboBox3.Items.Add(dorpDownListItem);
                    }
                }
            }

            comboBox3.SelectedIndex = 0;
            comboBox1.SelectedIndex = -1;
            comboBox2.SelectedIndex = -1;

            if (bIsNewNode)
            {
                // 这个会触发事件初始化下面两个 COMBOBOX
                comboBox1.SelectedIndex = 0;
                comboBox2.SelectedIndex = 0;
            }
            else
            {
                // 通过已经记录的信息进行初始化
                ConditionNodeTagData conditionNodeTag = (ConditionNodeTagData)SourceNode.Tag;
                // 下面的循环用来保证即使 ComboBox 3 的内容变化了, 也能正确定位选中事件
                foreach (Object comboItem in comboBox3.Items)
                {
                    DorpDownListItem dorpDownListItem = (DorpDownListItem)comboItem;
                    if (dorpDownListItem.Value == conditionNodeTag.ConditionID)
                    {
                        // 这个会触发事件初始化下面两个 COMBOBOX
                        comboBox3.SelectedIndex = conditionNodeTag.CompareCondition.Index;
                        break;
                    }
                }
                label1.Text = conditionNodeTag.ShownString;
                // 做简单检查避免表格改动导致宕掉
                if (comboBox1.Items.Count > conditionNodeTag.CompareOperator.Index)
                    comboBox1.SelectedIndex = conditionNodeTag.CompareOperator.Index;
                else
                    comboBox1.SelectedIndex = 0;

                if (comboBox2.Items.Count > conditionNodeTag.CompareTarget.Index)
                    comboBox2.SelectedIndex = conditionNodeTag.CompareTarget.Index;
                else
                    comboBox2.SelectedIndex = 0;
            }
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox3.SelectedItem == null)
                return;
            string szTabKey = ((DorpDownListItem)comboBox3.SelectedItem).Value;

            // 写文本提示文本
            label1.Text = comboBox3.Text;

            // 生成比较运算符列表
            string szCompareOperator = TabFile.ConditionData.GetTabCell(szTabKey, "CompareOperator");
            //szCompareOperator = szCompareOperator.Replace("\"", "");
            comboBox1.Items.Clear();
            foreach (string szListItem in szCompareOperator.Split(','))
            {
                DorpDownListItem dorpDownListItem = new DorpDownListItem(szListItem.Split(';'), 0);
                dorpDownListItem.Index = comboBox1.Items.Add(dorpDownListItem);     // 记录下 Index
            }

            // 生成被比较下拉列表
            string szCompareTarget = TabFile.ConditionData.GetTabCell(szTabKey, "CompareTarget");
            //szCompareTarget = szCompareTarget.Replace("\"", "");
            comboBox2.Items.Clear();
            foreach (string szListItem in szCompareTarget.Split(','))
            {
                DorpDownListItem dorpDownListItem = new DorpDownListItem(szListItem.Split(';'), 0);
                dorpDownListItem.Index = comboBox2.Items.Add(dorpDownListItem);     // 记录下 Index
            }

            comboBox1.SelectedIndex = -1;
            comboBox2.SelectedIndex = -1;

            TreeNode SourceNode = (TreeNode)this.Tag;
            // 下面通过判断 SourceNode 的 Tag 来判断是新建还是修改
            ConditionNodeTagData conditionNodeTag = (ConditionNodeTagData)SourceNode.Tag;
            if (conditionNodeTag != null && conditionNodeTag.ConditionID == szTabKey)
            {
                comboBox1.SelectedIndex = conditionNodeTag.CompareOperator.Index;
                comboBox2.SelectedIndex = conditionNodeTag.CompareTarget.Index;
            }
            else
            {
                comboBox1.SelectedIndex = 0;
                comboBox2.SelectedIndex = 0;
            }

            // 自动排版
            int nStartLocMax = label1.Width;
            if (nStartLocMax > 420)             // 确保不会让排版超过窗体宽度
                nStartLocMax = 420;
            comboBox1.Location = new Point(label1.Location.X + nStartLocMax + 4, comboBox1.Location.Y);
            comboBox2.Location = new Point(comboBox1.Location.X + comboBox1.Width + 4, comboBox2.Location.Y);
            label2.Location = new Point(comboBox2.Location.X + comboBox2.Width + 4, label2.Location.Y);
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox3.SelectedItem == null || comboBox2.SelectedItem == null || comboBox1.SelectedItem == null)
                return;

            string szTabKey = ((DorpDownListItem)comboBox3.SelectedItem).Value;
            // 生成代码文本
            if (((DorpDownListItem)comboBox1.SelectedItem).Value != "()")
                richTextBox2.Text = "if " + TabFile.ConditionData.GetTabCell(szTabKey, "CompareSource") + " " +
                        ((DorpDownListItem)comboBox1.SelectedItem).Value + " " +
                        ((DorpDownListItem)comboBox2.SelectedItem).Value + " then";
            else
                richTextBox2.Text = "if " + string.Format(TabFile.ConditionData.GetTabCell(szTabKey, "CompareSource"),
                    ((DorpDownListItem)comboBox2.SelectedItem).Value) + " then";
            // 生成预览文本
            richTextBox1.Text = label1.Text + " " + comboBox1.Text + " " + comboBox2.Text + "。";
        }

        // *************** 方法 ***************
        /// <summary>
        /// 用来获取已经拼接好的显示用的字符串。
        /// </summary>
        /// <returns>显示为树节点内容的字符串。</returns>
        public string GetConditionString()
        {
            return richTextBox1.Text;
        }

        public ConditionNodeTagData GetConditionNodeTag()
        {
            string szTabKey = ((DorpDownListItem)comboBox3.SelectedItem).Value;
            string szShownString = label1.Text;
            string szCode = TabFile.ConditionData.GetTabCell(szTabKey, "CompareSource");
            DorpDownListItem operatorItem = (DorpDownListItem)comboBox1.SelectedItem;
            DorpDownListItem targetItem = (DorpDownListItem)comboBox2.SelectedItem;
            DorpDownListItem conditionItem = (DorpDownListItem)comboBox3.SelectedItem;
            ConditionNodeTagData conditionNodeTag = new ConditionNodeTagData(szTabKey, szShownString, szCode, operatorItem, targetItem, conditionItem);
            return conditionNodeTag;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ConditionDefineEditor conditionDefineEditor = new ConditionDefineEditor();
            conditionDefineEditor.ChangeSelectedTab(1);

            // 选中当前相关的行
            DorpDownListItem dorpDownListItem = (DorpDownListItem)comboBox3.SelectedItem;
            conditionDefineEditor.ChangeSelectedRow(dorpDownListItem.Value);

            if (conditionDefineEditor.ShowDialog() == DialogResult.OK)      // 这里刷新面板以出现新的内容
            {
                // TODO: 考虑是否自动选中刚配置的项或者刷新后也回复刷新前的选择状态(下面的三行代码会强制保存, 不满足条件)
                //TreeNode SourceNode = (TreeNode)this.Tag;
                //Form1 form1 = (Form1)SourceNode.TreeView.FindForm();
                //form1.SaveConditionPageData();
                ConditionSelector_Load(sender, e);
            }

            conditionDefineEditor.Dispose();
        }
    }
}
