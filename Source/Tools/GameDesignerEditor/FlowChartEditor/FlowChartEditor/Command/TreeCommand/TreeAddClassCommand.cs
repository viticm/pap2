using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeAddClassCommand:TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令描述</param>
        public TreeAddClassCommand(object data, string description):
            base(data, description)
        {

        }        

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">命令参数</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = false;
            List<string> mapList = o as List<string>;
            TreeNode currentNode = tree.SelectedNode;
            TreeNode newNode;
            DialogResult result;
            string inputText;

            if (currentNode.Level > 0) // 在当前选中树结点下新建分类
            {
                InputForm iForm = new InputForm("请输入新建分类的名称", "新建分类");
                result = iForm.ShowDialog();
                inputText = iForm.InputText;
            }
            else // 在根结点下新建分类
            {
                ListForm lForm = new ListForm("请选择新建分类的地图名", mapList);
                result = lForm.ShowDialog();
                inputText = lForm.InputText;
            }

            if (result == DialogResult.OK)
            {
                if (CheckPathValid(currentNode, inputText)) // 当前无重复路径
                {
                    // 保存命令执行前的状态
                    description = ("新建分类 " + inputText);                    

                    if (currentNode != null) 
                    {
                        newNode = currentNode.Nodes.Add(inputText);
                    }
                    else 
                    {
                        newNode = tree.Nodes.Add(inputText);
                    }

                    newNode.Name = inputText;
                    newNode.ImageIndex = 0;
                    newNode.SelectedImageIndex = 0;
                    tree.SelectedNode = newNode;
                    documentManager.CurrentTreeMode = EditMode.Normal;

                    // 对数据库进行操作
                    DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
                    string path = documentManager.GetNodePath(newNode);
                    string id = dataBaseManager.CreateNewDiagram(path, true, documentManager.CurrentChartMode);
                    newNode.Tag = id;

                    // 保存命令执行后的状态                    
                    success = true;
                }
                else
                {
                    MessageBox.Show("该路径已经存在！", "路径有效性检查", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }

            return success;
        }
    }
}
