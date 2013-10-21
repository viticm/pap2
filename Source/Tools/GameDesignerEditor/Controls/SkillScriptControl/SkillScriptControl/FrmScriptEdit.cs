using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SkillScriptControl
{
    public partial class FrmScriptEdit : Form
    {
        private string m_strCode = "";

        public FrmScriptEdit(string strCode)
        {
            InitializeComponent();
            m_strCode = strCode;
            ReloadGridList();
        }

        public string GetScriptCode()
        {
            return m_strCode;
        }



        private void ReloadGridList()
        {
            dataGv.Columns.Clear();
            dataGv.Rows.Clear();
            ArrayList al_cols = SkillScriptControl.SSControl.GetColList(m_strCode);
            //fill col
            foreach(string strCol in al_cols)
            {
                dataGv.Columns.Add(strCol, strCol);
            }
            foreach(DataGridViewColumn vcol in dataGv.Columns )
            {
                vcol.SortMode = DataGridViewColumnSortMode.NotSortable;
            }

            //fill row
            string strCode = m_strCode.TrimStart(new char[] { '\t', '\r', '\n', ' '});
            strCode = m_strCode.TrimEnd(new char[] { '\t', '\r', '\n', ' '});
            string[] al_lines = strCode.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            if (al_lines.Length == 0) return;

            dataGv.Rows.Add(al_lines.Length);
            for (int j = 0; j < al_lines.Length; j++)
            {
                string strLine = al_lines[j];
                string[] al_Value = strLine.Split(new char[] { '=' });
                for (int i = 0; i < al_Value.Length - 1; i++ )
                {
                    string strValue = al_Value[i + 1];
                    strValue = strValue.Substring(0, strValue.IndexOf(','));
                    strValue = strValue.TrimEnd(new char[] { '}', ' ' });
                    dataGv.Rows[j].Cells[i].Value = strValue;
                }
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            string strTmp = "";

            for (int i = 0; i < dataGv.Rows.Count - 1; i++ )
            {
                strTmp += "{";
                for(int j = 0; j < dataGv.Columns.Count; j++)
                {
                    string strVarName = dataGv.Columns[j].HeaderCell.Value.ToString().Trim();
                    object objValue = dataGv.Rows[i].Cells[j].Value;
                    string strVarValue = "";
                    if(objValue == null)
                    {
                        strVarValue = "0";
                    }
                    else
                    {
                        strVarValue = objValue.ToString().Trim();
                    }
                    if (strVarName == "") strVarName = "noname";
                    if (strVarValue == "") strVarName = "0";
                    strTmp += strVarName + " = " + strVarValue + ",";
                }
                strTmp = strTmp.TrimEnd(new char[] { ',' });
                strTmp += "},\r\n";
            }
            this.m_strCode = strTmp;
            this.DialogResult = DialogResult.OK;
            this.Visible = false;
        }

        private void popColAdd_Click(object sender, EventArgs e)
        {
            string strNewValue = Helper.InputBox("", "添加新列");
            if (strNewValue != null)
                dataGv.Columns.Add(strNewValue, strNewValue);
        }

        private void Rename_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGv.SelectedCells.Count == 0) return; 
            int ic = dataGv.SelectedCells[0].ColumnIndex;
            string strNewValue = Helper.InputBox(dataGv.Columns[ic].HeaderCell.Value.ToString(), "重命名列名");
            if (strNewValue!= null)
                dataGv.Columns[ic].HeaderCell.Value = strNewValue;
        }

        private void Remove_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dataGv.SelectedCells.Count == 0) return; 
            int ic = dataGv.SelectedCells[0].ColumnIndex;
            if(MessageBox.Show("确定要删除列[" + dataGv.Columns[ic].HeaderCell.Value.ToString() + "]?", "确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.Yes)
            {
                dataGv.Columns.RemoveAt(ic);
            }
        }
    }
}