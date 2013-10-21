using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace VarManager
{
    public partial class EditForm : Form
    {
        DataRow dataRow; // 当前数据行
        private Hashtable editData = new Hashtable(); // 编辑的数据

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="dataRow"></param>
        public EditForm(DataRow dataRow)
        {
            this.dataRow = dataRow;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 编辑的数据
        /// </summary>
        public Hashtable EditData
        {
            get
            {
                return editData;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            textBoxX1.Text = dataRow["ID"].ToString();
            textBoxX2.Text = dataRow["BindType"] as string;
            textBoxX3.Text = dataRow["BindObject"] as string;
            textBoxX4.Text = dataRow["BindType"] as string;
            textBoxX5.Text = dataRow["Name"] as string;
            textBoxX6.Text = dataRow["Description"] as string;
            integerInput1.Value = (int)dataRow["StartIndex"];
            integerInput2.Value = (int)dataRow["Length"];
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string id = dataRow["ID"].ToString();
            string newName = textBoxX5.Text;
            string newDescription = textBoxX6.Text;
            string newIndex = integerInput1.Value.ToString();
            string newLength = integerInput2.Value.ToString();

            if(newName != "" && newDescription != "") // 输入不能为空
            {
                if(newName != dataRow["Name"].ToString() ||
                    newDescription != dataRow["Description"].ToString() ||
                    newIndex != dataRow["StartIndex"].ToString() ||
                    newLength != dataRow["Length"].ToString()) // 检查输入是否为新值
                {
                    editData["id"] = id;
                    editData["name"] = newName;
                    editData["description"] = newDescription;
                    editData["startIndex"] = newIndex;
                    editData["length"] = newLength;
                    DialogResult = DialogResult.OK;
                }
                else
                {
                    DialogResult = DialogResult.Cancel;
                }

                this.Close();
            }
            else
            {
                MessageBox.Show("输入不能为空！", "修改变量", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 用户选择取消
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