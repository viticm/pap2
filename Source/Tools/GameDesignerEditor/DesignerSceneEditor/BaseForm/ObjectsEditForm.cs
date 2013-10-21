using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;

using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	public partial class ObjectsEditForm: Form
	{       
        private  _AtlObjInfo[] objectInfoArray; // 对象数据数组
        private bool beginEdit = false; // 是否已经开始编辑
        private Font normalTextFont = new Font("SimSun", 9, FontStyle.Regular); // 普通文本字体
        private Font boldTextFont = new Font("SimSun", 9, FontStyle.Bold); // 粗体文本字体
        private string currentObjectType; // 当前的编辑对象类型
        private BaseDoc baseDoc;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="baseDoc">BaseDoc对象</param>
        /// <param name="objectInfoArray">对象数据数组</param>
        public ObjectsEditForm(BaseDoc baseDoc, _AtlObjInfo[] objectInfoArray)
		{
            this.baseDoc = baseDoc;
            this.objectInfoArray = objectInfoArray;
			InitializeComponent();
            Init();
		}        

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            // 构造数据列
            if(objectInfoArray.Length > 0)
            {
                _AtlObjInfo objectInfo = objectInfoArray[0];
                currentObjectType = objectInfo.strType;
               
                DataGridViewTextBoxColumn typeColumn = new DataGridViewTextBoxColumn();
                typeColumn.Name = "Type";
                typeColumn.Tag = "Type";
                typeColumn.HeaderText = "类别";
                typeColumn.ReadOnly = true;

                dataGridViewX1.Columns.Add(typeColumn);                

                for(int i = 0; i < objectInfo.iFieldsCount; i++)
                {                    
                    DataGridViewTextBoxColumn newColumn = new DataGridViewTextBoxColumn();
                    newColumn.Name = objectInfo.strFields[i];
                    newColumn.Tag = objectInfo.strFields[i];
                    newColumn.HeaderText = GetPropertyDisplayName(objectInfo.strFields[i]);
                    newColumn.ReadOnly = IsPropertyReadOnly(objectInfo.strFields[i]);                    

                    dataGridViewX1.Columns.Add(newColumn);
                }                              

                // 增加数据行
                ShowObjectData(objectInfo.strType);                          
            }                        
        }

        /// <summary>
        /// 将对象信息转换为数据表
        /// </summary>
        /// <param name="objectInfo">对象信息</param>
        /// <returns>数据表</returns>
        private Hashtable GetInfoTable(_AtlObjInfo objectInfo)
        {
            Hashtable infoTable = new Hashtable();

            for (int i = 0; i < objectInfo.iFieldsCount; i++)
            {
                infoTable[objectInfo.strFields[i]] = objectInfo.strValues[i];
            }

            return infoTable;
        }

        /// <summary>
        /// 保存数据
        /// </summary>
        private void SaveData()
        {
            foreach(DataGridViewRow row in dataGridViewX1.Rows)
            {
                _AtlObjInfo objectInfo = (_AtlObjInfo)row.Tag;
                Hashtable infoTable = GetObjectValues(row);

                for(int i = 0; i < objectInfo.iFieldsCount; i++)
                {
                    string strField = objectInfo.strFields[i];
                    string strValue = objectInfo.strValues[i];
                    string newValue = infoTable[strField] as string;

                    if(newValue != strValue)
                    {
                        objectInfo.strValues[i] = newValue;
                        objectInfo.bChanged = 1;
                    }
                }
            }
        }

        /// <summary>
        /// 获取数据行值哈希表
        /// </summary>
        /// <param name="row">当前数据行</param>
        /// <returns>值哈希表</returns>
        private Hashtable GetObjectValues(DataGridViewRow row)
        {
            Hashtable infoTable = new Hashtable();
            foreach(DataGridViewCell c in row.Cells)
            {
                if(c.ColumnIndex > 0)
                {
                    infoTable[c.OwningColumn.Tag] = c.Value != null ? c.Value.ToString() : string.Empty;
                }
            }

            return infoTable;
        }

        /// <summary>
        /// 显示对象数据
        /// </summary>
        /// <param name="objectType">对象类型</param>
        private void ShowObjectData(string objectType)
        {
            beginEdit = false;
            string filtValue = objectType;
            dataGridViewX1.Rows.Clear();

            int index = 0;
            for (int i = 0; i < objectInfoArray.Length; i++)
            {
                _AtlObjInfo objectInfo = objectInfoArray[i];
                Hashtable infoTable = GetInfoTable(objectInfo);

                if(objectInfo.strType == filtValue)
                {
                    dataGridViewX1.Rows.Add(1);
                    DataGridViewRow newRow = dataGridViewX1.Rows[index];                    
                    newRow.Cells["Type"].Value = objectInfo.strType;
                    newRow.Cells["Type"].Tag = objectInfo.strType;                    

                    for (int j = 0; j < objectInfo.iFieldsCount; j++)
                    {
                        string field = objectInfo.strFields[j];
                        newRow.Cells[field].Value = infoTable[field];
                        newRow.Cells[field].Tag = infoTable[field];
                    }                        
                    newRow.Tag = objectInfo;

                    index++;
                }                
            }

            beginEdit = true;
        }

        /// <summary>
        /// 挂接脚本文件
        /// </summary>
        /// <param name="currentRow">当前数据行</param>
        private void AttachScriptFile(DataGridViewRow currentRow)
        {
            string filePath = Path.Combine(Helper.RootDir, "scripts");
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = filePath;
            openFileDialog.Filter = "lua脚本文件|*.lua";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string fileName = openFileDialog.FileName.Replace(Helper.RootDir, "").TrimStart(new char[] { '\\' });
                currentRow.Cells["szScriptName"].Value = fileName;
            }
        }

        /// <summary>
        /// 更新NPC的AI参数
        /// </summary>
        /// <param name="npcIndex">NPC的编号</param>
        /// <param name="fieldName">参数名</param>
        /// <param name="fieldValue">参数值</param>
        private void UpdateNpcAIParameter(int npcIndex, string fieldName, int fieldValue)
        {
            baseDoc.DocLogical.UpdateNpcAIParameter(npcIndex, fieldName, fieldValue);
        }

        /// <summary>
        /// 属性是否只读
        /// </summary>
        /// <param name="fieldName">属性名称</param>
        /// <returns>属性是否只读</returns>
        private bool IsPropertyReadOnly(string fieldName)
        {
            bool propertyReadOnly = false;

            switch (currentObjectType)
            {
                case "NPC":
                    {
                        switch (fieldName)
                        {
                            case "szScriptName":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                            case "nIndex":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                            case "dwScriptID":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                            case "AISetID":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                        }

                        break;
                    }
                case "Doodad":
                    {
                        switch (fieldName)
                        {
                            case "szScriptName":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                            case "nIndex":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                            case "dwScriptID":
                                {
                                    propertyReadOnly = true;
                                    break;
                                }
                        }

                        break;
                    }
            }

            return propertyReadOnly;
        }

        /// <summary>
        /// 获取属性的表现名称
        /// </summary>
        /// <param name="fieldName">属性名称</param>
        /// <returns>表现名称</returns>
        private string GetPropertyDisplayName(string fieldName)
        {
            string displayName = fieldName;

            switch (currentObjectType)
            {
                case "NPC":
                    {
                        switch (fieldName)
                        {
                            case "szName":
                                {
                                    displayName = "名称";
                                    break;
                                }
                            case "szNickName":
                                {
                                    displayName = "别名";
                                    break;
                                }
                            case "szScriptName":
                                {
                                    displayName = "脚本";
                                    break;
                                }
                            case "nIndex":
                                {
                                    displayName = "编号";
                                    break;
                                }
                            case "nTempleteID":
                                {
                                    displayName = "模板ID";
                                    break;
                                }
                            case "nLevel":
                                {
                                    displayName = "等级";
                                    break;
                                }
                            case "nX":
                                {
                                    displayName = "x坐标";
                                    break;
                                }
                            case "nY":
                                {
                                    displayName = "y坐标";
                                    break;
                                }
                            case "nZ":
                                {
                                    displayName = "z坐标";
                                    break;
                                }
                            case "nKind":
                                {
                                    displayName = "类型";
                                    break;
                                }
                            case "dwScriptID":
                                {
                                    displayName = "脚本ID";
                                    break;
                                }
                            case "nFaceDirection":
                                {
                                    displayName = "朝向";
                                    break;
                                }
                            case "dwRepresentID":
                                {
                                    displayName = "表现ID";
                                    break;
                                }
                            case "ReliveID":
                                {
                                    displayName = "重生组ID";
                                    break;
                                }
                            case "RandomID":
                                {
                                    displayName = "随机组ID";
                                    break;
                                }
                            case "TeamID":
                                {
                                    displayName = "仇恨组ID";
                                    break;
                                }
                            case "AISetID":
                                {
                                    displayName = "AI组ID";
                                    break;
                                }
                            case "nProgressID":
                                {
                                    displayName = "副本进度ID";
                                    break;
                                }
                        }

                        break;
                    }
                case "Doodad":
                    {
                        switch (fieldName)
                        {
                            case "szName":
                                {
                                    displayName = "名称";
                                    break;
                                }
                            case "szNickName":
                                {
                                    displayName = "别名";
                                    break;
                                }
                            case "szScriptName":
                                {
                                    displayName = "脚本";
                                    break;
                                }
                            case "nIndex":
                                {
                                    displayName = "编号";
                                    break;
                                }
                            case "nTempleteID":
                                {
                                    displayName = "模板ID";
                                    break;
                                }
                            case "nLevel":
                                {
                                    displayName = "等级";
                                    break;
                                }
                            case "nX":
                                {
                                    displayName = "x坐标";
                                    break;
                                }
                            case "nY":
                                {
                                    displayName = "y坐标";
                                    break;
                                }
                            case "nZ":
                                {
                                    displayName = "z坐标";
                                    break;
                                }
                            case "nKind":
                                {
                                    displayName = "类型";
                                    break;
                                }
                            case "dwScriptID":
                                {
                                    displayName = "脚本ID";
                                    break;
                                }
                            case "nFaceDirection":
                                {
                                    displayName = "朝向";
                                    break;
                                }
                            case "dwRepresentID":
                                {
                                    displayName = "表现ID";
                                    break;
                                }
                            case "ReliveID":
                                {
                                    displayName = "重生组ID";
                                    break;
                                }
                        }

                        break;
                    }
            }

            return displayName;
        }

        /// <summary>
        /// 编辑AI参数
        /// </summary>
        /// <param name="currentRow">当前数据行</param>
        private void EditAIParameter(DataGridViewRow currentRow)
        {
            int npcIndex = int.Parse(currentRow.Cells["nIndex"].Value.ToString());
            string infoString = "";
            baseDoc.DocLogical.GetNpcAIParameterInfo(npcIndex, ref infoString);

            if (infoString != "")
            {
                Hashtable infoTable = new Hashtable();
                string[] tempArray = infoString.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                foreach (string s in tempArray)
                {
                    string[] dataArray = s.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    infoTable[dataArray[0]] = dataArray[1];                    
                }
                
                AIParametersEditForm aForm = new AIParametersEditForm(npcIndex, infoTable, new AIParametersEditForm.UpdateAIParameter(UpdateNpcAIParameter));
                aForm.ShowDialog();
            }            
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            SaveData();
            DialogResult = DialogResult.OK;
            this.Close();
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
        /// 单元值格值改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void dataGridViewX1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (beginEdit)
            {
                DataGridViewCell currentCell = dataGridViewX1[e.ColumnIndex, e.RowIndex] as DataGridViewCell;                                

                if (e.ColumnIndex > 0)
                {
                    if (currentCell.EditedFormattedValue.ToString() == currentCell.Tag.ToString())
                    {
                        currentCell.Style.Font = normalTextFont;                        
                    }
                    else
                    {
                        currentCell.Style.Font = boldTextFont;
                    }
                }                
            }
        }

        /// <summary>
        /// 鼠标点击对象属性
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void dataGridViewX1_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.RowIndex >= 0) // 先检查是不是有效的数据行
            {
                DataGridViewRow currentRow = dataGridViewX1.Rows[e.RowIndex];

                switch (currentObjectType)
                {
                    case "NPC":
                        {
                            switch (dataGridViewX1.Columns[e.ColumnIndex].Tag as string)
                            {
                                case "szScriptName":
                                    {
                                        AttachScriptFile(currentRow);
                                        break;
                                    }
                                case "AISetID":
                                    {
                                        EditAIParameter(currentRow);
                                        break;
                                    }
                                default:
                                    {
                                        break;
                                    }
                            }

                            break;
                        }
                    case "Doodad":
                        {
                            switch (dataGridViewX1.Columns[e.ColumnIndex].Tag as string)
                            {
                                case "szScriptName":
                                    {
                                        AttachScriptFile(currentRow);
                                        break;
                                    }
                                default:
                                    {
                                        break;
                                    }
                            }

                            break;
                        }
                }
            } 
        }
	}
}