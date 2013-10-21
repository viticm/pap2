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
    public partial class Form1 : Form
    {
        private SqlConnection Conn;
        private DataTable TblQuests;
        private DataTable TblNpcTemplate;
        private DataTable TblDoodadTemplate;
        private DataTable TblOther;
        private DataTable TblWeapon;
        private DataTable TblArmor;
        private DataTable TblTrinket;
        private DataTable TblSkills;
        private DataTable TblForces;
        private DataTable TblBook;
        private DataTable TblSkillAttrib;
        private DataTable TblSkillAttribMagic;

        private string RootDir = @"J:\Client";

        public Form1()
        {
            InitializeComponent();

            InitDB();

            GenerateDB();
        }

        private bool InitDB()
        {
            string sconn = "Server = 192.168.27.31; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            Conn = new SqlConnection(sconn);
            TblQuests = Helper.GetDataTable("select * from tbl_quests order by questid", Conn);
            TblNpcTemplate = Helper.GetDataTable("select * from npctemplate order by id", Conn);
            TblDoodadTemplate = Helper.GetDataTable("select * from tbl_doodad order by id", Conn);
            TblOther = Helper.GetDataTable("select * from other order by _category,id", Conn);
            TblWeapon = Helper.GetDataTable("select * from item_custom_weapon order by quality,subtype,detailtype", Conn);
            TblArmor = Helper.GetDataTable("select * from item_custom_armor order by gettype,belongmap,subtype", Conn);
            TblTrinket = Helper.GetDataTable("select * from custom_trinket order by id", Conn);
            TblSkills = Helper.GetDataTable("select * from tbl_skills order by skillid", Conn);
            TblForces = Helper.GetDataTable("select * from dic_npc_relation_force order by id", Conn);
            TblBook = Helper.GetDataTable("select * from book order by id,subid", Conn);
            TblSkillAttrib = Helper.GetDataTable("select * from skill_attribute order by attribute", Conn);
            TblSkillAttribMagic = Helper.GetDataTable("select * from skill_attrib_magic order by id", Conn);

            return true;
        }

        private bool GenerateDB()
        {
            //Fill_Quests();

            Fill_Npc();

            //Fill_Skill();

           // Fill_Weapon();

           // Fill_Armor();

           // Fill_Trinket();

            //Fill_Other();

            //Fill_Doodad();

            //Fill_Book();

            return true;
        }


        private string MoneyToString(int iMoney)
        {
            int iGold = iMoney / 10000;
            iMoney -= iGold * 10000;
            int iSilver = iMoney / 100;
            int iCopper = iMoney - iSilver * 100;
            string str = string.Empty;
            if (iGold > 0) str += iGold.ToString() + "金";
            if (iSilver > 0) str += iSilver.ToString() + "银";
            if (iCopper > 0) str += iCopper.ToString() + "铜";
            return str;
        }
        private string GetNpcNameByID(int npctemplateid)
        {
            object[] keys = new object[1];
            keys[0] = npctemplateid;
            if (TblNpcTemplate.Rows.Find(keys) != null)
                return TblNpcTemplate.Rows.Find(keys)["Name"].ToString().Trim();
            else
                return "<非法NPC>";
        }
        private string GetDoodadNameByID(int doodadtemplateid)
        {
            object[] keys = new object[1];
            keys[0] = doodadtemplateid;
            if (TblDoodadTemplate.Rows.Find(keys) != null)
                return TblDoodadTemplate.Rows.Find(keys)["Name"].ToString().Trim();
            else
                return "<非法Doodad>";
        }
        private string GetItemNameByTypeIndex(int type, int index, ref int count)
        {
            object[] keys = new object[1];
            keys[0] = index;

            string strResult = string.Empty;

            if (type == 5)
                if (TblOther.Rows.Find(keys) != null)
                    strResult = TblOther.Rows.Find(keys)["Name"].ToString().Trim();
                else
                    return "<非法物品>";
            else if (type == 6)
                if (TblWeapon.Rows.Find(keys) != null)
                    strResult = TblWeapon.Rows.Find(keys)["Name"].ToString().Trim();
                else
                    return "<非法物品>";
            else if (type == 7)
                if (TblArmor.Rows.Find(keys) != null)
                    strResult = TblArmor.Rows.Find(keys)["Name"].ToString().Trim();
                else
                    return "<非法物品>";
            else if (type == 8)
                if (TblTrinket.Rows.Find(keys) != null)
                    strResult = TblTrinket.Rows.Find(keys)["Name"].ToString().Trim();
                else
                    return "<非法物品>";
            else
                throw new Exception("未识别的StartItemType！");

            // 找书名
            if (strResult == "书" && count != 0)
            {
                int bookid = count / 8 + 1;
                int booksubid = count % 8 + 1;
                object[] bookkey = new object[2];
                bookkey[0] = bookid;
                bookkey[1] = booksubid;

                if (TblBook.Rows.Find(bookkey) != null)
                    strResult = TblBook.Rows.Find(bookkey)["Name"].ToString().Trim();
                else
                    strResult = "<非法书名>";

                count = 1;
            }

            return strResult;
        }
        private string GetSkillNameByID(int skillid)
        {
            object[] keys = new object[1];
            keys[0] = skillid;
            if (TblSkills.Rows.Find(keys) != null)
                return TblSkills.Rows.Find(keys)["SkillName"].ToString().Trim();
            else
                return "<非法技能>";
        }
        private string GetQuestNameByID(int questid)
        {
            object[] keys = new object[1];
            keys[0] = questid;
            if (TblQuests.Rows.Find(keys) != null)
                return TblQuests.Rows.Find(keys)["QuestName"].ToString().Trim();
            else
                return "<非法任务>";
        }
        private string GetForceNameByID(int forceid)
        {
            object[] keys = new object[1];
            keys[0] = forceid;
            if (TblForces.Rows.Find(keys) != null)
                return TblForces.Rows.Find(keys)["Name"].ToString().Trim();
            else
                return "<非法势力>";
        }
        private string GetItemIconName(int uiid)
        {
            string sql = string.Format("select iconid from tbl_icon_item where itemid={0}", uiid);
            DataTable tbl_item = Helper.GetDataTable(sql, Conn);
            if (tbl_item.Rows.Count <= 0)
                return string.Empty;
            else
            {
                int iconid = Convert.ToInt32(tbl_item.Rows[0]["IconID"]);
                string sql_2 = string.Format("select filename from icon where id={0}", iconid);
                DataTable tbl_icon = Helper.GetDataTable(sql_2, Conn);
                if (tbl_icon.Rows.Count <= 0)
                    return string.Empty;
                else
                {
                    string filename = tbl_icon.Rows[0]["FileName"].ToString().Trim();
                    return filename.Replace(".UITex", ".gif");
                }
            }
        }
        private string GetEquipDesc(DataRow row)
        {
            // 基础属性
            string desc = string.Empty;

            string base1type = row["Base1Type"].ToString().Trim();
            int base1 = IsLogicallyNull(row["Base1Min"]) ? 0 : Convert.ToInt32(row["Base1Min"]);
            string base2type = row["Base2Type"].ToString().Trim();
            int base2 = IsLogicallyNull(row["Base2Min"]) ? 0 : Convert.ToInt32(row["Base2Min"]);
            string base3type = row["Base3Type"].ToString().Trim();
            int base3 = IsLogicallyNull(row["Base3Min"]) ? 0 : Convert.ToInt32(row["Base3Min"]);

            if (base1type.ToLower() == "atmeleeweapondamagebase" && base2type.ToLower() == "atmeleeweapondamagerand")
            {
                desc += string.Format("近战伤害提高{0}-{1}; ", base1, base1 + base2);
            }
            else if (base1type.ToLower() == "atrangeweapondamagebase" && base2type.ToLower() == "atrangeweapondamagerand")
            {
                desc += string.Format("远程伤害提高{0}-{1}; ", base1, base1 + base2);
            }
            else if (base1type.ToLower() == "atphysicsshieldbase")
            {
                desc += string.Format("外功防御力+{0};", base1);
            }
            else
            {
                object[] keys = new object[1];
                keys[0] = base1type;
                DataRow r = TblSkillAttrib.Rows.Find(keys);
                if (r != null && base1 != 0)
                    desc += r["Name"].ToString().Trim() + "+" + base1.ToString() + "; ";

                keys[0] = base2type;
                r = TblSkillAttrib.Rows.Find(keys);
                if (r != null && base2 != 0)
                    desc += r["Name"].ToString().Trim() + "+" + base2.ToString() + "; ";
            }

            if (base3type.ToLower() == "atmeleeweaponattackspeedbase" || base3type.ToLower() == "atrangeweaponattackspeedbase")
            {
                double fSpeed = Math.Round((double)base3 / 16, 1);
                desc += string.Format("速度: {0}; ", fSpeed);
                desc += string.Format("秒伤: {0}; ", Math.Round((double)(base1 * 2 + base2) / 2 / fSpeed, 1));
            }
            else
            {
                object[] keys = new object[1];
                keys[0] = base3type;
                DataRow r = TblSkillAttrib.Rows.Find(keys);
                if (r != null && base3 != 0)
                    desc += r["Name"].ToString().Trim() + "+" + base3.ToString() + "; ";
            }

            // 魔法属性
            for (int i = 1; i <= 8; i++)
            {
                string col_magictype = string.Format("Magic{0}Type", i);
                int magictype = IsLogicallyNull(row[col_magictype]) ? 0 : Convert.ToInt32(row[col_magictype]);
                if (magictype != 0)
                {
                    object[] keys = new object[1];
                    keys[0] = magictype;
                    DataRow r = TblSkillAttribMagic.Rows.Find(keys);
                    if (r != null)
                    {
                        string basetype = r["ModifyType"].ToString().Trim();
                        int param1min = IsLogicallyNull(r["Param1Min"]) ? 0 : Convert.ToInt32(r["Param1Min"]);
                        int param1max = IsLogicallyNull(r["Param1Max"]) ? 0 : Convert.ToInt32(r["Param1Max"]);
                        object[] ks = new object[1];
                        ks[0] = basetype;
                        DataRow _r = TblSkillAttrib.Rows.Find(ks);
                        if (_r != null)
                            desc += _r["Name"].ToString().Trim() + "+" + param1min.ToString() + "; ";
                    }
                }
            }

            return desc;
        }


        public static bool IsLogicallyNull(object DbField)
        {
            if (DbField == null || DbField == DBNull.Value || DbField.ToString().Trim() == string.Empty || DbField.ToString().Trim() == "0") // 可以认为是空的数据
                return true;
            else
                return false;
        }

        private static bool IsLogicallyEqual(DataRow row, string field1, string field2)
        {
            if (row[field1].ToString().Trim() == row[field2].ToString().Trim())
                return true;
            else
                return false;
        }
    }
}