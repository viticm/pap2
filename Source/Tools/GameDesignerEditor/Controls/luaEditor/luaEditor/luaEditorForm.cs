using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace luaEditor
{
    public partial class luaEditorForm : Form
    {
        private string m_strText = "";
        
        public string StrCode
        {
            get
            {
                return m_strText;
            }
            set
            {
                this.m_strText = value;
                this.luaEditorControl1.Text = value;
            }
        }

        public luaEditorForm()
        {
            this.DialogResult = DialogResult.Cancel;
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.m_strText = luaEditorControl1.Text;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }

    public class luaEditorFormCaller
    {
        private luaEditorForm luaForm = null;

        public string ShowLuaEditor(string strCode)
        {
            if(luaForm == null) luaForm = new luaEditorForm();
            luaForm.StrCode = strCode;
            if ( luaForm.ShowDialog() == DialogResult.OK)
                return luaForm.StrCode;
            return strCode;
        }
    }
}