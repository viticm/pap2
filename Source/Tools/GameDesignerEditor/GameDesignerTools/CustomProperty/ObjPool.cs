using System;
using System.Collections.Generic;
using System.Text;

//未完成，暂时关闭此功能
//需要增加当收集的垃圾到一定大小后，要清除一些长期不用的，不然内存占用会无限增长

namespace GameDesingerTools
{
    class ObjPool
    {
        private static Dictionary<string, DBCustomProperty> m_poolprop = new Dictionary<string, DBCustomProperty>();
        private static Dictionary<string, DBCustomClass> m_pooldbclass = new Dictionary<string, DBCustomClass>();
        private static bool m_bEnabled = false; //关闭垃圾收集功能,此功能目前有问题，关闭还快一点？

        private static string GetPropKey(int nFieldID, string strID, string sName, object value, bool bReadOnly, bool bVisible, enumValueType vtType)
        {
            //string strkey = string.Format("id={0};sName={1};bReadOnly={2};bVisible={3};vtTtype={4}", strID, sName, bReadOnly.ToString(), bVisible.ToString(), vtType.ToString());
            string strkey = nFieldID.ToString();
            return strkey;
        }
        public static DBCustomProperty GetProperty(int nFieldID, CustomClass parent, string strID, string sName, object value, bool bReadOnly, bool bVisible, enumValueType vtType)
        {
            if (!m_bEnabled)
                return new DBCustomProperty(parent, strID, sName, value, bReadOnly, bVisible, vtType);
            string strkey = GetPropKey(nFieldID, strID, sName, value, bReadOnly, bVisible, vtType);
            DBCustomProperty prop = null;
            if (m_poolprop.ContainsKey(strkey))
            {
                prop = m_poolprop[strkey];
                m_poolprop.Remove(strkey);
                prop.Init(parent, value);
            }
            else
                prop = new DBCustomProperty(parent, strID, sName, value, bReadOnly, bVisible, vtType);
            prop.FieldID = nFieldID;
            return prop;
        }
        public static void DelProperty(DBCustomProperty prop)
        {
            if (!m_bEnabled)
                return;

            string strkey = GetPropKey(prop.FieldID, prop.ID, prop.Name, prop.Value, prop.ReadOnly, prop.Visible, prop.ValueType);
            if (!m_poolprop.ContainsKey(strkey))
            {
                m_poolprop.Add(strkey, prop);
            }
        }

        private static string GetDBClassKey(int nFieldID)
        {
            string strkey = nFieldID.ToString();
            return strkey;
        }
        public static DBCustomClass GetDBClass(object parent, LuaEx lua, int nTabId, int nParentFieldId, int nParentFieldType)
        {
            if (!m_bEnabled)
                return new DBCustomClass(parent, lua, nTabId, nParentFieldId, nParentFieldType);
            
            string strkey = GetDBClassKey(nParentFieldId);
            DBCustomClass cls = null;
            if (m_pooldbclass.ContainsKey(strkey))
            {
                cls = m_pooldbclass[strkey];
                m_pooldbclass.Remove(strkey);
                cls.ReInit(parent, nParentFieldType);
            }
            else
                cls = new DBCustomClass(parent, lua, nTabId, nParentFieldId, nParentFieldType);
            return cls;
        }
        public static void DelDBClass(DBCustomClass cls)
        {
            if (!m_bEnabled)
                return;

            string strkey = GetDBClassKey(cls.ParentFieldID);
            if (!m_pooldbclass.ContainsKey(strkey))
            {
                m_pooldbclass.Add(strkey, cls);
            }
        }
    }
}
