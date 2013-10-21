using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;
using System.Collections;

namespace ExportLearnSkill
{
    public class Class1
    {
        private SqlConnection Conn;
        private ArrayList ForceIDs = new ArrayList();

        public Class1()
        {

        }

        public bool Export(string rootdir, SqlConnection conn)
        {
            Conn = conn;

            // 清空数据表skill_learn
            Conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                string sql = "DELETE FROM skill_learn";
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                string err = string.Format("在将表 skill_learn 清空数据时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                Helper.outputDebugStringError(err);
            }

            // 导入各个force的skilllearn tab文件
            try
            {
                string strRelationForceFile = rootdir + "/settings/RelationForce.tab";
                string contentRelationForce = FileFolderHelper.FileToString(strRelationForceFile);
                string[] rows = contentRelationForce.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
                int i = 0;
                foreach (string row in rows)
                {
                    if (i == 0)
                    {
                        i++;
                        continue;
                    }

                    string[] cols = row.Split(new char[] { '\t' });
                    if (cols.Length < 3) continue;

                    string force = cols[0];
                    // 我只找玩家的forceid
                    if (!force.EndsWith("·玩家") && force != "玩家初始") continue;
                    force = force.Replace("·玩家", string.Empty);

                    int forceid = Convert.ToInt32(cols[1]);

                    string strSchoolTabFile = string.Empty;
                    if (forceid == 0)
                    {
                        strSchoolTabFile = rootdir + "/settings/skill/SkillLearning/扬州.tab";
                    }
                    else
                    {
                        strSchoolTabFile = rootdir + "/settings/skill/SkillLearning/" + force + ".tab";
                    }

                    FileInfo fi = new FileInfo(strSchoolTabFile);
                    if (!fi.Exists)
                        continue;

                    ForceIDs.Add(forceid);
                    ImportSchoolTab(strSchoolTabFile, forceid);
                }
            }
            catch (Exception ex)
            {
                string err = string.Format("导入各个force的skilllearn tab文件时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                Helper.outputDebugStringError(err);
            }
            Conn.Close();


            // 根据数据库导出客户端ui所需表
            StringBuilder sb = new StringBuilder();
            sb.Append("level\tschool\tskills\r\n0\t0\r\n");

            for (int level = 0; level <= 70; level++)
            {
                foreach (object forceid in ForceIDs)
                {
                    int fid = Convert.ToInt32(forceid);

                    string sql = string.Format("SELECT DISTINCT SkillID FROM skill_learn WHERE RequirePlayerLevel <= {0} AND ForceID = {1}", level, fid);
                    System.Data.DataTable tbl = Helper.GetDataTable(sql, Conn);
                    
                    if (tbl.Rows.Count == 0)
                        continue;

                    string strSkills = string.Empty;
                    foreach (System.Data.DataRow row in tbl.Rows)
                    {
                        strSkills += row["SkillID"].ToString().Trim();
                        strSkills += ",";
                    }
                    strSkills = strSkills.Substring(0, strSkills.Length - 1);

                    string strThisRow = string.Format("{0}\t{1}\t{2}\r\n", level, forceid, strSkills);
                    sb.Append(strThisRow);
                }
            }

            string strTargetFile = rootdir + "/ui/Scheme/Case/learnskill.txt";
            FileFolderHelper.StringToFile(sb.ToString(), strTargetFile);

            Helper.outputDebugString("成功导出：\r\n" + strTargetFile);
            return true;
        }

        private bool ImportSchoolTab(string tabFileName, int forceid)
        {
            string content = FileFolderHelper.FileToString(tabFileName);
            string[] rows = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            int i = 0;
            foreach (string row in rows)
            {
                if (i == 0)
                {
                    i++;
                    continue;
                }

                string[] cols = row.Split(new char[] {'\t'});
                if (cols.Length < 3)
                    continue;

                int skillid = Convert.ToInt32(cols[0]);
                int level = Convert.ToInt32(cols[3]);

                string sql = string.Empty;
                try
                {
                    SqlCommand cmd = Conn.CreateCommand();
                    sql = string.Format("INSERT INTO skill_learn VALUES({0}, {1}, {2})", skillid, level, forceid);
                    cmd.CommandText = sql;
                    cmd.ExecuteNonQuery();
                }
                catch (Exception ex)
                {
                    string err = string.Format("在表 skill_learn 中插入数据时发生错误。\r\nsql语句为：{0}\r\n\r\n错误信息：{0}", sql, ex.Message);
                    Helper.outputDebugStringError(err);
                    return false;
                }
            }

            return true;
        }
    }
}
