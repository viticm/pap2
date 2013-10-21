using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Data.SqlClient;
using System.IO;
using AtlKG3DEngineProxyLib;

namespace DesignerSceneEditor
{
	class EngineLayer
	{
        static public AtlBase ATLBase = new AtlBase();
        static public BaseLib Baselib = new BaseLib();
        //static public AtlEngineManager EngineMgr = null;
        //static public IntPtr EngineMgrPtr = IntPtr.Zero;

        static public int KGLOG_OPTION_FILE = 0x1;
        static public uint KG3DENG_DESIGN = 0x0010;


        static public void Init()
        {
            InitBaseLib();
            InitKG3DEngine();
            InitGlobalVars();
        }

        static private void InitBaseLib()
        {
            ATLBase.SetRootPath(EditorLayer.EngineDir);
            Baselib.InitLog("DesignerSceneEditor", 65536, KGLOG_OPTION_FILE, Application.StartupPath);
            Baselib.InitMemory("DesignerSceneEditor.memory");
        }

        unsafe static private void InitKG3DEngine()
        {
            ATLBase.Init3DEngineManager(Application.StartupPath, EditorLayer.MainFrmHwnd);
            //ATLBase.SetRootPath(EditorLayer.EngineDir);
        }

        static private void InitGlobalVars()
        {
            ATLBase.InitGlobalVars();
        }

        static public void Uninit()
        {
            ATLBase.UnInitEngine();
            Baselib.UnInitMemory();
        }
	}
}
