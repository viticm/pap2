using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security;
using System.Data.SqlClient;
using DevComponents.DotNetBar;
using System.Collections;
using System.Diagnostics;
using Nexus.Windows.Forms;

namespace biscuit
{
    public partial class MainForm : Form
    {
        private SqlConnection Conn = null;
        private string RootDir = string.Empty;
        public Control OutterCtrl = null;
        private string CataName = string.Empty;

        private static Random rand = new Random();
        private bool DataInited = false;

        public MainForm()
        {
            InitializeComponent();
            throw new Exception("请不要用这个构造函数。");
        }

        public MainForm(SqlConnection conn, string rootDir, string catname)
        {
            InitializeComponent();
            Conn = conn;
            RootDir = rootDir;
            OutterCtrl = tabControl1;
            CataName = catname;

            PieChart1.ItemStyle.SurfaceAlphaTransparency = 0.75F;
            PieChart1.FocusedItemStyle.SurfaceAlphaTransparency = 0.75F;
            PieChart1.FocusedItemStyle.SurfaceBrightnessFactor = 0.3F;
        }

        public bool hasChanged()
        {
            return false;
        }

        public bool saveAll()
        {
            return true;
        }

        public bool reloadAll()
        {
            return true;
        }

        private Color RandomColor()
        {
            return Color.FromArgb(rand.Next(255), rand.Next(255), rand.Next(255));
        }

        public bool reloadAll(string keyword)
        {
            // 初始化
            if (DataInited)
                return true;

            string[] keywords = keyword.Split(new char[] { ',' });
            if (keywords.Length != 2)
                MessageBox.Show("错误的reloadAll参数");

            string tbl_name = string.Empty;
            int tbl_id = 0;

            try
            {
                tbl_name = keywords[0];
                tbl_id = Convert.ToInt32(keywords[1]);
            }
            catch (Exception ex)
            {
                MessageBox.Show("reloadAll的参数keyword格式有误：" + ex.Message);
                return false;
            }

            // 获取db数据
            string sql = string.Format("SELECT {0}, COUNT(*) AS _count FROM {1} GROUP BY {2}", CataName, tbl_name, CataName);
            DataTable tbl = Helper.GetDataTable(sql, Conn);
            foreach (DataRow row in tbl.Rows)
            {
                string cata = row[CataName].ToString().Trim();
                int count = Convert.ToInt32(row["_count"]);
                string desc = string.Format("{0} : {1}", cata, count);
                PieChart1.Items.Add(new PieChartItem(count, RandomColor(), cata, desc, 0));
            }

            dataGridViewX1.DataSource = tbl;

            DataInited = true;
            return true;
        }

        private void panelEx1_SizeChanged(object sender, EventArgs e)
        {
            //int newWidth = panelEx1.Width;
            //int newHeight = panelEx1.Height;
            //panelEx1.ResetSize(newWidth, newHeight);
        }

        private void PieChart1_ItemClicked(object sender, PieChart.PieChartItemEventArgs e)
        {
            //if (e.Item.Tag == null || (bool)e.Item.Tag == true)
            //{
            //    e.Item.Offset += 50;
            //    e.Item.Tag = false;
            //}
            //else
            //{
            //    e.Item.Offset = Math.Max(0, e.Item.Offset - 50);
            //    e.Item.Tag = true;
            //}

            string itemname = e.Item.Text;

            foreach (DataGridViewRow row in dataGridViewX1.Rows)
            {
                bool rowfound = false;
                foreach (DataGridViewCell cell in row.Cells)
                {
                    if (cell.Value.ToString() == itemname)
                    {
                        rowfound = true;
                        cell.Selected = true;
                        break;
                    }
                }
                if (rowfound)
                {
                    break;
                }
            }
        }
    }
}