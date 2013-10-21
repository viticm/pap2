using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using BaseExp;
using System.Collections;

namespace ActionNode
{
    public partial class ActionListForm : DevComponents.DotNetBar.Office2007Form
    {
        public ActionExp[] actionExpList = null;

        private static ActionExp[] unsavedActionList = null;        //未保存的表达式，用于给变量模块统计变量用。虽然未保存，还是能选择到变量才行

        private GameEvent m_evt = null;
        private bool m_bChanged = false;

        public static ArrayList UnsavedActionList
        {
            get 
            {
                ArrayList al = new ArrayList();
                al.AddRange(unsavedActionList);
                return al; 
            }
        }

        //返回动作节点的屏显文本
        public string actionExpListText
        {
            get
            {
                if(actionExpList== null)
                {
                    return "";
                }
                string strRet = "";
                for (int i = 0; i < actionExpList.Length; i++)
                {
                    string strText = actionExpList[i].strFullText;
                    if (strText.Length > 15)
                        strText = strText.Substring(0, 13) + "...";
                    strRet += "◆ " + strText + "\r\n";
                }
                return strRet;
            }
        }

        //返回动作节点的Tip文本
        public string actionExpListText_Full
        {
            get
            {
                if(actionExpList== null)
                {
                    return "";
                }
                string strRet = "";
                for (int i = 0; i < actionExpList.Length; i++)
                {
                    strRet += "◆ " + actionExpList[i].ToString() + "\r\n";
                }
                return strRet;
            }
        }

        public ActionListForm(ActionExp[] aexpList, GameEvent evt)
        {
            if (evt == null) throw new Exception("动作列表中传入的事件为空");
            this.m_evt = evt;
            this.actionExpList = aexpList;
            if (this.actionExpList == null)
            {
                this.actionExpList = new ActionExp[0];
            }
            InitializeComponent();
        }

        private void btnAdd_Click(object sender, EventArgs e) // 添加
        {
            Exp expa = ExpManager.CreateNewExp(TypeDefine.NilType);
            ExpForm expform = new ExpForm(expa, this.m_evt, true, expa.ReturnType, null);
            expform.StartPosition = FormStartPosition.CenterParent;
            if(expform.ShowDialog() == DialogResult.OK)
            {
                this.listBox1.Items.Add(expform.RusultExp);
                //this.listBox1.Items.Insert(this.listBox1.SelectedIndex+1, );
                UpdateUnsavedActionList();
                this.m_bChanged = true;
            }
        }

        private void UpdateUnsavedActionList()
        {
            unsavedActionList = new ActionExp[listBox1.Items.Count];
            for(int i = 0 ; i < listBox1.Items.Count; i++)
            {
                unsavedActionList[i] = listBox1.Items[i] as ActionExp;
            }
        }

        private void btnCopy_Click(object sender, EventArgs e) // 复制
        {
            Exp copyFromExp = this.listBox1.SelectedItem as Exp;
            if (copyFromExp != null)
            {
                Exp newExp = copyFromExp.Clone() as Exp;
                this.listBox1.Items.Add(newExp);
                this.listBox1.SelectedItem = newExp;
                this.m_bChanged = true;
            }
        }

        private void ActionListForm_Load(object sender, EventArgs e)
        {
            this.listBox1.Items.AddRange(this.actionExpList);
        }

        private void btnDel_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem != null)
            {
                if(MessageBox.Show("要删除表达式，确定一定以及肯定？","确认",MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    this.listBox1.Items.Remove(this.listBox1.SelectedItem);
                    UpdateUnsavedActionList();
                    this.m_bChanged = true;
                }
            }
        }

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

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(this.m_bChanged == false)
            {
                unsavedActionList = null;
                this.DialogResult = DialogResult.Cancel;
                this.Close();
            }
            else
            {
                this.actionExpList = new ActionExp[listBox1.Items.Count];
                for (int i = 0; i < actionExpList.Length; i++)
                {
                    this.actionExpList[i] = listBox1.Items[i] as ActionExp;
                }
                unsavedActionList = null;
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem != null)
            {
                ActionExp expa = listBox1.SelectedItem as ActionExp;
                ExpForm expform = new ExpForm(expa, this.m_evt, true, expa.ReturnType, null);
                expform.StartPosition = FormStartPosition.CenterParent;
                if (expform.ShowDialog() == DialogResult.OK)
                {
                    int ind = listBox1.SelectedIndex;
                    listBox1.Items.RemoveAt(ind);
                    listBox1.Items.Insert(ind, expform.RusultExp);
                    this.m_bChanged = true;
                }
            }
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            unsavedActionList = null;
        }
    }
}