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
    public partial class Form1
    {
        private void button2_Click(object sender, EventArgs e)
        {
            ReInitMtl3();

            string[] mdlplugs = { "head", "body", "leg", "hand", "belt", "plait", "bang" };

            // 第一行
            StringBuilder sb = new StringBuilder();
            sb.Append("RepresentID"); sb.Append('\t');
            sb.Append("Map"); sb.Append('\t');
            sb.Append("Name"); sb.Append('\t');
            sb.Append("MainModelFile"); sb.Append('\t');
            sb.Append("IsPlayerModel"); sb.Append('\t');
            foreach (string mdlplug in mdlplugs)
            {
                sb.Append(mdlplug); sb.Append('\t');
                sb.Append(mdlplug + "_m3"); sb.Append('\t');
            }
            sb.Append("\r\n");

            // 
            string sql = @"SELECT RepresentID, map, Model, IsPlayerModel, MainModelFile FROM npc WHERE (IsPlayerModel = 1) ORDER BY map, Model";
            DataTable TblNpc = Helper.GetDataTable(sql, Conn);
            foreach (DataRow row in TblNpc.Rows)
            {
                int representid = Convert.ToInt32(row["RepresentID"]);
                string map = row["map"].ToString().Trim();
                string model = row["Model"].ToString().Trim();
                int isplayermodel = Convert.ToInt32(row["IsPlayerModel"]);
                string mainmodelfile = row["MainModelFile"].ToString().Trim();

                sb.Append(representid); sb.Append('\t');
                sb.Append(map); sb.Append('\t');
                sb.Append(model); sb.Append('\t');
                sb.Append(mainmodelfile); sb.Append('\t');
                sb.Append(isplayermodel); sb.Append('\t');

                string file = client + "/" + mainmodelfile;
                string mdlFileContent = FileFolderHelper.FileToString(file);
                string[] lines = mdlFileContent.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                string[] htMtl = { string.Empty, string.Empty, string.Empty, string.Empty, string.Empty, string.Empty, string.Empty };
                string[] htM3 = { string.Empty, string.Empty, string.Empty, string.Empty, string.Empty, string.Empty, string.Empty };

                foreach (string line in lines)
                {
                    string linelow = line.ToLower();

                    // 忽略首行
                    if (line.Contains(".txt"))
                        continue;

                    string[] cols = linelow.Split(new string[] { "  " }, StringSplitOptions.RemoveEmptyEntries);
                    string meshmtl = cols[cols.Length - 1]; // 取出mtl或者mesh
                    meshmtl = meshmtl.Substring(meshmtl.LastIndexOf('\\') + 1);
                    meshmtl = meshmtl.Replace(".mesh", string.Empty);
                    meshmtl = meshmtl.Replace(".mtl", string.Empty);

                    for (int i = 0; i < mdlplugs.Length; i++)
                    {
                        if (meshmtl.Contains(mdlplugs[i]))
                        {
                            htMtl[i] = meshmtl;
                            break;
                        }
                    }

                    for (int i = 0; i < htMtl.Length; i++)
                    {
                        if (mtl3.ContainsKey(htMtl[i]))
                        {
                            htM3[i] = "√";
                            mtl3[htMtl[i]] = true;
                        }
                    }
                }

                for (int i = 0; i < htMtl.Length; i++)
                {
                    sb.Append(htMtl[i]);
                    sb.Append('\t');
                    sb.Append(htM3[i]);
                    sb.Append('\t');
                }

                sb.Append("\r\n");
            }

            // 输出m3中没用到的
            OutputRestMtl3(ref sb);

            string filecontent = sb.ToString();
            FileFolderHelper.StringToFile(filecontent, "../output2.tab");
        }
    }
}
