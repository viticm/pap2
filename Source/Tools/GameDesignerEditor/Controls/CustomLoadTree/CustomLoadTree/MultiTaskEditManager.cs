using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Drawing;
using System.Collections;

namespace CustomLoadTree
{
    class MultiTaskEditManager
    {
        private SqlConnection conn; // sql连接
        private TreeNode node; // 当前树结点
        private DataTable taskTable; // 任务数据表
        private List<string> displayFieldList; // 显示的字段链表
        private Hashtable idTable = new Hashtable(); // 树结点文字索引id的哈希表
        private Hashtable iconIndexTable = new Hashtable();


        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="node">当前树结点</param>
        /// <param name="displayFieldList">显示的字段链表</param>
        public MultiTaskEditManager(SqlConnection conn, TreeNode node, List<string> displayFieldList)
        {
            this.conn = conn;
            this.node = node;
            this.displayFieldList = displayFieldList;
            iconIndexTable.Add(0, "前置任务");
            iconIndexTable.Add(1, "脚本任务");
            iconIndexTable.Add(2, "采集任务");
            iconIndexTable.Add(3, "收集任务");
            iconIndexTable.Add(4, "杀怪任务");
            iconIndexTable.Add(5, "送道具任务");
            iconIndexTable.Add(6, "对话任务");
            iconIndexTable.Add(7, "计时任务");
        }

        /// <summary>
        /// 进行选择操作
        /// </summary>
        public void Operate()
        {
            // 获取整个任务表的数据
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "tbl_quests");
                taskTable = Helper.GetDataTable(sqlString, conn);
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在读取任务信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            // 显示任务选择窗体
            List<string> taskStringList = new List<string>(); // 任务字符串链表
            List<string> taskIDList = new List<string>(); // 选中的任务id链表
            string taskID; // 任务的id

            FillTaskItems(node, taskStringList);
            MultiSelectForm mForm = new MultiSelectForm("请选择需要编辑的任务");
            mForm.ItemList = taskStringList;
            mForm.ShowItems();
            DialogResult result = mForm.ShowDialog();
            if(result != DialogResult.OK) // 用户没有确认选择，直接返回
            {
                return;
            }
            if(mForm.ItemList.Count == 0) // 用户没有选择任何任务，直接返回
            {
                MessageBox.Show("你没有选择需要编辑的任务!");
                return;
            }

            foreach(string s in mForm.ItemList)
            {
                taskID = GetTaskID(s);
                if(taskID != null)
                {
                    taskIDList.Add(taskID);
                }
            }

            // 显示字段选择窗体
            List<string> fieldList = new List<string>(); // 选中的字段链表
            foreach(DataColumn c in taskTable.Columns)
            {
                fieldList.Add(c.ColumnName);
            }
            mForm = new MultiSelectForm("请选择需要编辑的字段");
            mForm.ItemList = fieldList;
            mForm.SelectItemList = displayFieldList;
            mForm.ShowItems();
            result = mForm.ShowDialog();
            if (result != DialogResult.OK) // 用户没有确认选择，直接返回
            {
                return;
            }
            if (mForm.ItemList.Count == 0) // 用户没有选择任何任务，直接返回
            {
                MessageBox.Show("你没有选择需要编辑的字段!");
                return;
            }

            MultiTaskEditForm eForm = new MultiTaskEditForm(conn, taskTable, taskIDList, fieldList);
            eForm.ShowDialog();
        }

        /// <summary>
        /// 获取任务的id
        /// </summary>
        /// <param name="taskString">任务的混合字符串</param>
        /// <returns>任务的id</returns>
        private string GetTaskID(string taskString)
        {
            string id = idTable[taskString] as string;

            /*int firstIndex = taskString.IndexOf('[');
            int lastIndex = taskString.IndexOf(']');

            if((firstIndex >= 0) && (lastIndex >= 0))
            {
                id = taskString.Substring(firstIndex + 1, lastIndex - firstIndex - 1);
            }*/

            return id;
        }

        /// <summary>
        /// 填充任务列表内容
        /// </summary>
        /// <param name="currentNode">当前的树结点</param>
        /// <param name="list">选项内容链表</param>
        private void FillTaskItems(TreeNode currentNode, List<string> list)
        {
            object[] keys = currentNode.Tag as object[];
            if (keys != null)
            {
                int id = int.Parse(keys[0].ToString());
                if((id > 0) && (currentNode.ForeColor != Color.Olive)) // 过滤掉完成其一，全部完成和镜像结点
                {
                    string strNodeFullName = string.Format("[{0}]\t{1}",
                        iconIndexTable[currentNode.ImageIndex].ToString(), currentNode.Text);
                    list.Add(strNodeFullName);
                    idTable[strNodeFullName] = id.ToString(); // 保存id
                }            
            }

            foreach(TreeNode childNode in currentNode.Nodes)
            {
                FillTaskItems(childNode, list);
            }
        }
    }
}
