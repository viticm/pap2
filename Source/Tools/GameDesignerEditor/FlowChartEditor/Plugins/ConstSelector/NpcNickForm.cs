using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DBManager;
using DevComponents.AdvTree;
using System.IO;
using System.Runtime.InteropServices;

namespace ConstSelector
{
    public partial class NpcNickForm : DevComponents.DotNetBar.Office2007Form
    {
        private string m_SelectedKey = "";
        public NpcNickForm()
        {
            InitializeComponent();
            this.DialogResult = DialogResult.Cancel;
        }

        public string show(string oldVarIndex, string mapname, string ClientPath)
        {
            this.advTree1.Nodes.Clear();
            Node node_map = new Node();
            node_map.Text = mapname;
            node_map.Image = imageList1.Images[0];
            advTree1.Nodes.Add(node_map);

            string iniFileName = Path.Combine(ClientPath,
                @"data\source\maps\" + mapname + "\\" + mapname + ".Map.Logical");


            IniStructure m_inis = new IniStructure();
            m_inis = IniStructure.ReadIni(iniFileName);
            if (m_inis == null) 
            {
                MessageBox.Show("’“≤ªµΩ" + iniFileName, "¥ÌŒÛ",
                         MessageBoxButtons.OK, MessageBoxIcon.Error);
                return "";
            }   
            int nNpcNumber = Convert.ToInt32(m_inis.GetValue("MAIN", "NumNPC").ToString());

            for (int i = 0; i < nNpcNumber; i++)
            {
                string szName = m_inis.GetValue("NPC" + i.ToString(), "szName");
                string szNickName = m_inis.GetValue("NPC" + i.ToString(), "szNickName");
                string nTempleteID = m_inis.GetValue("NPC" + i.ToString(), "nTempleteID");
                string nX = m_inis.GetValue("NPC" + i.ToString(), "nX");
                string nY = m_inis.GetValue("NPC" + i.ToString(), "nY");
                string nZ = m_inis.GetValue("NPC" + i.ToString(), "nZ");

                if (szNickName != null && szNickName != "")
                {
                    Node node = new Node();
                    node.Text = szNickName;
                    node.Image = imageList1.Images[1];
                    node.Tag = szNickName;
                    node.Cells.Add(new Cell(szName == null ? "" : szName));
                    node.Cells.Add(new Cell(string.Format("{0},{1},{2}", nX, nY, nZ)));
                    node.Cells.Add(new Cell());
                    node_map.Nodes.Add(node);
                    if (node.Tag.ToString() == oldVarIndex)
                    {
                        advTree1.SelectedNode = node;
                    }
                }
            }

            if(this.ShowDialog() == DialogResult.OK)
            {
                return m_SelectedKey;
            }
            return oldVarIndex;
        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            if (this.advTree1.SelectedNode == null || this.advTree1.SelectedNode.Parent == null) return;
            this.DialogResult = DialogResult.OK;
            m_SelectedKey = this.advTree1.SelectedNode.Tag.ToString();
            return;
        }
    }
}

