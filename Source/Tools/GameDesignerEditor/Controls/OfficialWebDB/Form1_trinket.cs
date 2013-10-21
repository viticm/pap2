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
        private bool Fill_Trinket()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_trinket", Conn);

            // 填入
            DataRow defaultrow = TblTrinket.Rows[0];
            foreach (DataRow row in TblTrinket.Rows)
            {
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
                int level = Convert.ToInt32(row["Level"] == DBNull.Value ? defaultrow["Level"] : row["Level"]);

                // 需求等级
                int requirelevel = Convert.ToInt32(row["Require1Value"] == DBNull.Value ? defaultrow["Require1Value"] : row["Require1Value"]);

                // 绑定类型
                int b = Convert.ToInt32(row["BindType"] == DBNull.Value ? defaultrow["BindType"] : row["BindType"]);
                string bindtype = GetBindType(b);

                // 类型
                int st = Convert.ToInt32(row["SubType"] == DBNull.Value ? defaultrow["SubType"] : row["SubType"]);
                string detailtype = GetTrinketType(st);

                // 饰品来源
                //string source = row["GetType"].ToString().Trim();
                string magicKind = row["MagicKind"].ToString().Trim();
                string magicType = row["GetType"].ToString().Trim();
                string belongSchool = row["BelongSchool"].ToString().Trim();
                string getType = row["GetType"].ToString().Trim();
                string belongMap = row["BelongMap"].ToString().Trim();

                // 卖店价格
                string price = IsLogicallyNull(row["Price"]) ? "0" : MoneyToString(Convert.ToInt32(row["Price"]));

                // 最大耐久度
                int maxDurability = Convert.ToInt32(row["MaxDurability"] == DBNull.Value ? defaultrow["MaxDurability"] : row["MaxDurability"]);

                // 饰品属性
                string attrib = GetEquipDesc(row);

                // 插入一条quest的数据
                string sql = string.Format("insert into web_trinket values('{0}', {1}, '{2}', '{3}', {4}, {5}, '{6}', '{7}', '{8}', '{9}', '{10}', '{11}', '{12}', '{13}', {14}, '{15}')",
                    icon, id, name, quality, level, requirelevel, bindtype, detailtype, magicKind, magicType, belongSchool, getType, belongMap, price, maxDurability, attrib);
                Helper.ExecSqlCmd(sql, Conn);
            }


            return true;
        }

        private string GetTrinketType(int subtype)
        {
            string[] subtypes = {
                "近程武器", "远程武器", "上衣", "头部",
                "项链", "戒指", "腰带", "腰缀",
                "裤子", "鞋子", "护臂", "腰部扩展",
                "包裹", "暗器弹药", "背部扩展", "坐骑",
                "新建"
            };
            return subtypes[subtype];
        }
    }
}
