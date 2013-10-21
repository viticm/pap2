/*===================================
 * Purpose:
 * Created By: WilsonWei
 * Created On: 2007-2-7
 * (c)Kingsoft Corporation
 * Comments:
 * 
 * 
 * 
 * ====================================*/

using System;
using System.Collections.Generic;
using System.Text;
using XSJ.Sword3.Editor.Studio.Data;
using Microsoft.Win32;

namespace XSJ.Sword3.Editor.Studio.App
{
    class AppVariables
    {
        static private string m_outputdir =string.Empty;
        static public IDataProviderBase g_dataprovider = new SqlDataProvider();
        static public bool g_logon = false;
        static public string g_connstring =
            "data source=changan;initial catalog=s3design;persist security info=False;user id=%s1;password=%s2;packet size=4096";


        static public string OutputDir
        {
            get {return m_outputdir;}
            set {m_outputdir = value;}
        }
        public const int INVALID_TREENODE = -10000;
    }
}
