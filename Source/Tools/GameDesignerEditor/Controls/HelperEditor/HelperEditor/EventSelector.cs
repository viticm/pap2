using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;


namespace HelperEditor
{
    public partial class EventSelector : Form
    {
        private CustomListView customListView1;

        public EventSelector()
        {
            InitializeComponent();

            customListView1 = new HelperEditor.CustomListView();
            this.Controls.Add(customListView1);

            // 
            // customListView1
            // 
            customListView1.CheckBoxes = true;
            customListView1.Dock = System.Windows.Forms.DockStyle.Top;
            customListView1.FullRowSelect = true;
            customListView1.GridLines = true;
            customListView1.Location = new System.Drawing.Point(0, 0);
            customListView1.MultiSelect = false;
            customListView1.Name = "customListView1";
            customListView1.Size = new System.Drawing.Size(784, 523);
            customListView1.TabIndex = 5;
            customListView1.UseCompatibleStateImageBehavior = false;
            customListView1.View = System.Windows.Forms.View.Details;
            customListView1.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.customListView1_ItemChecked);
            customListView1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.customListView1_MouseClick);

            // 先录入内容
            string[,] tTabContent = TabFile.EventData.GetTabContent();
            for (int i = 1; i < TabFile.EventData.RecordLengthMax; i++)
            {
                int nMainKeyValue = TabFile.EventData.GetColIndexByFieldName(TabFile.EventData.MainKeyName);
                if (tTabContent[i, nMainKeyValue] != "")
                {
                    ListViewItem listViewItem = new ListViewItem();
                    for (int j = 1; j < TabFile.EventData.FieldLength; j++)
                    {
                        listViewItem.SubItems.Add(tTabContent[i, j]);
                    }

                    listViewItem.Text = tTabContent[i, nMainKeyValue];
                    listViewItem.Name = tTabContent[i, nMainKeyValue];
                    customListView1.Items.Add((listViewItem));
                }
            }

            // 再录入表头以及调整宽度(自适应)
            foreach (string szFieldName in TabFile.EventData.GetTabHeader())
            {
                ColumnHeader headerItem = customListView1.Columns.Add(szFieldName, szFieldName);
                headerItem.Width = -1;
            }
        }

        private void EventSelector_Load(object sender, EventArgs e)
        {
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Button ClickedButton = (Button)sender;
            if (ClickedButton.Text == "Select All")
            {
                ClickedButton.Text = "Deselect All";
                ClickedButton.ForeColor = Color.Red;
                foreach (ListViewItem Item in customListView1.Items)
                {
                    Item.Checked = true;
                }
            }
            else
            {
                ClickedButton.Text = "Select All";
                ClickedButton.ForeColor = Color.Black;
                foreach (ListViewItem Item in customListView1.Items)
                {
                    Item.Checked = false;
                }
            }
        }

        private void customListView1_MouseClick(object sender, MouseEventArgs e)
        {
            if (customListView1.SelectedItems.Count > 0 && checkBox1.Checked == true)
            {
                ListViewItem ClickedItem = customListView1.SelectedItems[0];
                if (e.Button == MouseButtons.Right)
                    ClickedItem.Checked = false;
                else
                    ClickedItem.Checked = true;
            }
            customListView1.SelectedItems.Clear();
        }

        private void customListView1_ItemChecked(object sender, ItemCheckedEventArgs e)
        {
            if (e.Item.Checked == true)
                e.Item.ForeColor = Color.Blue;
            else
                e.Item.ForeColor = Color.Black;
        }

        // *************** 方法 ***************
        /// <summary>
        /// 取得被选择的事件列表。
        /// </summary>
        /// <returns>被选择的事件列表数组。</returns>
        public string[] GetSelectedEventList()
        {
            List<string> EventList = new List<string>();
            foreach (ListViewItem Item in customListView1.Items)
            {
                if (Item.Checked == true)
                    EventList.Add(Item.Name);
            }
            return EventList.ToArray();
        }

        /// <summary>
        /// 设置 List 中所有记录的选中标记。
        /// </summary>
        /// <param name="bIsChecked">是否选中。</param>
        public void SetListAllItemChecked(bool bIsChecked)
        {
            foreach (ListViewItem Item in customListView1.Items)
            {
                Item.Checked = bIsChecked;
            }
        }

        /// <summary>
        /// 设置 List 中特定记录的选中标记。
        /// </summary>
        /// <param name="szKey">特定记录的 Key。</param>
        /// <param name="bIsChecked">是否选中。</param>
        public void SetListItemChecked(string szKey, bool bIsChecked)
        {
            if (customListView1.Items.ContainsKey(szKey))
                customListView1.Items[szKey].Checked = bIsChecked;
        }
    }
}
