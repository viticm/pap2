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
using System.Drawing.Design;
using System.Drawing.Drawing2D;


namespace LogicalEditor
{
    public partial class Form1
    {
        private void ScanAllLogicalIniFiles()
        {
            DirectoryInfo di = new DirectoryInfo(m_MapsPath);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                if (!d.Name.ToLower().Contains("minimap")) // 例如：[纯阳] 是我们要找的目录，[纯阳minimap] 则不是。
                {
                    string target = d.FullName + "\\" + d.Name + ".Map.Logical";
                    if (File.Exists(target)) // 找到.Map.Logical文件。
                    {
                        m_LogicIniFiles.Add(target);

                        // load 按钮的初始化
                        DevComponents.DotNetBar.ButtonItem item = new DevComponents.DotNetBar.ButtonItem(d.Name, d.Name);
                        item.Tag = target;
                        item.AutoCheckOnClick = true; 
                        item.Click += new EventHandler(this.loadButtonItem_Click);
                        this.buttonOpenMap.SubItems.Add(item);
                    }
                }
            }
        }
        private bool CheckFilesCount()
        {
            if (m_LogicIniFiles.Count > 0)
            {
                return true;
            }
            else
            {
                MessageBox.Show("目标路径下连一个 .Map.Logical 都找不到。\r\n请选择正确的 /Client/data/source/maps 目录。");
                return false;
            }
        }
        private void DoImportOneTarget(string strFile, string mapname)
        {
            this.progressBar1.Visible = true;

            // 读文件
            string content = Helper.FileToString(strFile, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(strFile, content);
            if (Helper.m_inis == null)
            {
                string strErr = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", strFile);
                MessageBox.Show(strErr);
                return;
            }

            // map 对象
            LO_Map map = new LO_Map();
            maps.Add(mapname, map);
            //maps.Add(map);

            map.logicalChanged = false;
            map.name = mapname;
            map.filename = strFile;
            LogicObj main = new LogicObj("MAIN");
            main.Read();
            map.MAIN = main;
            map.NPCs = new LogicObj[Convert.ToInt32(map.MAIN.values[0])];
            map.Doodads = new LogicObj[Convert.ToInt32(map.MAIN.values[1])];
            map.NPCRefreshPointSets = new LogicObj[Convert.ToInt32(map.MAIN.values[2])];
            map.DoodadRefreshPointSets = new LogicObj[Convert.ToInt32(map.MAIN.values[4])];
            map.AIGroups = new LogicObj[Convert.ToInt32(map.MAIN.values[6])];
            map.WayPointSets = new LogicObj[Convert.ToInt32(map.MAIN.values[8])];
            map.LogicalPolys = new LogicObj[Convert.ToInt32(map.MAIN.values[13])];

            this.progressBar1.Step = 1;
            this.progressBar1.Minimum = 0;

            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[0]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[0]); i++)
            {
                LogicObj npc = new LogicObj("NPC");
                npc.inDex = i;
                npc.Read();
                map.NPCs[i] = npc;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[1]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[1]); i++)
            {
                LogicObj doodad = new LogicObj("Doodad");
                doodad.inDex = i;
                doodad.Read();
                map.Doodads[i] = doodad;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[2]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[2]); i++)
            {
                LogicObj npcrefreshset = new LogicObj("NPCRefreshPointSet");
                npcrefreshset.inDex = i;
                npcrefreshset.Read();
                npcrefreshset.kids1 = new LogicObj[Convert.ToInt32(npcrefreshset.values[2])];
                npcrefreshset.kids2 = new LogicObj[Convert.ToInt32(npcrefreshset.values[3])];
                for (int j = 0; j < Convert.ToInt32(npcrefreshset.values[2]); j++)
                {
                    LogicObj point = new LogicObj("NPCRefreshPoint");
                    point.inDex = j;
                    point.parentindex = i;
                    point.Read();
                    npcrefreshset.kids1[j] = point;
                }
                for (int j = 0; j < Convert.ToInt32(npcrefreshset.values[3]); j++)
                {
                    LogicObj npc = new LogicObj("NPCRefreshNPC");
                    npc.inDex = j;
                    npc.parentindex = i;
                    npc.Read();
                    npcrefreshset.kids2[j] = npc;
                }
                map.NPCRefreshPointSets[i] = npcrefreshset;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[4]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[4]); i++)
            {
                LogicObj doodadrefreshset = new LogicObj("DoodadRefreshPointSet");
                doodadrefreshset.inDex = i;
                doodadrefreshset.Read();
                doodadrefreshset.kids1 = new LogicObj[Convert.ToInt32(doodadrefreshset.values[2])];
                doodadrefreshset.kids2 = new LogicObj[Convert.ToInt32(doodadrefreshset.values[3])];
                for (int j = 0; j < Convert.ToInt32(doodadrefreshset.values[2]); j++)
                {
                    LogicObj point = new LogicObj("DoodadRefreshPoint");
                    point.inDex = j;
                    point.parentindex = i;
                    point.Read();
                    doodadrefreshset.kids1[j] = point;
                }
                for (int j = 0; j < Convert.ToInt32(doodadrefreshset.values[3]); j++)
                {
                    LogicObj doodad = new LogicObj("DoodadRefreshDoodad");
                    doodad.inDex = j;
                    doodad.parentindex = i;
                    doodad.Read();
                    doodadrefreshset.kids2[j] = doodad;
                }
                map.DoodadRefreshPointSets[i] = doodadrefreshset;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[6]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[6]); i++)
            {
                LogicObj aigroup = new LogicObj("AIGroup");
                aigroup.inDex = i;
                aigroup.Read();
                aigroup.kids1 = new LogicObj[Convert.ToInt32(aigroup.values[5])];
                for (int j = 0; j < Convert.ToInt32(aigroup.values[5]); j++)
                {
                    LogicObj npc = new LogicObj("AIGroupNPC");
                    npc.inDex = j;
                    npc.parentindex = i;
                    npc.Read();
                    aigroup.kids1[j] = npc;
                }
                map.AIGroups[i] = aigroup;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[8]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[8]); i++)
            {
                LogicObj waypointset = new LogicObj("WayPointSet");
                waypointset.inDex = i;
                waypointset.Read();
                waypointset.kids1 = new LogicObj[Convert.ToInt32(waypointset.values[2])];
                for (int j = 0; j < Convert.ToInt32(waypointset.values[2]); j++)
                {
                    LogicObj waypoint = new LogicObj("WayPoint");
                    waypoint.inDex = j;
                    waypoint.parentindex = i;
                    waypoint.Read();
                    waypointset.kids1[j] = waypoint;
                }
                map.WayPointSets[i] = waypointset;
                this.progressBar1.Value++;
            }
            this.progressBar1.Value = 0;
            this.progressBar1.Maximum = Convert.ToInt32(map.MAIN.values[13]);
            for (int i = 0; i < Convert.ToInt32(map.MAIN.values[13]); i++)
            {
                LogicObj logicalpoly = new LogicObj("LogicalPoly");
                logicalpoly.inDex = i;
                logicalpoly.Read();
                logicalpoly.kids1 = new LogicObj[Convert.ToInt32(logicalpoly.values[4])];
                for (int j = 0; j < Convert.ToInt32(logicalpoly.values[4]); j++)
                {
                    LogicObj vertex = new LogicObj("LogicalPolyVertex");
                    vertex.inDex = j;
                    vertex.parentindex = i;
                    vertex.Read();
                    logicalpoly.kids1[j] = vertex;
                }
                map.LogicalPolys[i] = logicalpoly;
                this.progressBar1.Value++;
            }
            this.progressBar1.Visible = false;

            Helper.CloseIniFile();
        }
    }
}