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
using System.Threading;
using System.Diagnostics;
using LuaInterface;
using DevComponents.AdvTree;
using DevComponents.DotNetBar;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    unsafe public partial class BaseForm
	{
        private const int REPRESENTOBJECT_NPC = 1;
        private const int REPRESENTOBJECT_DOODAD = 8;
        private const int REPRESENTOBJECT_WAYPOINT = 9;
        private const int REPRESENTOBJECT_POLY = 11;


        private void InitVars()
        {
            for (int i = 0; i < CAMERA_STORAGE; i++)
            {
                m_bPosSaved[i] = false;
                m_SavedCameraPos[i] = new _AtlVector3();
                m_SavedCameraLookat[i] = new _AtlVector3();
            }

            m_types = new string[] {
                "MAIN", "NPC", "Doodad", "NPCRefreshPointSet",
                "NPCRefreshPoint", "NPCRefreshNPC", "DoodadRefreshPointSet", "DoodadRefreshPoint",
                "DoodadRefreshDoodad", "AIGroup", "AIGroupNPC", "WayPointSet",
                "WayPoint", "LogicalPoly", "LogicalPolyVertex"
                };

            // vPosition所在index
            htObjScenePosIndices = new Hashtable();
            htObjScenePosIndices.Add("NPC", 14);
            htObjScenePosIndices.Add("Doodad", 12);
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
            htObjLogicRotationIndices.Add("NPC", 15);
            htObjLogicRotationIndices.Add("Doodad", 13);
            htObjLogicRotationIndices.Add("NPCRefreshPoint", 1);
            htObjLogicRotationIndices.Add("DoodadRefreshPoint", 1);
            htObjLogicRotationIndices.Add("WayPoint", 1);

        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            LogicObj target = e.Node.Tag as LogicObj;
            if (target != null && target.RepresentObjPtr != 0)
            {
                MoveCameraToRepresentObj(target.RepresentObjPtr);
                SelectOnlyRepresentObj(target.RepresentObjPtr);
            }

            previewform.m_panel1.Refresh();
        }

        private void GetSceneSize(string mapName)
        {
            // 启动引擎读。
            //int selectedScenePtr = (int)htScenePtr[mapname];
            //Program.Engine.ReadMapTerrainInfo(selectedScenePtr, out curMapWidth, out curMapHeight);
            //curMapWidth *= 1600;
            //curMapHeight *= 1600;

            // 读配置ini。
            string strRISetting = string.Format("{0}/data/source/maps/{1}/{1}RegionInfo/RISettings.ini", EditorLayer.EngineDir, mapName);
            FileInfo fi = new FileInfo(strRISetting);
            if (!fi.Exists) return;
            StringBuilder sb = new StringBuilder(255);
            Helper.ReadIniFile("RLSplit", "RegionCountXZ", string.Empty, sb, 255, strRISetting);
            string[] results = sb.ToString().Split(new char[] { ',' });
            curMapWidth = Convert.ToInt32(results[0]) * 1600;
            curMapHeight = Convert.ToInt32(results[1]) * 1600;

            // 直接从.Map文件读取region数目。
            //string strMap = string.Format("{0}/data/source/maps/{1}/{1}.Map", m_BasePath, mapname);
            //FileStream fs = new FileStream(strMap, FileMode.Open, FileAccess.Read);
            //BinaryReader r = new BinaryReader(fs);
            //r.ReadBytes(32);
            //curMapWidth = r.ReadInt32() * 1600;
            //curMapHeight = r.ReadInt32() * 1600;
            //fs.Close();
        }
        private void ShowMap(string mapname)
        {
            if (mapname == "测试1") mapname = "测试";

            if (mapname != m_curMapName)
            {
                m_curMapName = mapname;

                GetSceneSize(mapname);

                string strJpg = string.Format("{0}/data/source/maps/{1}minimap/middlemap.tga", EditorLayer.EngineDir, mapname);
                previewform.LoadPicture(strJpg);
            }
            previewform.Show();
        }

        private void treeView1_AfterSelNodesAdd(object sender, MWControlSuite.MWPropertyEventArgs e)
        {
            TreeNode node = e.Updated as TreeNode;

            // 显示图片
            ShowMap(Helper.AncestorNodeOf(node).Tag.ToString());

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

        private void treeView1_AfterSelNodesRemove(object sender, MWControlSuite.MWPropertyEventArgs e)
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

        private PointF LogicObjPosToImagePointF(LogicObj target)
        {
            return PointF.Empty;
            //             if (htObjScenePosIndices.ContainsKey(target.tyPe))
            //             {
            //                 string vPosition = target.values[Convert.ToInt32(htObjScenePosIndices[target.tyPe])];
            //                 string[] fields = vPosition.Split(new char[] { ',' });
            //                 double x = Convert.ToDouble(fields[0]);
            //                 double y = Convert.ToDouble(fields[2]);
            //                 float px = (float)(x / curMapWidth);
            //                 float py = (float)(1.0 - y / curMapHeight);
            //                 PointF pfResult = new PointF(px, py);
            //                 return pfResult;
            //             }
            //             else
            //             {
            //                 return PointF.Empty;
            //             }
        }

        private int LogicObjToDirection(LogicObj target)
        {
            return -1;
            //             if (htObjLogicDirIndices.ContainsKey(target.tyPe))
            //             {
            //                 string nDirection = target.values[Convert.ToInt32(htObjLogicDirIndices[target.tyPe])];
            //                 return Convert.ToInt32(nDirection);
            //             }
            //             else
            //             {
            //                 return -1;
            //             }
        }

        public object FindTreeNodeByRepresentObjPtr(int representObjType, int representObjPtr)
        {
            object nodeResult = null;

            if (representObjType == REPRESENTOBJECT_NPC)
            {
                foreach (TreeNode node in LogicHelper.m_NodeNPCs.Nodes)
                {
                    LogicObj npc = node.Tag as LogicObj;
                    if (npc != null && npc.RepresentObjPtr == representObjPtr)
                    {
                        nodeResult = node;
                        break;
                    }

                    foreach (TreeNode n in node.Nodes)
                    {
                        npc = n.Tag as LogicObj;
                        if (npc != null && npc.RepresentObjPtr == representObjPtr)
                        {
                            nodeResult = n;
                            break;
                        }
                    }
                }
            }
            else if (representObjType == REPRESENTOBJECT_DOODAD)
            {
                foreach (TreeNode node in LogicHelper.m_NodeDoodads.Nodes)
                {
                    LogicObj doodad = node.Tag as LogicObj;
                    if (doodad != null && doodad.RepresentObjPtr == representObjPtr)
                    {
                        nodeResult = node;
                        break;
                    }

                    foreach (TreeNode n in node.Nodes)
                    {
                        doodad = n.Tag as LogicObj;
                        if (doodad != null && doodad.RepresentObjPtr == representObjPtr)
                        {
                            nodeResult = n;
                            break;
                        }
                    }
                }
            }
            else if (representObjType == REPRESENTOBJECT_POLY)
            {
                foreach (Node node in this.advTreePoly.Nodes[0].Nodes)
                {
                    _AtlObjInfo atlPolyInfo = (_AtlObjInfo)node.Tag;
                    if (atlPolyInfo.iRepresentObjPtr == representObjPtr)
                    {
                        nodeResult = node;
                        break;
                    }
                }
            }
            else if (representObjType == REPRESENTOBJECT_POLY+1)
            {
                foreach (Node node in this.nodeBrushes.Nodes)
                {
                    _AtlObjInfo atlBrushInfo = (_AtlObjInfo)node.Tag;
                    if (atlBrushInfo.iLogicObjPtr/*这里没有写错*/ == representObjPtr)
                    {
                        nodeResult = node;
                        break;
                    }
                }
            }

            if (nodeResult == null)
                if (representObjType == REPRESENTOBJECT_NPC || representObjType == REPRESENTOBJECT_DOODAD)
                    throw new Exception("找不到指定树结点。");

            return nodeResult;
        }

	}
}
