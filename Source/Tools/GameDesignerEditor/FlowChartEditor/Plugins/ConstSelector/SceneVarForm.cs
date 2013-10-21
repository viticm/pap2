using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DBManager;
using DevComponents.AdvTree;

namespace ConstSelector
{
    public partial class SceneVarForm : DevComponents.DotNetBar.Office2007Form
    {
        private string m_SelectedVarIndex = "";
        private SqlConnection m_conn = DataBaseManager.GetDataBaseManager().Connection_Jx3web;
        public SceneVarForm()
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;
        }

        public string show(string oldVarIndex, string mapname)
        {
            this.advTree1.Nodes.Clear();
            Node node_map = new Node();
            node_map.Text = mapname;
            node_map.Image = imageList1.Images[0];
            advTree1.Nodes.Add(node_map);

            string sql = "select varindex, name, description from tbl_sceneVar where mapid=(select id from maplist where name='" + mapname + "')";
            DataTable dt = DataBaseManager.GetDataBaseManager().GetDataTable(sql, this.m_conn);
            for (int i = 0; i < dt.Rows.Count; i++)
            {
                Node node = new Node();
                node.Text = dt.Rows[i]["name"].ToString();
                node.Image = imageList1.Images[1];
                node.Tag = dt.Rows[i]["varindex"];
                node.Cells.Add(new Cell(string.Format("{0}: {1}", dt.Rows[i]["varindex"], dt.Rows[i]["description"])));
                node.Cells.Add(new Cell());
                node_map.Nodes.Add(node);
                if(dt.Rows[i]["varindex"].ToString() == oldVarIndex)
                {
                    advTree1.SelectedNode = node;
                }
            }

            if(this.ShowDialog() == DialogResult.OK)
            {
                return m_SelectedVarIndex;
            }
            return oldVarIndex;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if(this.advTree1.SelectedNode == null || this.advTree1.SelectedNode.Nodes.ParentNode == null)
            this.DialogResult = DialogResult.OK;
            m_SelectedVarIndex = this.advTree1.SelectedNode.Tag.ToString();
            return;
        }

    }
}