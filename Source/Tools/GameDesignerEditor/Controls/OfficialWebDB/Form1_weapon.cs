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
        private bool Fill_Weapon()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_weapon", Conn);

            // 填入
            DataRow defaultrow = TblWeapon.Rows[0];
            foreach (DataRow row in TblWeapon.Rows)
            {
                // npc_id
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
                int dt = Convert.ToInt32(row["DetailType"] == DBNull.Value ? defaultrow["DetailType"] : row["DetailType"]);
                string detailtype = GetDetailType(dt);

                // 武器来源
                //string source = row["GetType"].ToString().Trim();
                string magicType = row["MagicType"].ToString().Trim();
                string belongSchool = row["BelongSchool"].ToString().Trim();
                string getType = row["GetType"].ToString().Trim();
                string belongMap = row["BelongMap"].ToString().Trim();

                // 卖店价格
                string price = IsLogicallyNull(row["Price"]) ? "0" : MoneyToString(Convert.ToInt32(row["Price"]));

                // 最大耐久度
                int maxDurability = Convert.ToInt32(row["MaxDurability"] == DBNull.Value ? defaultrow["MaxDurability"] : row["MaxDurability"]);

                // 装备属性
                string attrib = GetEquipDesc(row);

                // 插入一条quest的数据
                string sql = string.Format("insert into web_weapon values('{0}', {1}, '{2}', '{3}', {4}, {5}, '{6}', '{7}', '{8}', '{9}', '{10}', '{11}', '{12}', {13}, '{14}')",
                    icon, id, name, quality, level, requirelevel, bindtype, detailtype, magicType, belongSchool, getType, belongMap, price, maxDurability, attrib);
                Helper.ExecSqlCmd(sql, Conn);
            }


            return true;
        }

        private string GetQuality(int quality)
        {
            string[] qualities = {
                "无效", "白装", "绿装", "蓝装", "紫装", "橙装"
            };
            return qualities[quality];
        }

        private string GetBindType(int bindtype)
        {
            string[] bindtypes = {
                "无效", "不绑定", "装备后绑定", "拾取后绑定"
            };
            return bindtypes[bindtype];
        }

        private string GetDetailType(int detailtype)
        {
            string[] detailtypes = {
                "棍", "枪", "剑", "拳套",
                "双兵", "笔", "钝器", "锐器",
                "机关暗器", "新建"
            };
            return detailtypes[detailtype];
        }
    }
}
