using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree
{
    /// <summary>
    /// 结点信息处理类
    /// </summary>
    class NodeData
    {
        private Hashtable table = new Hashtable(); // 存贮结点与其信息的hashtable
        private List<List<string>> list = new List<List<string>>(); // 存贮结点信息的链表

        /// <summary>
        /// 结点数量属性
        /// </summary>
        public int NodeCount
        {
            get
            {
                return table.Count;
            }
        }

        /// <summary>
        /// 添加新数据
        /// </summary>
        /// <param name="node">添加的新结点</param>
        /// <param name="list">新结点的信息</param>
        public void AddData(TreeNode node, List<string> list)
        {
            try
            {
                table.Add(node, list);
            }
            catch (Exception ex)
            {
                MessageBox.Show("将结点信息添加入HashTable时出现异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取id索引任务等级的哈希表
        /// </summary>
        /// <returns>id索引任务等级的哈希表</returns>
        public Hashtable GetLevelTable()
        {
            Hashtable levelTable = new Hashtable();

            foreach(object o in table.Keys)
            {
                TreeNode node = o as TreeNode;
                List<string> data = table[o] as List<string>;
                if(data != null)
                {
                    levelTable[data[0]] = data[2];
                }
            }

            return levelTable;
        }

        /// <summary>
        /// 获取id索引任务名的哈希表
        /// </summary>
        /// <returns>id索引任务名的哈希表</returns>
        public Hashtable GetNameTable()
        {
            Hashtable nameTable = new Hashtable();

            foreach(object o in table.Keys)
            {
                TreeNode node = o as TreeNode;
                List<string> data = table[o] as List<string>;
                if(data != null)
                {
                    nameTable[data[0]] = data[1];
                }
            }

            return nameTable;
        }

        /// <summary>
        /// 获取数据
        /// </summary>
        /// <param name="node">要获取信息的结点</param>
        /// <returns>结点信息哈希表</returns>
        public Hashtable GetData(TreeNode node)
        {
            return table;
        }

        /// <summary>
        /// 获取结点的ID
        /// </summary>
        /// <param name="node">要获取ID的结点</param>
        /// <returns>结点的ID</returns>
        public string GetID(TreeNode node)
        {
            List<string> data = table[node] as List<string>;
            if (data == null)
            {
                return null;
            }
            else
            {
                return data[0];
            }
        }

        /// <summary>
        /// 获取结点的名称
        /// </summary>
        /// <param name="node">要获取名称的结点</param>
        /// <returns>结点的名称</returns>
        public string GetName(TreeNode node)
        {
            List<string> data = table[node] as List<string>;
            if (data == null)
            {
                return null;
            }
            else
            {
                return data[1];
            }
        }

        /// <summary>
        /// 获取结点的Level
        /// </summary>
        /// <param name="node">要获取Level的结点</param>
        /// <returns>结点的Level</returns>
        public string GetLevel(TreeNode node)
        {
            List<string> data = table[node] as List<string>;
            if (data == null)
            {
                return null;
            }
            else
            {
                return data[2];
            }
        }

        /// <summary>
        /// 清空数据
        /// </summary>
        public void ClearData()
        {
            table.Clear();
            list.Clear();
        }
    }
}
