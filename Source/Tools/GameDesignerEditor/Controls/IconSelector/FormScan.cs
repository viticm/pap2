using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using IconBrowser;
using System.IO;


namespace IconSelector
{
    public partial class FormScan : Form
    {
        private string m_IconDir;
        private object[] m_iconsinfo;
        private ImageList m_imglist;
        public static SqlConnection Conn;

        public FormScan()
        {
            InitializeComponent();
        }
        
        public static DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, Conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }
        
        public object[] Init(SqlConnection conn, string strIconDir, ImageList imglist)
        {
            Conn = conn;
            m_imglist = imglist;
            m_IconDir = strIconDir;
            sprViewer.SPRItem.IconFolder = strIconDir;

            DataTable tbl = GetDataTable("select *,(select count(*) from tbl_icon_item where icon.id=tbl_icon_item.iconid) as c from icon");
            object[] objret = new object[tbl.Rows.Count];

            for(int i = 0; i < tbl.Rows.Count; i ++)
            {
                DataRow row = tbl.Rows[i];
                Info info = new Info();
                info.id  = int.Parse(row["ID"].ToString());
                info.usedcount = int.Parse(row["c"].ToString());
                info.fielname = row["filename"].ToString();
                info.frame = 0;
                if (row["frame"].ToString().Length != 0)
                    info.frame = int.Parse(row["frame"].ToString());
                info.kind = row["kind"].ToString();
                info.tag1 = row["tag1"].ToString();
                info.tag2 = row["tag2"].ToString();
                info.soundID = -1;

                if (row["SoundID"] != DBNull.Value)
                {
                    info.soundID = int.Parse(row["SoundID"].ToString());
                }

                info.subKind = row["SubKind"].ToString();
                if (info.kind == "")
                {
                    info.kind = "无类型";
                }

                objret[i] = info;
            }
            
            m_iconsinfo = objret;

            return objret;
        }
        
        private void FormScan_Load(object sender, EventArgs e)
        {
        }

        private void FormScan_Activated(object sender, EventArgs e)
        {
        }

        private void FormScan_Shown(object sender, EventArgs e)
        {
            // Application.DoEvents();
            probar1.Maximum = m_iconsinfo.Length;
            FormScan_StartScan();
        }

        private void FormScan_StartScan()
        {
            foreach (object obj in m_iconsinfo)
            {
                Info info = (Info)obj;
                string strfilename = string.Format(@"{0}\{1}", m_IconDir, info.fielname);

                if (File.Exists(strfilename))
                {
                    sprViewer.SPRItem si = new sprViewer.SPRItem(strfilename, info.frame);
                    Image iconImage = si.IconImage;

                    if (iconImage == null)
                    {
                        string filePath = si.TGAList["1"].ToString();

                        if (File.Exists(filePath))
                        {
                            try
                            {
                                iconImage = Image.FromFile(filePath);  
                            }
                            catch (Exception ex)
                            {
                                iconImage = null;
                            }
                        }                                                
                    }

                    if (iconImage != null)
                    {
                        m_imglist.Images.Add(info.id.ToString(), iconImage);
                        info.ImageIndex = m_imglist.Images.Count - 1;
                    }
                    else
                    {
                        info.ImageIndex = -1;
                    }
                    
                    probar1.Value += 1;

                    if (probar1.Value % 10 == 0) // Application.DoEvents效率很低，不必每次更新都要调1次
                    {
                        label1.Text = info.fielname;
                        Application.DoEvents();
                    }                    
                }
                else
                {
                    info.ImageIndex = -1;
                }
            }

            this.Close();
        }
    }

    public class Info
    {
        public int id;
        public string fielname;
        public int frame;
        public int ImageIndex;
        public string kind;
        public string subKind;
        public string tag1;
        public string tag2;
        public int soundID;
        public int usedcount;
    };
}