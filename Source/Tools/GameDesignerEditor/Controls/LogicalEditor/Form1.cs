using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using MWControlSuite;

namespace LogicalEditor
{
    public partial class Form1 : Form
    {
        [DllImport("kernel32.dll")]
        static extern void ExitProcess(uint exitCode);

        public static string m_BasePath = string.Empty;
        public static string m_MapsPath = string.Empty;
        private ArrayList m_LogicIniFiles = new ArrayList();
        public Preview previewform = null;
        public PropertyGrid Pg { get { return this.propertyGrid1; } }
        
        public string m_curMapName = string.Empty;
        // 以下 4 个全部从 minimap 的 config.ini 读
        public int curMapLogicWidthTotal = -1;
        public int curMapLogicHeightTotal = -1;
        public float curLogicStartX = 0.0f;
        public float curLogicStartY = 0.0f;

        public Hashtable htScenePtr = new Hashtable(); // IEKG3DScene类型的指针们
        public Hashtable maps;

        private string[] m_types;
        public Hashtable htObjScenePosIndices = null;
        public Hashtable htObjLogicNXIndices = null;
        public Hashtable htObjLogicDirIndices = null;
        public Hashtable htObjLogicRotationIndices = null;


        public Form1()
        {
            try
            {
                InitializeComponent();
                this.Refresh();
                InitVars();
                InitSettings();
                InitData();
                MyInitControls();
                Init3DEngine();
                Init2DPreview();
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void MyInitControls()
        {
            // save 按钮的初始化
            DevComponents.DotNetBar.ButtonItem itemIniOnly = new DevComponents.DotNetBar.ButtonItem("IniOnly", "只保存到场景编辑器");
            itemIniOnly.Click += new EventHandler(this.buttonSave_Click);
            this.buttonSave.SubItems.Add(itemIniOnly);

            DevComponents.DotNetBar.ButtonItem itemLogicAlso = new DevComponents.DotNetBar.ButtonItem("LogicAlso", "同时保存场景编辑器和逻辑数据");
            itemLogicAlso.Click += new EventHandler(this.buttonSave_Click);
            this.buttonSave.SubItems.Add(itemLogicAlso);
        }

        private void Init2DPreview()
        {
            previewform = new Preview(this);
        }

        private void Init3DEngine()
        {
            try
            {
                //Helper.RegComDll("atlax.dll");
                Program.Engine = new atlaxLib.Test3DEngine();
            }
            catch (Exception ex)
            {
                MessageBox.Show("atlax初始化失败: " + ex.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            Program.Engine.Init((int)this.Handle, m_BasePath, Application.StartupPath);
        }

        private void InitData()
        {
            maps = new Hashtable();
            ScanAllLogicalIniFiles();
            CheckFilesCount();
        }

        private void InitVars()
        {
            m_types = new string[] {
                "MAIN", "NPC", "Doodad", "NPCRefreshPointSet",
                "NPCRefreshPoint", "NPCRefreshNPC", "DoodadRefreshPointSet", "DoodadRefreshPoint",
                "DoodadRefreshDoodad", "AIGroup", "AIGroupNPC", "WayPointSet",
                "WayPoint", "LogicalPoly", "LogicalPolyVertex"
                };

            // vPosition所在index
            htObjScenePosIndices = new Hashtable();
            htObjScenePosIndices.Add("NPC", 12);
            htObjScenePosIndices.Add("Doodad", 10);
            htObjScenePosIndices.Add("NPCRefreshPoint", 0);
            htObjScenePosIndices.Add("DoodadRefreshPoint", 0);
            htObjScenePosIndices.Add("WayPoint", 0);
            htObjScenePosIndices.Add("LogicalPolyVertex", 0);

            // nX所在index
            htObjLogicNXIndices = new Hashtable();
            htObjLogicNXIndices.Add("NPC", 4);
            htObjLogicNXIndices.Add("Doodad", 4);
            htObjLogicNXIndices.Add("NPCRefreshPoint", 2);
            htObjLogicNXIndices.Add("DoodadRefreshPoint", 2);
            htObjLogicNXIndices.Add("WayPoint", 2);

            // nDirection所在index
            htObjLogicDirIndices = new Hashtable();
            htObjLogicDirIndices.Add("NPC", 8);
            htObjLogicDirIndices.Add("Doodad", 8);
            htObjLogicDirIndices.Add("NPCRefreshPoint", 5);
            htObjLogicDirIndices.Add("DoodadRefreshPoint", 5);
            htObjLogicDirIndices.Add("WayPoint", 6);

            // vRotation所在index
            htObjLogicRotationIndices = new Hashtable();
            htObjLogicRotationIndices.Add("NPC", 13);
            htObjLogicRotationIndices.Add("Doodad", 11);
            htObjLogicRotationIndices.Add("NPCRefreshPoint", 1);
            htObjLogicRotationIndices.Add("DoodadRefreshPoint", 1);
            htObjLogicRotationIndices.Add("WayPoint", 1);

        }

        private void InitSettings()
        {
            StringBuilder sb = new StringBuilder(255);
            Helper.ReadIniFile("General", "RootDir", string.Empty, sb, 255, Path.Combine(Application.StartupPath, "GameDesingerTools.ini"));
            m_BasePath = sb.ToString();
            m_MapsPath = m_BasePath + "/data/source/maps";
        }

        private void loadButtonItem_Click(object sender, EventArgs e)
        {
            // 判断选中哪张地图
            DevComponents.DotNetBar.ButtonItem selectedItem = sender as DevComponents.DotNetBar.ButtonItem;
            if (selectedItem != null)
            {
                string selectedMapName = selectedItem.Name;
                string selectedFullFileName = selectedItem.Tag.ToString();

                // 显示信息：
                this.labelLoading.Text = "Loading " + selectedMapName + "...";
                this.labelLoading.Visible = true;
                this.Refresh();

                // 读取一张地图，不会重复
                if (!htScenePtr.Contains(selectedMapName))
                {
                    int thisScenePtr = 0;
                    string strMap = string.Format("{0}/data/source/maps/{1}/{1}.Map", m_BasePath, selectedMapName);
                    Program.Engine.LoadOneMap_NoOutputWindow(strMap, out thisScenePtr);
                    htScenePtr.Add(selectedMapName, thisScenePtr);

                    // 会加入到maps里
                    DoImportOneTarget(selectedFullFileName, selectedMapName);
                    LoadTree(selectedMapName);
                }

                // 显示信息
                this.labelLoading.Visible = false;
                this.Refresh();
            }
        }

        private void buttonX3_Click(object sender, EventArgs e) // load
        {
            DevComponents.DotNetBar.ButtonX butt = sender as DevComponents.DotNetBar.ButtonX;
            butt.Enabled = false;

            this.buttonOpenMap.ContextMenuStrip.Items.Clear();
            foreach (string s in m_LogicIniFiles)
            {
                string postfix = ".Map.Logical";
                int iStart = s.LastIndexOfAny(new char[] { '\\', '/' });
                string mapname = s.Substring(iStart + 1, s.Length - iStart - postfix.Length - 1);

                ToolStripItem addedItem = this.buttonOpenMap.ContextMenuStrip.Items.Add(mapname);
                addedItem.Tag = s; // full file name
            }
            this.buttonOpenMap.ContextMenuStrip.Show(this.buttonOpenMap, (e as MouseEventArgs).Location);
            
            butt.Enabled = true;
        }

        private void buttonSave_Click(object sender, EventArgs e) // save
        {
            // 场景编辑器 ini 保存
            int count = SaveIniFiles();
            this.Pg.Refresh();

            // 如果是直接点按钮的话，推出
            DevComponents.DotNetBar.ButtonItem item = sender as DevComponents.DotNetBar.ButtonItem;
            if (item == null) return;

            // split指定要保存逻辑
            if (item.Name == "LogicAlso") // 逻辑文件保存
            {
                Program.Engine.ShowProgressForm(1);
                foreach (DictionaryEntry entry in maps)
                {
                    LO_Map map = entry.Value as LO_Map;
                    if (map.logicalChanged)
                    {
                        int selectedScenePtr = (int)htScenePtr[map.name];
                        Program.Engine.SaveLogicScene(map.filename, selectedScenePtr);
                        map.logicalChanged = false;
                    }
                }

                Program.Engine.ShowProgressForm(0);
            }

            // report
            string str = string.Format("成功保存了 {0} 个对象。", count);
            MessageBox.Show(str);
        }

        private void tree1_AfterSelNodesAdd(object sender, MWPropertyEventArgs e)
        {
            TreeNode node = e.Updated as TreeNode;

            // 显示图片
            ShowMapContainSelectObj(node);

            // 检查是否大分类结点
            if (node.Tag is string)
            {
                return;
            }

            LogicObj lobj = node.Tag as LogicObj;
            if (lobj == null) // 添加一组
            {
                LogicObj onechild = node.Nodes[0].Tag as LogicObj;
                if (onechild != null)
                {
                    foreach (TreeNode n in node.Nodes)
                    {
                        LogicObj oneObj = n.Tag as LogicObj;
                        PointF pointf = LogicObjPosToImagePointF(oneObj);
                        int ndir = LogicObjToDirection(oneObj);
                        if (pointf != PointF.Empty)
                        {
                            ObjDrawInfo odi = new ObjDrawInfo(pointf, ndir, oneObj);
                            this.previewform.m_panel1.AddODI(n.Text, odi);
                        }
                    }
                }
            }
            else // 添加一个
            {
                PointF pointf = LogicObjPosToImagePointF(lobj);
                int ndir = LogicObjToDirection(lobj);
                if (pointf != PointF.Empty)
                {
                    ObjDrawInfo odi = new ObjDrawInfo(pointf, ndir, lobj);
                    this.previewform.m_panel1.AddODI(node.Text, odi);
                }
            }
        }

        private void tree1_AfterSelNodesRemove(object sender, MWPropertyEventArgs e)
        {
            TreeNode node = e.Updated as TreeNode;

            // 检查是否大分类结点
            if (node.Tag is string)
            {
                return;
            }

            LogicObj lobj = node.Tag as LogicObj;
            if (lobj == null) // 删除一组
            {
                LogicObj onechild = node.Nodes[0].Tag as LogicObj;
                if (onechild != null)
                {
                    foreach (TreeNode n in node.Nodes)
                    {
                        this.previewform.m_panel1.RemoveODI(n.Text);
                    }
                }
            }
            else // 删除一个
            {
                this.previewform.m_panel1.RemoveODI(node.Text);
            }
        }

        private void tree1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            // 生成propertygrid需要的对象
            LogicObj target = e.Node.Tag as LogicObj;
            if (target != null)
            {
                target.Clear();
                for (int i = 0; i < target.fields.Length; i++)
                {
                    target.Add(target.values[i]);
                }
                propertyGrid1.SelectedObject = target;
            }

            previewform.m_panel1.Refresh();
        }

        private void ShowMapContainSelectObj(TreeNode node)
        {
            ShowMap(AncestorNodeOf(node).Tag.ToString());
        }

        private void ShowMap(string mapname)
        {
            if (mapname != m_curMapName)
            {
                m_curMapName = mapname;

                string strConfig = string.Format("{0}/data/source/maps/{1}minimap/config.ini", m_BasePath, mapname);
                StringBuilder sb = new StringBuilder(255);
                Helper.ReadIniFile("middlemap0", "width", string.Empty, sb, 255, strConfig);
                int pic_width = Convert.ToInt32(sb.ToString());
                Helper.ReadIniFile("middlemap0", "height", string.Empty, sb, 255, strConfig);
                int pic_height = Convert.ToInt32(sb.ToString());
                Helper.ReadIniFile("middlemap0", "scale", string.Empty, sb, 255, strConfig);
                float scale = (float)Convert.ToDouble(sb.ToString());
                curMapLogicWidthTotal = Convert.ToInt32(pic_width / scale);
                curMapLogicHeightTotal = Convert.ToInt32(pic_height / scale);
                Helper.ReadIniFile("middlemap0", "startx", string.Empty, sb, 255, strConfig);
                curLogicStartX = (float)Convert.ToDouble(sb.ToString());
                Helper.ReadIniFile("middlemap0", "starty", string.Empty, sb, 255, strConfig);
                curLogicStartY = (float)Convert.ToDouble(sb.ToString());

                string strJpg = string.Format("{0}/data/source/maps/{1}minimap/middlemap.tga", m_BasePath, mapname);
                FileInfo fileinfo = new FileInfo(strJpg);
                if (fileinfo.Exists)
                    previewform.LoadPicture(strJpg);
            }
            previewform.Show();
        }

        private TreeNode AncestorNodeOf(TreeNode node)
        {
            TreeNode parent = node.Parent;
            if (parent == null)
            {
                return node;
            }
            while (parent != null)
            {
                node = node.Parent;
                parent = node.Parent;
            }
            return node;
        }

        private int LogicObjToDirection(LogicObj target)
        {
            if (htObjLogicDirIndices.ContainsKey(target.tyPe))
            {
                string nDirection = target.values[Convert.ToInt32(htObjLogicDirIndices[target.tyPe])];
                return Convert.ToInt32(nDirection);
            }
            else
            {
                return -1;
            }
        }

        private PointF LogicObjPosToImagePointF(LogicObj target)
        {
            if (htObjScenePosIndices.ContainsKey(target.tyPe))
            {
                string strNX = target.values[Convert.ToInt32(htObjLogicNXIndices[target.tyPe])];
                int nx = Convert.ToInt32(strNX);

                string strNY = target.values[Convert.ToInt32(htObjLogicNXIndices[target.tyPe]) + 1];
                int ny = Convert.ToInt32(strNY);

                float px = (float)((nx - curLogicStartX) / curMapLogicWidthTotal);
                float py = (float)(1.0 - (ny - curLogicStartY) / curMapLogicHeightTotal);
                PointF pfResult = new PointF(px, py);
                return pfResult;
            }
            else
            {
                return PointF.Empty;
            }
        }

        public void ForceMapChanged()
        {
            (maps[m_curMapName] as LO_Map).logicalChanged = true;
        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            ForceMapChanged();
        }

        private void buttonFind_Click(object sender, EventArgs e) // 查找窗口
        {
            FindForm findForm = new FindForm(this.tree1);
            findForm.Show();
        }

        // 关闭
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            ExitApp();
        }

        private void buttonX2_Click(object sender, EventArgs e) // close
        {
            ExitApp();
        }

        private void ExitApp()
        {
            try
            {
                //if (Program.Engine != null)
                //    Program.Engine.UnInit();
                ExitProcess(0);
            }
            catch
            {
                ExitProcess(0);
            }
        }
    }
}