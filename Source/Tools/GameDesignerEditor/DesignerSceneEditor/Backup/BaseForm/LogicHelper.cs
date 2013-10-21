using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;
using System.Runtime.InteropServices;
using DevComponents.DotNetBar;
using LuaInterface;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    public class ObjDrawInfo
    {
        public PointF pfPos;    // 点于图的百分比位置，坐上为 (0.0, 0.0)
        public int nDir;      // 点的朝向
        public LogicObj lObj; // 对象ref

        public ObjDrawInfo(PointF pos, int dir, LogicObj obj)
        {
            this.pfPos = pos;
            this.nDir = dir;
            this.lObj = obj;
        }
    };

    public class LO_Map
    {
        public string name;
        public LogicObj MAIN;
        public LogicObj[] NPCs;
        public LogicObj[] Doodads;
        public LogicObj[] AIGroups;
        public LogicObj[] WayPointSets;
        public LogicObj[] LogicalPolys;
        public LogicObj[] TrafficPointSets;
        public LogicObj[] TrafficPoints;
        public LogicObj[] NpcReviveGroups;
        public LogicObj[] DoodadReviveGroups;
        public LogicObj[] NpcRandomGroups;
        public LogicObj[] AISets;
    };

    public class LogicObj /*: CollectionBase, ICustomTypeDescriptor, ICloneable*/
    {
        public int RepresentObjPtr;
        public int LogicObjPtr;
        public string naMe; // 显示名字
        public string nickName; // 
        public bool hasScript; // 是否绑了脚本
        public int templateID; // npc doodad 专用，模板ID
//      public bool changed; // 标识本身有否修改，用于保存的判断
        public string tyPe; // 那种类型的对象
        public int inDex; // 索引
        public int parentindex; // 父亲的索引
//      public string[] fields; // 成员名
//      public string[] values; // 成员值
//      public bool[] fields_changed; // 标识某成员有否修改，用于黑体显示
        public LogicObj[] kids1; // 子对象最多有↓
        public LogicObj[] kids2; // 2个

        public LogicObj(string typeName, int index, int parentIndex, string name, string nickname, bool hasscript, int representobjptr, int logicobjptr, int templateid)
        {
            RepresentObjPtr = representobjptr;
            LogicObjPtr = logicobjptr;
            tyPe = typeName;
            inDex = index;
            parentindex = parentIndex;
            naMe = name;
            nickName = nickname;
            hasScript = hasscript;
            templateID = templateid;
        }
    };

    public class LogicHelper
    {
        public static int m_NpcCount = -1;
        public static int m_DoodadCount = -1;
        public static int m_AIGroupCount = -1;
        public static int m_WayPointSetCount = -1;
        public static int m_LogicalPolyCount = -1;
        public static int m_TrafficLittlePointSetCount = -1;
        public static int m_TrafficPointCount = -1;
        public static int m_NpcReviveGroupCount = -1;
        public static int m_DoodadReviveGroupCount = -1;
        public static int m_NpcRandomGroupCount = -1;
        public static int m_AISetCount = -1;
        public static IAtlIEKSceneEditorDocLogical m_LogicalData = null;

        public static TreeView m_LogicTree = null;
        public static TreeNode m_nodeMap = null;
        public static TreeNode m_NodeNPCs = null;
        public static TreeNode m_NodeDoodads = null;

        // 供GetObjDisplayInfo用的临时变量。
        static int logicObjPtr = 0;
        static int representObjPtr = 0;
        static int hasscript = 0;
        static string name = string.Empty;
        static string nickname = string.Empty;
        static int templateID = -1;


        public static void Init(IAtlIEKSceneEditorDocLogical logicaldata)
        {
            m_LogicalData = logicaldata;
            m_LogicalData.GetObjCount("NPC", -1, ref m_NpcCount);
            m_LogicalData.GetObjCount("Doodad", -1, ref m_DoodadCount);
            m_LogicalData.GetObjCount("AIGroup", -1, ref m_AIGroupCount);
            m_LogicalData.GetObjCount("WayPointSet", -1, ref m_WayPointSetCount);
            m_LogicalData.GetObjCount("LogicalPoly", -1, ref m_LogicalPolyCount);
            m_LogicalData.GetObjCount("TrafficLittlePointSet", -1, ref m_TrafficLittlePointSetCount);
            m_LogicalData.GetObjCount("TrafficPoint", -1, ref m_TrafficPointCount);
            m_LogicalData.GetObjCount("NpcReviveGroup", -1, ref m_NpcReviveGroupCount);
            m_LogicalData.GetObjCount("DoodadReviveGroup", -1, ref m_DoodadReviveGroupCount);
            m_LogicalData.GetObjCount("NpcRandomGroup", -1, ref m_NpcRandomGroupCount);
            m_LogicalData.GetObjCount("AISet", -1, ref m_AISetCount);
        }

        public static LO_Map DoImportOneTarget(string strFile, string mapname)
        {
            // map对象
            LO_Map map = new LO_Map();
            map.name = mapname;
            LogicObj main = new LogicObj("MAIN", 0, -1, mapname, string.Empty, false, 0, 0, -1);
            map.MAIN = main;

            // 各种对象
            map.NPCs = new LogicObj[m_NpcCount];
            map.Doodads = new LogicObj[m_DoodadCount];
            map.AIGroups = new LogicObj[m_AIGroupCount];
            map.WayPointSets = new LogicObj[m_WayPointSetCount];
            map.LogicalPolys = new LogicObj[m_LogicalPolyCount];
            map.TrafficPointSets = new LogicObj[m_TrafficLittlePointSetCount];
            map.TrafficPoints = new LogicObj[m_TrafficPointCount];
            map.NpcReviveGroups = new LogicObj[m_NpcReviveGroupCount];
            map.DoodadReviveGroups = new LogicObj[m_DoodadReviveGroupCount];
            map.NpcRandomGroups = new LogicObj[m_NpcRandomGroupCount];
            map.AISets = new LogicObj[m_AISetCount];

            for (int i = 0; i < m_NpcCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("NPC", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj npc = new LogicObj("NPC", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                map.NPCs[i] = npc;
            }
            for (int i = 0; i < m_DoodadCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("Doodad", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj doodad = new LogicObj("Doodad", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                map.Doodads[i] = doodad;
            }
            for (int i = 0; i < m_AIGroupCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("AIGroup", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj aigroup = new LogicObj("AIGroup", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                int npccount = -1;
                m_LogicalData.GetObjCount("AIGroupNPC", i, ref npccount);
                aigroup.kids1 = new LogicObj[npccount];
                for (int j = 0; j < npccount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("AIGroupNPC", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj npc = new LogicObj("AIGroupNPC", j, i, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                    aigroup.kids1[j] = npc;
                }
                map.AIGroups[i] = aigroup;
            }
            for (int i = 0; i < m_WayPointSetCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("WayPointSet", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj waypointset = new LogicObj("WayPointSet", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                int waypointcount = -1;
                m_LogicalData.GetObjCount("WayPoint", i, ref waypointcount);
                waypointset.kids1 = new LogicObj[waypointcount];
                for (int j = 0; j < waypointcount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("WayPoint", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj waypoint = new LogicObj("WayPoint", j, i, "Point" + j.ToString(), string.Empty, false, representObjPtr, logicObjPtr, templateID);
                    waypointset.kids1[j] = waypoint;
                }
                map.WayPointSets[i] = waypointset;
            }
            for (int i = 0; i < m_LogicalPolyCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("LogicalPoly", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj logicalpoly = new LogicObj("LogicalPoly", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                int vertexcount = -1;
                m_LogicalData.GetObjCount("LogicalPolyVertex", i, ref vertexcount);
                logicalpoly.kids1 = new LogicObj[vertexcount];
                for (int j = 0; j < vertexcount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("LogicalPolyVertex", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj vertex = new LogicObj("LogicalPolyVertex", j, i, "Vertex" + j.ToString(), string.Empty, false, representObjPtr, logicObjPtr, templateID);
                    logicalpoly.kids1[j] = vertex;
                }
                map.LogicalPolys[i] = logicalpoly;
            }
            for (int i = 0; i < m_TrafficLittlePointSetCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("TrafficLittlePointSet", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj trafficpointset = new LogicObj("TrafficLittlePointSet", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                int pointcount = -1;
                m_LogicalData.GetObjCount("TrafficLittlePoint", i, ref pointcount);
                trafficpointset.kids1 = new LogicObj[pointcount];
                for (int j = 0; j < pointcount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("TrafficLittlePoint", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj point = new LogicObj("TrafficLittlePoint", j, i, "Point" + j.ToString(), string.Empty, false, representObjPtr, logicObjPtr, templateID);
                    trafficpointset.kids1[j] = point;
                }
                map.TrafficPointSets[i] = trafficpointset;
            }
            for (int i = 0; i < m_TrafficPointCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("TrafficPoint", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj trafficpoint = new LogicObj("TrafficPoint", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                map.TrafficPoints[i] = trafficpoint;
            }
            for (int i = 0; i < m_NpcReviveGroupCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("NpcReviveGroup", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj npcrevivegroup = new LogicObj("NpcReviveGroup", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                // 因为重生组的成员可以通过直接遍历对象链表获得，额外维护组内单独的成员链表成本太高，所以简化一下。 modify by suntao
                List<LogicObj> logicObjList = new List<LogicObj>();

                for (int j = 0; j < m_NpcCount; j++)
                {
                    _AtlObjInfo objectInfo = new _AtlObjInfo();
                    m_LogicalData.GetObjDisplayInfo("NPC", j, 0, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    m_LogicalData.GetSetObjInfo("NPC", ref objectInfo, logicObjPtr, 1);

                    Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                    int groupID = i + 1;

                    if (infoTable["ReliveID"] as string == groupID.ToString())
                    {
                        LogicObj npc = new LogicObj("NPC", j, 0, name, nickname, false, representObjPtr, logicObjPtr, templateID);
                        logicObjList.Add(npc);                        
                    }                    
                }

                npcrevivegroup.kids1 = logicObjList.ToArray();

                /* 
                int npccount = -1;
                m_LogicalData.GetObjCount("NpcReviveGroupNpc", i, ref npccount);
                npcrevivegroup.kids1 = new LogicObj[npccount];
                for (int j = 0; j < npccount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("NpcReviveGroupNpc", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj npc = new LogicObj("NpcReviveGroupNpc", j, i, name, nickname, false, representObjPtr, logicObjPtr, templateID);
                    npcrevivegroup.kids1[j] = npc;
                }
                */

                map.NpcReviveGroups[i] = npcrevivegroup;
            }
            for (int i = 0; i < m_DoodadReviveGroupCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("DoodadReviveGroup", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj doodadrevivegroup = new LogicObj("DoodadReviveGroup", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                // 因为重生组的成员可以通过直接遍历对象链表获得，额外维护组内单独的成员链表成本太高，所以简化一下。 modify by suntao
                List<LogicObj> logicObjList = new List<LogicObj>();

                for (int j = 0; j < m_DoodadCount; j++)
                {
                    _AtlObjInfo objectInfo = new _AtlObjInfo();
                    m_LogicalData.GetObjDisplayInfo("Doodad", j, 0, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    m_LogicalData.GetSetObjInfo("Doodad", ref objectInfo, logicObjPtr, 1);

                    Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                    int groupID = i + 1;

                    if (infoTable["ReliveID"] as string == groupID.ToString())
                    {
                        LogicObj doodad = new LogicObj("Doodad", j, 0, name, nickname, false, representObjPtr, logicObjPtr, templateID);
                        logicObjList.Add(doodad);
                    }
                }

                doodadrevivegroup.kids1 = logicObjList.ToArray();

                /*
                int doodadcount = -1;
                m_LogicalData.GetObjCount("DoodadReviveGroupDoodad", i, ref doodadcount);
                doodadrevivegroup.kids1 = new LogicObj[doodadcount];
                for (int j = 0; j < doodadcount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("DoodadReviveGroupDoodad", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj doodad = new LogicObj("DoodadReviveGroupDoodad", j, i, name, nickname, false, representObjPtr, logicObjPtr, templateID);
                    doodadrevivegroup.kids1[j] = doodad;
                }
                */

                map.DoodadReviveGroups[i] = doodadrevivegroup;
            }
            for (int i = 0; i < m_NpcRandomGroupCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("NpcRandomGroup", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj npcrandomgroup = new LogicObj("NpcRandomGroup", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                map.NpcRandomGroups[i] = npcrandomgroup;
            }
            for (int i = 0; i < m_AISetCount; i++)
            {
                m_LogicalData.GetObjDisplayInfo("AISet", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj aiset = new LogicObj("AISet", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                int paramcount = -1;
                m_LogicalData.GetObjCount("AISetParam", i, ref paramcount);
                aiset.kids1 = new LogicObj[paramcount];
                for (int j = 0; j < paramcount; j++)
                {
                    m_LogicalData.GetObjDisplayInfo("AISetParam", j, i, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                    LogicObj param = new LogicObj("AISetParam", j, i, "Param" + j.ToString(), string.Empty, false, representObjPtr, logicObjPtr, templateID);
                    aiset.kids1[j] = param;
                }
                map.AISets[i] = aiset;
            }


            return map;
        }


        public static void LoadTree(LO_Map map, TreeView tree1, bool bShowAll)
        {
            m_LogicTree = tree1;

            tree1.Nodes.Clear();
            
            TreeNode nodeMap = tree1.Nodes.Add(map.name, map.name);
            nodeMap.Tag = map.name;
            m_nodeMap = nodeMap;

            TreeNode nodesNPC = nodeMap.Nodes.Add(string.Empty, "NPCs");
            m_NodeNPCs = nodesNPC;
            nodesNPC.Tag = nodesNPC.Text;
            foreach (LogicObj npc in map.NPCs)
            {
                //string npcName = npc.naMe;
                string npcName = string.Format("{0} [{1}]", npc.naMe, npc.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", npcName, npc.nickName.Length > 0 ? "[" + npc.nickName + "]" : string.Empty, npc.hasScript ? "[√]" : string.Empty);
                if (npc.nickName.Length == 0 && !bShowAll)
                    continue;

                TreeNode[] nodefathers = nodesNPC.Nodes.Find(npcName, false);
                if (nodefathers.Length == 0)
                {
                    TreeNode newnpcnode = nodesNPC.Nodes.Add(npcName, fullDisplayName);
                    newnpcnode.Tag = npc;
                }
                else if (nodefathers.Length == 1)
                {
                    // 把原来位于分类上的结点变成它自己的孩子
                    if (nodefathers[0].Tag != null)
                    {
                        TreeNode oldnewnpcnode = nodefathers[0].Nodes.Add(nodefathers[0].Name, nodefathers[0].Text);
                        oldnewnpcnode.Tag = nodefathers[0].Tag;

                        nodefathers[0].Text = nodefathers[0].Name;
                        nodefathers[0].Tag = null;
                    }

                    // 再添加这一个。
                    TreeNode newnpcnode = nodefathers[0].Nodes.Add(npcName, fullDisplayName);
                    newnpcnode.Tag = npc;
                }
            }

            TreeNode nodesDoodad = nodeMap.Nodes.Add(string.Empty, "Doodads");
            m_NodeDoodads = nodesDoodad;
            nodesDoodad.Tag = nodesDoodad.Text;
            foreach (LogicObj doodad in map.Doodads)
            {
                //string doodadName = doodad.naMe;
                string doodadName = string.Format("{0} [{1}]", doodad.naMe, doodad.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", doodadName, doodad.nickName.Length > 0 ? "[" + doodad.nickName + "]" : string.Empty, doodad.hasScript ? "[√]" : string.Empty);
                if (doodad.nickName.Length == 0 && !bShowAll)
                    continue;

                TreeNode[] nodefathers = nodesDoodad.Nodes.Find(doodadName, false);
                if (nodefathers.Length == 0)
                {
                    TreeNode newdoodadnode = nodesDoodad.Nodes.Add(doodadName, fullDisplayName);
                    newdoodadnode.Tag = doodad;
                }
                else if (nodefathers.Length == 1)
                {
                    // 把原来位于分类上的结点变成它自己的孩子
                    if (nodefathers[0].Tag != null)
                    {
                        TreeNode oldnewdoodadnode = nodefathers[0].Nodes.Add(nodefathers[0].Name, nodefathers[0].Text);
                        oldnewdoodadnode.Tag = nodefathers[0].Tag;

                        nodefathers[0].Text = nodefathers[0].Name;
                        nodefathers[0].Tag = null;
                    }

                    // 再添加这一个。
                    TreeNode newdoodadnode = nodefathers[0].Nodes.Add(doodadName, fullDisplayName);
                    newdoodadnode.Tag = doodad;
                }
            }


            /*
            TreeNode nodesAIGroup = nodeMap.Nodes.Add(string.Empty, "AIGroup");
            nodesAIGroup.Tag = nodesAIGroup.Text;
            foreach (LogicObj aigroup in map.AIGroups)
            {
                TreeNode newnodegroup = nodesAIGroup.Nodes.Add(aigroup.inDex.ToString(), aigroup.naMe);
                newnodegroup.Tag = aigroup;

                TreeNode nodeNPCs = newnodegroup.Nodes.Add(string.Empty, "NPCs");
                if (aigroup.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj npc in aigroup.kids1)
                    {
                        //string npcName = npc.naMe;
                        string npcName = string.Format("{0} [{1}]", npc.naMe, npc.templateID);
                        string fullDisplayName = string.Format("{0} {1} {2}", npcName, npc.nickName.Length > 0 ? "[" + npc.nickName + "]" : string.Empty, npc.hasScript ? "[√]" : string.Empty);

                        TreeNode newnodenpc = nodeNPCs.Nodes.Add(npcName, fullDisplayName);
                        newnodenpc.Tag = npc;
                    }
                }
            }

            TreeNode nodesWayPointSet = nodeMap.Nodes.Add(string.Empty, "WayPointSet");
            nodesWayPointSet.Tag = nodesWayPointSet.Text;
            foreach (LogicObj waypointset in map.WayPointSets)
            {
                TreeNode newnodeset = nodesWayPointSet.Nodes.Add(waypointset.inDex.ToString(), waypointset.naMe);
                newnodeset.Tag = waypointset;

                TreeNode nodePoint = newnodeset.Nodes.Add(string.Empty, "Points");
                if (waypointset.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj point in waypointset.kids1)
                    {
                        TreeNode newnodepoint = nodePoint.Nodes.Add(point.inDex.ToString(), "Point" + point.inDex.ToString());
                        newnodepoint.Tag = point;
                    }
                }
            }

            TreeNode nodesLogicalPoly = nodeMap.Nodes.Add(string.Empty, "LogicalPoly");
            nodesLogicalPoly.Tag = nodesLogicalPoly.Text;
            foreach (LogicObj poly in map.LogicalPolys)
            {
                TreeNode newnodepoly = nodesLogicalPoly.Nodes.Add(poly.inDex.ToString(), poly.naMe);
                newnodepoly.Tag = poly;

                TreeNode nodeVertex = newnodepoly.Nodes.Add(string.Empty, "Vertexs");
                if (poly.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj v in poly.kids1)
                    {
                        TreeNode newnodevertex = nodeVertex.Nodes.Add(v.inDex.ToString(), "Vertex" + v.inDex.ToString());
                        newnodevertex.Tag = v;
                    }
                }
            }

            TreeNode nodesTrafficPointSet = nodeMap.Nodes.Add(string.Empty, "TrafficLittlePointSet");
            nodesTrafficPointSet.Tag = nodesTrafficPointSet.Text;
            foreach (LogicObj pointset in map.TrafficPointSets)
            {
                TreeNode newnodePointset = nodesTrafficPointSet.Nodes.Add(pointset.inDex.ToString(), pointset.naMe);
                newnodePointset.Tag = pointset;

                TreeNode nodePoint = newnodePointset.Nodes.Add(string.Empty, "Point");
                if (pointset.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj v in pointset.kids1)
                    {
                        TreeNode newnodePoint = nodePoint.Nodes.Add(v.inDex.ToString(), "Point" + v.inDex.ToString());
                        newnodePoint.Tag = v;
                    }
                }
            }

            TreeNode nodesTrafficPoint = nodeMap.Nodes.Add(string.Empty, "TrafficPoint");
            nodesTrafficPoint.Tag = nodesTrafficPoint.Text;
            foreach (LogicObj trafficpoint in map.TrafficPoints)
            {
                TreeNode newnodepoint = nodesTrafficPoint.Nodes.Add(trafficpoint.inDex.ToString(), trafficpoint.naMe);
                newnodepoint.Tag = trafficpoint;
            }

            TreeNode nodesNpcReviveGroup = nodeMap.Nodes.Add(string.Empty, "NpcReviveGroup");
            nodesNpcReviveGroup.Tag = nodesNpcReviveGroup.Text;
            foreach (LogicObj npcrevivegroup in map.NpcReviveGroups)
            {
                TreeNode newnodegroup = nodesNpcReviveGroup.Nodes.Add(npcrevivegroup.inDex.ToString(), npcrevivegroup.naMe);
                newnodegroup.Tag = npcrevivegroup;

                TreeNode nodeNPCs = newnodegroup.Nodes.Add(string.Empty, "NPCs");
                if (npcrevivegroup.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj npc in npcrevivegroup.kids1)
                    {
                        TreeNode newnodeNpc = nodeNPCs.Nodes.Add(npc.inDex.ToString(), "NPC" + npc.inDex.ToString());
                        newnodeNpc.Tag = npc;
                    }
                }
            }

            TreeNode nodesDoodadReviveGroup = nodeMap.Nodes.Add(string.Empty, "DoodadReviveGroup");
            nodesDoodadReviveGroup.Tag = nodesDoodadReviveGroup.Text;
            foreach (LogicObj Doodadrevivegroup in map.DoodadReviveGroups)
            {
                TreeNode newnodegroup = nodesDoodadReviveGroup.Nodes.Add(Doodadrevivegroup.inDex.ToString(), Doodadrevivegroup.naMe);
                newnodegroup.Tag = Doodadrevivegroup;

                TreeNode nodeDoodads = newnodegroup.Nodes.Add(string.Empty, "Doodads");
                if (Doodadrevivegroup.kids1 != null)
                {
                    //int i = 0;
                    foreach (LogicObj Doodad in Doodadrevivegroup.kids1)
                    {
                        TreeNode newnodeDoodad = nodeDoodads.Nodes.Add(Doodad.inDex.ToString(), "Doodad" + Doodad.inDex.ToString());
                        newnodeDoodad.Tag = Doodad;
                    }
                }
            }

            TreeNode nodesNpcRandomGroup = nodeMap.Nodes.Add(string.Empty, "NpcRandomGroup");
            nodesNpcRandomGroup.Tag = nodesNpcRandomGroup.Text;
            foreach (LogicObj group in map.NpcRandomGroups)
            {
                TreeNode newnodegroup = nodesNpcRandomGroup.Nodes.Add(group.inDex.ToString(), group.naMe);
                newnodegroup.Tag = group;
            }

            TreeNode nodesAISet = nodeMap.Nodes.Add(string.Empty, "AISet");
            nodesAISet.Tag = nodesAISet.Text;
            foreach (LogicObj aiset in map.AISets)
            {
                TreeNode newnodeSet = nodesAISet.Nodes.Add(aiset.inDex.ToString(), aiset.naMe);
                newnodeSet.Tag = newnodeSet;

                TreeNode nodeParams = newnodeSet.Nodes.Add(string.Empty, "Params");
                if (aiset.kids1 != null)
                {
                    foreach (LogicObj param in aiset.kids1)
                    {
                        TreeNode newnodeparam = nodeParams.Nodes.Add(param.inDex.ToString(), param.naMe);
                        newnodeparam.Tag = param;
                    }
                }
            }
            */

            UpdateNodesCountDesc();
        }

        public static void UpdateNodesCountDesc()
        {
            // 删除个数描述
            foreach (TreeNode node in m_nodeMap.Nodes)
            {
                if (node.Text.StartsWith("NPCs") || node.Text.StartsWith("Doodads"))
                {
                    foreach (TreeNode n in node.Nodes)
                    {
                        if (n.Text.Contains("[共"))
                        {
                            n.Text = n.Text.Substring(0, n.Text.IndexOf("[共") - 1);
                        }
                    }
                }

                if (node.Text.Contains("[共"))
                {
                    node.Text = node.Text.Substring(0, node.Text.IndexOf("[共") - 1);
                }
            }

            // 加上个数描述
            foreach (TreeNode node in m_nodeMap.Nodes)
            {
                if (node.Text == "NPCs" || node.Text == "Doodads")
                {
                    foreach (TreeNode n in node.Nodes)
                    {
                        if (n.Nodes.Count > 0)
                            n.Text += string.Format(" [共{0}个]", n.Nodes.Count);
                    }
                }

                node.Text += string.Format(" [共{0}个]", node.Nodes.Count);
            }
        }

    };
}


