using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace StandardNpcCreate
{
    public partial class MultiUpdateForm : Form
    {
        private List<string> selectedNpcList; // 选中的npc链表
        private bool selectAll = true; // 是否全部选中
        private SqlConnection conn; // sql连接
        private string mapName; // 地图名

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="selectedNpcList">选中的npc链表</param>
        /// <param name="conn">sql连接</param>
        /// <param name="mapName">地图名</param>
        public MultiUpdateForm(List<string> selectedNpcList, SqlConnection conn, string mapName)
        {
            this.selectedNpcList = selectedNpcList;
            this.conn = conn;
            this.mapName = mapName;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 是否更新已有动作
        /// </summary>
        public bool UpdateExistAnimation
        {
            get
            {
                return checkBoxX1.Checked;
            }
        }

        /// <summary>
        /// 是否更新普通待机动作
        /// </summary>
        public bool UpdateCommonStandbyAnimation
        {
            get
            {
                return !checkBoxX2.Checked;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {            
            foreach(string s in selectedNpcList)
            {
                checkedListBox1.Items.Add(s);
            }
        }

        /// <summary>
        /// 从NPC描述字符串里取到NPC的ID
        /// </summary>
        /// <param name="npcString">NPC描述字符串</param>
        /// <return>NPC的ID</return>
        private int GetNpcID(string npcString)
        {
            int npcID = -1;

            int firstIndex = npcString.LastIndexOf("(");
            int lastIndex = npcString.LastIndexOf(")");

            if (firstIndex >= 0 && lastIndex >= 0)
            {
                npcID = int.Parse(npcString.Substring(firstIndex + 1, lastIndex - firstIndex - 1));
            }

            return npcID;
        }

        /// <summary>
        /// 从NPC描述字符串里取到NPC的名称
        /// </summary>
        /// <param name="npcString">NPC描述字符串</param>
        /// <returns>NPC的名称</returns>
        private string GetNpcName(string npcString)
        {
            string npcName = null;

            int index = npcString.LastIndexOf("(");

            if (index > 0)
            {
                npcName = npcString.Substring(0, index - 1);
            }

            return npcName;
        }

        /// <summary>
        /// 标记/取消标记特殊NPC
        /// </summary>
        /// <param name="npcID">npc的模板ID</param>
        /// <param name="npcName">npc的名称</param>
        /// <param name="mark">是否标记</param>
        private void SetNpcSpecific(string npcID, string npcName, bool mark)
        {           
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Name FROM npc_animation_record WHERE ID = '{0}'", npcID);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;

                object executeResult = cmd.ExecuteScalar();

                if (executeResult == null) // 需要插入
                {                                
                    sqlString = string.Format("INSERT INTO npc_animation_record (ID, MapName, Name) VALUES ('{0}', '{1}', '{2}')",
                                                            npcID, mapName, npcName);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                if (mark)
                {
                    sqlString = string.Format("UPDATE npc_animation_record SET IsSpecific = 1 WHERE ID = '{0}'", npcID);
                }
                else
                {
                    sqlString = string.Format("UPDATE npc_animation_record SET IsSpecific = NULL WHERE ID = '{0}'", npcID);
                }

                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在更新特殊NPC信息时产生sql异常：" + ex.Message, "更新特殊NPC信息",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            selectedNpcList.Clear();

            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {                
                if (checkedListBox1.GetItemChecked(i))
                {
                    string npcName = checkedListBox1.Items[i] as string;

                    if (!npcName.EndsWith("[特殊]")) // 特殊npc不能生成动作
                    {
                        selectedNpcList.Add(npcName.Replace("[动作不完整]", "").Trim());
                    }                  
                }                
            }

            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 全部选中/全部取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                checkedListBox1.SetItemChecked(i, selectAll);
            }

            if (selectAll)
            {
                buttonX3.Text = "全部取消";
            }
            else
            {
                buttonX3.Text = "全部选中";
            }

            selectAll = !selectAll;
        }

        /// <summary>
        /// 选择动作不完整的NPC
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                string itemName = checkedListBox1.Items[i] as string;

                if (itemName.EndsWith("[动作不完整]"))
                {
                    checkedListBox1.SetItemChecked(i, true);
                }
                else
                {
                    checkedListBox1.SetItemChecked(i, false);
                }
            }
        }

        /// <summary>
        /// 标记为特殊NPC
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMarkSpecialNpc_Click(object sender, EventArgs e)
        {
            int selectedIndex = checkedListBox1.SelectedIndex;

            if (selectedIndex >= 0 && selectedIndex < checkedListBox1.Items.Count)
            {
                string oldValue = checkedListBox1.Items[selectedIndex].ToString();
                string newString = string.Format("{0} [特殊]", oldValue.Replace("[动作不完整]", "").Trim()); ;
                checkedListBox1.Items[selectedIndex] = newString;
                checkedListBox1.SetItemChecked(selectedIndex, false);

                string npcID = GetNpcID(newString).ToString();
                string npcName = GetNpcName(newString);

                SetNpcSpecific(npcID, npcName, true);
            }
            else
            {
                MessageBox.Show("请先选择要标记的NPC", "标记特殊NPC", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 取消标记为特殊NPC
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bUnmarkSpecialNpc_Click(object sender, EventArgs e)
        {
            int selectedIndex = checkedListBox1.SelectedIndex;

            if (selectedIndex >= 0 && selectedIndex < checkedListBox1.Items.Count)
            {
                string oldValue = checkedListBox1.Items[selectedIndex].ToString();
                string newString = oldValue.Replace("[特殊]", "").Trim();
                checkedListBox1.Items[selectedIndex] = newString;
                checkedListBox1.SetItemChecked(selectedIndex, true);

                string npcID = GetNpcID(newString).ToString();
                string npcName = GetNpcName(newString);

                SetNpcSpecific(npcID, npcName, false);
            }
            else
            {
                MessageBox.Show("请先选择要取消标记的NPC", "取消标记特殊NPC", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 勾选/取消更新已有动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void checkBoxX1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxX1.Checked)
            {
                checkBoxX2.Enabled = true;
            }
            else
            {
                checkBoxX2.Enabled = false;
            }
        }      
    }
}