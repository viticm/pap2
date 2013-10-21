using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Xml;
using System.IO;

using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Controls;

using FieldInfoLib;

namespace GuideTool
{
    public partial class EditForm : Office2007Form
    {
        private SqlConnection conn; // sql连接
        private string guideID; // 向导ID
        private string guideName; // 向导名称

        private Size labelSize = new Size(150, 20);
        private Size itemSize = new Size(150, 20);
        private Size buttonSize = new Size(70, 20);
        private string tabTableName = "sys_guide_tab_config"; // 分页数据表名称
        private string fieldTableName = "sys_guide_field_config"; // 选项数据表名称

        private const int labelIndex = 0;
        private const int boxIndex = 1;
        private const int buttonIndex =2;        

        private int stepIndex = 0; // 当前步骤 
        private Color stepMarkColor = Color.Red;
        private Font markLabelFont = new Font("Sim Sun", 9, FontStyle.Bold);
        private Font normalLabelFont = new Font("Sim Sun", 9, FontStyle.Regular);
        private List<Label> stepLabelList = new List<Label>();
        private List<string> tabDescriptionList = new List<string>();
        private List<List<FieldInfo>> guideData = new List<List<FieldInfo>>(); // 向导数据
        private Dictionary<string, FieldInfo> fieldInfoDictionary = new Dictionary<string, FieldInfo>();        

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="guideID">向导ID</param>
        /// <param name="guideName">向导名称</param>
        public EditForm(SqlConnection conn, string guideID, string guideName)
        {
            this.conn = conn;
            this.guideID = guideID;
            this.guideName = guideName;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 向导数据
        /// </summary>
        public List<List<FieldInfo>> GuideData 
        {
            set
            {
                guideData = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            string sqlString = string.Format("SELECT ID, Name, Description FROM {0} WHERE ClassID = {1}",
                                             tabTableName, guideID);
            DataTable tabTable = Helper.GetDataTable(sqlString, conn);

            int index = 1;

            foreach (DataRow dataRow in tabTable.Rows)
            {
                string tabID = dataRow["ID"].ToString();
                string tabName = dataRow["Name"].ToString();
                string description = dataRow["Description"].ToString();

                Label newLabel = new Label();
                newLabel.Name = tabName;
                newLabel.Text = string.Format("{0} {1}", index, tabName);                
                newLabel.Tag = tabID;
                newLabel.Size = labelSize;
                newLabel.TextAlign = ContentAlignment.MiddleLeft;
                tabPanel.Controls.Add(newLabel);
                stepLabelList.Add(newLabel);
                tabDescriptionList.Add(description);

                List<FieldInfo> fieldInfoList = new List<FieldInfo>();
                guideData.Add(fieldInfoList);

                index++;
            }

            if (stepLabelList.Count > 0)
            {
                MarkStepLabel(stepIndex);
            }

            UpdateTitle();
            LoadFieldInfo();
        }

        /// <summary>
        /// 更新窗体标题
        /// </summary>
        private void UpdateTitle()
        {
            this.Text = string.Format("{0} {1}/{2}", guideName, stepIndex + 1, stepLabelList.Count);
        }

        /// <summary>
        /// 标记步骤标签
        /// </summary>
        /// <param name="index">标签编号</param>
        private void MarkStepLabel(int index)
        {
            Label currentLabel = stepLabelList[index];
            currentLabel.ForeColor = stepMarkColor;
            currentLabel.Font = markLabelFont;
        }

        /// <summary>
        /// 取消标记步骤标签
        /// </summary>
        /// <param name="index">标签编号</param>
        private void UnmarkStepLabel(int index)
        {
            Label currentLabel = stepLabelList[index];
            currentLabel.ForeColor = Color.Black;
            currentLabel.Font = normalLabelFont;
        }

        /// <summary>
        /// 设置按钮的状态
        /// </summary>
        private void SetButtonState()
        {
            if (stepIndex == 0)
            {
                bPreviousStep.Enabled = false;
            }
            else
            {
                bPreviousStep.Enabled = true;
            }

            if (stepIndex == stepLabelList.Count - 1)
            {
                bNextStep.Enabled = false;
                bFinish.Enabled = true;
            }
            else
            {
                bNextStep.Enabled = true;
                bFinish.Enabled = false;
            }
        }

        /// <summary>
        /// 显示分页描述
        /// </summary>
        private void ShowTabDescription()
        {
            string description = tabDescriptionList[stepIndex];
            descriptionBox.Text = description;
        }        

        /// <summary>
        /// 初始化下拉框
        /// </summary>
        /// <param name="comboBox">下拉框控件</param>        
        /// <param name="typeData">类型数据</param>
        private void InitComboBox(ComboBox comboBox, string typeData)
        {
            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode typeNode = xmlDocument["typeData"];

                if (typeNode.ChildNodes.Count > 0)
                {
                    XmlNode listNode = typeNode.ChildNodes[0];
                    string listType = listNode.Attributes["type"].Value;

                    switch (listType)
                    {
                        case "text":
                            {
                                foreach (XmlNode itemNode in listNode.ChildNodes)
                                {
                                    string itemName = itemNode.Attributes["name"].Value;
                                    string itemValue = itemNode.Attributes["value"].Value;

                                    string itemInfo = string.Format("{0} ({1})", itemName, itemValue);
                                    comboBox.Items.Add(itemInfo);
                                }

                                break;
                            }
                        case "database":
                            {
                                string tableName = listNode.Attributes["tableName"].Value;
                                string displayField = listNode.Attributes["displayField"].Value;
                                string valueField = listNode.Attributes["valueField"].Value;

                                string sqlString = string.Format("SELECT [{0}], [{1}] FROM {2}",
                                                                 displayField, valueField, tableName);
                                DataTable infoTable = Helper.GetDataTable(sqlString, conn);

                                foreach (DataRow dataRow in infoTable.Rows)
                                {
                                    string itemName = dataRow[displayField].ToString();
                                    string itemValue = dataRow[valueField].ToString();
                                    string itemInfo = string.Format("{0} ({1})", itemName, itemValue);

                                    comboBox.Items.Add(itemInfo);
                                }

                                break;
                            }
                    }
                }
            }            
        }        

        /// <summary>
        /// 自定义编辑
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void CustomEdit(object sender, EventArgs e)
        {
            ButtonX button = sender as ButtonX;
            TextBox textBox = button.Tag as TextBox;

        }

        /// <summary>
        /// 选择文件
        /// </summary>
        private void BrowseFile(object sender, EventArgs e)
        {            
            ButtonX button = sender as ButtonX;
            TextBox textBox = button.Tag as TextBox;

            string fileName = textBox.Text;
            string id = textBox.Tag as string;

            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "浏览文件";
            
            if (fileName != "" && File.Exists(fileName))
            {
                openFileDialog.FileName = fileName;
            }

            FieldInfo fieldInfo = fieldInfoDictionary[id];
            string typeData = fieldInfo.TypeData;
            string fileFolder = null;

            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode typeNode = xmlDocument["typeData"];
                fileFolder = typeNode.Attributes["fileFolder"].Value;

                if (!string.IsNullOrEmpty(fileFolder))
                {
                    // openFileDialog.InitialDirectory = fileFolder;
                }
            }
            
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                fileName = openFileDialog.FileName;
                
                if (!string.IsNullOrEmpty(fileFolder))
                {
                    fileName = fileName.Replace("fileFolder", "").TrimStart(new char[] { '\\' });                    
                }

                textBox.Text = fileName;
            }
        }        

        /// <summary>
        /// 加载向导信息
        /// </summary>
        private void LoadGuideInfo()
        {

        }

        /// <summary>
        /// 保存向导信息
        /// </summary>
        private void SaveGuideInfo()
        {
            string fileName = Path.Combine(Application.StartupPath, string.Format("{0}.GuideInfo", guideName));
            byte[] buffer = Helper.SeralizeData(guideData);
            Helper.SaveFile(fileName, buffer);

            MessageBox.Show(string.Format("向导数据文件 {0} 保存成功！", fileName), "保存向导数据",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /// <summary>
        /// 加载默认值
        /// </summary>
        /// <param name="newItem">当前控件</param>
        /// <param name="fieldType">选项类型</param>
        private void LoadDefaultValue(Control newItem, string fieldType, string defaultValue)
        {
            string newValue = defaultValue;

            switch (fieldType)
            {
                case "list":
                    {
                        ComboBox comboBox = newItem as ComboBox;

                        foreach (string listItem in comboBox.Items)
                        {
                            string[] itemInfo = Helper.GetListItemInfo(listItem);
                            string itemName = itemInfo[0];
                            string itemValue = itemInfo[1];

                            if (itemValue == defaultValue)
                            {
                                newValue = listItem;
                                break;
                            }
                        }

                        break;
                    }
                default:
                    {                        
                        break;
                    }
            }

            newItem.Text = newValue;
        }

        /// <summary>
        /// 加载选项信息
        /// </summary>
        private void LoadFieldInfo()
        {
            fieldPanel.Controls.Clear();

            Label currentLabel = stepLabelList[stepIndex];
            string tabID = currentLabel.Tag as string;
            string sqlString = string.Format("SELECT * FROM {0} WHERE TabID = {1} ORDER BY OrderIndex",
                                             fieldTableName, tabID);
            DataTable fieldTable = Helper.GetDataTable(sqlString, conn);

            bool inited = false;
            List<FieldInfo> fieldInfoList = guideData[stepIndex];

            if (fieldInfoList.Count > 0)
            {
                inited = true;
            }

            for (int i = 0; i < fieldTable.Rows.Count; i++)
            {
                DataRow dataRow = fieldTable.Rows[i];
                string fieldID = dataRow["ID"].ToString();
                string fieldName = dataRow["Name"].ToString();
                string fieldType = dataRow["Type"].ToString();
                string typeData = dataRow["TypeData"].ToString();
                string defaultValue = dataRow["DefaultValue"].ToString();
                bool allowNullValue = true;

                if (!Helper.IsNullOrDBNull(dataRow["AllowNullValue"]))
                {
                    allowNullValue = (bool)dataRow["AllowNullValue"];
                }

                Label fieldLabel = new Label();
                fieldLabel.Name = string.Format("{0}Label", fieldName);
                fieldLabel.Text = fieldName;
                fieldLabel.TextAlign = ContentAlignment.MiddleLeft;
                fieldLabel.Size = labelSize;
                fieldPanel.Controls.Add(fieldLabel, labelIndex, i);                   
                Control newItem = null;

                switch (fieldType)
                {
                    case "text":
                        {
                            TextBox textBox = new TextBox();
                            newItem = textBox;                            

                            break;
                        }
                    case "number":
                        {
                            TextBox textBox = new TextBox();
                            newItem = textBox;                           

                            break;
                        }
                    case "boolean":
                        {
                            ComboBox comboBox = new ComboBox();
                            comboBox.Items.Add("是");
                            comboBox.Items.Add("否");
                            comboBox.DropDownStyle = ComboBoxStyle.DropDownList;
                            newItem = comboBox;

                            break;
                        }
                    case "list":
                        {
                            ComboBox comboBox = new ComboBox();
                            comboBox.DropDownStyle = ComboBoxStyle.DropDownList;
                            newItem = comboBox;

                            InitComboBox(comboBox, typeData);                         

                            break;
                        }
                    case "file":
                        {
                            TextBox textBox = new TextBox();
                            newItem = textBox;

                            ButtonX button = new ButtonX();
                            button.Name = string.Format("{0}Button", fieldName);
                            button.Text = "浏览";
                            button.Size = buttonSize;
                            button.Tag = textBox;
                            button.Click += BrowseFile;
                            fieldPanel.Controls.Add(button, 2, i);

                            break;
                        }
                    case "custom":
                        {
                            TextBox textBox = new TextBox();
                            newItem = textBox;

                            ButtonX button = new ButtonX();                             
                            button.Name = string.Format("{0}Button", fieldName);
                            button.Text = "编辑";
                            button.Size = buttonSize;
                            button.Tag = textBox;
                            button.Click += CustomEdit;
                            fieldPanel.Controls.Add(button, 2, i);

                            break;
                        }
                }

                if (inited)
                {
                    FieldInfo fieldInfo = fieldInfoList[i];
                    newItem.Text = fieldInfo.FieldValue;                    
                }
                else
                {
                    FieldInfo fieldInfo = new FieldInfo(fieldID, fieldName, fieldType, typeData, defaultValue, allowNullValue);
                    fieldInfoList.Add(fieldInfo);

                    fieldInfoDictionary[fieldID] = fieldInfo;

                    LoadDefaultValue(newItem, fieldType, defaultValue);
                }

                newItem.Name = fieldName;
                newItem.Tag = fieldID;
                newItem.Size = itemSize;
                fieldPanel.Controls.Add(newItem, boxIndex, i);
            }
        }

        /// <summary>
        /// 保存分页数据
        /// </summary>
        /// <return>是否保存成功</return>
        private bool SaveFieldInfo()
        {
            bool saveSuccess = true;
            List<FieldInfo> fieldInfoList = guideData[stepIndex];

            for (int i = 0; i < fieldInfoList.Count; i++)
            {
                FieldInfo fieldInfo = fieldInfoList[i];
                
                string fieldName = fieldInfo.FieldName;
                string fieldType = fieldInfo.FieldType;
                string typeData = fieldInfo.TypeData;
                string fieldValue = "";

                switch (fieldType)
                {
                    case "text":
                        {
                            TextBox textBox = fieldPanel.GetControlFromPosition(boxIndex, i) as TextBox;
                            fieldValue = textBox.Text;

                            break;
                        }
                    case "number":
                        {
                            TextBox textBox = fieldPanel.GetControlFromPosition(boxIndex, i) as TextBox;
                            fieldValue = textBox.Text;
                            string numberType = Helper.GetNumberType(typeData);                                                       

                            switch (numberType)
                            {
                                case "integer":
                                    {
                                        int tempInt;
                                        saveSuccess = int.TryParse(fieldValue, out tempInt);

                                        break;
                                    }
                                case "float":
                                    {
                                        float tempFloat;
                                        saveSuccess = float.TryParse(fieldValue, out tempFloat);

                                        break;
                                    }
                            }

                            if (!saveSuccess)
                            {
                                MessageBox.Show(string.Format("{0}参数类型不正确！", fieldName), "保存参数",
                                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                            }

                            break;
                        }
                    case "boolean":
                        {
                            ComboBox comboBox = fieldPanel.GetControlFromPosition(boxIndex, i) as ComboBox;
                            fieldValue = comboBox.Text;

                            if (string.IsNullOrEmpty(fieldValue))
                            {
                                saveSuccess = false;
                                MessageBox.Show(string.Format("{0}值不能为空！", fieldName), "保存参数",
                                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                            }

                            break;
                        }
                    case "list":
                        {
                            ComboBox comboBox = fieldPanel.GetControlFromPosition(boxIndex, i) as ComboBox;
                            fieldValue = comboBox.Text;                            

                            break;
                        }
                    case "file":
                        {
                            TextBox textBox = fieldPanel.GetControlFromPosition(boxIndex, i) as TextBox;
                            fieldValue = textBox.Text;

                            break;
                        }
                    case "custom":
                        {
                            TextBox textBox = fieldPanel.GetControlFromPosition(boxIndex, i) as TextBox;
                            fieldValue = textBox.Text;

                            break;
                        }
                }

                if (saveSuccess)
                {
                    if (!fieldInfo.AllowNullValue && string.IsNullOrEmpty(fieldValue))
                    {
                        saveSuccess = false;
                        MessageBox.Show(string.Format("{0}值不能为空！", fieldName), "保存参数",
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        fieldInfo.FieldValue = fieldValue;
                    }                    
                }                
            }

            return saveSuccess;
        }

        /// <summary>
        /// 上一步
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bPreviousStep_Click(object sender, EventArgs e)
        {
            if (stepIndex > 0)
            {
                bool saveSuccess = SaveFieldInfo();

                if (saveSuccess)
                {
                    UnmarkStepLabel(stepIndex);
                    stepIndex--;
                    MarkStepLabel(stepIndex);
                    SetButtonState();
                    UpdateTitle();

                    LoadFieldInfo();
                }                
            }            
        }

        /// <summary>
        /// 下一步
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bNextStep_Click(object sender, EventArgs e)
        {
            if (stepIndex < stepLabelList.Count - 1)
            {
                bool saveSuccess = SaveFieldInfo();

                if (saveSuccess)
                {
                    UnmarkStepLabel(stepIndex);
                    stepIndex++;
                    MarkStepLabel(stepIndex);
                    SetButtonState();
                    UpdateTitle();

                    LoadFieldInfo();
                }                
            }
        }

        /// <summary>
        /// 完成
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bFinish_Click(object sender, EventArgs e)
        {
            bool saveSuccess = SaveFieldInfo();

            if (saveSuccess)
            {
                SaveGuideInfo();

                DialogResult = DialogResult.OK;
                this.Close();
            }            
        }

        /// <summary>
        /// 取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定要取消并退出当前向导吗？", "取消配置",
                                MessageBoxButtons.OKCancel, MessageBoxIcon.Information) 
                == DialogResult.OK)
            {
                DialogResult = DialogResult.Cancel;
                this.Close();
            }
        }
    }
}