using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace OfficialWebDB
{
    public partial class Form1
    {
        private bool Fill_Doodad()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_doodad", Conn);

            // 填入
            DataRow defaultrow = TblDoodadTemplate.Rows[0];
            foreach (DataRow row in TblDoodadTemplate.Rows)
            {
                // id
                int id = Convert.ToInt32(row["ID"]);

                // 名称
                string name = row["Name"].ToString().Trim();

                // 所在场景
                string mapname = row["MapName"].ToString().Trim();

                if (mapname != "阅读")
                    continue;

                // 需求等级
                int requireLevel = 0;

                // 插入一条quest的数据
                string sql = string.Format("insert into web_doodad values({0}, '{1}', '{2}', {3})",
                    id, name, mapname, requireLevel);
                Helper.ExecSqlCmd(sql, Conn);

            }
            return true;
        }
    }
}
