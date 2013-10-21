using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace GameDesingerTools.DBTools
{
    public partial class ConfigModelForm : Form
    {
        private SqlConnection conn; // sql连接
        private TreeNode node; // 树结点
        private ModelEdit modelEditor; // 模块编辑器对象
        private ImageList imageList; // 图标链表
        private string modelID; // 模块的id

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="node">模块对应的树结点</param>
        /// <param name="modelEditor">模块编辑器对象</param>
        public ConfigModelForm(SqlConnection conn, TreeNode node, ModelEdit modelEditor)
        {
            this.conn = conn;
            this.node = node;
            this.modelEditor = modelEditor;
            this.imageList = modelEditor.imglstIcons;
            this.modelID = node.Tag.ToString();
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {            
            if (node.ImageIndex >= 0)
            {
                buttonItem1.Image = imageList.Images[node.ImageIndex];
            }

            buttonItem1.Text = node.Text;
            buttonItem1.Tooltip = "鼠标单击选择图片";
            textBoxX1.Text = node.Text;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                /* remoting
                string sql = string.Format("SELECT displaytext, displaystyle FROM {0} WHERE id = {1}",
                    "sys_modl_def", modelID);
                DataTable table = Helper.GetDataTable(sql, conn);
                */

                string filter = string.Format("id = {0}", modelID);
                DataTable table = Helper.GetDataTableProxy("sys_modl_def", filter, null, conn);

                if(table.Rows.Count > 0 && !(table.Rows[0]["displaystyle"] is DBNull)) // 找到自定义数据
                {
                    buttonItem1.Text = table.Rows[0]["displaytext"].ToString();
                    textBoxX1.Text = table.Rows[0]["displaytext"].ToString();
                    comboBoxEx1.SelectedIndex = int.Parse(table.Rows[0]["displaystyle"].ToString());
                }
                else
                {
                    comboBoxEx1.SelectedIndex = 0;
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在初始化模块显示样式信息时产生异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }    
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        private void SaveData()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                string displayText = textBoxX1.Text; // 显示的文本
                int displayStyle = comboBoxEx1.SelectedIndex; // 显示的样式
                string sql = string.Format("UPDATE {0} SET {1} = \'{2}\', {3} = {4} WHERE id = {5}",
                    "sys_modl_def", "displaytext", displayText, "displaystyle", displayStyle.ToString(), modelID);

                Helper.ExecuteCommand("sys_modl_def", sql, conn);
                /* remoting
                SqlCommand cmd = new SqlCommand(sql, conn);
                cmd.ExecuteNonQuery();
                 */

            }
            catch(Exception ex)
            {
                MessageBox.Show("在保存模块自定义显示样式时产生异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            SaveData();
            Program.MainForm.RefreshButtonStyle(modelID, textBoxX1.Text, comboBoxEx1.SelectedIndex); // 刷新MainForm里面的显示样式
            this.Close();
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// 用户选择新的显示样式
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch(comboBoxEx1.SelectedIndex)
            {
                case 0: // 默认样式
                    {
                        buttonItem1.ButtonStyle = DevComponents.DotNetBar.eButtonStyle.Default;
                        buttonItem1.Tooltip = node.Text;
                        break;
                    }
                case 1: // 单一文本
                    {
                        buttonItem1.ButtonStyle = DevComponents.DotNetBar.eButtonStyle.TextOnlyAlways;
                        buttonItem1.Tooltip = node.Text;
                        break;
                    }
                case 2: // 图文混合
                    {
                        buttonItem1.ButtonStyle = DevComponents.DotNetBar.eButtonStyle.ImageAndText;
                        buttonItem1.Tooltip = node.Text;
                        break;
                    }
            }
        }

        /// <summary>
        /// 显示文本发生变化
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            buttonItem1.Text = textBoxX1.Text;
        }

        /// <summary>
        /// 更换模块图标
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonItem1_Click(object sender, EventArgs e)
        {
            IconConfig iconForm = new IconConfig(node, modelEditor);
            iconForm.ShowDialog();
            buttonItem1.Image = imageList.Images[node.ImageIndex];
        }
    }
}