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
        private bool Fill_Other()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_other", Conn);

            // 填入
            DataRow defaultrow = TblOther.Rows[0];
            foreach (DataRow row in TblOther.Rows)
            {
                // 类型
                string detailtype = row["_CATEGORY"].ToString().Trim();
                if (!CategoryRequired(detailtype))
                    continue;

                // id
                int id = Convert.ToInt32(row["ID"]);

                // 图标
                int uiid = Convert.ToInt32(row["UiID"] == DBNull.Value ? defaultrow["UiID"] : row["UiID"]);
                string icon = GetItemIconName(uiid);

                // 名字
                string name = row["Name"].ToString().Trim();

                // 品质
                int q = Convert.ToInt32(row["Quality"] == DBNull.Value ? defaultrow["Quality"] : row["Quality"]);
                string quality = GetQuality(q);

                // 等级
                int level = Convert.ToInt32(row["UiID"] == DBNull.Value ? defaultrow["UiID"] : row["UiID"]);

                // 需求等级
                int requirelevel = row["RequireLevel"] != DBNull.Value ? Convert.ToInt32(row["RequireLevel"]) : 0;

                // 绑定类型
                int b = Convert.ToInt32(row["BindType"] == DBNull.Value ? defaultrow["BindType"] : row["BindType"]);
                string bindtype = GetBindType(b);

                // 武器来源
                string source = string.Empty;

                // 卖店价格
                string price = row["Price"].ToString().Trim();

                // 最大耐久度
                int maxDurability = Convert.ToInt32(row["MaxDurability"] == DBNull.Value ? defaultrow["MaxDurability"] : row["MaxDurability"]);

                // 插入一条quest的数据
                string sql = string.Format("insert into web_other values('{0}', {1}, '{2}', '{3}', {4}, {5}, '{6}', '{7}', '{8}', {9}, {10})",
                    icon, id, name, quality, level, requirelevel, bindtype, detailtype, source, price, maxDurability);
                Helper.ExecSqlCmd(sql, Conn);
            }


            return true;
        }

        public bool CategoryRequired(string cat)
        {
            if (cat == "缝纫材料" || cat == "缝纫配方" || cat == "缝纫产物" ||
                cat == "烹饪材料" || cat == "烹饪配方" || cat == "烹饪产物" ||
                cat == "医术材料" || cat == "医术配方" || cat == "医术产物" ||
                cat == "铸造材料" || cat == "铸造配方" || cat == "铸造产物")
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
