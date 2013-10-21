using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security;
using System.Data.SqlClient;
using DevComponents.DotNetBar;
using System.Collections;
using System.Diagnostics;
using Paloma;
using System.IO;

namespace QuestGuide
{
    public partial class MainForm : Form
    {
        private SqlConnection Conn = null;
        private string RootDir = string.Empty;
        public Control OutterCtrl = null;
       
        const string QuestGuideTableName = "QuestGuide2";
        private int QuestId = 0;
        const int NodeCntCapcity = 23;//
        const int ColumnCntCapcity = 31;
        const int MapCntCapcity = 31;
        private ArrayList ArryNodes = new ArrayList(NodeCntCapcity);
       
        private string[] arrSpecialColumn;//需要编辑的列
        private string[] arrStartColumn;
        private string[] arrEndColumn;
        private string[] arrCommonNpcColumn;
        private string[] arrCommonDoodadColumn;

        private Hashtable htMapConfig;
        private Hashtable htRegion;
        private Hashtable htMapNameID;
        private Hashtable htMapIDName;
        private Hashtable htColumnNameDisplay;
        private Hashtable htMapNpcReaders;
        private Hashtable htMapDoodadReaders;

        private DataTable TableNpcTemplate = null;
        private DataTable TableDoodad = null;
      
        OneNode nodeCurrent = null;//处于编辑状态的节点
        private int DefaultMapId = 0;

        private bool bHasInitHashTable = false;


        public MainForm()
        {
            InitializeComponent();
        }

        public MainForm(SqlConnection conn, string rootDir)
        {
            InitializeComponent();
            Conn = conn;
            RootDir = rootDir;
            OutterCtrl = tabControl1;
        }

        public bool hasChanged()
        {
            foreach (OneNode node in ArryNodes)
            {
                if (node.IsChanged)
                    return true;
            }
            return false;
        }

        public bool saveAll()
        {
            SaveAllNodes();
            return true;
        }

        public bool reloadAll()
        {
            return true;
        }

        public bool reloadAll(string keyword)
        {
            if (!bHasInitHashTable)
            {
                InitHashtable();
                bHasInitHashTable = true;
            }

            panelEx1.ClearPaint();           

            string[] keywords = keyword.Split(new char[] {','});
            if (keywords.Length != 2)
                MessageBox.Show("错误的reloadAll参数");

            int questid = Convert.ToInt32(keywords[1]);
            if (questid < 0)
            {
                return false;
            }
            QuestId = questid;
            nodeRoot.Text = GetQuestName(questid);            
            reloadNodes(questid);           
            nodeCurrent = nodeRoot.Nodes[0].Tag as OneNode;
            UpdateStatus();
            dgvQuestInfo.DataSource = GetQuestMoreInfo();
            
            return true;
        }

        private void panelEx1_SizeChanged(object sender, EventArgs e)
        {
            int newWidth = panelEx1.Width;
            int newHeight = panelEx1.Height;
            panelEx1.ResetSize(newWidth, newHeight);
        }

      
        private bool reloadNodes(int nQuestId)
        { 
            string strSql = string.Format("select * from {0} where {1} = {2}", QuestGuideTableName, "QuestId", nQuestId);
            DataTable tbl = Helper.GetDataTable(strSql, Conn);
            if (tbl.Rows.Count <= 0)
            {
                //MessageBox.Show("错误的reloadAll参数: 找不到当前任务数据。");
                return false;
            }
            DataRow row = tbl.Rows[0];
            nodeRoot.Nodes.Clear();
            panelEx1.ClearNodes();
            ArryNodes.Clear();
            DefaultMapId = GetDefaultMapId(row);

            AddPointNodes(arrStartColumn, row, Brushes.Red, Color.Red);
            AddPointNodes(arrEndColumn, row, Brushes.Blue, Color.Blue);
            AddRegionNodes(arrSpecialColumn, row, Color.Purple, true, TemplateType.NONE);
            AddRegionNodes(arrCommonNpcColumn, row, Color.Purple, false, TemplateType.NPC_TEMPLATE);
            AddRegionNodes(arrCommonDoodadColumn, row, Color.Purple, false, TemplateType.DOODAD_TEMPLATE);
            return true;
        }
        /// <summary>
        /// 开始和结束的节点
        /// </summary>
        /// <param name="columns"></param>
        /// <param name="row"></param>
        /// <param name="color"></param>
        /// <param name="treeColor"></param>
        private void AddPointNodes(string[] columns, DataRow row, Brush color, Color treeColor)
        {
            string ColumnName_MapID;
            string ColumnName_nX;
            string ColumnName_nY;

            int nValue_MapID;
            int nValue_nX;
            int nValue_nY;

            foreach (string columnkey in columns)
            {
                ColumnName_MapID = string.Format("{0}_MapId", columnkey);
                ColumnName_nX = string.Format("{0}_nX", columnkey);
                ColumnName_nY = string.Format("{0}_nY", columnkey);

                if (row.IsNull(ColumnName_MapID) || Convert.ToInt32(row[ColumnName_MapID]) == 0)
                {
                    continue;
                }
                nValue_MapID = row.IsNull(ColumnName_MapID) ? 0 : Convert.ToInt32(row[ColumnName_MapID]);
                nValue_nX = row.IsNull(ColumnName_nX) ? 0 : Convert.ToInt32(row[ColumnName_nX]);
                nValue_nY = row.IsNull(ColumnName_nY) ? 0 : Convert.ToInt32(row[ColumnName_nY]);

                OneNode nodeTag = new OneNode();
                nodeTag.Init(nValue_MapID, nValue_nX, nValue_nY, color);
                SetMoreInfo(ref nodeTag);

                DevComponents.AdvTree.Node newTreenode = new DevComponents.AdvTree.Node();
                newTreenode.Text = htColumnNameDisplay[columnkey].ToString();
                newTreenode.Tag = nodeTag;
                newTreenode.Style = new ElementStyle(); newTreenode.Style.TextColor = treeColor;

                nodeRoot.Nodes.Add(newTreenode);
                panelEx1.AddNode(nodeTag);
            }
              

        }

        private void AddRegionNodes(string[] columns, DataRow row, Color treeColor, bool needEdit, TemplateType ttype)
        {
            string ColumnName_MapID; 
            string ColumnName_Areas;

            int nValue_MapID;
            string strValue_Areas;
           
            foreach (string columnkey in columns)
            {
                ColumnName_MapID = string.Format("{0}_MapId", columnkey);
                ColumnName_Areas = string.Format("{0}_Areas", columnkey);

                if (!needEdit && 
                    (row.IsNull(ColumnName_MapID) || Convert.ToInt32(row[ColumnName_MapID]) == 0)
                    )
                {
                    continue;
                }
                nValue_MapID = row.IsNull(ColumnName_MapID) ? 0 : Convert.ToInt32(row[ColumnName_MapID]);
                strValue_Areas = row.IsNull(ColumnName_Areas) ? "" : row[ColumnName_Areas].ToString();

                if (nValue_MapID == 0)
                {
                    nValue_MapID = DefaultMapId;
                }

                OneNode nodeTag = new OneNode();
                nodeTag.Init(nValue_MapID, strValue_Areas, ttype, columnkey);
                nodeTag.CanEdit = needEdit;
                SetBrothers(ref nodeTag, columnkey);
                MapConfig config = htMapConfig[nValue_MapID] as MapConfig;
                if (config != null)
                {
                    nodeTag.imageMap = config.TagImage;
                }
                DevComponents.AdvTree.Node newTreenode = new DevComponents.AdvTree.Node();
                newTreenode.Text = htColumnNameDisplay[columnkey].ToString();
                newTreenode.Tag = nodeTag;
                newTreenode.Style = new ElementStyle(); newTreenode.Style.TextColor = treeColor;

                nodeRoot.Nodes.Add(newTreenode);
                ArryNodes.Add(nodeTag);
            }


        }
       
        private void SetBrothers(ref OneNode node, string ColumnName)
        {
            int mapid = node.MapId;
            string mapName = "";

            if (htMapIDName.ContainsKey(mapid))
            {
                mapName = htMapIDName[mapid].ToString();
            }
            if (mapName == "")
            {
                return;
            }

            int nTemplateId = GetTemplateId(QuestId, ColumnName);
            ArrayList arrBrothers = GetBrothers(mapName, nTemplateId, node.templateType);
            node.arrBrothers = arrBrothers;

           
        }
        private ArrayList GetBrothers(string mapName, int nTemplateId, TemplateType ttype)
        {
            if (ttype == TemplateType.NONE)
            {
                return null;
            }
            Hashtable htReaders = null;
            string TableName = "NPC";

            if (ttype == TemplateType.NPC_TEMPLATE)
            {
                htReaders = htMapNpcReaders;
                TableName = "NPC";
            }
            else
            {
                htReaders = htMapDoodadReaders;
                TableName = "Doodad";
            }

            MapLogicalFileReader.TempleteIdXYProcessor processor;
            if (htReaders.ContainsKey(mapName))
            {
                processor = htReaders[mapName] as MapLogicalFileReader.TempleteIdXYProcessor;
            }
            else
            {
                string maplogicaFilePath = string.Format("{0}/data/source/maps/{1}/{1}.Map.Logical", RootDir, mapName);
                string configFilePath = string.Format("{0}/data/source/maps/{1}minimap/config.ini", RootDir, mapName);
                processor = new MapLogicalFileReader.TempleteIdXYProcessor(maplogicaFilePath, configFilePath, TableName);
                processor.Init();

                htReaders[mapName] = processor;
            }

            if (!processor.Available)
            {
                return null;
            }
            return processor.GetScaleArrList(nTemplateId);
        }
        private string GetMapName(int mapid)
        {
            string strRet = string.Empty;
            string sql = string.Format("select [Name] from MapList where ID = {0}", mapid);
            DataTable dt = Helper.GetDataTable(sql, Conn);
            if (dt != null && dt.Rows.Count == 1 && !dt.Rows[0].IsNull(0))
            {
                strRet = dt.Rows[0][0].ToString();
            }
            return strRet;
        }
        private int GetTemplateId(int questId, string columnName)
        {
            int nRet = 0;
            string sql = string.Format("select [{0}] from tbl_quests where QuestID = {1}", columnName, questId);
            DataTable dt = Helper.GetDataTable(sql, Conn);
            if (dt != null && dt.Rows.Count == 1 && !dt.Rows[0].IsNull(0))
            {
                Int32.TryParse(dt.Rows[0][0].ToString(), out nRet);
            }
            return nRet;
        }
        private void InitHashtable()
        {
            arrSpecialColumn = new string[8] {
                "QuestValue1",
                "QuestValue2",
                "QuestValue3",
                "QuestValue4",
                "QuestValue5",
                "QuestValue6",
                "QuestValue7",
                "QuestValue8",
            };
            arrStartColumn = new string[2]{
                "StartDoodadTemplateID",
                "StartNpcTemplateID"
            };
            arrEndColumn = new string[2] { 
                "EndNpcTemplateID",
                "EndDoodadTemplateID"
            };
            arrCommonNpcColumn = new string[8] {                 

                "KillNpcTemplateID1",
                "KillNpcTemplateID2",
                "KillNpcTemplateID3",
                "KillNpcTemplateID4",


                "DropNpcTemplateID1",
                "DropNpcTemplateID2",
                "DropNpcTemplateID3",
                "DropNpcTemplateID4",

            };
            arrCommonDoodadColumn = new string[4] {   

                "DropDoodadTemplateID1",
                "DropDoodadTemplateID2",
                "DropDoodadTemplateID3",
                "DropDoodadTemplateID4",
            };
          
            //数据库->列显示字符
            htColumnNameDisplay = new Hashtable(ColumnCntCapcity);
            htColumnNameDisplay["Objective"] = "任务目标";
            htColumnNameDisplay["StartDoodadTemplateID"] = "接任务doodad";
            htColumnNameDisplay["StartNpcTemplateID"] = "接任务npc";
            htColumnNameDisplay["EndNpcTemplateID"] = "交任务npc";
            htColumnNameDisplay["EndDoodadTemplateID"] = "交任务doodad";

            htColumnNameDisplay["KillNpcTemplateID1"] = "杀NPC1";
            htColumnNameDisplay["KillNpcTemplateID2"] = "杀NPC2";
            htColumnNameDisplay["KillNpcTemplateID3"] = "杀NPC3";
            htColumnNameDisplay["KillNpcTemplateID4"] = "杀NPC4";
            htColumnNameDisplay["DropDoodadTemplateID1"] = "掉落道具1的DOODAD";
            htColumnNameDisplay["DropDoodadTemplateID2"] = "掉落道具2的DOODAD";
            htColumnNameDisplay["DropDoodadTemplateID3"] = "掉落道具3的DOODAD";
            htColumnNameDisplay["DropDoodadTemplateID4"] = "掉落道具4的DOODAD";
            htColumnNameDisplay["DropNpcTemplateID1"] = "掉落道具1的NPC";
            htColumnNameDisplay["DropNpcTemplateID2"] = "掉落道具2的NPC";
            htColumnNameDisplay["DropNpcTemplateID3"] = "掉落道具3的NPC";
            htColumnNameDisplay["DropNpcTemplateID4"] = "掉落道具4的NPC";
            htColumnNameDisplay["QuestValue1"] = "任务变量1";
            htColumnNameDisplay["QuestValue2"] = "任务变量2";
            htColumnNameDisplay["QuestValue3"] = "任务变量3";
            htColumnNameDisplay["QuestValue4"] = "任务变量4";
            htColumnNameDisplay["QuestValue5"] = "任务变量5";
            htColumnNameDisplay["QuestValue6"] = "任务变量6";
            htColumnNameDisplay["QuestValue7"] = "任务变量7";
            htColumnNameDisplay["QuestValue8"] = "任务变量8";           

 
          

            //mapconfig
            htMapConfig = new Hashtable(MapCntCapcity);
            htRegion = new Hashtable(MapCntCapcity);
            htMapNameID = new Hashtable(MapCntCapcity);
            htMapIDName = new Hashtable(MapCntCapcity);
            string sql = "select [ID], [Name] from MapList where [Name] <> '测试1'";
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            
            string mapname = "";  
            int mapid = 0;
            int cfgWidth = 0;
            int cfgHeight = 0;
            float cfgScale = 0.0f;
            float cfgStartx = 0.0f;
            float cfgStarty = 0.0f;
            string mapTgaFileName = "";
            MapConfig config;

            htMapNameID["无"] = 0;
            combxItemMapId.Items.Add("无");
            foreach (DataRow row in tbl.Rows)
            {
                mapid = Convert.ToInt32(row[0]);
                mapname = row[1].ToString();
                if (LoadConfig(mapname, ref cfgWidth, ref cfgHeight, ref cfgStartx, ref cfgStarty, ref cfgScale))
                {
                    config = new MapConfig(cfgWidth, cfgHeight, cfgScale, cfgStartx, cfgStarty, mapid, mapname);
                    mapTgaFileName = string.Format("{0}/data/source/maps/{1}minimap/middlemap.tga", RootDir, mapname);
                    if (File.Exists(mapTgaFileName))
                    {
                        Image imgNew = null;
                        try
                        {
                            imgNew = Paloma.TargaImage.LoadTargaImage(mapTgaFileName);
                        }
                        catch (Exception ex)
                        {         
                            string errMsg = string.Format("{0}LoadTargaImage失败。\r\n\r\n{1}", mapTgaFileName, ex.ToString());
                            //MessageBox.Show(errMsg, "出错啦", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            imgNew = null;
                        }
                        config.TagImage = imgNew;
                    }
                    htMapConfig[mapid] = config;
                    combxItemMapId.Items.Add(mapname);
                    htMapNameID[mapname] = mapid;
                    htMapIDName[mapid] = mapname;
                }

                //QuestDirector2.RegionInfo region = new QuestDirector2.RegionInfo();
                //if (region.Init(RootDir, mapname))
                //{
                //    htRegion[mapid] = region;
                //}
            }

            htMapDoodadReaders = new Hashtable(MapCntCapcity);
            htMapNpcReaders = new Hashtable(MapCntCapcity);

            sql = string.Format("select [Name], [ID] from Npctemplate where [ID] <> 0");
            TableNpcTemplate = Helper.GetDataTable(sql, Conn);
            sql = string.Format("select [Name], [ID] from tbl_doodad where [ID] <> 0");
            TableDoodad = Helper.GetDataTable(sql, Conn);
            return;
        }
   
       
        private void SetMoreInfo(ref OneNode node)
        {
            int mapid = node.MapId;
            MapConfig config = htMapConfig[mapid] as MapConfig;
            if (config == null)
            {
                node.imageMap = null;
                node.fScaleX = 0.5f;
                node.fScaleY = 0.5f;
                return;
            }
            int cfgWidth = 0;
            int cfgHeight = 0;
            float cfgScale = 0.0f;
            float cfgStartx = 0;
            float cfgStarty = 0;
            config.GetConfig(ref cfgWidth, ref cfgHeight, ref cfgStartx, ref cfgStarty, ref cfgScale);
            node.fScaleX = (node.nX - cfgStartx) / (cfgWidth / cfgScale);
            node.fScaleY = 1 - (node.nY - cfgStarty) / (cfgHeight / cfgScale);
            node.imageMap = config.TagImage;
        }
        /// <summary>
        /// 读配置文件config.ini
        /// </summary>
        /// <param name="mapname"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="startx"></param>
        /// <param name="starty"></param>
        /// <param name="scale"></param>
        /// <returns></returns>
        private bool LoadConfig(string mapname, ref int width, ref int height, ref float startx, ref float starty, ref float scale)
        {
            string cfgFileName = string.Format("{0}/data/source/maps/{1}minimap/config.ini", RootDir, mapname);
            if (!File.Exists(cfgFileName))
            {
                return false;
            }
            // 读文件
            string content = QuestDirector2.Helper.FileToString(cfgFileName, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);

            FileInfo fiTarget = new FileInfo(cfgFileName);
            if (!fiTarget.Exists)
            {
                return false;
            }

            QuestDirector2.Helper.OpenIniFile(cfgFileName, content);
            if (QuestDirector2.Helper.m_inis == null)
            {
                string strErr = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", cfgFileName);
                MessageBox.Show(strErr);
                return false;
            }
            try
            {
                width = Convert.ToInt32(QuestDirector2.Helper.GetIniString("middlemap0", "width"));
                height = Convert.ToInt32(QuestDirector2.Helper.GetIniString("middlemap0", "height"));
                scale = Convert.ToSingle(QuestDirector2.Helper.GetIniString("middlemap0", "scale"));
                startx = Convert.ToSingle(QuestDirector2.Helper.GetIniString("middlemap0", "startx"));
                starty = Convert.ToSingle(QuestDirector2.Helper.GetIniString("middlemap0", "starty"));
            }
            catch (System.Exception e)
            {
                string strErr = string.Format("{0} 内容配置有误。\r\n{1}", cfgFileName, e.ToString());
                MessageBox.Show(strErr);
                return false;
            }
         

            return true;

        }
        private void advTree1_NodeClick(object sender, DevComponents.AdvTree.TreeNodeMouseEventArgs e)
        {
            DevComponents.AdvTree.Node advTreeNode = e.Node;
            if (advTreeNode == nodeRoot)
            {
                return;
            }

            OneNode nodeTag = advTreeNode.Tag as OneNode;          
            nodeCurrent = nodeTag;
            panelEx1.CurrentNode = nodeCurrent;

            UpdateStatus();
        }

    
        private void panelEx1_MouseMove(object sender, MouseEventArgs e)
        {  
            if (nodeCurrent == null || nodeCurrent.imageMap == null)
            {
                return;
            }

            int mapid = nodeCurrent.MapId;
            int logicalx = 0;
            int logicaly = 0;            
            float scalex = Convert.ToSingle(e.X) / Convert.ToSingle(panelEx1.Width);
            float scaley = Convert.ToSingle(e.Y) / Convert.ToSingle(panelEx1.Height);

            bool bResult = false;
            int grayId = 0;
            bResult = GetLogicalCoordinate(mapid, scalex, scaley, ref logicalx, ref logicaly);
            if (bResult)
            {
                GetGrayId(mapid, logicalx, logicaly, out grayId);
            }

            lbItemGray.Text = string.Format("灰度:{0}", grayId);
            lbItemCursor.Text = string.Format("({0},{1})", logicalx, logicaly);
        }
       
        private bool GetLogicalCoordinate(int mapid, float scalex, float scaley, ref int logicalx, ref int logicaly)
        {
            int cfgWidth = 0;
            int cfgHeight = 0;
            float cfgScale = 0.0f;
            float cfgStartx = 0;
            float cfgStarty = 0;

            MapConfig config = htMapConfig[mapid] as MapConfig;
            if (config == null)
            {
                return false;
            }
            config.GetConfig(ref cfgWidth, ref cfgHeight, ref cfgStartx, ref cfgStarty, ref cfgScale);
            logicalx = Convert.ToInt32((cfgWidth * scalex / cfgScale) + cfgStartx);
            logicaly = Convert.ToInt32((cfgHeight * (1 - scaley) / cfgScale) + cfgStarty);
            return true;
        }
        private bool GetGrayId(int mapid, int logicalx, int logicaly, out int grayId)
        {
            grayId = 0;
            QuestDirector2.RegionInfo region = htRegion[mapid] as QuestDirector2.RegionInfo;
            if (region == null)
            {
                return false;
            }
            return region.GetGreyDegree(logicalx, logicaly, ref grayId);
        }
        private void combxItemMapId_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (combxItemMapId.SelectedItem == null)
            {
                return;
            }
            if (combxItemMapId.SelectedItem.ToString() == "无")
            {
                nodeCurrent.imageMap = null;
                nodeCurrent.nX = 0;
                nodeCurrent.nY = 0;
                nodeCurrent.MapId = 0;
                SetMoreInfo(ref nodeCurrent);
            }
            else
            {
                string mapname = combxItemMapId.SelectedItem.ToString();
                if (!htMapNameID.ContainsKey(mapname))
                {
                    MessageBox.Show("");
                    return;
                }
                int mapid = Convert.ToInt32(htMapNameID[mapname]);
                if (!htMapConfig.ContainsKey(mapid))
                {
                    MessageBox.Show("mapid不存在");
                    return;
                }
                nodeCurrent.imageMap = (htMapConfig[mapid] as MapConfig).TagImage;
                nodeCurrent.MapId = mapid;
            }
            UpdateStatus();            
        }

        private void panelEx1_MouseClick(object sender, MouseEventArgs e)
        {
            if (nodeCurrent == null || !nodeCurrent.CanEdit)
            {
                return;
            } 
            if (combxItemMapId.SelectedItem == null || combxItemMapId.SelectedItem.ToString() == "无")
            {
                return;
            }

            int logicalx = 0;
            int logicaly = 0;
            string mapname = combxItemMapId.SelectedItem.ToString();

            if (!htMapNameID.ContainsKey(mapname))
            {
                MessageBox.Show(mapname + "不存在");
                return;
            }

            int mapid = Convert.ToInt32(htMapNameID[mapname]);
            float scalex = Convert.ToSingle(e.X) / Convert.ToSingle(panelEx1.Width);
            float scaley = Convert.ToSingle(e.Y) / Convert.ToSingle(panelEx1.Height);
            
            if (GetLogicalCoordinate(mapid, scalex, scaley, ref logicalx, ref logicaly))                
            {
                int grayId;
                if (GetGrayId(mapid, logicalx, logicaly, out grayId))
                {
                    if (nodeCurrent.ContainGrayId(grayId))
                    {
                        nodeCurrent.RemoveGrayId(grayId);
                    }
                    else
                    {
                        nodeCurrent.AddGrayId(grayId);
                    }
                    
                }
            }
            UpdateStatus();
        }

        private void SaveAllNodes()
        {            
            foreach (OneNode node in ArryNodes)
            {
                if (node.CanEdit && node.IsChanged)
                {
                    SaveOneNode(node);
                }
            }
           
            UpdateStatus();
        }
        private void SaveOneNode(OneNode node)
        {
            if (node.IsChanged)
            {   
                string strKey = node.ColumnNameKey;
                string ColumnName_nMapId = string.Format("{0}_MapId", strKey);
                string ColumnName_Areas = string.Format("{0}_Areas", strKey);

                string sql = string.Format( 
                    " update {0} set {1} = {2}, {3} = '{4}' where QuestId ={5}",
                    QuestGuideTableName, ColumnName_nMapId, node.MapId, ColumnName_Areas, node.StringGrayIds, QuestId
                    );
                Helper.SqlNoneQureyExcute(Conn, sql);
                node.BackUp();
            }
        }
        private void UpdateStatus()
        {
            if (nodeCurrent == null)
            {
                return;
            }
            
            switch(nodeCurrent.nodeType)
            {
                case NodeType.POINT:
                    lbXYorGrayIds.Text = string.Format("(X:{0}, Y:{1})", nodeCurrent.nX, nodeCurrent.nY);
                    panelEx1.CurrentRegion = null;                    
                    break;
                case NodeType.REGION:
                    lbXYorGrayIds.Text = string.Format("灰度：{0}", nodeCurrent.StringGrayIds);
                    panelEx1.CurrentRegion = htRegion[nodeCurrent.MapId] as QuestDirector2.RegionInfo;
                    break;
                default:
                    break;
            }
            combxItemMapId.Enabled = nodeCurrent.CanEdit;
            panelEx1.CurrentConfig = htMapConfig[nodeCurrent.MapId] as MapConfig;
            //this.colorPickerDropDown1.SelectedColor
            

            if (nodeCurrent.MapId == 0)
            {
                combxItemMapId.SelectedItem = "无";                
                lbItemCursor.Text = "请先选地图";
            }
            else
            {
                combxItemMapId.SelectedItem = (htMapConfig[nodeCurrent.MapId] as MapConfig).MapName;
                if (nodeCurrent.imageMap == null)
                {
                    lbItemCursor.Text = "找不到tga地图文件";
                }
                else
                {
                    lbItemCursor.Text = string.Format("({0},{0})", nodeCurrent.nX, nodeCurrent.nY);
                }                
            }
            
            btnDeleteQuestValue.Enabled = nodeCurrent.CanEdit;
            
            panelEx1.CurrentNode = nodeCurrent;            
            panelEx1.Refresh();
        }
        private string GetQuestName(int questid)
        {
            string sql = string.Format("select [QuestName] from tbl_quests where [QuestID] = {0}", questid);
            DataTable dt = Helper.GetDataTable(sql, Conn);
            if (dt.Rows.Count != 1)
            {
                MessageBox.Show(string.Format("id{0}在任务表里不存在", questid));
                return "错误的任务id";
            }
            string strRet = dt.Rows[0][0].ToString();
            return strRet;
        }


        private DataTable GetQuestMoreInfo()
        {
            DataTable dtRet = new DataTable();
            dtRet.Columns.Add("名称", typeof(string));
            dtRet.Columns.Add("更多信息", typeof(string));

            string strSql = string.Format("select * from tbl_quests where QuestID = {0}",  QuestId);

            DataTable dtTemp = Helper.GetDataTable(strSql, Conn);
            if (dtTemp.Rows.Count != 1)
                return null;
            DataRow row = dtTemp.Rows[0];

            string columnName;
            string displayName;
            string displayValue;

            DataRow newRow;

            foreach (DictionaryEntry de in htColumnNameDisplay)
            {
                columnName = de.Key as string;
                displayName = de.Value as string;
                
                if (row.IsNull(columnName) || row[columnName].ToString() == "" || row[columnName].ToString() == "0")
                    continue;

                if (displayName.ToLower().Contains("npc"))
                {
                    string id = row[columnName].ToString(); 
                    string filterExpression = string.Format("[ID] = {0}", id);
                    DataRow[] filterRows = TableNpcTemplate.Select(filterExpression);
                    if (filterRows.Length == 0)
                        continue;
                    displayValue = filterRows[0]["Name"].ToString();
                    newRow = dtRet.NewRow();
                    newRow[0] = displayName;
                    newRow[1] = displayValue;
                    dtRet.Rows.Add(newRow);
                }
                else if (displayName.ToLower().Contains("doodad"))
                {
                    string id = row[columnName].ToString();
                    string filterExpression = string.Format("[ID] = {0}", id);
                    DataRow[] filterRows = TableDoodad.Select(filterExpression);
                    if (filterRows.Length == 0)
                        continue;
                    displayValue = filterRows[0]["Name"].ToString();
                    newRow = dtRet.NewRow();
                    newRow[0] = displayName;
                    newRow[1] = displayValue;
                    dtRet.Rows.Add(newRow);
                }
                else
                {
                    displayValue = row[columnName].ToString();
                    newRow = dtRet.NewRow();
                    newRow[0] = displayName;
                    newRow[1] = displayValue;
                    dtRet.Rows.Add(newRow);
                }

            }

            return dtRet;

        }


        /// <summary>
        /// 获取默认的地图id
        /// </summary>
        /// <param name="QuestGuideRow"></param>
        /// <returns></returns>
        private int GetDefaultMapId(DataRow QuestGuideRow)
        {
            string strColumn_MapID;

            foreach (string columnkey in arrStartColumn)
            {
                strColumn_MapID = string.Format("{0}_MapId", columnkey);

                if (!QuestGuideRow.IsNull(strColumn_MapID) && Convert.ToInt32(QuestGuideRow[strColumn_MapID]) != 0)
                {
                   return Convert.ToInt32(QuestGuideRow[strColumn_MapID]);
                }                
            }

            foreach (string columnkey in arrEndColumn)
            {
                strColumn_MapID = string.Format("{0}_MapId", columnkey);

                if (!QuestGuideRow.IsNull(strColumn_MapID) && Convert.ToInt32(QuestGuideRow[strColumn_MapID]) != 0)
                {
                    return Convert.ToInt32(QuestGuideRow[strColumn_MapID]);
                }
            }

            foreach (string columnkey in arrCommonNpcColumn)
            {
                strColumn_MapID = string.Format("{0}_MapId", columnkey);

                if (!QuestGuideRow.IsNull(strColumn_MapID) && Convert.ToInt32(QuestGuideRow[strColumn_MapID]) != 0)
                {
                    return Convert.ToInt32(QuestGuideRow[strColumn_MapID]);
                }
            }
            foreach (string columnkey in arrCommonDoodadColumn)
            {
                strColumn_MapID = string.Format("{0}_MapId", columnkey);

                if (!QuestGuideRow.IsNull(strColumn_MapID) && Convert.ToInt32(QuestGuideRow[strColumn_MapID]) != 0)
                {
                    return Convert.ToInt32(QuestGuideRow[strColumn_MapID]);
                }
            }

            return 0;            
        }

       

        private void splitContainer1_Panel2_SizeChanged(object sender, EventArgs e)
        {
            int newWidth;
            int newHeight;
            float fnewWidth = (float)this.splitContainer1.Panel2.Width;
            float fnewHeight = (float)this.splitContainer1.Panel2.Height;
            //地图的长宽比例固定
            if (fnewWidth / fnewHeight > CircleType.WidthHeigthScale)
            {
                newWidth = Convert.ToInt32(fnewHeight * CircleType.WidthHeigthScale);
                newHeight = Convert.ToInt32(fnewHeight);
            }
            else
            {
                newWidth = Convert.ToInt32(fnewWidth);
                newHeight = Convert.ToInt32(fnewWidth / CircleType.WidthHeigthScale);                
            }
            panelEx1.Size = new Size(newWidth, newHeight);            
        }

        private void btnDeleteQuestValue_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("真的要重置选中的任务变量吗？", "Are you sure?", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                nodeCurrent.ClearGrayId();
                UpdateStatus();
            }            
        }       
    }
}