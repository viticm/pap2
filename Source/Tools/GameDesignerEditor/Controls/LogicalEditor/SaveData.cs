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

namespace LogicalEditor
{
    public partial class Form1
    {
        private int SaveIniFiles()
        {
            int affectedObjCount = 0;

            foreach (DictionaryEntry entry in maps)
            {
                LO_Map map = entry.Value as LO_Map;

                if (map.MAIN.changed)
                {
                    SaveOneLogicObj(map.MAIN, map.filename, ref affectedObjCount);
                }
                foreach (LogicObj npc in map.NPCs)
                {
                    if (npc.changed)
                    {
                        SaveOneLogicObj(npc, map.filename, ref affectedObjCount);
                    }
                }
                foreach (LogicObj doodad in map.Doodads)
                {
                    if (doodad.changed)
                    {
                        SaveOneLogicObj(doodad, map.filename, ref affectedObjCount);
                    }
                }
                foreach (LogicObj npcrefreshpointset in map.NPCRefreshPointSets)
                {
                    if (npcrefreshpointset.changed)
                    {
                        SaveOneLogicObj(npcrefreshpointset, map.filename, ref affectedObjCount);
                    }
                    if (npcrefreshpointset.kids1 != null)
                    {
                        foreach (LogicObj point in npcrefreshpointset.kids1)
                        {
                            if (point.changed)
                            {
                                SaveOneLogicObj(point, map.filename, ref affectedObjCount);
                            }
                        }
                        foreach (LogicObj npc in npcrefreshpointset.kids2)
                        {
                            if (npc.changed)
                            {
                                SaveOneLogicObj(npc, map.filename, ref affectedObjCount);
                            }
                        }
                    }
                }
                foreach (LogicObj doodadrefreshpointset in map.DoodadRefreshPointSets)
                {
                    if (doodadrefreshpointset.changed)
                    {
                        SaveOneLogicObj(doodadrefreshpointset, map.filename, ref affectedObjCount);
                    }
                    if (doodadrefreshpointset.kids1 != null)
                    {
                        foreach (LogicObj point in doodadrefreshpointset.kids1)
                        {
                            if (point.changed)
                            {
                                SaveOneLogicObj(point, map.filename, ref affectedObjCount);
                            }
                        }
                        foreach (LogicObj doodad in doodadrefreshpointset.kids2)
                        {
                            if (doodad.changed)
                            {
                                SaveOneLogicObj(doodad, map.filename, ref affectedObjCount);
                            }
                        }
                    }
                }
                foreach (LogicObj aigroup in map.AIGroups)
                {
                    if (aigroup.changed)
                    {
                        SaveOneLogicObj(aigroup, map.filename, ref affectedObjCount);
                    }
                    if (aigroup.kids1 != null)
                    {
                        foreach (LogicObj npc in aigroup.kids1)
                        {
                            if (npc.changed)
                            {
                                SaveOneLogicObj(npc, map.filename, ref affectedObjCount);
                            }
                        }
                    }
                }
                foreach (LogicObj waypointset in map.WayPointSets)
                {
                    if (waypointset.changed)
                    {
                        SaveOneLogicObj(waypointset, map.filename, ref affectedObjCount);
                    }
                    if (waypointset.kids1 != null)
                    {
                        foreach (LogicObj waypoint in waypointset.kids1)
                        {
                            if (waypoint.changed)
                            {
                                SaveOneLogicObj(waypoint, map.filename, ref affectedObjCount);
                            }
                        }
                    }
                }
                foreach (LogicObj logicalpoly in map.LogicalPolys)
                {
                    if (logicalpoly.changed)
                    {
                        SaveOneLogicObj(logicalpoly, map.filename, ref affectedObjCount);
                    }
                    if (logicalpoly.kids1 != null)
                    {
                        foreach (LogicObj vertex in logicalpoly.kids1)
                        {
                            if (vertex.changed)
                            {
                                SaveOneLogicObj(vertex, map.filename, ref affectedObjCount);
                            }
                        }
                    }
                }
            }

            return affectedObjCount;
        }

        private void SaveOneLogicObj(LogicObj lobj, string strFileName, ref int affectedObjCount)
        {
            affectedObjCount++;

            string iniBlock = string.Empty;
            string iniFieldPrefix = string.Empty;
            int inDex = lobj.inDex;
            int parentindex = lobj.parentindex;

            switch (lobj.tyPe)
            {
                case "MAIN":
                    iniBlock = "MAIN";
                    break;

                case "NPC":
                    iniBlock = "NPC" + inDex.ToString();
                    break;
                case "Doodad":
                    iniBlock = "Doodad" + inDex.ToString();
                    break;

                case "NPCRefreshPointSet":
                    iniBlock = "NPCRefreshPointSet" + inDex.ToString();
                    break;
                case "NPCRefreshPoint":
                    iniBlock = "NPCRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;
                case "NPCRefreshNPC":
                    iniBlock = "NPCRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "NPC" + inDex.ToString();
                    break;

                case "DoodadRefreshPointSet":
                    iniBlock = "DoodadRefreshPointSet" + inDex.ToString();
                    break;
                case "DoodadRefreshPoint":
                    iniBlock = "DoodadRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;
                case "DoodadRefreshDoodad":
                    iniBlock = "DoodadRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Doodad" + inDex.ToString();
                    break;

                case "AIGroup":
                    iniBlock = "AIGroup" + inDex.ToString();
                    break;
                case "AIGroupNPC":
                    iniBlock = "AIGroup" + parentindex.ToString();
                    iniFieldPrefix = "NPC" + inDex.ToString();
                    break;

                case "WayPointSet":
                    iniBlock = "WayPointSet" + inDex.ToString();
                    break;
                case "WayPoint":
                    iniBlock = "WayPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;

                case "LogicalPoly":
                    iniBlock = "LogicalPoly" + inDex.ToString();
                    break;
                case "LogicalPolyVertex":
                    iniBlock = "LogicalPoly" + parentindex.ToString();
                    iniFieldPrefix = "Vertex" + inDex.ToString();
                    break;
            }

            StringBuilder sb = new StringBuilder(255);
            for (int i = 0; i < lobj.fields.Length; i++)
            {
                Helper.WriteIniFile(iniBlock, iniFieldPrefix + lobj.fields[i], lobj.values[i], strFileName);
            }

            lobj.ForceInhandChange(false);
            lobj.ForcePreviewChange(false);
        }
    }
}