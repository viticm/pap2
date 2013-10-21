using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace XMLEditControl
{
    public partial class FrmXMLEdit : DevComponents.DotNetBar.Office2007Form
    {
        private string m_strCode = "";
        private Hashtable ht_id2name = new Hashtable();
        private Hashtable ht_name2id = new Hashtable();
        private Hashtable ht_name2enum = new Hashtable();

        public FrmXMLEdit(string strCode, DataTable dt)
        {
            InitializeComponent();
            m_strCode = strCode;

            DataGridViewComboBoxColumn comCol = dataGv.Columns[2] as DataGridViewComboBoxColumn;
            foreach(DataRow row in dt.Rows )
            {
                ht_id2name.Add(row["id"].ToString(), row["name"].ToString());
                ht_name2id.Add(row["name"].ToString(), row["id"].ToString());
                ht_name2enum.Add(row["name"].ToString(), Convert.ToBoolean(row["isenum"]) || Convert.ToBoolean(row["isduplicate"]));
                comCol.Items.Add(row["name"].ToString());
            }
            
            ReloadGridList();
        }

        public string GetScriptCode()
        {
            return m_strCode;
        }



        private void ReloadGridList()
        {
            dataGv.Rows.Clear();
            XmlDocument xmldoc = new XmlDocument();
            if (this.m_strCode == "")
                this.m_strCode = "<ArgList></ArgList>";
            xmldoc.LoadXml(this.m_strCode);
            XmlNode xnode = xmldoc.SelectSingleNode("ArgList");
            foreach(XmlNode xn in xnode.ChildNodes)
            {
                int nNew = dataGv.Rows.Add();
                dataGv.Rows[nNew].Cells[0].Value = xn.Attributes["ID"].Value;
                dataGv.Rows[nNew].Cells[1].Value = xn.Attributes["Text"].Value;
                dataGv.Rows[nNew].Cells[2].Value = ht_id2name[xn.Attributes["Type"].Value];
                dataGv.Rows[nNew].Cells[3].Value = xn.Attributes["Name"].Value;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            string strTmp = "<ArgList>";
            for (int i = 0; i < dataGv.Rows.Count - 1; i++ )
            {
                strTmp += "<Arg ID=\"";
                strTmp += dataGv.Rows[i].Cells[0].Value.ToString();
                strTmp += "\" Name=\"";
                strTmp += dataGv.Rows[i].Cells[3].Value.ToString();
                strTmp += "\" Type=\"";
                if (dataGv.Rows[i].Cells[2].Value == null) return;
                strTmp += ht_name2id[dataGv.Rows[i].Cells[2].Value];
                strTmp += "\" Text=\"";
                strTmp += dataGv.Rows[i].Cells[1].Value.ToString();
                strTmp += "\" />";
            }
            strTmp += "</ArgList>";
            this.m_strCode = strTmp;
            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        private void dataGv_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            ResetID();
        }

        private void dataGv_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
        {
            ResetID();
        }

        private void ResetID()
        {
            for (int i = 0; i < dataGv.Rows.Count - 1; i++)
            {
                dataGv.Rows[i].Cells[0].Value = i + 1;
            }
        }

        private void dataGv_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (dataGv.Rows[e.RowIndex].Cells[2].Value == null || ht_name2enum[dataGv.Rows[e.RowIndex].Cells[2].Value.ToString()].ToString().ToLower() != "true")
            {
                dataGv.Rows[e.RowIndex].Cells[3].Value = "";
            }
        }
    }

    public class Sitem
    {
        public string id;
        public string name;

        public Sitem(string id, string name)
        {
            this.id = id;
            this.name = name;
        }
        public override string ToString()
        {
            return this.name;
        }
    };
}