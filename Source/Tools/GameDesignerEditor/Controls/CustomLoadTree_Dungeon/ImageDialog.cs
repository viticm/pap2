using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree_Dungeon
{
    public partial class ImageDialog : Form
    {
        public ImageDialog(string filename)
        {
            InitializeComponent();
            m_strFileName = filename;

            int nPos = m_strFileName.LastIndexOf('.');
            string strExpandName = m_strFileName.Substring(nPos, m_strFileName.Length - nPos).ToLower();

            listBox1.Items.Clear();
            if (strExpandName == ".tga")
            {
                string itemText = "0 0 0 " + m_strFileName;
                listBox1.Items.Add(itemText);
            }
            else if (strExpandName == ".uitex")
            {
                string strfile = FileFolderHelper.FileToString(m_strFileName.Substring(0, nPos) + ".txt");
                DataTable dt = Helper.CreateDataTable(strfile);

                foreach (DataRow row in dt.Rows)
                {
                    string strFrame = row["Farme"].ToString();
                    string strWidth = row["Width"].ToString();
                    string strHeight = row["High"].ToString();

                    if (Int32.Parse(strWidth) != 0)
                    {
                        string itemText = strFrame + " " + strWidth + " " + strHeight + " " + row["File"].ToString();
                        listBox1.Items.Add(itemText);
                    }
                }
            }
            else
            {
                MessageBox.Show("不支持的文件格式");
            }
        }


        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;

            if (listBox1.SelectedIndex == -1)
            {
                strFrame = "0";
            }
            strWidth = textBoxWidth.Text;
            strHeight = textBoxHeight.Text;

            Hide();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Hide();
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string itemText = listBox1.SelectedItem.ToString();

            int nSPos = 0;
            int nPos = 0;
            nPos = itemText.IndexOf(" ", nPos);
            strFrame = itemText.Substring(0, nPos);

            nSPos = nPos + 1;
            nPos = itemText.IndexOf(" ", nPos + 1);
            strWidth = itemText.Substring(nSPos, nPos - nSPos);

            nSPos = nPos + 1;
            nPos = itemText.IndexOf(" ", nPos + 1);
            strHeight = itemText.Substring(nSPos, nPos - nSPos);

            textBoxHeight.Text = strHeight;
            textBoxWidth.Text = strWidth;
        }

        public string strFrame = string.Empty;
        public string strWidth = string.Empty;
        public string strHeight = string.Empty;

        string m_strFileName;
    }
}