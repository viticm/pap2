using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Xml;

using DevComponents.DotNetBar;

namespace GuideConfig
{
    public partial class ListConfigForm : Office2007Form
    {
        private SqlConnection conn;
        private string configInfo;
        private string listType; // 下拉框类型
        private bool initFinished = false;
        private Dictionary<string, DataTable> tableInfoDictionary = new Dictionary<string, DataTable>();

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="listType">下拉框类型</param> 
        /// <param name="conn">sql连接</param>
        /// <param name="configInfo">配置信息</param>
        public ListConfigForm(SqlConnection conn, string listType, string configInfo)
        {
            this.conn = conn;
            this.listType = listType;
            this.configInfo = configInfo;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 配置信息
        /// </summary>
        public string ConfigInfo
        {
            get
            {
                return configInfo;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {            
            // 初始化数据表相关信息
            string sqlString = "SELECT name FROM sysobjects WHERE xtype = 'u' AND name NOT LIKE 'sys_' ORDER BY name";
            DataTable dataTable = Helper.GetDataTable(sqlString, conn);

            foreach (DataRow dataRow in dataTable.Rows)
            {
                string tableName = dataRow["name"].ToString();
                sqlString = string.Format("SELECT TOP 1 * FROM {0}", tableName);
                DataTable rowInfo = Helper.GetDataTable(sqlString, conn);
                tableInfoDictionary[tableName] = rowInfo;

                tableNameBox.Items.Add(tableName);
            }

            // 显示分页信息
            switch (listType)
            {
                case "text":
                    {
                        listConfigControl.SelectedTabIndex = 0;
                        break;
                    }
                case "database":
                    {
                        listConfigControl.SelectedTabIndex = 1;
                        break;
                    }
            }

            // 读取配置信息
            if (!string.IsNullOrEmpty(configInfo))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(configInfo);

                XmlNode listNode = xmlDocument.ChildNodes[0];
                string typeString = listNode.Attributes["type"].Value;

                if (typeString == listType)
                {
                    switch (typeString)
                    {
                        case "text":
                            {
                                foreach (XmlNode itemNode in listNode.ChildNodes)
                                {
                                    string itemName = itemNode.Attributes["name"].Value;
                                    string itemValue = itemNode.Attributes["value"].Value;

                                    string itemInfo = string.Format("{0} ({1})", itemName, itemValue);
                                    listTextBox.Items.Add(itemInfo);
                                }

                                break;
                            }
                        case "database":
                            {
                                string tableName = listNode.Attributes["tableName"].Value;
                                string displayField = listNode.Attributes["displayField"].Value;
                                string valueField = listNode.Attributes["valueField"].Value;

                                tableNameBox.Text = tableName;
                                displayFieldBox.Text = displayField;
                                valueFieldBox.Text = valueField;

                                break;
                            }
                    }
                }                
            }

            initFinished = true;
        }

        /// <summary>
        /// 获取选项信息
        /// </summary>
        /// <param name="infoString">选项字符串描述</param>
        /// <returns>选项信息</returns>
        private string[] GetItemInfo(string infoString)
        {
            int index = infoString.LastIndexOf("(");
            string itemName = infoString.Substring(0, index - 1);
            string itemValue = infoString.Substring(index + 1, infoString.Length - index - 2);

            return new string[] { itemName, itemValue };
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            bool valid = true;

            switch (listConfigControl.SelectedTab.Text)
            {
                case "文本":
                    {
                        if (listTextBox.Items.Count > 0)
                        {
                            StringBuilder infoString = new StringBuilder();
                            infoString.AppendLine("<listInfo type = \"text\">");                            
                            
                            foreach (string listItem in listTextBox.Items)
                            {
                                string[] itemInfo = GetItemInfo(listItem);
                                infoString.AppendLine(string.Format("<listItem name = \"{0}\" value = \"{1}\">",
                                                                    itemInfo[0], itemInfo[1]));
                                infoString.AppendLine("</listItem>");
                            }

                            infoString.AppendLine("</listInfo>");

                            configInfo = infoString.ToString();
                        }
                        else
                        {
                            valid = false;
                            MessageBox.Show("请先添加文本选项！", "下拉框配置",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }

                        break;
                    }
                case "数据库索引":
                    {
                        string tableName = tableNameBox.Text;
                        string displayField = displayFieldBox.Text;
                        string valueField = valueFieldBox.Text;

                        if (!string.IsNullOrEmpty(tableName) && !string.IsNullOrEmpty(displayField)
                            && !string.IsNullOrEmpty(valueField))
                        {
                            StringBuilder infoString = new StringBuilder();
                            infoString.AppendLine(string.Format("<listInfo type = \"database\" tableName = \"{0}\" displayField = \"{1}\" valueField = \"{2}\">",
                                                                tableName, displayField, valueField));                                                    
                            infoString.AppendLine("</listInfo>");

                            configInfo = infoString.ToString();
                        }
                        else
                        {
                            valid = false;
                            MessageBox.Show("请先填完整数据库索引的参数！", "下拉框配置",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }

                        break;
                    }
            }

            if (valid)
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
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 添加选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddListItem_Click(object sender, EventArgs e)
        {
            string itemName = listItemNameBox.Text;
            string itemValue = listItemValueBox.Text;

            if (!string.IsNullOrEmpty(itemName) && !string.IsNullOrEmpty(itemValue))
            {
                string itemInfo = string.Format("{0} ({1})", itemName, itemValue);

                bool itemExist = false;

                foreach (string listItem in listTextBox.Items)
                {
                    if (listItem == itemInfo)
                    {
                        itemExist = true;
                        break;
                    }
                }

                if (!itemExist)
                {
                    listItemNameBox.Text = "";
                    listItemValueBox.Text = "";
                    listTextBox.Items.Add(itemInfo);
                    listTextBox.SelectedIndex = listTextBox.Items.Count - 1;
                }
                else
                {
                    MessageBox.Show("不能有重复的选项！", "添加选项",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先输入添加选项的名称和值！", "添加选项",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 删除选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemove_Click(object sender, EventArgs e)
        {
            int index = listTextBox.SelectedIndex;

            if (index != -1)
            {
                listTextBox.Items.RemoveAt(index);
            }
            else
            {
                MessageBox.Show("请先选择要删除的选项！", "删除选项",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 上移选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMoveUp_Click(object sender, EventArgs e)
        {
            int index = listTextBox.SelectedIndex;

            if (index != -1)
            {
                if (index > 0)
                {
                    string itemInfo = listTextBox.Items[index] as string;
                    listTextBox.Items.RemoveAt(index);
                    listTextBox.Items.Insert(index - 1, itemInfo);
                    listTextBox.SelectedIndex = index - 1;
                }
            }
            else
            {
                MessageBox.Show("请先选择要上移的选项！", "上移选项",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 下移选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bMoveDown_Click(object sender, EventArgs e)
        {
            int index = listTextBox.SelectedIndex;

            if (index != -1)
            {
                if (index < listTextBox.Items.Count - 1)
                {
                    string itemInfo = listTextBox.Items[index] as string;
                    listTextBox.Items.RemoveAt(index);
                    listTextBox.Items.Insert(index + 1, itemInfo);
                    listTextBox.SelectedIndex = index + 1;
                }
            }
            else
            {
                MessageBox.Show("请先选择要下移的选项！", "下移选项",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择数据表
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void tableNameBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (initFinished)
            {
                string tableName = tableNameBox.Text;
                DataTable tableInfo = tableInfoDictionary[tableName];

                foreach (DataColumn dataColumn in tableInfo.Columns)
                {
                    displayFieldBox.Items.Add(dataColumn.ColumnName);
                    valueFieldBox.Items.Add(dataColumn.ColumnName);
                }
            }
        }
    }
}