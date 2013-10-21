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

namespace AIChartLogicManager
{
    public partial class CreateChartForm : Office2007Form
    {
        private Hashtable modelTable = new Hashtable(); // 模块数据哈希表
        private string currentPath; // 当前目录
        private string chartID; // 新建绘图的ID
        private string chartName; // 新建绘图的名称

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="currentPath">当前目录</param>
        public CreateChartForm(string currentPath)
        {
            this.currentPath = currentPath;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 新建绘图的ID
        /// </summary>
        public string ChartID
        {
            get
            {
                return chartID;
            }
        }

        /// <summary>
        /// 新建绘图的名称
        /// </summary>
        public string ChartName
        {
            get
            {
                return chartName;
            }
        }

        /// <summary>
        /// 初始化
        /// </summary>
        private void Init()
        {
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            DataTable infoTable = dataBaseManager.GetDiagramInformation("AI图");

            foreach (DataRow row in infoTable.Rows)
            {
                string fullPath = row["FullPath"].ToString();

                if (fullPath.StartsWith("参考模板") && !(bool)row["IsFolder"])
                {
                    string modelName = fullPath.Replace("参考模板", "");
                    modelName = modelName.Replace("\\", "");
                    string modelID = row["ID"].ToString();

                    comboBoxEx1.Items.Add(modelName);                    
                    modelTable[modelName] = modelID;
                }
            }
        }

        /// <summary>
        /// 创建新AI图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton1.Checked)
            {
                radioButton2.Checked = false;
                comboBoxEx1.Enabled = false;
            }
        }

        /// <summary>
        /// 从模板复制
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton2.Checked)
            {
                radioButton1.Checked = false;
                comboBoxEx1.Enabled = true;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            bool editSuccess = false;
            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            
            chartName = textBoxX1.Text;
            if (chartName != "")
            {                
                string fullPath = string.Format("{0}\\{1}", currentPath, chartName);

                if (radioButton1.Checked) // 创建新AI图
                {
                    string originID = dataBaseManager.CreateNewDiagram(fullPath, false, "AI图");
                    chartID = originID;
                    string destinID = "311";

                    if (originID != null)
                    {
                        editSuccess = dataBaseManager.CopyDiagramData(originID, destinID);
                    }                
                }
                else // 从模板复制
                {
                    string modelName = comboBoxEx1.Text;

                    if (modelName != "")
                    {
                        string originID = dataBaseManager.CreateNewDiagram(fullPath, false, "AI图");
                        chartID = originID;
                        string destinID = modelTable[modelName] as string;

                        if (originID != null)
                        {
                            editSuccess = dataBaseManager.CopyDiagramData(originID, destinID);
                        } 
                    }
                    else
                    {
                        MessageBox.Show("复制模板不能为空！", "新建绘图", MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
                    }
                }
            }
            else
            {
                MessageBox.Show("绘图名称不能为空！", "新建绘图", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }            

            if (editSuccess)
            {
                DialogResult = DialogResult.OK;
                this.Close();
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
    }
}