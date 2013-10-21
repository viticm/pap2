using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Collections;
using System.Text.RegularExpressions;

namespace AnimationAutoFill
{
    public partial class MainForm : Form
    {
        private string clientFolder; // 客户端文件夹
        private string dataFilePath; // 数据文件路径
        private string relevatePath; // 相对路径
        private string absolutePath; // 绝对路径
        private string lastBoneNumber; // 上一个模型分类

        private string voltAnimation;
        private string wardoffAnimation;
        private string vertigoAnimation;
        private string beatbackAnimation;
        private string glissadeAnimation;
        private string knockdownStandupAnimation;
        private string knockdownAnimation;
        private string standupAnimation;
        private string onGroundAnimation;
        private string onGroundUnderAttackAnimation;
        private string time;
        private string deathAnimation;
        private string animationGroup;
        private string knowndownAnimationName;

        private bool requestUpdate = false; // 是否需要更新
        private int lineIndex = 0; // 当前行号
        private int groupIndex = 0; // 分组编号 
        private StringBuilder logText = new StringBuilder(); // 日志文本
        private DataTable boneToActionTable; // 数据表
        private UpdateResultForm updateResultForm = new UpdateResultForm(); // 更新结果窗体

        /// <summary>
        /// 构造函数
        /// </summary>
        public MainForm()
        {
            InitializeComponent();
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            StreamReader reader;
            string filePath = Path.Combine(Application.StartupPath, "AutoFillConfig.ini");
            string content = null;

            if (File.Exists(filePath))
            {
                try
                {                                        
                    reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                    content = reader.ReadToEnd();
                    reader.Close();
                }
                catch (IOException ex)
                {
                    MessageBox.Show(string.Format("在读取配置文件时产生异常：{0}", ex.ToString()), "读取配置文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            } 
            
            if (content != null)
            {
                IniHandler.IniStructure m_inis = new IniHandler.IniStructure();
                m_inis = IniHandler.IniStructure.ReadIniWithContent(content);
                clientFolder = m_inis.GetValue("General", "ClientFolder");
                dataFilePath = m_inis.GetValue("General", "DataFilePath");
                textBoxX1.Text = clientFolder;
                textBoxX2.Text = dataFilePath;
            }
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        private DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }
            }

            return table;
        }

        /// <summary>
        /// 初始化相对路径
        /// </summary>
        /// <param name="boneNumber">骨骼类型</param>
        private void InitRelevatePath(string boneNumber)
        {
            string firstLetter = boneNumber.Substring(0, 1);

            switch (firstLetter)
            {
                case "F": // player动作
                    {
                        relevatePath = string.Format(@"data\source\player\{0}\动作\", boneNumber);

                        break;
                    }
                case "M": // player动作
                    {
                        relevatePath = string.Format(@"data\source\player\{0}\动作\", boneNumber);

                        break;
                    }
                default: // npc动作
                    {
                        relevatePath = string.Format(@"data\source\NPC_source\{0}\动作\", boneNumber);

                        break;
                    }
            }

            absolutePath = Path.Combine(clientFolder, relevatePath);
        }

        /// <summary>
        /// 获取动作信息哈希表
        /// </summary>
        /// <param name="boneNumber">模型分类</param>
        /// <returns>动作信息哈希表</returns>
        private Hashtable GetAnimationInfoTable(string boneNumber)
        {
            Hashtable infoTable = new Hashtable();

            InitRelevatePath(boneNumber);
            DirectoryInfo di = new DirectoryInfo(absolutePath);

            if (di.Exists)
            {
                FileInfo[] files = di.GetFiles();
                string animationClass;
                string fileName;

                foreach(FileInfo fi in files)
                {
                    if (fi.Extension == ".ani" || fi.Extension == ".tani")
                    {
                        animationClass = GetAnimationClass(fi.Name, boneNumber);
                        fileName = fi.Name.Replace(".ani", "").Replace(".tani", "");

                        if (animationClass != null)
                        {
                            List<string> fileList;

                            if (infoTable[animationClass] == null)
                            {
                                fileList = new List<string>();
                                fileList.Add(fileName);
                                infoTable[animationClass] = fileList;
                            }
                            else
                            {
                                fileList = infoTable[animationClass] as List<string>;
                                fileList.Add(fileName);
                            }
                        }                        
                    }                    
                }
            }
            else
            {
                logText.Append(string.Format("Error：路径{0}不存在！\r\n", absolutePath));
            }

            return infoTable;
        }

        /// <summary>
        /// 获取动作分类
        /// </summary>
        /// <param name="animationString">动作字符串</param>
        /// <param name="boneNumber">模型分类</param>
        /// <returns>动作分类</returns>
        private string GetAnimationClass(string animationString, string boneNumber)
        {
            string animationClass = null;

            string regexPattern = string.Format(@"{0}_(?<SectionName>([a-z]|[A-Z])*)0", boneNumber);
            Regex r = new Regex(regexPattern); 
            MatchCollection matches = r.Matches(animationString);

            if (matches.Count > 0)
            {
                animationClass = matches[0].Result("${SectionName}");
            }            

            return animationClass;
        }

        /// <summary>
        /// 获取动作编号
        /// </summary>
        /// <param name="animationString">动作字符串</param>
        /// <param name="animationType">动作类型</param>
        /// <returns>动作编号</returns>
        private int GetAnimationIndex(string animationString, string animationType)
        {
            int animationIndex = 0;
            string animationIndexString = null;

            string regexPattern = string.Format(@"_{0}0(?<SectionName>\d*)", animationType);
            Regex r = new Regex(regexPattern);
            MatchCollection matches = r.Matches(animationString);

            if (matches.Count > 0)
            {
                animationIndexString = matches[0].Result("${SectionName}");
                animationIndex = int.Parse(animationIndexString);
            }

            return animationIndex;
        }

        /// <summary>
        /// 开始填充
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(clientFolder) && !string.IsNullOrEmpty(dataFilePath))
            {
                StreamReader reader;
                string filePath = dataFilePath;
                string content;
                string[] animationArray;

                // 初始化文件
                if (File.Exists(filePath))
                {
                    reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                    content = reader.ReadToEnd();
                    reader.Close();

                    boneToActionTable = CreateDataTable(content);
                    updateResultForm.Reset();

                    lineIndex = 0;
                    for (int i = 0; i < boneToActionTable.Rows.Count; i++)
                    {
                        lineIndex = i + 2;
                        DataRow dataRow = boneToActionTable.Rows[i];
                        string boneNumber = dataRow["BoneNumber"] as string;

                        if (string.IsNullOrEmpty(boneNumber))
                        {
                            logText.Append(string.Format("Error：第{0}数据行的BoneNumber为空！", lineIndex.ToString()));
                        }
                        else
                        {
                            logText.Append(string.Format("开始填充第{0}数据行...", lineIndex.ToString()));

                            Hashtable infoTable = GetAnimationInfoTable(boneNumber);

                            voltAnimation = dataRow["Volt"] as string;
                            if (string.IsNullOrEmpty(voltAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "sb");
                            }

                            wardoffAnimation = dataRow["Wardoff"] as string;
                            if (string.IsNullOrEmpty(wardoffAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "gd");
                            }

                            vertigoAnimation = dataRow["Vertigo"] as string;
                            if (string.IsNullOrEmpty(vertigoAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "xy");
                            }

                            beatbackAnimation = dataRow["Beatback"] as string;
                            if (string.IsNullOrEmpty(beatbackAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "jt");
                            }
                            
                            glissadeAnimation = dataRow["Glissade"] as string;
                            if (string.IsNullOrEmpty(glissadeAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "th");
                            }

                            knockdownStandupAnimation = dataRow["Knockdown_Standup"] as string;
                            knockdownAnimation = null;
                            standupAnimation = null;

                            if (string.IsNullOrEmpty(knockdownStandupAnimation))
                            {
                                animationArray = knockdownStandupAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                if (animationArray.Length > 0)
                                {
                                    animationArray = animationArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                                    if (animationArray.Length == 2)
                                    {
                                        knockdownAnimation = animationArray[0];
                                        standupAnimation = animationArray[1];

                                        FillAnimation(infoTable, dataRow, "jd");
                                        FillAnimation(infoTable, dataRow, "pq");
                                    }
                                }
                            }                            

                            onGroundAnimation = dataRow["OnGround"] as string;
                            if (string.IsNullOrEmpty(onGroundAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "jdz");
                            }

                            onGroundUnderAttackAnimation = dataRow["OnGroundUnderAttack"] as string;
                            if (string.IsNullOrEmpty(onGroundUnderAttackAnimation))
                            {
                                FillAnimation(infoTable, dataRow, "jdzbat");
                            }                           

                            time = dataRow["Time"] as string;
                            if (string.IsNullOrEmpty(time))
                            {
                                dataRow["Time"] = "24";
                                updateResultForm.AppendLog(lineIndex.ToString(), "Time", "24");
                                requestUpdate = true;
                            }

                            knockdownStandupAnimation = dataRow["Knockdown_Standup"] as string;                            
                            onGroundAnimation = dataRow["OnGround"] as string;
                            onGroundUnderAttackAnimation = dataRow["OnGroundUnderAttack"] as string;                           
                            time = dataRow["Time"] as string;
                            deathAnimation = dataRow["Death"] as string;
                            animationGroup = dataRow["Knockdown_OnGround_OnGroundUnderAttack_Standup_Death_Time"] as string;

                            if (string.IsNullOrEmpty(animationGroup) && !string.IsNullOrEmpty(knockdownStandupAnimation) && !string.IsNullOrEmpty(onGroundAnimation) &&
                                !string.IsNullOrEmpty(onGroundUnderAttackAnimation) &&!string.IsNullOrEmpty(time) && 
                                !string.IsNullOrEmpty(deathAnimation))
                            {
                                animationArray = knockdownStandupAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                if (animationArray.Length > 0)
                                {
                                    animationArray = animationArray[0].Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                                    if (animationArray.Length == 2)
                                    {
                                        knockdownAnimation = animationArray[0];
                                        standupAnimation = animationArray[1];

                                        animationArray = deathAnimation.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                                        deathAnimation = animationArray[0];
                                        animationGroup = string.Format("{0},{1},{2},{3},{4},{5}", knockdownAnimation, onGroundAnimation, onGroundUnderAttackAnimation,
                                                                       standupAnimation, deathAnimation, time);
                                        dataRow["Knockdown_OnGround_OnGroundUnderAttack_Standup_Death_Time"] = animationGroup;
                                        updateResultForm.AppendLog(lineIndex.ToString(), "Knockdown_OnGround_OnGroundUnderAttack_Standup_Death_Time", animationGroup);
                                        requestUpdate = true;
                                    }
                                }                                
                            }

                            if (lastBoneNumber == boneNumber || lastBoneNumber == null)
                            {
                                groupIndex++;
                            }
                            else
                            {
                                lastBoneNumber = boneNumber;
                                groupIndex = 0;                                                                
                            }
                        }
                    }

                    if (requestUpdate) // 需要更新
                    {
                        SaveDataTableToFile(boneToActionTable);
                    }
                    else
                    {
                        MessageBox.Show("表内容已经是最新！", "动作自动填充", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }                    
                }
                else
                {
                    MessageBox.Show(string.Format("文件{0}不存在！", filePath), "加载数据文件",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("输入信息不完整！", "动作自动填充", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 保存数据表内容
        /// </summary>
        /// <param name="dataTable">数据表</param>
        private void SaveDataTableToFile(DataTable dataTable)
        {
            StringBuilder content = new StringBuilder();
            StringBuilder firstLine = new StringBuilder();            

            foreach(DataColumn dataColumn in dataTable.Columns)
            {
                firstLine.Append(string.Format("{0}\t", dataColumn.ColumnName));
            }            

            content.Append(string.Format("{0}\r\n", firstLine.Remove(firstLine.Length - 1, 1).ToString()));

            foreach(DataRow dataRow in dataTable.Rows)
            {
                StringBuilder dataLine = new StringBuilder();

                for (int i = 0; i < dataTable.Columns.Count; i++)
                {
                    dataLine.Append(string.Format("{0}\t", dataRow[i].ToString()));
                }

                content.Append(string.Format("{0}\r\n", dataLine.Remove(dataLine.Length - 1, 1).ToString()));
            }

            bool saveSuccess = SaveDataToFile(dataFilePath, content.ToString());
            if (saveSuccess)
            {
                MessageBox.Show("文件保存成功！", "保存文件", MessageBoxButtons.OK, MessageBoxIcon.Information);
                updateResultForm.ShowDialog();
            }
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">文本内容</param>
        /// <return>是否保存成功</return>
        private bool SaveDataToFile(string fileName, string content)
        {
            bool success = false;

            try
            {
                string filePath = Path.GetDirectoryName(fileName);
                FileInfo fi = new FileInfo(fileName);                
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if (!Directory.Exists(filePath)) // 目录不存在
                    {
                        Directory.CreateDirectory(filePath); // 先创建目录，再创建文件
                    }

                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {                    
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                sw.Write(content);
                sw.Close();
                success = true;
            }
            catch (IOException ex)
            {
                MessageBox.Show(string.Format("写入文件{0}时产生异常：{1}", fileName, ex.Message));
            }

            return success;
        }

        /// <summary>
        /// 填充动作
        /// </summary>
        private void FillAnimation(Hashtable infoTable, DataRow dataRow, string animationType)
        {
            string fieldName = null;

            switch(animationType)
            {
                case "sb": // 闪避
                    {
                        fieldName = "Volt";
                        break;
                    }
                case "gd": // 格挡
                    {
                        fieldName = "Wardoff";
                        break;
                    }
                case "xy": // 眩晕
                    {
                        fieldName = "Vertigo";
                        break;
                    }
                case "jt": // 击退
                    {
                        fieldName = "Beatback";
                        break;
                    }
                case "th": // 停滑
                    {
                        fieldName = "Glissade";
                        break;
                    }
                case "jd": // 击倒
                    {
                        fieldName = "Knockdown";
                        break;
                    }
                case "jdz": // 击倒中
                    {
                        fieldName = "OnGround";
                        break;
                    }
                case "jdbat": // 击倒中被击
                    {
                        fieldName = "OnGroundUnderAttack";
                        break;
                    }
                case "pq": // 爬起
                    {
                        fieldName = "Standup";
                        break;
                    }
            }

            if (fieldName != null)
            {
                List<string> fileList = infoTable[animationType] as List<string>;

                if (fileList != null)
                {
                    fileList.Sort();
                    string animationName = null;

                    if (groupIndex < fileList.Count)
                    {
                        animationName = fileList[groupIndex];
                    }
                    else
                    {
                        animationName = fileList[0];
                    }

                    switch(fieldName)
                    {
                        case "jd":
                            {
                                knowndownAnimationName = animationName;
                                break;
                            }
                        case "pq":
                            {
                                fieldName = "Knockdown_Standup";
                                dataRow[fieldName] = string.Format("{0},{1}", knowndownAnimationName, animationName);
                                break;
                            }
                        default:
                            {
                                dataRow[fieldName] = animationName;
                                break;
                            }
                    }
                    
                    updateResultForm.AppendLog(lineIndex.ToString(), fieldName, animationName);
                    requestUpdate = true;
                }
            }
        }

        /// <summary>
        /// 退出程序
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
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
            folderBroserDialog.Description = "请选择客户端所在的目录";

            if (folderBroserDialog.ShowDialog() == DialogResult.OK)
            {
                clientFolder = folderBroserDialog.SelectedPath;
                textBoxX1.Text = clientFolder;
            }
        }

        /// <summary>
        /// 浏览文件
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (dataFilePath != null)
            {
                openFileDialog.InitialDirectory = dataFilePath;
            }
            openFileDialog.Title = "请选择要打开的数据文件";
            openFileDialog.Filter="TAB文件|*.tab";
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                dataFilePath = openFileDialog.FileName;
                textBoxX2.Text = dataFilePath;
            }
        }

        /// <summary>
        /// 窗体关闭中
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!string.IsNullOrEmpty(clientFolder) && !string.IsNullOrEmpty(dataFilePath))
            {
                string fileName = Path.Combine(Application.StartupPath, "AutoFillConfig.ini");
                string content = string.Format("[General]\r\nClientFolder={0}\r\nDataFilePath={1}\r\n", clientFolder, dataFilePath);
                SaveDataToFile(fileName, content);
            }            
        }
    }
}