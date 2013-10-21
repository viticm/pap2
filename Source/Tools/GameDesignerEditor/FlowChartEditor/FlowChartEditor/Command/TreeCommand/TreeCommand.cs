using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;

namespace FlowChartEditor
{
    class TreeCommand:Command
    {
        protected DocumentManager documentManager; // 分类管理器

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public TreeCommand(object data, string description):base(data, description)
        {
            dataBeforeExecute = new SerialMemento();
            dataAfterExecute = new SerialMemento();
        }

        /// <summary>
        /// 当前分类管理器属性
        /// </summary>
        public DocumentManager CurrentDocumentManager
        {
            set
            {
                documentManager = value;
            }
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
        /// 获取树结点的路径
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <returns>树结点的路径</returns>
        protected string GetNodePath(TreeNode node)
        {
            string path = node.Text;
            TreeNode parentNode = node.Parent;
            while (parentNode != null)
            {
                path = parentNode.Text + "\\" + path;
                parentNode = parentNode.Parent;
            }

            return path;
        }

        /// <summary>
        /// 构造分类树
        /// </summary>
        /// <param name="tree">当前分类树</param>
        /// <param name="table">记录分类树信息的数据表</param>
        protected void ConstructTree(TreeView tree, DataTable table)
        {
            string fullPath;
            string id;
            bool visible;
            bool focused;
            bool copy;
            bool cut;
            bool classifyNode;
            string[] data;
            TreeNode selectedNode = null;
            List<TreeNode> nodeList = new List<TreeNode>();
            string classification;
            TreeNodeCollection currentNodes;
            TreeNode newNode = null;

            tree.Nodes.Clear(); // 清空树结点
            documentManager.CurrentTreeMode = EditMode.Normal; // 重设树的状态

            foreach (DataRow row in table.Rows)
            {
                fullPath = row["FullPath"].ToString();
                id = row["ID"].ToString();
                visible = (bool)row["Visible"];
                focused = (bool)row["Focused"];
                copy = (bool)row["CopyNode"];
                cut = (bool)row["CutNode"];
                classifyNode = (bool)row["ClassifyNode"];
                data = fullPath.Split(new char[] { '\\' });
                currentNodes = tree.Nodes;

                for (int i = 0; i < data.Length; i++)
                {
                    classification = data[i];

                    if ((i == data.Length - 1) && !classifyNode) // 流程图结点
                    {
                        newNode = currentNodes.Add(classification);
                        newNode.Tag = id;
                        newNode.ImageIndex = 1;
                        newNode.SelectedImageIndex = 1;
                    }
                    else // 流程图分类
                    {
                        bool find = false;

                        foreach (TreeNode node in currentNodes)
                        {
                            if (node.Text == classification)
                            {
                                find = true;
                                currentNodes = node.Nodes;
                                break;
                            }
                        }

                        if (!find) // 当前分类不存在
                        {
                            newNode = currentNodes.Add(classification);
                            newNode.Tag = id;
                            newNode.ImageIndex = 0;
                            newNode.SelectedImageIndex = 0;
                            currentNodes = newNode.Nodes;
                        }
                    }
                }

                if (visible) // 当前结点可见
                {
                    nodeList.Add(newNode);
                }

                if (focused) // 当前结点被选中
                {
                    selectedNode = newNode;
                }

                if (copy) // 当前结点被复制
                {
                    documentManager.CopyTreeNode = newNode;
                    documentManager.CurrentTreeMode = EditMode.Copy;
                }

                if (cut) // 当前结点被剪切
                {
                    documentManager.CutTreeNode = newNode;
                    documentManager.CurrentTreeMode = EditMode.Cut;
                }
            }

            // 恢复树的伸展状态
            foreach (TreeNode node in nodeList)
            {
                node.EnsureVisible();
            }

            // 恢复树的焦点
            tree.SelectedNode = selectedNode;
        }

        /// <summary>
        /// 保存分类树
        /// </summary>
        /// <param name="tree">当前分类树</param>
        /// <returns>记录分类树信息的数据表</returns>
        protected DataTable SaveTree(TreeView tree)
        {
            DataTable table = new DataTable("");
            Type stringType = Type.GetType("System.String");
            Type boolType = Type.GetType("System.Boolean");

            table.Columns.Add("ID", stringType);
            table.Columns.Add("FullPath", stringType);
            table.Columns.Add("Visible", boolType);
            table.Columns.Add("Focused", boolType);
            table.Columns.Add("CopyNode", boolType);
            table.Columns.Add("CutNode", boolType);
            table.Columns.Add("ClassifyNode", boolType);
            List<TreeNode> flowChartNodeList = new List<TreeNode>();

            // 遍历找出所有的流程图结点
            foreach (TreeNode node in tree.Nodes)
            {
                SeekLeafNode(node, flowChartNodeList);
            }

            // 构造数据表
            string id;
            string fullPath;
            TreeNode parentNode;
            List<string> pathList = new List<string>();
            DataRow newRow;

            foreach (TreeNode node in flowChartNodeList)
            {
                id = node.Tag.ToString();
                parentNode = node.Parent;
                pathList.Clear();
                pathList.Add(node.Text);

                while (parentNode != null)
                {
                    pathList.Add(parentNode.Text);
                    parentNode = parentNode.Parent;
                }

                fullPath = "";
                for (int i = pathList.Count - 1; i >= 0; i--)
                {
                    fullPath += pathList[i] + "\\";
                }
                fullPath = fullPath.TrimEnd(new char[] { '\\' });

                newRow = table.NewRow();
                newRow["FullPath"] = fullPath;
                newRow["ID"] = id;
                newRow["Visible"] = node.IsVisible;
                newRow["Focused"] = node.IsSelected;
                newRow["CopyNode"] = ((documentManager.CurrentTreeMode == EditMode.Copy) && 
                    (node == documentManager.CopyTreeNode));
                newRow["CutNode"] = ((documentManager.CurrentTreeMode == EditMode.Cut) && 
                    (node == documentManager.CutTreeNode));
                newRow["ClassifyNode"] = (node.ImageIndex == 0);
                table.Rows.Add(newRow);
            }

            return table;
        }

        /// <summary>
        /// 检索叶子结点
        /// </summary>
        /// <param name="node">当前树结点</param>
        /// <param name="list">保存叶子结点的链表</param>
        private void SeekLeafNode(TreeNode node, List<TreeNode> list)
        {
            if (node.Nodes.Count == 0) // 当前树结点是叶子结点
            {
                list.Add(node);
            }
            else // 当前树结点是分类结点
            {
                foreach (TreeNode classifyNode in node.Nodes)
                {
                    SeekLeafNode(classifyNode, list);
                }
            }
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
            TreeView tree = data as TreeView;
            ConstructTree(tree, o as DataTable);
        }

        /// <summary>
        /// 取消执行命令
        /// </summary>
        /// <returns>当前对象</returns>
        public override void Unexecute()
        {
            object o = dataBeforeExecute.LoadData();
            TreeView tree = data as TreeView;
            ConstructTree(tree, o as DataTable);
        }
    }
}
