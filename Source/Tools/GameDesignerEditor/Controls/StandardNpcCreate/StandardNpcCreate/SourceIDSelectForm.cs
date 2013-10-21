using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Data.SqlClient;
using System.IO;

using DevComponents.AdvTree;

namespace StandardNpcCreate
{
    public partial class SourceIDSelectForm : Form
    {
        private SqlConnection conn; // sql连接
        private DataTable modelTable; // 模型表
        private DataTable skillTable; // 技能表
        private string editorID; // EditorID
        private string sourceID; // SourceID
        private string npcForce; // npc势力
        private string forceDetail; // 势力细节     
        private string colorChannel; // 颜色通道

        private int lastSearchForceIndex = 0; // 上次查找的Force树结点编号
        private int lastSearchForceDetailIndex = 0; // 上次查找的ForceDetail树节点的编号
        private int lastRowIndex = 0; // 上次查找的数据行的编号

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="modelTable">模型表</param>
        /// <param name="skillTable">技能表</param>
        public SourceIDSelectForm(SqlConnection conn, DataTable modelTable, DataTable skillTable)
        {
            InitializeComponent();
            this.conn = conn;
            this.modelTable = modelTable;
            this.skillTable = skillTable;            
            Init();
        }

        /// <summary>
        /// 颜色通道
        /// </summary>
        public string ColorChannel
        {
            get
            {
                return colorChannel;
            }
        }

        /// <summary>
        /// SourceID
        /// </summary>
        public string SourceID
        {
            get
            {
                return sourceID;
            }
        }

        /// <summary>
        /// EditorID
        /// </summary>
        public string EditorID
        {
            get
            {
                return editorID;
            }
        }

        /// <summary>
        /// npc势力
        /// </summary>
        public string NpcForce
        {
            get
            {
                return npcForce;
            }
        }

        /// <summary>
        /// 势力细节
        /// </summary>
        public string ForceDetail
        {
            get
            {
                return forceDetail;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 添加列数据
            DataGridViewTextBoxColumn animationTypeColumn = new DataGridViewTextBoxColumn();
            animationTypeColumn.Name = "AnimationType";
            animationTypeColumn.HeaderText = "技能属性";

            DataGridViewTextBoxColumn editorIDColumn = new DataGridViewTextBoxColumn();
            editorIDColumn.Name = "EditorID";
            editorIDColumn.HeaderText = "NPC编号";

            DataGridViewTextBoxColumn sourceIDColumn = new DataGridViewTextBoxColumn();
            sourceIDColumn.Name = "SourceID";
            sourceIDColumn.HeaderText = "模型编号";

            DataGridViewTextBoxColumn colorChannelColumn = new DataGridViewTextBoxColumn();
            colorChannelColumn.Name = "ColorChannel";
            colorChannelColumn.HeaderText = "颜色通道";

            editorView.Columns.Add(animationTypeColumn);
            editorView.Columns.Add(editorIDColumn);
            editorView.Columns.Add(sourceIDColumn);
            editorView.Columns.Add(colorChannelColumn);

            // 初始化分类树
            string[] tempArray;
            string animationType = "无分类";           
            char[] splitCharArray = new char[] { ';' };
            Hashtable forceTable = new Hashtable();
            Hashtable informationTable = new Hashtable();
            List<string[]> dataList;
            Node tempNode = null;
            Node leafNode = null;
            bool find = false;

            foreach(DataRow dataRow in modelTable.Rows)
            {                
                editorID = dataRow["EditorID"].ToString();
                sourceID = dataRow["SourceID"].ToString();
                npcForce = dataRow["Force"].ToString();
                forceDetail = dataRow["ForceDetail"].ToString();
                colorChannel = dataRow["ColorChannel"].ToString();                          

                // 检查第一层分类
                find = false;

                foreach(Node node in editorTree.Nodes)
                {
                    if (node.Text == npcForce)
                    {
                        tempNode = node;
                        find = true;
                        break;
                    }
                }

                if (!find)
                {
                    tempNode = new Node();
                    tempNode.Text = npcForce;
                    editorTree.Nodes.Add(tempNode);
                }
                
                // 检查第二层分类                
                tempArray = forceDetail.Split(splitCharArray, StringSplitOptions.RemoveEmptyEntries);

                foreach(string s in tempArray)
                {
                    DataRow[] rows = skillTable.Select(string.Format("势力 = '{0}' AND 名字 = '{1}'", npcForce, s));
                    if (rows.Length > 0)
                    {
                        animationType = rows[0]["动作分类2"].ToString();
                    }
                    else
                    {
                        animationType = "无分类";
                    }

                    find = false;

                    foreach(Node node in tempNode.Nodes)
                    {
                        if (node.Text == s)
                        {
                            leafNode = node;
                            find = true;
                            break;
                        }
                    }

                    if (!find)
                    {
                        leafNode = new Node();
                        leafNode.Text = s;
                        tempNode.Nodes.Add(leafNode);
                    }

                    if (leafNode.Tag == null)
                    {
                        dataList = new List<string[]>();
                        leafNode.Tag = dataList;
                    }
                    else
                    {
                        dataList = leafNode.Tag as List<string[]>;
                    }

                    dataList.Add(new string[] { animationType, editorID, sourceID, colorChannel });
                }
            }
        }

        /// <summary>
        /// 更新模型描述
        /// </summary>
        /// <param name="currentEditorID">EditorID</param>
        /// <param name="currentColorChannel">颜色通道</param>
        /// <param name="description">模型描述</param>
        private void UpdateModelDescription(string currentEditorID, string currentColorChannel, string description)
        {
            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                // 先检查是否已有该条记录
                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("SELECT Description FROM npc_model_record WHERE EditorID = '{0}' AND ColorChannel = '{1}'",
                                                 currentEditorID, currentColorChannel);
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    sqlString = string.Format("UPDATE npc_model_record SET Description = '{0}' WHERE EditorID = '{1}' AND ColorChannel = '{2}'",
                                              description, currentEditorID, currentColorChannel);
                }
                else
                {
                    sqlString = string.Format("INSERT INTO npc_model_record (EditorID, ColorChannel, Description) VALUES ('{0}', '{1}', '{2}')",
                                              currentEditorID, currentColorChannel, description);
                }

                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取模型描述数据时产生sql异常：" + ex.Message, "获取模型描述", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
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
        /// 获取模型描述
        /// </summary>
        /// <param name="currentEditorID">EditorID</param>
        /// <param name="currentColorChannel">颜色通道</param>
        /// <returns>模型描述</returns>
        private string GetModelDescription(string currentEditorID, string currentColorChannel)
        {
            string description = "";

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("SELECT Description FROM npc_model_record WHERE EditorID = '{0}' AND ColorChannel = '{1}'",
                                                 currentEditorID, currentColorChannel);
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    description = executeResult as string;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取模型描述数据时产生sql异常：" + ex.Message, "获取模型描述", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return description;
        }

        /// <summary>
        /// 获取模型选择的次数
        /// </summary>
        /// <param name="modelFile">模型文件</param>
        /// <param name="currentColorChannel">颜色通道</param>
        /// <returns>模型选择的次数</returns>
        private int GetSelectedCount(string modelFile, string currentColorChannel)
        {
            int selectedCount = 0;

            if (!string.IsNullOrEmpty(modelFile))
            {
                try
                {
                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    SqlCommand cmd = conn.CreateCommand();
                    string sqlString = null;
                    
                    if (currentColorChannel == "")
                    {
                        sqlString = string.Format("SELECT COUNT(RepresentID) FROM npc WHERE MainModelFile = '{0}' AND ColorChannel IS NULL",
                                                  modelFile);
                    }
                    else
                    {
                        sqlString = string.Format("SELECT COUNT(RepresentID) FROM npc WHERE MainModelFile = '{0}' AND ColorChannel = '{1}'",
                                                  modelFile, currentColorChannel);
                    }                                   
                    
                    cmd.CommandText = sqlString;
                    object executeResult = cmd.ExecuteScalar();

                    if (executeResult != null)
                    {
                        selectedCount = (int)executeResult;
                    }
                }
                catch (SqlException ex)
                {
                    MessageBox.Show("在获取模型使用记录数据时产生sql异常：" + ex.Message, "获取模型使用记录", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }
            }            

            return selectedCount;
        }

        /// <summary>
        /// 获取模型对应的图片
        /// </summary>
        /// <param name="currentEditorID">EditorID</param>
        /// <returns>模型对应的图片</returns>
        private Image GetModelImage(string currentEditorID)
        {
            Image image = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT Image FROM ModelImage WHERE EditorID = '{0}'", currentEditorID);
                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null)
                {
                    byte[] imageData = (byte[])executeResult;
                    MemoryStream ms = new MemoryStream();
                    ms.Write(imageData, 0, imageData.Length);
                    image = Image.FromStream(ms);
                    ms.Close();
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取模型图片时产生sql异常：" + ex.Message, "选择模型图片", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            // 图片缩放
            if (image != null)
            {
                int desWidth = image.Width;
                int desHeight = image.Height;

                if (desWidth > previewLabel.Width)
                {
                    desWidth = previewLabel.Width;
                }

                if (desHeight > previewLabel.Height)
                {
                    desHeight = previewLabel.Height;
                }

                if (desWidth != image.Width || desHeight != image.Height)
                {
                    Bitmap bmp = new Bitmap(desWidth, desHeight);
                    Graphics g = Graphics.FromImage(bmp);
                    g.DrawImage(image, 0, 0, desWidth, desHeight);
                    image = bmp;
                }
            }

            return image;
        }

        /// <summary>
        /// 显示模型信息
        /// </summary>
        /// <param name="currentRow">当前数据行</param>
        private void ShowModelInfo(DataGridViewRow currentRow)
        {
            string currentColorChannel = currentRow.Cells["ColorChannel"].Value.ToString();
            string currentSourceID = currentRow.Cells["SourceID"].Value.ToString();
            string currentEditorID = currentRow.Cells["EditorID"].Value.ToString();

            // 显示模型使用次数
            int selectedCount = GetSelectedCount(currentSourceID, currentColorChannel);
            descriptionLabel.Text = string.Format("模型使用次数：{0}", selectedCount.ToString());

            // 显示模型预览图片
            Image previewImage = GetModelImage(currentEditorID);
            previewLabel.Image = previewImage;

            // 显示模型描述
            string modelDescription = GetModelDescription(currentEditorID, currentColorChannel);
            modelDescriptionBox.Text = modelDescription;

            if (modelDescription != "")
            {
                MessageBox.Show("该模型有特殊用途，请参照描述使用！", "模型信息",
                                 MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
        
        /// <summary>
        /// 查找EditorID
        /// </summary>
        /// <param name="searchString">查找条件</param>
        private void SearchEditorID(string searchString)
        {
            Node searchNode = null;
            bool canSearchAgain = false;
            int currentRowIndex = 0;

            if (lastSearchForceIndex != 0 || lastSearchForceDetailIndex != 0)
            {
                canSearchAgain = true;
            }

            for (int i = lastSearchForceIndex; i < editorTree.Nodes.Count; i++)
            {
                Node forceNode = editorTree.Nodes[i];

                for (int j = lastSearchForceDetailIndex; j < forceNode.Nodes.Count; j++)
                {
                    Node forceDetailNode = forceNode.Nodes[j];
                    List<string[]> dataList = forceDetailNode.Tag as List<string[]>;

                    for (int k = lastRowIndex; k < dataList.Count; k++)
                    {
                        string[] dataArray = dataList[k];

                        if (dataArray[1].Contains(searchString))
                        {
                            searchNode = forceDetailNode;
                            currentRowIndex = k;
                            lastRowIndex = k + 1;
                            lastSearchForceDetailIndex = j;
                            lastSearchForceIndex = i;

                            if (lastRowIndex == dataList.Count)
                            {
                                lastRowIndex = 0;
                                lastSearchForceDetailIndex++;

                                if (lastSearchForceDetailIndex == forceNode.Nodes.Count)
                                {
                                    lastSearchForceDetailIndex = 0;
                                    lastSearchForceIndex++;

                                    if (lastSearchForceIndex == editorTree.Nodes.Count)
                                    {
                                        lastSearchForceIndex = 0;
                                    }
                                }
                            }

                            break;
                        }
                    }

                    if (searchNode != null)
                    {
                        break;
                    }
                    else
                    {
                        lastRowIndex = 0;
                    }
                }

                if (searchNode != null)
                {
                    break;
                }
                else
                {
                    lastSearchForceDetailIndex = 0;
                }
            }

            if (searchNode != null)
            {
                editorTree.SelectedNode = searchNode;
                editorView.CurrentCell = editorView.Rows[currentRowIndex].Cells[0];
            }
            else
            {
                if (canSearchAgain)
                {
                    if (MessageBox.Show("没有找到匹配的分类！\r\n\r\n已经查找到分类末尾，是否从头开始查找？", "分类查找",
                                        MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                    {
                        lastRowIndex = 0;
                        lastSearchForceIndex = 0;
                        lastSearchForceDetailIndex = 0;
                        SearchForceDetail(searchString);
                    }
                }
                else
                {
                    MessageBox.Show("没有找到匹配的分类！", "分类查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        /// <summary>
        /// 查找ForceDetail
        /// </summary>
        /// <param name="searchString">查找条件</param>
        private void SearchForceDetail(string searchString)
        {
            Node searchNode = null;
            
            bool canSearchAgain = false;

            if (lastSearchForceIndex != 0 || lastSearchForceDetailIndex != 0 || lastRowIndex != 0)
            {
                canSearchAgain = true;
            }

            for (int i = lastSearchForceIndex; i < editorTree.Nodes.Count; i++)
            {
                Node forceNode = editorTree.Nodes[i];

                for (int j = lastSearchForceDetailIndex; j < forceNode.Nodes.Count; j++)
                {
                    Node forceDetailNode = forceNode.Nodes[j];

                    if (forceDetailNode.Text.Contains(searchString))
                    {
                        searchNode = forceDetailNode;
                        lastSearchForceDetailIndex = j + 1;
                        lastSearchForceIndex = i;

                        if (lastSearchForceDetailIndex == forceNode.Nodes.Count)
                        {
                            lastSearchForceDetailIndex = 0;
                            lastSearchForceIndex++;

                            if (lastSearchForceIndex == editorTree.Nodes.Count)
                            {
                                lastSearchForceIndex = 0;
                            }
                        }

                        break;
                    }                    
                }

                if (searchNode != null)
                {
                    break;
                }
                else
                {
                    lastSearchForceDetailIndex = 0;
                }
            }

            if (searchNode != null)
            {
                editorTree.SelectedNode = searchNode;                
            }
            else
            {
                if (canSearchAgain)
                {
                    if (MessageBox.Show("没有找到匹配的分类！\r\n\r\n已经查找到分类末尾，是否从头开始查找？", "分类查找", 
                                        MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                    {
                        lastSearchForceIndex = 0;
                        lastSearchForceDetailIndex = 0;
                        SearchForceDetail(searchString);
                    }
                }
                else
                {
                    MessageBox.Show("没有找到匹配的分类！", "分类查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
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
            if (editorView.SelectedCells.Count > 0)
            {
                DataGridViewCell currentCell = editorView.SelectedCells[0];

                if (currentCell.RowIndex >= 0)
                {
                    DataGridViewRow currentRow = currentCell.OwningRow;
                    npcForce = editorTree.SelectedNode.Parent.Text;
                    forceDetail = editorTree.SelectedNode.Text;
                    editorID = currentRow.Cells["EditorID"].Value.ToString();
                    sourceID = currentRow.Cells["SourceID"].Value.ToString();
                    colorChannel = currentRow.Cells["ColorChannel"].Value.ToString();                                

                    DialogResult = DialogResult.OK;
                    this.Close();
                }
                else
                {
                    MessageBox.Show("请先进行选择！", "SourceID选择", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("请先进行选择！", "SourceID选择", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 选择关闭
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 选择分类树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = editorTree.SelectedNode;
            editorView.Rows.Clear();

            if (currentNode.Level == 1)
            {
                List<string[]> dataList = currentNode.Tag as List<string[]>;         
                DataGridViewRow firstRow = null;

                for (int i = 0; i < dataList.Count; i++)
                {
                    editorView.Rows.Add(1);
                    DataGridViewRow newRow = editorView.Rows[i];
                    newRow.Cells["AnimationType"].Value = dataList[i][0];
                    newRow.Cells["EditorID"].Value = dataList[i][1];
                    newRow.Cells["SourceID"].Value = dataList[i][2];
                    newRow.Cells["ColorChannel"].Value = dataList[i][3];

                    if (i == 0) // 默认显示第一行的信息
                    {
                        ShowModelInfo(newRow);
                    }
                }                                
            }
            else
            {
                // 清掉模型图片和描述
                previewLabel.Image = null;
                descriptionLabel.Text = "模型使用次数";
                modelDescriptionBox.Text = "";
            }
        }

        /// <summary>
        /// 选择SourceID
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void dataGridViewX1_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.RowIndex >= 0)
            {
                DataGridViewRow currentRow = editorView.Rows[e.RowIndex];

                ShowModelInfo(currentRow);
            }
        }

        /// <summary>
        /// 更新模型描述
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            string description = modelDescriptionBox.Text;

            if (description != "")
            {
                if (editorView.SelectedCells.Count > 0)
                {
                    DataGridViewCell currentCell = editorView.SelectedCells[0];

                    if (currentCell.RowIndex >= 0)
                    {
                        DataGridViewRow currentRow = editorView.Rows[currentCell.RowIndex];
                        string currentColorChannel = currentRow.Cells["ColorChannel"].Value.ToString();
                        string currentEditorID = currentRow.Cells["EditorID"].Value.ToString();

                        UpdateModelDescription(currentEditorID, currentColorChannel, description);
                    }
                    else
                    {
                        MessageBox.Show("请先选择要更新描述的模型类型！", "更新模型描述", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else
                {
                    MessageBox.Show("请先选择要更新描述的模型类型！", "更新模型描述", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("模型描述不能为空！", "更新模型描述", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 查找符合匹配条件的树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            string searchString = searchConditionBox.Text;            

            if (searchConditionBox.Text != "")
            {
                if (forceDetailRadioButton.Checked) // 查找ForceDetail
                {
                    SearchForceDetail(searchString);
                }
                else if (editorIDRadioButton.Checked) // 查找EditorID
                {
                    SearchEditorID(searchString);
                }
                else
                {
                    MessageBox.Show("请先选择查找类型！", "分类查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("查找条件不能为空！", "分类查找", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择ForceDetail
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void forceDetailRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            if (forceDetailRadioButton.Checked)
            {
                editorIDRadioButton.Checked = false;
            }
        }

        /// <summary>
        /// 选择EditorID
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void editorIDRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            if (editorIDRadioButton.Checked)
            {
                forceDetailRadioButton.Checked = false;
            }
        }
    }
}