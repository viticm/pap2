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
        private void LoadTree(string mapName)
        {
            // 添加结点
            foreach (DictionaryEntry entry in maps)
            {
                LO_Map map = entry.Value as LO_Map;
                if (map.name != mapName) continue;

                TreeNode nodeMap = tree1.Nodes.Add(map.name, map.name);
                nodeMap.Tag = map.name;

                TreeNode nodesNPC = nodeMap.Nodes.Add(string.Empty, "NPCs");
                nodesNPC.Tag = nodesNPC.Text;
                foreach (LogicObj npc in map.NPCs)
                {
                    string npcName = npc.values[0];
                    string fullDisplayName = string.Format("{0} [{1}] {2} {3}", npcName, npc.inDex, npc.values[1].Length != 0 ? "[" + npc.values[1] + "]" : string.Empty, npc.values[2].Length != 0 ? "[√]" : string.Empty);

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
                nodesDoodad.Tag = nodesDoodad.Text;
                foreach (LogicObj doodad in map.Doodads)
                {
                    string doodadName = doodad.values[0];
                    string fullDisplayName = string.Format("{0} [{1}] {2} {3}", doodadName, doodad.inDex, doodad.values[1].Length != 0 ? "[" + doodad.values[1] + "]" : string.Empty, doodad.values[2].Length != 0 ? "[√]" : string.Empty);

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

                TreeNode nodesNPCRefreshPointSet = nodeMap.Nodes.Add(string.Empty, "NPCRefreshPointSets");
                nodesNPCRefreshPointSet.Tag = nodesNPCRefreshPointSet.Text;
                foreach (LogicObj npcrefreshpointset in map.NPCRefreshPointSets)
                {
                    TreeNode newnodeSet = nodesNPCRefreshPointSet.Nodes.Add(npcrefreshpointset.inDex.ToString(), npcrefreshpointset.values[0]);
                    newnodeSet.Tag = npcrefreshpointset;

                    TreeNode nodePoints = newnodeSet.Nodes.Add(string.Empty, "Points");
                    if (npcrefreshpointset.kids1 != null)
                    {
                        //int i = 0;
                        foreach (LogicObj point in npcrefreshpointset.kids1)
                        {
                            TreeNode newnodePoint = nodePoints.Nodes.Add(point.inDex.ToString(), "Point" + point.inDex.ToString());
                            newnodePoint.Tag = point;
                        }
                    }
                    TreeNode nodeNPCs = newnodeSet.Nodes.Add(string.Empty, "NPCs");
                    if (npcrefreshpointset.kids2 != null)
                    {
                        //int i = 0;
                        foreach (LogicObj npc in npcrefreshpointset.kids2)
                        {
                            TreeNode newnodenpc = nodeNPCs.Nodes.Add(npc.inDex.ToString(), "NPC" + npc.inDex.ToString());
                            newnodenpc.Tag = npc;
                        }
                    }
                }

                TreeNode nodesDoodadRefreshPointSet = nodeMap.Nodes.Add(string.Empty, "DoodadRefreshPointSet");
                nodesDoodadRefreshPointSet.Tag = nodesDoodadRefreshPointSet.Text;
                foreach (LogicObj doodadrefreshpointset in map.DoodadRefreshPointSets)
                {
                    TreeNode newnodeSet = nodesDoodadRefreshPointSet.Nodes.Add(doodadrefreshpointset.inDex.ToString(), doodadrefreshpointset.values[0]);
                    newnodeSet.Tag = doodadrefreshpointset;

                    TreeNode nodePoints = newnodeSet.Nodes.Add(string.Empty, "Points");
                    if (doodadrefreshpointset.kids1 != null)
                    {
                        //int i = 0;
                        foreach (LogicObj point in doodadrefreshpointset.kids1)
                        {
                            TreeNode newnodepoint = nodePoints.Nodes.Add(point.inDex.ToString(), "Point" + point.inDex.ToString());
                            newnodepoint.Tag = point;
                        }
                    }
                    TreeNode nodeDoodads = newnodeSet.Nodes.Add(string.Empty, "Doodads");
                    if (doodadrefreshpointset.kids2 != null)
                    {
                        //int i = 0;
                        foreach (LogicObj doodad in doodadrefreshpointset.kids2)
                        {
                            TreeNode newnodedoodad = nodeDoodads.Nodes.Add(doodad.inDex.ToString(), "Doodad" + doodad.inDex.ToString());
                            newnodedoodad.Tag = doodad;
                        }
                    }
                }

                TreeNode nodesAIGroup = nodeMap.Nodes.Add(string.Empty, "AIGroup");
                nodesAIGroup.Tag = nodesAIGroup.Text;
                foreach (LogicObj aigroup in map.AIGroups)
                {
                    TreeNode newnodegroup = nodesAIGroup.Nodes.Add(aigroup.inDex.ToString(), aigroup.values[0]);
                    newnodegroup.Tag = aigroup;

                    TreeNode nodeNPCs = newnodegroup.Nodes.Add(string.Empty, "NPCs");
                    if (aigroup.kids1 != null)
                    {
                        //int i = 0;
                        foreach (LogicObj npc in aigroup.kids1)
                        {
                            TreeNode newnodenpc = nodeNPCs.Nodes.Add(npc.inDex.ToString(), "NPC" + npc.inDex.ToString());
                            newnodenpc.Tag = npc;
                        }
                    }
                }

                TreeNode nodesWayPointSet = nodeMap.Nodes.Add(string.Empty, "WayPointSet");
                nodesWayPointSet.Tag = nodesWayPointSet.Text;
                foreach (LogicObj waypointset in map.WayPointSets)
                {
                    TreeNode newnodeset = nodesWayPointSet.Nodes.Add(waypointset.inDex.ToString(), waypointset.values[0]);
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
                    TreeNode newnodepoly = nodesLogicalPoly.Nodes.Add(poly.inDex.ToString(), poly.values[6]);
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

                // 加上个数描述
                foreach (TreeNode node in nodeMap.Nodes)
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
        }
    }
}