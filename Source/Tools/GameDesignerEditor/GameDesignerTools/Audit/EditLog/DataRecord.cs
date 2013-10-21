using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Data;

namespace GameDesingerTools
{
    class DataRecord
    {
        private SqlConnection conn; // sql连接
        private static DataRecord dataRecord; // 唯一的一个实例
        private DateTime time; // 修改时间
        private string userName; // 用户名
        private string[] primaryKeys; // 主键集合
        private string catfields; // 分类
        private string oldValue; // 旧值
        private string newValue; // 新值
        private TreeNode currentNode; // 当前树结点
        private string nodeName; // 当前树结点的名称
        private int modelID; // 模块id
        private int tabID; // tab页id
        private int fieldID; // 字段id
        private string fieldDisplayName; // 字段的显示值
        private string remarks;//备注

        private string action = "修改值";

        /// <summary>
        /// 私有构造函数，不允许有新的实例
        /// </summary>
        private DataRecord()
        {

        }

        /// <summary>
        /// sql连接属性
        /// </summary>
        public SqlConnection Conn
        {
            set
            {
                conn = value;
            }
        }

        /// <summary>
        /// 字段的id属性
        /// </summary>
        public int FieldID
        {
            set
            {
                fieldID = value;
            }
        }

        /// <summary>
        /// 字段的显示值属性
        /// </summary>
        public string FieldDisplayName
        {
            set
            {
                fieldDisplayName = value;
            }
        }

        /// <summary>
        /// 当前树结点属性
        /// </summary>
        public TreeNode CurrentNode
        {
            set
            {
                currentNode = value;
            }     
        }

        /// <summary>
        /// 修改时间属性
        /// </summary>
        public DateTime Time 
        {
            set
            {
                time = value;
            }
        }

        /// <summary>
        /// 用户名属性
        /// </summary>
        public string UserName
        {
            set
            {
                userName = value;
            }
        }

        public string Action
        {
            set 
            { 
                action = value; 
            }
        }
        /// <summary>
        /// 主键集合属性
        /// </summary>
        public string[] PrimaryKeys
        {
            set
            {
                primaryKeys = value;
            }
        }

        /// <summary>
        /// 旧值属性
        /// </summary>
        public string OldValue
        {
            set
            {
                oldValue = value;
            }
        }

        /// <summary>
        /// 新值属性
        /// </summary>
        public string NewValue
        {
            set
            {
                newValue = value;
            }
        }

        /// <summary>
        /// 模块ID属性
        /// </summary>
        public int ModelID
        {
            set
            {
                modelID = value;
            }
        }

        /// <summary>
        /// tab页ID属性
        /// </summary>
        public int TabID
        {
            set
            {
                tabID = value;
            }
        }
        public string Remarks
        {
            set
            {
                remarks = value;
            }
        }

        /// <summary>
        /// 获取实例
        /// </summary>
        /// <returns>实例</returns>
        public static DataRecord GetDataRecord()
        {
            if(dataRecord == null) // 实例未初始化
            {
                dataRecord = new DataRecord();
            }
            return dataRecord;
        }
        
        public static string TranslateValue(string value)
        {
            // 将旧值和新值统一转换为true和false
            string newValue = value.ToString(); // 新值
            switch (value) // 替换旧值的显示
            {
                case "True":
                    {
                        newValue = "是";
                        break;
                    }
                case "False":
                    {
                        newValue = "否";
                        break;
                    }
                case "1":
                    {
                        newValue = "是";
                        break;
                    }
                case "0":
                    {
                        newValue = "否";
                        break;
                    }
                default:
                    {
                        newValue = "是";
                        break;
                    }
            }

            return newValue;
        }


        /// <summary>
        /// 保存字段修改的历史记录
        /// </summary>
        public void Save()
        {
            try
            {
                if(conn.State == System.Data.ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                nodeName = currentNode.Text;
                catfields = GetCatifields();

                string sql = string.Format("INSERT INTO {0} (hostname, edittime, keys, oldvalue, newvalue, modelid, tabid, nodename, " +
                    "catfields, fielddisplayname, fieldid, remarks, [action]) VALUES (\'{1}\', \'{2}\', \'{3}\', \'{4}\', \'{5}\', \'{6}\', \'{7}\', \'{8}\', \'{9}\', \'{10}\', \'{11}\', \'{12}\', \'{13}\')",  
                    "sys_dataRecord", userName, time, GetPrimaryKeys(), oldValue, newValue, modelID, tabID, nodeName, catfields, fieldDisplayName, fieldID, remarks, action);
                //SqlCommand cmd = new SqlCommand(sql, conn);
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand("sys_dataRecord", sql, conn);
            }
            catch(Exception ex)
            {
                MessageBox.Show("在更新字段修改历史记录时产生异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == System.Data.ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 获取包含其所有父字段名的完整显示名称
        /// </summary>
        /// <returns>包含其所有父字段名的完整显示名称</returns>
        private string GetFullDisplayName()
        {
            string fullDisplayName = fieldDisplayName.Trim(); // 完整显示名称

            try
            {
                /* remoting
                string sql = string.Format("SELECT parentfield FROM {0} WHERE fieldid = {1}", "sys_meta_info", fieldID.ToString());
                DataTable table = Helper.GetDataTable(sql, conn); 
                 */

                string filter = string.Format("fieldid = {0}", fieldID.ToString());
                DataTable table = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

                string id = table.Rows[0]["parentfield"].ToString();
                while((id != "0") || (id != ""))
                {
                    /* remoting
                    sql = string.Format("SELECT parentfield, fieldcnname FROM {0} WHERE fieldid = {1}", "sys_meta_info", id);
                    table = Helper.GetDataTable(sql, conn);
                     */

                    filter = string.Format("fieldid = {0}", id);
                    table = Helper.GetDataTableProxy("sys_meta_info", filter, null, conn);

                    if(table.Rows.Count > 0) // 找到父结点
                    {
                        fullDisplayName = string.Format("{0}.", table.Rows[0]["fieldcnname"].ToString().Trim()) + fullDisplayName;
                        id = table.Rows[0]["parentfield"].ToString();
                    }
                    else
                    {
                        break;
                    }
                }              
            }
            catch(Exception ex)
            {
                MessageBox.Show("在获取当前字段的完整显示名称时产生异常: " + ex.ToString());
                fullDisplayName = fieldDisplayName; // 恢复名称
            }

            return fullDisplayName;
        }

        /// <summary>
        /// 获取树结点的分类字符串
        /// </summary>
        /// <returns>树结点的分类字符串</returns>
        private string GetPrimaryKeys()
        {
            string primaryKeysString = ""; // 主键字符串

            object[] keys = currentNode.Tag as object[];
            if(keys != null) // 主键集合不为空
            {
                foreach(object o in keys)
                {
                    primaryKeysString += string.Format("{0},", o);
                }
            }

            return primaryKeysString.TrimEnd(',');
        }

        /// <summary>
        /// 获取树结点的分类信息
        /// </summary>
        /// <returns>树结点的分类信息字符串</returns>
        private string GetCatifields()
        {
            string catifieldString = ""; // 分类字符串
            TreeNode fatherNode = currentNode.Parent; // 父结点

            while(fatherNode != null)
            {
                if(fatherNode.Tag != null)
                {
                    if(fatherNode.Tag is string) // 记录分类信息
                    {
                        catifieldString += string.Format("{0},", fatherNode.Text);
                    }
                    else if((fatherNode.Tag as object[]) != null) // 记录主键信息
                    {
                        try
                        {
                            string tagValue = (fatherNode.Tag as object[])[0].ToString();
                            if(int.Parse(tagValue) < 0) // 是分类结点
                            {
                                catifieldString += string.Format("{0},", fatherNode.Text);
                            }
                        }
                        catch(Exception ex)
                        {
                            MessageBox.Show("在获取树结点的分类信息时产生异常: " + ex.ToString());
                        }
                    }                    
                }
                fatherNode = fatherNode.Parent;
            }

            return catifieldString.TrimEnd(',');
        }
    }
}
