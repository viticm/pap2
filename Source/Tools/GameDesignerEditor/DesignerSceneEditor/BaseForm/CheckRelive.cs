using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	public partial class CheckRelive: Form
	{
		public CheckRelive()
		{
			InitializeComponent();
            m_bCheck = true;
		}

        public bool Init(string filename, IAtlIEKSceneEditorDocLogical DocLogical, SceneEditor sceneeditor)
        {
            m_strFileName = filename;
            m_DocLogical = DocLogical;
            m_SceneEditor = sceneeditor;

            if (m_DocLogical == null)
                return false;
            
            int nTreeChildNodesCount = ReloadTree();

            string strFilePath = m_strFileName;
            strFilePath = strFilePath + ".Logical";
            StringBuilder sb = new StringBuilder(1024);
            Helper.ReadIniFile("MAIN", "CheckReliveEnable", "true", sb, 1024, strFilePath);
            if (sb.ToString() == "true")
            {
                m_bCheck = true;
            }
            else
            {
                m_bCheck = false;
            }
            //if (strcmp(szFileName, "测试") == 0)
            //{
            //    strcat(szFileName, "1");
            //}
            //else if (strcmp(szFileName, "光") == 0)
            //{
            //    strcpy(szFileName, "测试1");
            //}
            //else if (strcmp(szFileName, "登陆界面场景") == 0)
            //{
            //    strcpy(szFileName, "登陆界面");
            //}
            CK_Run.Checked = m_bCheck;

            if (m_bCheck && (nTreeChildNodesCount > 0))
            {
                this.Show();
            }

            return true;
        }

        public int Recheck()
        {
            int nTreeChildNodesCount = ReloadTree();
            if (m_bCheck && (nTreeChildNodesCount > 0))
            {
                this.Show();
                return 0;
            }
            return 1;
        }

        public void SaveSetting()
        {
            string strFilePath = m_strFileName;
            strFilePath = strFilePath + ".Logical";
            StringBuilder sb = new StringBuilder(1024);
            string strValue;
            if (m_bCheck)
            {
                strValue = "true";
            }
            else 
            {
                strValue = "false";
            }

            Helper.WriteIniFile("MAIN", "CheckReliveEnable", strValue, strFilePath);
        }

        private void CK_Run_CheckedChanged(object sender, EventArgs e)
        {
            m_bCheck = CK_Run.Checked;
        }

        private void BN_Recheck_Click(object sender, EventArgs e)
        {
            ReloadTree();
        }

        private void BN_Ignore_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        protected int ReloadTree()
        {
            treeView1.Nodes.Clear();
            TreeNode DoodadNode = treeView1.Nodes.Add("doodad对象树");
            int nCount = 0;
            int nDoodadCount = 0;

            m_DocLogical.GetObjCount("Doodad", -1, ref nDoodadCount);

            string name = string.Empty;
            string nickname = string.Empty;
            int dwReliveID = 0;
            int representObjPtr = 0;
            int logicObjPtr = 0;
            int templateID = 0;

            string strFileName = m_strFileName;
            int nIndex = strFileName.IndexOf("\\data\\source\\maps\\");
            if (nIndex == -1)
                return 0;
            strFileName = strFileName.Substring(0, nIndex) + "\\settings\\DoodadCheckList.tab";

            string content = Helper.GetFileContent(strFileName);
            DataTable tbl = Helper.CreateDataTable(content);
            //DataColumn column = tbl.Columns["ID"];

            for (int i = 0; i < nDoodadCount; i++)
            {
                m_DocLogical.GetObjDisplayInfo("DoodadRelive", i, -1, ref name, ref nickname, ref dwReliveID, ref representObjPtr, ref logicObjPtr, ref templateID);
                //LogicObj doodad = new LogicObj("Doodad", i, -1, name, nickname, hasscript == 0 ? false : true, representObjPtr, logicObjPtr, templateID);

                foreach (DataRow row in tbl.Rows)
                {
                    if (Int32.Parse(row.ItemArray[0].ToString()) == templateID)
                    {
                        if (dwReliveID == 0)
                        {
                            TreeNode newNode = DoodadNode.Nodes.Add(name);
                            newNode.Tag = i;
                            nCount++;
                        }
                    }
                }
            }

            return nCount;
        }

        bool m_bCheck;
        IAtlIEKSceneEditorDocLogical m_DocLogical;
        string m_strFileName;
        SceneEditor m_SceneEditor;

        private void ClosingE(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void NodeMouseDbClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            m_SceneEditor.ClearSelectedEntityList();
            
            int i = -1;
            if (e.Node.Tag != null)
            { 
                i = Int32.Parse(e.Node.Tag.ToString());
            }

            int nCount = 0;
            m_DocLogical.GetObjCount("Doodad", -1, ref nCount);

            string name = string.Empty;
            string nickname = string.Empty;
            int hasscript = 0;
            int representObjPtr = 0;
            int logicObjPtr = 0;
            int templateID = 0;

            if (i > 0 && i < nCount)
            {
                m_DocLogical.GetObjDisplayInfo("Doodad", i, -1, ref name, ref nickname, ref hasscript, ref representObjPtr, ref logicObjPtr, ref templateID);
                //LogicObj Obj = new LogicObj(name, i, -1, name, nickname, hasscrtpt != 0 ? 1 : 0, representObjPtr, logicObjPtr, templateID);
                m_SceneEditor.AddSelectedRepresentObject(representObjPtr, 1);

                _AtlVector3 objPos = new _AtlVector3();
                EngineLayer.ATLBase.GetRepresentObjTranslation(representObjPtr, ref objPos);

                MoveCameraToPosition(objPos);

                //_AtlVector3 oldpos = new _AtlVector3(), oldlookat = new _AtlVector3();
                //SceneSceneEditor.GetCameraPosLookat(ref oldpos, ref oldlookat);
                //_AtlVector3 delta = new _AtlVector3();
                //delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
                //_AtlVector3 newpos = new _AtlVector3();
                //newpos.x = objPos.x - delta.x; newpos.y = objPos.y - delta.y; newpos.z = objPos.z - delta.z;
                //SceneSceneEditor.SetCameraPosLookat(ref newpos, ref objPos);

                m_SceneEditor.UpdateSelectableEntityList(5);

                //float x = 0;
                //float y = 0;
                //float z = 0;
                
                //m_DocLogical.GetDoodadPos(logicObjPtr, ref x, ref y, ref z);
            }
              
            //e.Node.Tag;
        }
        private void MoveCameraToPosition(_AtlVector3 newPos)
        {
            _AtlVector3 oldpos = new _AtlVector3(), oldlookat = new _AtlVector3();
            m_SceneEditor.GetCameraPosLookat(ref oldpos, ref oldlookat);
            _AtlVector3 delta = new _AtlVector3();
            delta.x = oldlookat.x - oldpos.x; delta.y = oldlookat.y - oldpos.y; delta.z = oldlookat.z - oldpos.z;
            _AtlVector3 newpos = new _AtlVector3();
            newpos.x = newPos.x - delta.x; newpos.y = newPos.y - delta.y; newpos.z = newPos.z - delta.z;
            m_SceneEditor.SetCameraPosLookat(ref newpos, ref newPos);
        }
	}
}