using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace HelperEditor
{
    public partial class ActionSelector : Form
    {
        public ActionSelector()
        {
            InitializeComponent();
        }

        public void ActionSelector_load(object sender, EventArgs e)
        {
            cmbAction.Items.Clear();
            TreeNode SourceNode = (TreeNode)this.Tag;
            //TreeNode SourceNode 
            bool bIsNewNode = SourceNode.Tag == null;
            string szActionKey = "";

            //得出当前选定的节点的行为事件ID
            if (SourceNode != null)
            {
                Form1 forSource = (Form1)SourceNode.TreeView.FindForm();
                TreeNode selectedActionNode = forSource.GetSelectedActionNode();
                szActionKey = selectedActionNode.Name;
            }
            string[,] tTabcontent = TabFile.ActionData.GetTabContent();
            for (int i = 1; i < TabFile.ActionData.RecordLength; i++)
            {
                //通过表的主键的字段名获取其相应的系列号
                int nMainKeyValue = TabFile.ActionData.GetColIndexByFieldName(TabFile.ActionData.MainKeyName);
                //取单元格内容分析
                if (tTabcontent[i, nMainKeyValue] != "")
                {
                    string szAllowableActionList = TabFile.ActionData.GetTabCell(i, "AllowableEvent");
                    bool bAllow = true;
                    if (szAllowableActionList != "")
                    {
                        bAllow = false;
                        foreach (string szAllowableActionID in szAllowableActionList.Split(','))
                        {
                            if (szAllowableActionID == szActionKey)
                            {
                                bAllow = true;
                                break;
                            }
                        }

                    }

                    if (bAllow == true)

                    {
                        string szShowName = TabFile.ActionData.GetTabCell(i, "class");
                        DorpDownListItem dorpDownListItem = new DorpDownListItem(szActionKey, tTabcontent[i, nMainKeyValue], 0);
                        dorpDownListItem.Index = cmbAction.Items.Add(dorpDownListItem);
                    }

                }
            }

            cmbAction.SelectedIndex = 0;
        }
        public string GetActionString()
        {
            return richTextBox_text.Text;
        }


    }
}
