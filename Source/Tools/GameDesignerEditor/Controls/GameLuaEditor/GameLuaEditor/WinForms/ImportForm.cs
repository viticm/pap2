using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace GameLuaEditor
{
    public partial class ImportForm : Form
    {
        private List<string> scriptList;
        private string scriptFolder;
        private TreeNode rootNode;
        private string fileFolder;
        private bool selectAll = true;

        // 创建树结点用的回调
        public delegate TreeNode CreateNewNode(TreeNode parentNode, string scriptName, string scriptID);
        private CreateNewNode createNewNode;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="rootNode">根结点</param>
        /// <param name="scriptList">脚本链表</param>
        /// <param name="fileFolder">文件文件夹</param>
        public ImportForm(TreeNode rootNode,List<string> scriptList, string fileFolder)
        {
            this.rootNode = rootNode;
            this.scriptList = scriptList;
            this.fileFolder = fileFolder;
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 创建树结点
        /// </summary>
        public CreateNewNode CreateTheNewNode
        {
            set
            {
                createNewNode = value;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            scriptFolder = rootNode.FullPath;

            scriptListBox.Items.Clear();

            for (int i = 0; i < scriptList.Count; i++)
            {
                scriptListBox.Items.Add(scriptList[i]);
                scriptListBox.SetItemChecked(i, true);
            }            
        }

        /// <summary>
        /// 选择确定
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOK_Click(object sender, EventArgs e)
        {
            bool coverScript = coverScriptBox.Checked;

            if (coverScript)
            {
                if (MessageBox.Show("确定要覆盖已有脚本吗？", " 导入脚本",
                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
                    == DialogResult.Cancel) 
                {
                    coverScript = false;
                }
            }

            DataBaseManager dataBaseManager = DataBaseManager.GetDataBaseManager();
            int successCount = 0;

            scriptList.Clear();

            for (int i = 0; i < scriptListBox.Items.Count; i++)
            {
                if (scriptListBox.GetItemChecked(i))
                {
                    scriptList.Add(scriptListBox.Items[i] as string);
                }
            }

            ProgressForm progressForm = new ProgressForm(0, scriptList.Count);
            progressForm.Show();

            for (int i = 0; i < scriptList.Count; i++)
            {
                string scriptName = scriptList[i];
                string fileName = Path.Combine(fileFolder, scriptName);
                string content = Helper.FileToString(fileName);

                progressForm.ShowProgress(i, string.Format("导入脚本{0}的数据...", scriptName));

                if (!string.IsNullOrEmpty(content))
                {
                    string dataBasePath = Path.Combine(scriptFolder, scriptName);
                    string id = dataBaseManager.CreateScriptData(dataBasePath, content, coverScript);

                    if (id != null)
                    {
                        createNewNode(rootNode, scriptName, id);
                        successCount++;
                    }
                }
            }

            progressForm.Close();

            MessageBox.Show(string.Format("共导入{0}个脚本文件！", successCount), "导入脚本",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);

            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 选择取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// 全部选中/取消
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void selectAllBox_CheckedChanged(object sender, EventArgs e)
        {
            selectAll = !selectAll;

            for (int i = 0; i < scriptListBox.Items.Count; i++)
            {
                scriptListBox.SetItemChecked(i, selectAll);
            }
        }
    }
}