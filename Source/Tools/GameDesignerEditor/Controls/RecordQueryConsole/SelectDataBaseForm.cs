using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace RecordQueryConsole
{
    public partial class SelectDataBaseForm : Form
    {
        public string ChoosedDBName = string.Empty;
        private DataSet ds = new DataSet();
        string dbXmlFile = Application.StartupPath + "/DataBaseNames.xml";
        const string DataBaseNamesTable = "DataBaseNames";
        const string rootTable = "root";
        const string name_cnColumn = "name_cn";
        const string nameColumn = "name";

       
        public SelectDataBaseForm()
        {
            InitializeComponent();
        }
        private bool Init()
        {
            try
            {
                ds.ReadXml(dbXmlFile);
                DataTable dt = ds.Tables[DataBaseNamesTable];
                combxDataBases.DataSource = dt;
                combxDataBases.DisplayMember = name_cnColumn;
                combxDataBases.ValueMember = nameColumn;

                DataTable dtRoot = ds.Tables[rootTable];
                string defaultDataBaseName = dtRoot.Rows[0][0].ToString();
                combxDataBases.SelectedValue = defaultDataBaseName;
            }
            catch
            {
                return false;
            }
           

            return true;
        }
        private void tbnOK_Click(object sender, EventArgs e)
        {
            object select = combxDataBases.SelectedValue;
            if (select == null)
            {
                return;
            }
            ChoosedDBName = combxDataBases.SelectedValue.ToString();
            string oldDefault = ds.Tables["root"].Rows[0][0].ToString();
            if (oldDefault != ChoosedDBName && chkSetDefault.Checked)
            {
                ds.Tables["root"].Rows[0][0] = ChoosedDBName;
                System.Xml.XmlTextWriter writer = new System.Xml.XmlTextWriter(dbXmlFile, Encoding.UTF8);
                writer.Formatting = System.Xml.Formatting.Indented;
                writer.WriteStartDocument();

                ds.WriteXml(writer);
                writer.Close();
            }
            
            this.DialogResult = DialogResult.OK;
        }

       
        public new DialogResult  ShowDialog()
        {
            if (!Init())
            {
                MessageBox.Show("∂¡»°DataBaseNames.xml ß∞‹");
                return DialogResult.Cancel;
            }
            return base.ShowDialog();
        }
    }
}