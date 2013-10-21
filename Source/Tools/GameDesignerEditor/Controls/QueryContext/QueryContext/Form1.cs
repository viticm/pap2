using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace QueryContext
{
    public partial class Form1 : Form
    {
        const int MAX_STRING_ID = 24000;
        public Form1()
        {
            InitializeComponent();

            string strFileText = FileFolderHelper.FileToString(System.Environment.CurrentDirectory + "\\ContextData.txt");
            int nPos = strFileText.LastIndexOf("㊣") + 3;
            strStringTable = strFileText.Substring(0, nPos);
            strStringTable = "㊣\r\n" + strStringTable;
            strContextTable = strFileText.Substring(nPos, strFileText.Length - nPos);
            strContextTable = "\r\n" + strContextTable;

            tabControl1.TabPages.Clear();
            nCurrentID = 0;

            textBox2.Parent = this;
            textBox2.Hide();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string strFind = textBox1.Text;
            int nFindID = -1; 
            try
            {
                nFindID = Int32.Parse(strFind);
            }
            catch
            {
                MessageBox.Show("只能输入数字!");
                return;
            }
            nCurrentID = nFindID;

            FindID(nFindID);
        }

        private void FindID(int nFind)
        {
            string strContextIDs = GetStringContextIDs(nFind);
            tabControl1.TabPages.Clear();
            textBox2.Show();
            if (strContextIDs == string.Empty)
            {
                textBox1.Text = nFind.ToString() + ":" + GetString(nFind);
                //    string.Empty;
                //textBox2.Text = 
                tabControl1.TabPages.Add("-1", "未找到相应上下文！");
                tabControl1.TabPages["-1"].Tag = -1;
                tabControlSelectedChanged(null, null);
                return;
            }
            else
            {
                textBox1.Text = nFind.ToString() + ":" + GetString(nFind);
                //    string.Empty;
                //textBox2.Text = 
            }
            string[] strIDs = strContextIDs.Split(new char[] {','});

            foreach (string ContextID in strIDs)
            {
                if (ContextID == "")
                    continue;
                int nContextID = Int32.Parse(ContextID);

                tabControl1.TabPages.Add(ContextID, GetContextText(nContextID));
                tabControl1.TabPages[ContextID].Tag = nContextID;
            }

            //tabControl1.SelectedIndex = 0;

            tabControlSelectedChanged(null, null);
        }


        private string GetString(int nStringID)
        {
            string strRow = GetStringRowInTable(nStringID);
            if (strRow == "")
                return string.Empty;

            int nIndexP = strRow.IndexOf("\t") + 1;

            int nIndexE = strRow.LastIndexOf("\t");

            return strRow.Substring(nIndexP, nIndexE - nIndexP);
        }

        private string GetStringContextIDs(int nStringID)
        {
            string strRow = GetStringRowInTable(nStringID);
            if (strRow == "")
                return string.Empty;

            int nIndex = strRow.LastIndexOf("\t") + 1;
            return strRow.Substring(nIndex, strRow.Length - nIndex);
        }

        private string GetStringRowInTable(int nStringID)
        {
            int nIndexP = strStringTable.IndexOf("㊣\r\n" + nStringID.ToString() + "\t") + 3;
            if (nIndexP == 2)
                return string.Empty;

            int nIndexE = strStringTable.IndexOf("㊣\r\n", nIndexP);

            return strStringTable.Substring(nIndexP, nIndexE - nIndexP);
        }

        private string GetContextRowInTable(int nContextID)
        {
            int nIndexP = strContextTable.IndexOf("\r\n" + nContextID.ToString() + "\t");
            if (nIndexP == -1)
                return string.Empty;

            nIndexP = nIndexP + 2;
            int nIndexE = strContextTable.IndexOf("\r\n", nIndexP);

            return strContextTable.Substring(nIndexP, nIndexE - nIndexP);
        }

        private string GetContextText(int nContextID)
        {
            string strContextRow = GetContextRowInTable(nContextID);

            int nIndex = strContextRow.LastIndexOf("\t") + 1;

            return strContextRow.Substring(nIndex, strContextRow.Length - nIndex);
        }

        private string GetContextStringIDs(int nContextID)
        {
            string strContextRow = GetContextRowInTable(nContextID);
            int nIndexP = strContextRow.IndexOf("\t") + 1;
            int nIndexE = strContextRow.LastIndexOf("\t");

            return strContextRow.Substring(nIndexP, nIndexE - nIndexP);
        }

        string strStringTable;
        string strContextTable;
        int nCurrentID;

        private void tabControlSelectedChanged(object sender, EventArgs e)
        {
            if (tabControl1.SelectedIndex == -1)
            {
                return ;
            }
            textBox2.Parent = tabControl1.TabPages[tabControl1.SelectedIndex];
            if (textBox2.Parent.Tag == null)
            {
                return;
            }
            if (textBox2.Parent.Tag.ToString() == "-1")
            {
                textBox2.Text = "";
                return;
            }
            int nContextID = Int32.Parse(textBox2.Parent.Tag.ToString());

            StringBuilder sb = new StringBuilder();
           // sb.Append(textBox2.Text + "\r\n\r\n\r\n");
            sb.Append("id          string\r\n");
            //string text = "id          string\r\n";
            string[] strIDs = GetContextStringIDs(nContextID).Split(',');
            
            foreach(string strID in strIDs)
            {
                if (strID == "")
                {
                    continue;
                }
                string strToShow =  GetString(Int32.Parse(strID));
                //for (int i = 55; ; )
                //{
                //    if (strToShow.Length - i < 0)
                //    {
                //        break;
                //    }
                //    strToShow = strToShow.Insert(i, "\r\n             ");
                //    i += "\r\n             ".Length + 55;
                //}
                sb.Append("\r\n" + strID + "        " + strToShow + "\r\n"); 
            }


            textBox2.Text = sb.ToString();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            while(true)
            {
                ++nCurrentID;
                string Retstring = GetString(nCurrentID);
                if (Retstring != null && Retstring != "")
                {
                    break;
                }
                if (nCurrentID > MAX_STRING_ID)
                    nCurrentID = 0;
            }
            FindID(nCurrentID);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            while (true)
            {
                --nCurrentID;
                string Retstring = GetString(nCurrentID);
                if (Retstring != null && Retstring != "")
                {
                    break;
                }
                if (nCurrentID < 0)
                    nCurrentID = MAX_STRING_ID;
            }
            FindID(nCurrentID);
        }

        private void TextBoxKeyDown(object sender, KeyEventArgs e)
        {
            
            if (e.KeyValue == 13)
            {
                button1_Click(null, null);
            }
        }

        private void textBox1_MouseClick(object sender, MouseEventArgs e)
        {
            textBox1.SelectAll();
        }

    }
}