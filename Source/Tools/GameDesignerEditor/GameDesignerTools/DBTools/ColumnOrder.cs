using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class ColumnOrder : Form
    {
        private string[] mColumns;
        private string[] mResult;
        private bool mChanged;
        private ListViewItem mToItem;

        public ColumnOrder(string[] columns, string tablename)
        {
            InitializeComponent();

            this.Text += " " + tablename;

            listViewEx1.Items.Clear();
            mColumns = columns;
            foreach(string column in columns)
            {
                string[] itemText = new string[] { column, column };
                ListViewItem item = new ListViewItem(itemText, -1);
                item.SubItems[1].Text = column;
                listViewEx1.Items.Add(item);
                item.SubItems[0].Text = item.Index.ToString();
            }

            SetupID();
        }

        private void SetupID()
        {
            foreach (ListViewItem item in listViewEx1.Items)
            {
                item.SubItems[0].Text = item.Index.ToString();
            }
        }

        private void listViewEx1_ItemDrag(object sender, ItemDragEventArgs e)
        {
            listViewEx1.DoDragDrop(listViewEx1.SelectedItems, DragDropEffects.Move);
        }

        private void listViewEx1_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
            for (int i = 0; i < e.Data.GetFormats().Length; ++i)
            {
                if (!e.Data.GetFormats()[i].Equals("System.Windows.Forms.ListView+SelectedListViewItemCollection"))
                {
                    e.Effect = DragDropEffects.None;
                }
            }
            
        }

        private void listViewEx1_DragDrop(object sender, DragEventArgs e)
        {
            if (mToItem != null)
            {
                mToItem.BackColor = listViewEx1.BackColor;
                mToItem.SubItems[0].BackColor = listViewEx1.BackColor;
                mToItem.SubItems[1].BackColor = listViewEx1.BackColor;
            }
            Point p = listViewEx1.PointToClient(new Point(e.X, e.Y));
            ListViewItem itemTmp = listViewEx1.GetItemAt(p.X, p.Y);
            if (!itemTmp.Selected)
            {
                MoveItems(e.X, e.Y);
            }
        }

        private void listViewEx1_DragOver(object sender, DragEventArgs e)
        {
            if (mToItem != null)
            {
                mToItem.BackColor = listViewEx1.BackColor;
                mToItem.SubItems[0].BackColor = listViewEx1.BackColor;
                mToItem.SubItems[1].BackColor = listViewEx1.BackColor;
            }
            Point p = listViewEx1.PointToClient(new Point(e.X, e.Y));
            if (p.Y < 20)
            {
                ListViewItem item = listViewEx1.GetItemAt(p.X, p.Y);
                int index = item.Index;
                index -= 1;
                if (index < 0)
                {
                    index = 0;
                }
                listViewEx1.Items[index].EnsureVisible();
            }
            else if (p.Y > listViewEx1.Size.Height - 10)
            {
                ListViewItem item = listViewEx1.GetItemAt(p.X, p.Y);
                int index = item.Index;
                index += 1;
                if (index >= listViewEx1.Items.Count)
                {
                    index = listViewEx1.Items.Count - 1;
                }
                listViewEx1.Items[index].EnsureVisible();
            }

            ListViewItem itemTmp = listViewEx1.GetItemAt(p.X, p.Y);
            
            if (!itemTmp.Selected)
            {
                //MoveItems(e.X, e.Y);
                Color color = Color.FromArgb(64, 203, 215);
                itemTmp.BackColor = color;
                itemTmp.SubItems[0].BackColor = color;
                itemTmp.SubItems[1].BackColor = color;
                mToItem = itemTmp;
            }
        }

        private void MoveItems(int x, int y)
        {
            if (listViewEx1.SelectedItems.Count == 0)
            {
                return;
            }

            Point p = listViewEx1.PointToClient(new Point(x, y));
            if (p.Y > listViewEx1.Items[listViewEx1.Items.Count - 1].Position.Y)
            {
                p.Y = listViewEx1.Items[listViewEx1.Items.Count - 1].Position.Y;
            }

            ListViewItem toItem = listViewEx1.GetItemAt(p.X, p.Y);
            if (toItem == null)
            {
                return;
            }
            int toIndex = toItem.Index;

            ListViewItem[] selectItems = new ListViewItem[listViewEx1.SelectedItems.Count];
            for (int i = 0; i < selectItems.Length; ++i)
            {
                selectItems[i] = listViewEx1.SelectedItems[i];
            }

            int n = 0;
  
            foreach (ListViewItem item in selectItems)
            {

                int index = toIndex;
                if (item.Index == index)
                {
                    return;
                }

                if (item.Index < index)
                {
                    index++;
                }
                else
                {
                    index = toIndex + n;
                }



                ListViewItem newItem = item.Clone() as ListViewItem;

                listViewEx1.Items.Insert(index, newItem);

                listViewEx1.Items.Remove(item);

                n++;

                newItem.Selected = true;
            }

        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            mResult = new string[mColumns.Length];
            mChanged = false;
            foreach (ListViewItem item in listViewEx1.Items)
            {
                mResult[item.Index] = item.SubItems[1].Text;
                if (mResult[item.Index] != mColumns[item.Index])
                {
                    mChanged = true;
                }
            }
            this.Close();
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            mChanged = false;
            this.Close();
        }

        public bool Changed { get { return mChanged; } }
        public string[] Result { get { return mResult; } }

       
    }
}