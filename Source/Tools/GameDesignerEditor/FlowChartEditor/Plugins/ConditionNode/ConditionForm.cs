using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using BaseExp;
using System.Text.RegularExpressions;

namespace ConditionNode
{
    public partial class ConditionForm : DevComponents.DotNetBar.Office2007Form
    {
        private Exp m_actexp;
        private GameEvent m_gameEvent;
        private List<Exp> m_elist;
        private bool m_bChanged = false;

        private VarExp m_Vexp = new VarExp();

        //返回条件表达式
        public Exp ResultExp
        {
            get { return m_actexp; }
        }

        //返回线表达式列表
        public List<Exp> LineExpList
        {
            get { return m_elist; }
        }

        public ConditionForm(Exp actexp, GameEvent gameEvent, List<Exp> ConditionList)
        {
            InitializeComponent();

            this.m_actexp = actexp;
            this.m_elist = ConditionList;
            this.m_gameEvent = gameEvent;

            this.textBoxX1.Text = m_actexp.ToString();
            this.listBox1.Items.AddRange(ConditionList.ToArray());

        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (this.m_actexp == null || this.m_actexp.ReturnType == null || this.m_actexp.ReturnType.DBID == FixedValueProvider.TYPE_NIL_ID)
            {
                MessageBox.Show("先输入条件表达式", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            //选中填充API默认值
            ActionExp expa = ExpManager.CreateNewActionExp(new GameAPIDefine(FixedValueProvider.COMPARE_API_ID));

            expa.API.ArgValues[0] = m_Vexp;           //左值为可变类型
            expa.API.ArgValues[1] = ExpManager.CreateUnknownExp();      //右值为空类型
            m_Vexp.ReturnType = expa.API.ArgValues[1].ReturnType = this.m_actexp.ReturnType;     //让自动填充的左右两个表达式，类型和上面一致


            ExpForm expform = new ExpForm(expa, this.m_gameEvent, true, expa.ReturnType, m_Vexp);
            expform.StartPosition = FormStartPosition.CenterParent;
            if (expform.ShowDialog() == DialogResult.OK)
            {
                this.listBox1.Items.Add(expform.RusultExp);
                this.m_bChanged = true;
            }
        }

        private void btnExp_Click(object sender, EventArgs e)
        {
            ExpForm expform = new ExpForm(this.m_actexp, this.m_gameEvent, true, null, null);
            expform.StartPosition = FormStartPosition.CenterParent;
            if (expform.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (this.m_actexp.ReturnType != null && this.m_actexp.ReturnType.DBID != expform.RusultExp.ReturnType.DBID)
                {
                    //清空连接线
                    this.listBox1.Items.Clear();
                }
                this.m_actexp = expform.RusultExp;
                this.m_Vexp.ReturnType = this.m_actexp.ReturnType;      //可变类型的返回值和表达式一致
                this.textBoxX1.Text = this.m_actexp.ToString();
                this.m_bChanged = true;
            }
            else
            {
                return;
            }

            //自动添加分支
            if (this.m_actexp != null && this.m_actexp.ReturnType != null)
            {
                //尝试添加枚举的所有类型
                if (this.m_actexp.ReturnType.isEnum)
                {
                    ConstExp[] cexpArray = ExpManager.GetConstExpByReturnType(this.m_actexp.ReturnType);
                    if (cexpArray != null)
                    {
                        foreach (ConstExp cexp in cexpArray)
                        {
                            ActionExp expa = ExpManager.CreateNewActionExp(new GameAPIDefine(FixedValueProvider.COMPARE_API_ID));
                            expa.API.ArgValues[0] = m_Vexp;           //左值为可变类型
                            expa.API.ArgValues[1] = cexp;             //右值为常数值
                            expa.strText = m_Vexp.ToString() + "==" + cexp.ToString();
                            if (!listBox1.Items.Contains(expa))
                            {
                                this.listBox1.Items.Add(expa);
                                this.m_bChanged = true;
                            }
                        }
                    }
                }

                //尝试添加OpenWindow的选项
                if (this.m_actexp is ActionExp)
                {
                    ActionExp aexp = this.m_actexp as ActionExp;
                    if (aexp.API.DBID == FixedValueProvider.OPENWINDOW_ID)          //多选项窗口
                    {
                        Exp dialogExp = aexp.API.ArgValues[2];      //第三个参数
                        if (dialogExp is ConstExp)                  //如果格式文本输入的是常数才处理
                        {
                            string strDialog = (dialogExp as ConstExp).DBValue;
                            Regex reg = new Regex(@"(?!=<)\$[^>]*(?=>)");
                            foreach (Match mat in  reg.Matches(strDialog))
                            {
                                string strSelection = mat.Value.TrimStart(new char[] { '$', 'C', ' ' });
                                ActionExp expa = ExpManager.CreateNewActionExp(new GameAPIDefine(FixedValueProvider.COMPARE_API_ID));
                                expa.API.ArgValues[0] = m_Vexp;           //左值为可变类型
                                ConstExp cexp = new ConstExp(strSelection, new TypeDefine(FixedValueProvider.TYPE_STRING_ID));
                                m_Vexp.ReturnType = cexp.ReturnType;
                                expa.API.ArgValues[1] = cexp;             //右值为字符串
                                expa.strText = m_Vexp.ToString() + "==" + cexp.ToString();
                                if (!listBox1.Items.Contains(expa))
                                {
                                    this.listBox1.Items.Add(expa);
                                    this.m_bChanged = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        private void btnDel_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem != null)
            {
                if (MessageBox.Show("要删除表达式，确定一定以及肯定？", "确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    this.listBox1.Items.Remove(this.listBox1.SelectedItem);
                    this.m_bChanged = true;
                }
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

        private void listBox1_DoubleClick(object sender, EventArgs e)
        {
            if (listBox1.SelectedItem != null)
            {
                ActionExp expa = listBox1.SelectedItem as ActionExp;
                ExpForm expform = new ExpForm(expa, this.m_gameEvent, true, expa.ReturnType, m_Vexp);
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

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (this.m_bChanged == false)
            {
                this.DialogResult = DialogResult.Cancel;
                this.Close();
            }
            else
            {
                this.m_elist.Clear();
                for (int i = 0; i < listBox1.Items.Count; i++)
                {
                    this.m_elist.Add(listBox1.Items[i] as Exp);
                }
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {

        }
    }
}