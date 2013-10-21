using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DataTableRemotingClient;
using System.Collections;

namespace CustomLoadTree_CareerEvent
{
    public partial class InsertLinkForm : Form
    {
        VisualEditLibEx m_Parent;
        SqlConnection m_Conn;
        string m_Root;

        public int nPos;
        public int nFontID;
        public string linkText;
        public string modelText;

        ArrayList listBoxItemTag;
        ArrayList listBoxItemTagBak;
        ArrayList listBoxItemTextBak;
        public ArrayList LinkArray;
        int m_nDefFont;

        public InsertLinkForm(VisualEditLibEx parent, SqlConnection Conn, string Root, int nDefFont)
        {
            InitializeComponent();
            m_Parent = parent;
            m_Conn = Conn;
            m_Root = Root;
            m_nDefFont = nDefFont;

            int nPosCount = m_Parent.LinkArray.Count;
            for (int i = -1; i < nPosCount; i++)
            {
                comboBox_InsertPos.Items.Add(i + 2);
            }

            for (int i = 0; i < m_Parent.FontBox.Items.Count; i++)
            {
                comboBox_Font.Items.Add(m_Parent.FontBox.Items[i]);
            }
                
            comboBox_Model.Items.Add("等级提示");
            comboBox_Model.Items.Add("剑三知道");
            comboBox_Model.Items.Add("活动");
            comboBox_Model.Items.Add("秘境");
            comboBox_Model.Items.Add("日常任务");

            listBoxItemTag = new ArrayList();
            LinkArray = new ArrayList();

            //listBoxItemTagBak = new ArrayList();
            //listBoxItemTextBak = new ArrayList();

            for (int i = 0; i < m_Parent.LinkArray.Count; i++)
            {
                LinkArray.Add(m_Parent.LinkArray[i]);
                listBox2.Items.Add((m_Parent.LinkArray[i] as Object[])[0].ToString() + "<" + (m_Parent.LinkArray[i] as Object[])[2].ToString() + ">");
            }

            comboBox_Font.SelectedIndex = comboBox_Font.Items.Count - 1;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Hide();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Hide();
        }

        private string GetModelName()
        {
            string strRet = string.Empty;

            if (comboBox_Model.SelectedIndex == 0)
            {
                strRet = "Career";
            }
            else if (comboBox_Model.SelectedIndex == 1)
            {
                strRet = "Library";
            }
            else if (comboBox_Model.SelectedIndex == 2)
            {
                strRet = "Active";
            }
            else if (comboBox_Model.SelectedIndex == 3)
            {
                strRet = "DungeonInfo";
            }
            else if (comboBox_Model.SelectedIndex == 4)
            {
                strRet = "QuestDaily";
            }
            else if (comboBox_Model.SelectedIndex == -1)
            {
                MessageBox.Show("错误，未选择模块");
            }
            else 
            {
                MessageBox.Show("错误，未定义的选项");
            }

            return strRet;
        }

        private void comboBox_Model_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox1.Text = string.Empty;
            listBox1.Items.Clear();
            listBoxItemTag.Clear();
            
            if (comboBox_Model.SelectedIndex == 0)
            {
               // strRet = "Career";
                DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_CareerEvent", "level > 0 and tabid = -1", null, m_Conn);
                foreach (DataRow row in dt.Rows)
                {
                    listBox1.Items.Add(row["level"].ToString() + " _ " + row["name"].ToString());
                    listBoxItemTag.Add("/" + row["level"].ToString());
                }
            }
            else if (comboBox_Model.SelectedIndex == 1)
            {
               // strRet = "Library";
                DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_JX3Library", null, null, m_Conn);
                foreach (DataRow row in dt.Rows)
                {
                    string ItemsText = string.Empty;
                    if (row["classname"].ToString() != string.Empty)
                    {
                        ItemsText = row["classname"].ToString();
                    }
                    if (row["subclassname"].ToString() != string.Empty)
                    {
                        ItemsText = row["subclassname"].ToString();
                    }
                    if (row["title"].ToString() != string.Empty)
                    {
                        ItemsText = row["title"].ToString();
                    }

                    listBox1.Items.Add(ItemsText);
                    listBoxItemTag.Add("/" + row["classid"].ToString() + "/" + row["subclassid"].ToString() + "/" + row["id"].ToString());
                }
            }
            else if (comboBox_Model.SelectedIndex == 2)
            {
               // strRet = "Active";
                DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_ActivityInfo", "ClassID > 0", null, m_Conn);
                foreach (DataRow row in dt.Rows)
                {
                    string ItemsText = string.Empty;
                    if (row["classname"].ToString() != string.Empty)
                    {
                        ItemsText = row["classname"].ToString().TrimEnd(new char[] { ' ', '\n', '\r'});
                    }
                    if (row["title"].ToString() != string.Empty)
                    {
                        ItemsText = row["title"].ToString().TrimEnd(new char[] { ' ', '\n', '\r' });
                    }

                    string markstring = "/" + row["classid"].ToString() + "/" + row["ActivityID"].ToString();
                    listBox1.Items.Add(ItemsText + " __ " + GetModelName() + markstring);
                    listBoxItemTag.Add(markstring);
                }
            }
            else if (comboBox_Model.SelectedIndex == 3)
            {
                //strRet = "DungeonInfo";
                DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_Dungeon", "ClassID > 0", null, m_Conn);
                foreach (DataRow row in dt.Rows)
                {
                    string ItemsText = string.Empty;
                    if (row["classname"].ToString() != string.Empty)
                    {
                        ItemsText = row["classname"].ToString().TrimEnd(new char[] { ' ', '\n', '\r' });
                    }
                    if (row["MapID"].ToString() != "0")
                    {
                        ItemsText = GetMapNameByID(Int32.Parse(row["MapID"].ToString())).TrimEnd(new char[] { ' ', '\n', '\r' }); 
                    }

                    string markstring = "/" + row["classid"].ToString() + "/" + row["MapID"].ToString();
                    listBox1.Items.Add(ItemsText + " __ " + GetModelName() + markstring);
                    listBoxItemTag.Add(markstring);
                }
            }
            else if (comboBox_Model.SelectedIndex == 4)
            {
                //strRet = "QuestDaiky";
                DataTable dt = TableHelper.GetDataTableProxy("tbl_Guide_DailyQuestInfo", "TypeID > 0", null, m_Conn);
                foreach (DataRow row in dt.Rows)
                {
                    string ItemsText = string.Empty;

                    ItemsText = row["TypeName"].ToString().TrimEnd(new char[] { ' ', '\n', '\r' });

                    string markstring = "/" + row["TypeID"].ToString() + "/" + row["QuestID"].ToString();
                    listBox1.Items.Add(ItemsText + " __ " + GetModelName() + markstring);
                    listBoxItemTag.Add(markstring);
                }
            }
            else
            {
                MessageBox.Show("错误，未定义的选项");
            }

            listBoxItemTagBak = new ArrayList(listBoxItemTag);
            listBoxItemTextBak = new ArrayList(listBox1.Items);
        }

        private string GetMapNameByID(int nMapID)
        {
            string filter = string.Format("ID = {0}", nMapID);
            DataTable dt = TableHelper.GetDataTableProxy("maplist", filter, null, m_Conn);

            if (dt.Rows.Count == 0)
                return string.Empty;
            else
                return dt.Rows[0]["Name"].ToString();
        }


        private void button3_Click(object sender, EventArgs e)
        {
            nPos = comboBox_InsertPos.SelectedIndex;
            nFontID = comboBox_Font.SelectedIndex;
            linkText = textBox_Text.Text;

            string strModelName = GetModelName();

            if (comboBox_Model.SelectedIndex == -1
                || nPos == -1
                || linkText == string.Empty
                || strModelName == string.Empty
                || listBox1.SelectedIndex == -1)
            {
                return;
            }

            modelText = strModelName + listBoxItemTag[listBox1.SelectedIndex];

            listBox2.Items.Insert(nPos, linkText + "<" + modelText + ">");
            if (nFontID == -1 || nFontID == comboBox_Font.Items.Count - 1)
                nFontID = m_nDefFont;
            LinkArray.Insert(nPos, new Object[] { linkText, nFontID.ToString(), modelText });
            comboBox_InsertPos.Items.Add(comboBox_InsertPos.Items.Count + 1);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (listBox2.SelectedIndex != -1)
            {
                LinkArray.RemoveAt(listBox2.SelectedIndex);
                listBox2.Items.RemoveAt(listBox2.SelectedIndex);
                
                comboBox_InsertPos.Items.RemoveAt(comboBox_InsertPos.Items.Count - 1);
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            for (int i = 0; i < listBoxItemTextBak.Count; i++) 
                listBox1.Items.Add(listBoxItemTextBak[i]);
            listBoxItemTag = listBoxItemTagBak.Clone() as ArrayList;

            if (textBox1.Text != string.Empty)
            {
                for (int i = listBoxItemTag.Count - 1; i >= 0; i--)
                {
                    if (!listBox1.Items[i].ToString().Contains(textBox1.Text))
                    {
                        listBoxItemTag.RemoveAt(i);
                        listBox1.Items.RemoveAt(i);
                    }
                }
            }
        }
    }
}