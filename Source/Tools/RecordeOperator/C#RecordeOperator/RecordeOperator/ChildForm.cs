using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AtlKGRecordeOperatorLib;

namespace RecordeOperator
{
    public partial class ChildForm : Form
    {
        private RecordPlayer m_RecordPlayer = new RecordPlayer();
        string[] m_strType = new string[] { "Model", "Camera", "ModelST", "SceneEditor", "SFX" };

        SliderCtrl m_Slider = new SliderCtrl();
        List<int> m_ListInfo = new List<int>();

        CustomClass m_Preperty = new CustomClass();
        int m_nNowModelID;

        public ChildForm()
        {
            InitializeComponent();
            Init();
        }

        public ChildForm(RecordeOperator.MainForm parent)
        {
            InitializeComponent();
            Init();
            this.MdiParent = parent;
            
        }

        private void Init()
        {
            m_nNowModelID = -1;
            m_Slider.Init();
            m_Slider.SetChildForm(this);
            panel3.Controls.Add(m_Slider);
            m_Slider.Dock = DockStyle.Fill;
            propertyGrid1.SelectedObject = m_Preperty;
        }

        public void OpenFile(string strOpenFileName)
        {
            this.Text = System.IO.Path.GetFileName(strOpenFileName);

            m_RecordPlayer.LoadRecordFile(strOpenFileName);
            int nhWnd = this.pictureBox1.Handle.ToInt32();
            m_RecordPlayer.SetReplayWindow((uint)nhWnd);

            InitTreeView();
            SetSliderRangle();
        }

        public int RePlay()
        {
            int nRetCode = 0;
            m_RecordPlayer.RePlay(out nRetCode);
            return nRetCode;
        }

        public int PauseRePlay()
        {
            int nRetCode = 0;
            m_RecordPlayer.PauseRePlay(out nRetCode);
            return nRetCode;
        }

        public int IsFinished()
        {
            int nRetCode = 0;
            m_RecordPlayer.IsPlayFinished(out nRetCode);
            return nRetCode;
        }

        public int IsFreeCamera()
        {
            int nResult = 0;
            m_RecordPlayer.IsFreeCamera(out nResult);
            return nResult;
        }

        public int GetObjSize()
        {
            int nSize = 0;
            m_RecordPlayer.GetObjSize(out nSize);
            return nSize;
        }

        public int GetPlayState()
        {
            int playState = -1;
            m_RecordPlayer.GetPlayState(out playState);
            return playState;
        }

        private void Play_Click(object sender, EventArgs e)
        {
            if (RePlay() != 0)
            {
                Play.Text = "Pause";
                timer1.Enabled = true;
            }
            else if (PauseRePlay() != 0)
            {
                Play.Text = "Play";
            }
        }

        private void Stop_Click(object sender, EventArgs e)
        {
            m_RecordPlayer.StopRePlay();
            Play.Text = "Play";
        }

        private void FreeCamera_Click(object sender, EventArgs e)
        {
            m_RecordPlayer.FreeCamera();
            if (IsFreeCamera() == 1)
            {
                FreeCamera.Text = "RestoreCamera";
            }
            else
            {
                FreeCamera.Text = "FreeCamera";
            }
        }

        private void OnDraw()
        {
            if (IsFinished() != 0)
            {
                Play.Text = "Play";
                timer1.Enabled = false;
                return;
            }

            int playState = GetPlayState();
            if (playState == 0)
            {
                m_RecordPlayer.FrameMoveRePlay();
                int nTime = 0;
                m_RecordPlayer.GetNowTime(out nTime);
                if (m_Slider.GetPos() != nTime)
                {
                    m_Slider.SetPos(nTime);
                    m_Slider.Invalidate();
                    m_Slider.Update();
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            OnDraw();
        }

        // TreeView
        private void InitTreeView()
        {
            m_RecordPlayer.InitTree();

            TreeNode hParent = new TreeNode("All Objects");
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add(hParent);
            treeView1.SelectedNode = treeView1.Nodes[0];

            treeView1.SelectedNode.Nodes.Clear();
            foreach (string strNode in m_strType)
            {
                treeView1.SelectedNode.Nodes.Add(strNode);
            }
            FillTree();
        }

        private void FillTree()
        {
            if (m_RecordPlayer == null)
                return;

            int nID = 0, nType = 0;
            string strName = null;
            TreeNode treeNode = null;

            treeView1.SelectedNode = treeView1.Nodes[0];
            m_RecordPlayer.GetFirstObj(out nID, out strName, out nType);
            while (nID != -1)
            {
                if (nType != 0)
                {
                    treeNode = treeView1.SelectedNode.Nodes[nType - 1];
                    TreeNode InsertNode = new TreeNode(strName);
                    InsertNode.Tag = nID;
                    treeNode.Nodes.Add(InsertNode);
                    FillEvent(InsertNode);
                }
                m_RecordPlayer.GetNextObj(out nID, out strName, out nType);
            }
            return;
        }

        private void FillEvent(TreeNode InsertNode)
        {
            if (m_RecordPlayer == null)
                return;

            if (InsertNode == null)
                return;

            string strFuncName = null;
            int nInfo = 0;
            m_RecordPlayer.GetFirstEvent(out strFuncName, out nInfo);
            while (strFuncName != null)
            {
                TreeNode node = new TreeNode(strFuncName);
                node.Tag = nInfo;
                InsertNode.Nodes.Add(node);
                m_RecordPlayer.GetNextEvent(out strFuncName, out nInfo);
            }
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (treeView1.SelectedNode == null)
                return;

            if (treeView1.SelectedNode.Level == 2)
            {
                listBox1.Items.Clear();
                m_Preperty.Clear();
                propertyGrid1.Refresh();
                int nID = int.Parse(treeView1.SelectedNode.Tag.ToString());
                m_nNowModelID = nID;
                int nSuccess = 0;
                m_RecordPlayer.GetObjInfo(nID, out nSuccess);
                if (nSuccess != 0)
                {
                    m_Slider.ListClear();
                    int nTime = 0;
                    int nInfo = 0;
                    m_RecordPlayer.GetFirstEventTime(out nTime, out nInfo);
                    while (nTime > -2)
                    {
                        m_Slider.SetModelTime(nTime, nInfo);
                        m_RecordPlayer.GetNextEventTime(out nTime, out nInfo);
                    }
                    m_Slider.Invalidate();
                    m_Slider.Update();
                }
            }
        }

        // Event Done
        private void addEventToolStripMenuItem_Click(object sender, EventArgs e)
        {
            EditForm frmEdit = new EditForm();
            frmEdit.Init(treeView1.SelectedNode.Parent.Parent.Text);
            if (frmEdit.ShowDialog() == DialogResult.OK)
            {
                int nNum = frmEdit.GetNum();
                if (nNum == -1)
                    return;
                string strName = frmEdit.GetName();
                string strValue = frmEdit.GetValue();
                m_RecordPlayer.AddEvent(nNum, strName, strValue);
            }
        }

        private void deleteEventToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TreeNode node = treeView1.SelectedNode;
            int nInfo = int.Parse(node.Tag.ToString());
            // m_RecordPlayer.DeleteEvent(nInfo);
        }

        // Slider
        public void SetSliderRangle()
        {
            int nTime = 0;
            m_RecordPlayer.GetTotalTime(out nTime);
            m_Slider.SetRange(0, nTime);
        }

        public Size GetPanel3Size()
        {
            return this.panel3.Size;
        }

        public void GetInfo(List<int> listInfo)
        {
            m_ListInfo.Clear();
            listBox1.Items.Clear();
            m_Preperty.Clear();
            propertyGrid1.Refresh();
            for (int i = 0; i < listInfo.Count; i++)
            {
                int nInfo = listInfo[i];
                string strFunction = null;
                m_RecordPlayer.GetEventFunction(nInfo, out strFunction);
                listBox1.Items.Add(strFunction);
                m_ListInfo.Add(listInfo[i]);
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_Preperty.Clear();
            int nIndex = listBox1.SelectedIndex;
            if (nIndex < 0)
                return;
            int nInfo = m_ListInfo[nIndex];
            string strLabel = null;
            string strTextBox = null;
            string strFuncName = listBox1.Items[nIndex].ToString();
            int nNum = 0;
            m_RecordPlayer.GetInformation(nInfo, out strLabel, out strTextBox, out nNum);

            int i = 0, nLabelIndex = 0, nTextBoxIndex = 0;
            int nLabelLength = strLabel.Length;
            int nTextBoxLength = strTextBox.Length;

            string subLabel = null, subTextBox = null;
            for (i = 0; i < nNum; i++)
            {
                subLabel = null;
                for (; nLabelIndex < nLabelLength; nLabelIndex++)
                {
                    if (strLabel[nLabelIndex] == ' ')
                        break;
                    subLabel += strLabel[nLabelIndex];
                }
                nLabelIndex++;

                subTextBox = null;
                for (; nTextBoxIndex < nTextBoxLength; nTextBoxIndex++)
                {
                    if (strTextBox[nTextBoxIndex] == ' ')
                        break;
                    subTextBox += strTextBox[nTextBoxIndex];
                }
                nTextBoxIndex++;
                CustomProperty myPreperty = new CustomProperty(strFuncName, subLabel, subTextBox, false, true);
                m_Preperty.Add(myPreperty);
            }
            propertyGrid1.Refresh();
        }
    }
}