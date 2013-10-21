using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using LuaInterface;
using System.Collections;
using System.Text.RegularExpressions;
using DevComponents.DotNetBar;
using atlaxLib;


namespace GameDesingerTools
{
    public partial class BaseForm
    {
        int m_oldX = 0;
        int m_oldY = 0;
        enum MdlType { MT_PLAYER, MT_NPC, MT_DODEAD, MT_OTHER, MT_COUNT };
        enum UpdateType { ADD, MOD, DEL };

        static string[] strMdlDefaultPath = new string[] {
            "data\\source\\player\\",
            "data\\source\\npc\\",
            "data\\source\\doodad\\",
            ""};
        static string[] strMdlDefaultAnimationPathSuffix = new string[] {
            "动作", "", "", ""};
        static string[] strMdlDefaultFacePathSuffix = new string[] {
            "部件", "", "", ""};

        // OpenModelInDialogStatic相关
        static Hashtable staticDialogs = new Hashtable();

        public TreeView BaseTree { get { return baseTree; } set { baseTree = value; } }
        public DevComponents.DotNetBar.TabControl tabControl;

        private bool m_DisplayNode = false; // 是否用脚本控制叶结点的显示 
        private bool m_DisplayVarNode = false; // 是否每个结点所用的控制显示的字符串会有变化
        private DataRow m_defaultRow; // 当前默认行
        private string m_displayString; // 脚本控制显示的字符串
        private string m_displayField; // 脚本控制显示的字段名
        private Hashtable m_nodeTable = new Hashtable(); // 保存结点原名称的hash表
        private Hashtable m_nodeDisplayValueTable = new Hashtable(); // 保存结点显示名称的hash表

        ArrayList m_Pages;

        //脚本用
        public ArrayList Pages { get { return m_Pages; } }

        //C#用
        public ArrayList PagesCS { get { return m_Pages; } }

        SqlConnection m_conn = MainForm.conn;
        public SqlConnection Conn { get { return m_conn; } }

        LuaEx m_lua = new LuaEx();
        public Lua lua { get { return m_lua; } }

        bool m_canSaveDB = false;
        public bool CanSaveDB { get { return m_canSaveDB; } set { m_canSaveDB = value; } }

        bool m_canCfg = false;
        public bool CanCfg { get { return m_canCfg; } set { m_canCfg = value; } }

        int m_modl_id;
        public int ModID { get { return m_modl_id; } set { m_modl_id = value; } }

        bool m_bLoadTreeWithLua;
        public bool LoadTreeWithLua { get { return m_bLoadTreeWithLua; } set { m_bLoadTreeWithLua = value; } }

        string m_strModelName;
        public string ModelName { get { return m_strModelName; } set { m_strModelName = value; } }

        string m_strMainTableName;
        public string MainTableName { get { return m_strMainTableName; } set { m_strMainTableName = value; } }

        string[] m_strCatFields;
        public string[] CatFields { get { return m_strCatFields; } set { m_strCatFields = value; } }

        Hashtable[] m_htCatHashes;
        public Hashtable[] CatHashes { get { return m_htCatHashes; } set { m_htCatHashes = value; } }

        // 主表
        DataTable m_tblMain;
        public DataTable TblMain { get { return m_tblMain; } set { m_tblMain = value; } }
        bool m_btblMainHasDefaultRow;
        public bool HasDefaultRow { get { return m_btblMainHasDefaultRow; } set { m_btblMainHasDefaultRow = value; } }

        object[] m_iStartIDs;
        public object[] StartIDs { get { return m_iStartIDs; } set { m_iStartIDs = value; } }

        // 结点名字以主表的哪个字段显示
        string m_strDisplayField;
        public string DisplayField { get { return m_strDisplayField; } set { m_strDisplayField = value; } }

        bool m_bCatFieldsContainsDisplayField;
        public bool CatfieldsContainsDisplayfield { get { return m_bCatFieldsContainsDisplayField; } set { m_bCatFieldsContainsDisplayField = value; } }

        public bool KeysContainsDisplayfileld = false;

        public bool CatFieldsContainsKey = false;
        public int ToChangeIndex = -1;

        // 查找
        public FindForm1 m_FindFrm = null;

        // 关于复制,粘贴
        TreeNode m_tnCopyFromNode = null;
        public TreeNode CopyFromNode { get { return m_tnCopyFromNode; } set { m_tnCopyFromNode = value; } }


        ArrayList m_lsTblUpdateDefault = new ArrayList();
        public ArrayList TableUpdateDefault { get { return m_lsTblUpdateDefault; } set { m_lsTblUpdateDefault = value; } }

        const int WM_REFRESHPG = 0x400 + 2000; // WM_USER = ?
        const int WM_REFREPROP = WM_REFRESHPG + 1;
        //const int WM_REFREPRTREE = WM_REFRESHPG + 2;

        // 关于预览
        private int init_preview_index = -1; // 1-based, 和tab顺序一样。
        private MyPanelEx[] preview_panels;
        private static Test3DEngine m_3DEngie;
        private static bool m_bInit3DEngineFaield = false;
        public static Test3DEngine K3DEngine { get { return m_3DEngie; } }
        private static bool m_bWarning3dEngine = false;

        private object m_ChangedProperty_tree;
        private List<CustomProperty> m_lstChangedProperty = new List<CustomProperty>();
        private bool m_bRecordChanged;
        private TreeNode m_poupupNode;
        private TimeLog log = new TimeLog();
        private TimeLog logscript = new TimeLog();
        private TimeLog loglocalscript = new TimeLog();
        private RecordLockEx m_recLock;
        private bool m_bPluginTree;

        private ButtonItem[] m_ExtraMenuItem = null;
        public BaseDialog m_Dialog = null;



        class ArrayListEx : ArrayList
        {
            private BaseForm m_baseForm;
            public ArrayListEx(BaseForm baseform)
            {
                m_baseForm = baseform;
            }
            public override object this[int idx]
            {
                get
                {
                    //不行，因会被 foreach　调用到这里，只载入当前页的策略会失效
                    /*PageDesc pgdes = base[idx] as PageDesc;
                    if (pgdes.pg != null && pgdes.pg.SelectedObject == null && pgdes.pg.Parent.Tag != null)
                    {
                        m_baseForm.LoadPageDBClass(pgdes.pg);
                    }*/
                    return base[idx];
                }
            }
        };
    };

    public class TblUpdateDefault
    {
        public string tblName;
        public DataRow oldValue;
        public DataRow newValue;
    };
    public class PageDesc
    {
        public string name_cn;
        public int id;
        public PropertyGrid pg;
        public DBCustomClass dbClass;
        public DBCustomClass Properties;
        public bool bCustomTab;
        //add by cuiwei 07.9.20
        public queryCommand qc;
        //add end
    };
}
