using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using DevComponents.DotNetBar;
using DevComponents.AdvTree;

using DataInteractor;

namespace AIDiagramPlugin
{
    public partial class TemplateParmForm : Office2007Form
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        public TemplateParmForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            codeEditBox.ForWho = "GameLuaEditor";
            FlowChartInteractor it = Interactor.GetInteractor().CurrentFlowChartInteractor;
            codeEditBox.Text = it.CustomText;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (tabControl1.SelectedTabIndex == 0) // 当前是否在编辑脚本
            {
                if (!AIDiagramHelper.CheckScript(codeEditBox, codeEditBox.Text)) // 检查脚本
                {
                    if (MessageBox.Show("没有通过脚本检查，是否重新编辑脚本？", "脚本检查",
                                        MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                    {
                        return;
                    }
                }
            }

            // 保存数据
            FlowChartInteractor it = Interactor.GetInteractor().CurrentFlowChartInteractor;           

            if (codeEditBox.Text != "")
            {
                it.CustomText = codeEditBox.Text;
            }            

            this.DialogResult = DialogResult.OK;
            this.Close();
        }        

        /// <summary>
        /// 智能感知
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void codeEditBox_queryObjects(object sender, luaEditor.QueryEventsArgs e)
        {
            AIDiagramHelper.QueryObjects(sender, e);            
        }
    }
}