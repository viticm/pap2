using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeCopyCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeCopyCommand(object data, string description):
            base(data, description)
        {

        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前命令</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = false;
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null)
            {
                description = "复制树结点 " + currentNode.Text;              
                documentManager.CopyTreeNode = currentNode;
                documentManager.CurrentTreeMode = EditMode.Copy;               
                success = true;
            }

            return success;
        }
    }
}
