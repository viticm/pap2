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
using LuaInterface;
using DevComponents.DotNetBar;
using DevComponents.AdvTree;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    unsafe public partial class BaseForm
	{
        private Node lastSearchNpcNode = null; // 上一次搜索到的npc

        /// <summary>
        /// 初始化Npc面板 ahpho suntao
        /// </summary>
        private void InitNpc()
        {
            // 初始化数据表
            string sqlString = "SELECT ID, Name, RepresentID1, MapName FROM NpcTemplate";
            npcTable = Helper.GetDataTable(sqlString, Conn);
            sqlString = "SELECT * FROM npc";
            npcRepresentTable = Helper.GetDataTable(sqlString, Conn);

            // 构建分类树            
            npcTree.Nodes.Clear();
            foreach (DataRow row in npcTable.Rows)
            {
                string npcID = row["ID"].ToString();
                string npcName = row["Name"].ToString();
                string mapName = row["MapName"].ToString();
                string representID = row["RepresentID1"].ToString();

                if (mapName == "")
                {
                    mapName = "未分类";
                }

                Node newNode = new Node();
                newNode.Text = string.Format("{0} ({1})", npcName, npcID);
                npcInfoTable[newNode] = row;

                if (representID != "")
                {
                    DataRow[] rows = npcRepresentTable.Select(string.Format("RepresentID = '{0}'", representID));
                    if (rows.Length > 0)
                    {
                        string modelFile = rows[0]["MainModelFile"].ToString();
                        string faceMeshFile = rows[0]["S_Face_MeshFile"].ToString();
                        string faceMaterialFile = rows[0]["S_Face_MaterialFile"].ToString();
                        string hatMeshFile = rows[0]["S_Hat_MeshFile"].ToString();
                        string hatMaterialFile = rows[0]["S_Hat_MaterialFile"].ToString();
                        string leftHandMeshFile = rows[0]["S_LH_MeshFile"].ToString();
                        string leftHandMaterialFile = rows[0]["S_LH_MaterialFile"].ToString();
                        string lpMeshFile = rows[0]["S_LP_MeshFile"].ToString();
                        string lpMaterialFile = rows[0]["S_LP_MaterialFile"].ToString();
                        string lcMeshFile = rows[0]["S_LC_MeshFile"].ToString();
                        string lcMaterialFile = rows[0]["S_LC_MaterialFile"].ToString();
                        string rightHandMeshFile = rows[0]["S_RH_MeshFile"].ToString();
                        string rightHandMaterialFile = rows[0]["S_RH_MaterialFile"].ToString();
                        string rpMeshFile = rows[0]["S_RP_MeshFile"].ToString();
                        string rpMaterialFile = rows[0]["S_RP_MaterialFile"].ToString();
                        string rcMeshFile = rows[0]["S_RC_MeshFile"].ToString();
                        string rcMaterialFile = rows[0]["S_RC_MaterialFile"].ToString();
                        string longMeshFile = rows[0]["S_Long_MeshFile"].ToString();
                        string longMaterialFile = rows[0]["S_Long_MaterialFile"].ToString();
                        string spineMeshFile = rows[0]["S_Spine_MeshFile"].ToString();
                        string spineMaterialFile = rows[0]["S_Spine_MaterialFile"].ToString();
                        string spine2MeshFile = rows[0]["S_Spine2_MeshFile"].ToString();
                        string spine2MaterialFile = rows[0]["S_Spine2_MaterialFile"].ToString();

                        if (modelFile != "")
                        {
                            modelFile = Path.Combine(rootDir, modelFile);
                        }

                        if (faceMeshFile != "")
                        {
                            faceMeshFile = Path.Combine(rootDir, faceMeshFile);
                        }

                        if (faceMaterialFile != "")
                        {
                            faceMaterialFile = Path.Combine(rootDir, faceMaterialFile);
                        }

                        if (hatMeshFile != "")
                        {
                            hatMeshFile = Path.Combine(rootDir, hatMeshFile);
                        }

                        if (hatMaterialFile != "")
                        {
                            hatMaterialFile = Path.Combine(rootDir, hatMaterialFile);
                        }

                        if (leftHandMeshFile != "")
                        {
                            leftHandMeshFile = Path.Combine(rootDir, leftHandMeshFile);
                        }

                        if (leftHandMaterialFile != "")
                        {
                            leftHandMaterialFile = Path.Combine(rootDir, leftHandMaterialFile);
                        }

                        if (lpMeshFile != "")
                        {
                            lpMeshFile = Path.Combine(rootDir, lpMeshFile);
                        }

                        if (lpMaterialFile != "")
                        {
                            lpMaterialFile = Path.Combine(rootDir, lpMaterialFile);
                        }

                        if (lcMeshFile != "")
                        {
                            lcMeshFile = Path.Combine(rootDir, lcMeshFile);
                        }

                        if (lcMaterialFile != "")
                        {
                            lcMaterialFile = Path.Combine(rootDir, lcMaterialFile);
                        }

                        if (rightHandMeshFile != "")
                        {
                            rightHandMeshFile = Path.Combine(rootDir, rightHandMeshFile);
                        }

                        if (rightHandMaterialFile != "")
                        {
                            rightHandMaterialFile = Path.Combine(rootDir, rightHandMaterialFile);
                        }

                        if (rpMeshFile != "")
                        {
                            rpMeshFile = Path.Combine(rootDir, rpMeshFile);
                        }

                        if (rpMaterialFile != "")
                        {
                            rpMaterialFile = Path.Combine(rootDir, rpMaterialFile);
                        }

                        if (rcMeshFile != "")
                        {
                            rcMeshFile = Path.Combine(rootDir, rcMeshFile);
                        }

                        if (rcMaterialFile != "")
                        {
                            rcMaterialFile = Path.Combine(rootDir, rcMaterialFile);
                        }

                        if (longMeshFile != "")
                        {
                            longMeshFile = Path.Combine(rootDir, longMeshFile);
                        }

                        if (longMaterialFile != "")
                        {
                            longMaterialFile = Path.Combine(rootDir, longMaterialFile);
                        }

                        if (spineMeshFile != "")
                        {
                            spineMeshFile = Path.Combine(rootDir, spineMeshFile);
                        }

                        if (spineMaterialFile != "")
                        {
                            spineMaterialFile = Path.Combine(rootDir, spineMaterialFile);
                        }

                        if (spine2MeshFile != "")
                        {
                            spine2MeshFile = Path.Combine(rootDir, spine2MeshFile);
                        }

                        if (spine2MaterialFile != "")
                        {
                            spine2MaterialFile = Path.Combine(rootDir, spine2MaterialFile);
                        }

                        Hashtable infoTable = new Hashtable();
                        infoTable["modelFile"] = modelFile;
                        infoTable["faceMeshFile"] = faceMeshFile;
                        infoTable["faceMaterialFile"] = faceMaterialFile;
                        infoTable["hatMeshFile"] = hatMeshFile;
                        infoTable["hatMaterialFile"] = hatMaterialFile;
                        infoTable["leftHandMeshFile"] = leftHandMeshFile;
                        infoTable["leftHandMaterialFile"] = leftHandMaterialFile;
                        infoTable["lpMeshFile"] = lpMeshFile;
                        infoTable["lpMaterialFile"] = lpMaterialFile;
                        infoTable["lcMeshFile"] = lcMeshFile;
                        infoTable["lcMaterialFile"] = lcMaterialFile;
                        infoTable["rightHandMeshFile"] = rightHandMeshFile;
                        infoTable["rightHandMaterialFile"] = rightHandMaterialFile;
                        infoTable["rpMeshFile"] = rpMeshFile;
                        infoTable["rpMaterialFile"] = rpMaterialFile;
                        infoTable["rcMeshFile"] = rcMeshFile;
                        infoTable["rcMaterialFile"] = rcMaterialFile;
                        infoTable["longMeshFile"] = longMeshFile;
                        infoTable["longMaterialFile"] = longMaterialFile;
                        infoTable["spineMeshFile"] = spineMeshFile;
                        infoTable["spineMaterialFile"] = spineMaterialFile;
                        infoTable["spine2MeshFile"] = spine2MeshFile;
                        infoTable["spine2MaterialFile"] = spine2MaterialFile;                        

                        newNode.Tag = infoTable;
                    }
                }

                Node mapNode = null;

                foreach (Node node in npcTree.Nodes)
                {
                    if (node.Text == mapName)
                    {
                        mapNode = node;
                        break;
                    }
                }

                if (mapNode == null)
                {
                    mapNode = new Node();
                    mapNode.Text = mapName;
                    npcTree.Nodes.Add(mapNode);
                }

                mapNode.Nodes.Add(newNode);
            }

            // 加载场景
            previewCanvas1.PreviewType = "NPC";
            EngineLayer.ATLBase.LoadNpcScene((int)previewCanvas1.Handle);
        }

        /// <summary>
        /// 获取普通待机动作文件名
        /// </summary>
        /// <param name="modelFile">模型路径</param>
        /// <returns>普通待机动作文件名</returns>
        private string GetCommonStandbyAnimationFileName(string modelFile)
        {
            string commonStandbyAnimationFileName = null;

            // 获取动作文件路径
            string[] data = modelFile.Split(new char[] { '\\' });
            string rootFolder = "";
            for (int i = 0; i < data.Length - 2; i++)
            {
                rootFolder += string.Format("{0}\\", data[i]);
            }
            rootFolder = rootFolder + "动作";

            DirectoryInfo di = new DirectoryInfo(rootFolder);

            if (di.Exists)
            {
                foreach (FileInfo fi in di.GetFiles())
                {
                    if (fi.Extension == ".ani" && fi.Name.Contains("_st")) // 检查是否是普通待机动作命名
                    {
                        commonStandbyAnimationFileName = fi.FullName;
                    }
                }
            }

            return commonStandbyAnimationFileName;
        }

        /// <summary>
        /// 选择Npc
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void npcTree_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = npcTree.SelectedNode;

            if (currentNode != null)
            {
                if (currentNode.Level == 0) // 选择分类
                {
                    previewCanvas1.BeginInit = false;
                    buttonX19.Enabled = false;
                    buttonX20.Enabled = false;
                }
                else // 选择Npc
                {
                    Hashtable infoTable = currentNode.Tag as Hashtable;
                    if (infoTable == null)
                        return;
                    string modelFile = infoTable["modelFile"] as string;
                    string faceMeshFile = infoTable["faceMeshFile"] as string;
                    string faceMaterialFile = infoTable["faceMaterialFile"] as string;
                    string hatMeshFile = infoTable["hatMeshFile"] as string;
                    string hatMaterialFile = infoTable["hatMaterialFile"] as string;
                    string leftHandMeshFile = infoTable["leftHandMeshFile"] as string;
                    string leftHandMaterialFile = infoTable["leftHandMaterialFile"] as string;
                    string lpMeshFile = infoTable["lpMeshFile"] as string;
                    string lpMaterialFile = infoTable["lpMaterialFile"] as string;
                    string lcMeshFile = infoTable["lcMeshFile"] as string;
                    string lcMaterialFile = infoTable["lcMaterialFile"] as string;
                    string rightHandMeshFile = infoTable["rightHandMeshFile"] as string;
                    string rightHandMaterialFile = infoTable["rightHandMaterialFile"] as string;
                    string rpMeshFile = infoTable["rpMeshFile"] as string;
                    string rpMaterialFile = infoTable["rpMaterialFile"] as string;
                    string rcMeshFile = infoTable["rcMeshFile"] as string;
                    string rcMaterialFile = infoTable["rcMaterialFile"] as string;
                    string longMeshFile = infoTable["longMeshFile"] as string;
                    string longMaterialFile = infoTable["longMaterialFile"] as string;
                    string spineMeshFile = infoTable["spineMeshFile"] as string;
                    string spineMaterialFile = infoTable["spineMaterialFile"] as string;
                    string spine2MeshFile = infoTable["spine2MeshFile"] as string;
                    string spine2MaterialFile = infoTable["spine2MaterialFile"] as string;

                    // 获取模型文件路径
                    EngineLayer.ATLBase.LoadNpcModel(modelFile);

                    // 要先播普通待机动作才能看到脸部插槽
                    // string commonStandbyAnimationFileName = GetCommonStandbyAnimationFileName(modelFile);
                    // if (commonStandbyAnimationFileName != null)
                    // {
                    //     EngineLayer.ATLBase.ModelPlayAnimation(commonStandbyAnimationFileName);
                    // }

                    if (faceMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadFace(faceMeshFile);
                    }

                    if (hatMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(hatMeshFile, "s_hat");
                    }

                    if (leftHandMeshFile  != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(leftHandMeshFile, "s_lh");
                    }

                    if (lpMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(lpMeshFile, "s_lp");
                    }

                    if (lcMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(lcMeshFile, "s_lc");
                    }

                    if (hatMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(hatMeshFile, "s_hat");
                    }

                    if (rightHandMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(rightHandMeshFile, "s_rh");
                    }

                    if (rpMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(rpMeshFile, "s_rp");
                    }

                    if (rcMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(rcMeshFile, "s_rc");
                    }

                    if (longMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(longMeshFile, "s_long");
                    }

                    if (spineMeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(spineMeshFile, "s_spine");
                    }

                    if (spine2MeshFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginModel(spine2MeshFile, "s_spine2");
                    }

                    if (faceMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_face", faceMaterialFile);
                    }

                    if (hatMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_hat", hatMaterialFile);
                    }

                    if (leftHandMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_lh", leftHandMaterialFile);
                    }

                    if (lpMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_lp", lpMaterialFile);
                    }

                    if (lcMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_lc", lcMaterialFile);
                    }

                    if (rightHandMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_rh", rightHandMaterialFile);
                    }

                    if (rpMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_rp", rpMaterialFile);
                    }

                    if (rcMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_rc", rcMaterialFile);
                    }

                    if (longMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_long", longMaterialFile);
                    }

                    if (spineMaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_spine", spineMaterialFile);
                    }

                    if (spine2MaterialFile != "")
                    {
                        EngineLayer.ATLBase.LoadPluginMaterial("s_spine2", spine2MaterialFile);
                    }                    

                    previewCanvas1.BeginInit = true;

                    if (npcAnimationTable[currentNode] == null)
                    {
                        // 获取动作文件路径
                        string[] data = modelFile.Split(new char[] { '\\' });
                        string rootFolder = "";
                        for (int i = 0; i < data.Length - 2; i++)
                        {
                            rootFolder += string.Format("{0}\\", data[i]);
                        }
                        rootFolder = rootFolder + "动作";

                        DirectoryInfo di = new DirectoryInfo(rootFolder);
                        List<string> fileNameList = new List<string>();
                        if (di.Exists)
                        {
                            foreach (FileInfo fi in di.GetFiles())
                            {
                                if (fi.Extension == ".ani")
                                {
                                    fileNameList.Add(fi.FullName);
                                }
                            }
                        }

                        npcAnimationTable[currentNode] = fileNameList;
                        npcAnimationList = fileNameList;
                    }
                    else
                    {
                        npcAnimationList = npcAnimationTable[currentNode] as List<string>;
                    }

                    npcAnimationIndex = 0;
                    buttonX19.Enabled = true;
                    buttonX20.Enabled = true;

                    // 切换笔刷
                    DataRow row = npcInfoTable[currentNode] as DataRow;
                    int npcID = int.Parse(row["ID"].ToString());
                    string npcName = row["Name"].ToString();
                    EngineLayer.ATLBase.SwitchNpcBrush(npcID, npcName, 0, 0, 0);
                }

                labelX1.Text = "无动作";
            }
        }

        /// <summary>
        /// 搜索Npc
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX10_Click(object sender, EventArgs e)
        {
            string searchName = textBoxX1.Text;
            bool findLastNode = false;
            bool find = false;

            foreach (Node mapNode in npcTree.Nodes)
            {
                foreach (Node npcNode in mapNode.Nodes)
                {
                    string npcName = npcNode.Text;

                    if (lastSearchNpcNode == null)
                    {
                        findLastNode = true;
                    }

                    if (npcNode == lastSearchNpcNode)
                    {
                        findLastNode = true;
                        continue;
                    }

                    if (npcName.Contains(searchName) && findLastNode)
                    {
                        npcTree.SelectedNode = npcNode;
                        lastSearchNpcNode = npcNode;
                        find = true;

                        break;
                    }
                }

                if (find)
                {
                    break;
                }
            }

            if (!find)
            {
                lastSearchNpcNode = null;
            }
        }

        /// <summary>
        /// 刷新Npc
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX11_Click(object sender, EventArgs e)
        {
            // 构建分类树
            npcTree.Nodes.Clear();
            npcInfoTable.Clear();

            foreach (DataRow row in npcTable.Rows)
            {
                string npcID = row["ID"].ToString();
                string npcName = row["Name"].ToString();
                string mapName = row["MapName"].ToString();
                string representID = row["RepresentID1"].ToString();

                if (mapName == "")
                {
                    mapName = "未分类";
                }

                Node newNode = new Node();
                newNode.Text = string.Format("{0} ({1})", npcName, npcID);
                npcInfoTable[newNode] = row;

                if (representID != "")
                {
                    DataRow[] rows = npcRepresentTable.Select(string.Format("RepresentID = '{0}'", representID));
                    if (rows.Length > 0)
                    {
                        string modelFile = rows[0]["MainModelFile"].ToString();
                        string faceMeshFile = rows[0]["S_Face_MeshFile"].ToString();
                        string faceMaterialFile = rows[0]["S_Face_MaterialFile"].ToString();
                        string hatMeshFile = rows[0]["S_Hat_MeshFile"].ToString();
                        string hatMaterialFile = rows[0]["S_Hat_MaterialFile"].ToString();
                        string leftHandMeshFile = rows[0]["S_LH_MeshFile"].ToString();
                        string leftHandMaterialFile = rows[0]["S_LH_MaterialFile"].ToString();
                        string lpMeshFile = rows[0]["S_LP_MeshFile"].ToString();
                        string lpMaterialFile = rows[0]["S_LP_MaterialFile"].ToString();
                        string lcMeshFile = rows[0]["S_LC_MeshFile"].ToString();
                        string lcMaterialFile = rows[0]["S_LC_MaterialFile"].ToString();
                        string rightHandMeshFile = rows[0]["S_RH_MeshFile"].ToString();
                        string rightHandMaterialFile = rows[0]["S_RH_MaterialFile"].ToString();
                        string rpMeshFile = rows[0]["S_RP_MeshFile"].ToString();
                        string rpMaterialFile = rows[0]["S_RP_MaterialFile"].ToString();
                        string rcMeshFile = rows[0]["S_RC_MeshFile"].ToString();
                        string rcMaterialFile = rows[0]["S_RC_MaterialFile"].ToString();
                        string longMeshFile = rows[0]["S_Long_MeshFile"].ToString();
                        string longMaterialFile = rows[0]["S_Long_MaterialFile"].ToString();
                        string spineMeshFile = rows[0]["S_Spine_MeshFile"].ToString();
                        string spineMaterialFile = rows[0]["S_Spine_MaterialFile"].ToString();
                        string spine2MeshFile = rows[0]["S_Spine2_MeshFile"].ToString();
                        string spine2MaterialFile = rows[0]["S_Spine2_MaterialFile"].ToString();

                        if (modelFile != "")
                        {
                            modelFile = Path.Combine(rootDir, modelFile);
                        }

                        if (faceMeshFile != "")
                        {
                            faceMeshFile = Path.Combine(rootDir, faceMeshFile);
                        }

                        if (faceMaterialFile != "")
                        {
                            faceMaterialFile = Path.Combine(rootDir, faceMaterialFile);
                        }

                        if (hatMeshFile != "")
                        {
                            hatMeshFile = Path.Combine(rootDir, hatMeshFile);
                        }

                        if (hatMaterialFile != "")
                        {
                            hatMaterialFile = Path.Combine(rootDir, hatMaterialFile);
                        }

                        if (leftHandMeshFile != "")
                        {
                            leftHandMeshFile = Path.Combine(rootDir, leftHandMeshFile);
                        }

                        if (leftHandMaterialFile != "")
                        {
                            leftHandMaterialFile = Path.Combine(rootDir, leftHandMaterialFile);
                        }

                        if (lpMeshFile != "")
                        {
                            lpMeshFile = Path.Combine(rootDir, lpMeshFile);
                        }

                        if (lpMaterialFile != "")
                        {
                            lpMaterialFile = Path.Combine(rootDir, lpMaterialFile);
                        }

                        if (lcMeshFile != "")
                        {
                            lcMeshFile = Path.Combine(rootDir, lcMeshFile);
                        }

                        if (lcMaterialFile != "")
                        {
                            lcMaterialFile = Path.Combine(rootDir, lcMaterialFile);
                        }

                        if (rightHandMeshFile != "")
                        {
                            rightHandMeshFile = Path.Combine(rootDir, rightHandMeshFile);
                        }

                        if (rightHandMaterialFile != "")
                        {
                            rightHandMaterialFile = Path.Combine(rootDir, rightHandMaterialFile);
                        }

                        if (rpMeshFile != "")
                        {
                            rpMeshFile = Path.Combine(rootDir, rpMeshFile);
                        }

                        if (rpMaterialFile != "")
                        {
                            rpMaterialFile = Path.Combine(rootDir, rpMaterialFile);
                        }

                        if (rcMeshFile != "")
                        {
                            rcMeshFile = Path.Combine(rootDir, rcMeshFile);
                        }

                        if (rcMaterialFile != "")
                        {
                            rcMaterialFile = Path.Combine(rootDir, rcMaterialFile);
                        }

                        if (longMeshFile != "")
                        {
                            longMeshFile = Path.Combine(rootDir, longMeshFile);
                        }

                        if (longMaterialFile != "")
                        {
                            longMaterialFile = Path.Combine(rootDir, longMaterialFile);
                        }

                        if (spineMeshFile != "")
                        {
                            spineMeshFile = Path.Combine(rootDir, spineMeshFile);
                        }

                        if (spineMaterialFile != "")
                        {
                            spineMaterialFile = Path.Combine(rootDir, spineMaterialFile);
                        }

                        if (spine2MeshFile != "")
                        {
                            spine2MeshFile = Path.Combine(rootDir, spine2MeshFile);
                        }

                        if (spine2MaterialFile != "")
                        {
                            spine2MaterialFile = Path.Combine(rootDir, spine2MaterialFile);
                        }

                        Hashtable infoTable = new Hashtable();
                        infoTable["modelFile"] = modelFile;
                        infoTable["faceMeshFile"] = faceMeshFile;
                        infoTable["faceMaterialFile"] = faceMaterialFile;
                        infoTable["hatMeshFile"] = hatMeshFile;
                        infoTable["hatMaterialFile"] = hatMaterialFile;
                        infoTable["leftHandMeshFile"] = leftHandMeshFile;
                        infoTable["leftHandMaterialFile"] = leftHandMaterialFile;
                        infoTable["lpMeshFile"] = lpMeshFile;
                        infoTable["lpMaterialFile"] = lpMaterialFile;
                        infoTable["lcMeshFile"] = lcMeshFile;
                        infoTable["lcMaterialFile"] = lcMaterialFile;
                        infoTable["rightHandMeshFile"] = rightHandMeshFile;
                        infoTable["rightHandMaterialFile"] = rightHandMaterialFile;
                        infoTable["rpMeshFile"] = rpMeshFile;
                        infoTable["rpMaterialFile"] = rpMaterialFile;
                        infoTable["rcMeshFile"] = rcMeshFile;
                        infoTable["rcMaterialFile"] = rcMaterialFile;
                        infoTable["longMeshFile"] = longMeshFile;
                        infoTable["longMaterialFile"] = longMaterialFile;
                        infoTable["spineMeshFile"] = spineMeshFile;
                        infoTable["spineMaterialFile"] = spineMaterialFile;
                        infoTable["spine2MeshFile"] = spine2MeshFile;
                        infoTable["spine2MaterialFile"] = spine2MaterialFile;

                        newNode.Tag = infoTable;
                    }
                }

                Node mapNode = null;

                foreach (Node node in npcTree.Nodes)
                {
                    if (node.Text == mapName)
                    {
                        mapNode = node;
                        break;
                    }
                }

                if (mapNode == null)
                {
                    mapNode = new Node();
                    mapNode.Text = mapName;
                    npcTree.Nodes.Add(mapNode);
                }

                mapNode.Nodes.Add(newNode);
            }
        }

        /// <summary>
        /// 播放上一个动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX19_Click(object sender, EventArgs e)
        {
            npcAnimationIndex--;
            if (npcAnimationIndex < 0)
            {
                npcAnimationIndex = 0;
            }

            if (npcAnimationList.Count > 0)
            {
                string fileName = npcAnimationList[npcAnimationIndex];
                string[] data = fileName.Split(new char[] { '\\' });
                string aniName = data[data.Length - 1];

                labelX1.Text = aniName;
                fileName = fileName.Replace(rootDir, "");
                fileName = fileName.TrimStart(new char[] { '\\' });
                EngineLayer.ATLBase.ModelPlayAnimation(fileName);
            }
        }

        /// <summary>
        /// 播放下一个动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX20_Click(object sender, EventArgs e)
        {
            npcAnimationIndex++;
            if (npcAnimationIndex > npcAnimationList.Count - 1)
            {
                npcAnimationIndex = npcAnimationList.Count - 1;
            }

            if (npcAnimationList.Count > 0)
            {
                string fileName = npcAnimationList[npcAnimationIndex];
                string[] data = fileName.Split(new char[] { '\\' });
                string aniName = data[data.Length - 1];

                labelX1.Text = aniName;
                fileName = fileName.Replace(rootDir, "");
                fileName = fileName.TrimStart(new char[] { '\\' });
                EngineLayer.ATLBase.ModelPlayAnimation(fileName);
            }
        }

        /// <summary>
        /// 管理分组
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX21_Click(object sender, EventArgs e)
        {
            GroupManageForm groupManageForm = new GroupManageForm();
            groupManageForm.CurrentBaseDoc = m_doc;
            groupManageForm.Init();
            groupManageForm.ShowDialog();
        }

        /// <summary>
        /// 选中的Npc播放下一个动作 ahpho suntao
        /// </summary>
        private void NpcPlayNextAction()
        {
            SceneSceneEditor.NpcPlayNextAction();
        }

        /// <summary>
        /// 选中的Npc播放上一个动作 ahpho suntao
        /// </summary>
        private void NpcPlayPreviousAction()
        {
            SceneSceneEditor.NpcPlayPreviousAction();
        }

        /// <summary>
        /// 编辑分组 ahpho suntao
        /// </summary>
        private void EditGroup()
        {
            int entityCount = -1;
            SceneSceneEditor.GetSelectedEntityCount(ref entityCount);

            List<_AtlObjInfo> objectInfoList = new List<_AtlObjInfo>();
            string objectType = "";

            for (int i = 0; i < entityCount; i++)
            {
                int iLogicObjPtr = 0;
                string objTypeName = string.Empty;
                m_doc.DocLogical.GetSelectedLogicObjPtr(i, ref iLogicObjPtr, ref objTypeName);
                _AtlObjInfo atlinfo = new _AtlObjInfo();
                atlinfo.bChanged = 1;
                atlinfo.iLogicObjPtr = iLogicObjPtr;
                //_AtlObjInfo中要储存iLogicObjPtr entityPtr，方便C#对话框保存时 直接exchange去docLogical，
                //再用iLogicObjPtr之逻辑更新entityPtr指向的表现。
                m_doc.DocLogical.GetSetObjInfo(objTypeName, ref atlinfo, iLogicObjPtr, 1);

                if (objectType == "")
                {
                    objectType = atlinfo.strType;
                    objectInfoList.Add(atlinfo);
                }
                else
                {
                    if (atlinfo.strType == objectType)
                    {
                        objectInfoList.Add(atlinfo);
                    }
                }
            }

            if (entityCount > 0)
            {
                GroupForm gForm = new GroupForm();
                gForm.CurrentBaseDoc = m_doc;
                gForm.ObjectType = objectType;
                gForm.ObjectInfoList = objectInfoList;
                gForm.Init();
                gForm.ShowDialog();
            }
            else
            {
                MessageBox.Show("请先选择要设置分组的对象！", "设置分组", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 替换模板
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bReplaceNpcTemplate_Click(object sender, EventArgs e)
        {
            Node currentNode = npcTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                DataRow dataRow = npcInfoTable[currentNode] as DataRow;

                if (dataRow != null)
                {
                    int newTemplateID = int.Parse(dataRow["ID"].ToString());                

                    // 获取焦点，切换场景对象
                    PANEL.Focus();
                    
                    m_doc.DocLogical.ReplaceNpcTemplate(newTemplateID, 0);

                    
                }                
            }
            else
            {
                MessageBox.Show("请先选择要替换的模板类型", "替换模板", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 全场景替换模板
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bReplaceAllNpcTemplate_Click(object sender, EventArgs e)
        {
            Node currentNode = npcTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 1)
            {
                DataRow dataRow = npcInfoTable[currentNode] as DataRow;

                if (dataRow != null)
                {
                    int newTemplateID = int.Parse(dataRow["ID"].ToString());

                    // 获取焦点，切换场景对象
                    PANEL.Focus();

                    m_doc.DocLogical.ReplaceNpcTemplate(newTemplateID, 1);


                }
            }
            else
            {
                MessageBox.Show("请先选择要替换的模板类型", "替换模板", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        } 
	}
}
