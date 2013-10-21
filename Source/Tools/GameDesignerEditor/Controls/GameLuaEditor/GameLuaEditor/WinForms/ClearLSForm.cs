using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;

namespace GameLuaEditor
{
    public partial class ClearLSForm : Form
    {
        private bool beginEdit = false; // 是否已经开始编辑
        private bool selectAll = true; // 全部选中
        private DataTable lsTable; // ls数据表
        private DataTable scriptTable; // 脚本数据表
        private Hashtable lsHashtable = new Hashtable(); // ls的哈希表
        private DataBaseManager dataBaseManager; // 数据库交互类

        /// <summary>
        /// 构造函数
        /// </summary>
        public ClearLSForm()
        {
            InitializeComponent();
            InitData();
            beginEdit = true;
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void InitData()
        {
            dataBaseManager = DataBaseManager.GetDataBaseManager();
            lsTable = dataBaseManager.GetLSInformation();
            scriptTable = dataBaseManager.GetScriptInformation();

            if((lsTable != null) && (scriptTable != null)) // 数据表非空
            {
                // 添加下拉框内容
                List<string> lsClassification = dataBaseManager.GetLSClassification();
                foreach(string s in lsClassification)
                {
                    comboBoxEx1.Items.Add(s);
                }
            }
        }
       
        /// <summary>
        /// 获取指定分类里有使用过的LS字符串
        /// </summary>
        /// <param name="classification">分类</param>
        /// <param name="sceneScript">是否是场景下的脚本</param>
        /// <returns>LS字符串</returns>
        private DataRow[] GetValidStringDataRow(string classification, bool sceneScript)
        {
            List<DataRow> validStringDataRowList = new List<DataRow>();            
            List<string> idList = new List<string>(); // 脚本链表

            string folderName = string.Format(@"scripts\Map\{0}", classification);
            string condition = string.Format(@"path LIKE 'scripts\Map\{0}\%'", classification);

            if (!sceneScript) // 非场景分类下的字符串
            {
                folderName = string.Format(@"scripts\{0}", classification);
                condition = string.Format(@"path LIKE 'scripts\{0}%'", classification);
            }

            folderName = Path.Combine(Helper.ClientPath, folderName);
            DirectoryInfo di = new DirectoryInfo(folderName);

            if (di.Exists)
            {
                GetScriptID(di, idList);
            }

            DataRow[] lsRows = lsTable.Select(string.Format("classification = '{0}'", classification));

            foreach (DataRow r in lsRows)
            {                
                string lsID = r["id"].ToString();

                if (idList.Contains(lsID))
                {
                    validStringDataRowList.Add(r);
                }
            }

            return validStringDataRowList.ToArray();
        }

        /// <summary>
        /// 获取脚本字符串ID链表
        /// </summary>
        /// <param name="directoryInfo">当前目录</param>
        /// <param name="idList">字符串ID链表</param>
        private void GetScriptID(DirectoryInfo directoryInfo, List<string> idList)
        {
            foreach (DirectoryInfo di in directoryInfo.GetDirectories())
            {
                if (!di.Attributes.ToString().Contains("Hidden"))
                {
                    GetScriptID(di, idList);
                }
            }

            foreach (FileInfo fi in directoryInfo.GetFiles())
            {
                if (fi.Extension == ".lua")
                {
                    string fileName = fi.FullName;
                    string content = dataBaseManager.GetScriptDataFromFile(fileName);
                    List<string> list = Helper.GetScriptID(content, "SCENE_STRING_TABLE");

                    foreach (string lsID in list)
                    {
                        if (!idList.Contains(lsID))
                        {
                            idList.Add(lsID);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 获取指定分类里所有没有使用过的LS字符串
        /// </summary>
        /// <param name="classification">分类</param>
        /// <returns>LS字符串</returns>
        private List<string> GetUnuseStringList(string classification)
        {
            List<string> unuseStringList = new List<string>();            
            string content;            
            List<string> scriptList = new List<string>(); // 脚本链表

            string condition = string.Format(@"path LIKE 'scripts\Map\{0}\%'", classification);

            if (!checkBoxX1.Checked) // 非场景分类下的字符串
            {
                condition = string.Format(@"path LIKE 'scripts\{0}%'", classification);
            }

            DataRow[] scriptRows = scriptTable.Select(condition);
            
            foreach(DataRow r in scriptRows)
            {
                string id = r["id"].ToString();
                content = dataBaseManager.GetScriptData(id);
                List<string> list = CodeConvert.GetStringList(content);

                foreach (string s in list)
                {
                    if(!scriptList.Contains(s))
                    {
                        scriptList.Add(s);
                    }
                }
            }

            DataRow[] lsRows = lsTable.Select(string.Format("classification = '{0}'", classification));
            
            foreach(DataRow r in lsRows)
            {
                content = r["content"].ToString();

                if (!scriptList.Contains(content))
                {
                    unuseStringList.Add(content);
                }
            }

            return unuseStringList;
        }

        /// <summary>
        /// 清理ls表
        /// </summary>
        private void ClearLSTable()
        {
            List<string> lsList = new List<string>();
            string classification = comboBoxEx1.Text;
            string id;
            DataRow[] lsRows;
            int count = 0;

            if(classification == "") // 排除空分类的情况
            {
                return;
            }

            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                if (checkedListBox1.GetItemChecked(i))
                {
                    lsList.Add(checkedListBox1.Items[i].ToString());
                }
            }

            foreach(string s in lsList)
            {
                lsRows = lsTable.Select(string.Format("classification = '{0}' AND content = '{1}'", classification, s.Replace("'", "''")));
                if(lsRows.Length > 0)
                {
                    foreach(DataRow r in lsRows)
                    {
                        id = r["id"].ToString();
                        dataBaseManager.DeleteLsData(id);
                        count++;
                    }           
                }
            }

            // 清理数据
            lsHashtable[classification] = new List<string>();
            checkedListBox1.Items.Clear();
            MessageBox.Show(string.Format("{0}分类的{1}条ls冗余字符串已经清理!", classification, count.ToString()));
        }

        /// <summary>
        /// 开始清理ls表
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {            
            if (MessageBox.Show("请在确认无人对库中脚本进行编辑时再执行清理LS文件操作，清理LS文件后，部分已删除脚本可能无法恢复，是否继续？", 
                                "清理LS文件确认", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                ClearLSTable();
            }
        }

        /// <summary>
        /// 取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            this.Close();
        }

        /// <summary>
        /// 列表选项改变
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void comboBoxEx1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (beginEdit) // 开始编辑
            {
                int index = comboBoxEx1.SelectedIndex;
                if(index >= 0) // 有效的选择
                {
                    checkedListBox1.Items.Clear();
                    string value = comboBoxEx1.Text;
                    List<string> lsList = lsHashtable[value] as List<string>;
                    if(lsList == null) // 哈希表里没有缓存过数据，需要重新读取
                    {
                        lsList = GetUnuseStringList(value);
                    }
                    
                    foreach(string s in lsList)
                    {
                        checkedListBox1.Items.Add(s);
                    }

                    this.Text = string.Format("LS表字符串清理 共找到{0}条不被引用的字符串", lsList.Count.ToString());
                }
            }
        }

        /// <summary>
        /// 全部选中/取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < checkedListBox1.Items.Count; i++)
            {
                checkedListBox1.SetItemChecked(i, selectAll);
            }

            selectAll = !selectAll;

            if (selectAll)
            {                
                buttonX3.Text = "全部选中";
            }
            else
            {
                buttonX3.Text = "全部取消";
            }
        }

        /// <summary>
        /// 清理并导出所有文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bClearAndExport_Click(object sender, EventArgs e)
        {
            List<TreeNode> nodeList = new List<TreeNode>();

            foreach (TreeNode node in MainForm.CurrentMainForm.tree.Nodes["scripts"].Nodes)
            {
                if (node.Text == "Map")
                {
                    foreach (TreeNode sceneNode in node.Nodes)
                    {
                        nodeList.Add(sceneNode);
                    }
                }
                else
                {
                    nodeList.Add(node);
                }
            }

            ProgressForm pForm = new ProgressForm(1, nodeList.Count);
            pForm.Show();
            int index = 1;

            foreach (TreeNode node in nodeList)
            {
                string classification = node.Text;
                bool sceneScript = (node.Parent.Text == "Map");

                pForm.ShowProgress(index, string.Format("导出{0}分类下的LS数据表...", classification));
                index++;

                string tableName = "SCENE_STRING_TABLE";
                string fileName = string.Format(@"scripts\Map\{0}\include\scenestrings.ls", classification);

                if (!sceneScript)
                {
                    tableName = string.Format("{0}_STRING_TABLE", classification.ToUpper());
                    fileName = string.Format(@"scripts\{0}\include\strings.ls", classification);
                }                                

                DataRow[] lsRows = GetValidStringDataRow(classification, sceneScript);
                string content = CodeConvert.ConvertLsFileContent(lsRows, tableName);

                fileName = Path.Combine(Helper.ClientPath, fileName);
                Helper.WriteStringToFile(content, fileName);
            }

            MessageBox.Show("LS文件导出成功！", "导出所有LS文件",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}