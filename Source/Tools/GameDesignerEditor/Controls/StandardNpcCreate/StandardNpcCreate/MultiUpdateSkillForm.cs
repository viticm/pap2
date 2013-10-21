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
    public partial class MultiUpdateSkillForm : Form
    {
        private SqlConnection conn; // sql连接
        private List<string> npcInfoList; // npc信息链表
        private List<string> selectedNpcInfoList = new List<string>(); // 选中的npc信息链表
        private bool selectAll = true;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="npcInfoList">npc信息链表</param>
        public MultiUpdateSkillForm(SqlConnection conn, List<string> npcInfoList)
        {
            this.conn = conn;
            this.npcInfoList = npcInfoList;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 选中的npc信息链表
        /// </summary>
        public List<string> SelectedNpcInfoList
        {
            get
            {
                return selectedNpcInfoList;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach(string s in npcInfoList)
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
        /// <param name="mark">是否标记</param>
        private void SetNpcSpecific(string npcID, bool mark)
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT ID FROM npc_skill_record WHERE ID = '{0}'", npcID);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;

                object executeResult = cmd.ExecuteScalar();

                if (executeResult == null) // 需要插入
                {
                    sqlString = string.Format("INSERT INTO npc_skill_record (ID) VALUES ('{0}')", npcID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                if (mark)
                {
                    sqlString = string.Format("UPDATE npc_skill_record SET IsSpecific = 1 WHERE ID = '{0}'", npcID);
                }
                else
                {
                    sqlString = string.Format("UPDATE npc_skill_record SET IsSpecific = 0 WHERE ID = '{0}'", npcID);
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
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (checkedListBox1.GetItemChecked(i))
                {
                    string npcInfo = checkedListBox1.Items[i].ToString();

                    if (!npcInfo.EndsWith("[特殊]")) // 特殊NPC不参与更新
                    {
                        selectedNpcInfoList.Add(npcInfo);
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
        /// 全部选中
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
        /// 选中未填第二技能的NPC
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < npcInfoList.Count; i++)
            {
                if (npcInfoList[i].EndsWith("[未填第二技能]"))
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
        private void bMarkSpecificNpc_Click(object sender, EventArgs e)
        {
            int selectedIndex = checkedListBox1.SelectedIndex;

            if (selectedIndex >= 0 && selectedIndex < checkedListBox1.Items.Count)
            {
                string oldValue = checkedListBox1.Items[selectedIndex].ToString();
                string newString = string.Format("{0} [特殊]", oldValue.Replace("[未填第二技能]", "").Trim()); ;
                checkedListBox1.Items[selectedIndex] = newString;
                checkedListBox1.SetItemChecked(selectedIndex, false);

                string npcID = GetNpcID(newString).ToString();
                SetNpcSpecific(npcID, true);
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
        private void bUnmarkSpecificNpc_Click(object sender, EventArgs e)
        {
            int selectedIndex = checkedListBox1.SelectedIndex;

            if (selectedIndex >= 0 && selectedIndex < checkedListBox1.Items.Count)
            {
                string oldValue = checkedListBox1.Items[selectedIndex].ToString();
                string newString = oldValue.Replace("[特殊]", "").Trim();
                checkedListBox1.Items[selectedIndex] = newString;
                checkedListBox1.SetItemChecked(selectedIndex, true);

                string npcID = GetNpcID(newString).ToString();
                SetNpcSpecific(npcID, false);
            }
            else
            {
                MessageBox.Show("请先选择要取消标记的NPC", "取消标记特殊NPC", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }
    }
}