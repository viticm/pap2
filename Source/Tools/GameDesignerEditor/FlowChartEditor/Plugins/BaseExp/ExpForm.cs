using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DBManager;
using System.Collections;

namespace BaseExp
{
    public partial class ExpForm : DevComponents.DotNetBar.Office2007Form
    {
        public VarExp VExp = null;
        
        private Exp m_exp = null;                   //需要返回的表达式
        private bool enableAPIListChange = true;
        private GameEvent m_eventDefine = null;
        private bool m_showThisEvent = true;        //是否可以选择“本事件”
        private Control m_FousedControl = null;
        private TypeDefine m_requireRetType = null; //返回的类型。约定null为不指定类型

        //返回值表达式
        public Exp RusultExp
        {
            get { return m_exp; }
        }

        //通用,完整参数版
        public ExpForm(Exp exp, GameEvent gameEvent, bool showThisEvent, TypeDefine requireRetType, VarExp vexp)
        {
            this.m_showThisEvent = showThisEvent;
            this.m_requireRetType = requireRetType;
            this.VExp = vexp;
            ExpForm_Show(exp, gameEvent);
        }

        public void ExpForm_Show(Exp exp, GameEvent gameEvent)
        {
            System.Diagnostics.Debug.Assert(exp != null);
            System.Diagnostics.Debug.Assert(gameEvent != null);

            InitializeComponent();

            this.expControl1.VExp = this.VExp;

            enableAPIListChange = false;

            this.m_eventDefine = gameEvent;
            this.m_exp = exp.Clone() as Exp;
            if(exp.ReturnType == null)
            {
                this.Text = "任意表达式";
            }
            else
            {
                this.Text = exp.ReturnType.ToString() + " 表达式";
            }

            this.DialogResult = DialogResult.Cancel;

            // 刷新功能列表
            GameAPIDefine[] ret;
            if(this.m_requireRetType == null) // 表达式类型未知，用于条件判断中输入未知类型的节点用。
            {
                // 获取所有有返回值的API
                // ret = ExpManager.GetAPIDefineListExceptReturnType(TypeDefine.NilType); 如果只能选有返回值的API的话，那么几乎每个有返回值API都要做一个无返回值的版本才能在动作组里没选到，
                // 这样会带来很大的维护工作量，所以修正为可以选择所有的API。
                ret = ExpManager.GetAPIDefine_All();

                this.radioConst.Enabled = false;
                this.radioConstSel.Enabled = false;
                this.comboBoxConst.Enabled = false;
                this.txtConst.Enabled = false;
                this.btnSelect.Enabled = false;

                this.comboBoxAPI.TabIndex = 2;
            }
            else
            {
                // 专用API列出来
                ret = ExpManager.GetAPIDefineByReturnType(exp.ReturnType);
            }

            if (ret != null)
            {
                comboBoxAPI.Items_AddRange(Helper.SortByToString(ret));
            }

            // 刷新常数列表
            if (exp.ReturnType != null)
            {
                if (exp.ReturnType.isEnum)
                {
                    this.txtConst.Enabled = false;
                    this.radioConst.Enabled = false;
                    ConstExp[] const_ret = ExpManager.GetConstExpByReturnType(exp.ReturnType);
                    if (const_ret != null) comboBoxConst.Items.AddRange(const_ret);
                    if (comboBoxConst.Items.Count > 0)
                        comboBoxConst.SelectedItem = comboBoxConst.Items[0];
                    if (!exp.ReturnType.ContainsValueEdit)
                        this.btnSelect.Enabled = false;
                }
                else
                {
                    this.comboBoxConst.Enabled = false;
                    this.radioConstSel.Enabled = false;
                    this.btnSelect.Enabled = false;
                    if (exp.ReturnType.ContainsValueEdit)
                        this.btnInput.Enabled = true;
                }
            }

            //刷新本事件列表
            if(this.m_showThisEvent)
            {
                for (int i = 0; i < gameEvent.ArgList.Length; i++)
                {
                    if (exp.ReturnType == null || gameEvent.ArgList[i].ArgType.DBID == exp.ReturnType.DBID)
                    {
                        comboBoxEventArg.Items.Add(gameEvent.GetArgExp(i + 1));
                    }

                    if (comboBoxEventArg.Items.Count > 0)
                        comboBoxEventArg.SelectedItem = comboBoxEventArg.Items[0];
                }
                //对任意类型的支持，在这里可以选到
                if (this.VExp != null)
                {
                    if (exp.ReturnType == null || this.VExp.ReturnType== null || this.VExp.ReturnType.DBID == exp.ReturnType.DBID)
                    {
                        comboBoxEventArg.Items.Add(this.VExp);
                    }
                }
            }

            //灰掉某些东东
            if (comboBoxAPI.Items_Count == 0)
            {
                //灰掉API
                comboBoxAPI.Enabled = false;
                radioExp.Enabled = false;
            }
            //灰掉常数
            if (comboBoxConst.Items.Count == 0)// && exp.ReturnType.m_isEnum)
                comboBoxConst.Enabled = false;
            if (exp.ReturnType == null || !exp.ReturnType.isDuplicate)
                txtConst.Enabled = false;
            if (txtConst.Enabled == false && comboBoxConst.Enabled == false)
                radioConst.Enabled = false;
            //灰掉本事件
            if (comboBoxEventArg.Items.Count == 0)
            {
                comboBoxEventArg.Enabled = false;
                radioEventArg.Enabled = false;
            }

            //自动选中API
            if (m_exp is ActionExp)
            {
                foreach (GameAPIDefine apidefine in comboBoxAPI.Items_All)
                {
                    if (apidefine.DBID == (m_exp as ActionExp).API.DBID)
                    {
                        
                        this.comboBoxAPI.Text = apidefine.ToString() + " ";
                        //this.comboBoxAPI.Items;
                        break;
                    }
                }

                this.expControl1.SetComboText(m_exp as ActionExp, this.m_eventDefine);
                this.expControl1.VExp = this.VExp;
                this.radioExp.Checked = true;
                //this.expControl1.Focus();
                this.m_FousedControl = this.expControl1;
            }

            //自动选中常数
            if (m_exp is ConstExp)
            {
                ConstExp conExp = m_exp as ConstExp;
                if (conExp.ReturnType.isEnum)
                {
                    if(conExp.ReturnType.ContainsValueEdit) //使用lua脚本编辑
                    {
                        this.comboBoxConst.Items.Clear();
                        this.comboBoxConst.Items.Add(conExp);
                        this.comboBoxConst.SelectedItem = conExp;
                        //this.comboBoxConst.Enabled = true;
                    }
                    else                                    //使用下拉菜单编辑
                    {
                        foreach (ConstExp ex in comboBoxConst.Items)
                        {
                            if (ex.DBValue == conExp.DBValue)
                            {
                                this.comboBoxConst.SelectedItem = ex;
                                break;
                            }
                        }
                        if (this.comboBoxConst.SelectedItem == null && comboBoxConst.Items.Count > 0)
                            comboBoxConst.SelectedItem = comboBoxConst.Items[0];
                    }
                    this.radioConstSel.Checked = true;
                    this.m_FousedControl = this.radioConstSel;
                }
                else
                {
                    this.txtConst.Text = m_exp.strText;
                    this.radioConst.Checked = true;
                    //this.radioConst.Focus();
                    //this.txtConst.Focus();
                    this.m_FousedControl = this.txtConst;
                }
            }



            //自动选中本事件
            if (m_exp is ObjectExp)
            {
                ObjectExp oExp = m_exp as ObjectExp;
                foreach (ObjectExp ex in comboBoxEventArg.Items)
                {
                    if (ex.AsEventArgPos == oExp.AsEventArgPos)
                    {
                        this.comboBoxEventArg.SelectedItem = ex;
                        break;
                    }
                }
                this.radioEventArg.Checked = true;
                //this.comboBoxEventArg.Focus();
                this.m_FousedControl = this.comboBoxEventArg;
            }

            //自动选中无类型变量
            if (m_exp is VarExp)
            {
                foreach (Exp ex in  comboBoxEventArg.Items)
                {
                    if (ex is VarExp)
                    {
                        this.comboBoxEventArg.SelectedItem = ex;
                    }

                }
                this.radioEventArg.Checked = true;
                this.m_FousedControl = this.comboBoxEventArg;
            }


            if (!this.radioConst.Checked && !this.radioEventArg.Checked && !this.radioEventArg.Checked && !this.radioExp.Checked)
            {
                this.radioConst.Checked = true;
                this.m_FousedControl = txtConst;
            }
            enableAPIListChange = true;

            this.comboBoxAPI.SelectedIndexChanged += new System.EventHandler(this.comboBoxAPI_SelectedIndexChanged);
        }

        

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (!(this.radioConstSel.Checked || this.radioExp.Checked || this.radioConst.Checked || this.radioEventArg.Checked))
            {
                MessageBox.Show("必须先选择一个。","错误",MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (this.radioExp.Checked)
            {
                //if (!expControl1.GetReady)
                //{
                //    MessageBox.Show("请输入完整", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                //    return;
                //}
                this.m_exp = expControl1.ActionExp;
            }
            else if (this.radioConst.Checked)
            {
                if(txtConst.Text == "")
                {
                    MessageBox.Show("请输入完整", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                this.m_exp = new ConstExp(txtConst.Text, m_exp.ReturnType);
            }
            else if (this.radioConstSel.Checked)
            {
                if (this.comboBoxConst.SelectedItem == null)
                {
                    MessageBox.Show("输入不完整，请选择", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                this.m_exp = this.comboBoxConst.SelectedItem as Exp;
            }
            else if (this.radioEventArg.Checked)
            {
                if (this.comboBoxEventArg.SelectedItem == null)
                {
                    MessageBox.Show("输入不完整，请选择", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                this.m_exp = this.comboBoxEventArg.SelectedItem as Exp;
            }
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void comboBoxAPI_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (!radioExp.Checked)
            {
                radioExp.Checked = true;
            }

            if (comboBoxAPI.Items_SelectedItem == null) return;

            //创建一个空的动作表达式
            ActionExp expAction = ExpManager.CreateNewActionExp(comboBoxAPI.Items_SelectedItem as GameAPIDefine);

            //对比较API不默认赋值
            if (expAction.API.DBID != FixedValueProvider.COMPARE_API_ID)
            {
                //赋默认值（暂时只考虑本事件的参数、数字的情况）
                for (int i = 0; i < expAction.API.ArgList.Length; i++ )
                {
                    ArgsDefine ad = expAction.API.ArgList[i];
                    if(!ad.ArgType.isDuplicate)
                    {
                        //本事件的参数
                        for (int j = 0; j < this.m_eventDefine.ArgList.Length; j++)
                        {
                            if (ad.ArgType.DBID == this.m_eventDefine.ArgList[j].ArgType.DBID)
                            {
                                expAction.API.ArgValues[i] = this.m_eventDefine.GetArgExp(j + 1);
                                break;
                            }
                        }
                    }
                    else
                    {
                        //数字、字符串等(其他)常数默认值
                        if (ad.DefaultValue != null && ad.DefaultValue != "-" && ad.DefaultValue != "")
                        {
                            expAction.API.ArgValues[i] = new ConstExp(ad.DefaultValue, ad.ArgType);
                        }
                    }
                }
            }

            //让控件显示这个表达式
            expControl1.SetComboText(expAction, this.m_eventDefine);
        }


        private void comboBoxConst_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (!radioConstSel.Checked)
            {
                radioConstSel.Checked = true;
                comboBoxConst.Focus();
            }
        }



        private void comboBoxEventArg_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (!radioEventArg.Checked)
            {
                radioEventArg.Checked = true;
                comboBoxEventArg.Focus();
            }
        }

        private void txtConst_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (!radioConst.Checked)
            {
                radioConst.Checked = true;
                txtConst.Focus();
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void radioConst_CheckedChanged(object sender, EventArgs e)
        {
            if(radioConst.Checked) txtConst.Focus();
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            this.radioConstSel.Checked = true;
            ConstExp cexp = this.comboBoxConst.SelectedItem as ConstExp;
            string dbvalue = "";
            if(cexp != null)
            {
                dbvalue = cexp.DBValue;
            }
            string strLua = this.m_exp.ReturnType.QueryScript;
            object[] oRet = LuaManager.GetLuaManager().OnValueEdit(strLua, this.m_exp.ReturnType.QueryScript, dbvalue);
            if(oRet.Length > 0 && ((bool)oRet[0]))
            {
                if(oRet.Length > 1)
                {
                    ConstExp ce = new ConstExp(oRet[1].ToString(), this.m_exp.ReturnType);
                    this.comboBoxConst.Items.Clear();
                    this.comboBoxConst.Items.Add(ce);
                    this.comboBoxConst.SelectedItem = ce;
                    this.comboBoxConst.Enabled = true;
                }
            }
        }

        private void radioButton_EnabledChanged(object sender, EventArgs e)
        {
            RadioButton r = sender as RadioButton;
            if (r.Enabled == false)
                r.Checked = false;
        }

        private void btnInput_Click(object sender, EventArgs e)
        {
            this.radioConst.Checked = true;
            string dbvalue = txtConst.Text;
            string strLua = this.m_exp.ReturnType.QueryScript;
            object[] oRet = LuaManager.GetLuaManager().OnValueEdit(strLua, this.m_exp.ReturnType.QueryScript, dbvalue);
            if (oRet.Length > 0 && ((bool)oRet[0]))
            {
                if (oRet.Length > 1)
                {
                    this.txtConst.Text = oRet[1].ToString();
                }
            }          
        }

        private void radioConst_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (radioConst.Checked) txtConst.Focus();
        }

        private void radioConstSel_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (radioConstSel.Checked)
            {
                if (this.comboBoxConst.Enabled)
                    comboBoxConst.Focus();
                if (this.btnSelect.Enabled)
                    btnSelect.Focus();
            } 
        }

        private void radioEventArg_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (radioEventArg.Checked) comboBoxEventArg.Focus();
        }

        private void radioExp_Enter(object sender, EventArgs e)
        {
            if (!enableAPIListChange) return;
            if (!radioExp.Checked) return;
            if (this.comboBoxAPI.Items_SelectedItem == null)
                this.comboBoxAPI.Focus();
            else
                this.expControl1.Focus();
           
        }

        private void ExpForm_Load(object sender, EventArgs e)
        {
            //if (this.m_FousedControl != null)
            //    this.m_FousedControl.Focus();
        }

        private void ExpForm_Shown(object sender, EventArgs e)
        {
            if (this.m_FousedControl != null)
                this.m_FousedControl.Focus();
        }
    }

    public static class Helper
    {
        public static object[] SortByToString(object[] os)
        {
            if (os == null) return null;
            if (os.Length == 0) return os;
            ArrayList al_ret = new ArrayList();
            for(int i = 0; i < os.Length; i++)
            {
                al_ret.Add(os[i]);
            }
            al_ret.Sort();
            object[] os_ret = new object[os.Length];
            for (int i = 0; i < os.Length; i++)
            {
                os_ret[i] = al_ret[i];
            }
            return os_ret;
        }
    };
}