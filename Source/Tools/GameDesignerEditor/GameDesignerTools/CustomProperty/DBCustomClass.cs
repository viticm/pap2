using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Collections;
using System.Data.SqlClient;
using LuaInterface;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    public class DBCustomClass : CustomClass, ICustomTypeDescriptor
    {
        System.Data.DataTable m_tbl_MainData, m_tbl_FieldName_CN;
        SqlConnection m_conn;
        public SqlConnection Conn { get { return m_conn; } }
        SqlDataAdapter m_adp;
        //CustomClass m_PropertiesContainer = new CustomClass();
        Hashtable m_colName = new Hashtable(); //列中文名
        Hashtable m_combox_Keys = new Hashtable(); //所有的下拉列表的集合,显示值作为key
        Hashtable m_combox_Values = new Hashtable();//显示时根据id值显示名字,id值作为key
        Hashtable m_combox_ExValues = new Hashtable();//用于获取附加的字段内容，方便脚本调用
        Hashtable m_luacodes = new Hashtable();//记录当前表所有的脚本代码
        Hashtable m_scriptExecState = new Hashtable();
        string m_strMainTableName;
        string m_TabMainTableName; // 当前Tab分页的主表名
        string[] m_strCats;
        //DataRow m_currRow;
        CustomProperty m_property = null;
        int m_nSortType = 0;

        //
        string m_strSql, m_strMetaSql, m_strScriptSQL;
        public object[] m_findkeys;
        object[] m_findkeyvalues; 

        string[] m_strDBPrimaryKeys;
        string[] m_strPrimaryKeys;
        int m_nModlId, m_nModlTabId, m_nParentFieldId;
        object m_parent;
        bool m_bPrimaryKeysSameAsDB = false;//m_strPrimaryKeys是否与数据表中的主键定义相同
        bool m_bDBVirtualField = false;//是否设置为虚拟字段,只在表元中设置为虚拟字段的才会为真
        LuaEx m_lua;
        int m_nParentFieldType = 0;
        Hashtable m_events = null;
        CustomProperty m_ReplaceSubKeyProperty = null;
        int m_nRecordCound = 0;
        int m_nRecordModifyCount = 0; //被切换记录后，任何一次修改会递增一个数字，用于锁定行使用
        RecordLockEx m_reclock;
        int m_nLastServerVersion = -1;
        bool m_bReloadDataOnSave = false; //需要在每次保存时从数据库重新读取数据，以免发生共享冲突

        public delegate void AsynNotifyHandler(int nNotifyID, object param);
        public event AsynNotifyHandler OnAsynNotify;

        private List<string> m_listSortedCol = null;

        private bool m_valueChanged = false; // 值是否已经改变 add by suntao
        private bool m_initFinished = false; // 是否已经完成初始化 add by suntao
        
        public string StrSql = string.Empty;

        private List<string[]> mRecordList = new List<string[]>();
        private List<DBCustomProperty> mRecordProList = new List<DBCustomProperty>();

        //仅用于非虚拟字段下有子字段时调用，在本类的内部使用
        public DBCustomClass(object parent, LuaEx lua, int nTabId, int nParentFieldId, int nParentFieldType)
        {
            m_nParentFieldType = nParentFieldType;
            InitMetaInfo(parent, lua, nTabId, nParentFieldId);
        }
        public DBCustomClass(object parent, LuaEx lua, int nTabId, int nParentFieldId)
        {
            InitMetaInfo(parent, lua, nTabId, nParentFieldId);
        }

        /// <summary>
        /// 是否已经完成初始化
        /// </summary>
        public bool InitFinished
        {
            set
            {
                m_initFinished = value;
            }

            get
            {
                return m_initFinished;
            }
        }

        /// <summary>
        /// 值是否已经改变 add by suntao
        /// </summary>
        public bool ValueChanged
        {
            set
            {
                m_valueChanged = value;
            }

            get
            {
                return m_valueChanged;
            }
        }

        /// <summary>
        /// 标记值改变 add by suntao
        /// </summary>
        /// <param name="prop">CustomProperty</param>
        private void MarkValueChanged(CustomProperty prop)
        {
            DBCustomClass dbCustomClass = prop.Parent as DBCustomClass;

            if (dbCustomClass != null)
            {
                dbCustomClass.MarkValueChanged();
            }
        }

        /// <summary>
        /// 标记值改变 add by suntao
        /// </summary>
        public void MarkValueChanged()
        {
            m_valueChanged = true;
            
            DBCustomClass parentDBCustomClass = m_parent as DBCustomClass;            

            if (parentDBCustomClass != null)
            {
                parentDBCustomClass.MarkValueChanged();                
            }
        }

        public void OnAsynNotifyEvent(int nNotifyID, object param)
        {
            DBCustomClass cls = this;
            while (cls.Parent != null)
            {
                if (cls.Parent is DBCustomClass)
                {
                    cls = (DBCustomClass)(cls.Parent);
                }
            }
            if (cls.OnAsynNotify != null)
            {
                cls.OnAsynNotify(nNotifyID, param);
            }            
        }

        private List<string> GetSplitField(string splitString, bool virtualField)
        {
            List<string> list = new List<string>();

            string[] tempArray = splitString.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
            
            foreach (string s in tempArray)
            {
                int index = s.IndexOf(",");

                if (virtualField) // 虚拟字段读右边的值
                {
                    list.Add(s.Substring(index + 1, s.Length - index - 1));
                }
                else // 非虚拟字段读左边的值
                {
                    list.Add(s.Substring(0, index).ToLower());
                }
                
            }

            return list;
        }

        private void InitMetaInfo(object parent, LuaEx lua, int nTabId, int nParentFieldId)
        {
            m_findkeyvalues = null;            
            m_lua = lua;

            // 根据ParentFieldid不同，获取结果完全不同 = -1表示第一层
            string strParentField = nParentFieldId >= 0 ? " AND parentfield=" + nParentFieldId : "AND (parentfield IS NULL OR parentfield = '0')";
            
            // 根据TabId 获取主模块id
            DataTable tbl = CustomClassCache.GetModTabDefProxy("sys_modl_tab_def", string.Format("SELECT modid FROM sys_modl_tab_def WHERE modtabid = {0}", nTabId));
            int nModalId = int.Parse(tbl.Rows[0][0].ToString());
            
            // 获取本Tab页主表名
            tbl = CustomClassCache.GetModTabDefProxy("sys_modl_tab_def", string.Format("SELECT tablename, catfields FROM sys_modl_tab_def WHERE modid = {0} AND modtabid = {1}", nModalId, nTabId));
            m_TabMainTableName = tbl.Rows[0]["tablename"].ToString().Trim(); // no lower here.            
            
            if (nParentFieldId < 0)
            {
                m_strCats = GetFieldStr(tbl.Rows[0], "catfields").Split(new char[] { ',' });
            }
            
            // 获取本层主表名
            string currentTableName = null;
            tbl = CustomClassCache.GetModTabDefProxy("sys_meta_info", string.Format("SELECT tablename, fieldname FROM sys_meta_info WHERE fieldid = {0}", nParentFieldId));                            
            
            if (tbl.Rows.Count > 0)
            {
                currentTableName = tbl.Rows[0]["tablename"].ToString().Trim(); // no lower here.
            }            

            if (nParentFieldId >= 0)
            {
                m_strMainTableName = currentTableName;
            }
            else
            {
                m_strMainTableName = m_TabMainTableName;
            }

            // 生成 metainfo SQL语句, 只选择当前可见的字段
            m_strMetaSql = string.Format("SELECT * FROM sys_meta_info WHERE modtabid = {0} {1} ORDER BY {2}", nTabId, strParentField, "orderno");
            DataTable metatbl = CustomClassCache.GetDbMetaInfoProxy("sys_meta_info", m_strMetaSql);

            // visible设置不可用，隐藏之后会找不到该字段
            StringBuilder visibleFields = new StringBuilder();

            if (m_strMainTableName != m_TabMainTableName) // 和模块主表相同则直接从Cache里取记录即可
            {
                List<string> visibleFieldList = new List<string>();
                List<string> primaryKeyList = CustomClassCache.GetPrimaryKeys(m_strMainTableName);

                foreach (DataRow row in metatbl.Rows)
                {
                    string fieldType = row["fieldtype"].ToString();

                    if (fieldType != "1")
                    {
                        visibleFieldList.Add(GetFieldStr(row, "fieldname"));
                    }
                    else
                    {                        
                        // 虚拟字段需要读取关联条件相关字段的信息
                        List<string> splitFiledList = GetSplitField(row["subtablefilter"].ToString(), false);

                        foreach (string s in splitFiledList)
                        {
                            if (!visibleFieldList.Contains(s))
                            {
                                visibleFieldList.Add(s);
                            }
                        }                        
                    }
                }
               
                // 需要读父结点的关联信息
                if (parent != null && (parent as DBCustomClass).m_nParentFieldType == 1)
                {
                    tbl = CustomClassCache.GetDbMetaInfoProxy("sys_meta_info", string.Format("SELECT subtablefilter FROM sys_meta_info WHERE fieldid = '{0}'", nParentFieldId.ToString()));

                    List<string> splitFiledList = GetSplitField(tbl.Rows[0]["subtablefilter"].ToString(), true);

                    foreach (string s in splitFiledList)
                    {
                        if (!visibleFieldList.Contains(s))
                        {
                            visibleFieldList.Add(s);
                        }
                    }
                }                

                // 加入主键
                foreach (string s in primaryKeyList)
                {
                    string keyField = s.ToLower();

                    if (!visibleFieldList.Contains(keyField))
                    {
                        visibleFieldList.Add(keyField);
                    }
                }
 
                foreach (string s in visibleFieldList)
                {
                    visibleFields.Append(string.Format("[{0}],", s));
                }

                if (visibleFields.Length > 0)
                {
                    visibleFields.Remove(visibleFields.Length - 1, 1);
                }
            }
                        
            string strVisibleFeilds = visibleFields.ToString();          
            //m_strSql = string.Format("SELECT {1} FROM {0}", m_strMainTableName, strVisibleFeilds.Length > 0 ? strVisibleFeilds : "*");
            m_strSql = string.Format("SELECT {1} FROM {0}", m_strMainTableName, "*");
            m_nModlId = nModalId;
            m_nModlTabId = nTabId;
            m_nParentFieldId = nParentFieldId;
            m_parent = parent;
            
            // 查找主键
            m_strPrimaryKeys = GetPrimaryKeys();
            InitMetaInfo(m_strSql);

            // 初始化行锁
            m_reclock = new RecordLockEx(Conn, m_strMainTableName, m_strDBPrimaryKeys, m_nModlId);            
        }
     
        public bool ReloadDataOnSave
        {
            get { return m_bReloadDataOnSave; }
            set { m_bReloadDataOnSave = value; }
        }
        public object[] FindKeys
        {
            get { return m_findkeyvalues; }
        }
        public int SortType
        {
            get { return m_nSortType; }
            set { m_nSortType = value; }
        }

        public SqlDataAdapter DataAdapter
        {
            get { return m_adp; }
        }
        
        public CustomProperty Property
        {
            get { return m_property; }
            set { m_property = value; }
        }
        public int RecordCount
        {
            get { return m_nRecordCound; }
        }
        public int ParentFieldType
        {
            get { return m_nParentFieldType; }
        }
        public object Parent
        {
            get {return m_parent;}
        }
        public string MainTable
        {
            get {return m_strMainTableName;}    
        }
        public int ModalTabId
        {
            get { return m_nModlTabId; }
        }
        //add by cuiwei 07.10.16
        public int TabID { get { return this.ModalTabId; } }
        //add end

        //public int ModId
        //{
        //   get { return m_nModID; }
        //}
        /*
        public int ModalId
        {
            get {return m_nModalId;}
        }
         */
        public int ParentFieldID
        {
            get { return m_nParentFieldId; }
        }
        public bool IsVirtualField
        {
            get { return m_bDBVirtualField; }
            set { m_bDBVirtualField = value; }
        }

        public object[] DBPrimaryKey
        {
            get { return m_strDBPrimaryKeys; }
        }

        public object[] PrimaryKey
        {
            get { return m_strPrimaryKeys; }
        }        

        public bool IsPrimaryKeysSameAsDB
        {
            get { return m_bPrimaryKeysSameAsDB; }
        }
        public void ReInitMetaInfo()
        {
            InitMetaInfo(m_strSql);//, m_strMainTableName);//, m_strPrimaryKeys);
        }

        //获取当前使用的数据表
        public System.Data.DataTable DataTable
        {
            get { return m_tbl_MainData; }
        }

        //注意，仅为OBJPOOL使用
        public void ReInit(object Parent, int nParentFieldType)
        {
            m_parent = Parent;
            m_nParentFieldType = nParentFieldType;
            ClearDBDataCache();
        }

        public void ClearDBDataCache()
        {
            CustomClassCache.DelDataTableCache(m_strSql);

            foreach (DBCustomProperty subprop in this)
            {
                DBCustomClass dbCustomClass = subprop.Value as DBCustomClass;

                if (dbCustomClass != null)
                {                    
                    dbCustomClass.ClearDBDataCache();   
                }                
            }
        }        

        public CustomClass ReloadDBData(object[] keyvalues) // 重新载入除元数据之外的内容
        {
            CustomClass customClass = null;                                
            
            if (keyvalues != null) 
            {
                customClass = GetDynamicClass(keyvalues); // GetDynamicClass()已经调用过InitDBInfo()和ClearProperties()了
            }

            return customClass;
        }

        public void ReloadCurrentInfo()
        {
            CustomClassCache.DelDataTableCache(m_strSql);
            ReInitMetaInfo();
            if (m_findkeys != null)
            {
                GetDynamicClass(m_findkeys);
            }
        }
        
        //查找主键,如果是有父字段的情况,需要从父字段的SubTableFilter(关联关系)字段中读取主键定义
        public string[] GetKeyFromParentField(int nFieldId, bool bReadMainField)
        {
            string strMetaSql = string.Format("SELECT subtablefilter FROM sys_meta_info WHERE fieldid = {0}", nFieldId);//todo:这个应该在initmetadata中初始化完成
            DataRow row = CustomClassCache.GetDbMetaInfoProxy("sys_meta_info", strMetaSql).Rows[0];
            string strFilter = GetFieldStr(row, "subtablefilter");
            string[] strkeys = GetSplitField(strFilter, !bReadMainField).ToArray();
           
            return strkeys;
        }

        private bool CompareKeys(string[] keys1, string[] keys2)
        {
            if (keys1.Length != keys2.Length)
            {
                return false;
            }

            for (int i = 0; i < keys1.Length; i++)
            {
                if (keys1[i].ToLower() != keys2[i].ToLower())
                {
                    return false;
                }
            }

            return true;
        }

        private string[] GetPrimaryKeys()
        {
            string[] strKeys;
            m_strDBPrimaryKeys = CustomClassCache.GetPrimaryKeys(m_strMainTableName).ToArray();
            // m_strDBPrimaryKeys = CustomClassCache.GetKeyFromDB(m_strSql);

            if (m_nParentFieldId < 0 || m_nParentFieldType == 2) // 是主表字段或者是带子字段的非虚拟字段
            {
                strKeys = m_strDBPrimaryKeys;
                m_bPrimaryKeysSameAsDB = true;
            }
            else
            {                
                strKeys = GetKeyFromParentField(m_nParentFieldId, false);
                // m_bPrimaryKeysSameAsDB = Helper.CompareStringArray(m_strDBPrimaryKeys, strKeys, true);  
                m_bPrimaryKeysSameAsDB = CompareKeys(m_strDBPrimaryKeys, strKeys);               
            }

            return strKeys;
        }

        private void SetDBPrimaryKeys(string[] strkeys)
        {
            DataColumn[] keys = new DataColumn[strkeys.Length];
            int nKeyCount = 0;
            for (int i = 0; i < strkeys.Length; i++)
            {
                keys[i] = m_tbl_MainData.Columns[strkeys[i]];
                nKeyCount++;
            }
            m_tbl_MainData.PrimaryKey = keys;
        }
        private void ResetPrimaryKey()
        {
            //设置主表主键,如果同时匹配到多条记录，需要拆分成不同的CustomClass，直接执行PrimaryKey=xxx会异常
            if (m_bPrimaryKeysSameAsDB && m_strPrimaryKeys != null && m_tbl_MainData.PrimaryKey.Length == 0)
            {
                SetDBPrimaryKeys(m_strPrimaryKeys);
            }
        }

        // 重新读数据库数据
        private void InitDBInfo(string strmaindatasql, string[] keys, object[] keyvalues)
        {
            string strsql = string.Empty;
            string sql;
            if (keyvalues.Length > 0)
            {
                sql = GetFilter(keys, keyvalues);
                strsql = string.Format("{0} where {1}", strmaindatasql, sql);
                StrSql = strsql;

                m_tbl_MainData = CustomClassCache.GetDataTableWithoutKeyProxy(m_strMainTableName, sql, null, ref m_adp);
            }
            else
            {
                Debug.Assert(true);
                strsql = strmaindatasql;
                StrSql = strsql;
                m_tbl_MainData = CustomClassCache.GetDataTableWithoutKeyProxy(m_strMainTableName, ref m_adp);
            }

            /*m_tbl_MainData = CustomClassCache.GetDataTableWithoutKey(strsql, ref m_adp);*/

            ResetPrimaryKey();
        }

        //初始化数据库连接，及相关的元数据读取
        private void InitMetaInfo(string strsql)
        {
            m_conn = MainForm.conn;
            m_listSortedCol = new List<string>();
            m_tbl_FieldName_CN = CustomClassCache.GetDbMetaInfoProxy("", m_strMetaSql);
            m_colName.Clear();

            // 记录所有相关表元记录的内容
            foreach (DataRow row in m_tbl_FieldName_CN.Rows)
            {                
                string fieldname = GetFieldStr(row, "fieldname");
                string strEditorType = GetFieldStr(row, "editortype");

                object[] ao = new object[] { 
                    GetFieldStr(row, "fieldcnname"), 
                    GetFieldStr(row, "description"), 
                    GetFieldStr(row, "catname"), 
                    strEditorType,
                    GetFieldStr(row, "readonly") ,
                    GetFieldStr(row, "relativepath") ,
                    GetFieldStr(row, "modtabid") ,
                    GetFieldStr(row, "tablename") ,
                    fieldname,
                    GetFieldStr(row, "fieldid") ,
                    GetFieldStr(row, "fieldtype") ,
                    GetFieldStr(row, "sorttype") ,
                    GetFieldStr(row, "cutrelativepath") ,
                    GetFieldStr(row, "cutpreslash") ,
                    GetFieldStr(row, "visible") ,
                    };
               
                m_colName[fieldname] = ao; // 规定同一层全部来自同一张表，因此字段名不会相同
                m_listSortedCol.Add(fieldname);

                if (strEditorType == "lookupcombo") // 发现连接查询字段，需要从数据库获取相关下拉显示内容
                {
                    FillComboList(fieldname, row, m_conn);
                }
                else if (strEditorType == "textcombo")
                {
                    FillTextComboList(fieldname, row);
                }
            }

            // 初始化所有相关的脚本 #代表不针对某个字段的脚本            
            // luacode只存当前层的脚本            
            if (m_nParentFieldId >= 0)
            {
                m_strScriptSQL = string.Format("SELECT s.fieldname, s.scriptcode FROM sys_scriptstore s LEFT OUTER JOIN sys_meta_info m ON s.fieldid = m.fieldid WHERE (s.modtabid = '{0}') AND (s.tablename = '{1}') AND (m.parentfield={2})"
                                               , m_nModlTabId, m_strMainTableName, m_nParentFieldId);
            }
            else
            {
                m_strScriptSQL = string.Format("SELECT s.fieldname, s.scriptcode FROM sys_scriptstore s LEFT OUTER JOIN sys_meta_info m ON s.fieldid = m.fieldid WHERE (s.fieldid != -1) AND (s.modtabid = '{0}') AND (m.parentfield is null OR m.parentfield = 0)"
                                               , m_nModlTabId);
            }

            DataTable tblMainCode = CustomClassCache.GetScriptStoreProxy(new string[]{"sys_scriptstore","sys_meta_info"}, m_strScriptSQL);

            foreach (DataRow row in tblMainCode.Rows)
            {
                string strfname = GetFieldStr(row, "fieldname"); 
                string scriptText = Helper.GetStringFromByteArray(row["scriptcode"]).Trim();
                m_luacodes[strfname] = scriptText;
                
                if (strfname == "" || strfname == "#")
                {
                    m_luacodes["#"] = scriptText; // 设置为tab页服务的代码段
                }
            }

            string[] names = new string[] { "sys_modl_tab_def", "sys_modl_tab_def" };

            // ## 的处理
            string strsql_ = string.Format("SELECT TOP 1 modtabid FROM sys_modl_tab_def WHERE (modid = (SELECT modid FROM sys_modl_tab_def WHERE (modtabid = {0})))", m_nModlTabId);
            DataTable tbl_ = CustomClassCache.GetModTabDefProxy("sys_modl_tab_def", strsql_);
            string first_modtabid = tbl_.Rows[0][0].ToString();
            string strsql__ = string.Format("SELECT scriptcode FROM sys_scriptstore WHERE tablename='{0}' AND fieldid=-1 AND modtabid={1}", m_strMainTableName, first_modtabid);
            DataTable tbl__ = CustomClassCache.GetScriptStoreProxy(names, strsql__);
            
            /* 这里的代码好诡异，只取第一行数据的话完全没必要这样写 suntao
            foreach (System.Data.DataRow row in tbl__.Rows)
            {
                m_luacodes["##"] = Helper.GetStringFromByteArray(row["scriptcode"]).Trim();
                break;
            }
            */

            if (tbl__.Rows.Count > 0)
            {
                m_luacodes["##"] = Helper.GetStringFromByteArray(tbl__.Rows[0][0]).Trim();
            }            
        }

        private DBCustomProperty GetProperty(DataRow CurrRow, string strColId)
        {
            long beginTick = DateTime.Now.Ticks;

            object[] args = (object[])(m_colName[strColId]);
            bool bVirtualFeild = m_tbl_MainData.Columns.IndexOf(strColId) < 0;            
            string strcoldbid = strColId;                        
            string strcolname = (string)args[0]; // 中文列名
            string strdes = (string)args[1]; // 列描述内容            
            string strtype = (string)args[3]; // 数据类型名称，会影响值的编辑方式
            int nSortType = (string)args[11] == "" ? 0 : Convert.ToInt32(args[11]);//当为虚拟字段时，对从表记录的排序方式有影响
            string strrelativepath = (string)args[5]; // 相对路径名
            string strcutrelativepath = (string)args[12];
            string strmodtabid = (string)args[6]; // 模块tabid
            bool bCutPreSlash = ((string)args[13] == "") ? true : Convert.ToBoolean(args[13]);
            bool bVisible = (string)args[14] == "false" ? false : true;

            bool breadonly = false;

            if (args[4].ToString() != "")
            {
                breadonly = Convert.ToBoolean(args[4]);    //是否只读
            }
            else
            {
                breadonly = true;
            }

            string strcat = (string)args[2]; // 类别名

            if (strcat == "")
            {
                strcat = "未分类";
            }

            string strFieldType = args[10] == DBNull.Value ? "0" : (string)args[10]; // == "1" 为带子字段的虚拟字段

            if (strFieldType == "1" || strFieldType == "2") // ==1为虚拟字段，==2为普通字段下有虚拟字段
            {
                strtype = "tree"; // 强行设为树结点
            }

            object value = null;
            int nFieldID = args[9] == DBNull.Value ? 0 : Convert.ToInt32(args[9]);

            if (bVirtualFeild) // m_colName.ContainsKey(strcolname))                
            {                
                strtype = "tree"; // 强行设为树结点
                strcoldbid = strColId.ToLower();
            }
            else
            {
                value = CurrRow[strcoldbid];
                
                if (value == null || value == DBNull.Value)
                {
                    DataRow def_row = CustomClassCache.GetPropDefaultValue(m_strMainTableName);

                    if (def_row != null)
                    {
                        value = def_row[strcoldbid];
                    }
                }

                DataColumn col = m_tbl_MainData.Columns[strColId];
                strcoldbid = col.ColumnName.ToLower();
            }            

            enumValueType vtType = enumValueType.vtUnknown;
            DBCustomClass subNode = null;
            
            switch (strtype)
            {
                case "text":
                    vtType = enumValueType.vtText;
                    break;
                case "bool":
                    vtType = enumValueType.vtBool;
                    break;
                case "filename":
                    vtType = enumValueType.vtFileName;
                    break;
                case "filenamescript":
                    vtType = enumValueType.vtFileNameScript;
                    break;
                case "filenamesfx":
                    vtType = enumValueType.vtFileNameSfx;
                    break;
                case "pathname":
                    vtType = enumValueType.vtPath;
                    break;
                case "tree":
                    // 如果m_strPrimaryKeys不＝当前主表实际的主键值的话，就确定为一对多的关系, //需要开始分拆不同的CustomClass
                    vtType = enumValueType.vtExpandNode;
                    subNode = ObjPool.GetDBClass(this, m_lua, m_nModlTabId, nFieldID, int.Parse(strFieldType)); // 注意：只有虚拟字段才会切换到新的主表名
                    subNode.ID = strcoldbid; // 只能假定与真实字段名不相同,不然会混乱
                    subNode.Description = strdes + "...";
                    subNode.Value = "...";
                    subNode.SortType = nSortType;
                    object[] findkeyvalues;

                    if (strFieldType == "2") // 如果是普通字段下的虚拟字段，需要把自己的值作为虚拟字段的key值传递进去
                    {
                        findkeyvalues = GetChildKeyValues(CurrRow, m_strDBPrimaryKeys); // 修正主子表时,找错子表记录的问题
                        
                        if (m_ReplaceSubKeyProperty == null)
                        {
                            subNode.Value = CurrRow[strcoldbid];

                            if (subNode.Value == null || subNode.Value == DBNull.Value)
                            {
                                subNode.Value = value;                                
                            }
                        }
                        else
                        {
                            subNode.Value = Value;
                        }

                        subNode.DefaultValue = subNode.Value;
                    }
                    else
                    {
                        subNode.Value = strdes + "...";
                        
                        // bubfix:只要有关联关系的，一定是用主表的字段值传递进去查找从表记录
                        findkeyvalues = GetChildKeyValues(CurrRow, GetKeyFromParentField(nFieldID, true));                        
                    }

                    // 取本CLASS的KEY值字段,并且读取当前记录的内容作为findkeyvalues的内容
                    value = subNode.GetDynamicClass(findkeyvalues);
                    subNode.SetEvents(subNode.AllEvents);
                    
                    if (strFieldType == "1")
                    {
                        subNode.IsVirtualField = true;
                    }

                    break;
                case "customeditor":
                    vtType = enumValueType.vtCustomEditor;
                    break;
                default:
                    break;
            }

            if (m_luacodes[strcoldbid] != null && (m_luacodes[strcoldbid] as string).Trim().Length > 0)
            {
                strcolname += " [√]";
            }

            DBCustomProperty prop = ObjPool.GetProperty(nFieldID, this, strcoldbid, strcolname, value, breadonly, bVisible, vtType);            
            prop.Category = strcat;
            prop.Description = strdes;
            prop.Key = nFieldID.ToString();

            // 下拉框处理，将属性相关的hashtable存入属性中
            if (m_combox_Keys.ContainsKey(strcoldbid))
            {
                prop.ValueType = enumValueType.vtCombo;                
                prop.ComboValues = m_combox_Keys[strcoldbid] as Hashtable;
                Hashtable htValues = (Hashtable)m_combox_Values[strcoldbid];
                prop.ComboKeys = htValues;
                prop.ComboValuesEx = (Hashtable)m_combox_ExValues[strcoldbid];
                
                if (value != null)
                {
                    prop.Value = htValues[value.ToString()];

                    if (prop.Value == null)
                    {
                        prop.Value = value;
                    }
                }
                else
                {
                    prop.Value = "";
                }
            }

            switch (vtType)
            {
                case enumValueType.vtBool:
                    if (prop.Value != DBNull.Value)
                    {
                        if (m_tbl_MainData.Columns[prop.ID].DataType == typeof(System.Boolean))
                            prop.Value = Convert.ToBoolean(prop.Value) == true ? "是" : "否";
                        else
                            prop.Value = Convert.ToString(prop.Value) == "1" ? "是" : "否";
                    }
                    else
                        prop.Value = "否";
                    break;
                case enumValueType.vtText:
                    if (prop.Value == DBNull.Value)
                        prop.Value = "";
                    break;
                case enumValueType.vtCustomEditor:
                    if (prop.Value == DBNull.Value)
                        prop.Value = "";
                    break;
                case enumValueType.vtFileName:
                    strrelativepath = strrelativepath.TrimStart(new char[] { '\\' });
                    strcutrelativepath = strcutrelativepath.TrimStart(new char[] { '\\' });
                    prop.Prefix = Helper.CombinePaths(Program.RootDir, strrelativepath); 
                    prop.CutPrefix = Helper.CombinePaths(Program.RootDir, strcutrelativepath); 
                    prop.CutPreSlash = bCutPreSlash;

                    if (prop.Value == DBNull.Value)
                        prop.Value = "";
                    break;
                case enumValueType.vtExpandNode:
                    subNode.Property = prop;
                    break;
                default:
                    break;
            }

            long endTick = DateTime.Now.Ticks;
            string timeString = string.Format("GetProperty\t{0}\t{1}", strColId, endTick - beginTick);
            Debug.WriteLine(timeString);

            return prop;
        }

        //修改从表中的findkeys
        public void ReplaceSubFindKey(CustomProperty prop, object value)
        {
            //不能直接使用value,因可能是修改的非主键字段,要获取这个真正的主键
            //Value = value;
            //GetDynamicClass(value);
            //需要根据value值获得真正的key值
            //其实KEY没有变,只是与其他表关联的字段内容在内存中改变了(如represent1)
            object oldvalue = Value;
            prop.Tag = m_findkeyvalues;
            try
            {
                //先解锁之前关联的可能修改过的从表记录
                OnLockRecord(false, null, null);

                Value = value;
                m_ReplaceSubKeyProperty = prop; //记住当前修改的字段ID，以便临时改动子结点中的内容
                GetDynamicClass(m_findkeyvalues);
            }
            finally
            {
                m_ReplaceSubKeyProperty = null;
            }
        }
        //修改本层主表的findkeys
        private bool ModifyFindKeys(string strkeyfield, object value)
        {
            //如果非顶层的DBCLASS的主表关键值被修改，则需要修改findkeys
            //如果同时还是顶层的DBCLASS(parentfield=-1),则还需要在保存的同时通知模块树，FINDKEYS发生了变化
            int nFindKey = -1;
            for(int i = 0; i < m_strDBPrimaryKeys.Length; i ++)
            {
                string strkey = m_strDBPrimaryKeys[i].ToLower();
                if (strkey == strkeyfield.ToLower())
                {
                    nFindKey = i;
                    break;
                }
            }
            if (nFindKey >= 0)
            {
                m_findkeyvalues[nFindKey] = value.ToString();
                m_findkeys[nFindKey] = value.ToString();
                return true;
            }
            else
                return false;
        }
        public void ModifyFindKeys(CustomProperty prop, object value)
        {
            //如果非顶层的DBCLASS的主表关键值被修改，则需要修改findkeys
            //如果同时还是顶层的DBCLASS(parentfield=-1),则还需要在保存的同时通知模块树，FINDKEYS发生了变化            
            if (ModifyFindKeys(prop.ID, value))
            {
                DBCustomClass cls = this;
                while (cls.ParentFieldType != 2 && cls.Parent != null)
                {
                    if (cls.Parent is DBCustomClass)
                    {
                        cls = (DBCustomClass)(cls.Parent);
                    }
                }
                if (Parent == null || this.MainTable == cls.MainTable) //顶层或者是修改了模块主表中的主键值
                {
                    OnAsynNotifyEvent(3, prop);//通知FORM，保存时要刷新模块树
                }
            }
        }

        public void ClearProperties()
        {
            foreach (DBCustomProperty subprop in this)
            {
                DBCustomClass dbCustomClass = subprop.Value as DBCustomClass;

                if (dbCustomClass is DBCustomClass)
                {
                    dbCustomClass.ClearProperties();
                    ObjPool.DelDBClass(dbCustomClass);
                }

                ObjPool.DelProperty(subprop);
            }

            this.Clear();
        }

        private void GetClassContent(CustomClass clsParent, DataRow CurrRow, object[] keyvalues)
        {
            if (CurrRow == null) // 如果没有找到相关记录，则清除下属所有的属性。
            {
                ClearProperties();
                return;
            }           

            m_findkeys = keyvalues;
            ClearProperties();            

            for (int i = 0; i < m_listSortedCol.Count; i ++ )
            {
                this.Add(GetProperty(CurrRow, m_listSortedCol[i]));                
            }
        }

        private string GetFilter(string[] strkeys, object[] keyvalues)
        {
            string strfilter = "1 = 1"; //方便生成下一个条件
            if (strkeys.Length != keyvalues.Length)
            {
                System.Windows.Forms.MessageBox.Show("输入的查询键值与初始化时使用的键值数组大小不致，请确认 GetDynmamicClass(keys)中的keys是正确的。");
                return strfilter;
            }
            for(int i = 0; i < strkeys.Length; i ++)
            {
                strfilter += string.Format(" AND {0} = '{1}'", strkeys[i], keyvalues[i]);
            }
            return strfilter;
        }

        //根据键值，生成完全的可唯一定位从表一条记录的 findkeyvalues
        private object[] GetChildKeyValues(DataRow row, string[] primaryKeys)
        {
            object[] KeyValues = new string[primaryKeys.Length];
            DataRow def_row = null;
            bool readDefaultRow = false;

            for (int i =  0;i < primaryKeys.Length; i++)
            {
                string primaryKeyString = primaryKeys[i].ToLower();

                // add by cuiwei 07.11.15 尝试读取修改了但是没保存的值，因为datarow的数据太旧了，无法应对多主键的情况
                DBCustomClass dbc = this.Parent as DBCustomClass;
                if (dbc != null)
                {
                    CustomProperty prop = dbc[primaryKeyString];                    

                    if (prop != null)
                    {
                        string keyValue = prop.Value as string;

                        if (keyValue != null)
                        {
                            KeyValues[i] = keyValue;
                            continue;
                        }                        
                    }
                }
                // add end

                if (m_ReplaceSubKeyProperty != null && m_ReplaceSubKeyProperty.ID.ToLower() == primaryKeyString)
                {
                    KeyValues[i] = Value.ToString();
                }
                else
                {
                    object ovalue = row[primaryKeys[i]];

                    if ((ovalue == null || ovalue == DBNull.Value)) // 为空要读取默认行的值
                    {
                        if (def_row == null && !readDefaultRow) // 已经读过一次默认行就没必要继续读了
                        {
                            def_row = CustomClassCache.GetPropDefaultValue(m_strMainTableName);
                            readDefaultRow = true;
                        }
                        
                        if (def_row != null)
                        {
                            ovalue = def_row[primaryKeys[i]]; 
                        }                                                                       
                    }

                    KeyValues[i] = ovalue.ToString();
                }
            }

            return KeyValues;
        }

        // 按字段名排序：以字段名为父结点，记录号为叶结点组织显示
        // 一对多时，需要特殊处理，通过虚拟父结点(1,2,3为名的虚拟属性，一个属性代表一条唯一的记录)
        // 生成最子表中最低层的叶子结点
        private void Load1MultipleRecord(object[] keyvalues)
        {
            bool lockRecord = false; // 默认行的子表记录是不能被修改的，所以加锁没意义，优化掉。

            foreach (object key in keyvalues)
            {
                if (key.ToString() != "0")
                {
                    lockRecord = true;
                    break;
                }
            }

            foreach (string strcolname in m_listSortedCol)
            {
                // 虚拟字段，以子表一个字段名为名字
                DBCustomClass ChildNodes = new DBCustomClass(this, m_lua, m_nModlTabId, m_nParentFieldId); // 虚拟字段下第二级可展开字段                
                ChildNodes.Load1MultipleRecordLine(strcolname, m_strPrimaryKeys, keyvalues, m_strDBPrimaryKeys, lockRecord); // 生成一对多的特殊情况

                object[] args = (object[])(m_colName[strcolname]);
                int nFieldID = args[9] == DBNull.Value ? 0 : Convert.ToInt32(args[9]);                
                DBCustomProperty p = ObjPool.GetProperty(nFieldID, this, strcolname, (string)args[0], ChildNodes, false, true, enumValueType.vtExpandNode);
                p.Name = (string)args[0]; // 中文名
                p.Description = (string)args[1];
                p.Category = (string)args[2]; // 类别名
                p.Value = ChildNodes;
                p.Key = (string)args[9];
                p.IsCanHideNode = (Parent != null); // 如果不是顶层结点，则是可以被隐藏的结点
                p.ReadOnly = true;

                this.Add(p);

                //生成子记录的详细key值                
                ChildNodes.Value = "...";
                ChildNodes.SetEvents(ChildNodes.AllEvents);                
            }
        }

        //按字段名排序：以字段名为父结点，记录号为叶结点组织显示
        public void Load1MultipleRecordLine(string colname, string[] parentKeys, object[] parentKeyValues, string[] primaryKeys, bool lockRecord)
        {
            m_strPrimaryKeys = primaryKeys;
            m_bPrimaryKeysSameAsDB = true;

            // 需要初始化特殊的表内容，因此时还没有调用过 getdynamicclass,就没有初始化数据表
            InitDBInfo(m_strSql, parentKeys, parentKeyValues);
            SetDBPrimaryKeys(m_strPrimaryKeys);

            object[] childKeys = null;
            DataRow[] rows = m_tbl_MainData.Select(GetFilter(parentKeys, parentKeyValues)); // "skillid = '28' and skilllevel = '1'");

            // 显示额外的提示字段内容
            string strPrompField = null;                    

            for (int i = 0; i < rows.Length; i++) // N个记录,需要把所有符合的记录集检查出来
            {
                DataRow row = rows[i];
                childKeys = GetChildKeyValues(row, m_strPrimaryKeys);
                DBCustomProperty prop = GetProperty(row, colname);

                if (strPrompField != null)
                {
                    prop.Name = rows.Length == 1 ? prop.Name : string.Format("{0}# : {1}", i + 1, row[strPrompField].ToString());
                }
                else
                {
                    prop.Name = rows.Length == 1 ? prop.Name : string.Format("{0}#", i + 1);
                }

                prop.Tag = childKeys; // 仅用于保存时使用，也可以用于删除
                prop.Lock = new RecordLockEx(Conn, MainTable, DBPrimaryKey, m_nModlId);

                if (lockRecord) // 只有在修改非默认行的子表记录时才加锁。
                {
                    prop.UpdateLastVersion();
                }
                
                this.Add(prop); // 只生成指定中文名字(记录号)的属性名, colcnname, colid
            }

            m_nRecordCound = rows.Length;
        }

        //按记录号排序：以记录号为父结点，以属性为叶结点组织显示
        //1对多另一种排序生成方式，以记录为虚拟字段，下属的为此记录相关的属性
        private void Load1MultipleRecordRecordMode(string[] parentKeys, object[] parentKeyValues, string[] primaryKeys)
        {
            object[] childKeys = null;

            // 遍历每条记录，并每条记录生成一个CustomClass,以记录号为名,将下面的内容包装起来
            DataRow[] rows = m_tbl_MainData.Select(GetFilter(parentKeys, parentKeyValues)); // "skillid = '28' and skilllevel = '1'");
            m_nRecordCound = rows.Length;

            for(int i = 0; i < rows.Length; i ++)
            {
                DataRow row = rows[i];
                DBCustomClass ChildNodes = new DBCustomClass(this, m_lua, m_nModlTabId, m_nParentFieldId); // 虚拟字段下第二级可展开字段
                childKeys = GetChildKeyValues(row, m_strDBPrimaryKeys);
                ChildNodes.Load1MultipleRecordRecordModeLine(m_strDBPrimaryKeys, childKeys); // 生成一对多的特殊情况，以记录号为树结点

                string strName = string.Format("{0}#", i + 1);
                DBCustomProperty p = new DBCustomProperty(this, strName, strName, ChildNodes, false, true, enumValueType.vtExpandNode); // 假属性暂不处理                

                bool lockRecord = false; // 默认行的子表记录是不能被修改的，所以加锁没意义，优化掉。

                foreach (object key in childKeys)
                {
                    if (key.ToString() != "0")
                    {
                        lockRecord = true;
                        break;
                    }
                }

                p.Lock = new RecordLockEx(Conn, MainTable, DBPrimaryKey, m_nModlId);

                if (lockRecord) // 只有在修改非默认行的子表记录时才加锁。
                {
                    p.UpdateLastVersion();
                }                

                p.Name = strName; // 中文名
                p.Description = "从表中的记录";
                p.Value = ChildNodes;
                p.Key = "-1";
                p.Tag = childKeys;
                p.IsCanHideNode = Parent != null; // 如果不是顶层结点，则是可以被隐藏的结点
                p.ReadOnly = true;
                this.Add(p);

                ChildNodes.Value = "...";
                ChildNodes.SetEvents(ChildNodes.AllEvents);                
            }
        }

        //按记录号排序：
        public void Load1MultipleRecordRecordModeLine(string[] primaryKeys, object[] childKeyValues)
        {
            m_strPrimaryKeys = primaryKeys;
            m_bPrimaryKeysSameAsDB = true;

            // 需要初始化特殊的表内容，因此时还没有调用过 getdynamicclass,就没有初始化数据表
            InitDBInfo(m_strSql, m_strPrimaryKeys, childKeyValues);
            SetDBPrimaryKeys(m_strPrimaryKeys);
            GetDynamicClass(childKeyValues);

            foreach (CustomProperty prop in this)
            {
                if ((m_parent != null) && (m_parent as DBCustomClass).SortType == 1) // 如果是按记录排序，则需要把 # 号挪过来，不然每个记录名字一样
                {
                    StringBuilder strkeys = new StringBuilder();

                    for (int i = 0; i < childKeyValues.Length; i ++)
                    {
                        if (i > 0)
                        {
                            strkeys.Append(", ");
                        }

                        strkeys.Append(childKeyValues[i]);
                    }

                    prop.Name = string.Format("{0} [{1}]", prop.Name, strkeys.ToString());
                }                
            }
        }

        public Hashtable AllEvents
        {
            get
            {
                if (m_events == null)
                {
                    m_events = new Hashtable();
                    m_events["PropertyChangeEventHandler"] = new CustomClass.PropertyChangeEventHandler(BeforePropertyChangedEvent);
                    m_events["PropertyAfterChangeEventHandler"] = new CustomClass.PropertyAfterChangeEventHandler(AfterPropertyChangedEvent);
                    m_events["CustomEditValueEventHandler"] = new CustomClass.CustomEditValueEventHandler(OnCustomEditorEditValueEvent);
                    m_events["TreeValueEventHandler"] = new CustomClass.TreeValueEventHandler(OnGetTreeValueEvent);
                }

                return m_events;
            }
        }

        //如果有查找值有空字符串，必须转为NULL，否则会FIND出错
        object[] PreProcessKeys(object[] keys)
        {
            for(int i = 0; i < keys.Length; i ++)
            {
                object obj = keys[i];
                if (obj.ToString().Length <= 0)
                {
                    keys[i] = null;
                }
            }
            return keys;
        }
        //获得具有动态元信息的类实体
        //注意，如果是子表的话，可能不会唯一返回记录，需要自动为每条记录创建一个CustomClass
        public CustomClass GetDynamicClass(object[] keyvalues)
        {
            m_initFinished = false;

            if (keyvalues == null)
            {
                return null;
            }

            // 在开始显示内容时才作数据库初始化，只获取需要的主表数据
            InitDBInfo(m_strSql, m_strPrimaryKeys, keyvalues);
            m_findkeyvalues = keyvalues;   
            
            bool lockRecord = false;

            if (m_strMainTableName != m_TabMainTableName) // 虚拟字段如果是默认行子表的话是不能被修改的，所以不用加锁
            {                
                foreach (object o in keyvalues)
                {
                    if (o.ToString() != "0")
                    {
                        lockRecord = true;
                        break;
                    }
                }         
            }
            else
            {
                lockRecord = true;
            }

            if (lockRecord)
            {
                m_reclock.UpdateLocalVersion(m_findkeyvalues);
            }

            if (m_bPrimaryKeysSameAsDB)
            {
                DataRow row = m_tbl_MainData.Rows.Find(PreProcessKeys(keyvalues));
                m_nRecordCound = (row == null ? 0 : 1);
                GetClassContent(this, row, keyvalues);
            }
            else
            {
                this.IsVirtualField = true;
                ClearProperties();

                if (SortType == 0) // 以字段名为父结点组织1对多中不同记录的内容
                {
                    Load1MultipleRecord(keyvalues);
                }
                else // 以记录号为父结点组织形式，１对多不同记录
                {
                    Load1MultipleRecordRecordMode(m_strPrimaryKeys, keyvalues, m_strDBPrimaryKeys);
                }
            }

            SetEvents(AllEvents);
            m_initFinished = true;

            return this;
        }

        // 生成被修改的默认值的list，作为baseform的更新依据。
        public void GenerateModifiedDefaultList(CustomProperty prop, ref ArrayList lsTblNeedUpdateDefault)
        {
            // 判断 m_findkeys, 是否在修改默认值
            // 所有的tablename, 老值, 新值, 
            bool bThisPropInDefaultRow = true;

            if (m_findkeys != null)
            {
                foreach (object key in m_findkeys)
                {
                    if (key.ToString().Trim() != "0")
                    {
                        bThisPropInDefaultRow = false;
                        break;
                    }
                }
            }
            else
            {
                if (prop.Tag is object[])
                {
                    foreach (object obj in (object[])(prop.Tag))
                    {
                        if (obj.ToString().Trim() != "0")
                        {
                            bThisPropInDefaultRow = false;
                            break;
                        }
                    }
                }
            }

            if (bThisPropInDefaultRow)
            {
                bool bAlreadyAdd = false;
                foreach (TblUpdateDefault tud in lsTblNeedUpdateDefault)
                {
                    if (tud.tblName == m_strMainTableName)
                    {
                        bAlreadyAdd = true;
                        break;
                    }
                }

                if (!bAlreadyAdd)
                {
                    TblUpdateDefault tud = new TblUpdateDefault();
                    tud.tblName = m_strMainTableName;
                    //DataTable tbl = Helper.GetDataTable("SELECT * FROM " + m_strMainTableName, Conn);
                    DataTable tbl = Helper.GetDataTableProxy(m_strMainTableName, Conn);
                    DataRow row = tbl.Rows.Find(PreProcessKeys(m_findkeys));
                    tud.oldValue = row;
                    DataRow newrow = m_tbl_MainData.Rows.Find(PreProcessKeys(m_findkeys));
                    tud.newValue = newrow;
                    lsTblNeedUpdateDefault.Add(tud);
                }
            }
        }

        public bool DeleteRecord(CustomProperty prop)
        {
            string[] findKeys;
            if (prop.Tag == null)
            {
                DBCustomClass cls = prop.Parent as DBCustomClass;
                findKeys = (string[])cls.FindKeys;
            }
            else
                findKeys = (string[])prop.Tag;

            if (findKeys != null)
            {
                //SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(m_adp);
                //m_adp.DeleteCommand = cmdBuilder.GetDeleteCommand();
                DataRow delRow = m_tbl_MainData.Rows.Find(PreProcessKeys(findKeys));
                if (delRow == null)
                {
                    MessageBox.Show(string.Format("在[{0}]中删除记录失败。", MainTable), "错误",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
                delRow.Delete();
                //int nRet = m_adp.Update(new DataRow[] { delRow });
                Helper.SaveTable(m_tbl_MainData, StrSql, MainForm.conn);
                m_tbl_MainData.AcceptChanges();
                DBCustomClass cls = this;
                while (cls.ParentFieldType != 2 && cls.Parent != null)
                {
                    if (cls.Parent is DBCustomClass)
                    {
                        cls = (DBCustomClass)(cls.Parent);
                    }
                }
                CustomClassCache.DelAllDataTableCache(); //清除缓冲
                if (cls.Property == null) //虚拟字段在顶层
                {
                    cls.GetDynamicClass(cls.FindKeys);
                    cls.OnAsynNotifyEvent(1, null);//todo:多个页存在同一表同一字段时，会导致重复刷新多次                    
                }
                else
                    cls.Property.SetValue(cls.Value);

                {
                    DBCustomClass parentcls = prop.Parent as DBCustomClass;
                    if (parentcls != null)
                        parentcls = parentcls.Parent as DBCustomClass;

                    if (parentcls.Property != null)
                    {
                        LuaFunction fun2 = m_lua.GetFunction(GetRealFunctionName(parentcls.Property.Key, "onafterdelrecord"));
                        if (fun2 != null)
                        {
                            object[] retobjs = fun2.Call();
                        }
                    }
                }

                return true;
            }
            return false;
        }               

        // 将数据保存入数据表中
        public bool Save(ref ArrayList lsTblNeedUpdateDefault, string strRemarks)
        {
            // 先预处理所有的树结点，否则在树结点前修改过row中的内容，会引起并发错误
            int nModifiedCount = 0;
            List<bool> shouldSerializeValueList = new List<bool>();

            for (int i = 0; i < this.Count; i++)
            {
                CustomProperty prop = this[i];
                DBCustomClass dbCustomClass = prop.Value as DBCustomClass;

                if (prop.ValueType == enumValueType.vtExpandNode && dbCustomClass != null)
                {
                    if (dbCustomClass.ValueChanged) // 值改变的时候才需要保存
                    {
                        bool bok = dbCustomClass.Save(ref lsTblNeedUpdateDefault, strRemarks);

                        if (!bok)
                        {
                            Helper.AddLog(string.Format("{0}保存出现意外。", prop.Name));
                        }
                    }                    
                }

                if (prop.ShouldSerializeValue())
                {
                    shouldSerializeValueList.Add(true);
                    nModifiedCount++;
                }
                else
                {
                    shouldSerializeValueList.Add(false);
                }
            }

            if (nModifiedCount == 0) // 如果没有改过本记录，则直接退出
                return true;

            // 用户在数据表编辑里关于是否与默认值比较,保存null的设置.
            int savetype = 0;
            string sql = string.Format("SELECT * FROM sys_table_cfg WHERE tablename='{0}'", m_strMainTableName);

            /*DataTable tbl = CustomClassCache.GetDataTableWithoutKey(sql);*/

            DataTable tbl = CustomClassCache.GetDataTableWithoutKeyProxy("sys_table_cfg", 
                string.Format("tablename ='{0}'", m_strMainTableName), null);
            
            if (tbl.Rows.Count > 0)
                savetype = Convert.ToInt32(tbl.Rows[0]["savetype"]);

            // 重新载入数据库中的数据，防止共享冲突错误
            if (ReloadDataOnSave)
            {
                CustomClassCache.DelDataTableCache(m_strSql);
               
                //m_tbl_MainData = CustomClassCache.GetDataTableWithoutKey(m_strSql);
                m_tbl_MainData = CustomClassCache.GetDataTableWithoutKeyProxy(m_strMainTableName);
                ResetPrimaryKey();
                ReloadDataOnSave = false;
            }
            
            List<DBCustomProperty> lstSavedProp = new List<DBCustomProperty>();
            DataRow MainSaveRow = null;
            
            for (int i = 0; i < this.Count; i++)
            {
                DBCustomProperty prop = this[i] as DBCustomProperty;

                if (shouldSerializeValueList[i])
                {
                    object value = null;

                    switch (prop.ValueType)
                    {
                        case enumValueType.vtCombo:
                            if (prop.Value == null || prop.Value.ToString().Trim() == "")
                            {
                                value = DBNull.Value;
                            }
                            else
                            {
                                value = prop.ComboValues[prop.Value];
                                if (value == null)
                                    value = prop.Value;
                            }
                            break;
                        case enumValueType.vtBool:
                            string sBool = prop.Value.ToString().Trim().ToLower();
                            if (m_tbl_MainData.Columns[prop.ID].DataType == typeof(System.Boolean))
                                value = sBool == "是" ? true : false;
                            else
                                value = sBool == "是" ? "1" : "0";
                            break;
                        case enumValueType.vtExpandNode:
                            if (prop.Value is DBCustomClass)
                            {
                                DBCustomClass cls = ((DBCustomClass)prop.Value);
                                //cls.Save();//为避免并发错误，提交在函数头进行保存
                                if (cls.ParentFieldType == 2)
                                    value = cls.Value;
                                else
                                    continue;
                            }
                            break;
                        default:
                            value = prop.Value;
                            break;
                    }

                    if (value == null)
                    {
                        value = DBNull.Value;
                    }
                    
                    DataRow CurSaveRow;
                    bool bMainSaveRow = true;
                    if (m_nParentFieldId < 0 || (m_bPrimaryKeysSameAsDB && m_bDBVirtualField) || prop.Tag == null) //在以记录号为树结点情况下，prop.tag值为空
                    {
                        bMainSaveRow = true;
                        if (MainSaveRow == null)
                        {
                            MainSaveRow = m_tbl_MainData.Rows.Find(PreProcessKeys(m_findkeys)); //todo:需要重新从数据库获取数据，以免发生共享冲突
                            DataTable tblrow = ReloadData(MainSaveRow, m_strMainTableName);
                            MainSaveRow = tblrow.Rows[0];
                        }
                        CurSaveRow = MainSaveRow;
                    }
                    else
                    {
                        bMainSaveRow = false;
                        CurSaveRow = m_tbl_MainData.Rows.Find(PreProcessKeys((string[])prop.Tag));//因每个属性可能来自不同记录
                        DataTable tblrow = ReloadData(CurSaveRow, m_strMainTableName);
                        CurSaveRow = tblrow.Rows[0];
                    }
                    if (CurSaveRow == null)
                    {
                        //CurSaveRow.RejectChanges(); //避免出现数据库并发访问错误
                        string message = "保存时出现在错误，有可能是用户直接修改了主键值导致后续修改的值无法再找到相关的记录。";
                        MessageBox.Show(message);
                        return false; //有可能是用户直接修改了主键值，导致后续修改的值无法再找到相关的记录
                    }



                    if (!SavePropertyData(CurSaveRow, prop, value, savetype, strRemarks, bMainSaveRow))
                    {
                        return false;
                    }
                    lstSavedProp.Add(prop);                    
                }
                
                // 默认值相关
                if (MainForm.m_bModifyDbTableDefault)
                    GenerateModifiedDefaultList(prop, ref lsTblNeedUpdateDefault);
            }

            CommitSave(strRemarks, MainSaveRow);
            
            // 先解锁
            if (!this.OnLockRecord(false, null, this.FindKeys))
            {
                Helper.AddLog(string.Format("{0}表解锁失败。", this.ToString()));
                return false;
            }

            // 刷新版本号 add by suntao
            this.RefreshLastVersion();            
            
            //PostSave 需要把这次提交过的属性检查一次，如果有主键值被改动，则需要将所有同表同记录的属性tag(findkey)作一次改动，否则后续的属性会因找不到记录保存失败
            RefreshFindKeys(ref lstSavedProp);
            lstSavedProp.Clear();
            m_valueChanged = false; // 重置变量

            return true;
        }

        private DataTable ReloadData(DataRow row, string tblname)
        {
            DataColumn[] keyColumns = row.Table.PrimaryKey;
            string filter = string.Empty;
            foreach (DataColumn column in keyColumns)
            {
                filter += string.Format("{0} = {1} and ", column.ColumnName, row[column]);  
            }
            filter = filter.Substring(0, filter.Length - 4);
            string sql = string.Format("select * from {0} where {1}", tblname, filter);
            DataTable tbl = Helper.GetDataTableProxy(tblname, filter, null, MainForm.conn);
            tbl.ExtendedProperties["sql"] = sql;
            return tbl;
        }

        //获取strfieldname是否主键，是主键，则返回在m_strDBPrimaryKeys数组中的索引号
        private int GetKeyIndex(string strfieldname)
        {
            //如果同时还是顶层的DBCLASS(parentfield=-1),则还需要在保存的同时通知模块树，FINDKEYS发生了变化
            string strkeyfield = strfieldname.ToLower();
            for (int i = 0; i < m_strDBPrimaryKeys.Length; i++)
            {
                string strkey = m_strDBPrimaryKeys[i].ToLower();
                if (strkey == strkeyfield)
                {
                    return i;
                }
            }
            return -1;
        }
        //判断两个主键值数据是否相同
        private bool IsSameKey(object[] keyvalue1, object[] keyvalue2)
        {
            if (keyvalue1.Length != keyvalue2.Length)
                return false;

            for (int i = 0; i < keyvalue1.Length; i++)
            {
                if (keyvalue1[i].ToString() != keyvalue2[i].ToString())
                    return false;
            }
            return true;
        }
        //实施递归查找并替换prop.tag中保存的FindKeyValues
        public void ReplacePropertyKey(string strMainTableName, object[] oldfindkeyvalues, object[] newfindkeyvalues)
        {
            foreach (DBCustomProperty subprop in this)
            {
                if (subprop.Value != null && subprop.Value is DBCustomClass)
                {
                    (subprop.Value as DBCustomClass).ReplacePropertyKey(strMainTableName, oldfindkeyvalues, newfindkeyvalues);
                }
                else if (strMainTableName == this.MainTable && subprop.Tag != null)
                {
                    object[] subpropfindkeyvalues = subprop.Tag as object[];
                    if (IsSameKey(oldfindkeyvalues, subpropfindkeyvalues))
                    {
                        subprop.Tag = newfindkeyvalues;
                    }
                }

            }
        }
        //刷新当前页面所有同表同记录的所有prop.Tag中记录的FindKeyValues
        private void RefreshFindKeys(ref List<DBCustomProperty> lstSavedProp)
        {
            foreach (DBCustomProperty prop in lstSavedProp)
            {
                int nKeyIndex = GetKeyIndex(prop.ID);
                if (!(prop.Value is DBCustomClass) && nKeyIndex >= 0) //如果是主键字段修改，保存后，需要刷新同一父结点下所有同一记录的 prop.tag
                {
                    if (prop.Tag != null && prop.Tag is object[])
                    {
                        object[] propfindkeyvalues = prop.Tag as object[];
                        object[] oldfindkeyvalues = propfindkeyvalues.Clone() as object[];

                        propfindkeyvalues[nKeyIndex] = prop.Value.ToString();

                        DBCustomClass cls = this;
                        while (cls.Parent != null)
                        {
                            cls = cls.Parent as DBCustomClass;
                        }
                        //使用最外层的 DBClass 进行 tag 替换
                        cls.ReplacePropertyKey(this.MainTable, oldfindkeyvalues, propfindkeyvalues);
                    }
                }
            }
        }

        private void CommitSave(string strRemarks, DataRow saverow)
        {

            if (saverow != null)
            {
                DataTable tbl = saverow.Table;
                string sql = tbl.ExtendedProperties["sql"] as string;
                Helper.SaveTable(tbl, sql, m_conn);

                foreach (DBCustomProperty prop in mRecordProList)
                {
                    prop.UpdateDefaultValue();
                }

                foreach (string[] record in mRecordList)
                {
                    DataRecord dataRecord = DataRecord.GetDataRecord();
                    dataRecord.Conn = m_conn; // sql连接
                    dataRecord.ModelID = m_nModlId; // 模块id
                    dataRecord.TabID = m_nModlTabId; // tab页id
                    dataRecord.CurrentNode = m_lua["SelectedNode"] as TreeNode; // 树结点
                    dataRecord.Time = DateTime.Now; // 当前修改时间
                    dataRecord.UserName = Helper.GetHostName(); // 当前用户名
                    dataRecord.OldValue = record[0]; // 字段的旧值        
                    dataRecord.NewValue = record[1]; // 字段的新值
                    dataRecord.FieldDisplayName = record[2]; // 字段的全路径
                    dataRecord.FieldID = int.Parse(record[3]); // 字段的id
                    dataRecord.Remarks = strRemarks;
                    dataRecord.Save();
                }

                
            }

            mRecordList.Clear();
            mRecordProList.Clear();
            try
            {
                Helper.SaveTable(m_tbl_MainData, m_strSql, Conn);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
                Helper.AddLog(ex.Message);
                throw ex;             
            }
        }
        private bool SavePropertyData(DataRow SaveRow, DBCustomProperty prop, object value, int savetype, string strRemarks, bool bMainSaveRow)
        {
            string oldValue = SaveRow[prop.ID].ToString(); // 旧值
            if (oldValue == "") // 为空，取默认值
            {
                DataRow defaultRow = CustomClassCache.GetPropDefaultValue(m_strMainTableName);
                if (defaultRow != null) // 默认行非空
                {
                    oldValue = defaultRow[prop.ID].ToString();
                }
            }

            //保存到数据表中
            if (prop.ValueType != enumValueType.vtUnknown)
            {
                try
                {
                    SaveRow[prop.ID] = value;

                    DataRow def_row = CustomClassCache.GetPropDefaultValue(m_strMainTableName);
                    if (def_row != null)
                    {
                        bool IsProcessingDefaultRow = true;

                        if (m_findkeys != null)
                        {
                            foreach (object obj in m_findkeys)
                            {
                                if (obj.ToString().Trim() != "0")
                                {
                                    IsProcessingDefaultRow = false;
                                    break;
                                }
                            }
                        }
                        else if (prop.Tag is object[])
                        {
                            foreach (object obj in (object[])(prop.Tag))
                            {
                                if (obj.ToString().Trim() != "0")
                                {
                                    IsProcessingDefaultRow = false;
                                    break;
                                }
                            }
                        }

                        if (!IsProcessingDefaultRow) // 如果保存的本来就是default row，可以不用处理。
                        {
                            object def_val = def_row[prop.ID];

                            bool bPropIsCat = false;
                            if (m_strCats != null)
                            {
                                foreach (string cat in m_strCats)
                                {
                                    if (prop.ID == cat)
                                    {
                                        bPropIsCat = true;
                                        break;
                                    }
                                }
                            }

                            // 如果是主键, 也不必和默认值比较了, 并存成 DBNull
                            bool bPropIsPrimaryKeys = false;
                            foreach (DataColumn col in m_tbl_MainData.PrimaryKey)
                            {
                                if (col.ColumnName.ToLower() == prop.ID.ToLower())
                                {
                                    bPropIsPrimaryKeys = true;
                                    break;
                                }
                            }

                            if (def_val.ToString() == value.ToString() && !bPropIsCat && savetype == 0 && !bPropIsPrimaryKeys)
                                SaveRow[prop.ID] = DBNull.Value;
                        }
                    }
                }
                catch (Exception ex)
                {
                    SaveRow[prop.ID] = DBNull.Value;
                    Helper.AddLog(ex.Message);
                }
            }


            // 将旧值和新值统一转换为true和false
            string newValue = value.ToString(); // 新值
            if (prop.ValueType == enumValueType.vtBool) // 检查是否是真假值，显示值统一换为是和否
            {
                oldValue = DataRecord.TranslateValue(oldValue);
                newValue = DataRecord.TranslateValue(newValue);
            }
            oldValue = oldValue.Replace("'", "''");
            newValue = newValue.Replace("'", "''");

            if (!bMainSaveRow)
            {

                DataTable tbl = SaveRow.Table;
                string sqlrow = tbl.ExtendedProperties["sql"] as string;
                if (Helper.SaveTable(tbl, sqlrow, MainForm.conn) < 1)
                {
                    return false;
                }

                //修改内存显示用的默认值
                /*if (prop.Value is DBCustomClass)
                    prop.DefaultValue = ((DBCustomClass)prop.Value).Value;
                else
                    prop.DefaultValue = prop.Value;
                */
                prop.UpdateDefaultValue();



                // 记录表的值
                DataRecord dataRecord = DataRecord.GetDataRecord();
                dataRecord.Conn = m_conn; // sql连接
                dataRecord.ModelID = m_nModlId; // 模块id
                dataRecord.TabID = m_nModlTabId; // tab页id
                dataRecord.CurrentNode = m_lua["SelectedNode"] as TreeNode; // 树结点
                dataRecord.Time = DateTime.Now; // 当前修改时间
                dataRecord.UserName = Helper.GetHostName(); // 当前用户名
                dataRecord.OldValue = oldValue; // 字段的旧值        
                dataRecord.NewValue = newValue; // 字段的新值
                dataRecord.FieldDisplayName = GetFullDisplayName(prop); // 字段的全路径
                dataRecord.FieldID = int.Parse(prop.Key); // 字段的id
                dataRecord.Remarks = strRemarks;
                dataRecord.Save();
            }
            else
            {
                mRecordProList.Add(prop);
                string[] record = new string[4];
                record[0] = oldValue;
                record[1] = newValue;
                record[2] = GetFullDisplayName(prop);
                record[3] = prop.Key;
                mRecordList.Add(record);
            }

            return true;
        }

        /// <summary>
        /// 获取字段的全路径名
        /// </summary>
        /// <param name="dbClass">当前DBClass</param>
        /// <returns>全路径名</returns>
        private string GetFullDisplayName(DBCustomProperty prop)
        {
            
            string fullDisplayName = prop.Name.Trim(); // 完整的显示名称
            DBCustomClass parent = prop.Parent as DBCustomClass;

            while (parent != null && parent.Property != null && parent.ParentFieldID > 0) // 存在父字段且不为分类
            {
                fullDisplayName = string.Format("{0}.", parent.Property.Name.Trim()) + fullDisplayName;
                parent = parent.Parent as DBCustomClass;
            }

            return fullDisplayName;
        }

        public void SaveScript(CustomProperty prop)
        {
            //如果是从表里面的属性,需要找到正确地从表名
            if (prop != null && prop.Parent is DBCustomClass && prop.Parent != this)
            {
                //((DBCustomClass)(prop.Parent)).SaveScript(prop);
                DBCustomClass cls = GetTopParent(prop);
                if (this != cls)
                {
                    cls.SaveScript(prop);
                    return;
                }
            }
            string strFieldid = prop == null ? "0" : prop.Key;
            string strFieldName = prop == null ? "#" : prop.ID;
            string strMainTable = prop == null ? "" : (prop.Parent as DBCustomClass).MainTable;

            DBProcess.SaveScriptByID(strMainTable, strFieldid, m_nModlTabId.ToString(), m_luacodes[strFieldName].ToString());
            CustomClassCache.DelAllCaches();
#if false

            string ssql = string.Format("SELECT * FROM sys_scriptstore WHERE fieldid={0} AND modtabid = {1} AND tablename='{2}' AND fieldname='{3}'", strFieldid, m_nModlTabId, m_strMainTableName, strFieldName);
            SqlDataAdapter adp = new SqlDataAdapter(ssql, m_conn);
            System.Data.DataSet DS = new System.Data.DataSet();
            adp.Fill(DS);

            //先判断数据库中是否存在记录
            //存在，则更新
            DataTable tbl = DS.Tables[0];
            if (tbl.Rows.Count > 0)
            {
                DataRow row = tbl.Rows[0];
                row["fieldid"] = strFieldid;
                row["modtabid"] = m_nModlTabId;
                row["tablename"] = m_strMainTableName;
                row["fieldname"] = strFieldName;
                row["scriptlanguage"] = "LUA";
                row["scriptcode"] = Helper.GetByteArrayFromString(m_luacodes[strFieldName].ToString());
                //使用SqlCommandBuilder  对像填充SqlDataAdapter 的InsertCommand、DeleteCommand、UpdateCommand对像
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.UpdateCommand = cmdBuilder.GetUpdateCommand();

                int val = adp.Update(tbl);
                tbl.AcceptChanges();
            }
            //不存在，则新建记录
            else
            {
                DataRow row = tbl.NewRow();
                row["fieldid"] = strFieldid;
                row["modtabid"] = m_nModlTabId;
                row["tablename"] = m_strMainTableName;
                row["fieldname"] = strFieldName;
                row["scriptlanguage"] = "LUA";
                row["scriptcode"] = Helper.GetByteArrayFromString(m_luacodes[strFieldName].ToString());
                tbl.Rows.Add(row);
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.InsertCommand = cmdBuilder.GetInsertCommand();

                int val = adp.Update(tbl);
                tbl.AcceptChanges();
            }
#endif
        }

        public string GetFieldStr(System.Data.DataRow row, string strFieldName)
        {
            return row[strFieldName].ToString().Trim().ToLower();
        }

        public string GetFieldStrNoLower(System.Data.DataRow row, string strFieldName)
        {
            return row[strFieldName].ToString().Trim();
        }

        public Hashtable GetListFieldExArray(string strfieldex)
        {
            if (strfieldex.Trim().Length <= 0)
                return null;
            Hashtable hs = new Hashtable();
            string[] strLines = strfieldex.Split(new Char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string strLine in strLines)
            {
                hs[strLine.Trim().ToLower()] = "";
            }
            return hs;
        }

        // 对下拉框中出现的内容进行预处理,将数据库中的字典表值全部读下来预存到hashtable中
        private void FillComboList(string fieldnamekey, System.Data.DataRow row, SqlConnection conn)
        {
            // listtable strliscondition strlistf strlistk strlistfex 
            string strlisttable = GetFieldStr(row, "listtable");
            string strliscondition = row["listcondition"].ToString().Trim();
            string strlistf = GetFieldStr(row, "listfield");
            string strlistk = GetFieldStr(row, "keyfield");
            string strlistfex = GetFieldStr(row, "listexfields");
            string strCacheKey = string.Format("{0},{1},{2},{3},{4}", strlisttable, strliscondition, strlistf,
                                               strlistk, strlistfex);

            Hashtable[] ComboCaches = CustomClassCache.GetComboCaches(strCacheKey, this);            
            m_combox_Keys[fieldnamekey] = ComboCaches[0];
            m_combox_Values[fieldnamekey] = ComboCaches[1];
            m_combox_ExValues[fieldnamekey] = ComboCaches[2];
        }

        // 对下拉框中出现的内容进行预处理,将数据库中的手工输入的字典表值全部读下来预存到hashtable中
        private void FillTextComboList(string fieldnamekey, DataRow row)
        {
            string strlistvalues = row["listvalues"].ToString().Trim();
            string[] strLines =  strlistvalues.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            object[] strdescs = (object[])m_colName[fieldnamekey];
            string strfieldname = strdescs[8].ToString();

            Hashtable comhashkeys = new Hashtable();
            Hashtable comhashvalues = new Hashtable();

            if (strLines == null)
            {
                MessageBox.Show("该字段 " + fieldnamekey + " 选择为<文本下拉框>, 但是没有正确的设置.", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return; 
            }

            foreach (string line in strLines)
            {
                string[] strkv = line.Split(new Char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                if (strkv != null && strkv.Length > 1)
                {
                    string key = strkv[0];
                    string value = strkv[1];
                    comhashkeys[value] = key;
                    comhashvalues[key] = value;
                }
                else
                {
                    string s = string.Format("在读取 {0} 字段 ListValues 内容时出现错误，请修改元信息描述数据表。", strfieldname);
                    System.Windows.Forms.MessageBox.Show(s);
                    throw new Exception("错误的ListValues");
                }
            }

            m_combox_Keys[fieldnamekey] = comhashkeys;
            m_combox_Values[fieldnamekey] = comhashvalues;
            m_combox_ExValues[fieldnamekey] = null;
        }

        public int GetRealPropertySize()
        {
            int nCount = 0;
            CustomProperty prop = null;
            for (int i = 0; i < this.Count; i++)
            {
                prop = (CustomProperty)this[i];
                if (prop.Visible == true)
                {
                    if (prop.Value is DBCustomClass)
                    {
                        DBCustomClass cls = (DBCustomClass)prop.Value;
                        //if ((subcls.ParentFieldType == 1 || subcls.ParentFieldType == 0) && subcls.RecordCount == 1)
                        if (prop.IsCanHideNode && (Count == 1 || cls.Count == 1))
                        {
                            nCount += cls.GetRealPropertySize();
                            continue;
                        }
                        
                    }
                    nCount++;
                }
            }
            return nCount;
        }
        private Attribute[] ToAttribute(ArrayList attrs)
        {
            return (System.Attribute[])attrs.ToArray(typeof(Attribute));
        }
        public new PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            CustomProperty prop = null;
            CustomProperty subProp = null;
            int nPropCount = GetRealPropertySize();
            //PropertyDescriptor[] newProps = new PropertyDescriptor[this.Count];
            PropertyDescriptor[] newProps = new PropertyDescriptor[nPropCount];
            int nCount = 0;

            try
            {
                for (int i = 0; i < this.Count; i++)
                {
                    prop = (CustomProperty)this[i];
                    if (prop.Visible == true)
                    {
                        if (prop.Value is DBCustomClass)
                        {
                            DBCustomClass cls = (DBCustomClass)prop.Value;
                            //if ((cls.IsVirtualField && cls.IsPrimaryKeysSameAsDB && cls.RecordCount > 0) || cls.RecordCount == 1) //发现只会有唯一匹配的记录，隐藏树结点，并把其下的属性全部提上来一层
                            //if ((cls.IsVirtualField && cls.IsPrimaryKeysSameAsDB) || cls.RecordCount == 1) //发现只会有唯一匹配的记录，隐藏树结点，并把其下的属性全部提上来一层
                            //ParentFieldType == 0 表示自动生成的普通字段下一对多的的记录, 1表示虚拟字段，下面是从表各字段
                            //if ((cls.ParentFieldType == 1 || cls.ParentFieldType == 0) && cls.RecordCount == 1) //只有虚拟字段及１对多才可以将记录提上一层
                            if (prop.IsCanHideNode && (Count == 1 || cls.Count == 1)) //
                            {
                                for (int j = 0; j < cls.Count; j ++ )
                                {
                                    subProp = cls[j];
                                    if (subProp.Visible)
                                    {

                                        //if (this.SortType == 1)
                                        //    subProp.Name = string.Format("[{0}] {1}", prop.Name, subProp.Name);
                                        if (subProp.Value is DBCustomClass && (subProp.Value as DBCustomClass).ParentFieldType == 2)
                                        {
                                            ArrayList attrlist = new ArrayList();
                                            attrlist.AddRange(attributes);
                                            attrlist.Add(new EditorAttribute(typeof(CustomEditor), typeof(System.Drawing.Design.UITypeEditor))); //动态指定编辑器
                                            Attribute[] attrArray = ToAttribute(attrlist);// (System.Attribute[])attrs.ToArray(typeof(Attribute));
                                            if (subProp.DefaultValue == null)
                                                subProp.DefaultValue = subProp.Value;
                                            newProps[nCount] = new CustomPropertyDescriptor(ref subProp, attrArray);
                                        }
                                        else
                                        {
                                            if (subProp.DefaultValue == null)
                                                subProp.DefaultValue = subProp.Value;
                                            newProps[nCount] = new CustomPropertyDescriptor(ref subProp, attributes);
                                        }
                                        nCount++;
                                    }
                                }
                                continue;
                            }
                            else
                            {
                                ArrayList attrlist = new ArrayList();
                                attrlist.AddRange(attributes);
                                if ((cls.IsVirtualField || cls.ParentFieldType != 2) && Parent != null) //只有非顶层虚拟字段才禁止新建动作
                                    attrlist.Add(new EditorAttribute(typeof(TextEditor), typeof(System.Drawing.Design.UITypeEditor)));
                                else
                                {
                                    attrlist.Add(new EditorAttribute(typeof(CustomEditor), typeof(System.Drawing.Design.UITypeEditor)));
                                    //attrlist.Add(new DefaultValueAttribute(typeof(int), prop.Value.ToString()));
                                }
                                Attribute[] attrArray = ToAttribute(attrlist);// (System.Attribute[])attrs.ToArray(typeof(Attribute));
                                if (prop.DefaultValue == null)
                                    prop.DefaultValue = prop.Value;
                                newProps[nCount] = new CustomPropertyDescriptor(ref prop, attrArray);
                                nCount++;
                                continue;
                            }
                        }
                        /*
                        else
                        {
                            ArrayList attrlist = new ArrayList();
                            attrlist.AddRange(attributes);
                            attrlist.Add(new DefaultValueAttribute(prop.Value));
                            Attribute[] attrArray = ToAttribute(attrlist);// (System.Attribute[])attrs.ToArray(typeof(Attribute));
                            newProps[nCount] = new CustomPropertyDescriptor(ref prop, attrArray);
                            nCount++;
                            continue;
                        }*/
                        if (prop.DefaultValue == null)
                            prop.DefaultValue = prop.Value;
                        newProps[nCount] = new CustomPropertyDescriptor(ref prop, attributes);
                        nCount++;
                    }
                    else
                    {
                        if (prop.DefaultValue == null)
                            prop.DefaultValue = prop.Value;
                    }
                }
            }
            catch (Exception ex)
            {
                string s = string.Format("在读取 {0} 字段时出现错误，请修改元信息描述数据表。\r\n错误讯息为:{1}", prop.ID, ex.Message);
                System.Windows.Forms.MessageBox.Show(s);
            }

            return new PropertyDescriptorCollection(newProps);
        }


        #region "脚本实现相关"
        //获取当前表关联的ＬＵＡ代码
        public Hashtable ScriptCode
        {
            get { return m_luacodes; }
        }
        public Hashtable ScriptExecState
        {
            get { return m_scriptExecState; }
        }

        //找到有效的父结点,用于1对多时,每条记录的脚本存储位置应该是属于虚拟字段结点, 而不是以字段名为名的结点中
        private DBCustomClass _GetTopParent(DBCustomClass cls)
        {
            if (cls.IsVirtualField) return cls;
            if (cls.Parent is DBCustomClass)
            {
                return _GetTopParent((DBCustomClass)cls.Parent);
            }
            else
            {
                return null;
            }
        }
        private DBCustomClass GetTopParent(object prop)
        {
            DBCustomClass retcls = _GetTopParent((DBCustomClass)((CustomProperty)(prop)).Parent);
            if (retcls == null)
                retcls = this;
            return retcls;
        }
        public void  SetScriptCode(CustomProperty prop, string strCode)
        {
            string strfnID;
            if (prop == null)
                strfnID = "#";
            else
                strfnID = prop.ID;

            DBCustomClass cls = null;
            if (strfnID == "#")
            {
                cls = this;
            }
            else if (prop.Parent is DBCustomClass)
            {
                cls = GetTopParent(prop);
                //if (prop.Parent != this)
                //    cls = ((DBCustomClass)(prop.Parent));
                //else
                //    cls = this;
            }

            cls.ScriptCode[strfnID] = strCode;
            cls.ScriptExecState[strfnID] = false;
        }
        public string GetScriptCode(CustomPropertyDescriptor desc)
        {
            string strfnID;
            if (desc == null)
                strfnID = "#";
            else
                strfnID = desc.Property.ID;

            DBCustomClass cls;
            if (strfnID == "#")
            {
                cls = this;
            }
            else if (desc.Property.Parent is DBCustomClass)
            {
                //cls = ((DBCustomClass)(desc.Property.Parent));
                cls = GetTopParent(desc.Property);
            }
            else
                return "";
            
            object ocode = cls.ScriptCode[strfnID];
            if (ocode == null)
                return "";
            else
                return ocode.ToString();
        }

        private object OnGetTreeValueEvent(CustomClass sender, TreeValueEventArgs e)
        {
            CustomClass parent = sender;
            CustomProperty prop = e.tree;
            object svalue = null;
            try
            {
                if (m_lua != null && prop.ID != null)
                {
                    this.DoScript(prop);
                    LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, "ongettreevalue"));//"__fname__" + prop.Key + ".ongetreevalue");
                    if (fun != null)
                    {
                        CustomClass child = (CustomClass)e.tree.Value;
                        svalue = child.Value;//e.tree.Description;
                        object[] retobjs = fun.Call(prop, e);
                        if (retobjs != null && retobjs.GetLength(0) > 0)
                        {
                            if (retobjs[0] is bool)
                            {
                                if ((bool)retobjs[0] == true && retobjs.GetLength(0) > 1)
                                    return retobjs[1];
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
            }
            return svalue;//取消修改值，或者未更改值
        }

        private object OnCustomEditorEditValueEvent(CustomProperty sender, CustomEditorEventArgs e)
        {
            CustomProperty prop = sender;

            string svalue;
            if (e.value is DBCustomClass)
                svalue = ((DBCustomClass)e.value).Value.ToString();
            else
                svalue = e.value.ToString();
            
            try
            {
                if (m_lua != null)
                {
                    DoScript(prop);
                    
                    //如果是树结点，需要额外的增加事件，以支持新建从表记录
                    if (prop.Value is DBCustomClass)
                    {
                        LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, "neednewrecord"));//"__fname__" + prop.Key + ".oneditvalue");
                        if (fun != null)
                        {
                            object[] retobjs = fun.Call(prop, e);
                            if (retobjs != null && retobjs.GetLength(0) > 0)
                            {
                                if (retobjs[0] is bool && (bool)retobjs[0])
                                {
                                    NewRecord(prop);
                                }
                            }
                        }
                    }
                    //else
                    {
                        LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, "oneditvalue"));//"__fname__" + prop.Key + ".oneditvalue");
                        if (fun != null)
                        {
                            object[] retobjs = fun.Call(prop, e);
                            if (retobjs != null && retobjs.GetLength(0) > 0)
                            {
                                if (retobjs[0] is bool)
                                {
                                    if ((bool)retobjs[0] == true)
                                    {
                                        if (retobjs.GetLength(0) == 2)
                                        {
                                            //prop.DisplayValue = null;
                                            return retobjs[1];
                                        }
                                        else if (retobjs.GetLength(0) > 2)
                                        {
                                            //prop.DisplayValue = retobjs[2];
                                            //prop.IsClearDisplayValue = false;
                                            return retobjs[1];
                                        }
                                    }
                                }
                            }
                            return e.value;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
            }
            return e.value;//取消修改值，或者未更改值
        }
        public void NewRecord(CustomProperty prop)
        {
            if (prop.Value is DBCustomClass)
            {
                string svalue = ((DBCustomClass)prop.Value).Value.ToString();
                NewRowHelper helper = new NewRowHelper(prop);
                if (MessageBox.Show(string.Format("您确定要在[{0}]表中新建记录吗？", helper.NewRowTableName), "确认",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button2) == DialogResult.Yes)
                {
                    helper.GernateDefaultValue();
                    LuaFunction fun1 = m_lua.GetFunction(GetRealFunctionName(prop.Key, "onnewrecord"));//"__fname__" + prop.Key + ".oneditvalue");
                    DBCustomClass cls = prop.Value as DBCustomClass;
                    if (fun1 != null)
                    {
                        //object[] retobjs = fun1.Call(prop, svalue, helper.KeyValues, helper.SubProperty.ID); //需要传入从表主键Hash, key=字段名, value=空，让函数将hashtable填充好
                        object[] retobjs = fun1.Call(prop, svalue, helper.NewRowFieldValues, helper.SubProperty.ID); //需要传入从表主键Hash, key=字段名, value=空，让函数将hashtable填充好
                        
                        //如果返回真，就可以新建从表记录了
                        if (retobjs != null && retobjs.GetLength(0) > 0)
                        {
                            if (retobjs[0] is bool)
                            {
                                if ((bool)retobjs[0] == true)
                                {
                                    helper.NewRow(); //生成新记录并提交
                                    //prop.SetValue(helper.GetNewValue(prop)); //此函数强行刷新下属结点
                                    //OnAsynNotifyEvent(2, prop);
                                    helper.SetNewValue(prop);
                                }
                            }
                        }
                        //return e.value;
                    }
                    else //直接新建记录
                    {
                        helper.NewRow(); //生成新记录并提交
                        //prop.SetValue(helper.GetNewValue(prop)); //此函数强行刷新下属结点
                        //if (prop.Value is DBCustomClass && (prop.Value as DBCustomClass).ParentFieldType == 1) //为虚拟字段上新建从表记录时，需要 getDynamicclass
                        //OnAsynNotifyEvent(2, prop);
                        helper.SetNewValue(prop);
                    }

                    LuaFunction fun2 = m_lua.GetFunction(GetRealFunctionName(prop.Key, "afternewrecord"));
                    if (fun2 != null)
                    {
                        object[] retobjs = fun2.Call();
                    }
					else
					{
	                    LuaFunction fun3 = m_lua.GetFunction(GetRealFunctionName(prop.Key, "onafternewrecord"));
    	                if (fun3 != null)
        	            {
            	            object[] retobjs = fun3.Call();
                	    }					
					}
                }
            }
        }

        private void BeforePropertyChangedEvent(CustomProperty sender, PropertyChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                DBCustomProperty prop = (DBCustomProperty)sender;
                bool bRet = false;
                string sErrMsg = "未定义的错误";
                try
                {
                    if (m_lua != null)
                    {
                        DoScript(prop);//可能会有重名的字段，因不同表字段现在可以在同一TAB页中，需要加上parentfield.
                        LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, "onbeforevaluechang"));//"__fname__" + prop.Key + ".onbeforevaluechang");
                        if (fun != null)
                        {
                            object[] retobjs = fun.Call(prop, e);
                            if (retobjs != null && retobjs.GetLength(0) > 0)
                            {
                                if (retobjs[0] is bool)
                                {
                                    bRet = (bool)retobjs[0];
                                    if (!bRet)
                                        sErrMsg = (string)retobjs[1];
                                }
                            }
                        }
                        else
                            bRet = true; //无函数表示没有脚本指定
                    }
                }
                catch (Exception ex)
                {
                    //ScriptDebugForm frm = Program.MainForm.DebugForm;
                    //frm.OutputBox.Text += ex.Message + "\r\n";
                    //frm.Show();
                    //frm.BringToFront();
                    Helper.AddLog(ex.Message);
                    sErrMsg = "LUA运行时错误:\r\n" + ex.Message;
                }
                if (!bRet)
                {
                    sErrMsg = sErrMsg.Replace("\\n", "\n");
                    sErrMsg = sErrMsg.Replace("\\t", "\t");
                    throw new Exception(sErrMsg);
                }
                else
                {
                    //尝试先锁定再提交修改
                    object[] findkeys = FindKeys == null ? (object[])prop.Tag : FindKeys;

                    // 加入限制：只能在<默认值>结点中修改挂接的子表的默认行的内容
                    bool bModifyingSubTableDefaults = true;
                    foreach (object _key in findkeys)
                    {
                        if (_key.ToString() != "0")
                        {
                            bModifyingSubTableDefaults = false;
                            break;
                        }
                    }
                    if (bModifyingSubTableDefaults)
                    {
                        if (!Program.MainForm.m_CurBaseForm.BaseTree.SelectedNode.Text.Contains("默认值"))
                            throw new Exception("不能在这里直接修改子表默认值，请先选择<默认值>结点，再进行修改。");
                        else
                        { 
                            if (!CanModifyDefaultVal())
                                throw new Exception("不允许修改默认值。");
                        }
                    }

                    if (!OnLockRecord(true, prop, findkeys))
                    {
                        throw new Exception(string.Format("这条记录已经被[{0}]在模块[{1}]中锁定，请等待解锁后，再进行修改操作。", 
                            m_reclock.GetServerLastLockUser(findkeys), m_reclock.GetServerLastLockModID(findkeys)));
                    }
                }
            }
        }
        private void AfterPropertyChangedEvent(CustomProperty sender, PropertyAfterChangeEventArgs e)
        {
            if (sender is CustomProperty)
            {
                CustomProperty prop = (CustomProperty)sender;

                string svalue = e.value.ToString();
                try
                {
                    if (m_lua != null)
                    {
                        DoScript(prop);
                        LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, "onaftervaluechanged"));//"__fname__" + prop.Key + ".onaftervaluechanged");
                        if (fun != null)
                            fun.Call(prop, e);
                    }
                }
                catch (Exception ex)
                {
                    //ScriptDebugForm frm = Program.MainForm.DebugForm;
                    //frm.OutputBox.Text += ex.Message + "\r\n";
                    //frm.Show();
                    //frm.BringToFront();
                    Helper.AddLog(ex.Message);
                }
                //GridItem item = propertyGrid1.SelectedGridItem;
                //CustomPropertyDescriptor desc = (CustomPropertyDescriptor)item.PropertyDescriptor;
                //CustomProperty specprop = desc.Property;          
            }
        }
        private string GetRealFunctionName(string strkey, string strfunctionname)
        {
            string strRet;
            switch (strkey)
            {
                case null:
                    goto case "##";
                case "##": //全局环境,暂未实现
                    strRet = strfunctionname;
                    break;
                case "#": //TAB环境
                    strRet = string.Format("__fname__tab_global_{0}.{1}", ModalTabId, strfunctionname);
                    break;
                default:
                    strRet = string.Format("__fname__{0}.{1}", strkey, strfunctionname);
                    break;
            }
            return strRet;
        }
        private string GetPrefixCode(string strkey) //获得切换环境的代码
        {
            string strcodeprefix = "";
            //切换到私有环境
            if (m_lua != null)
            {
                /*if (strkey == "##")
                    strcodeprefix = "_ChgEnv()\r\n"; //全局环境
                
                else
                    strcodeprefix = string.Format("_ChgEnv(\"{0}\")\r\n", strkey);
                */
                switch (strkey)
                {
                    case "##": //全局环境,暂未实现
                        strcodeprefix = "_ChgEnv()\r\n";
                        strcodeprefix = string.Format("_ChgEnv('tab_global_{0}')\r\n", ModalTabId); // kuangsihao实现的模块全局
                        break;
                    case "#": //TAB环境
                        strcodeprefix = string.Format("_ChgEnv('tab_global_{0}')\r\n", ModalTabId);
                        break;
                    default:
                        strcodeprefix = string.Format("_ChgEnv('{0}', 'tab_global_{1}')\r\n", strkey, ModalTabId);
                        break;
                }
            }
            return strcodeprefix;
        }
        //注意本函数只能找到当前层的字段相关代码,不能在本类之外调用
        public void DoScript(string strkey, string strFieldName)
        {
            try
            {
                //string strcodeprefix = GetPrefixCode(strkey);
                m_lua.DoString(GetPrefixCode(strkey));//不可少,因需要创建Pacage才能作后续的函数是否存在的判断
                //m_lua.GetFunction("_ChgEnv").Call(strFieldName == "#" ? "" : strFieldName); //错误,不能直接调用.call,这样会导致setfenv(2,xx)失效

                object fn = m_scriptExecState[strFieldName];
                if (fn != null)
                {
                    bool bExecuted = (bool)fn;
                    if (bExecuted)
                        return;
                }
                else
                {
                    //return;//没有脚本内容，放弃执行
                    m_scriptExecState[strFieldName] = false;
                }
                if (ScriptCode[strFieldName] == null)
                    return;
                string strcode = ScriptCode[strFieldName].ToString();
                if (m_lua != null && strcode != null && strcode.Length > 0)
                {
                    string strcodeprefix = GetPrefixCode(strkey);
                    m_lua.DoString(strcodeprefix);
                    m_lua.DoString(strcodeprefix + strcode);
                    m_scriptExecState[strFieldName] = true;
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
            }
        }
        
        public void DoScript(CustomProperty prop)
        {   
            if (prop == null)
            {
                this.DoScript("##", "##");
                this.DoScript("#", "#");
            }
            else if (prop.Parent is DBCustomClass)
            {
                //((DBCustomClass)(prop.Parent)).DoScript(prop.Key, prop.ID);
                DBCustomClass cls = GetTopParent(prop);
                cls.DoScript(prop.Key, prop.ID);
            }
        }

        public bool ExistLuaFunction(string strfuncName)
        {
            bool bRet = false;
            if (m_lua != null)
            {
                m_lua.DoString(GetPrefixCode("#"));
                LuaFunction fun = m_lua.GetFunction(GetRealFunctionName("#", strfuncName));
                if (fun != null)
                    bRet = true;
            }
            return bRet;
        }
        //查看某属性中是否有函数,没有就到全局中找,找到就执行
        public object[] CallLuaFunction(CustomProperty prop, string strfuncName, object[] args)
        {            
            if (m_lua != null)
            {
                m_lua.DoString(prop == null ? GetPrefixCode("#") : GetPrefixCode(prop.Key));
                LuaFunction fun = m_lua.GetFunction(GetRealFunctionName(prop.Key, strfuncName));//"__fname__" + prop.Key + "." + strfuncName);
                if (fun != null)
                {
                    object[] retobjs = fun.Call(args);
                    return retobjs;
                }
            }
            return null;
        }

        public object[] CallLuaFunction(string strfuncName)
        {
            object[] ret = null;
            try
            {
                if (m_lua != null)
                {
                    m_lua.DoString(GetPrefixCode("#"));
                    LuaFunction fun = m_lua.GetFunction(GetRealFunctionName("#", strfuncName));
                    if (fun != null)
                        ret = fun.Call(null);
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
            }
            return ret;
        }

        public object[] CallLuaFunction(string strfuncName, object[] args)
        {
            object[] ret = null;
            try
            {
                if (m_lua != null)
                {
                    m_lua.DoString(GetPrefixCode("#"));
                    LuaFunction fun = m_lua.GetFunction(GetRealFunctionName("#", strfuncName));
                    if (fun != null)
                        ret = fun.Call(args);
                }
            }
            catch (Exception ex)
            {
                //ScriptDebugForm frm = Program.MainForm.DebugForm;
                //frm.OutputBox.Text += ex.Message + "\r\n";
                //frm.Show();
                //frm.BringToFront();
                Helper.AddLog(ex.Message);
            }
            return ret;
        }

        #endregion
        public object GetFieldValue(string strFieldName)
        {
            DataRow row = m_tbl_MainData.Rows.Find(PreProcessKeys(m_findkeyvalues));
            return row[strFieldName];
        }

        //当有其他页的某属性被修改时，会通知到每个TAB页，需要更新相同表相同字段的属性值
        public int ModifyValue(string strTable, string strField, object newValue, string findkeyvalues)
        {
            int nSameFieldCount = 0;
            //如果是主表的主键字段，需要刷新findkeys
            //if (this.MainTable == strTable)
            //    ModifyFindKeys(strField, newValue);

            if (this.IsVirtualField == false &&
                this.Property != null && this.ID == strField && this.MainTable == strTable &&
                newValue.ToString() != this.Value.ToString())
            {
                string thisfindkeyvalues = Helper.StringArrayToString(this.FindKeys);
                if (thisfindkeyvalues == findkeyvalues)
                {
                    this.Property.SetValue(newValue);
                    ReloadDataOnSave = true;
                    nSameFieldCount ++;
                }
            }

            try
            {
                foreach (CustomProperty prop in this)
                {
                    if (prop.ValueType == enumValueType.vtExpandNode && prop.Value is DBCustomClass)
                    {
                        DBCustomClass cls = ((DBCustomClass)prop.Value);
                        nSameFieldCount += cls.ModifyValue(strTable, strField, newValue, findkeyvalues);
                        if (cls.IsVirtualField == false &&
                            cls.Property != null && cls.ID == strField && cls.MainTable == strTable && 
                            newValue.ToString() != cls.Value.ToString())
                        {
                            string thisfindkeyvalues = Helper.StringArrayToString(cls.FindKeys);
                            if (thisfindkeyvalues == findkeyvalues)
                            {
                                cls.Property.SetValue(newValue);
                                cls.ReloadDataOnSave = true;
                                nSameFieldCount++;
                            }
                        }
                    }
                    else if (prop.ID == strField && MainTable == strTable)
                    {
                        string thisfindkeyvalues = Helper.StringArrayToString(this.FindKeys);
                        if (thisfindkeyvalues == findkeyvalues)
                        {
                            if (prop.ValueType == enumValueType.vtCombo)
                            {
                                if (prop.ComboKeys[newValue.ToString()] == null)
                                {
                                    CustomClassCache.DelComboCache();
                                    ReInitMetaInfo();
                                    prop.ComboKeys = (Hashtable)(m_combox_Values[prop.ID]);
                                }
                                prop.SetValue(prop.ComboKeys[newValue.ToString()]);
                            }
                            else
                            {
                                prop.SetValue(newValue);
                            }
                            ReloadDataOnSave = true;
                            nSameFieldCount++;
                        }
                    }
                    //if (prop.ID == strField && MainTable == strTable)
                    //{
                    //    ReloadDataOnSave = true;
                    //}
                }
            }
            catch (Exception ex)
            {
                Helper.AddLog(ex.Message);
            }
            return nSameFieldCount;
        }
        public bool OnLockRecord(bool bLock, DBCustomProperty prop, object[] findkeyvalues)
        {   
            RecordLock Lock;
            object[] keyvalue;
            int nLastServerVersion;
            if (prop == null || prop.Lock == null)
            {
                Lock = m_reclock;
                keyvalue = findkeyvalues;
                nLastServerVersion = m_reclock.LocalVersion;// m_nLastServerVersion;
            }
            else if (prop.Lock != null && prop.Tag != null)
            {
                Lock = prop.Lock;
                keyvalue = prop.Tag as object[];
                m_nRecordModifyCount = 0;
                nLastServerVersion = prop.LastVersion;
            }
            else
            {
                throw new Exception("系统内部错误，OnLockRecord 失败，请联系编辑器组相关同事。");
            }
      
            if (bLock)
            {
                if (m_nRecordModifyCount == 0 && (!Lock.Locked))// || Lock.UnLock(keyvalue)))
                {
                    if (Lock.Lock(keyvalue))
                    {
                        m_nRecordModifyCount++;
                        int serverver = Lock.GetServerLastVersion(keyvalue);
                        if (serverver != nLastServerVersion && serverver != 0)
                        {
                            MessageBox.Show("由于服务端数据已经更新，您当前修改的内容会被修正为服务端的内容。", "警告",
                                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            //需要刷新当前数据才能开始编辑
                            //ReloadCurrentInfo();
                            OnAsynNotifyEvent(4, null);
                        }
                    }
                    else
                        return false;
                }
            }
            else //解锁
            {
                //先将从表中的锁全解了
                if (prop == null)
                {
                    foreach (DBCustomProperty subprop in this)
                    {
                        if (subprop.ValueType == enumValueType.vtExpandNode && subprop.Value is DBCustomClass)
                        {
                            DBCustomClass cls = ((DBCustomClass)subprop.Value);

                            if (!cls.ValueChanged) // 如果值没变的话就必要较进行重复的解锁操作 add by suntao
                            {
                                cls.OnLockRecord(bLock, null, cls.FindKeys);
                            }                            
                        }
                        else if (subprop.Tag != null && subprop.Tag is object[])
                        {
                            OnLockRecord(bLock, subprop, null);
                        }
                        //subprop.UpdateLastVersion();
                    }
                }
                if (Lock.Locked)
                {
                    if (Lock.UnLock(keyvalue) > 0)
                    {
                        m_nRecordModifyCount = 0;
                        if (prop != null && prop.Lock != null && prop.Tag != null)
                            prop.UpdateLastVersion();
                        //nLastServerVersion = Lock.GetServerLastVersion(keyvalue);//放在模块级,一次全更新,否则各TAB间的版本会不一致
                    }
                    else
                        return false;
                }
            }
            /*if (m_nRecordModifyCount > 0)
            {
                //解锁记录
            }
            m_nRecordModifyCount = 0;
             */
            return true;
        }
        public void RefreshLastVersion()
        {
            if (m_findkeyvalues != null)
            {
                m_nLastServerVersion = m_reclock.GetServerLastVersion(m_findkeyvalues);
            }

            foreach (CustomProperty prop in this)
            {
                DBCustomClass dbCustomClass = prop.Value as DBCustomClass;

                if (prop.ValueType == enumValueType.vtExpandNode && dbCustomClass != null && dbCustomClass.ValueChanged)
                {
                    dbCustomClass.RefreshLastVersion();                    
                }
            }
        }
        private bool CanModifyDefaultVal()
        {
            bool bResult = false;
            string strUserID = Helper.GetHostName();
            string strIP = Helper.GetIPAddress();
            string strSQL = string.Format("select _index from sys_useracl where userid ='{0}' and ip='{1}'", strUserID, strIP);

            //DataTable dtab = Helper.GetDataTable(strSQL, Conn);
            DataTable dtab = Helper.GetDataTableWithSqlProxy("sys_useracl", strSQL, Conn);
            
            if (dtab.Rows.Count <= 0)
                return true;

            string strIndex = dtab.Rows[0][0].ToString();

            strSQL = string.Format("select canmodifydefaultval from sys_useracl_desc where userindex={0} and moduleid={1}", strIndex, Convert.ToString(m_nModlId));
            //dtab = Helper.GetDataTable(strSQL, Conn);
            dtab = Helper.GetDataTableWithSqlProxy("sys_useracl_desc", strSQL, Conn);
            string strCanModifyDefaultVal = "";
            if (dtab.Rows.Count != 0)
            {
                strCanModifyDefaultVal = dtab.Rows[0][0].ToString();
            }
            if (strCanModifyDefaultVal == "False" || strCanModifyDefaultVal == "" || strCanModifyDefaultVal == "0")
                return bResult;
            bResult = true;
            return bResult;
        }
    }
}
