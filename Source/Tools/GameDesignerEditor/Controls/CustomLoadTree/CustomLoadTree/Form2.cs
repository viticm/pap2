using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace CustomLoadTree
{
    public partial class Form2 : Form
    {
        private SqlConnection conn; // 数据库连接
        private string missionName; // 任务名称
        private int missionLevel; // 任务等级
        private string missionClass; // 任务Class
        private string missionCat; // 任务_Cat
        private string missionObject; // 任务目标
        private string startMap; // 开始地图
        private string endMap; // 结束地图
        private bool tongqiMission; // 是否是同期任务
        private string fatherid; // 父结点的id
        private string fatherLevel; // 父结点的等级
        private string fatherStartMap; // 父结点的开始地图
        private string fatherEndMap; // 父结点的结束地图
        private List<KeyValuePair<string, string>> list1 = new List<KeyValuePair<string, string>>(); // 键值对链表

        public Form2(SqlConnection conn, string missionCat)
        {
            this.conn = conn;
            conn.Open();
            InitializeComponent();
            textBoxX4.Text = missionCat;
            comboBoxEx3.Text = missionCat;
            FillList();
            FillClass();
            comboBoxEx1.Text = missionCat;
            comboBoxEx2.Text = missionCat;
            conn.Close();
        }

        public Form2(SqlConnection conn, int fatherid, string missionCat)
        {
            this.conn = conn;
            this.fatherid = fatherid.ToString();
            conn.Open();
            InitializeComponent();
            textBoxX4.Text = missionCat;
            comboBoxEx3.Text = missionCat;
            FillList();
            FillClass();
            GetFatherInfo();
            textBoxX2.Text = fatherLevel;
            comboBoxEx1.Text = fatherStartMap;
            comboBoxEx2.Text = fatherEndMap;
            conn.Close();
        }

        // 任务名称属性
        public string MissionName
        {
            get
            {
                return missionName;
            }
        }

        // 任务等级属性
        public int MissionLevel
        {
            get
            {
                return missionLevel;
            }
        }

        // 任务最小等级属性
        public int MissionMinLevel
        {
            get
            {
                if (missionLevel - 2 < 0)
                {
                    return 0;
                }
                else
                {
                    return missionLevel - 2;
                }

            }
        }

        // 任务Class属性
        public string MissionClass
        {
            get
            {
                return missionClass;
            }
        }

        // 任务_Cat属性
        public string MissionCat
        {
            get
            {
                return missionCat;
            }
        }

        // 任务Object属性
        public string MissionObject
        {
            get
            {
                return missionObject;
            }
        }

        // 开始地图属性
        public string StartMap
        {
            get
            {
                return startMap;
            }
        }

        // 结束地图属性
        public string EndMap
        {
            get
            {
                return endMap;
            }
        }

        // 同期任务属性
        public bool TongqiMission
        {
            get
            {
                return tongqiMission;
            }
        }

        // 获取父结点的信息
        private void GetFatherInfo()
        {
            string sql = string.Format("SELECT QuestLevel, StartMapID, EndMapID FROM tbl_quests WHERE QuestID = {0}", fatherid);
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                reader.Read();
                fatherLevel = reader[0].ToString().Trim();
                fatherStartMap = reader[1].ToString().Trim();
                fatherEndMap = reader[2].ToString().Trim();
                foreach (KeyValuePair<string, string> k in list1)
                {
                    if (fatherStartMap == k.Key)
                    {
                        fatherStartMap = k.Value;
                    }
                    if (fatherEndMap == k.Key)
                    {
                        fatherEndMap = k.Value;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("获取父结点信息时产生异常: " + ex.ToString());
            }
            finally
            {
                reader.Close();
            }
        }

        // 填充Map选择下拉框内容
        private void FillList()
        {
            string sql = string.Format("SELECT ID, Name FROM MapList");
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while (reader.Read())
                {
                    if (reader[0].ToString().Trim() != "")
                    {
                        list1.Add(new KeyValuePair<string, string>(reader[0].ToString().Trim(), reader[1].ToString().Trim()));
                        comboBoxEx1.Items.Add(reader[1].ToString().Trim());
                        comboBoxEx2.Items.Add(reader[1].ToString().Trim());
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("填充Map选择下拉框内容时产生Sql异常: " + ex.ToString());
            }
            finally
            {
                reader.Close();
            }
        }

        // 填充Class选择下拉框内容
        private void FillClass()
        {
            string sql = string.Format("SELECT DISTINCT Class FROM tbl_quests");
            SqlCommand cmd;
            SqlDataReader reader = null;

            try
            {
                cmd = new SqlCommand(sql, conn);
                reader = cmd.ExecuteReader();
                while (reader.Read())
                {
                    if (reader[0].ToString().Trim() != "")
                    {
                        comboBoxEx3.Items.Add(reader[0].ToString().Trim());
                    }
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("填充Class选择下拉框内容时产生Sql异常: " + ex.ToString());
            }
            finally
            {
                reader.Close();
            }
        }

        // 检查用户输入的有效性
        private bool CheckInput()
        {
            int level;

            if (textBoxX1.Text == "") // 任务名称为空
            {
                MessageBox.Show("任务名不能为空!");
                return false;
            }

            if (textBoxX1.Text.Length > 12)
            {
                MessageBox.Show("输入的任务名过长!");
                return false;
            }

            if (textBoxX2.Text == "") // 任务等级为空
            {
                MessageBox.Show("任务等级不能为空!");
                return false;
            }

            if (comboBoxEx3.Text == "") // 任务Class为空
            {
                MessageBox.Show("任务Class不能为空!");
                return false;
            }

            if (textBoxX5.Text == "") // 任务目标为空
            {
                MessageBox.Show("任务目标不能为空!");
                return false;
            }

            if (comboBoxEx1.Text == "")
            {
                MessageBox.Show("开始地图不能为空!");
                return false;
            }

            if (comboBoxEx2.Text == "")
            {
                MessageBox.Show("结束地图不能为空");
                return false;
            }

            try // 检查任务等级是否为整形
            {
                level = int.Parse(textBoxX2.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show("任务等级不是有效数值: " + ex.ToString());
                return false;
            }

            if (level < 0) // 检查任务等级输入是否在有效范围内
            {
                return false;
            }

            return true;
        }

        // 用户选择确定
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (CheckInput()) // 用户输入有效
            {
                missionName = textBoxX1.Text;
                missionLevel = int.Parse(textBoxX2.Text);
                missionClass = comboBoxEx3.Text;
                missionCat = textBoxX4.Text;
                missionObject = textBoxX5.Text;
                if (!missionObject.StartsWith("<G>")) // 任务目标前加上<G>
                {
                    missionObject = "<G>" + missionObject;
                }
                startMap = comboBoxEx1.Text;
                endMap = comboBoxEx2.Text;
                tongqiMission = checkBoxX1.Checked;
                bool findStartMapIndex = false;
                bool findEndMapIndex = false;

                foreach (KeyValuePair<string, string> k in list1) // 检测匹配地图的键值对
                {
                    if (startMap == k.Value)
                    {
                        startMap = k.Key;
                        findStartMapIndex = true;
                    }
                    if (endMap == k.Value)
                    {
                        endMap = k.Key;
                        findEndMapIndex = true;
                    }
                }

                if(findStartMapIndex && findEndMapIndex)
                {
                    DialogResult = DialogResult.OK;
                    this.Close();
                }
                else
                {
                    if(!findStartMapIndex) // 找不到任务起始地图对应的序号
                    {
                        MessageBox.Show("找不到任务起始地图对应的序号，请检查输入的有效性");
                    }

                    if(!findEndMapIndex) // 找不到任务结束地图对应的序号
                    {
                        MessageBox.Show("找不到任务结束地图对应的序号，请检查输入的有效性！");
                    }                    
                }
            }
        }

        // 用户选择取消
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }
    }
}