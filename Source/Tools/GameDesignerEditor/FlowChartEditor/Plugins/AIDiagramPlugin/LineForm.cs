using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;

namespace AIDiagramPlugin
{
    public partial class LineForm<T> : Office2007Form
    {
        private T m_ResultObj; // 当前选中的对象

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="selObj">当前对象</param>
        /// <param name="list">对象链表</param>
        private LineForm(T selObj, List<T> list)
        {
            this.DialogResult = DialogResult.Cancel;
            InitializeComponent();           
            List<object> listo = new List<object>();
            foreach (T t in list)
            {
                listo.Add(t);
            }
            this.listBox1.Items.AddRange(listo.ToArray());
            listBox1.SelectedItem = selObj;
        }

        /// <summary>
        /// 显示编辑窗体
        /// </summary>
        /// <param name="selObj">当前对象</param>
        /// <param name="list">对象链表</param>
        /// <returns>当前选中对象</returns>
        public static T ShowSelect(T selObj, List<T> list)
        {
            T selectedObject;

            switch (list.Count)
            {
                case 0:
                    {
                        selectedObject = default(T);
                        break;
                    }
                case 1:
                    {
                        selectedObject = list[0];
                        break;
                    }
                default:
                    {
                        LineForm<T> lineForm = new LineForm<T>(selObj, list);

                        if (lineForm.ShowDialog() == DialogResult.OK)
                        {
                            selectedObject = lineForm.m_ResultObj;
                        }
                        else
                        {
                            selectedObject = selObj;
                        }

                        break;
                    }
            }

            return selectedObject;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem != null)
            {
                m_ResultObj = (T)listBox1.SelectedItem;
                DialogResult = DialogResult.OK;
                this.Close();
            }
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

    }
}