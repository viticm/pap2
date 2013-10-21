using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeDeleteNodeCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeDeleteNodeCommand(object data, string description):
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
                DialogResult result = MessageBox.Show("确认删除该树结点及其所有的子结点吗？", "删除确认",
                                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    // 保存命令执行前的状态
                    description = "删除树结点 " + currentNode.Text;
                                      
                    // 对数据库进行操作
                    DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                    if(currentNode.ImageIndex == 0) // 删除分类
                    {
                        dataBaseManager.DeleteMultiDiagram(documentManager.GetNodePath(currentNode));
                    }
                    else // 删除流程图
                    {
                        dataBaseManager.DeleteDiagram(currentNode.Tag.ToString());
                    }

                    currentNode.Remove();
                    documentManager.CurrentTreeMode = EditMode.Normal; 

                    success = true;
                }
            }

            return success;
        }
    }
}
