using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Collections;
using System.Windows.Forms;

namespace GameLuaEditor
{
    public interface ITreeOpera
    {
        TreeNodeCollection LoadTreeNodeCollection(Hashtable ht_data);
    }
    
    /// <summary>
    /// 脚本分类树
    /// </summary>
    public class TreePhOpera : ITreeOpera
    {
        public TreeNodeCollection LoadTreeNodeCollection(Hashtable treeInfo)
        {
            DataTable nodeInfo = treeInfo["dtable"] as DataTable;
            TreeNode rootNode = new TreeNode();
            TreeNodeCollection treeNodeCollection = rootNode.Nodes;

            foreach (DataRow dataRow in nodeInfo.Rows)
            {
                string path = dataRow["path"].ToString();
                string id = dataRow["id"].ToString();                
                string[] as_path = path.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
                
                TreeNodeCollection nodeCollection = treeNodeCollection;

                for (int i = 0; i < as_path.Length ; i++)
                {
                    string strText = as_path[i];

                    if (i == 0)
                    {
                        nodeCollection = treeNodeCollection;
                    }

                    if (!nodeCollection.ContainsKey(strText))
                    {
                        Hashtable infoTable = new Hashtable();

                        if (i != as_path.Length - 1)
                        {
                            // 添加目录节点                            
                            infoTable["type"] = "folder";
                            TreeNode newNode = CreateNode(strText, "folder", treeInfo["pop_folder"] as ContextMenuStrip,
                                                          infoTable);
                            nodeCollection.Add(newNode);

                            nodeCollection = newNode.Nodes;
                        }
                        else
                        {
                            // 添加文件节点
                            infoTable["type"] = "file";
                            infoTable["scriptType"] = treeInfo["scriptType"];                            
                            infoTable["id"] = id;
                            infoTable["filePath"] = path;
                            TreeNode newNode = CreateNode(strText, "file", treeInfo["pop_file"] as ContextMenuStrip,
                                                          infoTable);

                            nodeCollection.Add(newNode);                            
                        }
                    }
                    else
                    {
                        if (nodeCollection[strText].Text != strText && nodeCollection[strText].Text.ToLower() == strText.ToLower())
                        {
                            MessageBox.Show(path + " 路径有错误。");
                        }

                        nodeCollection = nodeCollection[strText].Nodes;
                    }
                }
            }            

            return treeNodeCollection;
        }

        /// <summary>
        /// 创建树结点
        /// </summary>
        /// <param name="nodeText">树结点文本</param>
        /// <param name="imageKey">树结点图标键值</param>
        /// <param name="menuStrip">树结点右键菜单</param>
        /// <param name="infoTable">树结点信息</param>
        /// <returns>树结点</returns>
        public static TreeNode CreateNode(string nodeText, string imageKey, ContextMenuStrip menuStrip, Hashtable infoTable)
        {
            TreeNode newNode = new TreeNode(nodeText);
            newNode.Name = nodeText;
            newNode.ImageKey = imageKey;
            newNode.SelectedImageKey = imageKey;
            newNode.ContextMenuStrip = menuStrip;
            newNode.Tag = infoTable;

            return newNode;
        }

        //设置某个节点的文本颜色（通过ID寻找节点）
        public static void  SetNodeColorByID(TreeView tv, string strID, System.Drawing.Color color)
        {
            _setNodeColorByID(tv.Nodes, strID, color);
        }

        private static void _setNodeColorByID(TreeNodeCollection tnc, string strID, System.Drawing.Color color)
        {
            foreach(TreeNode tn in  tnc)
            {
                _setNodeColorByID(tn.Nodes, strID, color);
                Hashtable ht = tn.Tag as Hashtable;
                if(ht == null) continue;
                string type = ht["type"] as string;
                if (type != "file") continue;
                string id = ht["id"] as string;
                if(id == strID)             //中标
                {
                    tn.ForeColor = color;
                    return;
                }
            }
        }
    };

    /// <summary>
    /// 搜索替换树
    /// </summary>
    public class TreeReplaceOpera : ITreeOpera
    {
         public TreeNodeCollection LoadTreeNodeCollection(Hashtable ht_data)
         {
             TreeNodeCollection treeNodeCollection = null;

             if (ht_data["filelist"] != null)
             {
                 Hashtable ht_filelist = ht_data["filelist"] as Hashtable;
                 TreeNode _tn = new TreeNode();
                 treeNodeCollection = _tn.Nodes;
                 string strFind = ht_data["find"].ToString();
                 string strReplace = ht_data["replace"].ToString();                 

                 foreach (string strKey in ht_filelist.Keys)
                 {
                     Hashtable ht_file = ht_filelist[strKey] as Hashtable;
                     string fileName = ht_file["filename"] as string;
                     string content = ht_file["filecontent"] as string;
                     
                     if (content.IndexOf(strFind) != -1)
                     {                         
                         // 添加文件节点
                         TreeNode tn =  treeNodeCollection.Add(strKey,  fileName);
                         Hashtable ht_node1 = new Hashtable();
                         ht_node1["type"] = "file";
                         ht_node1["strFileContent"] = content;
                         ht_node1["id"] = strKey;
                         ht_node1["line_last"] = "";
                         tn.Tag = ht_node1;
                         tn.Checked = true;

                         // 添加行、列子节点
                         int tmpLineNumber = 1;
                         string[] asp = { strFind };
                         string[] as_lines = content.Split(asp, StringSplitOptions.None);

                         for(int i = 0; i < as_lines.Length - 1; i++)
                         {
                             string[] asp1 = { "\n" };
                             tmpLineNumber += as_lines[i].Split(asp1, StringSplitOptions.None).Length - 1;  //计算出行号

                             // 添加子结点
                             TreeNode tn_line = tn.Nodes.Add(string.Format("第{0}行", tmpLineNumber));
                             Hashtable ht_node2 = new Hashtable();
                             ht_node2["type"] = "line";
                             ht_node2["lineNumber"] = tmpLineNumber;
                             ht_node2["line_before"] = as_lines[i];
                             tn_line.Tag = ht_node2;
                             tn_line.Checked = true;
                         }

                         ht_node1["line_last"] = as_lines[as_lines.Length - 1];
                         tn.Expand();
                     }
                 }
             }
             
             return treeNodeCollection;
         }
    };
}
