using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Data.SqlClient;

namespace UploadImageToDatabase
{
    public partial class MainForm : Form
    {
        private string clientFolder; // 客户端文件夹
        private SqlConnection conn; // sql连接

        public MainForm()
        {
            InitializeComponent();
        }       

        /// <summary>
        /// 初始化sql连接
        /// </summary>
        private void InitConnection()
        {
            string connectString = "Server = 192.168.27.31; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            conn = new SqlConnection(connectString);
        }

        /// <summary>
        /// 更新图片
        /// </summary>
        /// <param name="tableName">数据表名</param>
        /// <param name="keyField">主键字段</param>
        private void UpdateImage(string tableName, string keyField)
        {
            if (conn == null)
            {
                InitConnection();
            }
            
            DirectoryInfo di = new DirectoryInfo(clientFolder);

            if (di.Exists)
            {
                FileInfo[] files = di.GetFiles();
                FileStream fileStream;
                int updateCount = 0;

                try
                {
                    if (conn.State == ConnectionState.Closed)
                    {
                        conn.Open();
                    }

                    string sqlString = null;
                    object executeResult = null;
                    SqlCommand cmd = conn.CreateCommand();

                    int totalCount = files.Length;
                    int currentCount = 0;

                    foreach (FileInfo fi in files)
                    {
                        if (fi.Extension == ".jpg")
                        {
                            fileStream = fi.OpenRead();
                            byte[] buffer = new byte[fileStream.Length];
                            fileStream.Read(buffer, 0, buffer.Length);
                            fileStream.Close();

                            string keyValue = fi.Name.Replace(".jpg", "");
                            sqlString = string.Format("SELECT EditTime FROM {0} WHERE [{1}] = '{2}'",
                                                      tableName, keyField, keyValue);
                            cmd.CommandText = sqlString;
                            executeResult = cmd.ExecuteScalar();

                            if (executeResult != null) // 图片记录已经存在
                            {
                                DateTime editTime = (DateTime)executeResult;                                

                                if (fi.LastWriteTime > editTime) // 需要更新图片
                                {
                                    DateTime newTime = fi.LastWriteTime.AddSeconds(1); // 故意增加一秒，不然if条件总是满足，很诡异

                                    sqlString = string.Format("UPDATE {0} SET EditTime = '{1}', Image = @image WHERE [{2}] = '{3}'",
                                                              tableName, newTime, keyField, keyValue);
                                    cmd.CommandText = sqlString;
                                    cmd.Parameters.Add("@image", SqlDbType.Image).Value = buffer;
                                    cmd.ExecuteNonQuery();
                                    cmd.Parameters.Clear();

                                    updateCount++;
                                }
                            }
                            else // 直接插入即可
                            {
                                sqlString = string.Format("INSERT INTO {0} ({1}, EditTime, Image) VALUES ('{2}', '{3}', @image)",
                                                          tableName, keyField, keyValue, fi.LastWriteTime);
                                cmd.CommandText = sqlString;
                                cmd.Parameters.Add("@image", SqlDbType.Image).Value = buffer;
                                cmd.ExecuteNonQuery();
                                cmd.Parameters.Clear();

                                updateCount++;
                            }
                        }

                        currentCount++;
                        this.Text = string.Format("预览图片批量更新 {0}/{1}", currentCount.ToString(), totalCount.ToString());
                    }

                    this.Text = "预览图片批量更新 （完成）";
                }
                catch (Exception ex)
                {
                    MessageBox.Show("在更新预览图片时产生异常：" + ex.Message, "更新预览图片", MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
                }
                finally
                {
                    if (conn.State == ConnectionState.Open)
                    {
                        conn.Close();
                    }
                }

                MessageBox.Show(string.Format("共更新{0}条记录", updateCount.ToString()), "更新预览图片", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show(string.Format("目录{0}不存在！", clientFolder), "更新武预览图片", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 选择更新
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            UpdateImage("WeaponImage", "ItemNumber");
        }

        /// <summary>
        /// 关闭窗体
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// 浏览文件夹
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBroserDialog = new FolderBrowserDialog();
            if (clientFolder != null)
            {
                folderBroserDialog.SelectedPath = clientFolder;
            }
            folderBroserDialog.Description = "请选择图片文件夹所在的目录";

            if (folderBroserDialog.ShowDialog() == DialogResult.OK)
            {
                clientFolder = folderBroserDialog.SelectedPath;
                textBoxX1.Text = clientFolder;
            }
        }

        /// <summary>
        /// 更新模型图片
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            UpdateImage("ModelImage", "EditorID");
        }
    }
}