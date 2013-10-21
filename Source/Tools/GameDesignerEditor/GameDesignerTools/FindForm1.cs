using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Collections;

namespace GameDesingerTools
{
    public partial class FindForm1 : Form
    {
        BaseForm m_FatherForm;
        string[] m_FieldsToFind;
        RadioButton[] m_Radiobuttons = new RadioButton[6];
        DataTable m_tbl;
        TreeView m_tree;
        ArrayList m_AllFoundNodes = new ArrayList();
        int m_CurIndex = -1;
		string[] catInfo = new string[0];
        OptimizeLoadTreeFinder theNodeFinder = new OptimizeLoadTreeFinder();

        public FindForm1(BaseForm father, TreeView tree, DataTable MainTbl, string DisplayField, ArrayList arrExtraFindColumnField)
        {
            InitializeComponent();
            m_FatherForm = father;
            m_tree = tree;
            m_tbl = MainTbl;
            m_Radiobuttons[0] = radioButton1;
            m_Radiobuttons[1] = radioButton2;
            m_Radiobuttons[2] = radioButton3;
            m_Radiobuttons[3] = radioButton4;
            m_Radiobuttons[4] = radioButton5;
            m_Radiobuttons[5] = radioButton6;
            radioButton1.Checked = true;

            bool bKeysContainDisplayField = false;
            foreach (DataColumn col in m_tbl.PrimaryKey)
            {
                if (col.ColumnName.ToLower() == DisplayField.ToLower())
                {
                    bKeysContainDisplayField = true;
                    break;
                }
            }

            if (bKeysContainDisplayField)
            {
                m_FieldsToFind = new string[m_tbl.PrimaryKey.Length];
                for (int i = 0; i < m_tbl.PrimaryKey.Length; i++)
                {
                    m_FieldsToFind[i] = m_tbl.PrimaryKey[i].ColumnName;
                    m_Radiobuttons[i].Text = m_FieldsToFind[i];
                    m_Radiobuttons[i].Tag = m_FieldsToFind[i];
                    m_Radiobuttons[i].Visible = true;
                }
            }
            else
            {
                m_FieldsToFind = new string[m_tbl.PrimaryKey.Length + 1];
                for (int i = 0; i < m_tbl.PrimaryKey.Length; i++)
                {
                    m_FieldsToFind[i] = m_tbl.PrimaryKey[i].ColumnName;
                    m_Radiobuttons[i].Text = m_FieldsToFind[i];
                    m_Radiobuttons[i].Tag = m_FieldsToFind[i];
                    m_Radiobuttons[i].Visible = true;
                }
                m_FieldsToFind[m_FieldsToFind.Length - 1] = DisplayField;
                m_Radiobuttons[m_FieldsToFind.Length - 1].Text = DisplayField;
                m_Radiobuttons[m_FieldsToFind.Length - 1].Tag = DisplayField;
                m_Radiobuttons[m_FieldsToFind.Length - 1].Visible = true;
            }
            //添加额外的查找字段（来自lua脚本)
            AddExtraFindFields(arrExtraFindColumnField, m_FieldsToFind.Length);

            // 初始化当前模块的分类信息
            InitCatInfo();
        }

        private void AddExtraFindFields(ArrayList arrExtraFindColumnField, int nBeginRadioIndex)
        {
            string fieldName;
            string displayName;
            string[] fieldgroup;
            foreach (string fieldStr in arrExtraFindColumnField)
            {
                fieldgroup = fieldStr.Split(new char[] { '@' }, StringSplitOptions.RemoveEmptyEntries);
                if (fieldgroup == null || fieldgroup.Length != 2) continue;

                fieldName = fieldgroup[0];
                displayName = fieldgroup[1];

                //检查字段名称的合法性
               if(!m_tbl.Columns.Contains(fieldName))
               {
                   continue;                   
               }
               else
               {
                   //是否已经在查找列表里了
                   bool bHavedYet = false;
                   foreach (string havedField in m_FieldsToFind)
                   {
                       if (havedField.ToLower() == fieldName.ToLower())
                       {
                           bHavedYet = true;
                           break;
                       }
                   }
                   if (bHavedYet)
                   {
                       continue;
                   }
               }
               if (nBeginRadioIndex == m_Radiobuttons.Length)
                {
                    MessageBox.Show("查找的单选按钮已经分配完毕，程序需要添加更多的单选按钮");
                    return;
                }

                m_Radiobuttons[nBeginRadioIndex].Text = displayName;
                m_Radiobuttons[nBeginRadioIndex].Visible = true;
                m_Radiobuttons[nBeginRadioIndex].Tag = fieldName;
            }
        }

        private void InitCatInfo()
        {
            SqlConnection conn = MainForm.conn;
            
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }            

            string sqlString = string.Format("SELECT catfields FROM sys_modl_tab_def WHERE modid = '{0}' AND catfields IS NOT NULL AND catfields <> ''",
                                             m_FatherForm.ModID.ToString());
            /* remoting
            SqlCommand cmd = conn.CreateCommand();
            cmd.CommandText = sqlString;
            object executeObject = cmd.ExecuteScalar();
            */

            object executeObject = Helper.ExecuteScalar("sys_modl_tab_def", sqlString, conn, false);

            if (executeObject != null)
            {
                string catString = executeObject as string;
                catInfo = catString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            }            

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }
        }

        private void DoFind()
        {
            string InputContent = textBox.Text;
            if (InputContent == string.Empty)
                return;

            string fieldToFind = string.Empty;
            foreach (RadioButton radio in m_Radiobuttons)
            {
                if (radio.Visible && radio.Checked)
                {
                    fieldToFind = radio.Tag.ToString().Trim();
                }
            }

            bool bFindByNodeText = false;
            if (radioButton6.Checked)
            {
                bFindByNodeText = true;
            }

            DBCustomClass dbClass = ((PageDesc)m_FatherForm.Pages[0]).dbClass;
           //正常加载树时的查找
            if (dbClass.ExistLuaFunction("NotUsingOptimize") &&
                (bool)(dbClass.CallLuaFunction("NotUsingOptimize")[0])  
                )
            {
                if (m_AllFoundNodes.Count == 0)
                {
                    GenerateFindResult(fieldToFind, InputContent, bFindByNodeText);
                }
                if (m_AllFoundNodes.Count == 0)
                {
                    MessageBox.Show("没找到符合的记录。");
                    return;
                }

                m_CurIndex++;
                if (m_CurIndex >= m_AllFoundNodes.Count)
                    m_CurIndex = 0;
                m_tree.SelectedNode = (TreeNode)(m_AllFoundNodes[m_CurIndex]);

                m_tree.Focus();
            }
            else //使用了优化方式加载树时的查找
            {
                theNodeFinder.Init(fieldToFind, InputContent, bFindByNodeText, m_tree, m_FatherForm.Conn, m_FatherForm.CatFields, m_FatherForm.MainTableName,
                    m_FatherForm.DisplayField, m_FatherForm.TblMain.PrimaryKey, (m_FatherForm.Pages[0] as PageDesc).id);
                theNodeFinder.FindNext();
            }
        }

        private void buttonFind_Click(object sender, EventArgs e) // 查找
        {
            DoFind();
        }

        private void GenerateFindResult(string strFieldToFind, string strContent, bool bFindByNodeText)
        {
            bool bFieldToFindIsKey = false;
            foreach (DataColumn col in m_tbl.PrimaryKey)
            {
                if (col.ColumnName.ToLower() == strFieldToFind.ToLower())
                {
                    bFieldToFindIsKey = true;
                    break;
                }
            }

            foreach (TreeNode node in m_tree.Nodes)
            {
                if (bFindByNodeText)
                    GenerateFindResult_NodeText(node, strContent);
                else
                    GenerateFindResult(node, strFieldToFind, strContent, bFieldToFindIsKey);
            }
        }

        private void GenerateFindResult_NodeText(TreeNode node, string strContent)
        {
            if (node.Text.Contains(strContent))
            {
                m_AllFoundNodes.Add(node);
            }
            foreach (TreeNode n in node.Nodes)
            {
                GenerateFindResult_NodeText(n, strContent);
            }
        }

        private void GenerateFindResult(TreeNode node, string strFieldToFind, string strContent, bool bFieldToFindIsKey)
        {
            if (node.Level >= catInfo.Length)
            {
                object[] tags = (object[])(node.Tag);
                DataRow row = m_tbl.Rows.Find(tags);
                if (row != null)
                {
                    object content = row[strFieldToFind];
                    if (bFieldToFindIsKey)
                    {
                        if (content.ToString().Trim().ToLower() == strContent.ToLower())
                            m_AllFoundNodes.Add(node);
                    }
                    else
                    {
                        if (content.ToString().Trim().ToLower().Contains(strContent.ToLower()))
                            m_AllFoundNodes.Add(node);
                    }
                }
            }
            foreach (TreeNode n in node.Nodes)
            {
                GenerateFindResult(n, strFieldToFind, strContent, bFieldToFindIsKey);
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e) // 取消
        {
            m_FatherForm.m_FindFrm = null;
            this.Dispose();
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void radioButton4_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void radioButton5_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void radioButton6_CheckedChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }
        private void textBox_TextChanged(object sender, EventArgs e)
        {
            m_AllFoundNodes.Clear();
        }

        // 回车就查找
        private void textBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                DoFind();
            }
            else if (e.KeyCode == Keys.Escape)
            {
                m_FatherForm.m_FindFrm = null;
                this.Dispose();
            }
        }

        // 修复点x不能再打开查找界面的bug
        private void FindForm1_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_FatherForm.m_FindFrm = null;
        }      
    }
}