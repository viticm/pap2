using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
    public class MyPanel : Panel
    {
        public BaseForm m_baseForm;

        public AtlInputProxy InputProxy;

        private tagMSG m_msg = new tagMSG();

        private const int WM_PAINT       = 0x000F;
        private const int WM_LBUTTONDOWN = 0x0201;
        private const int WM_LBUTTONUP   = 0x0202;
        private const int WM_MBUTTONDOWN = 0x0207;
        private const int WM_MBUTTONUP   = 0x0208;
        private const int WM_RBUTTONDOWN = 0x0204;
        private const int WM_RBUTTONUP   = 0x0205;
        private const int WM_MOUSEWHEEL  = 0x020A;
        private const int WM_MOUSEMOVE   = 0x0200;
        private const int WM_KEYDOWN     = 0x0100;
        private const int WM_KEYUP       = 0x0101;

        private const int WM_USER        = 0x0400;
        private const int WM_PLACED_OBJ  = WM_USER + 100;
        private const int WM_DELETE_OBJ  = WM_USER + 101;
        private const int WM_MODIFY_OBJ  = WM_USER + 102;

        private const int REPRESENTOBJECT_NPC       = 1;
        private const int REPRESENTOBJECT_DOODAD    = 8;
        private const int REPRESENTOBJECT_WAYPOINT  = 9;
        private const int REPRESENTOBJECT_POLY      = 11;


        // 供GetObjDisplayInfo用的临时变量。
        int logicObjPtr = 0;
        int representObjPtr = 0;
        int hasscript = 0;
        string name = string.Empty;
        string nickname = string.Empty;
        int templateID = -1;


        protected override void WndProc(ref Message msg)
        {
            if (InputProxy != null)
            {
                m_msg.hwnd = (IntPtr)msg.HWnd;
                m_msg.lParam = (int)msg.LParam;
                m_msg.message = (uint)(msg.Msg);
                m_msg.wParam = (uint)msg.WParam;

                int iRet = 0;
                InputProxy.CheckInput(ref m_msg, ref iRet);
            }

            //if (msg.Msg == WM_LBUTTONDOWN || msg.Msg == WM_LBUTTONUP   || msg.Msg == WM_MBUTTONDOWN || 
            //    msg.Msg == WM_MBUTTONUP   || msg.Msg == WM_RBUTTONDOWN || msg.Msg == WM_RBUTTONUP   || 
            //    msg.Msg == WM_MOUSEWHEEL  || msg.Msg == WM_MOUSEMOVE   || msg.Msg == WM_KEYDOWN     || 
            //    msg.Msg == WM_KEYUP)
            //{
            //    SceneEditor.EngineMgr.ProcessMessage((uint)msg.Msg, (int)msg.WParam, (int)msg.LParam);
            //}

            if (msg.Msg == WM_PLACED_OBJ)
            {
                ProcessPlaceObjMsg(ref msg);
            }
            else if (msg.Msg == WM_DELETE_OBJ)
            {
                ProcessDeleteObjMsg(ref msg);
            }
            else if (msg.Msg == WM_MODIFY_OBJ)
            {
                ProcessModifyObjMsg(ref msg);
            }

            base.WndProc(ref msg);
        }

        private void ProcessPlaceObjMsg(ref Message msg)
        {
            representObjPtr = (int)msg.LParam;

            if ((int)msg.WParam == REPRESENTOBJECT_NPC)
            {
                m_baseForm.DOC.DocLogical.GetObjDisplayInfo("NPC", -1, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj npc = new LogicObj("NPC", -1, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                //string npcName = npc.naMe;
                string npcName = string.Format("{0} [{1}]", npc.naMe, npc.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", npcName, npc.nickName.Length > 0 ? "[" + npc.nickName + "]" : string.Empty, npc.hasScript ? "[√]" : string.Empty);
                bool bExistThisTemplate = false;

                foreach (TreeNode node in LogicHelper.m_NodeNPCs.Nodes)
                {
                    if (node.Text.Contains(npcName))
                    {
                        if (node.Nodes.Count == 0)
                        {
                            TreeNode oldnewnpcnode = node.Nodes.Add(node.Name, node.Text);
                            oldnewnpcnode.Tag = node.Tag;

                            node.Text = node.Name;
                            node.Tag = null;
                        }

                        // 再添加这一个。
                        TreeNode newnpcnode = node.Nodes.Add(npcName, fullDisplayName);
                        newnpcnode.Tag = npc;

                        bExistThisTemplate = true;
                        break;
                    }
                }

                if (!bExistThisTemplate)
                {
                    TreeNode newnpcnode = LogicHelper.m_NodeNPCs.Nodes.Add(npcName, fullDisplayName);
                    newnpcnode.Tag = npc;
                }
            }
            else if ((int)msg.WParam == REPRESENTOBJECT_DOODAD)
            {
                m_baseForm.DOC.DocLogical.GetObjDisplayInfo("Doodad", -1, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                LogicObj doodad = new LogicObj("Doodad", -1, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                //string doodadName = doodad.naMe;
                string doodadName = string.Format("{0} [{1}]", doodad.naMe, doodad.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", doodadName, doodad.nickName.Length > 0 ? "[" + doodad.nickName + "]" : string.Empty, doodad.hasScript ? "[√]" : string.Empty);
                bool bExistThisTemplate = false;

                foreach (TreeNode node in LogicHelper.m_NodeDoodads.Nodes)
                {
                    if (node.Text.Contains(doodadName))
                    {
                        if (node.Nodes.Count == 0)
                        {
                            TreeNode oldnewdoodadnode = node.Nodes.Add(node.Name, node.Text);
                            oldnewdoodadnode.Tag = node.Tag;

                            node.Text = node.Name;
                            node.Tag = null;
                        }

                        // 再添加这一个
                        TreeNode newdoodadnode = node.Nodes.Add(doodadName, fullDisplayName);
                        newdoodadnode.Tag = doodad;

                        bExistThisTemplate = true;
                        break;
                    }
                }

                if (!bExistThisTemplate)
                {
                    TreeNode newdoodadnode = LogicHelper.m_NodeDoodads.Nodes.Add(doodadName, fullDisplayName);
                    newdoodadnode.Tag = doodad;
                }
            }
            else if ((int)msg.WParam == REPRESENTOBJECT_POLY)
            {
                m_baseForm.AddPolyTreeNode();
            }

            LogicHelper.UpdateNodesCountDesc();
        }

        private void ProcessDeleteObjMsg(ref Message msg)
        {
            int representObjPtr = (int)msg.LParam;

            object r = m_baseForm.FindTreeNodeByRepresentObjPtr((int)msg.WParam, representObjPtr);
            TreeNode node = r as TreeNode;
            DevComponents.AdvTree.Node advNode = r as DevComponents.AdvTree.Node;
            if (node != null)
            {
                node.Parent.Nodes.Remove(node);
                node.Remove();
            }
            else if (advNode != null)
            {
                advNode.Parent.Nodes.Remove(advNode);
                advNode.Remove();
            }

            LogicHelper.UpdateNodesCountDesc();
        }

        private void ProcessModifyObjMsg(ref Message msg)
        {
            int representObjPtr = (int)msg.LParam;
            object r = m_baseForm.FindTreeNodeByRepresentObjPtr((int)msg.WParam, representObjPtr);
            TreeNode node = r as TreeNode;
            DevComponents.AdvTree.Node advNode = r as DevComponents.AdvTree.Node;
            LogicObj obj;

            if ((int)msg.WParam == REPRESENTOBJECT_NPC)
            {
                m_baseForm.DOC.DocLogical.GetObjDisplayInfo("NPC", -1, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                obj = new LogicObj("NPC", -1, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);
                
                string objName = string.Format("{0} [{1}]", obj.naMe, obj.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", objName, obj.nickName.Length > 0 ? "[" + obj.nickName + "]" : string.Empty, obj.hasScript ? "[√]" : string.Empty);
                node.Name = objName;
                node.Text = fullDisplayName;
            }
            else if ((int)msg.WParam == REPRESENTOBJECT_DOODAD)
            {
                m_baseForm.DOC.DocLogical.GetObjDisplayInfo("Doodad", -1, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                obj = new LogicObj("Doodad", -1, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                string objName = string.Format("{0} [{1}]", obj.naMe, obj.templateID);
                string fullDisplayName = string.Format("{0} {1} {2}", objName, obj.nickName.Length > 0 ? "[" + obj.nickName + "]" : string.Empty, obj.hasScript ? "[√]" : string.Empty);
                node.Name = objName;
                node.Text = fullDisplayName;
            }
            else if ((int)msg.WParam == REPRESENTOBJECT_WAYPOINT)
            {

            }
            else if ((int)msg.WParam == REPRESENTOBJECT_POLY)
            {
                m_baseForm.DOC.DocLogical.GetObjDisplayInfo("LogicalPoly", -1, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                advNode.Name = name;
                advNode.Text = string.Format("{0}{1}", name, hasscript != 0 ? " [√]" : string.Empty);
            }
            else if ((int)msg.WParam == REPRESENTOBJECT_POLY + 1) // REPRESENTOBJECT_BRUSH
            {
                _AtlObjInfo Brushinfo = new _AtlObjInfo();
                m_baseForm.DOC.DocLogical.GetSetObjInfo("LogicalBrush", ref Brushinfo, representObjPtr, 1);
                Brushinfo.iLogicObjPtr = representObjPtr;
                advNode.Tag = Brushinfo;
            }
        }
    }
}
