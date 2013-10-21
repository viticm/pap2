using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace GameDesingerTools
{
    public partial class AttachAttribute : Form
    {
        private ImageList imageList; // 图片链表
        private object dragItem; // 拖动的Item
        private enum DragView { view1, view2, none } // 拖动来源窗口枚举
        private enum State { unpair, pair } // 关联状态枚举
        private DragView view = DragView.none; // 拖动来源窗口
        private State state = State.pair; // 关联状态
        private List<string[]> dataList = new List<string[]>(); // 关联数据链表
        private List<string> attributeList1 = new List<string>(); // 属性链表1
        private List<string> attributeList2 = new List<string>(); // 属性链表2 
        private List<string> existAttributes1 = new List<string>(); // 存在的属性链表1
        private List<string> existAttributes2 = new List<string>(); // 存在的属性链表2

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="imageList">图片链表</param>
        public AttachAttribute(ImageList imageList)
        {
            this.imageList = imageList;
            InitializeComponent();
            listView1.SmallImageList = imageList;
            listView1.LargeImageList = imageList;
            listView1.StateImageList = imageList;
            listView2.SmallImageList = imageList;
            listView2.LargeImageList = imageList;
            listView2.StateImageList = imageList;
        }

        /// <summary>
        /// 关联数据链表属性
        /// </summary>
        public List<string[]> DataList
        {
            get
            {
                return dataList;
            }
        }

        /// <summary>
        /// 属性链表1属性
        /// </summary>
        public List<string> AttributeList1
        {
            set
            {
                attributeList1 = value;
            }
        }

        /// <summary>
        /// 属性链表2属性
        /// </summary>
        public List<string> AttributeList2
        {
            set
            {
                attributeList2 = value;
            }
        }

        /// <summary>
        /// 存在属性链表1
        /// </summary>
        public List<string> ExistAttributes1
        {
            set
            {
                existAttributes1 = value;
            }
        }

        /// <summary>
        /// 存在属性链表2
        /// </summary>
        public List<string> ExistAttributes2
        {
            set
            {
                existAttributes2 = value;
            }
        }

        /// <summary>
        /// 填充数据
        /// </summary>
        public void FillContent()
        {
            foreach (string s in attributeList1) // 填充主表属性列表
            {
                ListViewItem item = listView1.Items.Add(s);
                item.ImageIndex = 2;
            }

            foreach (string s in attributeList2) // 填充虚拟字段表属性列表
            {
                ListViewItem item = listView2.Items.Add(s);
                item.ImageIndex = 2;
            }

            for(int i = 0; i < existAttributes1.Count; i++) // 填充关联关系列表
            {
                listBox3.Items.Add(existAttributes1[i] + "  ——  " + existAttributes2[i]);
                dataList.Add(new string[] { existAttributes1[i], existAttributes2[i] });
            }
        }

        /// <summary>
        /// 拖动进入控件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listBox3_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.Text))
            {
                e.Effect = DragDropEffects.Move;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        /// <summary>
        /// 拖动放下处理
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listBox3_DragDrop(object sender, DragEventArgs e)
        {
            string dummy = "temp";
            string text = "";
            //获得进行"Drag"操作中拖动的字符串
            string info = e.Data.GetData(dummy.GetType()) as string;
            string regexPattern = @"ListViewItem: \{(?<value>.*)\}";
            Regex r = new Regex(regexPattern);

            if (r.IsMatch(info))
            {
                text = r.Match(info).Result("${value}");
            }
            else
            {
                return;
            }

            if (state == State.pair && view == DragView.view2)
            {
                MessageBox.Show("必须先设置主表中的字段!");
                return;
            }

            if (state == State.unpair && view == DragView.view1)
            {
                MessageBox.Show("主表中的字段已经设置过!");
                return;
            }
            
            int count = listBox3.Items.Count;

            switch (view)
            {
                case DragView.none:
                    {
                        break;
                    }
                case DragView.view1:
                    {
                        dataList.Add(new string[] { "", "" });
                        listView1.Items.Remove(dragItem as ListViewItem);
                        state = State.unpair;
                        dataList[count][0] = text;
                        listBox3.Items.Add(text);
                        break;
                    }
                case DragView.view2:
                    {
                        listView2.Items.Remove(dragItem as ListViewItem);
                        state = State.pair;
                        dataList[count - 1][1] = text;
                        text = dataList[count - 1][0] + " - " + text;
                        listBox3.Items[count - 1] = text;
                        break;
                    }
            }

            listBox3.SelectedItem = text;
        }

        /// <summary>
        /// 用户拖动组件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listView1_ItemDrag(object sender, ItemDragEventArgs e)
        {
            dragItem = e.Item;
            view = DragView.view1;
            string strItem = e.Item.ToString();
            //开始进行"Drag"操作
            DoDragDrop(strItem, DragDropEffects.Move);
        }

        /// <summary>
        /// 用户拖动组件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void listView2_ItemDrag(object sender, ItemDragEventArgs e)
        {
            dragItem = e.Item;
            view = DragView.view2;
            string strItem = e.Item.ToString();
            //开始进行"Drag"操作
            DoDragDrop(strItem, DragDropEffects.Move);
        }

        /// <summary>
        /// 用户选择删除关联
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (listBox3.SelectedItem != null)
            {
                object o = listBox3.SelectedItem;
                int index = listBox3.SelectedIndex;
                if (dataList[index][0] != "")
                {
                    ListViewItem item = listView1.Items.Add(dataList[index][0]);
                    item.ImageIndex = 2;
                }
                if (dataList[index][1] != "")
                {
                    ListViewItem item = listView2.Items.Add(dataList[index][1]);
                    item.ImageIndex = 2;
                }
                if(index == listBox3.Items.Count -1)
                {
                    state = State.pair;
                }
                dataList.RemoveAt(index);
                listBox3.Items.Remove(o);
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            int count = listBox3.Items.Count;
            if (count > 0 && dataList[count - 1][1] == "")
            {
                MessageBox.Show("关联关系不完整!");
                return;
            }
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}