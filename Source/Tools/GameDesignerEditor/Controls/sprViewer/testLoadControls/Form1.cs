using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Text.RegularExpressions;
using System.Collections;
using System.IO;

namespace testLoadControls
{
    public partial class Form1 : Form
    {
        sprViewer.doit sv;

        sprSelector.sprViewControl svc = null;
        descHelper.descEditor de = new descHelper.descEditor();



        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            sv = new sprViewer.doit();
            Control ct = sv.Init();
            this.splitContainer1.Panel1.Controls.Add(ct);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            sv.SetItemInfo(
                "道具名称", 3, false, 100,
                2, 3,
                50,
                "<Text>text=\"使用：在收割过的稻田里寻找稻梗。不管不管我不管" + @"\\\n" + "\" font=105 </text><Text>text=\"小心，不要碰到前面的部分……\" font=100 </text>",
                true, 12035,
                @"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\QuestPuzzle10.UITex", 0, 1
                //@"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\m5yangzhou09.UITex", 0
                );
        }

        private void button2_Click(object sender, EventArgs e)
        {
            sv.SetUtexFileName(@"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\m5yangzhou09.UITex", 0);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            //sv.SetFileName(@"D:\Kingsoft\sword3-products\trunk\client\data\source\NPC\A002\A002a.mdl");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (null == svc)
            {
                svc = new sprSelector.sprViewControl();
            }
            //string s;
            //s = svc.show(@"D:\Kingsoft\sword3-products\trunk\client\gamebin\images", "选文件", @"D:\Kingsoft\sword3-products\trunk\client\gamebin\images\shaolin\shaolin\sl_90_wxz_d.spr", 10);
            //s = svc.show4ff(@"D:\Kingsoft\sword3-products\trunk\client\data\source\NPC","");
            //MessageBox.Show(s);
        }


        private IconSelector.Implement isi = null;

        private void button5_Click(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection();
            conn.ConnectionString = "Server = changan; DataBase = s3design_rtm; Uid = sa; Password = ddd12345678;";
            if(isi == null)
            {
                isi = new IconSelector.Implement();
                isi.Init(conn, @"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\");
            }

            isi.IconID = 12;
            /*
            de.showEdit(
                "道具名称", 2, false, 100,
                2, 3,
                50,
                "<Text>text=\"使用：在收割过的稻田里寻找稻梗。不管不管我不管" + @"\\\n" + "\" font=105 </text><Text>text=\"小心，不要碰到前面的部分……\" font=100 </text>",
                true, 12035,2,isi,conn,  @"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\"
                );
             * */
            MessageBox.Show(descHelper.itemProp.iName);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection();
            conn.ConnectionString = "Server = changan; DataBase = s3design_rtm; Uid = sa; Password = 123;";
            if (isi == null)
            {
                isi = new IconSelector.Implement();
                isi.Init(conn, @"D:\Kingsoft\sword3-products\trunk\client\ui\Image\icon\");
            }
            isi.IconID = 12;
            int ret = isi.Show();
            MessageBox.Show(ret.ToString());

        }

        private void button7_Click(object sender, EventArgs e)
        {
            Regex regex = new Regex(@"\b\w+\.\w+\(\b");
            string strCode = "";
            foreach(FileInfo fi in (new DirectoryInfo(@"D:\Kingsoft\sword3-products\trunk\client\scripts\Map")).GetFiles("*.l*", SearchOption.AllDirectories))
            {
                StreamReader sr = new StreamReader(fi.FullName);
                strCode += sr.ReadToEnd();
                sr.Close();
            }

            Hashtable ht = new Hashtable();
            foreach(Match m in  regex.Matches(strCode) )
            {
                string strM = m.ToString();
                strM = strM.TrimEnd(new char[] { '(' });
                //strM = strM.Split(new char[] { '.' })[1];
                if(ht.ContainsKey(strM))
                {
                    int count = Int32.Parse(ht[strM].ToString());

                    ht[strM] = count + 1;
                }
                else
                {
                    ht.Add(strM, 1);
                }
            }

            string strResult = "";
            foreach(string strKey in ht.Keys)
            {
                strResult += strKey + "\t" + ht[strKey].ToString() + "\r\n";
            }

            //break here, and watch strResult, save text as .tab file. open it with excel.
        }
    }
}