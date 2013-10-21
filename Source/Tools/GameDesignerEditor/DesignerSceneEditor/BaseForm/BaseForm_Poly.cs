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
    unsafe public partial class BaseForm : Form, IKDocument
	{
        private int LSTATE_BLOCKCHARACTER   = 1;
        private int LSTATE_PUTOBJECT        = 2;
        private int LSTATE_STALL            = 4;
        private int LSTATE_INDOOR           = 8;
        private int LSTATE_REST             = 16;
        private int LSTATE_RIDEHORSE        = 32;
        //private int LSTATE_SCRIPTFILE       = 64;

        private int SCENESTATE_SELECT       = 0;
        private int SCENESTATE_PLACEOBJ     = 200;
        private int SCENESTATE_CELLLOGICAL  = 400;

        private Node nodePolies = null;
        private Node nodeBrushes = null;

        private bool InitPolyComponents()
        {
            this.checkBoxBlock.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.checkBoxPut.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.checkBoxStall.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.checkBoxIndoor.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.checkBoxRest.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.checkBoxRideHorse.CheckedChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.textBoxScript.TextChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.textBoxHeight.TextChanged += new System.EventHandler(this.PolyPropUIChanged);
            this.textLogicName.TextChanged += new System.EventHandler(this.PolyPropUIChanged);
            return true;
        }

        private bool InitPoly()
        {
            this.advTreePoly.Nodes.Clear();
            nodePolies = new Node();
            nodePolies.Text = "多边形";
            this.advTreePoly.Nodes.Add(nodePolies);
            nodeBrushes = new Node();
            nodeBrushes.Text = "笔刷";
            this.advTreePoly.Nodes.Add(nodeBrushes);

            // 加载多边形生成结点
            int polyCount = 0;
            m_doc.DocLogical.GetObjCount("LogicalPoly", 0, ref polyCount);
            for (int i = 0; i < polyCount; i++)
            {
                _AtlObjInfo polyinfo = new _AtlObjInfo();
                m_doc.DocLogical.GetObjDisplayInfo("LogicalPoly", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                m_doc.DocLogical.GetSetObjInfo("LogicalPoly", ref polyinfo, logicObj, 1);
                polyinfo.iLogicObjPtr = logicObj;

                Node polyNode = new Node();
                polyNode.Text = string.Format("{0}{1}", name, hasScript != 0 ? " [√]" : string.Empty);
                polyNode.Tag = polyinfo;
                nodePolies.Nodes.Add(polyNode);
            }

            // 加载逻辑数据生成结点
            int brushCount = 0;
            m_doc.DocLogical.GetObjCount("LogicalBrush", 0, ref brushCount);
            for (int i = 0; i < brushCount; i++)
            {
                _AtlObjInfo Brushinfo = new _AtlObjInfo();
                m_doc.DocLogical.GetObjDisplayInfo("LogicalBrush", i, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
                m_doc.DocLogical.GetSetObjInfo("LogicalBrush", ref Brushinfo, logicObj, 1);
                Brushinfo.iLogicObjPtr = logicObj;

                Node BrushNode = new Node();
                BrushNode.Text = string.Format("{0}{1}", name, hasScript != 0 ? " [√]" : string.Empty);
                BrushNode.Tag = Brushinfo;
                nodeBrushes.Nodes.Add(BrushNode);
            }


            return true;
        }

        private void advTreePoly_AfterNodeSelect(object sender, AdvTreeNodeEventArgs e)
        {
            Node selectedNode = this.advTreePoly.SelectedNode;
            if (selectedNode == null) return;

            if (selectedNode.Level == 0)
            {
                if (selectedNode.Text == "笔刷")
                    SceneSceneEditor.SetEditState(SCENESTATE_CELLLOGICAL);
                else if (selectedNode.Text == "多边形")
                    SceneSceneEditor.SetEditState(SCENESTATE_SELECT);
            }
            else if (selectedNode.Level == 1)
            {
                if (selectedNode.Parent.Text == "多边形")
                {
                    //SceneSceneEditor.SetEditState(SCENESTATE_SELECT);

                    _AtlObjInfo atlobj = (_AtlObjInfo)selectedNode.Tag;
                    ShowPolyInfoUI(atlobj);
                    this.buttonOK.Enabled = false;

                    if (atlobj.iRepresentObjPtr != 0)
                    {
                        if (SceneSceneEditor.GetEditState() != SCENESTATE_PLACEOBJ)
                            MoveCameraToRepresentObj(atlobj.iRepresentObjPtr);
                        SelectOnlyRepresentObj(atlobj.iRepresentObjPtr);
                    }
                }
                else if (selectedNode.Parent.Text == "笔刷")
                {
                    _AtlObjInfo atlobj = (_AtlObjInfo)selectedNode.Tag;
                    ShowBrushInfoUI(atlobj);
                    this.buttonOK.Enabled = false;
                    //set camera
                    string strPos = atlobj.strValues[5];
                    if (strPos != "0.000000,0.000000,0.000000")
                    {
                        string[] strPoses = strPos.Split(new char[] {','},StringSplitOptions.RemoveEmptyEntries);
                        float x = 0.0f, y = 0.0f, z = 0.0f;
                        if (Single.TryParse(strPoses[0], out x) && Single.TryParse(strPoses[1], out y) && Single.TryParse(strPoses[2], out z))
                        {
                            _AtlVector3 brushPos = new _AtlVector3();
                            brushPos.x = x; brushPos.y = y; brushPos.z = z;
                            MoveCameraToPosition(brushPos);
                        }
                    }

                    SetLogicSceneEditorBrushState(atlobj);
                }
            }

        }

        private void SetLogicSceneEditorBrushState(_AtlObjInfo atlobj)
        {
            SceneSceneEditor.SetEditState(SCENESTATE_CELLLOGICAL);
            SceneSceneEditor.ClearLogicModifyState();
            SceneSceneEditor.SetLogicModifyState(Convert.ToInt32(atlobj.strValues[4]), atlobj.strValues[3]);
            SceneSceneEditor.SetLogicCurrentColor(stringToColor(atlobj.strValues[1]));
            SceneSceneEditor.SetCurrentEditBrushIndex(Convert.ToInt32(atlobj.strValues[0]));
        }

        private void ShowPolyInfoUI(_AtlObjInfo info)
        {
            // 状态掩码
            int modifyState = Convert.ToInt32(info.strValues[3]);
            this.checkBoxBlock.Checked = (modifyState & LSTATE_BLOCKCHARACTER) != 0;
            this.checkBoxPut.Checked = (modifyState & LSTATE_PUTOBJECT) != 0;
            this.checkBoxStall.Checked = (modifyState & LSTATE_STALL) != 0;
            this.checkBoxIndoor.Checked = (modifyState & LSTATE_INDOOR) != 0;
            this.checkBoxRest.Checked = (modifyState & LSTATE_REST) != 0;
            this.checkBoxRideHorse.Checked = (modifyState & LSTATE_RIDEHORSE) != 0;
            // 脚本文件
            this.textBoxScript.Text = info.strValues[0];
            // 高度
            this.textBoxHeight.Text = info.strValues[5];
            // 名字
            this.textLogicName.Text = info.strValues[1];
            // 颜色
            int color = stringToColor(info.strValues[4]);
            this.colorPicker.Style.BackColor1.Color = Color.FromArgb(color);
            this.colorPicker.Style.BackColor2.Color = Color.FromArgb(color);
        }
        private int stringToColor(string strcolor)
        {
            int color = 0;
            Int32.TryParse(strcolor, out color);
            return color;
        }
        private void ShowBrushInfoUI(_AtlObjInfo info)
        {
            // 状态掩码
            int modifyState = Convert.ToInt32(info.strValues[4]);
            this.checkBoxBlock.Checked = (modifyState & LSTATE_BLOCKCHARACTER) != 0;
            this.checkBoxPut.Checked = (modifyState & LSTATE_PUTOBJECT) != 0;
            this.checkBoxStall.Checked = (modifyState & LSTATE_STALL) != 0;
            this.checkBoxIndoor.Checked = (modifyState & LSTATE_INDOOR) != 0;
            this.checkBoxRest.Checked = (modifyState & LSTATE_REST) != 0;
            this.checkBoxRideHorse.Checked = (modifyState & LSTATE_RIDEHORSE) != 0;
            // 脚本文件
            this.textBoxScript.Text = info.strValues[3];
            // 高度
            this.textBoxHeight.Text = string.Empty;
            // 名字
            this.textLogicName.Text = info.strValues[2];
            // 颜色
            int color = stringToColor(info.strValues[1]);
            this.colorPicker.Style.BackColor1.Color = Color.FromArgb(color);
            this.colorPicker.Style.BackColor2.Color = Color.FromArgb(color);
        }

        private void colorPicker_Click(object sender, EventArgs e)
        {
            DialogResult dr = colorDialog1.ShowDialog();
            if (dr == DialogResult.OK)
            {
                int color = colorDialog1.Color.ToArgb();
                this.colorPicker.Style.BackColor1.Color = colorDialog1.Color;
                this.colorPicker.Style.BackColor2.Color = colorDialog1.Color;
                PolyPropUIChanged(sender, e);
            }
        }

        private void SaveLogicInfoUI(_AtlObjInfo info)
        {
            m_doc.DocLogical.GetSetObjInfo(info.strType, ref info, info.iLogicObjPtr, 0);
        }

        private void PolyPropUIChanged(object sender, EventArgs e)
        {
            buttonOK.Enabled = true;
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            if (this.advTreePoly.SelectedNode == null || this.advTreePoly.SelectedNode.Level != 1)
            {
                MessageBox.Show("未选中任何多边形/笔刷结点");
                return;
            }

            _AtlObjInfo selectedinfo = (_AtlObjInfo)this.advTreePoly.SelectedNode.Tag;
            if (this.advTreePoly.SelectedNode.Parent.Text == "多边形")
            {
                selectedinfo.strValues[0] = this.textBoxScript.Text;
                selectedinfo.strValues[5] = this.textBoxHeight.Text;
                int iNewMask = 0
                    + (this.checkBoxBlock.Checked ? LSTATE_BLOCKCHARACTER : 0)
                    + (this.checkBoxPut.Checked ? LSTATE_PUTOBJECT : 0)
                    + (this.checkBoxStall.Checked ? LSTATE_STALL : 0)
                    + (this.checkBoxIndoor.Checked ? LSTATE_INDOOR : 0)
                    + (this.checkBoxRest.Checked ? LSTATE_REST : 0)
                    + (this.checkBoxRideHorse.Checked ? LSTATE_RIDEHORSE : 0);
                selectedinfo.strValues[3] = iNewMask.ToString();
                selectedinfo.strValues[1] = this.textLogicName.Text;
                selectedinfo.strValues[4] = this.colorPicker.Style.BackColor1.Color.ToArgb().ToString();

                SaveLogicInfoUI((_AtlObjInfo)this.advTreePoly.SelectedNode.Tag);
            }
            else if (this.advTreePoly.SelectedNode.Parent.Text == "笔刷")
            {
                int oldMask = Convert.ToInt32(selectedinfo.strValues[4]);
                string oldScript = selectedinfo.strValues[3];

                selectedinfo.strValues[3] = this.textBoxScript.Text;
                //selectedinfo.strValues[5] = this.textBoxHeight.Text;
                int iNewMask = 0
                    + (this.checkBoxBlock.Checked ? LSTATE_BLOCKCHARACTER : 0)
                    + (this.checkBoxPut.Checked ? LSTATE_PUTOBJECT : 0)
                    + (this.checkBoxStall.Checked ? LSTATE_STALL : 0)
                    + (this.checkBoxIndoor.Checked ? LSTATE_INDOOR : 0)
                    + (this.checkBoxRest.Checked ? LSTATE_REST : 0)
                    + (this.checkBoxRideHorse.Checked ? LSTATE_RIDEHORSE : 0);
                selectedinfo.strValues[4] = iNewMask.ToString();
                selectedinfo.strValues[2] = this.textLogicName.Text;
                selectedinfo.strValues[1] = this.colorPicker.Style.BackColor1.Color.ToArgb().ToString();

                SaveLogicInfoUI((_AtlObjInfo)this.advTreePoly.SelectedNode.Tag);
                SceneSceneEditor.SetLogicCurrentColor(stringToColor(selectedinfo.strValues[1]));

                // 中途更新笔刷数据，要把原来刷的也改了，不然会有问题。
                SceneSceneEditor.ModifyCurrentCellInfo(SceneSceneEditor.TransToEngineFormat(oldMask), SceneSceneEditor.TransToEngineFormat(iNewMask), oldScript, selectedinfo.strValues[3]);
                
                SetLogicSceneEditorBrushState(selectedinfo);
            }

            this.advTreePoly.SelectedNode.Text = string.Format("{0}{1}", this.textLogicName.Text, this.textBoxScript.Text.Length > 0 ? " [√]" : string.Empty);
            this.buttonOK.Enabled = false;
        }


        private void buttonX13_Click(object sender, EventArgs e) // 新建
        {
            int nid = 0;
            m_doc.DocLogical.AddOneNewLogicalBrush(ref nid);

            int brushCount = 0;
            m_doc.DocLogical.GetObjCount("LogicalBrush", 0, ref brushCount);

            _AtlObjInfo Brushinfo = new _AtlObjInfo();
            m_doc.DocLogical.GetObjDisplayInfo("LogicalBrush", brushCount - 1, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
            m_doc.DocLogical.GetSetObjInfo("LogicalBrush", ref Brushinfo, logicObj, 1);
            Brushinfo.iLogicObjPtr = logicObj;

            Node BrushNode = new Node();
            BrushNode.Text = string.Format("{0}{1}", name, hasScript != 0 ? " [√]" : string.Empty);
            BrushNode.Tag = Brushinfo;
            nodeBrushes.Nodes.Add(BrushNode);

            this.advTreePoly.SelectedNode = BrushNode;
        }

        private void buttonDel_Click(object sender, EventArgs e) // 删除
        {
            if (this.advTreePoly.SelectedNode == null)
            {
                MessageBox.Show("未选中任何结点");
                return;
            }

            _AtlObjInfo selectedinfo = (_AtlObjInfo)this.advTreePoly.SelectedNode.Tag;
            if (this.advTreePoly.SelectedNode.Parent.Text == "多边形")
            {
                //m_doc.DocLogical.DeleteSelectedPoly();
                m_doc.DocLogical.DeleteSelectedObject();
                //this.advTreePoly.SelectedNode.Remove();
            }
            else if (this.advTreePoly.SelectedNode.Parent.Text == "笔刷")
            {
                int iMask = Convert.ToInt32(selectedinfo.strValues[4]);
                if (iMask != 0)
                {
                    SceneSceneEditor.DeleteCurrentCellInfo();

                    int iID = Convert.ToInt32(selectedinfo.strValues[0]);
                    m_doc.DocLogical.DeleteOneLogicalBrush(iID);

                    this.advTreePoly.Nodes.Remove(this.advTreePoly.SelectedNode);
                    this.advTreePoly.SelectedNode.Remove();
                }
            }

        }

        private void buttonX15_Click(object sender, EventArgs e) // 新建多边形
        {
            SceneSceneEditor.BeginAddPolyVertex();
        }

        private void buttonBuildPoly_Click(object sender, EventArgs e) // build 多边形
        {
            BuildPolyForm bpf = new BuildPolyForm();
            DialogResult dr = bpf.ShowDialog();
            if (dr == DialogResult.OK)
            {
                SceneSceneEditor.BuildPolyAndSetInHand(bpf.m_nVergeNum, bpf.m_fRadius, bpf.m_fHeight, bpf.m_fDegree);
            }
        }

        public void AddPolyTreeNode()
        {
            //InitPoly();
            int polyCount = 0;
            m_doc.DocLogical.GetObjCount("LogicalPoly", 0, ref polyCount);

            _AtlObjInfo polyinfo = new _AtlObjInfo();
            m_doc.DocLogical.GetObjDisplayInfo("LogicalPoly", polyCount - 1, 0, ref name, ref nickName, ref hasScript, ref representObj, ref logicObj, ref templateID);
            m_doc.DocLogical.GetSetObjInfo("LogicalPoly", ref polyinfo, logicObj, 1);
            polyinfo.strValues[3] = "0";
            polyinfo.strValues[1] = name = "未命名";
            m_doc.DocLogical.GetSetObjInfo("LogicalPoly", ref polyinfo, logicObj, 0);
            polyinfo.iLogicObjPtr = logicObj;
            polyinfo.iRepresentObjPtr = representObj;

            Node polyNode = new Node();
            polyNode.Text = string.Format("{0} ({1})", name, nickName);
            polyNode.Tag = polyinfo;
            nodePolies.Nodes.Add(polyNode);

            this.advTreePoly.SelectedNode = polyNode;
        }

        private void buttonX14_Click(object sender, EventArgs e) // 添加顶点结束
        {
            SceneSceneEditor.EndAddPolyVertex(0);
            //AddPolyTreeNode();
        }

        private void checkBoxX1_CheckedChanged(object sender, EventArgs e) // 显示多边形
        {
            SceneSceneEditor.EnableRenderPoly((sender as DevComponents.DotNetBar.Controls.CheckBoxX).Checked ? 1 : 0);
        }

        private void LogicDisplayParamsChanged(object sender, EventArgs e) // 逻辑显示参数
        {
            if ((sender as DevComponents.DotNetBar.Controls.CheckBoxX) == this.checkShowLogicCell)
            {
                this.checkShowTerrainCell.Enabled = this.checkShowItemCell.Enabled = checkShowSelectedCell.Enabled = this.checkShowLogicCell.Checked;
            }

            int showcell = this.checkShowLogicCell.Checked ? 1 : 0;
            int showterrain = this.checkShowTerrainCell.Checked ? 1 : 0;
            int showitem = this.checkShowItemCell.Checked ? 1 : 0;
            int showselected = this.checkShowSelectedCell.Checked ? 1 : 0;
            SceneSceneEditor.GetSetLogicDisplayParams(0, ref showcell, ref showterrain, ref showitem, ref showselected);
        }

        private void LogicBrushParamsChanged(object sender, EventArgs e) // 笔刷参数改变
        {
            this.sliderBrushRadius.Text = this.sliderBrushRadius.Value.ToString();
            this.sliderDisplayWidth.Text = this.sliderDisplayWidth.Value.ToString();
            this.sliderBrushHeight.Text = this.sliderBrushHeight.Value.ToString();

            int brushradius = this.sliderBrushRadius.Value;
            int brushtype = this.radioRect.Checked ? 1 : 0;
            int displaywidth = this.sliderDisplayWidth.Value;
            int brushheight = this.sliderBrushHeight.Value;
            SceneSceneEditor.SetLogicBrushParams(brushradius, brushtype, displaywidth, brushheight);
        }

        private void ToSelectPoly(int logicObj)
        {
            foreach (Node node in this.advTreePoly.Nodes)
            {
                _AtlObjInfo polyinfo = (_AtlObjInfo)node.Tag;
                if (polyinfo.iLogicObjPtr == logicObj)
                {
                    this.advTreePoly.SelectedNode = node;
                }
            }

            advTreePoly_AfterNodeSelect(null, null);
            this.SideCtrlBar.SelectedDockTab = 5;
        }

	}
}
