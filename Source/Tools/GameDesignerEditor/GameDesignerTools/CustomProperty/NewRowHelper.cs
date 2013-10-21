using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    class NewRowKeyValueHelper
    {
        private Hashtable m_ht;

        public NewRowKeyValueHelper(Hashtable ht)
        {
            m_ht = ht;
        }
        public void SetValue(string key, object value)
        {
            this[key] = value;
        }
        public int Count
        {
            get { return m_ht.Count; }
        }
        
        public object this[string key]
        {
            get { 
                if (m_ht.Contains(key))
                    return m_ht[key]; 
                else
                {
                    Helper.AddLog(string.Format("找不到指定键值[{0}]相关的内容", key));
                    return null;
                }
            }
            set {
                if (m_ht.Contains(key))
                    m_ht[key] = value; 
                else
                {
                    Helper.AddLog(string.Format("找不到指定键值[{0}]相关的内容", key));                    
                }
            }
        }
    };

    //新建从表记录的帮助类
    class NewRowHelper
    {
        //CustomProperty m_virtualprop; //实际新建记录的虚拟字段
        DBCustomClass m_dbclass;        //实际新建记录的dbclass
        CustomProperty m_opprop;        //实际操作的Property
        object m_key;
        object[] m_findkeyvalues;
        Hashtable m_keyvalue = new Hashtable(); //记录新建从表记录的主键名(key)及主键值(value)的 hashtable
        Hashtable m_htnewrow = new Hashtable(); //记录新建从表记录的所有字段名(key)与字段内容(value)
        Hashtable m_newvalue = new Hashtable(); //用于刷新主表字段的判断，记录主从表记录内容：key=(主表主键字段名)及value=(从表主键字段名)的 hashtable
        NewRowKeyValueHelper m_keyvaluehelper;
        NewRowKeyValueHelper m_newrowkeyvaluehelper;

        public CustomProperty SubProperty //表示最终进行操作的DBClass的容器属性
        {
            get { return m_opprop; }
        }

        public NewRowKeyValueHelper KeyValues
        {
            get { return m_keyvaluehelper; }
        }
        public NewRowKeyValueHelper NewRowFieldValues
        {
            get { return m_newrowkeyvaluehelper; }
        }

        public NewRowHelper(DBCustomClass cls)
        {
            Init(null, cls);
        }
        public NewRowHelper(CustomProperty prop)
        {
            if (prop.Value is DBCustomClass)
            {
                Init(prop, prop.Value as DBCustomClass);
            }
        }
        private void Init(CustomProperty prop, DBCustomClass cls)
        {
            m_keyvaluehelper = new NewRowKeyValueHelper(m_keyvalue);
            m_newrowkeyvaluehelper = new NewRowKeyValueHelper(m_htnewrow); //用于在 onnewrecord事件中让用户可以有机会更改新记录的内容
            
            DBCustomClass dbclass = cls;
            if (dbclass.ParentFieldType == 2) //如果是真实字段，要找到虚拟字段
            {
                NewRecordForm frm = new NewRecordForm();
                ArrayList arr = new ArrayList();
                foreach (CustomProperty subprop in dbclass)
                {
                    if (subprop.ValueType == enumValueType.vtExpandNode && subprop.Value is DBCustomClass)
                    {
                        DBCustomClass subdbclass = subprop.Value as DBCustomClass;
                        if (subdbclass.ParentFieldType != 2)
                        {
                            //m_key = subprop.Key;
                            //m_dbclass = subprop.Value as DBCustomClass;
                            int nAddIdx = frm.listBox1.Items.Add(subprop.Name);
                            arr.Add(subprop);
                            //break;
                        }
                    }
                }
                if (frm.listBox1.Items.Count > 1)
                    frm.listBox1.SelectedIndex = 0;
                if (frm.listBox1.Items.Count == 1 || (frm.listBox1.Items.Count > 1 && frm.listBox1.SelectedIndex >= 0 && frm.ShowDialog() == System.Windows.Forms.DialogResult.OK))
                {
                    CustomProperty subprop = (frm.listBox1.Items.Count == 1 ? arr[0] : arr[frm.listBox1.SelectedIndex]) as CustomProperty;
                    m_key = subprop.Key;
                    m_dbclass = subprop.Value as DBCustomClass;
                    m_opprop = subprop;
                }
                else
                    throw new Exception("新建动作被取消。");

            }
            else
            {
                m_opprop = prop;
                //m_key = null;//暂时置空
                m_key = prop.Key;
                m_dbclass = cls;
            }
            if (m_dbclass == null || m_key == null)
                throw new Exception("不能在此字段中新建子表记录。");

            m_findkeyvalues = new object[m_dbclass.DBPrimaryKey.Length];
            for (int i = 0; i < m_findkeyvalues.Length; i++)
            {
                m_findkeyvalues[i] = null;
            }
            foreach (string strKeyField in m_dbclass.DBPrimaryKey)
            {
                m_keyvalue[strKeyField] = null;
            }
        }

        public string NewRowTableName
        {
            get
            {
                if (m_dbclass != null)
                {
                    return m_dbclass.MainTable;
                }
                return null;
            }
        }

        //从ID管理表中获取最后一个ID号，如果没有此字段的ID管理，则新增一条记录
        int GetNextID(string strfieldname)
        {
            string sql = string.Format("SELECT * FROM sys_id_pool WHERE tablename='{0}' and fieldname='{1}'", m_dbclass.MainTable, strfieldname);
            //DataTable tbl = Helper.GetDataTable(sql, MainForm.conn);
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter(sql, MainForm.conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
             * */
            string filter = string.Format("tablename='{0}' and fieldname='{1}'", m_dbclass.MainTable, strfieldname);
            DataTable tbl = Helper.GetDataTableProxy("sys_id_pool", filter, null, MainForm.conn);

            int nLastID = 0;
            DataRow row;
            try
            {
                if (tbl.Rows.Count > 0)
                {
                    row = tbl.Rows[0];
                    nLastID = (int)row["lastid"];
                }
            }
            catch//(Exception ex)
            {
            }
            tbl.Dispose();
            return nLastID + 1;
        }

        void SetLastID(string strfieldname, int nLastID)
        {
            string sql = string.Format("SELECT * FROM sys_id_pool WHERE tablename='{0}' and fieldname='{1}'", m_dbclass.MainTable, strfieldname);
            //DataTable tbl = Helper.GetDataTable(sql, MainForm.conn);
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter(sql, MainForm.conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string filter = string.Format("tablename='{0}' and fieldname='{1}'", m_dbclass.MainTable, strfieldname);
            DataTable tbl = Helper.GetDataTableProxy("sys_id_pool", filter, null, MainForm.conn);

            DataRow row;
            try
            {
                if (tbl.Rows.Count <= 0)
                {
                    //新增一条记录
                    row = tbl.NewRow();
                    row.BeginEdit();
                    row["tablename"] = m_dbclass.MainTable;
                    row["fieldname"] = strfieldname;
                    row["lastid"] = nLastID;
                    row.EndEdit();
                    tbl.Rows.Add(row);
                    /* remoting
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                     */
                }
                else
                {
                    row = tbl.Rows[0];
                    row["lastid"] = nLastID;
                    /* remoting
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                     */
                }

                //int val = adp.Update(tbl);
                //row.AcceptChanges();

                int val = Helper.SaveTable(tbl, sql, MainForm.conn);

                tbl.AcceptChanges();
            }
            catch//(Exception ex)
            {
                tbl.RejectChanges();
            }
            tbl.Dispose();
        }


        private int GetUnusedID(string strFieldName)
        {
            return GetUnusedID(strFieldName, null);
        }
        private int GetUnusedID(string strFieldName, object oDeafult)
        {
            if (m_keyvalue == null) return -1;
            DBCustomClass subdbclass = m_dbclass;
            //string sql = string.Format("SELECT * FROM {0}", subdbclass.MainTable);
            //DataTable tbl = Helper.GetDataTable(sql, MainForm.conn);
            DataTable tbl = Helper.GetDataTableProxy(subdbclass.MainTable, MainForm.conn);
            int nFieldIndex = -1;
            for (int i = 0; i < subdbclass.DBPrimaryKey.Length; i++)
            {
                m_findkeyvalues[i] = null;// GetNextID(subdbclass.DBPrimaryKey[i]);
                if (subdbclass.DBPrimaryKey[i].ToString().ToLower() == strFieldName.ToLower())
                    nFieldIndex = i;
                else if (m_keyvalue.Contains(subdbclass.DBPrimaryKey[i]))
                {
                    m_findkeyvalues[i] = m_keyvalue[subdbclass.DBPrimaryKey[i]];
                }
            }
            if (oDeafult != null && oDeafult != DBNull.Value)
                m_findkeyvalues[nFieldIndex] = oDeafult;
            else
                m_findkeyvalues[nFieldIndex] = GetNextID(strFieldName);
            if (Program.EnableRemoting && subdbclass.MainTable.Length > 7 && subdbclass.MainTable.Substring(0, 7) == "RTTEST_")
            {
                m_findkeyvalues[nFieldIndex] = Convert.ToInt32(m_findkeyvalues[nFieldIndex]);
                DataTableProxy proxy = DataTableServerProxy.GetDataTableProxy();
                m_findkeyvalues = proxy.UTGetTableID(subdbclass.MainTable, m_findkeyvalues);
            }
            else
            {
                bool bFirstTry = true;
                while (tbl.Rows.Find(m_findkeyvalues) != null)
                {
                    m_findkeyvalues[nFieldIndex] = Convert.ToInt32(m_findkeyvalues[nFieldIndex]) + (bFirstTry == true ? 1 : 1/*1000*/); // 现在的id太大了
                    bFirstTry = false;
                }
            }
            m_keyvalue[strFieldName] = m_findkeyvalues[nFieldIndex];
            if (oDeafult == DBNull.Value || Convert.ToInt32(m_findkeyvalues[nFieldIndex]) != Convert.ToInt32(oDeafult))
                SetLastID(strFieldName, Convert.ToInt32(m_findkeyvalues[nFieldIndex]));
            return Convert.ToInt32(m_findkeyvalues[nFieldIndex]);
        }

        private void GetMainFieldAndSubField(ref string strMainField, ref string strSubField, string[] strMainRelations, string[] strSubRelations, string strKeyField)
        {
            for (int i = 0; i < strSubRelations.Length; i++)
            {
                if (strKeyField.ToLower() == strSubRelations[i].ToLower())
                {
                    strMainField = strMainRelations[i];
                    strSubField = strSubRelations[i];
                    break;
                }
            }
        }

        //生成新记录主键值及其他字段值, 必须在 GernateDefaultValue 生成主键字段值后，才能调用
        private bool GernateDefaultRowValue()
        {
            DataTable tbl = m_dbclass.DataTable;
            return FillNewRowDefaultValues(tbl, m_htnewrow);            
        }
        //生成新记录的默认值(主键值及其他字段值)
        public bool GernateDefaultValue()
        {
            DBCustomClass subdbclass = m_dbclass;
            string[] strSubReleations = subdbclass.GetKeyFromParentField(Convert.ToInt32(m_key), false);//获取关联关系的行数
            if (strSubReleations == null) return false;

            switch (strSubReleations.Length)
            {
                case 0: //无关联关系
                    goto case 1;
                case 1: //单行关联关系
                    //主表关联字段内容要复制到子表对应字段
                    if (m_dbclass.Parent != null)
                    {
                        DBCustomClass parentClass = m_dbclass.Parent as DBCustomClass;
                        //Hashtable MainTableKeys = Helper.TransToHashTable(parentClass.DBPrimaryKey, true);
                        Hashtable subTableKeys = Helper.TransToHashTable(m_dbclass.DBPrimaryKey, true);
                        Hashtable htSubReleations = Helper.TransToHashTable(strSubReleations, true);
                        string[] strMainReleations = subdbclass.GetKeyFromParentField(Convert.ToInt32(m_key), true);//获取关联关系的行数
                        //填充主从表关联字段都为主键的内容
                        for (int i = 0; i < subdbclass.DBPrimaryKey.Length; i++)
                        {
                            string strKeyField = (string)subdbclass.DBPrimaryKey[i];
                            if (i >/*=*/ strSubReleations.Length)
                                m_keyvalue[strKeyField] = GetUnusedID(strKeyField);
                            else
                            {
                                string strMainField = string.Empty; // = strMainReleations[i].ToLower();
                                string strSubField = string.Empty;  // = strSubReleations[i].ToLower();
                                GetMainFieldAndSubField(ref strMainField, ref strSubField, strMainReleations, strSubReleations, strKeyField);

                                if (htSubReleations.Contains(strKeyField.ToLower()))
                                {//如果从表主键字段在关联关系中，则为外键，需要使用主表字段值作为内容
                                    //m_keyvalue[strKeyField] = parentClass.Value;      //使用主键字段值作为默认值
                                    if (parentClass.ID != null && parentClass.ID.ToLower() == strMainField)
                                        m_keyvalue[strKeyField] = GetUnusedID(strKeyField, parentClass.Value); //使用主表字段值作为默认值，如果子表已经存在则自动换为不存在的值
                                    else
                                        m_keyvalue[strKeyField] = GetUnusedID(strKeyField, parentClass.GetFieldValue(strMainField)); //如果真实字段不是关联关系中的主表字段，则需要从数据库中读,todo:可能其他TAB页在内存中修改了这个值，则会有问题

                                    m_newvalue[strMainField] = strSubField;
                                    //m_newvalue[strKeyField.ToString]
                                    /*
                                    if (subdbclass.DBPrimaryKey.Length > 1)
                                    {
                                        m_keyvalue[strKeyField] = parentClass.Value;
                                    }
                                    else
                                    {
                                        string strmsg = string.Format("从表({0})的主键({1})在关联关系中，不能新建记录。请检查关联关系设置或者从表设置是否正确。",
                                            subdbclass.MainTable, strKeyField);
                                        System.Windows.Forms.MessageBox.Show(strmsg, "错误", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Stop);
                                        throw new Exception(strmsg);
                                    }*/
                                }
                                else
                                {
                                    //将从表每个主键字段生成现在没有ID值
                                    m_keyvalue[strKeyField] = GetUnusedID(strKeyField);
                                    // 必然s trMainField 和 strSubField 都是 string.empty， 算了
                                    //m_newvalue[strMainField] = strSubField;
                                }
                            }
                        }
                        //当从表中的关联字段非主键时，也需要正确填充
                        for(int i = 0; i < strSubReleations.Length; i ++)
                        {
                            string strsubfield = strSubReleations[i];

                            // 判断 strsubfield 是否是从表的 key
                            bool bIsSubKey = false;
                            foreach (object subDbKey in subdbclass.DBPrimaryKey)
                            {
                                if (strsubfield == subDbKey.ToString().ToLower())
                                {
                                    bIsSubKey = true;
                                    break;
                                }
                            }

                            if (!bIsSubKey)
                            {
                                if (parentClass.ID != null && parentClass.ID.ToLower() == strMainReleations[i])
                                    m_keyvalue[strsubfield] = parentClass.Value;
                                else
                                    m_keyvalue[strsubfield] = parentClass.GetFieldValue(strMainReleations[i]);
                            }
                        }
                    }
                    break;
                default://多行关联关系
                    if (m_dbclass.Parent != null)
                    {
                        DBCustomClass parentClass = m_dbclass.Parent as DBCustomClass;
                        Hashtable MainTableKeys = Helper.TransToHashTable(parentClass.DBPrimaryKey, true);
                        Hashtable subTableKeys = Helper.TransToHashTable(m_dbclass.DBPrimaryKey, true);
                        string[] strMainReleations = subdbclass.GetKeyFromParentField(Convert.ToInt32(m_key), true);//获取关联关系的行数
                        
                        for (int i = 0; i < strMainReleations.Length; i++)
                        {
                            string strMainField = strMainReleations[i].ToLower();
                            string strSubField = strSubReleations[i].ToLower();
                            bool bMainDBKey = MainTableKeys.Contains(strMainField);
                            bool bSubDBKey = subTableKeys.Contains(strSubField);
                            string strSubDBKeyFieldName = subTableKeys[strSubField].ToString();
                            if (bMainDBKey && bSubDBKey)
                            {//主从表字段都为主键，则使用主表字段内容作为默认值
                                m_keyvalue[strSubDBKeyFieldName] = parentClass.GetFieldValue(strMainField);//parentClass.Value; //parentClass[MainTableKeys[strMainField].ToString()];
                                m_newvalue[strMainField] = strSubField;
                            }
                            else if (!bSubDBKey)
                            {//从表为非主键，则默认值为默认行中的内容
                                DataRow def_row = CustomClassCache.GetPropDefaultValue(parentClass.MainTable);
                                if (def_row != null)
                                    m_keyvalue[strSubDBKeyFieldName] = def_row[strSubDBKeyFieldName];
                            }
                            else if (bSubDBKey)
                            {//主表字段非主键，从表字段为主键字段，则从表默认值为自动递增就可以了
                                m_keyvalue[strSubDBKeyFieldName] = GetUnusedID(strSubDBKeyFieldName, parentClass.GetFieldValue(strMainField));
                                m_newvalue[strMainField] = strSubField;
                            }
                        }

                        // keys还是要全部填满，即使不在对应关系中。已修复，待测试。
                        foreach (object key in m_keyvalue.Keys)
                            if (m_keyvalue[key] == null)
                            {
                                m_keyvalue[key] = GetUnusedID(key.ToString(), 1000);
                                break;
                            }
                    }
                    break;
            }

            //return true;
            return GernateDefaultRowValue(); //生成剩余其他的字段值
        }
        private bool InitNewRowValues(DataTable tbl, DataRow newrow)
        {
            if (tbl.Rows.Count > 1)
            {
                //DataRow defaultrow = tbl.Rows[0];
                foreach (DataColumn col in tbl.Columns)
                {
                    newrow[col.ColumnName] = DBNull.Value;//defaultrow[col.ColumnName];
                }
                return true;
            }
            return false;
        }
        private bool ColumnIsKeys(Hashtable keyvalue, string columnname)
        {
            foreach (string kname in keyvalue.Keys)
            {
                if (kname.ToLower().Trim() == columnname.ToLower().Trim())
                {
                    return true;
                }
            }
            return false;
        }
        
        //将预设的行内容设置好,以便用户在事件中动态修改
        private bool FillNewRowDefaultValues(DataTable tbl, Hashtable htnewrow)
        {
            //先复制主键值
            foreach(string key in m_keyvalue.Keys)
            {
                htnewrow[key] = m_keyvalue[key];//这里与另一个FillNewRowDefaultValues不同
            }

            //DataTable FullTbl = Helper.GetDataTable("SELECT * FROM " + NewRowTableName, m_dbclass.Conn);
            DataTable FullTbl = Helper.GetDataTableProxy(NewRowTableName, m_dbclass.Conn);
            if (FullTbl.Rows.Count >= 1 && Helper.HasTableDefaultRow(NewRowTableName, m_dbclass.Conn))//tbl.Rows.Count >= 1 && Helper.HasTableDefaultRow(tbl.TableName, MainForm.conn))
            {   //有默认行就先全复制默认行内容
                //DataRow defaultrow = FullTbl.Rows[0]; //tbl.Rows[0];
                DataRow defaultrow = FullTbl.Rows.Count > 0 ? FullTbl.Rows[0] : null;
                foreach (DataColumn col in tbl.Columns)
                {
                    if (!ColumnIsKeys(m_keyvalue, col.ColumnName) && defaultrow != null)
                    {
                        htnewrow[col.ColumnName] = defaultrow[col.ColumnName];
                    }
                }
            }
            else //无默认行
            {
                foreach (DataColumn col in tbl.Columns)
                {
                    if (!ColumnIsKeys(m_keyvalue, col.ColumnName))
                    {
                        htnewrow[col.ColumnName] = "";
                    }
                }
            }
            return true;
        }
        private bool FillNewRowDefaultValues(DataTable tbl, DataRow newrow)
        {
            //DataTable FullTbl = Helper.GetDataTable("SELECT * FROM " + NewRowTableName, m_dbclass.Conn);
            DataTable FullTbl = Helper.GetDataTableProxy(NewRowTableName, m_dbclass.Conn);

            DataRow defaultrow = FullTbl.Rows.Count > 0 ? FullTbl.Rows[0] : null;
            if (FullTbl.Rows.Count >= 1 && Helper.HasTableDefaultRow(NewRowTableName, MainForm.conn))//tbl.TableName, MainForm.conn))
            {
                //DataRow defaultrow = tbl.Rows[0];
                foreach (DataColumn col in tbl.Columns)
                {
                    if (!ColumnIsKeys(m_keyvalue, col.ColumnName))
                    {
                        if (m_htnewrow.ContainsKey(col.ColumnName) && m_htnewrow[col.ColumnName].ToString() != defaultrow[col.ColumnName].ToString())
                            newrow[col.ColumnName] = m_htnewrow[col.ColumnName];
                        else if (defaultrow != null)
                            newrow[col.ColumnName] = defaultrow[col.ColumnName];
                    }
                }
            }
            else //无默认行
            {
                int savetype = Helper.GetTableSaveType(NewRowTableName, m_dbclass.Conn);
                if (savetype == 1)
                {
                    //DataTable FullTbl = Helper.GetDataTable("SELECT * FROM " + NewRowTableName, m_dbclass.Conn);
                    //DataRow defaultrow = FullTbl.Rows[0];
                    foreach (DataColumn col in FullTbl.Columns)
                    {
                        if (!ColumnIsKeys(m_keyvalue, col.ColumnName))
                        {
                            if (m_htnewrow.ContainsKey(col.ColumnName) && m_htnewrow[col.ColumnName].ToString() != defaultrow[col.ColumnName].ToString())
                                newrow[col.ColumnName] = m_htnewrow[col.ColumnName];
                            //else if (defaultrow != null)
                            //    newrow[col.ColumnName] = defaultrow[col.ColumnName];
                        }
                    }
                }
            }
            return true;
        }

        private bool IsAutoIncreamentField(string strField)
        {
            //支持自动编号、链接服务器的表添加记录
            //当主键为1个时判断是否为自增长列
            //add by cuiwei 08.5.20
            bool zidongfenpei = false;
            string tblname = m_dbclass.MainTable;
            
            string[] ss = tblname.Split(new char[] { '.' });

            string sql;
            if (ss.Length > 1) // 是链接服务器
            {
                sql = string.Format("select usertype from {0}.{1}.dbo.syscolumns where name='{3}' and usertype = 7 and id = (SELECT id from {0}.{1}.dbo.sysobjects WHERE name = '{2}')", ss[0], ss[1], ss[3], strField);
            }
            else
            {
                sql = string.Format("select usertype from syscolumns where name='{0}' and usertype = 7 and colstat = 1 and id = (SELECT id from sysobjects WHERE name = '{1}')", strField, ss[0]);
            }
            DataTable dt_findAutoInc = Helper.GetDataTableWithSqlProxy("syscolumns", sql, m_dbclass.Conn);

            if (dt_findAutoInc.Rows.Count > 0)
                zidongfenpei = true;

            return zidongfenpei;
        }

        public bool NewRow()
        {
            //先将主键值复制回来
            foreach(string key in m_htnewrow.Keys)
            {
                if (m_keyvalue.ContainsKey(key))
                    m_keyvalue[key] = m_htnewrow[key];
            }
            
            NewSubTalbeRecordCommand cmd = new NewSubTalbeRecordCommand();
            cmd.Execute();

            //GernateDefaultValue();//在外面调用，因脚本要干涉这个过程
            SqlDataAdapter adp = m_dbclass.DataAdapter;
            DataTable tbl = m_dbclass.DataTable;

            try
            {
                DataRow newrow = tbl.NewRow();
                newrow.BeginEdit();
                InitNewRowValues(tbl, newrow);
                foreach (string fieldname in m_keyvalue.Keys)
                {
                    bool bIsAutoIncre = false;
                    for (int i = 0; i < m_dbclass.DBPrimaryKey.Length; i++)
                    {
                        if (m_dbclass.DBPrimaryKey[i].ToString().ToLower() == fieldname.ToLower())
                        {
                            bIsAutoIncre = IsAutoIncreamentField(m_dbclass.DBPrimaryKey[i].ToString());
                            break;
                        }
                    }
                    if (!bIsAutoIncre)
                        newrow[fieldname] = m_keyvalue[fieldname];
                }
                FillNewRowDefaultValues(tbl, newrow);

                tbl.Rows.Add(newrow);

                /* remoting
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.InsertCommand = cmdBuilder.GetInsertCommand();
                int val = adp.Update(tbl);
                tbl.AcceptChanges();
                */

                int val = Helper.SaveTable(tbl, m_dbclass.StrSql, MainForm.conn);

                CustomClassCache.DelAllDataTableCache(); //清除缓冲
            }
            catch(Exception ex)
            {
                tbl.RejectChanges();
                Helper.AddLog(ex.Message);
                return false;
            }
            return true;
        }
        //完成刷新后，主表字段值的新内容可能会变化，需要下面给出
        //主表字段的外键是从表的关联关系中的一个字段，则返回新值
        private object GetNewValue(CustomProperty prop)
        {
            if (m_newvalue.Contains(prop.ID.ToLower()))
            {
                Hashtable htkeyvalue = Helper.TransHashTableCase(m_keyvalue, true);
                return htkeyvalue[m_newvalue[prop.ID.ToLower()]];
            }
            
            return (prop.Value as DBCustomClass).Value;
        }
        private object GetNewValue(string key)
        {
            if (m_newvalue.Contains(key))
            {
                Hashtable htkeyvalue = Helper.TransHashTableCase(m_keyvalue, true);
                return htkeyvalue[m_newvalue[key.ToLower()]];
            }

            return null;
        }
        //完成新建记录后，需要修改 DBClass 当前主表记录中所有与从表的关联字段，不然不能正确地显示从表的内容
        public void SetNewValue(CustomProperty prop)
        {
            DBCustomClass cls = prop.Parent as DBCustomClass;
            //if (cls != null)
            //{
            //    cls.MarkValueChanged();
            //}

            foreach (string key in m_newvalue.Keys) //获取所有的主表字段名
            {
                CustomProperty dbprop = null;
                if (key.ToLower() == prop.ID.ToLower())
                    dbprop = prop;
                else
                    dbprop = cls[key];
                
                if (dbprop != null)
                {
                    dbprop.SetValue(GetNewValue(key));
                    cls.OnAsynNotifyEvent(2, dbprop);
                }
            }
            //当前字段不是关联字段，可能是顶层虚拟字段或者其他虚拟字段，需要重新载入当前
            if (m_dbclass != null && m_dbclass.PrimaryKey.Length > 1) 
            {
                object[] keyvalues = new object[m_dbclass.PrimaryKey.Length];
                Hashtable htkeyvalue = Helper.TransHashTableCase(m_keyvalue, true);
                for (int i = 0; i < m_dbclass.PrimaryKey.Length; i++)
                {
                    keyvalues[i] = htkeyvalue[m_dbclass.PrimaryKey[i]];
                }
                m_dbclass.ReloadDBData(keyvalues);
            }
            else if (m_dbclass != null && m_dbclass.PrimaryKey.Length == 1)
            {
                DBCustomClass dbParent = prop.Parent as DBCustomClass;
                if (dbParent != null)
                {
                    if (m_dbclass.PrimaryKey.Length == dbParent.FindKeys.Length)
                    {
                        m_dbclass.ReloadDBData(dbParent.FindKeys);
                    }
                    else
                    {
                        object[] keyvalues = new object[m_dbclass.PrimaryKey.Length];
                        Hashtable htkeyvalue = Helper.TransHashTableCase(m_keyvalue, true);
                        for (int i = 0; i < m_dbclass.PrimaryKey.Length; i++)
                        {
                            keyvalues[i] = htkeyvalue[m_dbclass.PrimaryKey[i]];
                        }
                        m_dbclass.ReloadDBData(keyvalues);
                    }
                }
            }

            cls.OnAsynNotifyEvent(1, null);
        }
    };
}
