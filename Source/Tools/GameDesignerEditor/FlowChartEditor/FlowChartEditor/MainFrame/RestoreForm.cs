using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using DevComponents.DotNetBar;

using DBManager;

namespace FlowChartEditor
{
    public partial class RestoreForm : Office2007Form
    {
        string originID; // 原绘图ID
        DataTable backupTable; // 绘图备份信息数据表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="originID">原绘图ID</param>
        /// <param name="backupTable">绘图备份信息数据表</param>
        public RestoreForm(string originID, DataTable backupTable)
        {
            this.originID = originID;
            this.backupTable = backupTable;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 添加数据列
            DataGridViewTextBoxColumn hostColumn = new DataGridViewTextBoxColumn();
            hostColumn.Name = "Host";
            hostColumn.HeaderText = "编辑者";

            DataGridViewTextBoxColumn editTimeColumn = new DataGridViewTextBoxColumn();
            editTimeColumn.Name = "EditTime";
            editTimeColumn.HeaderText = "编辑时间";

            DataGridViewTextBoxColumn versionColumn = new DataGridViewTextBoxColumn();
            versionColumn.Name = "Version";
            versionColumn.HeaderText = "版本号";

            dataGridViewX1.Columns.Add(hostColumn);
            dataGridViewX1.Columns.Add(editTimeColumn);
            dataGridViewX1.Columns.Add(versionColumn);

            // 添加数据行            
            for (int i = 0; i < backupTable.Rows.Count; i++)
            {
                DataRow row = backupTable.Rows[i];
                dataGridViewX1.Rows.Add(1);
                DataGridViewRow newRow = dataGridViewX1.Rows[i];
                newRow.Cells["Host"].Value = row["Host"].ToString();
                newRow.Cells["EditTime"].Value = row["EditTime"].ToString();
                string diagramVersion = row["Version"].ToString();
                if (diagramVersion == "")
                {
                    diagramVersion = "0";
                }
                newRow.Cells["Version"].Value = diagramVersion;
                newRow.Tag = row["ID"].ToString();
            }
        }

        /// <summary>
        /// 选择恢复
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {            
            if (dataGridViewX1.SelectedCells.Count >= 0)
            {
                string id = dataGridViewX1.SelectedCells[0].OwningRow.Tag.ToString();
                DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                string lockUser = dataBaseManager.Diagram_WhoLocked(int.Parse(originID));

                if (lockUser == null)
                {
                    bool success = dataBaseManager.RestoreDiagramData(originID, id);

                    if (success)
                    {
                        MessageBox.Show("绘图恢复成功！", "恢复绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        DialogResult = DialogResult.OK;
                        this.Close();
                    }
                }
                else
                {
                    MessageBox.Show(string.Format("该绘图当前被{0}锁定！"), "恢复绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先选择要恢复的绘图版本！", "恢复绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
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
        /// 查看绘图内容
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {            
            if (dataGridViewX1.SelectedCells.Count >= 0)
            {
                string id = dataGridViewX1.SelectedCells[0].OwningRow.Tag.ToString();
                string version = dataGridViewX1.SelectedCells[0].OwningRow.Cells["Version"].Value.ToString();
                DocumentManager.GetDocumentManager().OpenDiaramAtNewPanel(id, version);
            }
            else
            {
                MessageBox.Show("请先选择要恢复的绘图版本！", "恢复绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            this.Close();
        }
    }
}