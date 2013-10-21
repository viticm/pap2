using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class FiltrateForm : Form
    {
        TreeView baseTree; // 分类树
        List<TreeNode> treeNodeList = new List<TreeNode>(); // 叶子树结点链表
        List<TreeNode> baseNodeList = new List<TreeNode>(); // BaseForm叶子树结点链表

        public FiltrateForm(TreeView baseTree)
        {
            this.baseTree = baseTree;
            InitializeComponent();
            ReconstructTree(baseTree);
        }

        /// <summary>
        /// 重新构造树
        /// </summary>
        /// <param name="tree">旧的树</param>
        private void ReconstructTree(TreeView tree)
        {
            treeView1.Nodes.Clear();
            treeNodeList.Clear();
            foreach(TreeNode node in tree.Nodes)
            {
                TreeNode newNode = new TreeNode(node.Text);
                treeView1.Nodes.Add(newNode);
                ReconstructTreeNode(node, newNode);
            }
        }

        /// <summary>
        /// 重新构造树结点
        /// </summary>
        /// <param name="oldNode">旧的树结点</param>
        /// <param name="newNode">新的树结点</param>
        private void ReconstructTreeNode(TreeNode oldNode, TreeNode newNode)
        {
            foreach(TreeNode node in oldNode.Nodes)
            {
                TreeNode freshNode = new TreeNode(node.Text);
                newNode.Nodes.Add(freshNode);
                ReconstructTreeNode(node, freshNode);
            }

            if(oldNode.Nodes.Count == 0) // 如果是叶子结点的话记录到索引链表里
            {
                treeNodeList.Add(newNode);
                baseNodeList.Add(oldNode);
            }
        }

        /// <summary>
        /// 过滤出符合条件的树结点
        /// </summary>
        /// <param name="text">关键字</param>
        private void FiltrateTreeNode(string text)
        {
            int count = 0;

            if(text == "") // 关键字为空的话，显示全部树结点
            {
                ReconstructTree(baseTree);       
            }
            else
            {
                List<TreeNode> nodeList = new List<TreeNode>();
                TreeNode parentNode;
                TreeNode tempNode;
                TreeNodeCollection currentNodes;

                treeView1.Nodes.Clear();

                foreach(TreeNode node in treeNodeList)
                {
                    if(node.Text.Contains(text))
                    {
                        nodeList.Clear();
                        parentNode = node;
                        nodeList.Add(node);

                        while(parentNode.Parent != null)
                        {
                            parentNode = parentNode.Parent;
                            nodeList.Add(parentNode);
                        }

                        currentNodes = treeView1.Nodes;
                        for(int i = nodeList.Count - 1; i >= 0; i--)
                        {
                            tempNode = nodeList[i];
                            bool find = false;

                            foreach(TreeNode currentNode in currentNodes)
                            {
                                if(currentNode.Text == tempNode.Text)
                                {
                                    currentNodes = currentNode.Nodes;
                                    find = true;
                                    break;
                                }
                            }

                            if(!find)
                            {
                                TreeNode newNode = new TreeNode(tempNode.Text);
                                currentNodes.Add(newNode);
                                currentNodes = newNode.Nodes;
                            }
                        }

                        count++;
                    }
                }

                this.Text = string.Format("关键字过滤 （共找到{0}条记录）", count.ToString());
            }   

            treeView1.ExpandAll();
        }

        /// <summary>
        /// 选择树结点
        /// </summary>
        /// <return>是否选择成功</return>
        private bool SelectTreeNode()
        {
            bool result = false;
            TreeNode selectedNode = treeView1.SelectedNode;

            if(selectedNode != null)
            {
                bool find = true;
                TreeNode parentNode1;
                TreeNode parentNode2;

                foreach(TreeNode node in baseNodeList)
                {
                    find = true;

                    if(node.Text == selectedNode.Text)
                    {
                        parentNode1 = node.Parent;
                        parentNode2 = selectedNode.Parent;

                        while(parentNode1 != null && parentNode2 != null)
                        {
                            if(parentNode1.Text != parentNode2.Text)
                            {
                                find = false;
                                break;
                            }

                            parentNode1 = parentNode1.Parent;
                            parentNode2 = parentNode2.Parent;
                        }

                        if(find)
                        {
                            baseTree.SelectedNode = node;
                            result = true;
                            break;
                        }
                    }
                }
            }
            else
            {
                MessageBox.Show("未选择树结点!", "关键字过滤", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return result;
        }

        /// <summary>
        /// 用户选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            bool result = SelectTreeNode();
            if(result)
            {
                DialogResult = DialogResult.OK;
                this.Close();
            }
            else
            {
                MessageBox.Show("无效的选择!", "关键字过滤", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 用户选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 关键字改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_TextChanged(object sender, EventArgs e)
        {
            FiltrateTreeNode(textBoxX1.Text);
        }

        /// <summary>
        /// 鼠标点击过滤框
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void textBoxX1_MouseClick(object sender, MouseEventArgs e)
        {
            if(textBoxX1.Text == "输入关键字...")
            {
                textBoxX1.Text = "";
            }
        }
    }
}