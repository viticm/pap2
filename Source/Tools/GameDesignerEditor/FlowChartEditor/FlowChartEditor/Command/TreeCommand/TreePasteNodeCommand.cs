using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreePasteNodeCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreePasteNodeCommand(object data, string description):
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
            TreeNode nodeToAdd;
            TreeNode nodeToClone = null;

            if (currentNode != null)
            {
                if (currentNode.ImageIndex == 0) // 当前结点是分类结点
                {
                    nodeToAdd = currentNode;
                }
                else // 当前结点是流程图结点
                {
                    TreeNode parentNode = currentNode.Parent;
                    nodeToAdd = parentNode;
                }

                switch (documentManager.CurrentTreeMode)
                {
                    case EditMode.Cut:
                        {                            
                            nodeToClone = documentManager.CutTreeNode;                            

                            break;
                        }
                    case EditMode.Copy:
                        {                            
                            nodeToClone = documentManager.CopyTreeNode;                            

                            break;
                        }
                }

                if (nodeToClone != null)
                {
                    string nodeText = nodeToClone.Text;
                    bool nodeExist = false;

                    foreach(TreeNode node in nodeToAdd.Nodes)
                    {
                        if(node.Text == nodeText)
                        {
                            nodeExist = true;
                            break;
                        }
                    }

                    if (nodeExist) // 相同名字的
                    {
                        nodeText += " 复件";
                    }

                    // 保存命令执行前的状态
                    description = "粘贴树结点 " + nodeText;

                    TreeNode newNode = nodeToAdd.Nodes.Add(nodeText);
                    newNode.Name = nodeText;
                    newNode.Tag = nodeToClone.Tag;
                    newNode.ImageIndex = nodeToClone.ImageIndex;
                    newNode.SelectedImageIndex = nodeToClone.SelectedImageIndex;
                    tree.SelectedNode = newNode;

                    // 对数据库进行操作
                    DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                    string path = documentManager.GetNodePath(newNode);

                    if (documentManager.CurrentTreeMode == EditMode.Cut) // 剪切树结点
                    {
                        dataBaseManager.UpdateDiagramPath(newNode.Tag.ToString(), path);
                    }
                    else // 粘贴树结点
                    {
                        string id = nodeToClone.Tag.ToString();
                        string chartClassString = documentManager.CurrentChartMode;
                        object flowChartData = dataBaseManager.GetDiagramData(id);
                        id = dataBaseManager.CreateNewDiagram(path, false, chartClassString);
                        newNode.Tag = id;

                        if (flowChartData != null) // 原图为空的时候就不用再存了
                        {
                            dataBaseManager.SaveDiagramData(id, flowChartData);
                        }                        
                    }                    

                    success = true;
                }
            }  

            return success;
        }
    }
}
