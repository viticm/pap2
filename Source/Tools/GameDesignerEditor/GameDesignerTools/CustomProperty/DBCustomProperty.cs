using System;
using System.Collections.Generic;
using System.Text;

namespace GameDesingerTools
{
    public class DBCustomProperty : CustomProperty
    {
        private RecordLockEx m_lock;
        //private int m_nLastServerVersion = -1;
        private int m_nFieldID = -1;

        public DBCustomProperty(CustomClass parent, string strID, string sName, object value, bool bReadOnly, bool bVisible, enumValueType vtType)
            :base(parent, strID, sName, value, bReadOnly, bVisible, vtType)
        {
            //DBCustomClass cls = parent as DBCustomClass;
            //m_lock = new RecordLock(cls.Conn, cls.MainTable, cls.DBPrimaryKey, cls.ModId);
        }
        public int FieldID
        {
            get { return m_nFieldID; }
            set { m_nFieldID = value; }
        }
        //仅为 objpool 服务
        public void Init(CustomClass Parent, object value)
        {
            //m_nLastServerVersion = -1;
            m_parent = Parent;
            this.Value = value;
            DefaultValue = null;
        }
        
        public RecordLockEx Lock
        {
            get { return m_lock; }
            set { m_lock = value; }
        }
        public int LastVersion
        {
            //get { return m_nLastServerVersion; }
            //set { m_nLastServerVersion = value; }
            get {
                return m_lock.LocalVersion;
            }
        }
        public bool UpdateLastVersion()
        {
            if (m_lock != null && this.Tag != null)
            {
                //m_nLastServerVersion = m_lock.GetServerLastVersion(this.Tag as object[]);
                m_lock.UpdateLocalVersion(this.Tag as object[]);
                return true;
            }
            return false;
        }
        public void UpdateDefaultValue()
        {
            if (this.Value is DBCustomClass)
                this.DefaultValue = ((DBCustomClass)this.Value).Value;
            else
                this.DefaultValue = this.Value;

        }
    }
}
