using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Collections;
using System.Data;
using System.Windows.Forms;

namespace MasterIO
{
    class OperateTree
    {
        private SqlConnection conn; // sql连接

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="conn">sql连接</param>
        public OperateTree(SqlConnection conn)
        {
            this.conn = conn;
        }

        /// <summary>
        /// 进行粘贴
        /// </summary>
        public void Paste(TreeNode sourceNode, TreeNode destinateNode)
        {
            if (sourceNode.Level == destinateNode.Level) // 检查是否是同级结点
            {
                switch (destinateNode.Level)
                {
                    case 0: // 粘贴文件夹
                        {
                            PasteFolder(sourceNode, destinateNode);
                            break;
                        }
                    case 1: // 粘贴文件
                        {
                            PasteFile(sourceNode, destinateNode);
                            break;
                        }
                    case 2: // 粘贴道具
                        {
                            PasteItem(sourceNode, destinateNode.Parent);
                            break;
                        }
                }
            }
            else
            {
                MessageBox.Show("非同级结点不允许复制粘贴!");
            }
        }

        /// <summary>
        /// 粘贴学习师文件夹
        /// </summary>
        /// <param name="sourceNode">源树结点</param>
        /// <param name="destinateNode">目标树结点</param>
        private void PasteFolder(TreeNode sourceNode, TreeNode destinateNode)
        {
            TreeNode newNode;

            if (sourceNode.Nodes.Count > 0)
            {
                foreach (TreeNode fileNode in sourceNode.Nodes)
                {
                    if (fileNode.Nodes.Count > 0)
                    {
                        newNode = destinateNode.Nodes.Add(fileNode.Text);
                        newNode.Tag = fileNode.Tag;
                        PasteFile(fileNode, newNode);
                    }
                    else
                    {
                        MessageBox.Show(string.Format("无意义的粘贴操作: {0}文件下面没有任何内容!", fileNode.Text));
                    }
                }
            }
            else
            {
                MessageBox.Show(string.Format("无意义的粘贴操作: {0}文件夹下面没有任何内容!", sourceNode.Text));
            }
        }

        /// <summary>
        /// 粘贴学习师文件
        /// </summary>
        /// <param name="sourceNode">源树结点</param>
        /// <param name="destinationNode">目标树结点</param>
        private void PasteFile(TreeNode sourceNode, TreeNode destinateNode)
        {
            if (sourceNode.Nodes.Count > 0)
            {
                foreach (TreeNode childNode in sourceNode.Nodes)
                {
                    PasteItem(childNode, destinateNode);
                }
            }
            else
            {
                MessageBox.Show(string.Format("无意义的粘贴操作: {0}文件下面没有任何内容!", sourceNode.Text));
            }
        }

        /// <summary>
        /// 粘贴学习师道具
        /// </summary>
        /// <param name="sourceNode">源树结点</param>
        /// <param name="destinateNode">目标树结点</param>
        private void PasteItem(TreeNode sourceNode, TreeNode destinateNode)
        {
            TreeNode newNode;
            string folderName = destinateNode.Parent.Text;
            string fileName = destinateNode.Text;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "tbl_master_skills");
                SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
                DataSet ds = new DataSet();
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                adp.Fill(ds);
                DataTable shopTable = ds.Tables[0];

                string id = (sourceNode.Tag as object[])[0].ToString();
                DataRow row = shopTable.Rows.Find(id);

                if (row != null)
                {
                    DataRow newRow = shopTable.NewRow();
                    newRow.BeginEdit();
                    foreach (DataColumn c in shopTable.Columns)
                    {
                        switch (c.ColumnName)
                        {
                            case "id":
                                {
                                    break;
                                }
                            case "filename":
                                {
                                    newRow[c] = fileName;
                                    break;
                                }
                            case "foldername":
                                {
                                    newRow[c] = folderName;
                                    break;
                                }
                            case "MasterID": // 先设为0，导出时自动匹配更新
                                {
                                    newRow[c] = 0;
                                    break;
                                }
                            default:
                                {
                                    newRow[c] = row[c];
                                    break;
                                }
                        }
                    }
                    newRow.EndEdit();

                    shopTable.Rows.Add(newRow);
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    adp.Update(shopTable);
                    shopTable.AcceptChanges();

                    string newID = newRow["id"].ToString();
                    newNode = destinateNode.Nodes.Add(sourceNode.Text);
                    newNode.Tag = new object[] { newID };
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在粘贴学习师道具时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }
    }
}
