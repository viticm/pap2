using System;
using System.Collections.Generic;
using System.Text;
using DevComponents.DotNetBar;
using System.Windows.Forms;
using System.Data;
using System.Data.SqlClient;
namespace GameDesingerTools
{
    /// <summary>
    /// 脚本调用的都放这里来
    /// </summary>
    public class LuaCallBack
    {
        private TreeView mBaseTree = null;
        private SqlConnection mConn = null;
        public LuaCallBack(TreeView basetree)
        {
            mBaseTree = basetree;
            mConn = MainForm.conn;
        }

        public void MatchSkill()
        {
            string filter = "IsActorSkill = 1";
            DataTable table = Helper.GetDataTableProxy("tbl_skills", filter, null, mConn);
            int nf1 = 0;
            int nf2 = 0;
            int nm2 = 0;
            foreach (DataRow row in table.Rows)
            {
                filter = string.Format("SkillID = '{0}'", row["SkillID"]);
                DataTable tblf1 = Helper.GetDataTableProxy("skill_caster_f1", filter, null, mConn);
                string sqlf1 = string.Format("select * from {0} where {1}", "skill_caster_f1", filter);

                DataTable tblf2 = Helper.GetDataTableProxy("skill_caster_f2", filter, null, mConn);
                string sqlf2 = string.Format("select * from {0} where {1}", "skill_caster_f2", filter);

                DataTable tblm2 = Helper.GetDataTableProxy("skill_caster_m2", filter, null, mConn);
                string sqlm2 = string.Format("select * from {0} where {1}", "skill_caster_m2", filter);

                if (tblf1.Rows.Count == 0)
                {
                    DataRow rowf1 = tblf1.NewRow();
                    rowf1["SkillID"] = row["SkillID"];
                    rowf1["SkillLevel"] = 1;
                    rowf1["CastSkillAnimationID0"] = 0;
                    tblf1.Rows.Add(rowf1);
                    Helper.SaveTable(tblf1, sqlf1, mConn);
                    nf1++;
                }
                if (tblf2.Rows.Count == 0)
                {
                    DataRow rowf2 = tblf2.NewRow();
                    rowf2["SkillID"] = row["SkillID"];
                    rowf2["SkillLevel"] = 1;
                    rowf2["CastSkillAnimationID0"] = 0;
                    tblf2.Rows.Add(rowf2);
                    Helper.SaveTable(tblf2, sqlf2, mConn);
                    nf2++;
                }
                if (tblm2.Rows.Count == 0)
                {
                    DataRow rowm2 = tblm2.NewRow();
                    rowm2["SkillID"] = row["SkillID"];
                    rowm2["SkillLevel"] = 1;
                    rowm2["CastSkillAnimationID0"] = 0;
                    tblm2.Rows.Add(rowm2);
                    Helper.SaveTable(tblm2, sqlm2, mConn);
                    nm2++;
                }
            }

            string message = string.Format("skill_caster_f1增加了{0}行。\r\n skill_caster_f2增加了{1}行。\r\n skill_caster_m2增加了{2}行。", nf1, nf2, nm2);
            MessageBox.Show(message);

        }

        public void MatchSkill(int skillid)
        {
            string filter = string.Format("SkillID = '{0}'", skillid);
            DataTable tblf1 = Helper.GetDataTableProxy("skill_caster_f1", filter, null, mConn);
            string sqlf1 = string.Format("select * from {0} where {1}", "skill_caster_f1", filter);

            DataTable tblf2 = Helper.GetDataTableProxy("skill_caster_f2", filter, null, mConn);
            string sqlf2 = string.Format("select * from {0} where {1}", "skill_caster_f2", filter);

            DataTable tblm2 = Helper.GetDataTableProxy("skill_caster_m2", filter, null, mConn);
            string sqlm2 = string.Format("select * from {0} where {1}", "skill_caster_m2", filter);

            if (tblf1.Rows.Count == 0)
            {
                DataRow rowf1 = tblf1.NewRow();
                rowf1["SkillID"] = skillid;
                rowf1["SkillLevel"] = 1;
                rowf1["CastSkillAnimationID0"] = 0;
                tblf1.Rows.Add(rowf1);
                Helper.SaveTable(tblf1, sqlf1, mConn);
            }
            if (tblf2.Rows.Count == 0)
            {
                DataRow rowf2 = tblf2.NewRow();
                rowf2["SkillID"] = skillid;
                rowf2["SkillLevel"] = 1;
                rowf2["CastSkillAnimationID0"] = 0;
                tblf2.Rows.Add(rowf2);
                Helper.SaveTable(tblf2, sqlf2, mConn);
            }
            if (tblm2.Rows.Count == 0)
            {
                DataRow rowm2 = tblm2.NewRow();
                rowm2["SkillID"] = skillid;
                rowm2["SkillLevel"] = 1;
                rowm2["CastSkillAnimationID0"] = 0;
                tblm2.Rows.Add(rowm2);
                Helper.SaveTable(tblm2, sqlm2, mConn);
            }
        }

        public void MatchSubSkill()
        {
            int n = 0;
            string filter = string.Format("SubSkillIDs <> '' and SubSkillIDs is not NULL");
            DataTable skillstbl = Helper.GetDataTableProxy("tbl_skills", filter, null, mConn);
            foreach (DataRow row in skillstbl.Rows)
            {
                string subskillstr = row["SubSkillIDs"] as string;
                int skillid = (int)row["SkillID"];
                n += MatchSubSkill(skillid, subskillstr);
            }

            string message = string.Format("一共增加了{0}行数据。", n);
            MessageBox.Show(message);
        }

        public int MatchSubSkill(int skillid, string subskillstr)
        {
            int n = 0;
            string[] subskills = subskillstr.Split(new char[] { ',' });
            string where = string.Format("SkillID = {0}", skillid);
            string sql = string.Format("select * from dic_npc_skill_caster_npc where {0}", where);
            DataTable tbl = Helper.GetDataTableProxy("dic_npc_skill_caster_npc", where, null, mConn);
            foreach (DataRow row in tbl.Rows)
            {
                string RepresentID = row["RepresentID"] as string;
                foreach (string subskill in subskills)
                {
                    string sfilter = string.Format("RepresentID = {0} and SkillID = {1}", RepresentID, subskill.Trim());
                    string subsql = string.Format("select * from dic_npc_skill_caster_npc where {0}", sfilter);
                    DataTable subtbl = Helper.GetDataTableProxy("dic_npc_skill_caster_npc", sfilter, null, mConn);
                    if (subtbl.Rows.Count == 0)
                    {
                        DataRow newrow = subtbl.NewRow();
                        newrow["RepresentID"] = int.Parse(RepresentID);
                        newrow["SkillID"] = int.Parse(subskill.Trim());
                        newrow["SkillLevel"] = 1;
                        newrow["CastSkillAnimationID0"] = 0;
                        subtbl.Rows.Add(newrow);
                        Helper.SaveTable(subtbl, subsql, mConn);
                        n++;
                    }
                }
            }
            return n;
        }
    }
}
