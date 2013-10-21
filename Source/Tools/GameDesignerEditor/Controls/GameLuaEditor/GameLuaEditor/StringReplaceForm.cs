using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace GameLuaEditor
{
    public partial class StringReplaceForm : Form
    {
        private MainForm m_parent = null;
        private string m_strAllMaps = "[所有脚本]";
        private List<string> findTextList = new List<string>();
        private List<string> replaceTextList = new List<string>();        

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="mainForm">主窗体</param>
        public StringReplaceForm(MainForm mainForm)
        {
            InitializeComponent();
            this.m_parent = mainForm;
        }

        /// <summary>
        /// 初始化下拉框
        /// </summary>
        private void InitComboBox()
        {
            findTextBox.Items.Clear();

            foreach (string s in findTextList)
            {
                findTextBox.Items.Add(s);
            }

            replaceTextBox.Items.Clear();

            foreach (string s in replaceTextList)
            {
                replaceTextBox.Items.Add(s);
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        /// <param name="findText">查找的文本</param>
        /// <param name="mapName">地图名</param>
        public void Init(string findText, string mapName)
        {
            InitComboBox();

            // 初始化地图选择框
            if (m_parent.treePh.Nodes.Count > 0)
            {
                cbMaps.Items.Clear();
                cbMaps.Items.Add(m_strAllMaps);

                TreeNode tn = m_parent.treePh.Nodes["scripts"].Nodes["Map"];

                foreach (TreeNode tnz in tn.Nodes)
                {
                    cbMaps.Items.Add(tnz.Text);
                }                
            }

            findTextBox.Text = findText;
            replaceTextBox.Text = "";
            cbMaps.Text = mapName;
                                  
            // 自动查找
            if (findTextBox.Text != "")
            {
                this.btnFind_Click(this, new EventArgs());
            }

            // 显示窗体
            this.Show();

            // 获取焦点
            this.Focus();
        }

        /// <summary>
        /// 初始化替换树
        /// </summary>
        /// <param name="menpaiName">地图名</param>
        /// <param name="strFind">查找的字符串</param>
        /// <param name="strReplace">替换的字符串</param>
        private void InitTreeReplace(string menpaiName, string strFind, string strReplace)
        {
            Hashtable ht_data = new Hashtable();
            TreeReplaceOpera tro = new TreeReplaceOpera();
            Hashtable ht_filelist = new Hashtable();
            ArrayList al_fileIDlist = new ArrayList();

            if (!string.IsNullOrEmpty(strFind) && !findTextList.Contains(strFind))
            {
                findTextList.Add(strFind);
            }

            if (!string.IsNullOrEmpty(strReplace) && !replaceTextList.Contains(strReplace))
            {
                replaceTextList.Add(strReplace);
            }

            if (menpaiName != m_strAllMaps)
            {
                ht_filelist = this.m_parent.getFileListFromPath("scripts\\Map\\" + menpaiName);
            }
            else
            {
                ht_filelist = this.m_parent.getFileListFromPath("scripts");
            }

            ht_data["filelist"] = ht_filelist;
            ht_data["find"] = strFind;
            ht_data["replace"] = strReplace;
            TreeNodeCollection tnc = tro.LoadTreeNodeCollection(ht_data);
            treePreview.Nodes.Clear();

            foreach (TreeNode tn in tnc)
            {
                this.treePreview.Nodes.Add(tn);
            }

            if (tnc.Count > 0)
            {
                btnReplace.Enabled = true;
            }

            wbPriview.DocumentText = "";

            InitComboBox();
        }

        /// <summary>
        /// 查找字符串
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void btnFind_Click(object sender, EventArgs e)
        {
            SetControlsEnabled(false);
            string strFind = findTextBox.Text;
            string strReplace = replaceTextBox.Text;
            InitTreeReplace(cbMaps.Text, strFind, strReplace);

            if (treePreview.Nodes.Count == 0)
            {
                MessageBox.Show("没有找到匹配的文本！", "查找文本", 
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            SetControlsEnabled(true);
        }

        private void btnReplace_Click(object sender, EventArgs e)
        {

            if (MessageBox.Show("替换结果将不直接写入数据库，而且不能恢复，确认继续操作？", "确认", MessageBoxButtons.YesNo, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button2) != DialogResult.Yes)
            {
                return;
            }

            SetControlsEnabled(false);
            if(treePreview.Nodes.Count == 0)
            {
                MessageBox.Show("请先搜索出文件，再针对文件进行替换！");
                SetControlsEnabled(true);
                return;
            }

            DataBaseIO.DataBaseManager dbm = DataBaseIO.DataBaseManager.GetDataBaseManager();

            //第1步 检查锁
            foreach(TreeNode tn_file in treePreview.Nodes)
            {
                if(!tn_file.Checked) continue;
                Hashtable ht_file = tn_file.Tag as Hashtable;
                string strID = ht_file["id"].ToString();
                if( dbm.IsScriptLocked(strID) )
                {
                     //文件锁了
                    MessageBox.Show("失败：文件[" + tn_file.Text + "]被正在被人编辑，无法启动替换。");
                    SetControlsEnabled(true);
                    return;
                }
            }

            //第2步 检查在选择过程中，选中项的服务器端数据变过没有
            foreach(TreeNode tn_file in treePreview.Nodes)
            {
                if(!tn_file.Checked) continue;
                Hashtable ht_file = tn_file.Tag as Hashtable;
                string strFileContent = ht_file["strFileContent"].ToString();
                string strID = ht_file["id"].ToString();
                if( strFileContent != dbm.GetScriptData(strID))
                {
                     //服务器端变过了
                    MessageBox.Show("失败：查找之后，替换之前，文件内容已变更，所以无法替换，请重新启动替换程序！");
                    SetControlsEnabled(true);
                    return;
                }
            }

            //第3步 锁定选择项
            foreach (TreeNode tn_file in treePreview.Nodes)
            {
                if (!tn_file.Checked) continue;
                Hashtable ht_file = tn_file.Tag as Hashtable;
                string strID = ht_file["id"].ToString();
                if(!dbm.LockScript(strID))
                {
                    MessageBox.Show("失败：检查一致性的过程中有人锁定了脚本，请迅速保存所有文件后重新运行脚本编辑器再次替换。");
                    SetControlsEnabled(true);
                    return;
                }
            }

            //第4步 替换选择项
            string strResult = "";
            string strFind = findTextBox.Text;
            string strReplace = replaceTextBox.Text;

            foreach (TreeNode tn_file in treePreview.Nodes)
            {
                if (!tn_file.Checked) continue;
                Hashtable ht_file = tn_file.Tag as Hashtable;
                string strContent = "";
                foreach (TreeNode tnz in tn_file.Nodes)
                {
                    Hashtable ht_nodez = tnz.Tag as Hashtable;
                    strContent += ht_nodez["line_before"].ToString();
                    if (!tnz.Checked)
                        strContent +=  strFind ;
                    else
                        strContent +=  strReplace;
                }
                strContent += ht_file["line_last"].ToString();
                string strView = "";
                bool ret = dbm.SaveScriptData(tn_file.Text, strContent, out strView); 
                strResult += tn_file.Text + (ret ? "...OK\n" : "...NG\n");
            }

            //第5步 解锁选择项
            foreach (TreeNode tn_file in treePreview.Nodes)
            {
                if (!tn_file.Checked) continue;
                Hashtable ht_file = tn_file.Tag as Hashtable;
                string strID = ht_file["id"].ToString();
                if (!dbm.UnLockScript(strID))
                {
                    MessageBox.Show(strResult + "成功：替换完成后解锁失败，原因不明，请联系管理员");
                    SetControlsEnabled(true);
                    return;
                }
            }

            MessageBox.Show("替换完成！");//\n" + strResult);
            treePreview.Nodes.Clear();
            wbPriview.DocumentText = "";
            SetControlsEnabled(true);
        }

        /// <summary>
        /// 设置控件是否可用
        /// </summary>
        /// <param name="b">控件是否可用</param>
        private void SetControlsEnabled(bool b)
        {
            btnReplace.Enabled = b;
            btnFind.Enabled = b;
            btnCancel.Enabled = b;            
            findTextBox.Enabled = b;
        }

        private void treePreview_AfterSelect(object sender, TreeViewEventArgs e)
        {
            ReloadPreview();
        }

        private void ReloadPreview()
        {
            TreeNode tn = treePreview.SelectedNode;
            if (tn == null) return;
            Hashtable ht_node = tn.Tag as Hashtable;
            if (ht_node == null) return;
            string strlineNumber = "";
            if (ht_node["type"].ToString() == "file")
            {
                //do nothing
                strlineNumber = "0";

            }
            else if (ht_node["type"].ToString() == "line")
            {
                strlineNumber = ht_node["lineNumber"].ToString();
                tn = tn.Parent;
                ht_node = tn.Tag as Hashtable;
                if (ht_node == null) return;
            }
            else
            {
                throw (new Exception("树结构被破坏"));
            }

            string strContent = "";
            string strFind = findTextBox.Text;
            string strReplace = replaceTextBox.Text;
//             {
//                 if (strFind.IndexOf('\n') != -1)
//                 {
//                     strFind = "[[" + strFind + "]]";
//                 }
//                 else
//                 {
//                     strFind = "\"" + strFind + "\"";
//                 }
//                 if (strReplace.IndexOf('\n') != -1)
//                 {
//                     strReplace = "[[" + strReplace + "]]";
//                 }
//                 else
//                 {
//                     strReplace = "\"" + strReplace + "\"";
//                 }
//             }
            foreach(TreeNode tnz in tn.Nodes)
            {
                Hashtable ht_nodez = tnz.Tag as Hashtable;
                strContent += ht_nodez["line_before"].ToString();
                if (!tnz.Checked || !tnz.Parent.Checked)
                {
                    strContent += "@@key@@" + strFind + "@@key1@@";
                }
                else
                {
                    if (strReplace == "")
                    {
                        strContent += "@@key6@@" + strFind + "@@key1@@";
                    }
                    else
                    {
                        strContent += "@@key3@@" + strReplace + "@@key1@@";
                    }
                }

            }
            strContent += ht_node["line_last"].ToString();
            
            //加行号
            string[] asp = {"\n"};
            string[] as_strContent = strContent.Split(asp, StringSplitOptions.None);
            strContent = "";
            for(int i = 0; i < as_strContent.Length; i++)
            {
                string strLine = strFillZero(i + 1, as_strContent.Length);
                strContent += "@@key2@@" + strLine + "@@key4@@"  +  strLine + "@@key5@@   " + as_strContent[i] + "\n";
            }

            if (Int32.Parse(strlineNumber) > 5) strlineNumber = (Int32.Parse(strlineNumber) - 5).ToString();
            string url = System.IO.Path.GetTempFileName() + ".htm";
            Helper.WriteStringToFile(HTMLFormat(strContent), url);
            url += "#" + strFillZero(Int32.Parse(strlineNumber.ToString()), as_strContent.Length);
            wbPriview.Url = new Uri(url);
            System.IO.File.Delete(url);
        }

        private string strFillZero(int nLine, int nCount)
        {
            string sret = nLine.ToString();
            while(sret.Length < nCount.ToString().Length)
            {
                sret = "0" + sret;
            }
            return sret;
        }

        private string HTMLFormat(string strContent)
        {
            strContent = strContent.Replace("&", "&amp");
            strContent = strContent.Replace("<", "&lt");
            strContent = strContent.Replace(">", "&gt");
            strContent = strContent.Replace("chr(60)", "&lt");
            strContent = strContent.Replace("chr(37)", "&gt");
            strContent = strContent.Replace("\"", "&quot");
            strContent = strContent.Replace(";", ";");
            strContent = strContent.Replace("\n", "<br/>");
            strContent = strContent.Replace(" ", "&nbsp");
            strContent = strContent.Replace("\t", "&nbsp&nbsp&nbsp&nbsp");
            strContent = strContent.Replace("@@key@@", "<span class='cw_before'>");
            strContent = strContent.Replace("@@key1@@", "</span>");
            strContent = strContent.Replace("@@key2@@", "<span class='line'>");
            strContent = strContent.Replace("@@key3@@", "<span class='cw_after'>");
            strContent = strContent.Replace("@@key4@@", "</span><a id='");
            strContent = strContent.Replace("@@key5@@", "'></a>");
            strContent = strContent.Replace("@@key6@@", "<span class='cw_deleted'>");

            string strHead = @"
<html>
    <head>
    <meta http-equiv=Content-Type content='text/html;charset=gb2312'>
    <style type='text/css'>
    <!--
    .cw_after {
	    background-color: #FF8080;
	    color: #000000;
	    text-decoration: blink;
    }
    .cw_before {
	    background-color: #440000;
	    color: #FFFFFF;
	    text-decoration: blink;
    }
    .cw_deleted {
	    background-color: #FF8080;
	    color: #000000;
	    text-decoration: line-through;
    }
    body,td,th {
	    font-family: Verdana, Arial, Helvetica, sans-serif;
	    font-size: 9pt;
    }
    body {
	    margin-top: 1px;
	    margin-left: 1px;
    }
    .line {
	    color: #888888;
    }
    -->
    </style>
    </head>
";

            return strHead + strContent;
        }

        private void treePreview_AfterCheck(object sender, TreeViewEventArgs e)
        {
            ReloadPreview();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void txtFind_TextChanged(object sender, EventArgs e)
        {
            btnReplace.Enabled = false;
        }

        private void txtReplace_TextChanged(object sender, EventArgs e)
        {
            ReloadPreview();
        }

        /// <summary>
        /// 打开文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (treePreview.SelectedNode == null)
            {
                MessageBox.Show("请先选择要打开的文件！", "打开文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                if (!MainForm.CurrentMainForm.OpenFileByName(treePreview.SelectedNode.Text))
                {
                    MessageBox.Show("打开文件失败！", "打开文件",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// 窗体关闭事件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void StringReplaceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // 刷新主窗口内未锁定的编辑窗口
            foreach (EditForm editForm in m_parent.MdiChildren)
            {
                if (!editForm.IsChanged)
                {
                    editForm.Reload();
                }
            }

            e.Cancel = true;
            this.Visible = false;
        }

    }
}