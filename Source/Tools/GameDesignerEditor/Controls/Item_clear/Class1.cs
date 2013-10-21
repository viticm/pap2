using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using DevComponents.DotNetBar;

namespace Item_clear
{
    public class Class1
    {
        private SqlConnection Conn = null;

        public Class1(object extraMenuItem, SqlConnection conn)
        {
            Conn = conn;
            ButtonItem[] extraItem = extraMenuItem as ButtonItem[];
            if (extraItem == null) return;
            extraItem[0].BeginGroup = true;
            extraItem[0].Visible = true;
            extraItem[0].Name = "ClearNotUsingItem";
            extraItem[0].Text = "删除冗余数据";
            extraItem[0].Tag = "这将扫描整个 tbl_icon_item( item.txt ) 表，并在数据库中删除冗余数据( item_Custom_Armor, item_Custom_Weapon, Custom_Trinket, Other 四个表都没有引用到的行 )。\r\n\r\n你确定要继续吗？";

            extraItem[1].BeginGroup = true;
            extraItem[1].Visible = true;
            extraItem[1].Name = "ExportRequirement";
            extraItem[1].Text = "导出需求表到本地";
            extraItem[1].Tag = "这将生成 /Client/UI/Scheme/Case/IconSourceRequirement.tab 文件。\r\n\r\n你确定要继续吗？";
        }

        public void ClearItemNotUsing()
        {
            string sql = @"SELECT * FROM tbl_icon_item WHERE
                (ItemID NOT IN (SELECT UiID FROM Other              WHERE UiID IS NOT NULL)) AND
                (ItemID NOT IN (SELECT UiID FROM item_Custom_Armor  WHERE UiID IS NOT NULL)) AND
                (ItemID NOT IN (SELECT UiID FROM item_Custom_Weapon WHERE UiID IS NOT NULL)) AND
                (ItemID NOT IN (SELECT UiID FROM Custom_Trinket     WHERE UiID IS NOT NULL))";
            PreviewForm pf = new PreviewForm(Conn, sql);
            pf.ShowDialog();
        }

        public void ExportRequirement(string fileName)
        {
            string sql = "SELECT * FROM tbl_icon_item WHERE IconID = 572";
            DataTable tbl = GetDataTable(sql);

            StringBuilder sb = new StringBuilder();
            sb.Append("分类\t图标名\t描述\t需求描述\r\n");

            foreach (DataRow row in tbl.Rows)
            {
                string ItemID = row["ItemID"].ToString();
                if (ItemID == "0") continue;

                string _s = string.Format("SELECT * FROM Other WHERE UiID = {0}", ItemID);
                DataTable tbl_Other = GetDataTable(_s);
                string Cat = string.Empty;
                if (tbl_Other.Rows.Count > 0)
                {
                    Cat = tbl_Other.Rows[0]["_CATEGORY"].ToString();
                }

                string Name = row["Name"].ToString().Trim();
                string Desc = row["Desc"].ToString().Trim();
                string Requirement = row["Requirement"].ToString().Trim();

                string sOneLine = string.Format("{0}\t{1}\t{2}\t{3}\r\n", Cat, Name, Desc, Requirement);
                sb.Append(sOneLine);
            }

            StringToFile(sb.ToString(), fileName);
            MessageBox.Show("成功！");
        }

        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }

        // 保存string到文件
        static public void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }
        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                CreatePath(strFileName);
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }
        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }
    }
}
