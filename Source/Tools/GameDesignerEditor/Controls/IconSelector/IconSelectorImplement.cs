using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Drawing;

using sprViewer;

namespace IconSelector
{

    public class Implement
    {
        private FormSelector m_formselector;
        private int m_id;
        private bool m_bEdit = false;
        private bool inited = false; // 是否已经初始化过
        private SqlConnection connection; // sql连接
        private string iconDirectory; // 图标所在目录
        private Helper helper = Helper.GetHelper();

        public int IconID
        {
            get { return m_id; }
            set { m_id = value; }
        }

        /// <summary>
        /// 是否已经初始化过
        /// </summary>
        public bool Inited
        {
            get
            {
                return inited;
            }
        }

        public string IconDirectory
        {
            set
            {
                iconDirectory = value;
            }
        }

        public SqlConnection Connection
        {
            set
            {
                connection = value;
            }
        }

        public bool Init()
        {
            return Init(connection, iconDirectory);
        }

        public void EnterEditMode()
        {
            m_bEdit = true;
        }

        public bool Init(SqlConnection Conn, string strIconDir)
        {
            // 初始化数据库连接及数据表
            helper.CurrentSqlConnection = Conn;
            helper.InitCacheTable();

            tgaIO.sqlConn = Conn;
            m_id = -1;
            tgaIO.StrIconFolder = strIconDir;
            m_formselector = new FormSelector();
            m_formselector.Tag = this;
            if (m_bEdit)
                m_formselector.EnterEditMode();
            FormScan form = new FormScan();
            object[] objs = form.Init(Conn, strIconDir, m_formselector.imageIcons);
            m_formselector.IconsInfo = objs;
            inited = true;

            if (objs.Length > 0)
            {
                form.ShowDialog(); //显示图像并创建ImageList
            }
            else
            {
                return false;
            }

            return true;
        }

        public int Show()
        {
            m_formselector.SelectedID = m_id;
            DialogResult r = m_formselector.ShowDialog();
            int nRet = m_formselector.SelectedID;
            if (r == DialogResult.OK)
                return nRet;
            else
                return -1;
        }
    }
   

    public static class tgaIO
    {
        public static string tga2rtf(int id, System.Data.SqlClient.SqlConnection conn, string strIconDir)
        {
            FormScan form = new FormScan();
            string filename = "";
            int frame = 0;

            //查询数据库查得文件名
            DataTable tbl = GetDataTable("select * from icon where id=" + id.ToString(), conn);
            if (tbl.Rows.Count == 0)
            {
                return ("");
            }
            DataRow row = tbl.Rows[0];
            filename = strIconDir + "\\" + row["filename"].ToString();
            frame = Convert.ToInt32(row["frame"]);
            if (!System.IO.File.Exists(filename))
            {
                return ("");
            }

            //根据文件名读出图形
            Image timg = sprViewer.tga.uitex2img(strIconDir, filename, frame);
            if(timg == null)
            {
                return("");
            }

            //备份剪切板
            //Object obj = new Object();
            //obj = Clipboard.GetDataObject();

            //恢复剪切板
            //Clipboard.SetDataObject(obj);


            //转换图像
            Clipboard.SetImage(timg);
            form.txt.Clear();
            form.txt.Paste();

            //返回结果
            form.txt.SelectAll();
            return form.txt.SelectedRtf;
        }


        public static SqlConnection sqlConn = null;
        public static string StrIconFolder = "";
        public static DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }
        
        //用于修改某个图标在数据库中的分类
        public static void ResetAllKind(string id, string newKind, string newSubKind)
        {
            if ( sqlConn == null ) return;
            sqlConn.Open();

            string sql = string.Format("UPDATE icon SET Kind = '{0}', SubKind = '{1}' WHERE ID = {2}",
                newKind, newSubKind, id);
            SqlCommand sqlCmd = new SqlCommand(sql, sqlConn);
            sqlCmd.ExecuteNonQuery();
            sqlConn.Close();
        }
        public static void ResetKind(string id, string newKind)
        {
            if ( sqlConn == null ) return;
            sqlConn.Open();

            string sql = string.Format("UPDATE icon SET Kind = '{0}' WHERE ID = {1}", newKind, id);
            SqlCommand sqlCmd = new SqlCommand(sql, sqlConn);
            sqlCmd.ExecuteNonQuery();
            sqlConn.Close();
        }
        public static void ResetSubKind(string id, string newSubKind)
        {
            if ( sqlConn == null ) return;
            sqlConn.Open();

            string sql = string.Format("UPDATE icon SET SubKind = '{0}' WHERE ID = {1}", newSubKind, id);
            SqlCommand sqlCmd = new SqlCommand(sql, sqlConn);
            sqlCmd.ExecuteNonQuery();
            sqlConn.Close();
        }

        //用于分类重命名
        public static void RnameType(string oldType, string newType)
        {
            if ( sqlConn == null ) return;
            sqlConn.Open();
            SqlCommand sqlCmd = new SqlCommand("update icon set kind='" + newType + "' where Kind='" + oldType + "'", sqlConn);
            sqlCmd.ExecuteNonQuery();
            sqlConn.Close();
        }

        //用于更新tag1和tag2
        public static void UpdateTag12(int[] ids, string fieldname, string strvalue)
        {
            if (sqlConn == null) return;
            sqlConn.Open();

            string sqlNewValue = strvalue.Length == 0 ? "NULL" : string.Format("'{0}'", strvalue);
            string sql = string.Format("update icon set {0}={1} where ", fieldname, sqlNewValue);
            foreach (int oneid in ids)
            {
                sql += "id=" + oneid.ToString() + " or ";
            }
            sql += "0=1";
            SqlCommand sqlCmd = new SqlCommand(sql, sqlConn);
            sqlCmd.ExecuteNonQuery();
            sqlConn.Close();
        }

        //添加新记录,返回ID
        public static int InsertNewRecord(string FileName, string tag1, string tag2)
        {
            return InsertNewRecord(FileName, tag1, tag2, sqlConn);
        }

        // 添加新记录,返回ID
        public static int InsertNewRecord(string FileName, string tag1, string tag2, SqlConnection Conn)
        {
            if (Conn == null) return 0;

            string sql = string.Format("SELECT FileName FROM icon WHERE FileName = '{0}'", FileName);
            DataTable tbl = GetDataTable(sql, Conn);
            if (tbl.Rows.Count > 0) return 0;

            Conn.Open();
            string strSQL = @"
begin transaction;
declare @newid int;
set @newid = (select max(id) from icon);
insert into icon(id, filename, frame, kind, subkind, tag1, tag2) values (
    @newid+1,'{0}','0','新资源','','{1}','{2}'
);
select max(id) from icon;
commit transaction;
";

            SqlCommand sqlCmd = new SqlCommand(string.Format(strSQL, FileName, tag1, tag2), Conn);
            int nRet = Convert.ToInt32(sqlCmd.ExecuteScalar().ToString());
            Conn.Close();
            return nRet;
        }
   };
}
