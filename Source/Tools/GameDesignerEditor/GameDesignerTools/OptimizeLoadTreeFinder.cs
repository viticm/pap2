using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;

using System.Data;
using System.Data.SqlClient;

namespace GameDesingerTools
{
    class OptimizeLoadTreeFinder
    {
        private string FindField = string.Empty;
        private string FindValue = string.Empty;
        private bool FindByNodeText = false;
        private TreeView treeView = null;       
        private DataRowCollection FindResultRows = null;
       
        private SqlConnection Conn;
       
        private int CurIndex = -1;


        private string MainTableName = string.Empty;

        string[] CatFields = null;
        string LeafField = string.Empty;
        DataColumn[] PrimaryColumns;
        int ModTabId = 0;
          
        ArrayList arrRepeated = new ArrayList();
        const string WhichField = "whichfield";

        bool bHasErr = false;


        public void Init(string strField, string strValue, bool bFindByNodeText, TreeView tree, SqlConnection conn,
            string[] catFields, string maintablename, string leafField, DataColumn[] primaryColumn, int modtabid)
        {
            if (FindField != strField || FindValue != strValue || FindByNodeText != bFindByNodeText || MainTableName != maintablename)
            {
                FindField = strField;
                FindValue = strValue;
                treeView = tree;
                Conn = conn;
                FindByNodeText = bFindByNodeText;
                MainTableName = maintablename;  
                CatFields = catFields;
                LeafField = leafField;
                PrimaryColumns = primaryColumn;
                ModTabId = modtabid;              


                CurIndex = -1;
                FindResultRows = null;
                arrRepeated.Clear();

                bHasErr = !ReInit();
            }            
        
        }    
       
        private string SelectFields
        {
            get 
            {
                string strRet = ",";
                for (int i = 0; i < CatFields.Length; ++i)
                {
                    if (CatFields[i].Trim() != "")
                    {
                        strRet += string.Format("[{0}],", CatFields[i]);
                    }                    
                }
                if (!strRet.Contains(string.Format(",[{0}],", LeafField)))
                {
                    if (LeafField.Trim() != "")
                    {
                        strRet += string.Format("[{0}],", LeafField);
                    }
                }
                if (strRet != "")
                {
                    foreach (DataColumn column in PrimaryColumns)
                    {
                        if (!strRet.Contains(string.Format(",[{0}],", column.ColumnName)))
                        {
                            strRet += string.Format("[{0}],", column.ColumnName);
                        }
                    }
                }
                if (strRet.Length > 1)
                {
                    strRet = strRet.Substring(1, strRet.Length - 2);
                }
                else
                {
                    strRet = string.Empty;
                }

                return strRet;
            }
        }
        
        private string GetSql()
        {
            string sqlRet = string.Empty;            
            string strWhere = string.Empty;
            string strWhereNotEmpty = string.Empty;

            bool bCatContainLeaf = false;
            string strCase = string.Empty;
            string OneConditon = string.Empty;
         

            foreach(string field in CatFields)
            {
                if (field.Trim() == "")
                {
                    continue;
                }
                OneConditon = GetFieldFieterCondition(field); 
                if (field == LeafField)
                {
                    bCatContainLeaf = true;
                }
                strWhere += "(" + OneConditon + ") or";
                strWhereNotEmpty += string.Format("({0} is not null ) and", field);
                strCase = string.Format("when {0} then '{1}'", OneConditon, field) + strCase;
            }
            if (!bCatContainLeaf)
            {
                OneConditon = GetFieldFieterCondition(LeafField);
                strWhere += "(" + OneConditon + ")";
                strWhereNotEmpty += string.Format("({0} is not null )", LeafField);
                strCase = string.Format("when {0} then '{1}'", OneConditon, LeafField) + strCase;
            }
            else
            {
                strWhere = strWhere.Substring(0, strWhere.Length - 3);
                strWhereNotEmpty = strWhereNotEmpty.Substring(0, strWhereNotEmpty.Length - 4);
            }

            strCase = string.Format("case {0} else 'wrong' end as {1}", strCase, WhichField);
          
            sqlRet = string.Format("select {0},{1} from {2} where ({3}) and ({4}) order by {0}", SelectFields, strCase, MainTableName, strWhere, strWhereNotEmpty);

            return sqlRet;           
            
        }
        private string GetFieldFieterCondition(string fieldName)
        {
            string strConditionRet = string.Empty;
            string sql = string.Format("SELECT * FROM sys_meta_info WHERE tablename='{0}' AND fieldname='{1}' AND modtabid={2}", MainTableName, fieldName, ModTabId);
            DataTable dtMetaInfo = GetDataTable(sql, Conn);            
            
            if(dtMetaInfo.Rows.Count > 0)
            {
                DataRow r = dtMetaInfo.Rows[0];
                string editortype = r["editortype"].ToString().Trim();
                const string OR = " or ";
                switch (editortype)
                {
                    case "customeditor":
                    case "lookupcombo":
                        string keyfield = r["keyfield"].ToString().Trim();
                        string listfield = r["listfield"].ToString().Trim();

                        string tblname = r["listtable"].ToString().Trim();
                        string strCond = "";
                        if (r["listcondition"].ToString().Trim() != "")
                        {
                            strCond = string.Format(" where ({0}) and {1} like '%{2}%'", r["listcondition"], listfield, FindValue);
                        }
                        else
                        {
                            strCond = string.Format(" where {0} like '%{1}%'", listfield, FindValue);
                        }
                           
                        sql = string.Format("SELECT [{0}] FROM {1} {2}", keyfield, tblname, strCond) ;

                        strConditionRet = string.Format("[{0}] in ({1})", fieldName, sql);
                       
                        break;
                    case "textcombo":
                        string listvalue = r["listvalues"].ToString().Trim();

                        string[] texts = listvalue.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                        foreach (string text in texts)
                        {
                            string[] s = text.Split(new char[] { ',' });
                            if (s != null && s.Length == 2)
                            {
                                if (s[1].Contains(FindValue))
                                {
                                    strConditionRet += string.Format("[{0}] = '{1}'{2}", fieldName, s[0], OR);
                                }
                            }
                            else
                            {
                                MessageBox.Show("错误的listvalues，请修改表元数据。");
                                throw new Exception("错误的 listvalues 格式!");
                            }
                        }
                        if (strConditionRet.Contains(OR))
                        {
                            strConditionRet = strConditionRet.Substring(0, strConditionRet.Length - OR.Length);
                        }
                        if (strConditionRet == string.Empty)
                        {
                            int NotExistValue = -100;
                            strConditionRet = string.Format("[{0}] = '{1}'", fieldName, NotExistValue);
                        }
                      
                        break;
                    default:
                        strConditionRet = string.Format("[{0}] like '%{1}%'", fieldName, FindValue);
                        break;
                }
            }
            else
            {
                strConditionRet = string.Format("[{0}] like '%{1}%'", fieldName, FindValue);
            }
            return strConditionRet;

        }
       
       
        private bool ReInit()
        { 
            string sql = string.Empty;
            DataTable dt = null;
           
            if (SelectFields == string.Empty)
            {
                MessageBox.Show("分类字段和显示字段都为空");
                return false;
            }
            
            if (FindByNodeText)
            {               
                sql = GetSql();             
            }           
            else
            {
                bool bNotLike = true;
                int nTemp = 0;
                bNotLike = int.TryParse(FindValue, out nTemp);

                if (bNotLike)//全部匹配
                {
                    sql = string.Format("select {0} from {1} where [{2}] = '{3}' order by {0}", SelectFields, MainTableName, FindField, FindValue);
                }
                else//模糊匹配
                {
                    sql = string.Format("select {0} from {1} where [{2}] like '%{3}%' order by {0}", SelectFields, MainTableName, FindField, FindValue);
                }
             
            }
            dt = GetDataTable(sql, Conn);
            if (dt == null)
            {
                return false;
            }
            FindResultRows = dt.Rows;

            return true;
        }

        public void FindNext()
        {
            if (bHasErr || !FindTip(FindResultRows.Count))
            {
                return;
            }
            if (CurIndex == FindResultRows.Count - 1)
            {
                arrRepeated.Clear();
            }
            CurIndex = (CurIndex + 1) % FindResultRows.Count;
            string theFindField = string.Empty;
            string theLastValue = string.Empty;
            string theResultKey = string.Empty;
            if (FindByNodeText)
            {
                theFindField = FindResultRows[CurIndex][WhichField].ToString();
                theLastValue = FindResultRows[CurIndex][theFindField].ToString();
                theResultKey = string.Format("{0}:{1}", theFindField, theLastValue);
                foreach (string field in CatFields)
                {
                    if (field == theFindField)
                    {
                        break;
                    }
                    if (field != string.Empty)
                    {
                        theResultKey = string.Format("{0}:{1}+{2}", field, FindResultRows[CurIndex][field], theResultKey);
                    }

                }

                    while (theFindField != LeafField && arrRepeated.Contains(theResultKey))
                    {
                        if (CurIndex == FindResultRows.Count - 1)
                        {
                            arrRepeated.Clear();
                        }
                        CurIndex = (CurIndex + 1) % FindResultRows.Count;
                        theFindField = FindResultRows[CurIndex][WhichField].ToString();
                        theLastValue = FindResultRows[CurIndex][theFindField].ToString();
                        theResultKey = string.Format("{0}:{1}", theFindField, theLastValue);
                        foreach (string field in CatFields)
                        {
                            if (field == theFindField)
                            {
                                break;
                            }
                            if (field != string.Empty)
                            {
                                theResultKey = string.Format("{0}:{1}+{2}", field, FindResultRows[CurIndex][field], theResultKey);
                            }
                            
                        }
                    }
                if (theFindField != LeafField)
                {
                    arrRepeated.Add(theResultKey);
                }
            }
            else
            {
                theFindField = FindField;
            }

            FindTreeNode(theFindField, FindResultRows[CurIndex]);
            
        }
        
        private void FindTreeNode(string endField, DataRow row)
        {
            string value;
            string field = string.Empty;
            TreeNode node = null;
            TreeNodeCollection tnc = treeView.Nodes;

            bool bFinded = false;

            int i = 0;
            for (i = 0; i < CatFields.Length; ++i)
            {
                field = CatFields[i];
                if (field.Trim() == "")
                {
                    continue;
                }
                if (field == LeafField)
                {
                    break;
                }
                value = row[field].ToString();
                object[] findTag = new object[1];
                findTag[0] = value;
                node = GetChildNode(tnc, findTag);
                if (node == null)
                {
                    return;
                }
                node.Expand();
                tnc = node.Nodes;
                if (field == endField)
                {
                    bFinded = true;
                    break;
                }
            }
            if (field == LeafField ||
                (!bFinded))
            {
                object[] findTag = new object[PrimaryColumns.Length];
                for (int j = 0; j < PrimaryColumns.Length; ++j)
                {
                    findTag[j] = row[PrimaryColumns[j].ColumnName].ToString();
                }
                node = GetChildNode(tnc, findTag);                  
            }
            if (node != null)
            {
                treeView.SelectedNode = node;
            }

        }      
       
        //找Tag等于指定文本的节点
        private TreeNode GetChildNode(TreeNodeCollection Nodes, object[] findTag)
        {
            TreeNode nodeRet = null;
            object[] nodeTag = null;
            int i = 0;
            foreach (TreeNode node in Nodes)
            {
                if (!node.Tag.GetType().IsArray)
                {
                    nodeTag = new object[1];
                    nodeTag[0] = node.Tag;
                }
                else
                {
                    nodeTag = node.Tag as object[];
                }                
                
                if (nodeTag.Length != findTag.Length)
                {
                    continue;
                }

                for ( i = 0; i < findTag.Length; i++)
                {
                    if (findTag[i].ToString() != nodeTag[i].ToString())
                    {
                        break;
                    }
                }
                if (i == findTag.Length)
                {
                    nodeRet = node;
                    break;
                } 
            }
            return nodeRet;
        }
       
        //查找结果提示
        private bool FindTip(int findCnt)
        {
            if (findCnt == 0)
            {
                MessageBox.Show("没有符合条件的节点");
                return false;
            }
            return true;
        }
    


        // db相关
        public System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
    }
}
