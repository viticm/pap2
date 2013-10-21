using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using DevComponents.DotNetBar;

using GraphElementLibrary;
using DataElementLibrary;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    public partial class SearchForm : Office2007Form
    {
        private string searchType = "Node"; // 搜索类型
        private string contentType = "Text"; // 搜索内容 
        private int searchID = 0; // 要搜索的ID
        private string searchText = ""; // 要搜索的文本
        private FlowChartManager flowChartManager; // 流程图管理对象

        private List<string> searchTextList = new List<string>(); // 查找文本的记录
        private List<string> searchScriptList = new List<string>(); // 查找脚本的记录

        /// <summary>
        /// 构造函数
        /// </summary>
        public SearchForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 当前流程图管理对象
        /// </summary>
        public FlowChartManager CurrentFlowChartManager
        {
            set
            {
                flowChartManager = value;
            }
        }        

        /// <summary>
        /// 初始化文本下拉框列表
        /// </summary>
        /// <param name="searchText">查找的文本</param>
        private void InitTextList(string searchText)
        {
            if (!searchTextList.Contains(searchText))
            {
                searchTextList.Add(searchText);
                textBox.Items.Add(searchText);
            }
        }

        /// <summary>
        /// 初始化脚本下拉框列表
        /// </summary>
        /// <param name="searchScript">查找的脚本</param>
        private void InitScriptList(string searchScript)
        {
            if (!searchScriptList.Contains(searchScript))
            {
                searchScriptList.Add(searchScript);
                scriptBox.Items.Add(searchScript);
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (radioButton1.Checked) // 搜索连接线
            {
                searchType = "Node";
            }
            else if (radioButton2.Checked) // 搜索结点
            {
                searchType = "Line";
            }
            else
            {
                MessageBox.Show("请先选择要查找的类型！", "查找",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (radioButton3.Checked) // 搜索ID
            {
                contentType = "ID";
                searchID = idBox.Value;

                if (searchID == 0)
                {
                    MessageBox.Show("要查找的ID不能为空！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }                
            }
            else if (radioButton4.Checked) // 搜索文本
            {
                contentType = "Text";
                searchText = textBox.Text;
                InitTextList(searchText);

                if (searchText == "")
                {
                    MessageBox.Show("要查找的文本不能为空！", "查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
            }
            else if (radioButton5.Checked) // 搜索脚本
            {
                contentType = "Script";
                searchText = scriptBox.Text;
                InitScriptList(searchText);

                if (searchText == "")
                {
                    MessageBox.Show("要查找的脚本不能为空！", "查找", 
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
            }
            else 
            {
                MessageBox.Show("请先选择要查找的内容！", "查找",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            List<GraphElement> graphElementList = new List<GraphElement>();

            switch (searchType)
            {
                case "Node":
                    {
                        switch(contentType)
                        {
                            case "ID":
                                {
                                    foreach (GraphElement graphElement in flowChartManager.CurrentGraphManager.SlotContainerList)
                                    {
                                        if (graphElement.ID == searchID)
                                        {
                                            graphElementList.Add(graphElement);
                                        }
                                    }

                                    break;
                                }
                            case "Text":
                                {
                                    foreach(GraphElement graphElement in flowChartManager.CurrentGraphManager.SlotContainerList)
                                    {
                                        if (graphElement.Text.Contains(searchText))
                                        {
                                            graphElementList.Add(graphElement);
                                        }
                                    }                                    

                                    break;
                                }
                            case "Script":
                                {
                                    foreach (GraphElement graphElement in flowChartManager.CurrentGraphManager.SlotContainerList)
                                    {
                                        DataElement dataElement = flowChartManager.CurrentDataManager.GetDataElement(graphElement);

                                        if (dataElement != null)
                                        {
                                            switch (dataElement.DataType)
                                            {
                                                case "AIActionNode":
                                                    {
                                                        Hashtable infoTable = dataElement.GetOutputArgs();
                                                        string scriptContent = infoTable["Script"] as string;
                                                        
                                                        if (scriptContent != null)
                                                        {
                                                            if (scriptContent.Contains(searchText))
                                                            {
                                                                graphElementList.Add(graphElement);
                                                            }
                                                        }
                                                        
                                                        break;
                                                    }
                                                case "AIActionsNode":
                                                    {
                                                        Hashtable infoTable = dataElement.GetOutputArgs();
                                                        List<string> scriptContentList = infoTable["ScriptList"] as List<string>;

                                                        if (scriptContentList != null)
                                                        {
                                                            foreach (string s in scriptContentList)
                                                            {
                                                                if (s.Contains(searchText))
                                                                {
                                                                    graphElementList.Add(graphElement);
                                                                    break;
                                                                }
                                                            }
                                                        }                                                        

                                                        break;
                                                    }
                                            }
                                        }
                                    }

                                    break;
                                }
                        }

                        break;
                    }
                case "Line":
                    {
                        switch(contentType)
                        {
                            case "ID":
                                {
                                    foreach(GraphElement graphElement in flowChartManager.CurrentGraphManager.ConnectorContainerList)
                                    {
                                        if (graphElement.ID == searchID)
                                        {
                                            graphElementList.Add(graphElement);
                                        }
                                    }

                                    break;
                                }
                            case "Text":
                                {
                                    foreach(GraphElement graphElement in flowChartManager.CurrentGraphManager.ConnectorContainerList)
                                    {
                                        if (graphElement.Text.Contains(searchText))
                                        {
                                            graphElementList.Add(graphElement);
                                        }
                                    }                                    

                                    break;
                                }
                        }

                        break;
                    }
            }                

            SearchResultForm sForm = new SearchResultForm(flowChartManager.CurrentGraphManager, graphElementList);
            sForm.Show();            
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Visible = false;
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void SearchForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }
    }
}