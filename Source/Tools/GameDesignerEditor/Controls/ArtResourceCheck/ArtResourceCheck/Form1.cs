using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.IO;


namespace ArtResourceCheck
{
    public partial class Form1 : Form
    {
        SqlConnection Conn = null;
        string sqlConn = string.Empty;
        string[] playermodels = { "m1", "m2", "m3", "f1", "f2", "f3" };
        string client = @"I:/Client";
        string configfile = "../Start.ini";
        bool bImport = false;
        string sword3itemtabfile = string.Empty;
        Hashtable mtl3 = new Hashtable();


        public Form1()
        {
            InitializeComponent();
            InitConn();
            if (!InitConfig())
            {
                this.Close();
            }
        }

        private bool InitConfig()
        {
            StringBuilder sb = new StringBuilder(512);

            if (File.Exists(configfile))
            {
                FileFolderHelper.ReadIniFile("Main", "Client", string.Empty, sb, 512, configfile);
                client = sb.ToString();
                textboxClient.Text = client;

                FileFolderHelper.ReadIniFile("Main", "Import", string.Empty, sb, 512, configfile);
                bImport = (sb.ToString().Trim() == "1");

                FileFolderHelper.ReadIniFile("Main", "Sword3ItemTbl", string.Empty, sb, 512, configfile);
                sword3itemtabfile = sb.ToString();

                return true;
            }
            else
            {
                MessageBox.Show("请先设置客户端路径（修改ArtResourceCheck.ini），再使用本工具。");
                return false;
            }
        }

        // 设置client路径
        private void button4_Click(object sender, EventArgs e)
        {
            string output = FileFolderHelper.BrowserFolder("请选择客户端路径。");
            if (output.Length != 0)
            {
                client = output;
                FileFolderHelper.WriteIniFile("Main", "Client", client, configfile);
                textboxClient.Text = client;
            }
        }

        private void ReInitMtl3()
        {
            mtl3.Clear();

            string dir = "/data/source/player/M2/部件/";
            string filepath = client + dir;
            DirectoryInfo di = new DirectoryInfo(filepath);
            foreach (FileInfo fi in di.GetFiles("*.mtl", SearchOption.TopDirectoryOnly))
            {
                mtl3.Add(fi.Name.ToLower().Replace(".mtl", string.Empty), false);
            }

            dir = "/data/source/player/F2/部件/";
            filepath = client + dir;
            di = new DirectoryInfo(filepath);
            foreach (FileInfo fi in di.GetFiles("*.mtl", SearchOption.TopDirectoryOnly))
            {
                mtl3.Add(fi.Name.ToLower().Replace(".mtl", string.Empty), false);
            }
        }

        private void InitConn()
        {
            sqlConn = "Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            Conn = new SqlConnection(sqlConn);
        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            if (bImport)
            {
                // 导入到数据表：mm_sword3Item
                Helper.ClearDbTable("mm_sword3Item", Conn);
                bool bSucc = Helper.ImportTab("mm_sword3Item", sword3itemtabfile, Conn);
                if (!bSucc)
                {
                    MessageBox.Show("导入失败！");
                    return;
                }
            }


            ReInitMtl3();

            StringBuilder sb = new StringBuilder();
            sb.Append("RepresentID"); sb.Append('\t');
            sb.Append("套名"); sb.Append('\t');
            sb.Append("Color"); sb.Append('\t');
            sb.Append("m1"); sb.Append('\t');
            sb.Append("m2"); sb.Append('\t');
            sb.Append("m2_m3"); sb.Append('\t');
            sb.Append("m3"); sb.Append('\t');
            sb.Append("f1"); sb.Append('\t');
            sb.Append("f2"); sb.Append('\t');
            sb.Append("f2_f3"); sb.Append('\t');
            sb.Append("f3"); sb.Append('\t');

            sb.Append("部件"); sb.Append('\t');
            sb.Append("删除重复前"); sb.Append('\t');
            sb.Append("删除重复后"); sb.Append('\t');
            sb.Append("Name"); sb.Append('\t');
            sb.Append("GetType"); sb.Append('\t');
            sb.Append("Quality"); sb.Append('\t');
            sb.Append("Level"); sb.Append('\t');
            sb.Append("RequireLevel");
            sb.Append("\r\n");

            DataTable tblColorNum = Helper.GetDataTable("select * from mm_colorNum order by representid", Conn);
            DataTable tblEquipName = Helper.GetDataTable("select * from mm_EquipName order by name", Conn);
            DataTable tblSword3Item = Helper.GetDataTable("select * from mm_sword3Item order by tabtype, id", Conn);

            foreach (DataRow row in tblColorNum.Rows)
            {
                int representid = Convert.ToInt32(row["RepresentID"]);
                int colorNum = Convert.ToInt32(row["ColorNum"]);
                string name = row["Name"].ToString().Trim();

                for (int color = 1; color <= colorNum; color++)
                {
                    foreach (DataRow r in tblEquipName.Rows)
                    {
                        string equip = r["name"].ToString().Trim();
                        string equipcn = r["name_cn"].ToString().Trim();

                        string strselect = string.Format("RepresentID={0} and 实际偏色={1} and 部件='{2}'", representid, color, equipcn);
                        DataRow[] rowsMatch = tblSword3Item.Select(strselect, "TabType, ID");

                        // 先看看去掉重复名字后的装备有多少
                        ArrayList namesArr = new ArrayList();
                        foreach (DataRow rowMatch in rowsMatch)
                        {
                            string namematch = rowMatch["Name"].ToString().Trim();
                            if (!namesArr.Contains(namematch))
                                namesArr.Add(namematch);
                        }
                        int CountNoDup = namesArr.Count;

                        foreach (DataRow rowMatch in rowsMatch)
                        {
                            string namematch = rowMatch["Name"].ToString().Trim();
                            string gettype = rowMatch["GetType"].ToString().Trim();
                            string quality = rowMatch["Quality"].ToString().Trim();
                            string level = rowMatch["Level"].ToString().Trim();
                            string requirelevel = rowMatch["RequireLevel"].ToString().Trim();

                            sb.Append(representid); sb.Append('\t');
                            sb.Append(name); sb.Append('\t');
                            sb.Append(color); sb.Append('\t');

                            foreach (string playermodel in playermodels)
                            {
                                string targetFile = equip + "_" + playermodel + ".txt";
                                targetFile = client + "/represent/player/equip/" + targetFile;
                                string mtl = FindMtlInFile(targetFile, representid);
                                sb.Append(mtl); sb.Append('\t');

                                if (playermodel == "m2" || playermodel == "f2")
                                {
                                    if (mtl3.ContainsKey(mtl.ToLower()))
                                    {
                                        sb.Append("√"); sb.Append('\t');
                                        mtl3[mtl.ToLower()] = true;
                                    }
                                    else
                                    {
                                        sb.Append(string.Empty); sb.Append('\t');
                                    }
                                }
                            }
                            sb.Append(equipcn); sb.Append('\t');
                            sb.Append(rowsMatch.Length); sb.Append('\t');
                            sb.Append(CountNoDup); sb.Append('\t');
                            sb.Append(namematch); sb.Append('\t');
                            sb.Append(gettype); sb.Append('\t');
                            sb.Append(quality); sb.Append('\t');
                            sb.Append(level); sb.Append('\t');
                            sb.Append(requirelevel);
                            sb.Append("\r\n");
                        }
                    }
                }
            }

            // 输出m3中没用到的
            OutputRestMtl3(ref sb);

            string content = sb.ToString();
            FileFolderHelper.StringToFile(content, "../output1.tab");
        }

        private string FindMtlInFile(string file, int representid)
        {
            if (!File.Exists(file))
                return string.Empty;

            string content = FileFolderHelper.FileToString(file);
            string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            string[] colnames = lines[0].Split('\t');
            int materialIndex = 0;
            for (int i = 0; i < colnames.Length; i++)
            {
                string colname = colnames[i];
                if (colname.ToLower() == "material")
                {
                    materialIndex = i;
                    break;
                }
            }

            // 忽略首行
            for (int i = 1; i < lines.Length; i++)
            {
                string[] cols = lines[i].Split('\t');
                int thisrepresentid = Convert.ToInt32(cols[0]);
                string material = cols[materialIndex];

                if (thisrepresentid == representid)
                {
                    if (material.Trim().Length != 0)
                    {
                        int startindex = material.LastIndexOfAny(new char[] { '/', '\\' });
                        string mtl = material.Substring(startindex + 1, material.Length - startindex - 1);
                        mtl = mtl.Substring(0, mtl.IndexOf('.'));
                        return mtl;
                    }
                    else
                    {
                        return string.Empty;
                    }
                }
            }

            return string.Empty;
        }

    }
}