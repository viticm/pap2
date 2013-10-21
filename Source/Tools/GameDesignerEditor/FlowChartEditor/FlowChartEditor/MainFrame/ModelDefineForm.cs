using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using DevComponents.DotNetBar;

using DBManager;

namespace FlowChartEditor
{
    public partial class ModelDefineForm : Office2007Form
    {
        DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager(); // 数据库管理器
        private string chartClass; // 流程图类型
        private string nodeType; // 结点类型
        private string modelName; // 模板名称
        private string modelDescription; // 模板描述

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="chartClass">流程图类型</param>
        /// <param name="nodeType">结点类型</param>
        public ModelDefineForm(string chartClass, string nodeType)
        {
            this.chartClass = chartClass;
            this.nodeType = nodeType;
            InitializeComponent();
            textBoxX3.Text = chartClass;
            textBoxX4.Text = nodeType;
        }

        /// <summary>
        /// 模板名称
        /// </summary>
        public string ModelName
        {
            get
            {
                return modelName;
            }
        }

        /// <summary>
        /// 模板描述
        /// </summary>
        public string ModelDescription
        {
            get
            {
                return modelDescription;
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            modelName = textBoxX1.Text;
            modelDescription = textBoxX2.Text;

            if(modelName != "" && modelDescription != "")
            {
                if(!dataBaseManager.ModelNameExist(modelName, chartClass, nodeType))
                {
                    DialogResult = DialogResult.OK;
                    this.Close();
                }
                else
                {
                    MessageBox.Show("当前分类的模板名称已经存在！", "自定义模板",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }                
            }
            else
            {
                MessageBox.Show("请设置完整的参数信息！", "自定义模板",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
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