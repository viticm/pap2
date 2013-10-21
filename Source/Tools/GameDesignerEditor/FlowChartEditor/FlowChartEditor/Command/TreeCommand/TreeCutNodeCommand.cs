using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeCutNodeCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeCutNodeCommand(object data, string description):
            base(data, description)
        {
            
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = false;
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null)
            {                
                TreeNode parentNode = currentNode.Parent;

                if (parentNode != null)
                {
                    documentManager.CutTreeNode = currentNode;
                    documentManager.CutTreeNodePath = documentManager.GetNodePath(currentNode);
                    description = "剪切树结点 " + currentNode.Text;
                    parentNode.Nodes.Remove(currentNode);
                    documentManager.CurrentTreeMode = EditMode.Cut;
                    success = true;
                }
            }

            return success;
        }
    }
}
