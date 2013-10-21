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
        private void button3_Click(object sender, EventArgs e)
        {
            string dir = "/data/source/player/M2/部件/";
            string outputM2 = DirectoryToString(dir);
            FileFolderHelper.StringToFile(outputM2, "../output3_M2.tab");
            
            dir = "/data/source/player/F2/部件/";
            string outputF2 = DirectoryToString(dir);
            FileFolderHelper.StringToFile(outputF2, "../output3_F2.tab");
            
        }

        private string DirectoryToString(string dir)
        {
            StringBuilder sb = new StringBuilder();
            string filepath = client + dir;
            DirectoryInfo di = new DirectoryInfo(filepath);

            foreach (FileInfo fi in di.GetFiles("*.mtl", SearchOption.TopDirectoryOnly))
            {
                sb.Append(fi.Name.Replace(".mtl", string.Empty));
                sb.Append("\r\n");
            }

            return sb.ToString();
        }

        private void OutputRestMtl3(ref StringBuilder sb)
        {
            // 输出m3中没用到的
            sb.Append("\r\n\r\n============================================================\r\n\r\n");

            ArrayList aKeys = new ArrayList();
            foreach (DictionaryEntry de in mtl3)
            {
                if (!(bool)de.Value)
                {
                    aKeys.Add(de.Key);
                }
            }
            aKeys.Sort();
            foreach (string key in aKeys)
            {
                sb.Append(key); sb.Append("\r\n");
            }
        }
    }
}
