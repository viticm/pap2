using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AnimationAutoFill
{
    public partial class UpdateResultForm : Form
    {
        private int currentRowIndex = 0; // 当前行号

        /// <summary>
        /// 构造函数
        /// </summary>
        public UpdateResultForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 数据初始化
        /// </summary>
        private void Init()
        {
            DataGridViewTextBoxColumn rowIndexColumn = new DataGridViewTextBoxColumn();
            rowIndexColumn.Name = "RowIndex";
            rowIndexColumn.HeaderText = "行号";

            DataGridViewTextBoxColumn fieldNameColumn = new DataGridViewTextBoxColumn();
            fieldNameColumn.Name = "FieldName";
            fieldNameColumn.HeaderText = "字段名";

            DataGridViewTextBoxColumn animationNameColumn = new DataGridViewTextBoxColumn();
            animationNameColumn.Name = "AnimationName";
            animationNameColumn.HeaderText = "动作名";

            dataGridViewX1.Columns.Add(rowIndexColumn);
            dataGridViewX1.Columns.Add(fieldNameColumn);
            dataGridViewX1.Columns.Add(animationNameColumn);
        }

        /// <summary>
        /// 清空当前数据
        /// </summary>
        public void Reset()
        {
            dataGridViewX1.Rows.Clear();
            currentRowIndex = 0;
        }

        /// <summary>
        /// 增加日志
        /// </summary>
        /// <param name="rowIndex">行号</param>
        /// <param name="fieldName">字段名</param>
        /// <param name="animationName">动作名</param>
        public void AppendLog(string rowIndex, string fieldName, string animationName)
        {
            dataGridViewX1.Rows.Add(1);
            DataGridViewRow newRow = dataGridViewX1.Rows[currentRowIndex];
            newRow.Cells["RowIndex"].Value = rowIndex;
            newRow.Cells["FieldName"].Value = fieldName;
            newRow.Cells["AnimationName"].Value = animationName;

            currentRowIndex++;
        }
    }
}