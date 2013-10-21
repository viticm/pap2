using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    public partial class UniteModel : Form
    {
        private DataTableProxy mDataTableProxy;
        private string mModelName;
        private int mModelID;

        public bool RunUnite = true;

        public UniteModel(string mdlname)
        {
            InitializeComponent();
            bool bRet = false;
            if (Program.EnableRemoting)
            { 
                mDataTableProxy = DataTableServerProxy.GetDataTableProxy();
                bRet = mDataTableProxy.UTExistDatabase(Program.Database);
            }
            if (!bRet)
            {
                MessageBox.Show("本库不是主干或分技或没有连接远程数据服务");
                RunUnite = false;
                return;
            }

            mModelName = mdlname;
            InitTables();
            
        }

        private void InitTables()
        {

            string filter = string.Format("modelname_en = '{0}'", mModelName);
            DataTable dt_mdl_def = Helper.GetDataTableProxy("sys_modl_def", filter, null, MainForm.conn);
            int id = Convert.ToInt32(dt_mdl_def.Rows[0][0]);

            filter = string.Format("modid = {0}", id);
            DataTable dt_mdl_tbl = Helper.GetDataTableProxy("sys_modl_tab_def", filter, null, MainForm.conn);

            filter = string.Empty;
           
            foreach(DataRow dr in dt_mdl_tbl.Rows)
            {
                if (filter != string.Empty)
                {
                    filter += " or ";
                }
                filter += string.Format("modtabid = {0}", dr["modtabid"]);
            }

            DataTable dt_meta_info = Helper.GetDataTableProxy("sys_meta_info", filter, null, MainForm.conn);

            List<string> tblnames = new List<string>();
            foreach (DataRow dr in dt_meta_info.Rows)
            {
                string name = dr["tablename"].ToString();
                if (!tblnames.Contains(name))
                {
                    tblnames.Add(name);
                }
            }

            checkedListBox1.Items.Clear();
            foreach (string name in tblnames)
            {
                checkedListBox1.Items.Add(name);
            }

            string dbname = Program.Database;

            string[] dbs = mDataTableProxy.UTGetDatabases();
            foreach (string db in dbs)
            {
                listViewEx2.Items.Add(db);
            }
        }

        private void buttonX2_Click(object sender, EventArgs e)
        {
            bool bRTtable = true;
            for (int index = 0; index < checkedListBox1.Items.Count; ++index)
            {
                bool bCheck = checkedListBox1.GetItemChecked(index);
                if (bCheck)
                {
                    string tblname = checkedListBox1.Items[index].ToString().Trim();
                    if (tblname.Length > 7 && tblname.Substring(0, 7) != "RTTEST_")
                    {
                        bRTtable = false;
                        break;
                    }
                }
            }

            if (!bRTtable)
            {
                string message = string.Format("该功能还没使用!");
                MessageBox.Show(message);
                return;
            }

            string todbname = Program.Database;
            string[] undbname = new string[listViewEx1.Items.Count];
            bool bExist = false;
            for (int i = 0; i < listViewEx1.Items.Count; ++i)
            {
                undbname[i] = listViewEx1.Items[i].SubItems[0].Text;
                if (undbname[i] == todbname)
                {
                    bExist = true;
                }
            }
            if (!bExist)
            {
                MessageBox.Show("请添加本库!");
                return;
            }



            for (int index = 0; index < checkedListBox1.Items.Count; ++index)
            {
                bool bCheck = checkedListBox1.GetItemChecked(index);
                if (bCheck)
                {
                    string tblname = checkedListBox1.Items[index].ToString().Trim();
                    int[] ret = mDataTableProxy.UTUniteTable(tblname, todbname, undbname);
                    string message = string.Empty;
                    if (ret == null || ret[0] == -1)
                    {
                        message = "出错啦！";
                    }
                    else
                    {
                        message = string.Format("{2}共导入{0}和修改{1}行数据", ret[0], ret[1], tblname);
                    }
                    
                    MessageBox.Show(message);
                }
            }
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void buttonX3_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection collection = listViewEx2.SelectedItems;
            foreach (ListViewItem item in collection)
            {
                listViewEx1.Items.Add(item.SubItems[0].Text);
                listViewEx2.Items.Remove(item);
            }
        }

        private void buttonX4_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection collection = listViewEx1.SelectedItems;
            foreach (ListViewItem item in collection)
            {
                listViewEx2.Items.Add(item.SubItems[0].Text);
                listViewEx1.Items.Remove(item);
            }
        }
    }
}