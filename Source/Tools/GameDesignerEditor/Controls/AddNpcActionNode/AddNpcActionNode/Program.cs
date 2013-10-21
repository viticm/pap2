using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.IO;

namespace AddNpcActionNode
{
    public class Program
    {
        private ConfigForm cForm; // 显示的窗口
        private MultiAddForm mForm; // 多选的窗口
        private string fileName; // 文件名

        /// <summary>
        /// 模块名称属性
        /// </summary>
        public string ModelName
        {
            get
            {
                return cForm.ModelName;
            }
        }

        /// <summary>
        /// 动作id属性
        /// </summary>
        public String AnimationID
        {
            get
            {
                return cForm.AnimationID;
            }
        }

//         /// <summary>
//         /// 优先级属性
//         /// </summary>
//         public String Priority
//         {
//             get
//             {
//                 return cForm.Priority;
//             }
//         }

        /// <summary>
        /// 序列类型属性
        /// </summary>
        public String KindID
        {
            get
            {
                return cForm.KindID;
            }
        }

        /// <summary>
        /// 动作序号数组属性
        /// </summary>
        public string[] ActionIndexArray
        {
            get
            {
                return mForm.ActionIndexArray;
            }
        }

        /// <summary>
        /// 序列类型数组
        /// </summary>
        public string[] KindIDArray
        {
            get
            {
                return mForm.KindIDArray;
            }
        }

//         /// <summary>
//         /// 优先级数组
//         /// </summary>
//         public string[] PropertyArray
//         {
//             get
//             {
//                 return mForm.PropertyArray;
//             }
//         }

        /// <summary>
        /// 是否循环数组
        /// </summary>
        public string[] LoopArray
        {
            get
            {
                return mForm.LoopArray;
            }
        }

        /// <summary>
        /// 文件名属性
        /// </summary>
        public string FileName
        {
            get
            {
                return fileName;
            }
        }

        /// <summary>
        /// 是否循环属性
        /// </summary>
        public string Loop
        {
            get
            {
                return cForm.Loop;
            }
        }

        /// <summary>
        /// 文件路径属性
        /// </summary>
        public string FilePath
        {
            get
            {
                return cForm.FilePath;
            }
        }

        /// <summary>
        /// 显示输入窗口
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="representID">表现ID</param>
        /// <returns>用户是否确定选择</returns>
        public int Show(SqlConnection conn, string representID)
        {
            if (cForm == null)
                cForm = new ConfigForm(conn);

            cForm.InitFileInfo(representID);

            DialogResult result = cForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// 显示多选的窗口
        /// </summary>
        /// <returns>用户是否确定选择</returns>
        public int ShowMultiAddForm(SqlConnection conn)
        {
            mForm = new MultiAddForm(conn);
            DialogResult result = mForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// 检查文件是否存在
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <param name="fileName">文件名</param>
        /// <returns>文件是否存在</returns>
        public bool CheckFileExist(string filePath, string fileName)
        {
            fileName = fileName.ToLower();
            bool result = false;

            DirectoryInfo di = new DirectoryInfo(filePath);
            if(di.Exists)
            {
                bool newType = false; // 是否是新存储类型

                DirectoryInfo[] folders = di.GetDirectories();
                foreach(DirectoryInfo dInfo in folders)
                {
                    if (dInfo.Name == "动作")
                    {
                        newType = true;
                        break;
                    }
                }

                if(newType)
                {
                    di = new DirectoryInfo(Path.Combine(di.FullName, "动作"));
                }
                FileInfo[] files = di.GetFiles();
                foreach(FileInfo fi in files)
                {
                    string str1 = fi.Name.ToLower();
                    string str2 = fi.Extension.ToLower();
                    if(str1.Contains(fileName) && (str2 == ".ani" || str2 == ".tani"))
                    {
                        this.fileName = str1;
                        result = true;
                        break;
                    }
                }

                if(newType)
                {
                    this.fileName = "动作\\" + this.fileName;
                }
            }

            return result;
        }
    }
}