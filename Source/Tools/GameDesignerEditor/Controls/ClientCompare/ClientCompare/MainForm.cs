using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using DevComponents.DotNetBar;
using ClientDiff;

namespace ClientCompare
{
    public partial class MainForm : Office2007Form
    {
        MapLogicalCompare.MainForm mapLocicalCompareForm;
        ClientDiff.Form1 tabCompareForm;

        private bool compareTabFile = true;
        private bool compareIniFile = true;
        private bool compareTxtFile = true;
        private bool compareLuaFile = true;
        private bool compareLSFile = true;
        private bool compareLHFile = true;
        private bool compareLogicalFile = true;
        private bool compareOtherFile = false;
        private string compareApplicationPath = "";

        IconManager iconManager = new IconManager();
        private string leftFolder;
        private string rightFolder;
        private char[] pathSplitArray = new char[] { '\\', '.' };

        Hashtable m_diffColInfo;
        string m_diffLog = string.Empty;

        [DllImport("ntdll.dll")]
        public static extern int RtlCompareMemory(IntPtr Destination, IntPtr Source, int Length);

        private int copyedFileCount = 0;
        private delegate void ThreadUIDelegate(string labelText);

        public MainForm()
        {
            InitializeComponent();

            this.Text += " - " + Application.ProductVersion.ToString();

            string strIniFile = Application.StartupPath + "/config.ini";
            StringBuilder sb = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("General", "compareApplicationPath", "", sb, 255, strIniFile);
            compareApplicationPath = sb.ToString();
            Init();
            LoadHistory();
        }

        private void LoadHistory()
        {
            string strfile = Path.Combine(Application.StartupPath, "history.txt");
            string strHistory = FileFolderHelper.FileToString(strfile);
            if (strHistory.Length == 0) return;

            string[] historys = strHistory.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string history in historys)
            {
                DevComponents.DotNetBar.ButtonItem newitemLeft = new DevComponents.DotNetBar.ButtonItem();
                newitemLeft.ImagePaddingHorizontal = 8;
                newitemLeft.Name = history;
                newitemLeft.Text = history;
                newitemLeft.Click += new System.EventHandler(this.historyItem_Click_Left);
                this.bOpenFolder1.SubItems.Add(newitemLeft);

                DevComponents.DotNetBar.ButtonItem newitemRight = new DevComponents.DotNetBar.ButtonItem();
                newitemRight.ImagePaddingHorizontal = 8;
                newitemRight.Name = history;
                newitemRight.Text = history;
                newitemRight.Click += new System.EventHandler(this.historyItem_Click_Right);
                this.bOpenFolder2.SubItems.Add(newitemRight);
            }
        }
        private void historyItem_Click_Left(object sender, EventArgs e)
        {
            DevComponents.DotNetBar.ButtonItem item = sender as DevComponents.DotNetBar.ButtonItem;
            string filename = item.Text;
            OpenFolder("left", filename);
        }
        private void historyItem_Click_Right(object sender, EventArgs e)
        {
            DevComponents.DotNetBar.ButtonItem item = sender as DevComponents.DotNetBar.ButtonItem;
            string filename = item.Text;
            OpenFolder("right", filename);
        }
        private void UpdateHistory(string filename)
        {
            string strfile = Path.Combine(Application.StartupPath, "history.txt");
            string strHistory = FileFolderHelper.FileToString(strfile);
            string[] historys = strHistory.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            if (historys.Length < 4)
            {
                bool bHistoryContainsThisDoc = false;
                foreach (string history in historys)
                {
                    if (history == filename)
                    {
                        bHistoryContainsThisDoc = true;
                        break;
                    }
                }

                if (!bHistoryContainsThisDoc)
                {
                    string strContent = strHistory + (historys.Length == 0 ? string.Empty : "\r\n") + filename;
                    FileFolderHelper.StringToFile(strContent, strfile);
                }
            }
            else
            {
                bool bHistoryContainsThisDoc = false;
                foreach (string history in historys)
                {
                    if (history == filename)
                    {
                        bHistoryContainsThisDoc = true;
                        break;
                    }
                }

                if (!bHistoryContainsThisDoc)
                {
                    string[] newhistorys = new string[] { historys[1], historys[2], historys[3], filename };
                    string content = newhistorys[0] + "\r\n" + newhistorys[1] + "\r\n" + newhistorys[2] + "\r\n" + newhistorys[3];
                    FileFolderHelper.StringToFile(content, strfile);
                }
            }

        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            leftView.LargeImageList = iconManager.LargeIconImageList;
            leftView.SmallImageList = iconManager.SmallIconImageList;

            rightView.LargeImageList = iconManager.LargeIconImageList;
            rightView.SmallImageList = iconManager.SmallIconImageList;
        }

        /// <summary>
        /// 打开目标目录中对应的文件夹
        /// </summary>
        /// <param name="currentFolder">当前文件夹</param>
        /// <returns>是否打开成功</returns>
        private bool OpenRelevateFolder(string currentFolder)
        {
            bool openSuccess = false;

            string destFolder = currentFolder.Replace(leftFolder, rightFolder);
            DirectoryInfo di = new DirectoryInfo(destFolder);

            if (di.Exists)
            {
                ShowFolderContent(rightView, di.FullName);
                openSuccess = true;
            }

            return openSuccess;
        }

        /// <summary>
        /// 更新配置信息
        /// </summary>
        /// <param name="cForm">配置窗体对象</param>
        private void UpdateConfigInfo(ConfigForm cForm)
        {
            compareApplicationPath = cForm.FilePath;
            compareTabFile = cForm.CompareTabFile;
            compareTxtFile = cForm.CompareTxtFile;
            compareIniFile = cForm.CompareIniFile;
            compareLuaFile = cForm.CompareLuaFile;
            compareLSFile = cForm.CompareLSFile;
            compareLHFile = cForm.CompareLHFile;
            compareLogicalFile = cForm.CompareLogicalFile;
            compareOtherFile = cForm.CompareOtherFile;
            FileFolderHelper.WriteIniFile("General", "compareApplicationPath", compareApplicationPath, Application.StartupPath + "/config.ini");
        }

        /// <summary>
        /// 比较文件
        /// </summary>
        /// <param name="currentFilePath">当前文件目录</param>
        /// <returns>是否比较成功</returns>
        private bool CompareFile(string currentFilePath)
        {
            bool compareSuccess = false;

            string destFilePath = currentFilePath.Replace(leftFolder, rightFolder);
            FileInfo fi = new FileInfo(destFilePath);

            if (fi.Exists)
            {
                switch (fi.Extension.ToLower())
                {
                    case ".logical":
                        {
                            mapLocicalCompareForm = new MapLogicalCompare.MainForm();
                            mapLocicalCompareForm.CompareFile(currentFilePath, destFilePath);
                            mapLocicalCompareForm.WindowState = FormWindowState.Maximized;
                            mapLocicalCompareForm.ShowDialog();
                            mapLocicalCompareForm.Dispose();
                            break;
                        }
                    case ".tab":
                        {
                            if (tabCompareForm == null)
                            {
                                tabCompareForm = new ClientDiff.Form1();
                            }

                            tabCompareForm.CompareFile(currentFilePath, destFilePath);
                            tabCompareForm.WindowState = FormWindowState.Maximized;
                            //tabCompareForm.ShowDialog();

                            break;
                        }
                    case ".txt":
                        {
                            goto case ".tab";
                        }
                    default:
                        {
                            ViewDiffExternal(currentFilePath, destFilePath);
                            break;
                        }
                }

                compareSuccess = true;
            }

            return compareSuccess;
        }

        private void ViewDiffExternal(string currentFilePath, string destFilePath)
        {
            if (compareApplicationPath != "")
            {
                if (File.Exists(compareApplicationPath))
                {
                    try
                    {
                        System.Diagnostics.Process.Start(compareApplicationPath, string.Format("\"{0}\" \"{1}\"", currentFilePath, destFilePath));
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(string.Format("在运行程序{0}时产生异常：" + ex.Message, compareApplicationPath), "文件比较",
                                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }

                }
            }
            else
            {
                MessageBox.Show("请先配置默认的比对程序！", "文件比较",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                ConfigForm cForm = new ConfigForm(compareApplicationPath);

                if (cForm.ShowDialog() == DialogResult.OK)
                {
                    UpdateConfigInfo(cForm);

                    if (File.Exists(compareApplicationPath))
                    {
                        try
                        {
                            System.Diagnostics.Process.Start(compareApplicationPath, string.Format("{0} {1}", currentFilePath, destFilePath));
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(string.Format("在运行程序{0}时产生异常：" + ex.Message, compareApplicationPath), "文件比较",
                                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 获取文件的二进制数据
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>二进制数据</returns>
        private byte[] GetFileBytes(string filePath)
        {
            FileStream fs = new FileStream(filePath, FileMode.Open);
            byte[] buffer = new byte[fs.Length];
            fs.Read(buffer, 0, buffer.Length);
            fs.Close();

            return buffer;
        }

        /// <summary>
        /// 获取文件字符串
        /// </summary>
        /// <param name="filePath">文件路径</param>
        private string GetFileString(string filePath)
        {
            StreamReader sr = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
            string content = sr.ReadToEnd();
            sr.Close();

            return content;
        }

        /// <summary>
        /// 比较文件内容
        /// </summary>
        /// <param name="srcFilePath">源文件路径</param>
        /// <param name="destFilePath">目标文件路径</param>
        /// <returns>是否相等</returns>
        private bool CompareFileContent(string srcFilePath, string destFilePath)
        {
            bool equal = false;

            string[] tempArray = srcFilePath.Split(pathSplitArray, StringSplitOptions.RemoveEmptyEntries);
            string fileExtention = tempArray[tempArray.Length - 1];

            switch (fileExtention.ToLower())
            {
                case "tab":
                    {
                        if (compareTabFile)
                        {
                            string srcContent = GetFileString(srcFilePath);
                            string destContent = GetFileString(destFilePath);

                            if (srcContent == destContent)
                            {
                                equal = true;
                            }
                        }

                        break;
                    }
                case "txt":
                    goto case "tab";
                case "ini":
                    goto case "tab";
                case "lua":
                    goto case "tab";
                case "lh":
                    goto case "tab";
                case "ls":
                    goto case "tab";
                case "logical":
                    goto case "tab";
                default: // 二进制比较，不推荐
                    {
                        if (compareOtherFile)
                        {
                            FileInfo srcFileInfo = new FileInfo(srcFilePath);
                            FileInfo destFileInfo = new FileInfo(destFilePath);

                            if (srcFileInfo.Length == destFileInfo.Length) // 先比较文件大小，可以快速过滤掉一些不必具体比较的文件
                            {
                                byte[] srcContent = GetFileBytes(srcFilePath);
                                byte[] destContent = GetFileBytes(destFilePath);

                                if (RtlCompareMemory(Marshal.UnsafeAddrOfPinnedArrayElement(srcContent, 0),
                                                     Marshal.UnsafeAddrOfPinnedArrayElement(destContent, 0),
                                                     srcContent.Length)
                                    == srcContent.Length)
                                {
                                    equal = true;
                                }
                            }                            
                        }

                        break;
                    }
            }
           
            return equal;
        }

        /// <summary>
        /// 隐藏字符串相等的文件
        /// </summary>
        private void MarkSameFile()
        {
            foreach (ListViewItem listViewItem in leftView.Items)
            {
                string[] data = listViewItem.Tag as string[];

                if (data[0] == "File")
                {
                    string srcFilePath = data[1].ToLower();

                    if (srcFilePath.EndsWith(".ini") || srcFilePath.EndsWith(".txt") || srcFilePath.EndsWith(".tab")
                        || srcFilePath.EndsWith(".logical") || srcFilePath.EndsWith(".lua") || srcFilePath.EndsWith(".lh")
                        || srcFilePath.EndsWith(".ls"))
                    {
                        string destFilePath = data[1].Replace(leftFolder, rightFolder);

                        ListViewItem equalItem = null;

                        foreach (ListViewItem item in rightView.Items)
                        {
                            string[] equalData = item.Tag as string[];

                            if (destFilePath == equalData[1])
                            {
                                equalItem = item;
                                break;
                            }
                        }

                        if (equalItem != null)
                        {
                            if (!CompareFileContent(srcFilePath, destFilePath))
                            {
                                listViewItem.ForeColor = Color.Red;
                                equalItem.ForeColor = Color.Red;
                            }                            
                        }
                        else
                        {
                            listViewItem.ForeColor = Color.Red;                            
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 在ListView中显示文件夹的内容
        /// </summary>
        /// <param name="listView">ListView对象</param>
        /// <param name="folderPath">文件夹路径</param>
        private void ShowFolderContent(ListView listView, string folderPath)
        {
            DirectoryInfo directoryInfo = new DirectoryInfo(folderPath);

            listView.Items.Clear();

            if (directoryInfo.Parent != null)
            {
                ListViewItem backItem = new ListViewItem();
                backItem.Text = "向上";
                backItem.Tag = new string[] { "GoBack", "" };
                listView.Items.Add(backItem);
            }            

            foreach (DirectoryInfo di in directoryInfo.GetDirectories())
            {
                ListViewItem folderItem = new ListViewItem();
                folderItem.Text = di.Name;
                folderItem.SubItems.Add("");
                folderItem.SubItems.Add(di.LastWriteTime.ToString());
                folderItem.ImageIndex = iconManager.AddFolderIcon(di.FullName, IconType.Small);
                folderItem.Tag = new string[] { "Folder", di.FullName };
                listView.Items.Add(folderItem);
            }

            foreach (FileInfo fi in directoryInfo.GetFiles())
            {
                ListViewItem fileItem = new ListViewItem();
                fileItem.Text = fi.Name;
                fileItem.SubItems.Add(string.Format("{0}B", fi.Length));
                fileItem.SubItems.Add(fi.LastAccessTime.ToString());
                fileItem.ImageIndex = iconManager.AddFileIcon(fi.FullName, IconType.Small);
                fileItem.Tag = new string[] { "File", fi.FullName };
                listView.Items.Add(fileItem);
            }

            listView.Tag = folderPath;
        }

        /// <summary>
        /// 比较框体大小改变，需要动态调整分割线的位置
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void compareContainer_Resize(object sender, EventArgs e)
        {
            Size containerSize = compareContainer.Size;
            int splitDistance = (int)((containerSize.Width - 4) / 2);
            if (splitDistance > 0)
                compareContainer.SplitterDistance = splitDistance;
        }

        /// <summary>
        /// 双击源目录中的图标
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void leftView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (leftView.SelectedItems.Count > 0 && rightFolder != null)
            {
                ListViewItem listViewItem = leftView.SelectedItems[0];
                string[] itemData = listViewItem.Tag as string[];

                string newDir = string.Empty;

                bool bErrorNoSameSubDir = false;

                switch (itemData[0])
                {
                    case "Folder": // 文件夹
                        {
                            newDir = itemData[1];

                            if (OpenRelevateFolder(itemData[1]))
                            {
                                DirectoryInfo directoryInfo = new DirectoryInfo(itemData[1]);

                                leftView.Items.Clear();

                                ListViewItem backItem = new ListViewItem();
                                backItem.Text = "向上";
                                backItem.Tag = new string[] { "GoBack", "" };
                                leftView.Items.Add(backItem);

                                foreach (DirectoryInfo di in directoryInfo.GetDirectories())
                                {
                                    ListViewItem folderItem = new ListViewItem();
                                    folderItem.Text = di.Name;
                                    folderItem.SubItems.Add("");
                                    folderItem.SubItems.Add(di.LastWriteTime.ToString());
                                    folderItem.ImageIndex = iconManager.AddFolderIcon(di.FullName, IconType.Small);
                                    folderItem.Tag = new string[] { "Folder", di.FullName };
                                    leftView.Items.Add(folderItem);
                                }

                                foreach (FileInfo fi in directoryInfo.GetFiles())
                                {
                                    ListViewItem fileItem = new ListViewItem();
                                    fileItem.Text = fi.Name;
                                    fileItem.SubItems.Add(string.Format("{0}B", fi.Length));
                                    fileItem.SubItems.Add(fi.LastAccessTime.ToString());
                                    fileItem.ImageIndex = iconManager.AddFileIcon(fi.FullName, IconType.Small);
                                    fileItem.Tag = new string[] { "File", fi.FullName };
                                    leftView.Items.Add(fileItem);
                                }

                                leftView.Tag = itemData[1];
                            }
                            else
                            {
                                bErrorNoSameSubDir = true;
                                MessageBox.Show("没有在目标目录下找到匹配的文件夹！", "文件比较", MessageBoxButtons.OK, MessageBoxIcon.Information);
                            }                                                        

                            break;
                        }
                    case "File":
                        {
                            CompareFile(itemData[1]);

                            break;
                        }
                    case "GoBack":
                        {
                            DirectoryInfo directoryInfo = new DirectoryInfo(leftView.Tag as string);
                            directoryInfo = directoryInfo.Parent;
                            newDir = directoryInfo.FullName;

                            if (directoryInfo != null)
                            {
                                if (OpenRelevateFolder(directoryInfo.FullName))
                                {
                                    leftView.Items.Clear();

                                    if (leftView.Parent != null)
                                    {
                                        ListViewItem backItem = new ListViewItem();
                                        backItem.Text = "向上";
                                        backItem.Tag = new string[] { "GoBack", "" };
                                        leftView.Items.Add(backItem);
                                    }

                                    foreach (DirectoryInfo di in directoryInfo.GetDirectories())
                                    {
                                        ListViewItem folderItem = new ListViewItem();
                                        folderItem.Text = di.Name;
                                        folderItem.SubItems.Add("");
                                        folderItem.SubItems.Add(di.LastWriteTime.ToString());
                                        folderItem.ImageIndex = iconManager.AddFolderIcon(di.FullName, IconType.Small);
                                        folderItem.Tag = new string[] { "Folder", di.FullName };
                                        leftView.Items.Add(folderItem);
                                    }

                                    foreach (FileInfo fi in directoryInfo.GetFiles())
                                    {
                                        ListViewItem fileItem = new ListViewItem();
                                        fileItem.Text = fi.Name;
                                        fileItem.SubItems.Add(string.Format("{0}B", fi.Length));
                                        fileItem.SubItems.Add(fi.LastAccessTime.ToString());
                                        fileItem.ImageIndex = iconManager.AddFileIcon(fi.FullName, IconType.Small);
                                        fileItem.Tag = new string[] { "File", fi.FullName };
                                        leftView.Items.Add(fileItem);
                                    }

                                    leftView.Tag = directoryInfo.FullName;
                                }
                                else
                                {
                                    MessageBox.Show("没有在目标目录下找到匹配的文件夹！", "文件比较", MessageBoxButtons.OK, MessageBoxIcon.Information);
                                }
                            }                            

                            break;
                        }
                }

                if (newDir.Length != 0)
                {
                    UpdateCurrentDirName("left", newDir);
                    if (!bErrorNoSameSubDir)
                    {
                        string relative = newDir.Substring(leftFolder.Length);
                        UpdateCurrentDirName("right", rightFolder + relative);
                    }
                }

                MarkSameFile(); // 标记出相同的文件来
            }            
        }

        // 使用外部程序
        private void MenuItemExtern_Click(object sender, EventArgs e)
        {
            string leftFile = string.Empty;
            string rightFile = string.Empty;
            if (leftView.SelectedItems.Count > 0 && rightFolder != null)
            {
                ListViewItem listViewItem = leftView.SelectedItems[0];
                string[] itemData = listViewItem.Tag as string[];

                if (itemData[0] == "File")
                {
                    leftFile = itemData[1];
                    rightFile = leftFile.Replace(leftFolder, rightFolder);
                    FileInfo fi = new FileInfo(rightFile);
                    if (fi.Exists)
                    {
                        ViewDiffExternal(leftFile, rightFile);
                    }
                }
            }
        }

        // 查看差异
        private void MenuItemDiff_Click(object sender, EventArgs e)
        {
            leftView_MouseDoubleClick(null, null);
        }

        /// <summary>
        /// 双击目标视图中的图标
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void rightView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            MessageBox.Show("只能在左边的原始目录视图中打开文件和文件夹！", "文件比较", MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
        }

        /// <summary>
        /// 选择目录1
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOpenFolder1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderDialog = new FolderBrowserDialog();
            folderDialog.RootFolder = Environment.SpecialFolder.MyComputer;
            folderDialog.Description = "请选择客户端所在的目录";
            
            if (folderDialog.ShowDialog() == DialogResult.OK)
            {
                UpdateHistory(folderDialog.SelectedPath);
                OpenFolder("left", folderDialog.SelectedPath);

                //leftFolder = folderDialog.SelectedPath;
                //ShowFolderContent(leftView, leftFolder);
                //UpdateCurrentDirName("left", leftFolder);

                //if (rightFolder != null)
                //{
                //    MarkSameFile(); // 标记出相同的文件来
                //}
            }
        }
        /// <summary>
        /// 选择目录2
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bOpenFolder2_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderDialog = new FolderBrowserDialog();
            folderDialog.RootFolder = Environment.SpecialFolder.MyComputer;
            folderDialog.Description = "请选择客户端所在的目录";

            if (folderDialog.ShowDialog() == DialogResult.OK)
            {
                UpdateHistory(folderDialog.SelectedPath);
                OpenFolder("right", folderDialog.SelectedPath);

                //rightFolder = folderDialog.SelectedPath;
                //ShowFolderContent(rightView, rightFolder);
                //UpdateCurrentDirName("right", rightFolder);

                //if (leftFolder != null)
                //{
                //    MarkSameFile(); // 标记出相同的文件来
                //}
            }
        }
        private void OpenFolder(string side, string target)
        {
            if (side == "left")
            {
                leftFolder = target;
                ShowFolderContent(leftView, leftFolder);
                UpdateCurrentDirName("left", leftFolder);

                if (rightFolder != null)
                {
                    MarkSameFile(); // 标记出相同的文件来
                }
            }
            else if (side == "right")
            {
                rightFolder = target;
                ShowFolderContent(rightView, rightFolder);
                UpdateCurrentDirName("right", rightFolder);

                if (leftFolder != null)
                {
                    MarkSameFile(); // 标记出相同的文件来
                }
            }
        }
        private void UpdateCurrentDirName(string side, string dir)
        {
            if (side == "left")
                textPathLeft.Text = dir;
            if (side == "right")
                textPathRight.Text = dir;
        }



        /// <summary>
        /// 配置程序参数
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bSet_Click(object sender, EventArgs e)
        {
            ConfigForm cForm = new ConfigForm(compareApplicationPath);

            if (cForm.ShowDialog() == DialogResult.OK)
            {
                UpdateConfigInfo(cForm);
            }
        }

        private void buttonItem1_Click(object sender, EventArgs e) // 复制差异文件
        {
            if (leftFolder == null || rightFolder == null)
            {
                MessageBox.Show("请先选择源目录与目标目录后再使用此功能。");
                return;
            }

            string targetSaveDir = string.Empty;

            FolderBrowserDialog folderDialog = new FolderBrowserDialog();
            folderDialog.RootFolder = Environment.SpecialFolder.MyComputer;
            folderDialog.Description = "请选择一个目录以保存差异文件";
            if (folderDialog.ShowDialog() == DialogResult.OK)
            {
                targetSaveDir = folderDialog.SelectedPath;
                if (targetSaveDir.Length == 0)
                    return;
            }

            Thread newThreat = new Thread(new ParameterizedThreadStart(BeginCompare));
            ThreadParams param = new ThreadParams();
            param.CurPathL = leftFolder;
            param.CurPathR = rightFolder;
            param.clientL = leftFolder;
            param.clientR = rightFolder;
            param.target = targetSaveDir;
            newThreat.Start(param);
        }

        private void BeginCompare(object threadParam)
        {
            //CopyDiffFiles(leftFolder, rightFolder, leftFolder, rightFolder, targetSaveDir);
            ThreadParams param = (ThreadParams)threadParam;
            CopyDiffFiles(param.CurPathL, param.CurPathR, param.clientL, param.clientR, param.target);
            MessageBox.Show("完成。");
        }

        private void CopyDiffFiles(string CurPathL, string CurPathR, string clientL, string clientR, string target)
        {
            DirectoryInfo leftDI = new DirectoryInfo(CurPathL);

            foreach (FileInfo fi1 in leftDI.GetFiles())
            {
                if (FileExtensionNeedCompare(fi1.Extension))
                {
                    string filename2 = fi1.FullName.Replace(clientL, clientR);
                    FileInfo fi2 = new FileInfo(filename2);
                    if (fi2.Exists)
                    {
                        string content1 = FileFolderHelper.FileToString(fi1.FullName);
                        string content2 = FileFolderHelper.FileToString(filename2);
                        if (content1 != content2)
                        {
                            string targetL = fi1.FullName.Replace(clientL, target + "\\左");
                            string targetR = fi2.FullName.Replace(clientR, target + "\\右");

                            string targetDIRL = targetL.Substring(0, targetL.LastIndexOf('\\'));
                            string targetDIRR = targetR.Substring(0, targetR.LastIndexOf('\\'));

                            if (!Directory.Exists(targetDIRL))
                                Directory.CreateDirectory(targetDIRL);
                            if (!Directory.Exists(targetDIRR))
                                Directory.CreateDirectory(targetDIRR);

                            fi1.CopyTo(targetL);
                            fi2.CopyTo(targetR);

                            copyedFileCount++;
                            string strStatus = string.Format("复制文件数：{0}   当前处理文件：{1}", copyedFileCount, fi1.FullName.Replace(clientL, string.Empty));
                            this.Invoke(new ThreadUIDelegate(DelegateMethod), new object[] { strStatus });
                            //this.labelStatus.Text = strStatus;
                        }
                    }
                }
            }

            foreach (DirectoryInfo di1 in leftDI.GetDirectories())
            {
                if (!DirectoryNeedCompare(di1.FullName))
                    continue;

                DirectoryInfo di2 = new DirectoryInfo(di1.FullName.Replace(clientL, clientR));
                if (di2.Exists)
                {
                    CopyDiffFiles(di1.FullName, di2.FullName, clientL, clientR, target);
                }
            }
        }

        private void DelegateMethod(string labeltext)
        {
            this.labelStatus.Text = labeltext;
        }

        private bool FileExtensionNeedCompare(string ext)
        {
            ext = ext.ToLower();
            if (ext == ".ini" || ext == ".txt" || ext == ".tab" || ext == ".lua" || 
                ext == ".lh"  || ext == ".ls"  || ext == ".bat" || ext == ".logical")
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private bool DirectoryNeedCompare(string dirName)
        {
            if (dirName.ToLower() == ".svn")
                return false;
            else
                return true;
        }

        private DiffColInfo FindDiffColInfo(string filename)
        {
            return (DiffColInfo)m_diffColInfo[filename.ToLower()];
        }

        private bool AnynalizeTabKeysAndNameField(string filename, ref string keys, ref string names)
        {
            string content = FileFolderHelper.FileToString(filename);
            string[] lines = content.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            string[] colnames = lines[0].Split(new char[] { '\t' });
            keys = colnames[0];
            int keyindex = 0;
            //for (int i = colnames.Length - 1; i >= 0; i--)
            //{
            //    string col = colnames[i].ToLower();
            //    if (col.Contains("id"))
            //    {
            //        keys = colnames[i];
            //        keyindex = i;
            //    }
            //}

            string[] firstRowCols = lines[1].Split(new char[] { '\t' });
            int dummi = 0;
            return Int32.TryParse(firstRowCols[keyindex], out dummi);
        }

        private void GenerateTabDiffOutput(string dir, string outFolder)
        {
            string dir1 = Path.Combine(leftFolder, dir);

            DirectoryInfo leftDirInfo = new DirectoryInfo(dir1);
            foreach (FileInfo fi in leftDirInfo.GetFiles())
            {
                if (fi.Extension.ToLower() == ".tab" || fi.Extension.ToLower() == ".txt")
                {
                    string rightFileName = fi.FullName.Replace(leftFolder, rightFolder);
                    if (!File.Exists(rightFileName))
                        continue;

                    if (FileFolderHelper.FileToString(fi.FullName) == FileFolderHelper.FileToString(rightFileName))
                        continue;

                    DiffColInfo info = FindDiffColInfo(fi.Name);

                    string postfix = "tab";
                    string keys = string.Empty;
                    string namefield = string.Empty;
                    string rowcondition = string.Empty;
                    string content = string.Empty;
                    string outputbasic = string.Empty;
                    List<string> diffCols1 = new List<string>();
                    List<string> diffCols2 = new List<string>();

                    if (info != null)
                    {
                        keys = info.keys;
                        namefield = info.displays;
                        if (keys == "$" || namefield == "$") // ignore this file
                            continue;
                    }
                    else
                    {
                        if (!AnynalizeTabKeysAndNameField(fi.FullName, ref keys, ref namefield))
                            continue;
                    }

                    // try here
                    try
                    {
                        DiffGenerator.DiffGen(fi.FullName, rightFileName, postfix, keys, namefield, rowcondition, ref content, ref outputbasic, ref diffCols1, ref diffCols2);
                    }
                    catch (System.Exception e)
                    {
                        string strThisLog = string.Format("文件{0}的差异结果生成有误：{1}。\r\n", fi.FullName, e.Message);
                        m_diffLog += strThisLog;
                    }

                    string destFilename = fi.FullName.Replace(leftFolder, outFolder);
                    FileFolderHelper.CreatePath(destFilename);
                    FileFolderHelper.StringToFile(content, destFilename);
                }
            }

            foreach (DirectoryInfo di in leftDirInfo.GetDirectories())
            {
                if (di.Name.ToLower().Contains("svn"))
                    continue;

                GenerateTabDiffOutput(di.FullName, outFolder);
            }
        }

        private void buttonGenAllDiff_Click(object sender, EventArgs e) // 批量生成差异结果
        {
            string outFolder = FileFolderHelper.BrowserFolder("请选择输出路径。");
            if (outFolder.Length == 0)
            {
                MessageBox.Show("你取消了此次命令。");
                return;
            }

            string strIniFile = Application.StartupPath + "/config_public.ini";
            StringBuilder sb = new StringBuilder(255);
            FileFolderHelper.ReadIniFile("DiffDirList", "Count", "", sb, 255, strIniFile);
            int pathCount = Convert.ToInt32(sb.ToString());
            string[] paths = new string[pathCount];

            for (int i = 1; i <= pathCount; i++)
            {
                FileFolderHelper.ReadIniFile("DiffDirList", "Dir" + i.ToString(), "", sb, 255, strIniFile);
                paths[i - 1] = sb.ToString();
            }

            m_diffColInfo = new Hashtable();
            FileFolderHelper.ReadIniFile("DiffColumns", "Count", "", sb, 255, strIniFile);
            int diffColDescCount = Convert.ToInt32(sb.ToString());
            for (int i = 1; i <= diffColDescCount; i++)
            {
                FileFolderHelper.ReadIniFile("DiffColumns", "Table" + i.ToString(), "", sb, 255, strIniFile);
                string tblname = sb.ToString();
                DiffColInfo dci = new DiffColInfo();
                FileFolderHelper.ReadIniFile("DiffColumns", "Keys" + i.ToString(), "", sb, 255, strIniFile);
                dci.keys = sb.ToString();
                FileFolderHelper.ReadIniFile("DiffColumns", "Display" + i.ToString(), "", sb, 255, strIniFile);
                dci.displays = sb.ToString();
                m_diffColInfo.Add(tblname.ToLower(), dci);
            }

            m_diffLog = string.Empty;
            foreach (string dir in paths)
            {
                GenerateTabDiffOutput(dir, outFolder);
            }
            FileFolderHelper.StringToFile(m_diffLog, Path.Combine(Application.StartupPath, "diff.log"));
        }

        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F1)
            {
                bOpenFolder1_Click(null, null);
            }
            else if (e.KeyCode == Keys.F2)
            {
                bOpenFolder2_Click(null, null);
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void bFile_Click(object sender, EventArgs e)
        {

        }


    };

    public class DiffColInfo
    {
        public string keys;
        public string displays;
    };

    public class ThreadParams
    {
        public string CurPathL;
        public string CurPathR;
        public string clientL;
        public string clientR;
        public string target;
    };
}