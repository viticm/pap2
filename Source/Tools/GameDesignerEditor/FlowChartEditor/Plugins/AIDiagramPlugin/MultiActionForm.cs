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
    public partial class MultiActionForm : Office2007Form
    {
        private AI_MultiAction m_maction; // 数据类 
        private int groupIndex = 0; // 自动生成动作标号
        private int graphElementID; // 当前编辑的图元ID

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="maction">动作参数</param>
        public MultiActionForm(AI_MultiAction maction)
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;

            if (maction != null)
            {
                this.m_maction = maction;

                if (maction.ActionList == null)
                {
                    maction.ActionList = new List<AI_SingleAction>();
                }

                foreach (AI_SingleAction action in maction.ActionList)
                {
                    listBox1.Items.Add(action.Clone());
                }
            }                       
        }

        /// <summary>
        /// 当前编辑图元的ID
        /// </summary>
        public int GraphElementID
        {
            set
            {
                graphElementID = value;
            }
        }        

        /// <summary>
        /// 初始化自定义动作自动命名的种子
        /// </summary>
        /// <param name="aiSingleAction">ai动作</param>
        private void InitActionIndex(AI_SingleAction aiSingleAction)
        {
            int maxID = 0;
            
            if (aiSingleAction != null)
            {
                string actionName = aiSingleAction.Action.Ename;
                int index = actionName.LastIndexOf("_");

                if (index > 0)
                {
                    int tempID = -1;

                    if (int.TryParse(actionName.Substring(index + 1, actionName.Length - index - 1), out tempID))
                    {
                        maxID = tempID + 1;
                    }
                }
            }
            else
            {
                foreach (object o in listBox1.Items)
                {
                    AI_SingleAction singleAction = o as AI_SingleAction;

                    if (singleAction != null)
                    {
                        AI_Action aiAction = singleAction.Action;

                        if (int.Parse(aiAction.ActionID) > 2000)
                        {
                            string actionName = aiAction.Ename;
                            int index = actionName.LastIndexOf("_");

                            if (index > 0)
                            {
                                int tempID = -1;

                                if (int.TryParse(actionName.Substring(index + 1, actionName.Length - index - 1), out tempID))
                                {
                                    maxID = tempID + 1;
                                }
                            }
                        }
                    }
                }
            }            

            groupIndex = maxID;
        }
        
        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.m_maction.ActionList.Clear();
            foreach (object o in listBox1.Items)
            {
                this.m_maction.ActionList.Add(o as AI_SingleAction);
            }

            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 添加动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnAdd_Click(object sender, EventArgs e)
        {
            // 初始化自动重命名种子
            InitActionIndex(null);

            AI_SingleAction singleAction = new AI_SingleAction();
            SingleActionForm singleActionForm = new SingleActionForm();

            singleActionForm.GraphElementID = graphElementID;
            singleActionForm.InActionGroup = true;
            singleActionForm.GroupIndex = groupIndex;
            singleActionForm.Reload(singleAction);
            singleActionForm.ShowDialog();

            if (singleActionForm.EditSuccess)
            {
                listBox1.Items.Add(singleAction);
            }         
        }

        /// <summary>
        /// 删除动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnDel_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem != null)
            {
                if (MessageBox.Show("确定要删除当前动作吗？", "删除动作", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    this.listBox1.Items.Remove(this.listBox1.SelectedItem);
                }
            }
        }

        /// <summary>
        /// 向上移动
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnMoveUp_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem != null && this.listBox1.SelectedIndex > 0)
            {
                int nIndex = this.listBox1.SelectedIndex;
                object o = this.listBox1.SelectedItem;
                this.listBox1.Items.Remove(o);
                this.listBox1.Items.Insert(nIndex - 1, o);
                this.listBox1.SelectedItem = o;
            }
        }

        /// <summary>
        /// 向下移动
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnMoveDown_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedIndex >= 0 && this.listBox1.SelectedIndex < this.listBox1.Items.Count)
            {
                int nIndex = this.listBox1.SelectedIndex;
                if (nIndex == this.listBox1.Items.Count - 1) return;
                object o = this.listBox1.SelectedItem;
                this.listBox1.Items.Remove(o);
                this.listBox1.Items.Insert(nIndex + 1, o);
                this.listBox1.SelectedItem = o;
            }
        }

        /// <summary>
        /// 复制动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonCopy_Click(object sender, EventArgs e)
        {
            ICloneable copyFromExp = this.listBox1.SelectedItem as ICloneable;

            if (copyFromExp != null)
            {
                object newExp = copyFromExp.Clone();
                this.listBox1.Items.Add(newExp);
                this.listBox1.SelectedItem = newExp;
            }
        }

        /// <summary>
        /// 编辑动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            AI_SingleAction singleAction = this.listBox1.SelectedItem as AI_SingleAction;

            if (singleAction != null)
            {
                // 初始化自动重命名种子
                InitActionIndex(singleAction);

                SingleActionForm saForm = new SingleActionForm();
                saForm.GraphElementID = graphElementID;
                saForm.InActionGroup = true;
                saForm.GroupIndex = groupIndex;
                saForm.Reload(singleAction);
                saForm.ShowDialog();

                if (saForm.EditSuccess)
                {
                    int ind = listBox1.SelectedIndex;
                    listBox1.Items.RemoveAt(ind);
                    listBox1.Items.Insert(ind, singleAction);
                }
            }                
        }
    }
}