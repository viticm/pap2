using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DesignerSceneEditor
{
	public partial class ObjectSelectMaskForm: Form
	{
        private int selectObjectTypeMask = 0; // 选择对象掩码
        private bool confirmSelection = false; // 是否确认选择

        /// <summary>
        /// 构造函数s
        /// </summary>
		public ObjectSelectMaskForm()
		{
			InitializeComponent();
		}        

        /// <summary>
        /// 是否确认选择
        /// </summary>
        public bool ConfirmSelection
        {
            get
            {
                return confirmSelection;
            }
        }

        /// <summary>
        /// 选择对象掩码
        /// </summary>
        public int SelectObjectTypeMask
        {
            get
            {
                return selectObjectTypeMask;
            }
        }

        /// <summary>
        /// 刷新数据
        /// </summary>
        public void RefreshData()
        {
            confirmSelection = false;
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            selectObjectTypeMask = 0;

            if (cNpc.Checked)
            {
                selectObjectTypeMask += 2;
            }
            if (cDoodad.Checked)
            {
                selectObjectTypeMask += (int)Math.Pow(2, 8);
            }
            if (cWayPoint.Checked)
            {
                selectObjectTypeMask += (int)Math.Pow(2, 9);
            }
            if (cPoly.Checked)
            {
                selectObjectTypeMask += (int)Math.Pow(2, 11);
            }
            if (cRefreshPoint.Checked)
            {
                selectObjectTypeMask += (int)Math.Pow(2, 10);
            }
            if (cTrafficPoint.Checked)
            {
                selectObjectTypeMask += (int)Math.Pow(2, 22);
            }

            if (checkboxAll.Checked)
            {
                selectObjectTypeMask = -1;
            }

            confirmSelection = true;
            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            confirmSelection = false;
            this.Close();
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void ObjectSelectMaskForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Visible = false;
        }
	}
}