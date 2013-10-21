using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using DataInteractor;
using System.Collections;
using System.Threading;

namespace AIDiagramPlugin
{
    public partial class TParmEditForm : Office2007Form
    {
        public AI_TemplateParmItem TemplateParmItem = null;
        public List<string> Other_NameArray;
        public List<string> Other_ENameArray;
        private int m_keyID = 0;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="maxID">最大ID</param>
        public TParmEditForm(int maxID)
        {
            InitializeComponent();
            this.Text = "新增变量";
            this.m_keyID = maxID + 1;
            InitEnumType();
        }

        public TParmEditForm(AI_TemplateParmItem item, bool bLockName)
        {
            InitializeComponent();

            // 是否锁定名字不允许编辑
            if(bLockName)
            {
                this.txtName.Enabled = false;
                this.txtEname.Enabled = false;
            }

            this.Text = "编辑变量";
            this.m_keyID = Int32.Parse(item.keyID);
            this.cmbVarType.Enabled = false;
            this.TemplateParmItem = item;
            // 填写变量名、变量类型
            this.txtName.Text = item.Name;
            this.txtEname.Text = item.Ename;
            this.cmbVarType.Text = item.GetTypeString();

            // 填写默认值
            if (this.cmbVarType.Text == "数字" ||
                this.cmbVarType.Text == "字符串")
            {
                this.cmbDefaultValue.Visible = false;
                this.txtDefaultValue.Visible = true;
                this.txtDefaultValue.Text = item.defaultValue;
                return;
            }
            else
            {
                this.cmbDefaultValue.Visible = true;
                this.txtDefaultValue.Visible = false;
                cmbDefaultValue.Items.Clear();

                EnumType etype = EnumTable.GetEnumTypeByID(item.m_type.ToString());
                cmbDefaultValue.Items.AddRange(etype.Items.ToArray());
                if(item.defaultValue!= null && item.defaultValue != "")
                    cmbDefaultValue.SelectedIndex = Int32.Parse((item.defaultValue));
            }
        }

        private void InitEnumType()
        {
            this.cmbDefaultValue.Visible = false;
            this.DialogResult = DialogResult.Cancel;
            this.cmbVarType.Items.Add("数字");
            this.cmbVarType.Items.Add("字符串");
            foreach (EnumType et in EnumTable.EnumTypes)
            {
                this.cmbVarType.Items.Add(et);
            }
            this.cmbVarType.SelectedIndex = 0;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            // 检查完整性
            if (txtName.Text == "" || txtEname.Text == "" || (cmbVarType.Enabled && cmbVarType.SelectedItem == null))
            {
                MessageBox.Show("请输入完整", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 检查重名
            if(this.Other_NameArray.Contains(this.txtName.Text))
            {
                MessageBox.Show("显示名不能有重复", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (this.Other_ENameArray.Contains(this.txtEname.Text))
            {
                MessageBox.Show("变量名不能有重复", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 构造模板参数对象
            AI_TemplateParmItem tpnew = new AI_TemplateParmItem();
            if (this.Text == "编辑变量") // 编辑变量
            {
                tpnew = this.TemplateParmItem;
                tpnew.Name = this.txtName.Text;
                tpnew.Ename = this.txtEname.Text;
                if (cmbDefaultValue.Visible)
                {
                    // 存枚举默认值
                    if (cmbDefaultValue.SelectedItem != null)
                        tpnew.defaultValue = (cmbDefaultValue.SelectedItem as EnumItem).m_asParentsPos.ToString();
                }
                else
                {
                    // 存数字字符串默认值
                    tpnew.defaultValue = txtDefaultValue.Text;
                }
            }
            else  // 新增变量
            {
                tpnew.Name = this.txtName.Text;
                tpnew.Ename = this.txtEname.Text;
                tpnew.keyID = this.m_keyID.ToString();

                if (cmbVarType.SelectedItem.ToString() == "数字")
                {
                    tpnew.m_type = -1;
                    tpnew.defaultValue = txtDefaultValue.Text;
                }
                else if (cmbVarType.SelectedItem.ToString() == "字符串")
                {
                    tpnew.m_type = -2;
                    tpnew.defaultValue = txtDefaultValue.Text;
                }
                else
                {
                    tpnew.m_type = Int32.Parse((cmbVarType.SelectedItem as EnumType).DBID);
                    if (cmbDefaultValue.SelectedItem != null)
                        tpnew.defaultValue = (cmbDefaultValue.SelectedItem as EnumItem).m_asParentsPos.ToString();
                }
            }

            this.TemplateParmItem = tpnew;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void cmbVarType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbVarType.SelectedItem == null) return;
            if (cmbVarType.SelectedItem.ToString() == "数字" ||
                cmbVarType.SelectedItem.ToString() == "字符串")
            {
                this.cmbDefaultValue.Visible = false;
                this.txtDefaultValue.Visible = true;
                return;
            }
            this.cmbDefaultValue.Visible = true;
            this.txtDefaultValue.Visible = false;
            cmbDefaultValue.Items.Clear();
            EnumType etype = cmbVarType.SelectedItem as EnumType;
            cmbDefaultValue.Items.AddRange(etype.Items.ToArray());
            if(cmbDefaultValue.Items.Count > 0) 
                cmbDefaultValue.SelectedIndex = 0;
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cmbDefaultValue_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}