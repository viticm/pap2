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
        private bool Fill_Skill()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_skills", Conn);

            // 填入
            DataRow defaultrow = TblSkills.Rows[0];
            foreach (DataRow row in TblSkills.Rows)
            {
                // skill_id
                int skillid = Convert.ToInt32(row["SkillID"]);

                // skill学习等级
                int skilllevel = GetSkillLearnLevel(skillid);
                if (skilllevel > 70)
                    continue;
                if (skilllevel == -1)
                    skilllevel = 0;

                // skill名称
                string skillname = row["SkillName"].ToString().Trim();

                // skill图标名 招式描述 招式纯描述
                string desc = string.Empty, specialDesc = string.Empty;
                string skillicon = GetSkillIconName(skillid, ref desc, ref specialDesc);

                // skill 重数
                int maxlevel = Convert.ToInt32(row["MaxLevel"] == DBNull.Value ? defaultrow["MaxLevel"] : row["MaxLevel"]);

                // 武器需求
                int weaponRequest = Convert.ToInt32(row["WeaponRequest"] == DBNull.Value ? defaultrow["WeaponRequest"] : row["WeaponRequest"]);
                string weapon = GetSkillWeapon(weaponRequest);

                // 装配内功
                int mounttype = Convert.ToInt32(row["MountRequestType"] == DBNull.Value ? defaultrow["MountRequestType"] : row["MountRequestType"]);
                string mountname = GetMountName(mounttype);

                // 所属门派
                int belongschool = Convert.ToInt32(row["BelongSchool"] == DBNull.Value ? defaultrow["BelongSchool"] : row["BelongSchool"]);
                string strSchool = GetSchoolName(belongschool);

                // 插入一条quest的数据
                string sql = string.Format("insert into web_skills values('{0}', {1}, '{2}', '{3}', {4}, {5}, {6}, {7}, '{8}', {9}, {10}, '{11}', '{12}', '{13}')",
                    skillicon, skillid, strSchool, skillname, skilllevel, maxlevel, 0, 0, weapon, 0, 0, mountname, desc, specialDesc);
                Helper.ExecSqlCmd(sql, Conn);
            }

            return true;
        }

        private string GetSchoolName(int belongschool)
        {
            string sql = string.Format("select name from dic_school where schoolid={0}", belongschool);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            if (tbl.Rows.Count <= 0)
                return string.Empty;
            else
                return tbl.Rows[0][0].ToString().Trim();
        }

        private string GetMountName(int mounttype)
        {
            string[] strMounts = {
                "无", "天策", "万花", "纯阳", "七秀", "少林", "藏剑", "丐帮", "明教", "五毒", "唐门", "江湖"
            };
            return strMounts[mounttype];
        }

        private int GetSkillLearnLevel(int skillid)
        {
            string sql = string.Format("select top 1 RequirePlayerLevel from skill_learn where skillid={0}", skillid);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            if (tbl.Rows.Count <= 0)
                return -1;
            else
                return Convert.ToInt32(tbl.Rows[0][0]);
        }

        private string GetSkillIconName(int skillid, ref string desc, ref string specialDesc)
        {
            string sql = string.Format("select top 1 iconid, [desc], specialdesc from ui_skill where skillid={0}", skillid);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            if (tbl.Rows.Count <= 0)
                return string.Empty;
            else
            {
                desc = tbl.Rows[0]["Desc"].ToString().Trim();
                specialDesc = tbl.Rows[0]["SpecialDesc"].ToString().Trim();
                
                sql = string.Format("select filename from icon where id={0}", Convert.ToInt32(tbl.Rows[0]["IconID"]));
                DataTable tbl2 = Helper.GetDataTable(sql, Conn);
                if (tbl2.Rows.Count <= 0)
                    return string.Empty;
                else
                {
                    string filename = tbl2.Rows[0][0].ToString().Trim();
                    return filename.Replace(".UITex", ".gif");
                }
            }
        }

        private string GetSkillWeapon(int weaponRequest)
        {
            string[] strWeapons = {
                "不限", "棍", "枪", "剑", "拳套", "双兵", "笔", "暗器"
            };
            return strWeapons[weaponRequest];
        }
    }
}
