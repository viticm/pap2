using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using LuaInterface;
using DevComponents.DotNetBar;

namespace Mask32Ctrl
{
    public partial class Form1 : Form
    {
        public uint ret = 0;

        private const int panel_width = 170;

        private ItemPanel[] ItemPanels;
        private ItemContainer[] ItemContainers;

        public Form1()
        {
            InitializeComponent();
        }

        public Form1(uint oldvalue, LuaTable nums, LuaTable texts)
        {
            InitializeComponent();

            int text_index = 0;
            int group_count = nums.Values.Count;
            uint r = 0x80000000;

            // 更新form大小
            int old_width = this.ClientSize.Width;
            int old_height = this.ClientSize.Height;
            this.ClientSize = new System.Drawing.Size(old_width + (group_count - 4) * panel_width, old_height);

            // 动态生成控件们
            ItemPanels = new ItemPanel[group_count];
            ItemContainers = new ItemContainer[group_count];
            for (int i = 0; i < group_count; i++)
            {
                // checkboxes
                CheckBoxItem[] checkboxes = new CheckBoxItem[Convert.ToInt32(nums[i + 1])]; // 1-based lua-table
                for (int j = 0; j < checkboxes.Length; j++)
                {
                    checkboxes[j] = new CheckBoxItem();
                    checkboxes[j].Name = "CheckBoxItem" + Convert.ToString(j + 1);
                    checkboxes[j].Text = texts[++text_index].ToString(); // 1-based lua-table
                    if ((oldvalue & r) != 0)
                    {
                        checkboxes[j].Checked = true;
                    }
                    r >>= 1;
                }

                // container
                ItemContainers[i] = new ItemContainer();
                ItemContainers[i].LayoutOrientation = itemContainer1.LayoutOrientation;
                ItemContainers[i].MinimumSize = new System.Drawing.Size(0, 0);
                ItemContainers[i].Name = "ItemContainer" + Convert.ToString(i + 1);
                ItemContainers[i].SubItems.AddRange(checkboxes);

                // panel
                ItemPanels[i] = new ItemPanel();
                ItemPanels[i].BackgroundStyle.BackColor = itemPanel1.BackgroundStyle.BackColor;
                ItemPanels[i].BackgroundStyle.BorderBottom = itemPanel1.BackgroundStyle.BorderBottom;
                ItemPanels[i].BackgroundStyle.BorderBottomWidth = itemPanel1.BackgroundStyle.BorderBottomWidth;
                ItemPanels[i].BackgroundStyle.BorderColor = itemPanel1.BackgroundStyle.BorderColor;
                ItemPanels[i].BackgroundStyle.BorderLeft = itemPanel1.BackgroundStyle.BorderLeft;
                ItemPanels[i].BackgroundStyle.BorderLeftWidth = itemPanel1.BackgroundStyle.BorderLeftWidth;
                ItemPanels[i].BackgroundStyle.BorderRight = itemPanel1.BackgroundStyle.BorderRight;
                ItemPanels[i].BackgroundStyle.BorderRightWidth = itemPanel1.BackgroundStyle.BorderRightWidth;
                ItemPanels[i].BackgroundStyle.BorderTop = itemPanel1.BackgroundStyle.BorderTop;
                ItemPanels[i].BackgroundStyle.BorderTopWidth = itemPanel1.BackgroundStyle.BorderTopWidth;
                ItemPanels[i].BackgroundStyle.Class = itemPanel1.BackgroundStyle.Class;
                ItemPanels[i].BackgroundStyle.PaddingBottom = itemPanel1.BackgroundStyle.PaddingBottom;
                ItemPanels[i].BackgroundStyle.PaddingLeft = itemPanel1.BackgroundStyle.PaddingLeft;
                ItemPanels[i].BackgroundStyle.PaddingRight = itemPanel1.BackgroundStyle.PaddingRight;
                ItemPanels[i].BackgroundStyle.PaddingTop = itemPanel1.BackgroundStyle.PaddingTop;
                ItemPanels[i].LayoutOrientation = itemPanel1.LayoutOrientation;
                ItemPanels[i].Location = new System.Drawing.Point(12 + i * panel_width, 12);
                ItemPanels[i].Name = "ItemPanel" + Convert.ToString(i + 1);
                ItemPanels[i].Size = itemPanel1.Size;
                ItemPanels[i].Text = ItemPanels[i].Name;
                ItemPanels[i].Items.AddRange(new BaseItem[] { ItemContainers[i] });

                // add panel to form
                this.panelEx1.Controls.Add(ItemPanels[i]);
            }
        }

        private void buttonX1_Click(object sender, EventArgs e) // 确定
        {
            ret = 0;
            uint r = 0x80000000;

            for (int i = 0; i < ItemContainers.Length; i++)
            {
                for (int j = 0; j < ItemContainers[i].SubItems.Count; j++)
                {
                    CheckBoxItem checkbox = (CheckBoxItem)ItemContainers[i].SubItems[j];
                    if (checkbox.Checked)
                    {
                        ret += r;
                    }
                    r >>= 1;
                }
            }

            DialogResult = DialogResult.OK;
            this.Close();
        }

        private void buttonX2_Click(object sender, EventArgs e) // 取消
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}