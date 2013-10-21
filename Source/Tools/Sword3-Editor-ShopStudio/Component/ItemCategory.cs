/*===================================
 * Purpose:
 * Created By: WilsonWei
 * Created On: 2007-2-7
 * (c)Kingsoft Corporation
 * Comments:
 * 
 * 
 * 
 * ====================================*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace XSJ.Sword3.Editor.Studio.Component
{
    class ItemCategory
    {
        private int m_recid = -1;
        private string m_categoryname = string.Empty;
        private string m_filename = string.Empty;

        public int RECID
        {
            get { return m_recid; }
            set { m_recid = value; }
        }
        public string CategoryName
        {
            get { return m_categoryname; }
            set { m_categoryname = value; }
        }
        public string Filename
        {
            get { return m_filename; }
            set { m_filename = value; }
        }
    }

    class ItemCategoryCollection : ArrayList
    {
        public ItemCategoryCollection() : base() { }
        public ItemCategoryCollection(ICollection c) : base(c) { }
    }
}
