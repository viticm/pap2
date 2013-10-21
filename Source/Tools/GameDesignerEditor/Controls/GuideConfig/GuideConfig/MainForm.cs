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
using DevComponents.AdvTree;

namespace GuideConfig
{
    public partial class MainForm : Office2007Form
    {
        private SqlConnection conn; // sql连接
        private DataTable classTable;
        private DataTable tabTable;
        private DataTable fieldTable;

        private bool classInfoChanged = false;
        private bool tabInfoChanged = false;
        private bool fieldInfoChanged = false;

        private const string classTableName = "sys_guide_class_config";
        private const string tabTableName = "sys_guide_tab_config";
        private const string fieldTableName = "sys_guide_field_config";

        private Node lastSelectedNode = null; // 上一次选中的树结点，保存时用

        private enum FieldType { Text, Number, Boolean, List, File, Custom }; // 选项类型

        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// sql连接
        /// </summary>
        public SqlConnection Connection
        {
            set
            {
                conn = value;
            }
        }

        /// <summary>
        /// 重新加载向导数据表
        /// </summary>
        private void ReloadClassTable()
        {
            string sqlString = string.Format("SELECT ID, Name FROM {0}", classTableName);
            classTable = Helper.GetDataTable(sqlString, conn);
        }

        /// <summary>
        /// 重新加载分页数据表
        /// </summary>
        private void ReloadTabTable()
        {
            string sqlString = string.Format("SELECT ID, Name, ClassID, OrderIndex FROM {0}", tabTableName);
            tabTable = Helper.GetDataTable(sqlString, conn);
        }

        /// <summary>
        /// 重新加载选项数据表
        /// </summary>
        private void ReloadFieldTable()
        {
            string sqlString = string.Format("SELECT ID, Name, TabID, OrderIndex FROM {0}", fieldTableName);
            fieldTable = Helper.GetDataTable(sqlString, conn);
        }

        /// <summary>
        /// 转换选项类型
        /// </summary>
        /// <param name="typeInfo">描述字符串</param>
        /// <returns>选项类型</returns>
        private FieldType ConvertFieldType(string typeInfo)
        {
            FieldType fieldType = FieldType.Text;

            switch (typeInfo)
            {
                case "text":
                    {
                        fieldType = FieldType.Text;
                        break;
                    }
                case "number":
                    {
                        fieldType = FieldType.Number;
                        break;
                    }
                case "boolean":
                    {
                        fieldType = FieldType.Boolean;
                        break;
                    }
                case "list":
                    {
                        fieldType = FieldType.List;
                        break;
                    }
                case "file":
                    {
                        fieldType = FieldType.File;
                        break;
                    }
                case "custom":
                    {
                        fieldType = FieldType.Custom;
                        break;
                    }
            }

            return fieldType;
        }

        /// <summary>
        /// 转换选项类型
        /// </summary>
        /// <param name="fieldType">选项类型</param>
        /// <returns>描述字符串</returns>
        private string ConvertFieldType(FieldType fieldType)
        {
            string typeInfo = "";

            switch (fieldType)
            {
                case FieldType.Text:
                    {
                        typeInfo = "text";
                        break;
                    }
                case FieldType.Number:
                    {
                        typeInfo = "number";
                        break;
                    }
                case FieldType.Boolean:
                    {
                        typeInfo = "boolean";
                        break;
                    }
                case FieldType.List:
                    {
                        typeInfo = "list";
                        break;
                    }
                case FieldType.File:
                    {
                        typeInfo = "file";
                        break;
                    }
                case FieldType.Custom:
                    {
                        typeInfo = "custom";
                        break;
                    }
            }

            return typeInfo;
        }

        /// <summary>
        /// 重新加载分类树
        /// </summary>
        private void ReloadTree()
        {
            guideTree.Nodes.Clear();

            // 读取树结点
            Node rootNode = new Node();
            rootNode.Text = "向导分类";
            rootNode.ContextMenu = bRootMenu;
            guideTree.Nodes.Add(rootNode);

            foreach (DataRow classRow in classTable.Rows)
            {
                string className = classRow["Name"].ToString();
                string classID = classRow["ID"].ToString();

                Node classNode = new Node();
                classNode.Text = className;
                classNode.Tag = classID;
                classNode.ContextMenu = bClassMenu;

                DataRow[] tabRows = tabTable.Select(string.Format("ClassID = {0}", classID), "OrderIndex");

                foreach (DataRow tabRow in tabRows)
                {
                    string tabName = tabRow["Name"].ToString();
                    string tabID = tabRow["ID"].ToString();

                    Node tabNode = new Node();
                    tabNode.Text = tabName;
                    tabNode.Tag = tabID;
                    tabNode.ContextMenu = bTabMenu;

                    DataRow[] fieldRows = fieldTable.Select(string.Format("TabID = {0}", tabID), "OrderIndex");

                    foreach (DataRow fieldRow in fieldRows)
                    {
                        string fieldName = fieldRow["Name"].ToString();
                        string fieldID = fieldRow["ID"].ToString();

                        Node fieldNode = new Node();
                        fieldNode.Text = fieldName;
                        fieldNode.Tag = fieldID;
                        fieldNode.ContextMenu = bFieldMenu;

                        tabNode.Nodes.Add(fieldNode);
                    }

                    classNode.Nodes.Add(tabNode);
                }

                rootNode.Nodes.Add(classNode);
            }

            rootNode.Expand();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 初始化sql连接
            string connectString = "Server = jx3web; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            conn = new SqlConnection(connectString);

            // 加载数据表
            ReloadClassTable();
            ReloadTabTable();
            ReloadFieldTable();

            // 加载分类树
            ReloadTree();            
        }

        /// <summary>
        /// 输出异常消息
        /// </summary>
        /// <param name="ex">当前异常</param>
        private void PrintExceptionMessage(Exception ex)
        {
            MessageBox.Show(string.Format("当前操作产生异常：{0}", ex.Message, "操作失败",
                                          MessageBoxButtons.OK, MessageBoxIcon.Error));
        }

        /// <summary>
        /// 配置下拉框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bConfigList_Click(object sender, EventArgs e)
        {
            string listType = "text";

            if (rListDatabaseType.Checked)
            {
                listType = "database";
            }

            string listData = listDataBox.Text;

            ListConfigForm listConfigForm = new ListConfigForm(conn, listType, listData);

            if (listConfigForm.ShowDialog() == DialogResult.OK)
            {
                listDataBox.Text = listConfigForm.ConfigInfo;
            }
        }

        /// <summary>
        /// 切换字段类型
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void ChangeFieldType(object sender, EventArgs e)
        {
            RadioButton radioButton = sender as RadioButton;

            if (radioButton.Checked)
            {
                switch (radioButton.Text)
                {
                    case "文本":
                        {
                            fieldConfigControl.SelectedTabIndex = 0;
                            break;
                        }
                    case "数字":
                        {
                            fieldConfigControl.SelectedTabIndex = 1;
                            break;
                        }
                    case "下拉框":
                        {
                            fieldConfigControl.SelectedTabIndex = 2;
                            break;
                        }
                    case "文件":
                        {
                            fieldConfigControl.SelectedTabIndex = 3;
                            break;
                        }
                    default:
                        {
                            fieldConfigControl.SelectedTabIndex = 4;
                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 打开sql连接
        /// </summary>
        private void OpenConnection()
        {
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
        }

        /// <summary>
        /// 关闭sql连接
        /// </summary>
        private void CloseConnection()
        {
            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }
        }

        /// <summary>
        /// 执行sql命令
        /// </summary>
        /// <param name="sqlString">sql命令</param>
        private void ExecuteSqlCommand(string sqlString)
        {
            try
            {
                OpenConnection();

                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                PrintExceptionMessage(ex);
            }
            finally
            {
                CloseConnection();
            }
        }

        /// <summary>
        /// 保存分类信息
        /// </summary>
        /// <param name="currentNode">当前树结点</param>
        private void SaveClassInfo(Node currentNode)
        {
            if (classInfoChanged)
            {
                string id = currentNode.Tag as string;
                string className = classNameBox.Text;
                string classDescription = classDescriptionBox.Text;
                string sqlString = null;

                if (className != "")
                {
                    sqlString = string.Format("UPDATE {0} SET Name = '{1}' WHERE ID = {2}",
                                              classTableName, className, id);
                    ExecuteSqlCommand(sqlString);

                    currentNode.Text = className;
                }

                if (classDescription != "")
                {
                    sqlString = string.Format("UPDATE {0} SET Description = '{1}' WHERE ID = {2}",
                                              classTableName, classDescription, id);
                    ExecuteSqlCommand(sqlString);
                }
            }            
        }

        /// <summary>
        /// 保存分页信息
        /// </summary>
        /// <param name="currentNode">当前树结点</param>
        private void SaveTabInfo(Node currentNode)
        {
            if (tabInfoChanged)
            {
                string id = currentNode.Tag as string;
                string tabName = tabNameBox.Text;
                string tabDescription = tabDescriptionBox.Text;
                string sqlString = null;

                if (tabName != "")
                {
                    sqlString = string.Format("UPDATE {0} SET Name = '{1}' WHERE ID = {2}",
                                              tabTableName, tabName, id);
                    ExecuteSqlCommand(sqlString);

                    currentNode.Text = tabName;
                }

                if (tabDescription != "")
                {
                    sqlString = string.Format("UPDATE {0} SET Description = '{1}' WHERE ID = {2}",
                                              tabTableName, tabDescription, id);
                    ExecuteSqlCommand(sqlString);
                }
            }            
        }

        /// <summary>
        /// 保存选项信息
        /// </summary>
        /// <param name="currentNode">当前树结点</param>
        private void SaveFieldInfo(Node currentNode)
        {
            if (fieldInfoChanged)
            {
	            string id = currentNode.Tag as string;
	            string fieldName = fieldNameBox.Text;
	            string fieldDescription = fieldDescriptionBox.Text;
	            string sqlString = null;
	
	            if (fieldName != "")
	            {
	                sqlString = string.Format("UPDATE {0} SET Name = '{1}' WHERE ID = {2}",
	                                          fieldTableName, fieldName, id);
	                ExecuteSqlCommand(sqlString);
	
	                currentNode.Text = fieldName;
	            }
	
	            if (fieldDescription != "")
	            {
	                sqlString = string.Format("UPDATE {0} SET Description = '{1}' WHERE ID = {2}",
	                                          fieldTableName, fieldDescription, id);
	                ExecuteSqlCommand(sqlString);
	            }

                FieldType fieldType = FieldType.Text;
                string typeData = "";

                if (rTextType.Checked)
                {
                    fieldType = FieldType.Text;
                    int maxLength = textLengthInput.Value;
                    typeData = string.Format("<typeData maxLength = \"{0}\"></typeData>", maxLength);
                }
                else if (rNumberType.Checked)
                {
                    fieldType = FieldType.Number;
                    string numberType = "integer";

                    if (rFloatType.Checked)
                    {
                        numberType = "float";
                    }

                    typeData = string.Format("<typeData numberType = \"{0}\"></typeData>", numberType);
                }
                else if (rBooleanType.Checked)
                {
                    fieldType = FieldType.Boolean;
                }
                else if (rListType.Checked)
                {
                    fieldType = FieldType.List;
                    typeData = string.Format("<typeData>{0}</typeData>", listDataBox.Text);
                }
                else if (rFileType.Checked)
                {
                    fieldType = FieldType.File;
                    string fileFolder = fileFolderBox.Text;
                    typeData = string.Format("<typeData fileFolder = \"{0}\"></typeData>", fileFolder);
                }
                else if (rCustomType.Checked)
                {
                    fieldType = FieldType.Custom;
                }
                
                string typeInfo = ConvertFieldType(fieldType);
                sqlString = string.Format("UPDATE {0} SET Type = '{1}', TypeData = '{2}' WHERE ID = {3}",
                                          fieldTableName, typeInfo, typeData, id);
                ExecuteSqlCommand(sqlString);
            }
        }

        /// <summary>
        /// 保存记录
        /// </summary>
        private void SaveRecord()
        {
            Node currentNode = lastSelectedNode;

            if (currentNode != null)
            {
                switch (currentNode.Level)
                {
                    case 1: // 分类
                        {
                            SaveClassInfo(currentNode);

                            break;
                        }
                    case 2: // 分页
                        {
                            SaveTabInfo(currentNode);

                            break;
                        }
                    case 3: // 记录
                        {
                            SaveFieldInfo(currentNode);

                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 加载分类信息
        /// </summary>
        /// <param name="id">ID</param>
        private void LoadClassInfo(string id)
        {
            string sqlString = string.Format("SELECT Name, Description FROM {0} WHERE ID = {1}", 
                                             classTableName, id);
            DataTable infoTable = Helper.GetDataTable(sqlString, conn);

            string className = "";
            string classDescription = "";

            if (infoTable.Rows.Count > 0)
            {
                DataRow currentRow = infoTable.Rows[0];
                className = currentRow["Name"].ToString();
                classDescription = currentRow["Description"].ToString();
            }

            classNameBox.Text = className;
            classDescriptionBox.Text = classDescription;
            classInfoChanged = false;
        }

        /// <summary>
        /// 加载分页信息
        /// </summary>
        /// <param name="id">ID</param>
        private void LoadTabInfo(string id)
        {
            string sqlString = string.Format("SELECT Name, Description FROM {0} WHERE ID = {1}", 
                                             tabTableName, id);
            DataTable infoTable = Helper.GetDataTable(sqlString, conn);

            string tabName = "";
            string tabDescription = "";

            if (infoTable.Rows.Count > 0)
            {
                DataRow currentRow = infoTable.Rows[0];
                tabName = currentRow["Name"].ToString();
                tabDescription = currentRow["Description"].ToString();
            }

            tabNameBox.Text = tabName;
            tabDescriptionBox.Text = tabDescription;
            tabInfoChanged = false;
        }

        /// <summary>
        /// 加载文本选项的信息
        /// </summary>
        /// <param name="typeData">信息字符串</param>
        private void LoadTextFieldInfo(string typeData)
        {
            int maxLength = 100;

            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode xmlNode = xmlDocument["typeData"];
                string lengthInfo = xmlNode.Attributes["maxLength"].Value;
                maxLength = int.Parse(lengthInfo);
            }
            
            textLengthInput.Value = maxLength;
        }

        /// <summary>
        /// 加载数字选项的信息
        /// </summary>
        /// <param name="typeData">信息字符串</param>
        private void LoadNumberFieldInfo(string typeData)
        {
            string numberType = "number";

            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode xmlNode = xmlDocument["typeData"];
                numberType = xmlNode.Attributes["numberType"].Value;
            }            

            switch (numberType)
            {
                case "number":
                    {
                        rIntegerType.Checked = true;
                        break;
                    }
                case "float":
                    {
                        rFloatType.Checked = true;
                        break;
                    }
            }
        }

        /// <summary>
        /// 加载下拉框选项的信息
        /// </summary>
        /// <param name="typeData">信息字符串</param>
        private void LoadListFieldInfo(string typeData)
        {
            string listData = "";

            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode xmlNode = xmlDocument["typeData"];
                listData = xmlNode.InnerXml;                
            }
            
            listDataBox.Text = listData;
        }

        /// <summary>
        /// 加载文件选项的信息
        /// </summary>
        /// <param name="typeData"></param>
        private void LoadFileFieldInfo(string typeData)
        {
            string fileFolder = "";

            if (!string.IsNullOrEmpty(typeData))
            {
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(typeData);
                XmlNode xmlNode = xmlDocument["typeData"];
                fileFolder = xmlNode.Attributes["fileFolder"].Value;
            }
            
            fileFolderBox.Text = fileFolder;
        }

        /// <summary>
        /// 加载选项信息
        /// </summary>
        /// <param name="id">ID</param>
        private void LoadFieldInfo(string id)
        {
            string sqlString = string.Format("SELECT * FROM {0} WHERE ID = {1}", 
                                             fieldTableName, id);
            DataTable infoTable = Helper.GetDataTable(sqlString, conn);

            string fieldName = "";
            string fieldDescription = "";

            if (infoTable.Rows.Count > 0)
            {
                DataRow currentRow = infoTable.Rows[0];
                fieldName = currentRow["Name"].ToString();
                fieldDescription = currentRow["Description"].ToString();
                string typeInfo = currentRow["Type"].ToString();
                FieldType fieldType = ConvertFieldType(typeInfo);
                string typeData = currentRow["TypeData"].ToString();

                switch (fieldType)
                {
                    case FieldType.Text:
                        {
                            rTextType.Checked = true;
                            LoadTextFieldInfo(typeData);

                            break;
                        }
                    case FieldType.Number:
                        {
                            rNumberType.Checked = true;
                            LoadNumberFieldInfo(typeData);

                            break;
                        }
                    case FieldType.Boolean:
                        {
                            rBooleanType.Checked = true;

                            break;
                        }
                    case FieldType.List:
                        {
                            rListType.Checked = true;
                            LoadListFieldInfo(typeData);

                            break;
                        }
                    case FieldType.File:
                        {
                            rFileType.Checked = true;
                            LoadFileFieldInfo(typeData);

                            break;
                        }
                    case FieldType.Custom:
                        {
                            rCustomType.Checked = true;
                            break;
                        }
                }
            }

            fieldNameBox.Text = fieldName;
            fieldDescriptionBox.Text = fieldDescription;
            fieldInfoChanged = true;
        }

        /// <summary>
        /// 加载记录
        /// </summary>
        private void LoadRecord()
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                string id = currentNode.Tag as string;

                switch (currentNode.Level)
                {
                    case 0:
                        {
                            configControl.SelectedTabIndex = 3;
                            break;
                        }
                    case 1:
                        {
                            LoadClassInfo(id);
                            configControl.SelectedTabIndex = 0;

                            break;
                        }
                    case 2:
                        {
                            LoadTabInfo(id);
                            configControl.SelectedTabIndex = 1;

                            break;
                        }
                    case 3:
                        {
                            LoadFieldInfo(id);
                            configControl.SelectedTabIndex = 2;

                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 删除记录
        /// </summary>        
        private void DeleteRecord()
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                string id = currentNode.Tag as string;

                switch (currentNode.Level)
                {
                    case 1: // 向导
                        {
                            List<Node> tabNodeList = new List<Node>();

                            foreach (Node tabNode in currentNode.Nodes)
                            {
                                tabNodeList.Add(tabNode);
                            }

                            foreach (Node tabNode in tabNodeList)
                            {
                                List<Node> fieldNodeList = new List<Node>();

                                foreach (Node fieldNode in tabNode.Nodes)
                                {
                                    fieldNodeList.Add(fieldNode);
                                }

                                foreach (Node fieldNode in fieldNodeList)
                                {
                                    DeleteRecord(fieldNode, fieldTableName);
                                }

                                DeleteRecord(tabNode, tabTableName);
                            }

                            DeleteRecord(currentNode, classTableName);

                            break;
                        }
                    case 2: // 分页
                        {
                            List<Node> fieldNodeList = new List<Node>();
                            
                            foreach (Node fieldNode in currentNode.Nodes)
                            {
                                fieldNodeList.Add(fieldNode);
                            }
                            
                            foreach (Node fieldNode in fieldNodeList)
                            {
                                DeleteRecord(fieldNode, fieldTableName);
                            }

                            DeleteRecord(currentNode, tabTableName);

                            break;
                        }
                    case 3: // 选项
                        {
                            DeleteRecord(currentNode, fieldTableName);

                            break;
                        }
                }
            }
        }

        /// <summary>
        /// 删除记录
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <param name="tableName">数据表名</param>
        private void DeleteRecord(Node node, string tableName)
        {
            string id = node.Tag as string;
            string sqlString = string.Format("DELETE FROM {0} WHERE ID = {1}",
                                             tableName, id);

            ExecuteSqlCommand(sqlString);            
            node.Remove();
        }

        /// <summary>
        /// 排列记录
        /// </summary>
        /// <param name="tableName">数据表名</param>
        private void SortRecord(string tableName)
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                List<string> recordList = new List<string>();
                Dictionary<string, string> idDictionary = new Dictionary<string, string>();

                foreach (Node childNode in currentNode.Nodes)
                {
                    string nodeName = childNode.Text;
                    string nodeID = childNode.Tag as string;
                    idDictionary[nodeName] = nodeID;

                    recordList.Add(nodeName);
                }

                SortForm sortForm = new SortForm(recordList);

                if (sortForm.ShowDialog() == DialogResult.OK)
                {
                    // 数据记录排序
                    List<string> idList = new List<string>();

                    foreach (string recordName in sortForm.RecordList)
                    {
                        idList.Add(idDictionary[recordName]);
                    }

                    SetRecordOrder(tableName, idList);

                    // 树结点排序
                    Node[] nodeArray = new Node[idList.Count];

                    foreach (Node childNode in currentNode.Nodes)
                    {
                        string nodeID = childNode.Tag as string;
                        int index = idList.IndexOf(nodeID);
                        nodeArray[index] = childNode;
                    }

                    currentNode.Nodes.Clear();

                    foreach (Node node in nodeArray)
                    {
                        currentNode.Nodes.Add(node);
                    }
                }
            }
        }

        /// <summary>
        /// 设置记录的顺序
        /// </summary>
        /// <param name="tableName">数据表名</param>
        /// <param name="idList">ID链表</param>
        private void SetRecordOrder(string tableName, List<string> idList)
        {
            try
            {
                OpenConnection();

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = null;

                for (int i = 0; i < idList.Count; i++)
                {
                    string recordID = idList[i];
                    sqlString = string.Format("UPDATE {0} SET OrderIndex = {1} WHERE ID = {2}",
                                              tableName, i, recordID);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }
            }
            catch (SqlException ex)
            {
                PrintExceptionMessage(ex);
            }
            finally
            {
                CloseConnection();
            }
        }

        /// <summary>
        /// 新建向导
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddClass_Click(object sender, EventArgs e)
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                string nodeName = string.Format("向导{0}", currentNode.Nodes.Count + 1);
                AddNodeForm addNodeForm = new AddNodeForm("新建向导", nodeName, "");

                if (addNodeForm.ShowDialog() == DialogResult.OK)
                {
                    nodeName = addNodeForm.NodeName;
                    string nodeDescription = addNodeForm.NodeDescription;
                    string id = null;

                    SqlTransaction transaction = null;                    

                    try
                    {
                        OpenConnection();

                        transaction = conn.BeginTransaction();
                        SqlCommand cmd = conn.CreateCommand();
                        cmd.Transaction = transaction;

                        string sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'", 
                                                         classTableName, nodeName);
                        cmd.CommandText = sqlString;

                        object executeResult = cmd.ExecuteScalar();

                        if (executeResult == null)
                        {
                            sqlString = string.Format("INSERT INTO {0} (Name, Description) VALUES ('{1}', '{2}')",
                                                      classTableName, nodeName, nodeDescription);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'",
                                                      classTableName, nodeName);
                            cmd.CommandText = sqlString;
                            executeResult = cmd.ExecuteScalar();
                            id = executeResult.ToString();
                        }

                        transaction.Commit();
                    }
                    catch (SqlException ex)
                    {
                        if (transaction != null)
                        {
                            transaction.Rollback();
                        }

                        PrintExceptionMessage(ex);
                    }
                    finally
                    {
                        CloseConnection();
                    }

                    if (id != null)
                    {
                        Node newNode = new Node();
                        newNode.Text = nodeName;
                        newNode.Tag = id;
                        newNode.ContextMenu = bClassMenu;
                        currentNode.Nodes.Add(newNode);
                        guideTree.SelectedNode = newNode;
                    }
                }                                
            }
        }

        /// <summary>
        /// 新建分页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddTab_Click(object sender, EventArgs e)
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                int orderIndex = currentNode.Nodes.Count;
                string nodeName = string.Format("分页{0}", orderIndex + 1);
                AddNodeForm addNodeForm = new AddNodeForm("新建分页", nodeName, "");

                if (addNodeForm.ShowDialog() == DialogResult.OK)
                {
                    nodeName = addNodeForm.NodeName;
                    string nodeDescription = addNodeForm.NodeDescription;
                    string classID = currentNode.Tag as string;
                    string id = null;

                    SqlTransaction transaction = null;

                    try
                    {
                        OpenConnection();

                        transaction = conn.BeginTransaction();
                        SqlCommand cmd = conn.CreateCommand();
                        cmd.Transaction = transaction;

                        string sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'", 
                                                         tabTableName, nodeName);
                        cmd.CommandText = sqlString;

                        object executeResult = cmd.ExecuteScalar();

                        if (executeResult == null)
                        {
                            sqlString = string.Format("INSERT INTO {0} (Name, Description, ClassID, OrderIndex) VALUES ('{1}', '{2}', {3}, {4})",
                                                      tabTableName, nodeName, nodeDescription, classID, orderIndex);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'",
                                                      tabTableName, nodeName);
                            cmd.CommandText = sqlString;
                            executeResult = cmd.ExecuteScalar();
                            id = executeResult.ToString();
                        }

                        transaction.Commit();
                    }
                    catch (SqlException ex)
                    {
                        if (transaction != null)
                        {
                            transaction.Rollback();
                        }

                        PrintExceptionMessage(ex);
                    }
                    finally
                    {
                        CloseConnection();
                    }

                    if (id != null)
                    {
                        Node newNode = new Node();
                        newNode.Text = nodeName;
                        newNode.Tag = id;
                        newNode.ContextMenu = bTabMenu;
                        currentNode.Nodes.Add(newNode);
                        guideTree.SelectedNode = newNode;
                    }
                }
            }
        }

        /// <summary>
        /// 新建选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAddField_Click(object sender, EventArgs e)
        {
            Node currentNode = guideTree.SelectedNode;

            if (currentNode != null)
            {
                int orderIndex = currentNode.Nodes.Count;
                string nodeName = string.Format("选项{0}", orderIndex + 1);
                AddNodeForm addNodeForm = new AddNodeForm("新建选项", nodeName, "");

                if (addNodeForm.ShowDialog() == DialogResult.OK)
                {
                    nodeName = addNodeForm.NodeName;
                    string nodeDescription = addNodeForm.NodeDescription;
                    string tabID = currentNode.Tag as string;
                    string id = null;

                    SqlTransaction transaction = null;

                    try
                    {
                        OpenConnection();

                        transaction = conn.BeginTransaction();
                        SqlCommand cmd = conn.CreateCommand();
                        cmd.Transaction = transaction;

                        string sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'", 
                                                         fieldTableName, nodeName);
                        cmd.CommandText = sqlString;

                        object executeResult = cmd.ExecuteScalar();

                        if (executeResult == null)
                        {
                            sqlString = string.Format("INSERT INTO {0} (Name, Description, TabID, OrderIndex) VALUES ('{1}', '{2}', {3}, {4})",
                                                      fieldTableName, nodeName, nodeDescription, tabID, orderIndex);
                            cmd.CommandText = sqlString;
                            cmd.ExecuteNonQuery();

                            sqlString = string.Format("SELECT ID FROM {0} WHERE Name = '{1}'",
                                                      fieldTableName, nodeName);
                            cmd.CommandText = sqlString;
                            executeResult = cmd.ExecuteScalar();
                            id = executeResult.ToString();
                        }

                        transaction.Commit();
                    }
                    catch (SqlException ex)
                    {
                        if (transaction != null)
                        {
                            transaction.Rollback();
                        }

                        PrintExceptionMessage(ex);
                    }
                    finally
                    {
                        CloseConnection();
                    }

                    if (id != null)
                    {
                        Node newNode = new Node();
                        newNode.Text = nodeName;
                        newNode.Tag = id;
                        newNode.ContextMenu = bFieldMenu;
                        currentNode.Nodes.Add(newNode);
                        guideTree.SelectedNode = newNode;
                    }
                }
            }
        }

        /// <summary>
        /// 排列分页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSortTab_Click(object sender, EventArgs e)
        {
            SortRecord(tabTableName);
        }

        /// <summary>
        /// 排列选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSortField_Click(object sender, EventArgs e)
        {
            SortRecord(fieldTableName);
        }

        /// <summary>
        /// 刷新
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRefresh_Click(object sender, EventArgs e)
        {
            ReloadClassTable();
            ReloadTabTable();
            ReloadFieldTable();
            ReloadTree();
        }

        /// <summary>
        /// 删除向导
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemoveClass_Click(object sender, EventArgs e)
        {
            DeleteRecord();
        }

        /// <summary>
        /// 删除分页
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bRemoveTab_Click(object sender, EventArgs e)
        {
            DeleteRecord();
        }

        /// <summary>
        /// 删除选项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDeleteField_Click(object sender, EventArgs e)
        {
            DeleteRecord();
        }

        /// <summary>
        /// 分类信息改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void OnClassInfoChanged(object sender, EventArgs e)
        {
            classInfoChanged = true;
        }

        /// <summary>
        /// 分页信息改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void OnTabInfoChanged(object sender, EventArgs e)
        {
            tabInfoChanged = true;
        }

        /// <summary>
        /// 选项信息改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void OnFieldInfoChanged(object sender, EventArgs e)
        {
            fieldInfoChanged = true;
        }

        /// <summary>
        /// 选择树结点之后
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void guideTree_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            LoadRecord();
            lastSelectedNode = guideTree.SelectedNode;
        }

        /// <summary>
        /// 选择树结点之前
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void guideTree_BeforeNodeSelect(object sender, AdvTreeNodeCancelEventArgs e)
        {
            SaveRecord();
        }

        /// <summary>
        /// 更新数据
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            SaveRecord();
        }

        /// <summary>
        /// 关闭向导
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// 编辑分页脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bEditScript_Click(object sender, EventArgs e)
        {
            Node currentNode = guideTree.SelectedNode;
            string id = currentNode.Tag as string;
            string scriptText = "";

            SqlCommand cmd;
            string sqlString;
            byte[] buffer;

            try
            {
                OpenConnection();

                cmd = conn.CreateCommand();
                sqlString = string.Format("SELECT Script FROM {0} WHERE ID = {1}",
                                          tabTableName, id);                
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null && !(executeResult is DBNull))
                {
                    buffer = executeResult as byte[];
                    scriptText = Encoding.Unicode.GetString(buffer);
                }
            }
            catch (SqlException ex)
            {
                PrintExceptionMessage(ex);
            }
            finally
            {
                CloseConnection();
            }

            ScriptEditForm scriptEditForm = new ScriptEditForm(scriptText);

            if (scriptEditForm.ShowDialog() == DialogResult.OK)
            {
                scriptText = scriptEditForm.ScriptText;
                buffer = Encoding.Unicode.GetBytes(scriptText);

                try
                {
                    OpenConnection();

                    cmd = conn.CreateCommand();
                    SqlParameter sqlParameter = new SqlParameter("@data", buffer);
                    cmd.Parameters.Add(sqlParameter);
                    sqlString = string.Format("UPDATE {0} SET Script = @data WHERE ID = {1}",
                                              tabTableName, id);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteScalar();
                }
                catch(SqlException ex)
                {
                    PrintExceptionMessage(ex);
                }
                finally
                {
                    CloseConnection();
                }
            }
        }        
    }
}