using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;
using System.Data;
using System.Text;
using System.Collections;

namespace SkillScriptExport
{
    public class Program
    {
        private string rootPath; // 根目录
        private SqlConnection conn; // sql连接
        private List<string> exportedFiles = new List<string>(); // 要导出的文件名链表
        private Hashtable fileTable = new Hashtable(); // 保存文件名及其数据的hash表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="rootPath">根目录</param>
        public Program(SqlConnection conn, string rootPath)
        {
            this.conn = conn;
            this.rootPath = rootPath;
        }

        /// <summary>
        /// 导出数据表
        /// </summary>
        /// <returns></returns>
        public bool ExportScript()
        {
            bool success = true; // 是否已经导出成功

            FolderBrowserDialog openFolderDialog = new FolderBrowserDialog();
            openFolderDialog.Description = "请选择导出文件的保存目录";
            openFolderDialog.ShowNewFolderButton = true;
            if(rootPath == null)
            {
                openFolderDialog.RootFolder = Environment.SpecialFolder.MyComputer;
            }
            else
            {
                openFolderDialog.SelectedPath = rootPath;
            }

            DataTable dtExportTbl = GetDataTable("SELECT filename FROM sys_export_table_cfg WHERE tablename = 'tbl_skill_script'");
            string strRelaPath = dtExportTbl.Rows[0][0].ToString();
            rootPath += strRelaPath;
            //rootPath = openFolderDialog.SelectedPath;
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "tbl_skill_script");
                DataTable skillTable = GetDataTable(sqlString);
                sqlString = string.Format("SELECT * FROM {0}", "tbl_skill_AddAttribute");
                DataTable attributeTable = GetDataTable(sqlString);

                foreach (DataRow r in skillTable.Rows)
                {
                    string id = r["id"].ToString(); // 技能的id
                    List<string> dataList = new List<string>(); // 导出的数据链表
                    string fileName = Path.Combine(r["foldername"].ToString(), r["filename"].ToString());
                    string fullPath = Path.Combine(rootPath, fileName);

                    // 添加头文件
                    dataList.Add("Include(\"Scripts/Include/Skill.lh\")");
                    dataList.Add("Include(\"Scripts/Include/Player.lh\")");
                    dataList.Add("");

                    // 添加tskilldata表
                    dataList.Add("tSkillData =");
                    dataList.Add("{");
                    dataList.Add("\t" + r["tskilldata"].ToString().Replace("\r\n", "\r\n\t"));
                    dataList.Add("};");
                    dataList.Add("");

                    // 添加GetSkillLevelData方法
                    dataList.Add("function GetSkillLevelData(skill)");
                    dataList.Add("    local bRetCode = false");
                    dataList.Add("    local dwSkillLevel = skill.dwLevel");
                    dataList.Add("");

                    // 添加AddAttribute方法
                    DataRow[] attributes = attributeTable.Select(string.Format("id = '{0}'", id));
                    foreach (DataRow row in attributes)
                    {
                        dataList.Add("    skill.AddAttribute(");
                        dataList.Add("        " + row["parm1"].ToString() + ",");
                        dataList.Add("        " + row["parm2"].ToString() + ",");
                        dataList.Add("        " + row["parm3"].ToString() + ",");
                        dataList.Add("        " + row["parm4"].ToString());
                        dataList.Add("    );");
                        dataList.Add("");
                    }

                    // 添加CostMana
                    dataList.Add("    skill.nCostMana = " + r["nCostMana"].ToString());

                    // 添加LevelUpExp
                    dataList.Add("    skill.dwLevelUpExp = " + r["dwLevelUpExp"].ToString());

                    // 添加dwBaseKungfuExpAdd
                    dataList.Add("    skill.dwBaseKungfuExpAdd = " + r["dwBaseKungfuExpAdd"].ToString());

                    // 添加nMinRadius
                    dataList.Add("    skill.nMinRadius = " + r["nMinRadius"].ToString() + " * LENGTH_BASE");

                    // 添加nMaxRadius
                    dataList.Add("    skill.nMaxRadius = " + r["nMaxRadius"].ToString() + " * LENGTH_BASE");

                    // 添加nAngleRange
                    dataList.Add("    skill.nAngleRange = " + r["nAngleRange"].ToString());

                    // 添加nPrepareFrames
                    dataList.Add("    skill.nPrepareFrames = " + r["nPrepareFrames"].ToString());

                    // 添加nBulletVelocity
                    dataList.Add("    skill.nBulletVelocity = " + r["nBulletVelocity"].ToString());

                    // 添加nBreakRate
                    dataList.Add("    skill.nBreakRate = " + r["nBreakRate"].ToString());

                    // 添加CoolDown
                    for (int i = 1; i <= 4; i++)
                    {
                        object o1 = r["SetCoolDown" + i + "_1"];
                        object o2 = r["SetCoolDown" + i + "_2"];
                        if ((o1 != null) && (o2 != null) && (o1.ToString() != "") && (o2.ToString() != ""))
                        {
                            dataList.Add(string.Format("    skill.SetCoolDown({0}, {1})", o1, o2));
                        }
                    }
                    dataList.Add("");

                    dataList.Add("    return true");
                    dataList.Add("end");
                    dataList.Add("");

                    // 添加CanCast方法
                    dataList.Add("function CanCast(player, nPreResult)");
                    dataList.Add("    return nPreResult");
                    dataList.Add("end");
                    dataList.Add("");

                    // 添加CanLearnSkill方法
                    dataList.Add("function CanLearnSkill(skill, player)");
                    dataList.Add("    return true");
                    dataList.Add("end");
                    dataList.Add("");

                    // 添加OnSkillLevelUp方法
                    dataList.Add("function OnSkillLevelUp(skill, player)");
                    dataList.Add("end");

                    fileTable[fullPath] = dataList;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取技能表脚本数据时产生sql异常: " + ex.ToString());
                success = false;
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }


            List<string> fileNameList = new List<string>();
            foreach (string s in fileTable.Keys)
            {
                fileNameList.Add(s);
            }

            // 选择要导出的文件
            ChooseFileForm cForm = new ChooseFileForm(fileNameList);
            DialogResult result = cForm.ShowDialog();
            if (result != DialogResult.OK)
            {
                return false;
            }

            // 导出文件
            foreach (string s in fileTable.Keys)
            {
                if (fileNameList.Contains(s))
                {
                    List<string> lines = fileTable[s] as List<string>;
                    SaveData(s, lines);
                }
            }

            // 显示导出文件列表
            string message = "下列文件:\n\n";
            foreach (string s in fileNameList)
            {
                message = message + string.Format("{0}\n", s);
            }
            message = message + "\n\n";
            message = message + "已经成功导出!";
            MessageBox.Show(message);

            return success;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="values">数据内容</param>
        private void SaveData(string fileName, List<string> values)
        {
            try
            {
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    MessageBox.Show(string.Format("文件{0}不存在,将新建该文件!", fileName));
                    string path = Path.GetDirectoryName(fileName); // 获取目录
                    if (!Directory.Exists(path)) // 目录不存在
                    {
                        Directory.CreateDirectory(path); // 先创建目录，再创建文件
                    }
                    FileStream fs = fi.Create();
                    fs.Close();
                    // sw = fi.CreateText(); // 新建文本
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else
                {
                    if (!exportedFiles.Contains(fileName)) // 第一次写文件
                    {
                        // sw = fi.CreateText();
                        sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                        exportedFiles.Add(fileName);
                    }
                    else // 添加新行
                    {
                        // sw = fi.AppendText(); // 添加文本
                        sw = new StreamWriter(fileName, true, Encoding.GetEncoding("gb2312"));
                    }
                }

                foreach (string s in values)
                {
                    sw.WriteLine(s);
                }
                sw.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
            }
        }
    }
}