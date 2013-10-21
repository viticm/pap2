using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeRenameCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeRenameCommand(object data, string description)
            : base(data, description)
        {
            
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <return>是否执行成功</return>
        public override bool Execute(object o)
        {
            bool success = false;
            TreeNode currentNode = tree.SelectedNode;

            if (currentNode != null)
            {
                // 显示输入管理器
                InputForm iForm = new InputForm("请输入重命名结点的名称", currentNode.Text);
                DialogResult result = iForm.ShowDialog();

                if (result == DialogResult.OK)
                {
                    if (CheckPathValid(currentNode, iForm.InputText)) // 当前无重复路径
                    {
                        description = "重命名树结点 " + iForm.InputText;
                        currentNode.Text = iForm.InputText;
                        currentNode.Name = iForm.InputText;
                        documentManager.CurrentTreeMode = EditMode.Normal;
                        
                        // 对数据库进行操作
                        DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();                                             

                        // 批量更新绘图路径
                        List<TreeNode> nodeList = GetChildNodeList(currentNode);
                        foreach (TreeNode node in nodeList)
                        {
                            string newPath = documentManager.GetNodePath(node);
                            dataBaseManager.UpdateDiagramPath(node.Tag.ToString(), newPath);
                        }               

                        success = true;
                    }
                    else
                    {
                        MessageBox.Show("该路径已经存在！", "路径有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }                                      
                }
            }

            return success;
        }
    }
}
