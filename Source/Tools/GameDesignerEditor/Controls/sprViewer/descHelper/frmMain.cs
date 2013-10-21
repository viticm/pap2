using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using sprViewer;

namespace descHelper
{
    public partial class frmMain : Form
    {
        public string m_desc = "";
        private string m_icoFileName = "";
        private int m_icoFrame = 0;
        doit sv = new doit();

        public frmMain()
        {
            InitializeComponent();
            this.panelEx2.Controls.Add(sv.Init());            
        }

        private void splitDesc()
        {
            //解释desc先
            string strDesc1 = "";
            string strDesc2 = "";
            {
                string[] asp = { "<Text>text=\"", "\" font=", "</text>", @"\\\n" };
                string[] sr = m_desc.Replace("<text>","<Text>").Split(asp, StringSplitOptions.RemoveEmptyEntries);
                if (sr.Length == 4)
                {
                    strDesc1 = sr[0];
                    strDesc2 = sr[2];
                }
                else if (sr.Length == 2)
                {
                    if (sr[0].StartsWith("使用：")
                        || sr[0].StartsWith("装备：")
                        || sr[0].StartsWith("携带：")
                        )
                    {
                        strDesc1 = sr[0];
                    }
                    else
                    {
                        strDesc2 = sr[0];
                    }
                }
            }
            if (strDesc1.StartsWith("使用：") )
            {
                comboBox1.Text = "使用";
                textBox1.Text = strDesc1.Substring(3);
            }
            else if (strDesc1.StartsWith("装备："))
            {
                comboBox1.Text = "装备";
                textBox1.Text = strDesc1.Substring(3);
            }
            else if (strDesc1.StartsWith("携带："))
            {
                comboBox1.Text = "携带";
                textBox1.Text = strDesc1.Substring(3);
            }
            else
            {
                comboBox1.Text = "使用";
                textBox1.Text = strDesc1;
            }
            textBox2.Text = strDesc2;
            if (textBox1.Text == "" && textBox2.Text == "")
            {
                textBox2.Text = m_desc;
            }

            //填好其他的
            txtName.Text = itemProp.iName;
            txtIco.Text = itemProp.icoID.ToString();
            textSoundID.Text = itemProp.SoundID.ToString();
            textRequirement.Text = itemProp.requirement;
        }

        private string comboDesc()
        {
            string ret = "";
            bool bText1NotNull = textBox1.Text.Trim().Length > 0;
            bool bText2NotNull = textBox2.Text.Trim().Length > 0;

            if (bText1NotNull)
            {
                ret += "<Text>text=\"" + comboBox1.Text + "：" + textBox1.Text + (bText2NotNull ? @"\\\n" : string.Empty) + "\" font=105 </text>";
            }
            if (bText2NotNull)
            {
                ret += "<Text>text=\"" + textBox2.Text + "\" font=100 </text>";
            }
            return (ret);
        }

        private void resetPic(string desc)
        {
            sv.SetItemInfo(itemProp.iName, itemProp.iColor, itemProp.iCanStack, itemProp.iMax, 
                itemProp.tGenre, itemProp.tBindType, 
                itemProp.maxExist, desc, itemProp.canTrade, itemProp.price, 
                m_icoFileName, m_icoFrame, itemProp.maxexisttime);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            timer1.Enabled = true;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            timer1.Enabled = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            resetPic(comboDesc());
            timer1.Enabled = false;
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            if (itemProp.desc == null) 
                m_desc = "";
            else
                m_desc = itemProp.desc;
            splitDesc();
            timer1.Enabled = true;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            timer1.Enabled = true;
        }

        private void buttonX1_Click(object sender, EventArgs e) // OnOK
        {
            textBox1.Text = charReplace(textBox1.Text);
            textBox2.Text = charReplace(textBox2.Text);
            m_desc = comboDesc();
            itemProp.iName = txtName.Text;
            itemProp.icoID = Int32.Parse(txtIco.Text);
            itemProp.desc = m_desc;
            itemProp.requirement = textRequirement.Text.Trim();
            if (textSoundID.Text.Length != 0)
                itemProp.SoundID = Int32.Parse(textSoundID.Text);
            else
                itemProp.SoundID = -1;
            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        private void buttonX2_Click(object sender, EventArgs e) // OnCancel
        {
            this.DialogResult = DialogResult.Cancel;
            this.Visible = false;
        }

        private void txtName_TextChanged(object sender, EventArgs e)
        {
            itemProp.iName = txtName.Text;
            timer1.Enabled = true;
        }

        private void txtIco_TextChanged(object sender, EventArgs e)
        {
            itemProp.icoID = Int32.Parse(txtIco.Text);
            SqlDb sqldb = new SqlDb();
            try
            {
                sqldb.SetConnection(itemProp.conn);
                DataSet ds = sqldb.ExeSqlDs("select filename,frame from icon where id=" + txtIco.Text, "icon");
                m_icoFileName = ds.Tables[0].Rows[0]["filename"].ToString();
                m_icoFileName = System.IO.Path.Combine(itemProp.iconDir, m_icoFileName);
                m_icoFrame = Int32.Parse(ds.Tables[0].Rows[0]["frame"].ToString());
            }
            catch
            {
                m_icoFileName = "";
                m_icoFrame = 0;
            }
            timer1.Enabled = true;
        }

        private void buttonX3_Click(object sender, EventArgs e)
        {
            if(itemProp.icoSelector != null)
            {
                itemProp.icoSelector.IconID = itemProp.icoID;

                if (!itemProp.icoSelector.Inited) // 延迟初始化
                {
                    itemProp.icoSelector.Init();
                }

                int ret = itemProp.icoSelector.Show();

                if (ret >= 0)
                {
                    txtIco.Text = ret.ToString();
                    SqlDb sqldb = new SqlDb();
                    sqldb.SetConnection(itemProp.conn);
                    DataSet ds = sqldb.ExeSqlDs("select soundid from icon where id=" + txtIco.Text, "icon");
                    textSoundID.Text = ds.Tables[0].Rows[0][0].ToString();
                }
            }
            else
            {
                MessageBox.Show("icoSelector尚未初始化");
            }
        }




        private string charReplace(string s)
        {
            s = s.Replace(",", "，");
            s = s.Replace(",", "，");
            s = s.Replace(".", "。");
            //s = s.Replace("<", "《");
            //s = s.Replace(">", "》");
            s = s.Replace("!", "！");
            s = s.Replace("?", "？");
            s = s.Replace("-", "—");
            s = s.Replace("^", "……");
            s = s.Replace("_", "——");
            s = s.Replace("@", "·");
            //s = s.Replace("\\", "、");
            s = s.Replace("\t", "\\t");
            s = s.Replace("\r\n", "\\n");
            s = s.Replace("\n", "\\n");
            s = s.Replace(";", "；");
            s = s.Replace(":", "：");
            s = s.Replace(")", "）");
            s = s.Replace("(", "（");
            s = s.Replace("'", "’");
            s = s.Replace("\"", "“");

            s = s.Replace("#", "");
            //s = s.Replace(" ", "");
            s = s.Replace("$", "");
            s = s.Replace("&", "");
            s = s.Replace("*", "");
            s = s.Replace("|", "");
            s = s.Replace("~", "");
            s = s.Replace("`", "");
            s = s.Replace("+", "");
            s = s.Replace("=", "");
            s = s.Replace("{", "");
            s = s.Replace("}", "");
            s = s.Replace("/", "");
            s = s.Replace("[", "");
            s = s.Replace("]", "");
            return s;
        }

    }

}