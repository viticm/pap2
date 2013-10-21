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
using System.Diagnostics;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    public partial class BaseForm
    {
        private void InitLua()
        {
            try
            {
                //m_lua["MainForm"] = (MainForm)ParentForm;
                m_lua["BaseForm"] = this;
                m_lua["TblMain"] = m_tblMain;
                m_lua["Pages"] = Pages;
                m_lua["BranchDB"] = Program.m_bLoadBackupDB;
                m_lua["BaseTree"] = this.baseTree;
                m_lua["Form"] = this;
                m_lua["Conn"] = m_conn;
                m_lua["RootDir"] = Program.RootDir;
                m_lua["StartUpDir"] = Application.StartupPath;
                m_lua["ExtraMenuItem"] = this.m_ExtraMenuItem;
                m_lua["MainTableName"] = this.m_strMainTableName;
                m_lua["bCommentOnSave"] = Program.m_bCommentOnSave;
                //m_lua["DebugBox"] = MainForm.DebugForm.OutputBox;

                //m_lua["oo"] = new tcls1();
                //建立环境帮助函数
                //m_lua.DoString("function trace() \r\n end\r\n");
                //string sCode = "____main_env_varTable = {}\r\nlocal function _MakeEnv(envname)\r\n	____main_env_varTable[envname] = {}\r\n	_G[\"__fname__\"..envname] = ____main_env_varTable[envname]\r\n____main_env_varTable[envname].envname = envname\r\n	setmetatable(____main_env_varTable[envname], {__index = _G})\r\nend\r\nfunction _ChgEnv(envname)\r\n	if (envname == nil) then\r\n        setfenv(2, _G)\r\n	elseif (____main_env_varTable[envname] == nil) then\r\n  		_MakeEnv(envname)\r\n  		setfenv(2, ____main_env_varTable[envname])\r\n else\r\n    	setfenv(2, ____main_env_varTable[envname])\r\n	end\r\nend\r\n";
                //m_lua.DoString(sCode);
                //m_lua.RegisterFunction("ClearTreeNodes", this, typeof(BaseForm).GetMethod("ClearTreeNodes"));
                m_lua.RegisterFunction("FindTreeNode", this, typeof(BaseForm).GetMethod("FindTreeNode"));
                m_lua.RegisterFunction("FindDataRow", this, typeof(BaseForm).GetMethod("FindDataRow"));
                m_lua.RegisterFunction("AddTreeNodes", this, typeof(BaseForm).GetMethod("AddTreeNodes"));
                m_lua.RegisterFunction("AddTreeNodeClone", this, typeof(BaseForm).GetMethod("AddTreeNodeClone"));
                m_lua.RegisterFunction("RemoveTreeNode", this, typeof(BaseForm).GetMethod("RemoveTreeNode"));
                m_lua.RegisterFunction("ExpandTreeNode", this, typeof(BaseForm).GetMethod("ExpandTreeNode"));
                m_lua.RegisterFunction("MoveTreeNode", this, typeof(BaseForm).GetMethod("MoveTreeNode"));
                m_lua.RegisterFunction("SqlQueryOnce", this, typeof(BaseForm).GetMethod("SqlQueryOnce"));
                m_lua.RegisterFunction("SqlInsertRecord", this, typeof(BaseForm).GetMethod("SqlInsertRecord"));
                m_lua.RegisterFunction("SqlInsertRecord_NoReturn", this, typeof(BaseForm).GetMethod("SqlInsertRecord_NoReturn"));
                m_lua.RegisterFunction("SqlInsertRecordWithoutKey", this, typeof(BaseForm).GetMethod("SqlInsertRecordWithoutKey"));
                m_lua.RegisterFunction("SqlInsertSubsequenceRecord", this, typeof(BaseForm).GetMethod("SqlInsertSubsequenceRecord"));
                m_lua.RegisterFunction("SqlUpdateRecordAsCertainRow", this, typeof(BaseForm).GetMethod("SqlUpdateRecordAsCertainRow"));
                m_lua.RegisterFunction("SqlUpdateRecord", this, typeof(BaseForm).GetMethod("SqlUpdateRecord"));
                m_lua.RegisterFunction("SqlUpdateRecord_NoReturn", this, typeof(BaseForm).GetMethod("SqlUpdateRecord_NoReturn"));
                m_lua.RegisterFunction("NewSqlUpdateRecord_NoReturn", this, typeof(BaseForm).GetMethod("NewSqlUpdateRecord_NoReturn"));
                m_lua.RegisterFunction("SqlUpdateRecords", this, typeof(BaseForm).GetMethod("SqlUpdateRecords"));
                m_lua.RegisterFunction("SqlExec", this, typeof(BaseForm).GetMethod("SqlExec"));
                m_lua.RegisterFunction("SqlUpdateRecordUsingSql", this, typeof(BaseForm).GetMethod("SqlUpdateRecordUsingSql"));
                m_lua.RegisterFunction("SqlRemoveRecord", this, typeof(BaseForm).GetMethod("SqlRemoveRecord"));
                m_lua.RegisterFunction("NewSqlRemoveRecord", this, typeof(BaseForm).GetMethod("NewSqlRemoveRecord"));
                m_lua.RegisterFunction("SqlRemoveRecords", this, typeof(BaseForm).GetMethod("SqlRemoveRecords"));
                m_lua.RegisterFunction("FindRowInTable", this, typeof(BaseForm).GetMethod("FindRowInTable"));
                m_lua.RegisterFunction("ObjToObjArr", this, typeof(BaseForm).GetMethod("ObjToObjArr"));
                m_lua.RegisterFunction("ObjToObjArr2", this, typeof(BaseForm).GetMethod("ObjToObjArr2"));
                m_lua.RegisterFunction("ObjToObjArr3", this, typeof(BaseForm).GetMethod("ObjToObjArr3"));
                m_lua.RegisterFunction("FindAllTreeNodesByLevel", this, typeof(BaseForm).GetMethod("FindAllTreeNodesByLevel"));
                m_lua.RegisterFunction("InitPgAndPghelper", this, typeof(BaseForm).GetMethod("InitPgAndPghelper"));
                m_lua.RegisterFunction("SelectTreeNode", this, typeof(BaseForm).GetMethod("SelectTreeNode"));
                m_lua.RegisterFunction("GetPropDefaultValue", this, typeof(BaseForm).GetMethod("GetPropDefaultValue"));
                m_lua.RegisterFunction("DataBaseNull", this, typeof(BaseForm).GetMethod("DataBaseNull"));
                m_lua.RegisterFunction("GetUnusedID", this, typeof(BaseForm).GetMethod("GetUnusedID"));
                m_lua.RegisterFunction("RegComDll", this, typeof(BaseForm).GetMethod("RegComDll"));
                m_lua.RegisterFunction("ConvertToInt32", this, typeof(BaseForm).GetMethod("ConvertToInt32"));
                m_lua.RegisterFunction("ForceRecordChanged", this, typeof(BaseForm).GetMethod("ForceRecordChanged"));
                m_lua.RegisterFunction("RefreshPropertyGrid", this, typeof(BaseForm).GetMethod("RefreshPropertyGrid"));
                m_lua.RegisterFunction("InitPreview", this, typeof(BaseForm).GetMethod("InitPreview"));
                m_lua.RegisterFunction("InitPreviewPlugIn", this, typeof(BaseForm).GetMethod("InitPreviewPlugIn"));
                m_lua.RegisterFunction("InitPreviewNpcPortrait", this, typeof(BaseForm).GetMethod("InitPreviewNpcPortrait"));
                m_lua.RegisterFunction("LoadModel", this, typeof(BaseForm).GetMethod("LoadModel"));
                m_lua.RegisterFunction("LoadModelAniOnce", this, typeof(BaseForm).GetMethod("LoadModelAniOnce"));
                m_lua.RegisterFunction("BeginEdit", this, typeof(BaseForm).GetMethod("BeginEdit"));
                m_lua.RegisterFunction("msgbox", this, typeof(BaseForm).GetMethod("msgbox"));
				m_lua.RegisterFunction("ShowModelInDialog", this, typeof(BaseForm).GetMethod("ShowModelInDialog"));
                m_lua.RegisterFunction("ExtraExportAs", this, typeof(BaseForm).GetMethod("ExtraExportAs"));
                m_lua.RegisterFunction("GetHostName", this, typeof(BaseForm).GetMethod("GetHostName"));
                m_lua.RegisterFunction("GetDateTime", this, typeof(BaseForm).GetMethod("GetDateTime"));
                m_lua.RegisterFunction("ForceReloadOnSave", this, typeof(BaseForm).GetMethod("ForceReloadOnSave"));
                //注册添加修改记录和备注函数
                m_lua.RegisterFunction("AddDataRecord", this, typeof(BaseForm).GetMethod("AddDataRecord"));
                // mainFrame
                m_lua.RegisterFunction("OpenForm", ParentForm, typeof(MainForm).GetMethod("OpenForm"));
                
                //增加默认的一些函数
                /*m_lua.DoString("local mt = {\r\n	__index = function(package, classname)\r\n		local class = rawget(package, classname)\r\n		if class == nil then\r\n			class = luanet.import_type(package.packageName .. \".\" .. classname) \r\n			package[classname] = class		\r\n		end\r\n		return class\r\n	end	\r\n}\r\n");
                m_lua.DoString("function CLRPackage(assemblyName, packageName)\r\n  local table = {}\r\n  luanet.load_assembly(assemblyName)\r\n  table.packageName = packageName\r\n  setmetatable(table, mt)\r\n  return table\r\n end\r\n");
                m_lua.DoString("Forms = CLRPackage('System.Windows.Forms', 'System.Windows.Forms')\r\nDrawing = CLRPackage(\"System.Drawing\", \"System.Drawing\")\r\nLuaInterface = CLRPackage(\"LuaInterface\", \"LuaInterface\")\r\nIO = CLRPackage(\"System.IO\", \"System.IO\")\r\nSystem = CLRPackage(\"System\", \"System\")\r\n");
                m_lua.DoString("msgbox = Forms.MessageBox.Show");
                 */

                //m_lua.DoString("function trace(txt)\r\n    MainForm.DebugForm.Visible = true\r\n    MainForm.DebugForm.OutputBox.Text = MainForm.DebugForm.OutputBox.Text ..txt..'\\r\\n' \r\n end");
                m_lua.DoString(@"
                    function trace(txt)
                        MainForm.LogEditBox.Visible = true
                        --MainForm.LogEditBox.Text = MainForm.LogEditBox.Text ..txt..'\\r\\n'
                        MainForm:AddLog(txt)
                    end
                ");
                m_lua.DoString("function onbeforevaluechang(prop, e)\r\n    return true\r\nend");
                m_lua.DoString("function onaftervaluechanged(prop, e)\r\nend");
                //m_lua.DoString("function oneditvalue(prop, e)\r\n    return false\r\nend");
                //m_lua.DoString("function ongetreevalue(prpo, e) \r\nreturn true, \"***\"\r\nend");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public string BrowseFile(string strExtends)
        {
            string[] extends = strExtends.Split(',');
            return FileFolderHelper.BrowserFile(extends);
        }

        public int GetMainFormHwnd()
        {
            return (int)this.Handle;
        }

        public int msgbox(string info, string caption, int buttonCount)
        {
            if (buttonCount == 1)
            {
                MessageBox.Show(info, caption, MessageBoxButtons.OK);
                return 1;
            }
            else if (buttonCount == 2)
            {
                DialogResult dr = MessageBox.Show(info, caption, MessageBoxButtons.YesNo);
                if (dr == DialogResult.Yes)
                {
                    return 1;
                }
                else if (dr == DialogResult.No)
                {
                    return 2;
                }
                return -1;
            }
            else
            {
                return -1;
            }
        }


        public string GetHostName()
        {
            return Helper.GetHostName();
        }

        public string GetDateTime()
        {
            return DateTime.Now.ToString();
        }

        /// <summary>
        /// 将一个数据表导出到文件
        /// 中间加的是\t每行后加\r\n
        /// </summary>
        /// <param name="mydt">数据表</param>
        /// <param name="strFilePath">目标文件</param>
        /// <returns></returns>
        public bool ExtraExportAs(DataTable mydt, string strFilePath)
        {            
            string strExamPath = strFilePath.Substring(0,strFilePath.LastIndexOf('\\'));
            if (!Directory.Exists(strExamPath))
            {
                MessageBox.Show(strFilePath, "目录错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            StringBuilder strFinal = new StringBuilder();

            foreach (DataColumn dc in mydt.Columns)//添加列名
            {
                strFinal.Append(dc.ColumnName);
                strFinal.Append('\t');
            }
            strFinal.Remove(strFinal.Length - 1, 1);
            strFinal.Append("\r\n");
            int nColCnt = mydt.Columns.Count;
            DataTableReader dr = mydt.CreateDataReader();
            string strItem = string.Empty;
            Type type;
            
            while (dr.Read())
            { 
                for (int i = 0; i < nColCnt; i++)
                {
                    strItem = dr[i].ToString();
                    if (strItem.Contains("\n"))
                        strItem = strItem.Replace("\n", "");  
                    type = dr[i].GetType();
                    if (type.Name == "Boolean")
                    {
                        strItem = strItem.ToLower() == "true" ? "1" : "0";
                    }
                    strFinal.Append(strItem);
                    strFinal.Append('\t');
                }
                strFinal.Remove(strFinal.Length - 1, 1);
                strFinal.Append("\r\n");                
            }
            try
            {
                FileFolderHelper.StringToFile(strFinal.ToString(), strFilePath);
                return true;
            }
            catch (Exception ex)
            {
                string s = string.Format("导出 tab 文件:\r\n    {0}\r\n时出现错误。请检查文件名，磁盘空间等。\r\n\r\n错误讯息为:{1}", strFilePath, ex.Message);
                System.Windows.Forms.MessageBox.Show(s, "Error");
                return false;
            } 
        }
        public DataRow FindDataRow(DataTable table, object[] keys)
        {
            DataRow row = table.Rows.Find(keys);
            return row;
        }

        public void ToggleEngine()
        {
            //this.tmrRender.Enabled = !this.tmrRender.Enabled;
            if (m_3DEngie != null)
                m_3DEngie.ToggleEngine();
        }

        public void GCCollect()
        {
            GC.Collect();
        }

        public TreeNode FindTreeNode(object[] tags, int level)
        {
            foreach (TreeNode node in FindAllTreeNodesByLevel(level))
            {
                object[] node_tags = (object[])node.Tag;
                bool keysTheSame = true;
                for (int i = 0; i < tags.Length; i++)
                {
                    if (node_tags[i].ToString().Trim() != tags[i].ToString().Trim())
                    {
                        keysTheSame = false;
                        break;
                    }
                }
                if (keysTheSame)
                    return node;
            }
            return null;
        }

        public TreeNode AddTreeNodes(ref TreeNodeCollection nodes, string name, object tags)
        {
            object[] KEYS = null;
            if (tags is LuaTable)
            {
                KEYS = new object[((LuaTable)tags).Keys.Count];
                for (int i = 0; i < ((LuaTable)tags).Keys.Count; i++)
                {
                    KEYS[i] = ((LuaTable)tags)[i];
                }
            }
            else if (tags is object[])
            {
                KEYS = (object[])(tags);
            }

            TreeNode newNode = nodes.Add(name);
            newNode.Tag = tags;
            if (KEYS != null)
                newNode.Tag = KEYS;
            return newNode;
        }

        public int AddTreeNodeClone(ref TreeNodeCollection nodes, ref TreeNode node)
        {
            int count = nodes.Add((TreeNode)node.Clone());
            return count;
        }

        public void RemoveTreeNode(ref TreeNode node)
        {
            node.Remove();
        }

        public void ExpandTreeNode(ref TreeNode node)
        {
            node.Expand();
        }

        public TreeNode MoveTreeNode(LuaTable CatSeq, ref TreeNode node)
        {
            int level = -1;
            TreeNodeCollection nodes = this.baseTree.Nodes;
            for (int i = 0; i < CatSeq.Values.Count; i++)
            {
                foreach (TreeNode eachnode in nodes)
                {
                    object[] eachNodeTags = eachnode.Tag as object[];
                    object[] checkNodeTags = CatSeq[i] as object[];

                    if (eachNodeTags != null && checkNodeTags != null) // 如果待检查的是数组
                    {
                        bool bSame = true;
                        for (int j = 0; j < eachNodeTags.Length; j++)
                        {
                            if (eachNodeTags[j].ToString().Trim() != checkNodeTags[j].ToString().Trim())
                            {
                                bSame = false;
                                break;
                            }
                        }
                        if (bSame)
                        {
                            nodes = eachnode.Nodes;
                            level = eachnode.Level;
                            break;
                        }
                    }

                    else if (eachnode.Tag.ToString().Trim() == CatSeq[i].ToString().Trim())
                    {
                        nodes = eachnode.Nodes;
                        level = eachnode.Level;
                        break;
                    }
                }
            }

            if (level == node.Level - 1) // 精确找到要添加的父亲,添加在此结点下
            {
                TreeNode newnode = (TreeNode)node.Clone();
                nodes.Add(newnode);
                node.Remove();
                return newnode;
            }
            else if (level == node.Level - 2) // 未精确找到,添加在最后找到的结点的第一个儿子下.
            {
                TreeNode newnode = (TreeNode)node.Clone();
                nodes[0].Nodes.Add(newnode);
                node.Remove();
                return newnode;
            }
            return null;
        }

        public DataTable SqlGetDataTable(string sql, SqlConnection Conn)
        {
            Debug.Assert(true, "remoting");
            return Helper.GetDataTableWithSqlProxy("", sql, Conn);
        }

        public DataSet SqlQueryOnce(string sql, SqlConnection Conn)
        {
            Debug.Assert(true, "remoting");
            SqlDataAdapter adpt = new SqlDataAdapter(sql, Conn);
            DataSet ds = new DataSet();
            ds.Clear();
            try
            {
                adpt.Fill(ds);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
            return ds;
        }

        public DataRow SqlInsertRecordWithUniqueField(string tblname, object keys, string[] uniqueFields)
        {
            // 获得DataTable
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            DataRow newRow = tbl.NewRow();

            // 获得默认行
            DataRow defaultRow = null;
            if (tbl.PrimaryKey.Length > 0)
            {
                object[] default_keys = new object[tbl.PrimaryKey.Length];
                for (int i = 0; i < default_keys.Length; i++)
                {
                    default_keys[i] = 0;
                }
                defaultRow = tbl.Rows.Find(default_keys);
            }
            else
            {
                string err = string.Format("在函数 SqlInsertRecord 调用时发生错误，数据表没有主键。");
                MessageBox.Show(err);
                return null;
            }
            if (defaultRow == null)
                defaultRow = tbl.Rows[0];
            
            bool bHasDefualtRow = Helper.HasTableDefaultRow(tblname, Conn);
            // 编辑此行
            newRow.BeginEdit();
            if (bHasDefualtRow)
                foreach (DataColumn col in tbl.Columns)
                    newRow[col] = defaultRow[col];
            for (int i = 0; i < tbl.PrimaryKey.Length; i++)
            {
                if (keys is LuaTable)
                    newRow[tbl.PrimaryKey[i]] = ((LuaTable)keys)[i];
                else if (keys is object[])
                    newRow[tbl.PrimaryKey[i]] = ((object[])keys)[i];
                else
                    MessageBox.Show("错误的输入参数，函数：SqlInsertRecord");
            }
            // unique fields
            foreach (string uniquefield in uniqueFields)
            {
                for (int i = 0; ; i++)
                {
                    string sql = string.Format("SELECT {0} FROM {1} WHERE {0}='{0}{2}'", uniquefield, tblname, i);
                    string filter = string.Format("{0}='{0}{1}'", uniquefield, i);

                    //DataTable tblcheck = Helper.GetDataTable(sql, Conn);

                    DataTable tblcheck = Helper.GetDataTableProxy(tblname, filter, null, Conn);

                    if (tblcheck.Rows.Count == 0)
                    {
                        newRow[uniquefield] = uniquefield + i.ToString();
                        break;
                    }
                }
            }
            newRow.EndEdit();

            // 提交
            try
            {
                tbl.Rows.Add(newRow);
            }
            catch (Exception ex)
            {
                string err = string.Format("未能向表 {0} 中插入记录：\r\n\r\n{1}", tblname, ex.Message);
                //MessageBox.Show(err);
                return null;
            }

            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */
            string sql_tbl = "SELECT * FROM " + tblname;
            Helper.SaveTable(tbl, sql_tbl, Conn);

            return newRow;
        }

        public DataRow SqlInsertRecord(string tblname, object keys)
        {
            // 获得DataTable
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT * FROM " + tblname;

            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);
            
            DataRow newRow = tbl.NewRow();

            // 获得默认行
            DataRow defaultRow = null;
            if (tbl.PrimaryKey.Length > 0)
            {
                object[] default_keys = new object[tbl.PrimaryKey.Length];
                for (int i = 0; i < default_keys.Length; i++)
                {
                    default_keys[i] = 0;
                }
                defaultRow = tbl.Rows.Find(default_keys);
            }
            else
            {
                string err = string.Format("在函数 SqlInsertRecord 调用时发生错误，数据表没有主键。");
                MessageBox.Show(err);
                return null;
            }
            if (defaultRow == null)
                defaultRow = tbl.Rows[0];

            // 编辑此行
            //自增长字段处理
            //add by cuiwei 08.04.03
            bool zidongfenpei = false;
            if(tblname.Contains(".") && tbl.PrimaryKey.Length == 1)
            {
                string[] ss = tblname.Split(new char[]{'.'});
                string sql = string .Format("select usertype from {0}.{1}.dbo.syscolumns where name='{3}' and usertype = 7 and id = (SELECT id from {0}.{1}.dbo.sysobjects WHERE name = '{2}')", ss[0],ss[1],ss[3],tbl.PrimaryKey[0].ColumnName);
                //DataTable dt_findAutoInc = Helper.GetDataTable(sql,this.m_conn);

                DataTable dt_findAutoInc = Helper.GetDataTableProxy(sql, this.m_conn);

                if (dt_findAutoInc.Rows.Count > 0)
                    zidongfenpei = true;
            }
            bool bHasDefualtRow = Helper.HasTableDefaultRow(tblname, Conn);
            if (zidongfenpei)
            {
                string strCols = "";
                foreach (DataColumn col in tbl.Columns)
                    strCols += col.ColumnName + ",";
                strCols = strCols.Replace(tbl.PrimaryKey[0].ColumnName + ",", "");
                strCols = strCols.TrimEnd(new char[] { ',' });
                string sql = "insert into {0}({1}) select {1} from {0} where {3} = {2} select max({3}) as 'id' from {0}";
                sql = string.Format(sql, tblname, strCols, defaultRow[tbl.PrimaryKey[0]].ToString(), tbl.PrimaryKey[0].ColumnName);
                //DataTable dt_ret = Helper.GetDataTable(sql, this.m_conn);
                DataTable dt_ret = Helper.GetDataTableWithSqlProxy(tblname, sql, Conn);
                int newid = ConvertToInt32(dt_ret.Rows[0][0]);
                newRow.BeginEdit(); 
                if (bHasDefualtRow)
                    foreach (DataColumn col in tbl.Columns)
                        newRow[col] = defaultRow[col]; //默认行
                newRow[tbl.PrimaryKey[0]] =  newid;
                newRow.EndEdit();
                return newRow;
            }
            else
            {
                newRow.BeginEdit();
                if (bHasDefualtRow)
                    foreach (DataColumn col in tbl.Columns) //默认行
                        newRow[col] = defaultRow[col];
                for (int i = 0; i < tbl.PrimaryKey.Length; i++)
                {
                    if (keys is LuaTable)
                        newRow[tbl.PrimaryKey[i]] = ((LuaTable)keys)[i];
                    else if (keys is object[])
                        newRow[tbl.PrimaryKey[i]] = ((object[])keys)[i];
                    else
                        MessageBox.Show("错误的输入参数，函数：SqlInsertRecord");
                }
                //add end
                newRow.EndEdit();

                // 提交
                object[] findKeys = new object[tbl.PrimaryKey.Length];
                if (keys is LuaTable)
                {
                    for (int i = 0; i< tbl.PrimaryKey.Length; i++)
                    {
                        findKeys[i] = ((LuaTable)keys)[i];
                    }
                }
                else if (keys is object[])
                {
                    findKeys = (object[])keys;
                }

                DataRow rowAlreadyExist = tbl.Rows.Find(findKeys);
                if (rowAlreadyExist != null)
                {
                    return rowAlreadyExist;
                }

                try
                {
                    tbl.Rows.Add(newRow);
                }
                catch (Exception ex)
                {
                    string err = string.Format("未能向表 {0} 中插入记录：\r\n\r\n{1}", tblname, ex.Message);
                    //MessageBox.Show(err);
                    return null;
                }

               
                try
                {
                    /* remoting
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = adp.Update(tbl);
                    tbl.AcceptChanges();
                    */

                    Helper.SaveTable(tbl, sql_tbl, Conn);
                }
                catch (Exception ex)
                {
                    string err = string.Format("未能向表 {0} 中插入记录：\r\n\r\n{1}", tblname, ex.Message);
                    //MessageBox.Show(err);
                    return null;
                }
                return newRow;
            }
        }

        public void SqlInsertRecord_NoReturn(string tblname, object keys)
        {
            // 获得DataTable
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT TOP 1 * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);
             
            DataRow newRow = tbl.NewRow();

            // 获得默认行
            DataRow defaultRow = null;
            if (tbl.PrimaryKey.Length > 0)
            {
                object[] default_keys = new object[tbl.PrimaryKey.Length];
                for (int i = 0; i < default_keys.Length; i++)
                {
                    default_keys[i] = 0;
                }
                defaultRow = tbl.Rows.Find(default_keys);
            }
            else
            {
                string err = string.Format("在函数 SqlInsertRecord 调用时发生错误，数据表没有主键。");
                MessageBox.Show(err);
                goto RETURN;
            }
            if (defaultRow == null)
                defaultRow = tbl.Rows[0];

            // 编辑此行
            //自增长字段处理
            //add by cuiwei 08.04.03
            bool zidongfenpei = false;
            if (tblname.Contains(".") && tbl.PrimaryKey.Length == 1)
            {
                string[] ss = tblname.Split(new char[] { '.' });
                string sql = string.Format("select usertype from {0}.{1}.dbo.syscolumns where name='{3}' and usertype = 7 and id = (SELECT id from {0}.{1}.dbo.sysobjects WHERE name = '{2}')", ss[0], ss[1], ss[3], tbl.PrimaryKey[0].ColumnName);
                DataTable dt_findAutoInc = Helper.GetDataTableWithSqlProxy("syscolumns", sql, this.m_conn);
                if (dt_findAutoInc.Rows.Count > 0)
                    zidongfenpei = true;
            }
            bool bHasDefualtRow = Helper.HasTableDefaultRow(tblname, Conn);
            if (zidongfenpei)
            {
                string strCols = "";
                foreach (DataColumn col in tbl.Columns)
                    strCols += col.ColumnName + ",";
                strCols = strCols.Replace(tbl.PrimaryKey[0].ColumnName + ",", "");
                strCols = strCols.TrimEnd(new char[] { ',' });
                string sql = "insert into {0}({1}) select {1} from {0} where {3} = {2} select max({3}) as 'id' from {0}";
                sql = string.Format(sql, tblname, strCols, defaultRow[tbl.PrimaryKey[0]].ToString(), tbl.PrimaryKey[0].ColumnName);
                DataTable dt_ret = Helper.GetDataTableWithSqlProxy(tblname, sql, this.m_conn);
                int newid = ConvertToInt32(dt_ret.Rows[0][0]);
                newRow.BeginEdit();
                if (bHasDefualtRow)
                    foreach (DataColumn col in tbl.Columns) //默认行
                        newRow[col] = defaultRow[col];
                newRow[tbl.PrimaryKey[0]] = newid;
                newRow.EndEdit();
                goto RETURN;
            }
            else
            {
                newRow.BeginEdit();
                if (bHasDefualtRow)
                    foreach (DataColumn col in tbl.Columns) //默认行
                        newRow[col] = defaultRow[col];
                for (int i = 0; i < tbl.PrimaryKey.Length; i++)
                {
                    if (keys is LuaTable)
                        newRow[tbl.PrimaryKey[i]] = ((LuaTable)keys)[i];
                    else if (keys is object[])
                        newRow[tbl.PrimaryKey[i]] = ((object[])keys)[i];
                    else
                        MessageBox.Show("错误的输入参数，函数：SqlInsertRecord");
                }
                //add end
                newRow.EndEdit();

                // 提交
                object[] findKeys = new object[tbl.PrimaryKey.Length];
                if (keys is LuaTable)
                {
                    for (int i = 0; i < tbl.PrimaryKey.Length; i++)
                    {
                        findKeys[i] = ((LuaTable)keys)[i];
                    }
                }
                else if (keys is object[])
                {
                    findKeys = (object[])keys;
                }

                DataRow rowAlreadyExist = tbl.Rows.Find(findKeys);
                if (rowAlreadyExist != null)
                {
                    goto RETURN;
                }

                try
                {
                    tbl.Rows.Add(newRow);
                }
                catch (Exception ex)
                {
                    string err = string.Format("未能向表 {0} 中插入记录：\r\n\r\n{1}", tblname, ex.Message);
                    //MessageBox.Show(err);
                    goto RETURN;
                }

                
                try
                {
                    /* remoting
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = adp.Update(tbl);
                    tbl.AcceptChanges();
                    */
                    Helper.SaveTable(tbl, sql_tbl, Conn);
                }
                catch (Exception ex)
                {
                    string err = string.Format("未能向表 {0} 中插入记录：\r\n\r\n{1}", tblname, ex.Message);
                    //MessageBox.Show(err);
                }
            }

        RETURN:
            tbl.Dispose();
        }

        public DataRow SqlInsertSubsequenceRecord(string tblname)
        {
            // 获得DataTable
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);
            
            DataRow newRow = tbl.NewRow();

            // 获得默认行
            DataRow defaultRow = null;
            if (tbl.PrimaryKey.Length > 0)
            {
                object[] default_keys = new object[tbl.PrimaryKey.Length];
                for (int i = 0; i < default_keys.Length; i++)
                {
                    default_keys[i] = 0;
                }
                defaultRow = tbl.Rows.Find(default_keys);
            }
            else
            {
                string err = string.Format("在函数 SqlInsertSubsequenceRecord 调用时发生错误，数据表没有主键。");
                MessageBox.Show(err);
                return null;
            }
            if (defaultRow == null)
                defaultRow = tbl.Rows[0];

            bool bHasDefualtRow = Helper.HasTableDefaultRow(tblname, Conn);
            // 编辑此行
            newRow.BeginEdit();
            if (bHasDefualtRow)
                foreach (DataColumn col in tbl.Columns) //默认行
                    newRow[col] = defaultRow[col];
            for (int i = 0; i < tbl.PrimaryKey.Length; i++)
            {
                string sql = string.Format("SELECT MAX({0}) FROM {1}", tbl.PrimaryKey[i].ColumnName, tblname);
                DataTable dt_maxid = Helper.GetDataTableWithSqlProxy(tblname, sql, m_conn);
                int max_this_key = Convert.ToInt32(dt_maxid.Rows[0][0]);
                newRow[tbl.PrimaryKey[i]] = max_this_key + 1;
            }
            newRow.EndEdit();

            // 提交
            tbl.Rows.Add(newRow);

            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sql_tbl, Conn);
             
            return newRow;
        }

        public DataRow SqlUpdateRecord(string tblname, object keys, string field, object value)
        {  
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            if (keys is LuaTable)
            {
                for (int i = 0; i < KEYS.Length; i++)
                {
                    KEYS[i] = ((LuaTable)keys)[i];
                }
            }
            else if (keys is object[])
            {
                KEYS = (object[])keys;
            }

            DataRow row = tbl.Rows.Find(KEYS);
            row[field] = value;

            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sql_tbl, Conn);

            return row;
        }

        public void SqlUpdateRecord_NoReturn(string tblname, object keys, string field, object value)
        {
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            if (keys is LuaTable)
            {
                for (int i = 0; i < KEYS.Length; i++)
                {
                    KEYS[i] = ((LuaTable)keys)[i];
                }
            }
            else if (keys is object[])
            {
                KEYS = (object[])keys;
            }

            DataRow row = tbl.Rows.Find(KEYS);
            row[field] = value;

            /*
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sql_tbl, Conn);

            tbl.Dispose();
            //return row;
        }

        public void NewSqlUpdateRecord_NoReturn(string tblname, object keys, object keynames, string field, object value)
        {
            object[] _keys = (object[])keys;
            object[] _keynames = (object[])keynames;
            string sqlcondition = " where ";
            for (int i = 0; i < _keys.Length; i++)
            {
                sqlcondition += _keynames[i].ToString().Trim() + "=" + _keys[i].ToString().Trim();
                sqlcondition += " and ";
            }
            sqlcondition = sqlcondition.Substring(0, sqlcondition.Length - 5);

            string sql_tbl = "SELECT * FROM " + tblname + sqlcondition;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            if (keys is LuaTable)
            {
                for (int i = 0; i < KEYS.Length; i++)
                {
                    KEYS[i] = ((LuaTable)keys)[i];
                }
            }
            else if (keys is object[])
            {
                KEYS = (object[])keys;
            }

            DataRow row = tbl.Rows.Find(KEYS);
            row[field] = value;

            Helper.SaveTable(tbl, sql_tbl, Conn);

            tbl.Dispose();
        }     
   
        public int SqlExec(string sql)
        {
            /* remoting
            int affectedCount = 0;
            m_conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = sql;
                affectedCount = cmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                string err = string.Format("在更新表时发生错误。\r\n\r\n错误信息：{1}", ex.Message);
                MessageBox.Show(err);
            }
            m_conn.Close();
            return affectedCount;
            */
            return Helper.ExecuteCommand(sql, Conn);
        }
       
        public string DotNetToString(object obj)
        {
            return obj.ToString();
        }

        public bool AddDataRecord(string extraRemarks)
        {
            //设置记录备注
            string strRemarks = "未添加备注";
            if (Program.m_bCommentOnSave)
            {
                InputBoxForm ibf = new InputBoxForm();
                if (ibf.ShowDialog() == DialogResult.OK)
                {
                    strRemarks = ibf.Remarks;
                }
                else
                {
                    return false;
                }
            }
            strRemarks = string.Format("{0}:({1})", strRemarks, extraRemarks);
            // 记录表的值
            DataRecord dataRecord = DataRecord.GetDataRecord();
            dataRecord.Conn = m_conn; // sql连接            
            dataRecord.CurrentNode = m_lua["SelectedNode"] as TreeNode; // 树结点
            dataRecord.Time = DateTime.Now; // 当前修改时间
            dataRecord.UserName = Helper.GetHostName(); // 当前用户名
            dataRecord.Remarks = strRemarks;
            dataRecord.Save();
            return true;
        }
        /// <summary>
        /// 添加修改记录
        /// </summary>
        /// <param name="field"></param>
        /// <param name="oldvalue"></param>
        /// <param name="newvalue"></param>
        /// <returns></returns>
        private  bool AddDataRecord(string extraRemarks, object oldvalue, object newvalue)
        {
            //设置记录备注
            string strRemarks = "未添加备注";
            if (Program.m_bCommentOnSave)
            {
                InputBoxForm ibf = new InputBoxForm();
                if (ibf.ShowDialog() == DialogResult.OK)
                {
                    strRemarks = ibf.Remarks;
                }
                else
                {
                    return false;
                }
            }
            strRemarks = string.Format("{0}:({1})", strRemarks, extraRemarks);
            string oldValue = oldvalue.ToString();
            string newValue = newvalue.ToString();
            if (oldvalue is bool) // 检查是否是真假值，显示值统一换为是和否
            {
                oldValue = DataRecord.TranslateValue(oldValue);
                newValue = DataRecord.TranslateValue(newValue);
            }
            oldValue = oldValue.Replace("'", "''");
            newValue = newValue.Replace("'", "''");
            
            // 记录表的值
            DataRecord dataRecord = DataRecord.GetDataRecord();
            dataRecord.Conn = m_conn; // sql连接            
            dataRecord.CurrentNode = m_lua["SelectedNode"] as TreeNode; // 树结点
            dataRecord.Time = DateTime.Now; // 当前修改时间
            dataRecord.UserName = Helper.GetHostName(); // 当前用户名
            dataRecord.OldValue = oldValue; // 字段的旧值        
            dataRecord.NewValue = newValue; // 字段的新值
            dataRecord.Remarks = strRemarks;
            dataRecord.Save();
            return true;
        }

        public void SqlUpdateRecords(string tblname, string field, object oldvalue, object newvalue)
        { 
            m_conn.Open();
            try
            {
                SqlCommand cmd = Conn.CreateCommand();
                string bracket = (oldvalue is string) ? "'" : string.Empty;
                cmd.CommandText = string.Format("UPDATE {0} SET {1}={2}{3}{4} WHERE {5}={6}{7}{8}",
                    tblname, field, bracket, newvalue, bracket, field, bracket, oldvalue, bracket);
                //cmd.ExecuteNonQuery();
                Helper.ExecuteCommand(tblname, cmd.CommandText, Conn);
            }
            catch (Exception ex)
            {
                string err = string.Format("在更新表 {0} 时发生错误。\r\n\r\n错误信息：{1}", tblname, ex.Message);
                MessageBox.Show(err);
            }
            m_conn.Close();
        }
       
        public DataRow SqlUpdateRecord_Native(string tblname, object[] keys, string[] fields, object[] values)
        {
            if (fields.Length != values.Length)
            {
                MessageBox.Show("函数参数错误, 在SqlUpdateRecord_Native.");
                return null;
            }
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = "SELECT * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            DataRow row = tbl.Rows.Find(keys);

            for (int i = 0; i < fields.Length; i++)
            {
                switch (tbl.Columns[fields[i]].DataType.Name)
                {
                    case "Decimal":
                        row[fields[i]] = -2.24159;
                        break;
                    case "Numeric":
                        row[fields[i]] = -2.24159;
                        break;
                    case "Int32":
                        row[fields[i]] = -1;
                        break;
                    case "String":
                        row[fields[i]] = "_ahpho";
                        break;
                    default:
                        break;
                }
            }

            for (int i = 0; i < fields.Length; i++)
                row[fields[i]] = values[i];

            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sql_tbl, Conn);

            return row;
        }

        public void SqlUpdateRecordUsingSql(string tblname, string keyname, string key, object fields, object newvalues)
        {          
            LuaTable ltFields = fields as LuaTable;
            LuaTable ltNewValues = newvalues as LuaTable;

            if (ltFields == null || ltNewValues == null)
                return;

            if (m_conn.State != ConnectionState.Open)
                m_conn.Open();
            try
            {
                

                string strSET = string.Format("SET ");
                for (int i = 0; i < ltFields.Values.Count; i++)
                {
                    string s = string.Format("{0}='{1}'", ltFields[i], ltNewValues[i]);
                    strSET += s;
                    if (i != ltFields.Values.Count - 1)
                    {
                        strSET += ", ";
                    }
                }
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = string.Format("UPDATE {0} {1} WHERE {2}={3}", tblname, strSET, keyname, key);
                cmd.ExecuteNonQuery();
                */
                string cmd = string.Format("UPDATE {0} {1} WHERE {2}={3}", tblname, strSET, keyname, key); ;
                Helper.ExecuteCommand(tblname, cmd, Conn);

            }
            catch (Exception ex)
            {
                string err = string.Format("在更新表 {0} 时发生错误。\r\n\r\n错误信息：{1}", tblname, ex.Message);
                MessageBox.Show(err);
            }
            if (m_conn.State != ConnectionState.Closed)
                m_conn.Close();
        }

        /// <summary>
        /// 插入新的记录
        /// </summary>
        /// <param name="tableName">数据表名称</param>
        /// <param name="fields">列名hash表</param>
        /// <param name="values">数值hash表</param>
        /// <returns>插入后获得的主键</returns>
        public object[] SqlInsertRecordWithoutKey(string tableName, object fields, object values)
        {
            object[] primaryKeys; // 插入后获得的主键
            List<string> columns = new List<string>(); // 列名链表
            List<string> datas = new List<string>(); // 数值链表

            if (fields is LuaTable)
            {
                foreach (object o in ((LuaTable)fields).Values)
                {
                    columns.Add(o.ToString());
                }
            }
            else
            {
                MessageBox.Show("错误的输入参数! 函数: SqlInsertRecordWithoutKey");
                return null;
            }

            if (values is LuaTable)
            {
                foreach (object o in ((LuaTable)values).Values)
                {
                    datas.Add(o.ToString());
                }
            }
            else
            {
                MessageBox.Show("错误的输入参数! 函数: SqlInsertRecordWithoutKey");
                return null;
            }

            try
            {
                // 获得DataTable
                /* remoting
                SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tableName, m_conn);
                DataSet ds = new DataSet();
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                adp.Fill(ds);
                DataTable tbl = ds.Tables[0];
                */

                string sql_tbl = "SELECT * FROM " + tableName;
                DataTable tbl = Helper.GetDataTableProxy(tableName, Conn);


                DataRow newRow = tbl.NewRow();

                // 获得默认行
                DataRow defaultRow = null;
                List<string> keyValues = new List<string>();

                if (tbl.PrimaryKey.Length > 0)
                {
                    object[] default_keys = new object[tbl.PrimaryKey.Length];
                    for (int i = 0; i < default_keys.Length; i++)
                    {
                        default_keys[i] = 0;
                        keyValues.Add(tbl.PrimaryKey[i].ColumnName);
                    }
                    defaultRow = tbl.Rows.Find(default_keys);
                }
                else
                {
                    string err = string.Format("在函数 SqlInsertRecordWithoutKey 调用时发生错误，数据表没有主键。");
                    MessageBox.Show(err);
                    return null;
                }

                if (defaultRow == null)
                {
                    defaultRow = tbl.Rows[0];
                }

                bool bHasDefualtRow = Helper.HasTableDefaultRow(tableName, Conn);
                // 填充默认行的值
                newRow.BeginEdit();
                foreach (DataColumn col in tbl.Columns)
                {
                    if (!keyValues.Contains(col.ColumnName)) // 主键值不能使用默认行的值
                    {
                        if (bHasDefualtRow)
                            newRow[col] = defaultRow[col];
                    }
                }

                // 更新数据
                for (int i = 0; i < columns.Count; i++)
                {
                    newRow[columns[i]] = datas[i];
                }
                newRow.EndEdit();

                // 提交
                tbl.Rows.Add(newRow);

                /*
                SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                adp.InsertCommand = cmdBuilder.GetInsertCommand();
                int val = adp.Update(tbl);
                tbl.AcceptChanges();
                */

                Helper.SaveTable(tbl, sql_tbl, Conn);

                primaryKeys = new object[keyValues.Count];
                for (int i = 0; i < keyValues.Count; i++)
                {
                    primaryKeys[i] = newRow[keyValues[i]];
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("在更新数据表{0}时产生sql异常: ", tableName) + ex.ToString());
                primaryKeys = null;
            }

            return primaryKeys;
        }



        public void SqlUpdateRecordAsCertainRow(string tblname, object[] des_keys, object[] src_keys)
        {
            // 获得这个表的分类字段有哪些
            string sql = string.Format("SELECT * FROM sys_modl_tab_def WHERE tablename='{0}'", tblname);
            //DataTable cat_tbl = Helper.GetDataTable(sql, m_conn);

            string filter = string.Format("tablename='{0}'", tblname);
            DataTable cat_tbl = Helper.GetDataTableProxy("sys_modl_tab_def", filter, null, Conn);

            DataRow cat_row = cat_tbl.Rows[0];
            string[] cat_fields = cat_row["catfields"].ToString().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

            string sql_tbl = "SELECT * FROM " + tblname;
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            DataRow des_row = tbl.Rows.Find(des_keys);
            DataRow src_row = tbl.Rows.Find(src_keys);
            if (des_row == null || src_row == null)
                return;

            foreach (DataColumn col in tbl.Columns)
            {
                bool bColIsKeys = false;
                foreach (DataColumn c in tbl.PrimaryKey)
                {
                    if (col.ColumnName == c.ColumnName)
                    {
                        bColIsKeys = true;
                        break;
                    }
                }

                bool bColIsCats = false;
                foreach (string cat in cat_fields)
                {
                    if (col.ColumnName == cat)
                    {
                        bColIsCats = true;
                        break;
                    }
                }

                bool bColIsDisplayField = (col.ColumnName == DisplayField);

                if (!bColIsKeys && !bColIsCats && !bColIsDisplayField) // 不拷贝 主键 分类字段 显示字段
                {
                    des_row[col.ColumnName] = src_row[col.ColumnName];
                }
            }
            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            int val = Helper.SaveTable(tbl, sql_tbl, Conn);
        }

        public void SqlInsertRecordAsCertainRow(string tblname, object keys, object[] copyRowKeys)
        {
            // 获得DataTable
            string sql = "SELECT * FROM " + tblname;
            /* remoting
            SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM " + tblname, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);
            
            DataRow newRow = tbl.NewRow();

            // 编辑此行
            newRow.BeginEdit();
            DataRow copyRow = tbl.Rows.Find(copyRowKeys);
            foreach (DataColumn col in tbl.Columns)
                newRow[col] = copyRow[col];
            for (int i = 0; i < tbl.PrimaryKey.Length; i++)
            {
                if (keys is LuaTable)
                    newRow[tbl.PrimaryKey[i]] = ((LuaTable)keys)[i];
                else if (keys is object[])
                    newRow[tbl.PrimaryKey[i]] = ((object[])keys)[i];
                else
                    MessageBox.Show("错误的输入参数，函数：SqlInsertRecordAsCertainRow");
            }
            newRow.EndEdit();

            // 提交
            tbl.Rows.Add(newRow);

            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.InsertCommand = cmdBuilder.GetInsertCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */

            Helper.SaveTable(tbl, sql, Conn);
        }

       
        public int SqlRemoveRecords(string tblname, string field, object value)
        {
            /* remoting
            string sql = string.Format("SELECT * FROM {0} WHERE {1}={2}", tblname, field, value);
            SqlDataAdapter adp = new SqlDataAdapter(sql, m_conn);
            DataSet ds = new DataSet();
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            */

            string sql_tbl = string.Format("SELECT * FROM {0} WHERE {1}={2}", tblname, field, value);
            string filter = string.Format("{0}={1}", field, value);

            DataTable tbl = Helper.GetDataTableProxy(tblname, filter, null, Conn);

            foreach (DataRow rowToDel in tbl.Rows)
                rowToDel.Delete();
            /* remoting
            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
            adp.DeleteCommand = cmdBuilder.GetDeleteCommand();
            int val = adp.Update(tbl);
            tbl.AcceptChanges();
            */
            int val = Helper.SaveTable(tbl, sql_tbl, Conn);

            return val;
        }

        public int NewSqlRemoveRecord(string tblname, object keys, object keynames)
        {
            object[] _keys = (object[])keys;
            object[] _keynames = (object[])keynames;
            string sqlcondition = string.Empty;
            for (int i = 0; i < _keys.Length; i++)
            {
                sqlcondition += _keynames[i].ToString().Trim() + "=" + _keys[i].ToString().Trim();
                sqlcondition += " and ";
            }
            sqlcondition = sqlcondition.Substring(0, sqlcondition.Length - 5);

            string sql = string.Format("delete from {0} where {1}", tblname, sqlcondition);
            return Helper.ExecuteCommand(sql, Conn);
        }

        public int SqlRemoveRecord(string tblname, object keys)
        {
            string sql_tbl = "SELECT * FROM " + tblname;
            DataTable tbl = Helper.GetDataTableProxy(tblname, Conn);

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            if (keys is LuaTable)
            {
                for (int i = 0; i < KEYS.Length; i++)
                {
                    KEYS[i] = ((LuaTable)keys)[i];
                }
            }
            else if (keys is object[])
            {
                KEYS = (object[])keys;
            }
            DataRow rowToDel = tbl.Rows.Find(KEYS);
            if (rowToDel != null)
                rowToDel.Delete();

            int val = Helper.SaveTable(tbl, sql_tbl, Conn);
            return val;
        }

        public bool ObjectsEqual(object obj1, object obj2)
        {
            if (obj1 is object[] && obj2 is object[])
            {
                object[] aobj1 = (object[])obj1;
                object[] aobj2 = (object[])obj2;
                if (aobj1.Length == aobj2.Length)
                {
                    bool allEquals = true;
                    for (int i = 0; i < aobj1.Length; i++)
                    {
                        if (aobj1[i].ToString() != aobj2[i].ToString())
                        {
                            allEquals = false;
                            break;
                        }
                    }
                    if (allEquals)
                    {
                        return true;
                    }
                }

                return false;
            }
            else if (obj1.ToString() == obj2.ToString())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        public object[] ObjToObjArr(object obj)
        {
            return new object[] { obj };
        }
        public object[] ObjToObjArr2(object obj1, object obj2)
        {
            return new object[] { obj1, obj2 };
        }
        public object[] ObjToObjArr3(object obj1, object obj2, object obj3)
        {
            return new object[] { obj1, obj2, obj3 };
        }

        public DataRow FindRowInTable(DataTable tbl, object[] keys)
        {
            return tbl.Rows.Find(keys);
        }

        public ArrayList FindAllTreeNodesByLevel(int iLevel)
        {
            ArrayList ls = new ArrayList();
            if (iLevel == -1)
            {
                TreeNode node = new TreeNode();
                node.Name = "root";
                ls.Add(node);
                return ls;
            }
            else
            {
                GetAll(this.baseTree.Nodes, ref ls, iLevel);
                return ls;
            }
        }

        public object DataBaseNull()
        {
            return DBNull.Value;
        }

        public int ConvertToInt32(object number)
        {
            int iRet = 0;
            try
            {
                iRet = Convert.ToInt32(number);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            return iRet;
        }

        public void ForceRecordChanged()
        {
            m_bRecordChanged = true;
        }
      
        public string QueryMetaComboText(string tblName, string fieldName, int key)
        {
            string sql = string.Format("SELECT * FROM sys_meta_info WHERE tablename='{0}' AND fieldname='{1}'", tblName, fieldName);
            
            string filter = string.Format("tablename='{0}' AND fieldname='{1}'", tblName, fieldName);

            //DataTable tblMetaInfo = Helper.GetDataTable(sql, Conn);

            DataTable tblMetaInfo = Helper.GetDataTableProxy("sys_meta_info", filter, null, Conn);

            DataRow row = tblMetaInfo.Rows[0];
            string foundComboText = string.Empty;
            switch (row["editortype"].ToString().Trim())
            {
                case "lookupcombo":
                    break;

                case "textcombo":
                    {
                        string listvalues = row["listvalues"].ToString().Trim();
                        string[] lines = listvalues.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                        // 找到指定的listvalues的key
                        foreach (string str in lines)
                        {
                            string[] s = str.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                            if (s.Length != 2)
                            {
                                MessageBox.Show("listvalues错误, 请检查表元数据.");
                                break;
                            }
                            if (s[0].Trim() == key.ToString())
                            {
                                foundComboText = s[1];
                                break;
                            }
                        }
                    }
                    break;

                default:
                    break;
            }
            return foundComboText;
        }

        public void AdjustPortraitRecordsCountToNpc()
        {
            //DataTable tblPort = Helper.GetDataTable("SELECT * FROM tbl_npc_portrait", Conn);
            DataTable tblPort = Helper.GetDataTableProxy("tbl_npc_portrait", Conn);
            try
            {
                string sql_tblNpc = "SELECT * FROM tbl_npc_portrait";
                /* remoting
                SqlDataAdapter adp = new SqlDataAdapter("SELECT * FROM tbl_npc_portrait", Conn);
                DataTable tblNpc = Helper.GetDataTable("SELECT RepresentID, map, Model FROM npc", Conn);
                */

                DataTable tblNpc = Helper.GetDataTableProxy("tbl_npc_portrait", Conn);

                DataRow defaultRow = tblPort.Rows[0];
                bool bNeedUpdateDB = false;

                bool bHasDefualtRow = Helper.HasTableDefaultRow("npc", Conn);
                // 从 npc 表添加数据到 portrait
                foreach (DataRow row in tblNpc.Rows)
                {
                    int Id = Convert.ToInt32(row["RepresentID"]);
                    if (tblPort.Rows.Find(Id) == null)
                    {
                        bNeedUpdateDB = true;
                        DataRow newrow = tblPort.NewRow();
                        newrow.BeginEdit();
                        if (bHasDefualtRow)
                            foreach (DataColumn col in tblPort.Columns)
                            {
                                newrow[col] = defaultRow[col]; //默认行
                            }
                        newrow["RepresentID"] = row["RepresentID"];
                        newrow["Name"] = row["Model"];
                        newrow["Map"] = row["map"];
                        newrow.EndEdit();
                        tblPort.Rows.Add(newrow);
                    }
                }

                // 删除 tbl_portrait 中的无用行
                int iDelCount = 0;
                ArrayList toDelList = new ArrayList();
                foreach (DataRow row in tblPort.Rows)
                {
                    int Id = Convert.ToInt32(row["RepresentID"]);
                    if (tblNpc.Rows.Find(Id) == null)
                    {
                        toDelList.Add(row);
                    }
                }
                foreach (DataRow row in toDelList)
                {
                    iDelCount++;
                    row.Delete();
                    bNeedUpdateDB = true;
                }

                // 提交
                if (bNeedUpdateDB)
                {
                    /*
                    SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(adp);
                    adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = adp.Update(tblPort);
                    tblPort.AcceptChanges();
                    */

                    int val = Helper.SaveTable(tblPort, sql_tblNpc, Conn);
                }
            }
            catch //(Exception ex)
            {
                tblPort.RejectChanges();
            }
        }

        public void ReloadTree()
        {
            TreeNode beforeSelectedNode = (TreeNode)this.BaseTree.SelectedNode.Clone();
            LoadTree();
            foreach (TreeNode node in this.BaseTree.Nodes)
            {
                TreeNode nodeReturn = FindNodeByTagAndText(node, beforeSelectedNode);
                if (nodeReturn != null)
                {
                    this.BaseTree.SelectedNode = nodeReturn;
                    break;
                }
            }
        }

        public object[] GetUnusedID()
        {
            string sql = string.Format("SELECT * FROM {0}", MainTableName);
            //DataTable tbl = Helper.GetDataTable(sql, Conn);
            DataTable tbl = Helper.GetDataTableProxy(MainTableName, Conn);
            if (Program.EnableRemoting && MainTableName.Length > 7 && MainTableName.Substring(0, 7) == "RTTEST_")
            {
                DataTableProxy proxy = DataTableServerProxy.GetDataTableProxy();
                StartIDs = proxy.UTGetTableID(MainTableName, StartIDs);
            }
            else
            {
                while (FindRowInTable(tbl, StartIDs) != null)
                {
                    StartIDs[0] = (int)(StartIDs[0]) + 1;
                }
            }
            if (CatFieldsContainsKey) // 如果分类字段中有key
            {
                StartIDs[ToChangeIndex] = (int)(StartIDs[ToChangeIndex]) + 1;
            }

            SetLastID(MainTableName, TblMain.PrimaryKey[0].ColumnName, Convert.ToInt32(StartIDs[0]));
            if (CatFieldsContainsKey)
            {
                SetLastID(MainTableName, TblMain.PrimaryKey[ToChangeIndex].ColumnName, Convert.ToInt32(StartIDs[ToChangeIndex]));
            }

            return StartIDs;
        }

        public object[] GetUnusedID_SpecifyScope(int iBase) // 生成没有用过的, 第一个key比iBase大的id
        {
            string sql = string.Format("SELECT * FROM {0}", MainTableName);
            //DataTable tbl = Helper.GetDataTable(sql, Conn);
            DataTable tbl = Helper.GetDataTableProxy(MainTableName, Conn);
            if (Program.EnableRemoting && MainTableName.Length > 7 && MainTableName.Substring(0, 7) == "RTTEST_")
            {
                DataTableProxy proxy = DataTableServerProxy.GetDataTableProxy();
                if ((int)StartIDs[0] <= iBase)
                {
                    StartIDs[0] = iBase + 1;
                }
                StartIDs = proxy.UTGetTableID(MainTableName, StartIDs);
            }
            else
            {
                while (FindRowInTable(tbl, StartIDs) != null || (int)StartIDs[0] <= iBase)
                {
                    StartIDs[0] = (int)(StartIDs[0]) + 1;
                }
            }
            if (CatFieldsContainsKey) // 如果分类字段中有key
            {
                StartIDs[ToChangeIndex] = (int)(StartIDs[ToChangeIndex]) + 1;
            }

            SetLastID(MainTableName, TblMain.PrimaryKey[0].ColumnName, Convert.ToInt32(StartIDs[0]));
            if (CatFieldsContainsKey)
            {
                SetLastID(MainTableName, TblMain.PrimaryKey[ToChangeIndex].ColumnName, Convert.ToInt32(StartIDs[ToChangeIndex]));
            }

            return StartIDs;
        }
    };
}