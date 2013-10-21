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
using XSJ.Sword3.Editor.Studio.Component;

namespace XSJ.Sword3.Editor.Studio.AppLogic
{
    class EquipmentLogic
    {
        static public Equipment AddEquipment(Equipment equip)
        {
            return App.AppVariables.g_dataprovider.AddEquipment(equip);
        }
        static public bool UpdateEquipment(Equipment equip)
        {
            return App.AppVariables.g_dataprovider.UpdateEquipment(equip);
        }
        static public bool DeleteEquipment(int equipid)
        {
            return App.AppVariables.g_dataprovider.DeleteEquipment(equipid);
        }
        static public int GetEquipmentCount()
        {
            return App.AppVariables.g_dataprovider.GetEquipmentCount();
        }
        static public Equipment GetOneEquipment(int equipid)
        {
            return App.AppVariables.g_dataprovider.GetOneEquipment(equipid);
        }

        static public EquipmentCollection GetEquipmentsBySql(string sql)
        {
            return App.AppVariables.g_dataprovider.GetEquipmentsBySql(sql);
        }
        static public EquipmentCollection GetEquipmentsBySql(string sql, int PageSize, int PageIndex)
        {
            return App.AppVariables.g_dataprovider.GetEquipmentsBySql(sql, PageSize, PageIndex);
        }
        static public EquipmentCollection GetAllEquipments()
        {
            return App.AppVariables.g_dataprovider.GetAllEquipments();
        }
        static public EquipmentCollection GetAllEquipments(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            return App.AppVariables.g_dataprovider.GetAllEquipments(PageSize, PageIndex, SortField, DESC, out retCnt);
        }
        static public EquipmentCollection GetEquipmentsByParentID(int parentid)
        {
            return App.AppVariables.g_dataprovider.GetEquipmentsByParentID(parentid);
        }
    }
}
