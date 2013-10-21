using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;

namespace GameLuaEditor
{
    public partial class ExportForm : Form
    {
        private List<string> idList;
        private string clietFolder;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="idList">脚本ID链表</param>
        /// <param name="clietFolder">客户端目录</param>
        public ExportForm(List<string> idList, string clietFolder)
        {
            InitializeComponent();
            this.idList = idList;
            this.clietFolder = clietFolder;
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            DataBaseManager databaseManager = DataBaseManager.GetDataBaseManager();

            if (idList != null)
            {
                Hashtable infoTable = databaseManager.OutputScriptData(idList, false);

                if (infoTable != null)
                {
                    foreach (string key in infoTable.Keys)
                    {
                        string content = infoTable[key].ToString();
                        FileListItem listItem = new FileListItem(key, clietFolder, content);
                        listFiles.Items.Add(listItem, true);
                    }
                }
            }
        }

        private void ckOverride_CheckedChanged(object sender, EventArgs e)
        {
            ckBakBeforeOverride.Enabled = ckOverride.Checked;
        }        

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void btnExport_Click(object sender, EventArgs e)
        {
            foreach(Control c in this.Controls)
            {
                if (c is Button) c.Enabled = !c.Enabled;
            }
            foreach(FileListItem fli in listFiles.CheckedItems)
            {
                string filename = fli.fullFilename;
                if(File.Exists(filename))
                {
                    if(!ckOverride.Checked) continue;
                    if(ckBakBeforeOverride.Checked)
                    {
                        string stradd = DateTime.Now.ToString("s").Replace("-","").Replace(":","");
                        File.Copy(filename, filename + "." + stradd + ".bak", true);
                        File.Delete(filename);
                    }
                }
               
                Helper.WriteStringToFile(fli.FileContent, fli.fullFilename);
            }

            foreach (Control c in this.Controls)
            {
                if (c is Button) c.Enabled = !c.Enabled;
            }

            MessageBox.Show("文件已成功导出！", "成功");
            this.Visible = false;
        }

        private void btnAll_Click(object sender, EventArgs e)
        {
            for(int i = 0; i < listFiles.Items.Count;i++)
            {
                listFiles.SetItemChecked(i, true);
            }
        }

        private void btnReverse_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < listFiles.Items.Count; i++)
            {
                listFiles.SetItemChecked(i, !listFiles.GetItemChecked(i));
            }
        }
    }

    public class FileListItem
    {
        private string m_filename = "";
        private string m_path = "";
        private string m_filecontent = "";
        public FileListItem(string filename, string path, string filecontent)
        {
            this.m_filename = filename;
            this.m_path = path;
            this.m_filecontent = filecontent;
        }

        public string fullFilename
        {
            get{return Path.Combine(m_path, m_filename);}
        }

        public string FileContent
        {
            get { return m_filecontent; }
        }

        public override string ToString()
        {
            string strView = "";
            strView = this.fullFilename;
            return strView;
        }
    }
}