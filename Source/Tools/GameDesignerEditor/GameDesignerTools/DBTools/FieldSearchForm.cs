using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public partial class FieldSearchForm : Form
    {
        TreeView treeView = null;
        TreeNode modelNode = null;
        TreeNode findNode = null;
        bool bLabel = false;
        TreeNode lastFindNode = null;

        public FieldSearchForm(TreeView treeView)
        {
            this.treeView = treeView;
            this.modelNode = treeView.SelectedNode;
            InitializeComponent();
        }

        /// <summary>
        /// 查找匹配的树结点
        /// </summary>
        /// <param name="currentNode">当前树结点</param>
        /// <param name="searchString">条件字符串</param>
        private void SearchFiledNode(TreeNode currentNode, string searchString)
        {
            if (lastFindNode == null)
            {
                bLabel = true;
            }

            if (currentNode == lastFindNode)
            {
                bLabel = true;                
            }
            else
            {
                if (currentNode.Text.ToLower().Contains(searchString) && bLabel)
                {
                    findNode = currentNode;
                    lastFindNode = currentNode;
                }
                else
                {
                    foreach (TreeNode childNode in currentNode.Nodes)
                    {
                        SearchFiledNode(childNode, searchString);  
                      
                        if (findNode != null)
                        {
                            break;
                        }
                    }
                }
            }            
        }

        /// <summary>
        /// 查找下一个
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            string searchString = textBoxX1.Text.ToLower();

            if (searchString != "")
            {
                findNode = null;
                bLabel = false;

                foreach (TreeNode tabNode in modelNode.Nodes)
                {
                    foreach (TreeNode fieldNode in tabNode.Nodes)
                    {
                        SearchFiledNode(fieldNode, searchString);

                        if (findNode != null)
                        {
                            break;
                        }
                    }

                    if (findNode != null)
                    {
                        break;
                    }
                }

                if (findNode != null)
                {
                    treeView.SelectedNode = findNode;
                }
                else                
                {
                    MessageBox.Show("没有找到匹配的树结点，或者当前已搜索到最后一个匹配的树结点！", "查找字段", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    lastFindNode = null;
                }
            }
            else
            {
                MessageBox.Show("请先输入查找条件！", "查找字段", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}