using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using DevComponents.DotNetBar;

namespace CustomLoadTree_Activity
{
    public class Class1
    {
        private Form m_OldForm;
        private PanelDockContainer m_PDC;
        private TreeViewEventHandler afterSelect;
        private TreeViewCancelEventHandler beforeSelect;
        private NodeLabelEditEventHandler beforeLabelEdit;
        private NodeLabelEditEventHandler afterLabelEdit;
        private SqlConnection m_Conn;
        private VisualEditActEx m_EditLibEx;

        private bool m_isReadOnly = false;

        public Form TreeForm
        {
            get { return m_OldForm; }
        }

        public Class1(PanelDockContainer pdc, TreeViewEventHandler afterselect, TreeViewCancelEventHandler beforeselect, NodeLabelEditEventHandler beforelabeledit, NodeLabelEditEventHandler afterlabeledit, SqlConnection conn)
        {
            m_OldForm = null;

            m_PDC = pdc;
            this.afterSelect = afterselect;
            this.beforeSelect = beforeselect;
            this.beforeLabelEdit = beforelabeledit;
            this.afterLabelEdit = afterlabeledit;
            m_Conn = conn;
            // helper = new Helper();
            Helper.Conn = m_Conn;
            Helper.adp_hash = new Hashtable();
        }

        public TreeView LoadTreePlugin_Activity(int toSelectID)
        {
            if (m_OldForm != null)
            {
                m_PDC.Controls.Remove((m_OldForm as Form_Activity).Spliter);
                m_OldForm.Dispose();
            }

            Form_Activity newFrm = new Form_Activity(this, m_Conn, this.m_isReadOnly);
            //  newFrm.FieldList = m_fieldList;
            m_OldForm = newFrm;
            TreeView tree = newFrm.ReloadTree();
            m_PDC.Controls.Add(newFrm.Spliter);
            newFrm.Spliter.Dock = DockStyle.Fill;

            tree.AfterSelect += afterSelect;
            tree.BeforeSelect += beforeSelect;
            tree.BeforeLabelEdit += beforeLabelEdit;
            tree.AfterLabelEdit += afterLabelEdit;

            return tree;
        }

        public Control GetTabControl(string RootDir, int nFont)
        {
            m_EditLibEx = new VisualEditActEx(m_Conn, this, RootDir, true, nFont);
            return m_EditLibEx.Table;
        }

        /// <summary>
        /// 是否被用户改变
        /// </summary>
        /// <returns>返回一个是否修改的标志</returns>
        public Boolean hasChanged()
        {
            if (m_EditLibEx != null)
                return m_EditLibEx.hasChanged();
            else
                return false;
        }


        /// <summary>
        /// 保存所有页
        /// </summary>
        public void saveAll()
        {
            if (m_EditLibEx != null)   
               m_EditLibEx.saveAll();
        }

        /// <summary>
        /// 刷新所有页面
        /// </summary>
        public void reloadAll()
        {
            if (m_EditLibEx != null) 
                m_EditLibEx.reloadAll();
        }

        /// <summary>
        /// 按照新关键字重新载入
        /// </summary>
        /// <param name="keyword">新关键字</param>
        public void reloadAll(string keyword)
        {
            if (m_EditLibEx != null) 
                m_EditLibEx.reloadAll(keyword);
        }
    }
}
