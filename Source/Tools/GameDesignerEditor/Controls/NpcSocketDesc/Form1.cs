using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using sprSelector;
using atlaxLib;

namespace NpcSocketDesc
{
    public partial class Form1 : Form
    {
        private string m_EngineDir;
        private string m_RepresentID;
        private string m_Socket;
        private string m_sql;
        private SqlDataAdapter m_adp;
        private SqlConnection Conn;
        private Test3DEngine m_engine;
        private string m_strLast = string.Empty;
        public string m_strFirstDesc = string.Empty;

        public Form1(string enginedir, Test3DEngine engine, string representid, string socket, SqlConnection conn)
        {
            InitializeComponent();

            m_EngineDir = enginedir;
            m_engine = engine;
            m_RepresentID = representid;
            m_Socket = socket;
            Conn = conn;

            m_sql = string.Format("SELECT Model FROM npc WHERE RepresentID = {0}", representid);
            DataTable tbl_npc = Helper.GetDataTable(m_sql, Conn);
            string strTitle = string.Format("{0} - {1}, {2}, {3}", this.Text, representid, tbl_npc.Rows[0][0], socket);
            this.Text = strTitle;

            UpdateGVData();
        }

        private void UpdateGVData()
        {
            m_sql = string.Format("SELECT * FROM dic_npc_socket_desc WHERE RepresentID = {0} AND Socket = '{1}'", m_RepresentID, m_Socket);
            m_adp = new SqlDataAdapter(m_sql, Conn);
            m_adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            m_adp.Fill(ds);
            DataTable tbl = ds.Tables[0];

            this.dataGridViewX1.DataSource = tbl;
            for (int i = 0; i < this.dataGridViewX1.ColumnCount - 1; i++)
                this.dataGridViewX1.Columns[i].Width = 75;
            this.dataGridViewX1.Columns[this.dataGridViewX1.ColumnCount - 1].Width = 360;
        }

        private void button2_Click(object sender, EventArgs e) // Ìí¼Ó
        {
            UpdateGVData();

            string sql = string.Format("SELECT map, Model FROM npc WHERE RepresentID = {0}", m_RepresentID);
            DataTable tbl_trash = Helper.GetDataTable(sql, Conn);
            string strMap = tbl_trash.Rows[0]["map"].ToString().Trim();
            string strName = tbl_trash.Rows[0]["Model"].ToString().Trim();

            sql = string.Format("SELECT MAX(_index) FROM dic_npc_socket_desc");
            tbl_trash = Helper.GetDataTable(sql, Conn);
            int newIndex = tbl_trash.Rows[0][0] == DBNull.Value ? 1 : Convert.ToInt32(tbl_trash.Rows[0][0]) + 1;

            DataTable tbl = this.dataGridViewX1.DataSource as DataTable;
            DataRow row = tbl.NewRow();
            row["_index"] = newIndex;
            row["Map"] = strMap;
            row["RepresentID"] = m_RepresentID;
            row["Name"] = strName;
            row["Socket"] = m_Socket;
            row["FileName"] = this.textBox1.Text;
            tbl.Rows.Add(row);

            SqlCommandBuilder cmdBuilder = new SqlCommandBuilder(m_adp);
            m_adp.InsertCommand = cmdBuilder.GetInsertCommand();

            int val = m_adp.Update(tbl);
            tbl.AcceptChanges();
        }

        private void dataGridViewX1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F3)
            {
                button1_Click(this.button1, null);
            }
        }
        private void button1_Click(object sender, EventArgs e) // ä¯ÀÀ
        {
            string strBrowseBase = m_EngineDir + "\\data\\source";
            string strDefaultFile = m_EngineDir + m_strLast;

            sprViewControl svc = new sprViewControl();
            string strRet = svc.show4ff_new(m_EngineDir, strBrowseBase, strDefaultFile, m_engine, "mesh", Conn);
            if (strRet != null && strRet.Length != 0)
            {
                m_strLast = strRet;
                textBox1.Text = strRet;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e) // meshÓÐÐÞ¸Ä
        {
            string strFullFileName = m_EngineDir + "\\" + this.textBox1.Text;
            m_engine.ClearPluginModel(m_Socket);

            if (this.textBox1.Text.Length == 0)
                return;

            m_engine.LoadPluginModel(strFullFileName, m_Socket);
        }

        private void dataGridViewX1_SelectionChanged(object sender, EventArgs e)
        {
            if (this.dataGridViewX1.SelectedRows.Count != 1)
                return;


            string strmesh = this.dataGridViewX1.SelectedRows[0].Cells[this.dataGridViewX1.ColumnCount - 1].Value.ToString();
            m_engine.ClearPluginModel(m_Socket);

            if (strmesh.Length == 0)
                return;

            m_engine.LoadPluginModel(m_EngineDir + "\\" + strmesh, m_Socket);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            UpdateGVData();
            DataTable tbl = this.dataGridViewX1.DataSource as DataTable;
            if (tbl.Rows.Count > 0)
            {
                m_strFirstDesc = tbl.Rows[0]["FileName"].ToString().Trim();
            }
        }

        private void buttonDel_Click(object sender, EventArgs e)
        {
            if (this.dataGridViewX1.SelectedRows.Count != 1)
                return;

            DataGridViewRow row = this.dataGridViewX1.SelectedRows[0];
            if (row.IsNewRow)
                return;

            int i = Convert.ToInt32(row.Cells["_index"].Value);
            this.dataGridViewX1.Rows.Remove(row);

            SqlCommand cmd = Conn.CreateCommand();
            string sql = "DELETE FROM dic_npc_socket_desc WHERE _index = " + i.ToString();
            cmd.CommandText = sql;
            Conn.Open();
            cmd.ExecuteNonQuery();
            Conn.Close();            
        }

    }
}

