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
        /// <summary>
        /// 初始化路径数据
        /// </summary>
        private void InitPatrol()
        {
            RefreshPatrol();
        }

        /// <summary>
        /// 初始化巡逻路径链表
        /// </summary>
        private void InitPatrolPathTable()
        {
            int patrolCount = 0;
            m_doc.DocLogical.GetObjCount("WayPointSet", 0, ref patrolCount);

            _AtlObjInfo patrolInfo = new _AtlObjInfo();
            patrolPathTable.Rows.Clear();

            for (int i = 0; i < patrolCount; i++)
            {
                int patroPathIndex = i + 1;
                m_doc.DocLogical.GetObjDisplayInfo("WayPointSet", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                m_doc.DocLogical.GetSetObjInfo("WayPointSet", ref patrolInfo, logicObj, 1);
                Hashtable patrolInfoTable = Helper.GetInfoTable(patrolInfo);

                DataRow dataRow = patrolPathTable.NewRow();
                dataRow["ID"] = patroPathIndex.ToString();
                dataRow["Name"] = patrolInfoTable["szName"];
                patrolPathTable.Rows.Add(dataRow);
            }
        }

        /// <summary>
        /// 初始化npc阵营数据表
        /// </summary>
        private void InitNpcOrderTable()
        {
            string filePath = string.Format("{0}\\settings\\NpcOrder\\OrderList.tab", Helper.RootDir);
            FileInfo fi = new FileInfo(filePath);

            if (fi.Exists)
            {
                string content = FileFolderHelper.FileToString(filePath);
                string[] lines = content.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
                bool firstLine = true;

                foreach (string s in lines)
                {
                    if (firstLine)
                    {
                        firstLine = false;
                    }
                    else
                    {
                        string line = s.TrimEnd(new char[] { '\r', ' ' });
                        string[] data = line.Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
                        string orderID = data[0];
                        string orderName = data[3];
                        string orderFile = data[1];
                        string orderFilePath = string.Format("{0}\\settings\\NpcOrder\\{1}", Helper.RootDir, orderFile);

                        int orderCount = 0;
                        FileInfo orderFileInfo = new FileInfo(orderFilePath);

                        if (orderFileInfo.Exists)
                        {
                            content = FileFolderHelper.FileToString(orderFilePath);
                            string[] orderLines = content.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
                            orderCount = orderLines.Length - 1;
                            if (orderCount < 0)
                            {
                                orderCount = 0;
                            }

                            DataRow dataRow = npcOrderTable.NewRow();
                            dataRow["ID"] = orderID;
                            dataRow["Name"] = orderName;
                            dataRow["Count"] = orderCount;
                            npcOrderTable.Rows.Add(dataRow);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 填充巡逻阵型链表
        /// </summary>
        private void FillPatrolPathOrderList()
        {
            if (patrolPathOrderBox.Items.Count == 0)
            {
                patrolPathOrderBox.Items.Clear();

                foreach (DataRow dataRow in BaseForm.npcOrderTable.Rows)
                {
                    patrolPathOrderBox.Items.Add(string.Format("{0} ({1})", dataRow["Name"].ToString(), dataRow["ID"].ToString()));
                }
            }            
        }

        /// <summary>
        /// 刷新路径数据
        /// </summary>
        private void RefreshPatrol()
        {
            patrolTree.Nodes.Clear();

            int patrolCount = 0;
            m_doc.DocLogical.GetObjCount("WayPointSet", 0, ref patrolCount);

            if (patrolCount > 0)
            {
                _AtlObjInfo patrolInfo = new _AtlObjInfo();

                for (int i = 0; i < patrolCount; i++)
                {
                    int logicObj = 0;
                    int representObj = 0;
                    int hasScript = 0;
                    string name = "";
                    string nickName = "";

                    m_doc.DocLogical.GetObjDisplayInfo("WayPointSet", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    m_doc.DocLogical.GetSetObjInfo("WayPointSet", ref patrolInfo, logicObj, 1);
                    Hashtable patrolInfoTable = Helper.GetInfoTable(patrolInfo);

                    // 添加路径树结点
                    Node patrolNode = new Node();
                    patrolNode.Text = string.Format("{0} ({1})", patrolInfoTable["szName"], i.ToString());
                    patrolNode.Tag = patrolInfo;
                    patrolTree.Nodes.Add(patrolNode);

                    int patrolIndex = int.Parse(patrolInfoTable["nIndex"] as string);
                    int patrolPointCount = 0;
                    m_doc.DocLogical.GetObjCount("WayPoint", i, ref patrolPointCount);

                    for (int j = 0; j < patrolPointCount; j++)
                    {
                        _AtlObjInfo patrolPointInfo = new _AtlObjInfo();
                        m_doc.DocLogical.GetObjDisplayInfo("WayPoint", j, i, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                        m_doc.DocLogical.GetSetObjInfo("WayPoint", ref patrolPointInfo, logicObj, 1);
                        Hashtable patrolPointInfoTable = Helper.GetInfoTable(patrolPointInfo);

                        // 刷新路径点类型
                        int wayPointType = 1;

                        if (j == 0)
                        {
                            wayPointType = 0;
                        }
                        else if (j == patrolPointCount - 1)
                        {
                            wayPointType = 2;
                        }

                        m_doc.DocLogical.RefreshWayPoint(representObj, wayPointType);

                        // 添加路径点树结点
                        Node patrolPointNode = new Node();
                        patrolPointNode.Text = j.ToString();
                        patrolPointNode.Tag = patrolPointInfo;
                        patrolNode.Nodes.Add(patrolPointNode);
                    }
                }

                m_doc.DocLogical.RefreshWay();
            }
        }

        /// <summary>
        /// 更新路径信息
        /// </summary>        
        /// <param name="fieldName">属性名</param>
        /// <param name="newValue">属性值</param>
        /// <return>是否更新成功</return>
        private bool UpdatePatrolInfo(string fieldName, string newValue)
        {
            bool updateSuccess = false;
            Node currentNode = patrolTree.SelectedNode;

            if (beginEdit && currentNode != null)
            {                
                int parentIndex = 0;
                int index = currentNode.Index;

                if (currentNode.Parent != null)
                {
                    parentIndex = currentNode.Parent.Index;
                }

                _AtlObjInfo objectInfo = (_AtlObjInfo)currentNode.Tag;

                UpdateObjectInfo(objectInfo, fieldName, newValue, index, parentIndex);
                updateSuccess = true;
            }

            return updateSuccess;
        }

        /// <summary>
        /// 路径点分类数结点选择结束
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void advTree1_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node currentNode = patrolTree.SelectedNode;
            beginEdit = false;

            if (currentNode != null)
            {
                _AtlObjInfo objectInfo = (_AtlObjInfo)currentNode.Tag;
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                Node cameraNode = null;

                if (currentNode.Level == 1) // 刷新路径点信息
                {
                    wayPointSetParameterPanel.Enabled = false;
                    wayPointParameterPanel.Enabled = true;

                    if (infoTable["nStayFrame"] != null)
                    {
                        wayPointStayFrameBox.Text = infoTable["nStayFrame"] as string;
                    }
                    else
                    {
                        wayPointStayFrameBox.Text = "";
                    }

                    if (infoTable["nStayDirection"] != null)
                    {
                        wayPointStayDirectionBox.Text = infoTable["nStayDirection"] as string;
                    }
                    else
                    {
                        wayPointStayDirectionBox.Text = "";
                    }

                    if (infoTable["nStayAnimation"] != null)
                    {
                        wayPointStayAnimationBox.Text = infoTable["nStayAnimation"] as string;
                    }
                    else
                    {
                        wayPointStayAnimationBox.Text = "";
                    }

                    if (infoTable["bStayAniLoop"] as string == "1")
                    {
                        cWayPointAnimationLoop.Checked = true;
                    }
                    else
                    {
                        cWayPointAnimationLoop.Checked = false;
                    }

                    if (infoTable["bIsRun"] as string == "1")
                    {
                        cWayPointRun.Checked = true;
                    }
                    else
                    {
                        cWayPointRun.Checked = false;
                    }

                    if (infoTable["szScriptName"] != null)
                    {
                        wayPointScriptBox.Text = infoTable["szScriptName"] as string;
                    }
                    else
                    {
                        wayPointScriptBox.Text = "";
                    }

                    cameraNode = currentNode;
                }
                else // 刷新路径信息
                {
                    wayPointSetParameterPanel.Enabled = true;
                    wayPointParameterPanel.Enabled = false;

                    FillPatrolPathOrderList();

                    if (infoTable["szName"] != null)
                    {
                        patrolPathNameBox.Text = infoTable["szName"] as string;
                    }
                    else
                    {
                        patrolPathNameBox.Text = "";
                    }

                    if (infoTable["nPatrolPathOrderID"] != null)
                    {
                        patrolPathOrderBox.SelectedIndex = int.Parse(infoTable["nPatrolPathOrderID"].ToString()) - 1;
                    }
                    else
                    {
                        patrolPathOrderBox.SelectedIndex = -1;
                    }

                    if (infoTable["nPatrolPathWalkSpeed"] != null)
                    {
                        patrolPathWalkSpeedBox.Text = infoTable["nPatrolPathWalkSpeed"] as string;
                    }
                    else
                    {
                        patrolPathWalkSpeedBox.Text = "";
                    }

                    if (currentNode.Nodes.Count > 0)
                    {
                        cameraNode = currentNode.Nodes[0];
                    }
                }

                // 移动摄像机位置
                if (cameraNode != null)
                {
                    m_doc.DocLogical.GetObjDisplayInfo("WayPoint", cameraNode.Index, cameraNode.Parent.Index, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    MoveCameraToRepresentObj(representObj);
                    SelectOnlyRepresentObj(representObj);
                }
            }

            beginEdit = true;
        }

        /// <summary>
        /// 添加路径
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX2_Click(object sender, EventArgs e)
        {
            InputForm iForm = new InputForm("新建路径", "请输入路径名称");

            if (iForm.ShowDialog() == DialogResult.OK)
            {
                int newWayPointSet = 0;
                m_doc.DocLogical.DecoratedAddOneWayPointSet(ref newWayPointSet, iForm.InputText);

                int index = patrolTree.Nodes.Count;
                _AtlObjInfo patrolInfo = new _AtlObjInfo();
                m_doc.DocLogical.GetObjDisplayInfo("WayPointSet", index, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                m_doc.DocLogical.GetSetObjInfo("WayPointSet", ref patrolInfo, logicObj, 1);

                Node patrolNode = new Node();
                patrolNode.Text = string.Format("{0} ({1})", iForm.InputText, index + 1);
                patrolNode.Tag = patrolInfo;
                patrolTree.Nodes.Add(patrolNode);
                patrolTree.SelectedNode = patrolNode;

                // 刷新路径数据
                InitPatrolPathTable();
            }
        }

        /// <summary>
        /// 删除路径
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX3_Click(object sender, EventArgs e)
        {
            Node currentNode = patrolTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 0)
            {
                DialogResult dialogResult = MessageBox.Show("确定要删除选中的路径吗？", "删除路径",
                                                            MessageBoxButtons.OKCancel, MessageBoxIcon.Information);
                if (dialogResult == DialogResult.OK)
                {
                    int index = currentNode.Index;
                    m_doc.DocLogical.GetObjDisplayInfo("WayPointSet", index, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                    m_doc.DocLogical.DecoratedDeleteWayPointSet(logicObj);
                    m_doc.DocLogical.RefreshWay();
                    patrolTree.Nodes.Remove(currentNode);

                    // 刷新路径数据
                }
            }
            else
            {
                MessageBox.Show("请先选中要删除的路径！", "删除路径",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 刷新路径
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX1_Click(object sender, EventArgs e)
        {
            RefreshPatrol();
        }

        /// <summary>
        /// 添加路径点
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX4_Click(object sender, EventArgs e)
        {
            Node currentNode = patrolTree.SelectedNode;

            if (currentNode != null)
            {
                if (currentNode.Level == 1)
                {
                    currentNode = currentNode.Parent;
                }

                _AtlObjInfo objectInfo = (_AtlObjInfo)currentNode.Tag;
                Hashtable infoTable = Helper.GetInfoTable(objectInfo);
                int parentIndex = currentNode.Index;
                int index = currentNode.Nodes.Count;
                int wayPointSetID = int.Parse(infoTable["dwSetID"] as string);

                m_doc.DocLogical.AddOneWayPoint(wayPointSetID, index.ToString());
                editMode = EditMode.AddWayPoint;
            }
            else
            {
                MessageBox.Show("请先选中要添加路径点的路径！", "添加路径点",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 浏览路径点的脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX17_Click(object sender, EventArgs e)
        {            
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = Helper.RootDir;
            openFileDialog.Filter = "脚本文件|*.lua";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string scriptFileName = openFileDialog.FileName;
                scriptFileName = scriptFileName.Replace(Helper.RootDir, "");
                scriptFileName = scriptFileName.TrimStart(new char[] { '\\' });
                wayPointScriptBox.Text = scriptFileName;
            }
        }

        /// <summary>
        /// 清除路径点的脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonX18_Click(object sender, EventArgs e)
        {            
            wayPointScriptBox.Text = "";                
        }

        /// <summary>
        /// 自动为关联的NPC列队
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bAutoAttackPathNpc_Click(object sender, EventArgs e)
        {
            Node currentNode = patrolTree.SelectedNode;

            if (currentNode != null && currentNode.Level == 0)
            {
                // 路径点的Index是其编号 + 1
                m_doc.DocLogical.ResortNpcOrderInPatrolPath(currentNode.Index + 1);

                MessageBox.Show("自动为关联的NPC列队完成！", "自动为关联的NPC列队", MessageBoxButtons.OK,
                                MessageBoxIcon.Information);
            }            
        }

        /// <summary>
        /// 修改路径名称
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void patrolPathNameBox_TextChanged(object sender, EventArgs e)
        {
            if (UpdatePatrolInfo("szName", patrolPathNameBox.Text))
            {
                // 对应的树结点刷新
                Node currentNode = patrolTree.SelectedNode;
                int patrolPathID = currentNode.Index;

                currentNode.Text = string.Format("{0} ({1})", patrolPathNameBox.Text, patrolPathID.ToString());                
            }
        }

        /// <summary>
        /// 修改路径行走速度
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void patrolPathWalkSpeedBox_TextChanged(object sender, EventArgs e)
        {
            UpdatePatrolInfo("nPatrolPathWalkSpeed", patrolPathWalkSpeedBox.Text);
        }

        /// <summary>
        /// 修改路径阵型
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void patrolPathOrderBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            int patrolOrderID = patrolPathOrderBox.SelectedIndex + 1;
            UpdatePatrolInfo("nPatrolPathOrderID", patrolOrderID.ToString());
        }

        /// <summary>
        /// 修改路径点停留帧数
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void wayPointStayFrameBox_TextChanged(object sender, EventArgs e)
        {
            UpdatePatrolInfo("nStayFrame", wayPointStayFrameBox.Text);
        }

        /// <summary>
        /// 修改路径点停留朝向
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void wayPointStayDirectionBox_TextChanged(object sender, EventArgs e)
        {
            UpdatePatrolInfo("nStayDirection", wayPointStayDirectionBox.Text);
        }

        /// <summary>
        /// 修改路径点停留动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void wayPointStayAnimationBox_TextChanged(object sender, EventArgs e)
        {
            UpdatePatrolInfo("nStayAnimation", wayPointStayAnimationBox.Text);
        }

        /// <summary>
        /// 修改路径点脚本
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void wayPointScriptBox_TextChanged(object sender, EventArgs e)
        {
            UpdatePatrolInfo("szScriptName", wayPointScriptBox.Text);
        }

        /// <summary>
        /// 修改是否循环播放停留动作
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cWayPointAnimationLoop_CheckedChanged(object sender, EventArgs e)
        {
            string boolString = "0";

            if (cWayPointAnimationLoop.Checked)
            {
                boolString = "1";
            }

            UpdatePatrolInfo("bStayAniLoop", boolString);
        }

        /// <summary>
        /// 修改是否跑步
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void cWayPointRun_CheckedChanged(object sender, EventArgs e)
        {
            string boolString = "0";

            if (cWayPointRun.Checked)
            {
                boolString = "1";
            }

            UpdatePatrolInfo("bIsRun", boolString);
        }
	}
}
