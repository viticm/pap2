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
    public partial class ViewForm : Form
    {
        private DataTable table; // 变量数据表
        private Hashtable bindTypeTable = new Hashtable(); // 绑定类型的哈希表
        private bool beginView = false; // 是否已经开始查看
        private Hashtable viewData = new Hashtable(); // 过滤条件哈希表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="table">变量数据表</param>
        public ViewForm(DataTable table)
        {
            this.table = table;
            InitializeComponent();
            InitData();
        }

        /// <summary>
        /// 过滤条件哈希表
        /// </summary>
        public Hashtable ViewData
        {
            get
            {
                return viewData;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            List<string> userNameList = new List<string>();
            List<string> varTypeList = new List<string>();
            List<string> bindTypeList = new List<string>();
            
            userNameList.Add("所有用户");
            varTypeList.Add("所有变量类型");
            bindTypeList.Add("所有绑定类型");

            List<string> list = new List<string>();
            list.Add("所有绑定对象");
            bindTypeTable["所有绑定类型"] = list;
            
            string userName;
            string varType;
            string bindType;
            string bindObject;

            foreach(DataRow row in table.Rows)
            {
                userName = row["UserName"] as string;
                varType = row["Type"] as string;
                bindType = row["bindType"] as string;
                bindObject = row["bindObject"] as string;

                if(!userNameList.Contains(userName))
                {
                    userNameList.Add(userName);
                }

                if(!varTypeList.Contains(varType))
                {
                    varTypeList.Add(varType);
                }

                if(!bindTypeList.Contains(bindType))
                {
                    bindTypeList.Add(bindType);
                    List<string> bindObjectList = new List<string>();
                    bindObjectList.Add("所有绑定对象");
                    bindObjectList.Add(bindObject);
                    bindTypeTable[bindType] = bindObjectList;
                }
                else
                {
                    List<string> bindObjectList = bindTypeTable[bindType] as List<string>;
                    if(!bindObjectList.Contains(bindObject))
                    {
                        bindObjectList.Add(bindObject);
                    }
                }                
            }

            foreach(string s in userNameList)
            {
                comboBoxEx1.Items.Add(s);
            }

            foreach(string s in varTypeList)
            {
                comboBoxEx2.Items.Add(s);
            }

            foreach(string s in bindTypeList)
            {
                comboBoxEx3.Items.Add(s);
            }

            comboBoxEx1.Text = "所有用户";
            comboBoxEx2.Text = "所有变量类型";
            comboBoxEx3.Text = "所有绑定类型";
            comboBoxEx4.Text = "所有绑定对象";

            beginView = true;
        }


        /// <summary>
        /// 改变绑定类型
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx3_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(beginView)
            {
                List<string> bindObjectList = bindTypeTable[comboBoxEx3.Text] as List<string>;
                comboBoxEx4.Items.Clear();

                foreach(string s in bindObjectList)
                {
                    comboBoxEx4.Items.Add(s);
                }

                comboBoxEx4.Text = "所有绑定对象";
            }
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string userName = comboBoxEx1.Text;
            string varType = comboBoxEx2.Text;
            string bindType = comboBoxEx3.Text;
            string bindObject = comboBoxEx4.Text;

            if(userName == "" || varType == "" || bindType == "" || bindObject == "")
            {
                MessageBox.Show("过滤条件不能为空！", "参数检查",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                viewData["userName"] = userName;
                viewData["type"] = varType;
                viewData["bindType"] = bindType;
                viewData["bindObject"] = bindObject;

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