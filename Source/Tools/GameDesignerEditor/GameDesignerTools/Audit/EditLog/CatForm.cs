using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class CatForm : Form
    {
        private List<string> catList = new List<string>(); // 分类信息链表
        private string catNames; // 分类字符串
        private bool beginSelect = false; // 是否已经开始选择
        private string oldValue; // 旧的分类字符串
        private EditLog editLog; // 查询窗体对象

        public CatForm()
        {         
            InitializeComponent();           
        }

        /// <summary>
        /// 设置参数
        /// </summary>
        /// <param name="startPoint">开始位置</param>
        /// <param name="oldValue">旧输入值</param>
        /// <param name="editLog">查询窗体对象</param>
        public void SetParameters(Point startPoint, string oldValue, EditLog editLog)
        {
            this.oldValue = oldValue;
            this.Location = startPoint;  
            this.editLog = editLog;
            catList.Clear();
        }

        /// <summary>
        /// 添加分类字符串
        /// </summary>
        /// <param name="s">分类字符串</param>
        public void AddData(string s)
        {
            if(!catList.Contains(s))
            {
                catList.Add(s);
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        public void InitData()
        {           
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add("所有分类");

            foreach(string s in catList)
            {
                string[] catString = s.Split(',');
                TreeNodeCollection currentNodes = treeView1.Nodes; // 当前树结点集合
                TreeNode currentNode; // 当前树结点
                bool foundNode = false; // 是否已经找到树结点

                for (int i = catString.Length - 1; i >= 0; i--) // 倒序添加
                {
                    foundNode = false;
                    foreach (TreeNode node1 in currentNodes)
                    {
                        if (node1.Text == catString[i])
                        {
                            currentNode = node1;
                            currentNodes = node1.Nodes;
                            foundNode = true;
                            break;
                        }
                    }

                    if (!foundNode) // 该树结点未存在
                    {
                        currentNode = currentNodes.Add(catString[i]);
                        currentNodes = currentNode.Nodes;
                    }
                }
            }

            ShowData();
        }

        /// <summary>
        /// 显示当前的分类
        /// </summary>
        private void ShowData()
        {
            string[] catString = oldValue.Split(',');
            TreeNodeCollection currentNodes = treeView1.Nodes; // 当前树结点集合
            TreeNode currentNode = null; // 当前树结点
            bool foundNode = false; // 是否已经找到树结点

            for (int i = catString.Length - 1; i >= 0; i--) // 倒序添加
            {
                foundNode = false;
                foreach (TreeNode node1 in currentNodes)
                {
                    if (node1.Text == catString[i])
                    {
                        currentNode = node1;
                        currentNodes = node1.Nodes;
                        foundNode = true;
                        break;
                    }
                }

                if (!foundNode) // 该树结点未存在
                {
                    break;
                }
            }

            if(currentNode != null) // 选中当前树结点
            {
                treeView1.SelectedNode = currentNode;
            }
        }

        /// <summary>
        /// 用户选择树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if(beginSelect) // 忽略第一次选择
            {
                TreeNode currentNode = treeView1.SelectedNode;

                if(currentNode != null) // 已经选中树结点 
                {
                    string tempCatNames = currentNode.Text + ",";
                    TreeNode parentNode = currentNode.Parent;
                    while(parentNode != null)
                    {
                        tempCatNames += string.Format("{0},", parentNode.Text);
                        parentNode = parentNode.Parent;
                    }
                    catNames = tempCatNames.TrimEnd(',');
                    editLog.ResetCatName(catNames);
                    this.Visible = false;
                }  
            }
            else
            {
                beginSelect = true;
            }
        }

        /// <summary>
        /// 用户鼠标点击树结点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode currentNode = treeView1.SelectedNode;
            if(currentNode == e.Node) // 当前已选的树结点无法再次选中
            {
                // treeView1_AfterSelect(null, null); // 强制选中已经选中的树结点
            }
        }

        /// <summary>
        /// 窗体不再活跃
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void CatForm_Deactivate(object sender, EventArgs e)
        {
            this.Visible = false;
        }
    }
}