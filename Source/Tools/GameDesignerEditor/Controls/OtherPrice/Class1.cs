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
using LuaInterface;

namespace OtherPrice
{
    public class Class1
    {
        private SqlConnection Conn = null;
        private DataRow m_curRow = null;

        public Class1(object extraMenuItem, SqlConnection conn)
        {
            Conn = conn;
            ButtonItem[] extraItem = extraMenuItem as ButtonItem[];
            if (extraItem == null) return;
            extraItem[0].BeginGroup = true;
            extraItem[0].Visible = true;
            extraItem[0].Name = "UpdateAllPrice";
            extraItem[0].Text = "更新所有道具价格(脚本)";
            extraItem[0].Tag = "这将更新整个 Other 表。\r\n\r\n你确定要继续吗？";

            extraItem[1].Visible = true;
            extraItem[1].Name = "UpdateAllPriceTbl";
            extraItem[1].Text = "更新所有道具价格(读表)";
            extraItem[1].Tag = "这将更新整个 Other 表。\r\n\r\n你确定要继续吗？";
        }

        public void UpdateAllPriceTbl()
        {
            try
            {
                int cannotfindcount = 0;
                string cannotfindmsg = "\r\n\r\n找不到以下材料：\r\n\r\n";
                System.Data.DataTable tblPrice = GetDataTable("SELECT * FROM CraftMaterialPrice", Conn);
                System.Data.DataTable tblOther = GetDataTable("SELECT * FROM Other", Conn);
                foreach (DataRow row in tblPrice.Rows)
                {
                    int craftprice = Convert.ToInt32(row["Price"]);
                    int craftid = Convert.ToInt32(row["ID"]);
                    string craftname = row["Name"].ToString();
                    DataRow rowOther = tblOther.Rows.Find(craftid);
                    if (rowOther == null)
                    {
                        string strTmp = string.Format("{0}    {1}    {2}\r\n", craftname, craftid, craftprice);
                        cannotfindmsg += strTmp;
                        cannotfindcount++;
                    }
                    else
                    {
                        rowOther["Price"] = craftprice;
                    }
                }

                SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM Other", Conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                int val = adp.Update(tblOther);
                tblOther.AcceptChanges();

                cannotfindmsg = cannotfindcount > 0 ? cannotfindmsg : string.Empty;
                MessageBox.Show("更新Other表成功。" + cannotfindmsg, "提示");
            }
            catch (Exception ex)
            {
                MessageBox.Show("更新Other表失败！\r\n错误信息为：" + ex.Message);
            }
        }

        public void UpdateAllPrice()
        {
            LuaEx lua = new LuaEx();
            lua["Conn"] = Conn;
            lua.RegisterFunction("GetDbRowField", this, typeof(Class1).GetMethod("GetDbRowField"));
            lua.RegisterFunction("SetDbRowField", this, typeof(Class1).GetMethod("SetDbRowField"));
            lua.DoString("function trace(txt)\r\n    MainForm.LogEditBox.Visible = true\r\n    MainForm.LogEditBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n' \r\n end");
            try
            {
                string luafile = Application.StartupPath + @"\OtherTblUpdatePrice.lua";
                if (!File.Exists(luafile))
                    return;
                lua.DoFile(luafile);
                LuaFunction fun = lua.GetFunction("UpdateAllPrice");
                if (fun == null)
                    return;

                DataTable tblOther = GetDataTable("SELECT * FROM Other", Conn);

                for (int i = 0; i < tblOther.Rows.Count; i++)
                {
                    m_curRow = tblOther.Rows[i];
                    fun.Call();
                }

                SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM Other", Conn);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                int val = adp.Update(tblOther);
                tblOther.AcceptChanges();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Other更新错误。\r\n\r\n" + ex.Message);
            }   
        }

        public static System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        public string GetDbRowField(string fieldname)
        {
            return m_curRow[fieldname].ToString();
        }

        public void SetDbRowField(string fieldname, object _value)
        {
            m_curRow[fieldname] = _value.ToString();
        }
    }
}
