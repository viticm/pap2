using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace GameDesingerTools
{
    //add by cuiwei 07.8.23

    //需要同步命令的参数缓存，用于传给DBCustomClass.ModifyValue()
    class syncItem
    {
        public syncItem(string str1, string str2, object o1, string str3)
        {
            this.m_strFieldName = str2;
            this.m_strMainTableName = str1;
            this.m_strFindKeys = str3;
            this.m_oFieldValue = o1;
        }
        public string MainTableName
        {
            get { return m_strMainTableName; }
        }
        public string FieldName
        {
            get { return m_strFieldName; }
        }
        public object FieldValue
        {
            get { return m_oFieldValue; }
        }
        public string FindKeys
        {
            get { return m_strFindKeys; }
        }

        private string m_strMainTableName;  //主表名
        private string m_strFieldName;      //字段名
        private object m_oFieldValue;       //字段值
        private string m_strFindKeys;       //FindKeys
    };

    //用于保存后通知取缔黑体
    public class unBoldItem : queryCommand
    {

    };

    //去数据库拿数据并转换成数据结构的命令参数缓存，用于传给SelectObject()
    public class queryCommand
    {
        public queryCommand()
        {

        }
        public queryCommand(DBCustomClass dc, object[] ao)
        {
            this.ao = ao;
            this.dc = dc;
        }
        public DBCustomClass cls = null;

        private DBCustomClass dc;
        private object[] ao;
        public ArrayList m_syncList = new ArrayList();      //需要通知同步的项目列表

        public ArrayList syncList
        {
            get { return m_syncList; }
        }
        public DBCustomClass dbClass
        {
            get { return dc; }
        }
        public object[] Args
        {
            get { return ao; }
        }
    };
    //add end
}
