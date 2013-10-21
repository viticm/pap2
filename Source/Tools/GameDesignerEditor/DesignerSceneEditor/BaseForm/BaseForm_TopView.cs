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
        private void InitTopView_Tga(string strMapName)
        {
            bool bTPScale = false, bTPStartX = false, bTPStartY = false, bTPWidth = false, bTPHeight = false, bTPLogicXStart = false, bTPLogicZStart = false;
            StringBuilder sb = new StringBuilder(MAX_PATH);

            float fScale = 0.0f;
            int width = 0, height = 0;

            string pureMapName = FileFolderHelper.GetMapName(strMapName);
            if (pureMapName == "测试1")
                pureMapName = "测试";

            string iniFile = Path.Combine(EditorLayer.EngineDir, string.Format("data/source/maps/{0}minimap/config.ini", pureMapName));
            FileFolderHelper.ReadIniFile("middlemap0", "scale", "", sb, MAX_PATH, iniFile);
            bTPScale = float.TryParse(sb.ToString(), out fScale);
            FileFolderHelper.ReadIniFile("middlemap0", "startx", "", sb, MAX_PATH, iniFile);
            bTPStartX = float.TryParse(sb.ToString(), out m_fMiddleMapStartX);
            FileFolderHelper.ReadIniFile("middlemap0", "starty", "", sb, MAX_PATH, iniFile);
            bTPStartY = float.TryParse(sb.ToString(), out m_fMiddleMapStartY);
            FileFolderHelper.ReadIniFile("middlemap0", "width", "", sb, MAX_PATH, iniFile);
            bTPWidth = int.TryParse(sb.ToString(), out width);
            FileFolderHelper.ReadIniFile("middlemap0", "height", "", sb, MAX_PATH, iniFile);
            bTPHeight = int.TryParse(sb.ToString(), out height);
            if (!bTPScale || !bTPStartX || !bTPStartY || !bTPWidth || !bTPHeight)
                MessageBox.Show(string.Format("数据错误！请检查{0}的数据。", iniFile));

            string mapLogicalIni = Path.Combine(EditorLayer.EngineDir, string.Format("data/source/maps/{0}/{1}.Map.Logical", pureMapName, FileFolderHelper.GetMapName(strMapName)));
            FileFolderHelper.ReadIniFile("MAIN", "LogicalSceneXStart", "", sb, MAX_PATH, mapLogicalIni);
            bTPLogicXStart = float.TryParse(sb.ToString(), out m_fLogicStartX);
            FileFolderHelper.ReadIniFile("MAIN", "LogicalSceneZStart", "", sb, MAX_PATH, mapLogicalIni);
            bTPLogicZStart = float.TryParse(sb.ToString(), out m_fLogicStartZ);
            if (!bTPLogicXStart || !bTPLogicZStart)
                MessageBox.Show(string.Format("数据错误！请检查{0}的数据。", mapLogicalIni));

            m_fLogicTotalWidth = width / fScale;
            m_fLogicTotalHeight = height / fScale;

            previewform = new Preview(curMapWidth, curMapHeight, m_scenesceneeditor, m_fLogicTotalWidth, m_fLogicTotalHeight, m_fMiddleMapStartX, m_fMiddleMapStartY, m_fLogicStartX, m_fLogicStartZ);

        }

        private void InitTopView_Old()
        {
            m_topView = new TopViewForm(this);
        }
    }
}
