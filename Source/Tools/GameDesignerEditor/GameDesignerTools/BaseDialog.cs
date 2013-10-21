using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace GameDesingerTools
{
    public partial class BaseDialog : Form
    {
        private string modelName; // 模块名称
        private Hashtable propertyTable; // 属性hash表对象
        BaseForm bForm; // BaseForm对象
        public bool useHideMode = false;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="modelName">模块名称</param>
        public BaseDialog(string modelName)
        {
            this.modelName = modelName;
            InitializeComponent();
            InitBaseForm();
            this.Text = modelName;
        }

        /// <summary>
        /// 属性hash表对象
        /// </summary>
        public Hashtable PropertyTable
        {
            get
            {
                return propertyTable;
            }
        }

        /// <summary>
        /// 初始化BaseForm子窗体
        /// </summary>
        private void InitBaseForm()
        {
            try
            {
                bForm = new BaseForm(modelName);
                bForm.MdiParent = this;
                bForm.WindowState = FormWindowState.Maximized;      
                bForm.Show();
                bForm.Update();     
            }
            catch (Exception ex)
            {
                if (bForm != null)
                    bForm.Close();
                MessageBox.Show(ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Stop);
            }
        }

        /// <summary>
        /// 获取BaseForm中特定的属性值
        /// </summary>
        /// <param name="propertyName">属性的名称</param>
        /// <returns>属性的值</returns>
        public object GetPropertyValue(string propertyName)
        {
            return propertyTable[propertyName];
        }

        private void CloseOrHideForm()
        {
            // 可以用隐藏代替，以免打开多次，提高效率。
            if (this.useHideMode)
            {
                this.Hide();
            }
            else
            {
                foreach (Form f in this.MdiChildren) // 关闭子窗体
                {
                    f.Close();
                }
                this.Close();
            }
        }

        /// <summary>
        /// 关闭对话框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            CloseOrHideForm();
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (this.useHideMode || propertyTable == null)
            {
                propertyTable = bForm.GetPropertyGridData();
            }

            if(propertyTable.Count != 0)
            {
                DialogResult = DialogResult.OK;
            }

            CloseOrHideForm();
        }

        /// <summary>
        /// 关键字过滤
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void bFind_Click(object sender, EventArgs e)
        {
            FiltrateForm filtrateForm = new FiltrateForm(bForm.BaseTree);
            DialogResult result = filtrateForm.ShowDialog();
        }
    }
}