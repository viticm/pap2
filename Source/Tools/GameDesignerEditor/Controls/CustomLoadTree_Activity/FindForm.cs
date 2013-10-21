using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CustomLoadTree_Activity
{
    public partial class FindForm : Form
    {
        Form_Activity m_Parent;
        TreeNode CurFindPos;
        
        public FindForm(Form_Activity parent)
        {
            InitializeComponent();

            m_Parent = parent;
            TopMost = true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_Parent.Tree.ExpandAll();

            while(true)
            {
                if (CurFindPos == null)
                {
                    MessageBox.Show("未找到下一个节点");
                    CurFindPos = m_Parent.Tree.Nodes[0];
                    break;
                }
                if (CurFindPos.Text.Contains(textBox1.Text))
                {
                    m_Parent.Tree.SelectedNode = CurFindPos;
                    m_Parent.Tree.LabelEdit = true;
                    CurFindPos.BeginEdit();
                    CurFindPos.EndEdit(true);
                    m_Parent.Tree.LabelEdit = false;
                    CurFindPos = CurFindPos.NextVisibleNode;
                    break;
                } 
                else
                {
                    CurFindPos = CurFindPos.NextVisibleNode;
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Hide();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            CurFindPos = m_Parent.Tree.Nodes[0];
        }
    }
}