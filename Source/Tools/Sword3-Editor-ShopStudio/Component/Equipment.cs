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

namespace XSJ.Sword3.Editor.Studio.Component
{
    class Equipment
    {
        private int m_recid = -1;
        private int m_id = -1;
        private string m_parentId = "-1";
        private string m_name = string.Empty;
        private string m_comment = string.Empty;
        private int m_gener = -1;
        private int m_subType = -1;
        private int m_detailType = -1;

        public int RECID
        {
            get { return m_recid; }
            set { m_recid = value; }
        }
        public int ID
        {
            get { return m_id; }
            set { m_id = value; }
        }
        public string ParentID
        {
            get { return m_parentId; }
            set { m_parentId = value; }
        }
        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        public string Comment
        {
            get { return m_comment; }
            set { m_comment = value; }
        }
        public int Gener
        {
            get { return m_gener; }
            set { m_gener = value; }
        }
        public int SubType
        {
            get { return m_subType; }
            set { m_subType = value; }
        }
        public int DetailType
        {
            get { return m_detailType; }
            set { m_detailType = value; }
        }
    }
}
