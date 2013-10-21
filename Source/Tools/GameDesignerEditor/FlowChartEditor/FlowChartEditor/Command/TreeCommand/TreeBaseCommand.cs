using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;
using FlowChartEditor.Command;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeBaseCommand:Command
    {
        protected DocumentManager documentManager; // 分类管理器
        protected TreeView tree; // 分类树

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeBaseCommand(object data, string description):base(data, description)
        {
            documentManager = DocumentManager.GetDocumentManager();
            tree = data as TreeView;
        }

        /// <summary>
        /// 检查当前路径是否有效
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <param name="text">当前文本</param>
        /// <returns>当前路径是否有效</returns>
        protected bool CheckPathValid(TreeNode node, string text)
        {
            bool valid = true;

            if (node != null)
            {
                foreach (TreeNode childNode in node.Nodes)
                {
                    if (childNode.Text == text)
                    {
                        valid = false;
                        break;
                    }
                }
            }
            else // 在根结点上检查
            {
                TreeView tree = data as TreeView;
                foreach(TreeNode childNode in tree.Nodes)
                {
                    if (childNode.Text == text)
                    {
                        valid = false;
                        break;
                    }
                }
            }

            return valid;
        }

        /// <summary>
        /// 获取子结点链表
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <returns>子结点链表</returns>
        protected List<TreeNode> GetChildNodeList(TreeNode node)
        {
            List<TreeNode> nodeList = new List<TreeNode>();

            FillChildNodeInList(nodeList, node);

            return nodeList;
        }

        /// <summary>
        /// 将子结点填充入链表中
        /// </summary>
        /// <param name="nodeList">链表</param>
        /// <param name="node">当前树结点</param>
        protected void FillChildNodeInList(List<TreeNode> nodeList, TreeNode node)
        {
            nodeList.Add(node);

            foreach(TreeNode treeNode in node.Nodes)
            {
                FillChildNodeInList(nodeList, treeNode);
            }
        }

        /// <summary>
        /// 在命令执行前保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void  SaveBeforeExecute(object o)
        {
            dataBeforeExecute.SaveData(o);
        }

        /// <summary>
        /// 在命令执行后保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected override void  SaveAfterExecute(object o)
        {
            dataAfterExecute.SaveData(o);
        }        

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <return>是否执行成功</return>
        public override bool Execute(object o)
        {
            return false;
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        public override void Execute()
        {
            object o = dataAfterExecute.LoadData();            
        }

        /// <summary>
        /// 取消执行命令
        /// </summary>
        /// <returns>当前对象</returns>
        public override void Unexecute()
        {
            object o = dataBeforeExecute.LoadData();            
        }
    }
}
