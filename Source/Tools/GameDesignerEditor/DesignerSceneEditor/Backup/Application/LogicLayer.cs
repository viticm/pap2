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
	class LogicsLayer
	{
        static public SO3World SO3world = new SO3World();
        static public int CLIENT_ID_PREFIX = 0xf;

        static public void Init()
        {
            if (EditorLayer.UseSO3Logic)
            {
                SO3world.Init(CLIENT_ID_PREFIX);
            }
        }

        static public void Uninit()
        {
            if (EditorLayer.UseSO3Logic)
            {
                SO3world.UnInit();
            }
        }
	}
}
