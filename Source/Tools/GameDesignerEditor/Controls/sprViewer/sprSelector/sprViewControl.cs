using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using atlaxLib;


namespace sprSelector
{
    public class sprViewControl
    {
        frmSelect fs = null;

        public static string m_strBase = string.Empty;

        private static System.Data.DataTable GetDataTable(string sql, System.Data.SqlClient.SqlConnection conn)
        {
            System.Data.SqlClient.SqlDataAdapter adp = new System.Data.SqlClient.SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = System.Data.MissingSchemaAction.AddWithKey;
            System.Data.DataSet ds = new System.Data.DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        public string show4ff_new_socket(int representid, string socketname, string strBasePath, string strBrowsePath, string strDefaultFile, Test3DEngine engine, string strFilter, SqlConnection conn)
        {
            string sql = string.Format("SELECT FileName FROM dic_npc_socket_desc WHERE RepresentID = {0} AND Socket = '{1}'", representid, socketname);
            DataTable tbl = GetDataTable(sql, conn);

            if (null == fs)
            {
                fs = new frmSelect(engine, conn);
            }

            fs.Label3.Enabled = true;
            fs.ButtonX5.Enabled = true;
            fs.ComboBox1.Enabled = true;

            fs.ComboBox1.Items.Clear();
            foreach (DataRow row in tbl.Rows)
            {
                fs.ComboBox1.Items.Add(row[0].ToString());
            }

            return show4ff_new(strBasePath, strBrowsePath, strDefaultFile, engine, strFilter, conn);
        }

        public string show4ff_new(string strBasePath, string strBrowsePath, string strDefaultFile, Test3DEngine engine, string strFilter, SqlConnection conn)
        {
            m_strBase = strBasePath;
            sprViewer.SPRItem.IconFolder = strBasePath;

            if (null == fs)
            {
                fs = new frmSelect(engine, conn);
            }

            fs.strSelectedFileName = strDefaultFile;
            fs.Text = "请选择文件";
            fs.nQueMax = 10;
            switch (strFilter.ToLower())
            {
                case "mdl,mesh":fs.strFilter = ";.mesh;.mdl;";              break;
                case "mesh":    fs.strFilter = ";.mesh;";                   break;
                case "mdl":     fs.strFilter = ";.mdl;";                    break;
                case "ani":     fs.strFilter = ";.ani;.tani;";              break;
                default:        throw new Exception("unrecognized type.");
            }
            fs.showFolder = true;
            fs.LoadFolder(strBrowsePath);
            System.Windows.Forms.DialogResult r = fs.ShowDialog();
            if (r == System.Windows.Forms.DialogResult.OK)
            {
                string srt = fs.strSelectedFileName.Replace(strBasePath, string.Empty);
                //fs.Hide();
                return srt;
            }
            else
            {
                //fs.Hide();
                return null;
            }
        }

        public string show4ff(string strPath, string strDefaultFile, Test3DEngine engine, SqlConnection conn)
        {
            if( null == fs )
            {
                fs = new frmSelect(engine, conn);
            }
            fs.strSelectedFileName = strDefaultFile;
            fs.Text = "请选择文件";
            fs.nQueMax = 10;
            //fs.strFilter = ";.mdl;.mtl;.ani;.mesh;";
            fs.strFilter = ";.mdl;.ani;.tani;.mesh;";
            fs.showFolder = true;
            fs.LoadFolder(strPath);
            System.Windows.Forms.DialogResult r = fs.ShowDialog();
            if (r == System.Windows.Forms.DialogResult.OK)
            {
                string srt = fs.strSelectedFileName;
                //fs.Hide();
                return (fs.strSelectedFileName);
            }
            else
            {
                //fs.Hide();
                return null;
            }
        }

        public string show(string strPath)
        {
            return (show(strPath, "请选择SPR文件","",10));
        }

        public string show(string strPath, string strTitle, string strDefaultFile)
        {
            return (show(strPath, strTitle, strDefaultFile, 10));
        }

        public string show(string strPath, string strTitle, string strDefaultFile, int nQueMax)
        {
            if( null == fs )
            {
                fs = new frmSelect();
            }
            fs.strSelectedFileName = strDefaultFile;
            fs.Text = strTitle;
            fs.nQueMax = nQueMax;
            fs.strFilter = ".spr";
            fs.LoadFolder(strPath);
            System.Windows.Forms.DialogResult r = fs.ShowDialog();
            if (r == System.Windows.Forms.DialogResult.OK)
            {
                string srt = fs.strSelectedFileName;
                fs.Hide();
                return (fs.strSelectedFileName);
            }
            else
            {
                fs.Hide();
                return null;
            }
        }
    }
}
