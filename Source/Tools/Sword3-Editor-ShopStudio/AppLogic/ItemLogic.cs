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
using XSJ.Sword3.Editor.Studio.Data;

namespace XSJ.Sword3.Editor.Studio.AppLogic
{
    class ItemLogic
    {
        public static Item AddItem(Item item)
        {
            return App.AppVariables.g_dataprovider.AddItem(item);
        }
        public static bool UpdateItem(Item item)
        {
            return App.AppVariables.g_dataprovider.UpdateItem(item);
        }
        public static bool DeleteItem(int itemid)
        {
            return App.AppVariables.g_dataprovider.DeleteItem(itemid);
        }
        public static int GetItemCount()
        {
            return App.AppVariables.g_dataprovider.GetItemCount();
        }
        public static Item GetOneItem(int itemid)
        {
            return App.AppVariables.g_dataprovider.GetOneItem(itemid);
        }

        public static ItemCollection GetItemsBySql(string sql)
        {
            return App.AppVariables.g_dataprovider.GetItemsBySql(sql);
        }
        public static ItemCollection GetItemsBySql(string sql, int PageSize, int PageIndex)
        {
            return App.AppVariables.g_dataprovider.GetItemsBySql(sql, PageSize, PageIndex);
        }
        public static ItemCollection GetAllItems()
        {
            return App.AppVariables.g_dataprovider.GetAllItems();
        }
        public static ItemCollection GetAllItems(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            return App.AppVariables.g_dataprovider.GetAllItems(PageSize, PageIndex, SortField, DESC, out retCnt);
        }
        public static ItemCollection GetItemsByCategory(int genre, int subType, int detailType)
        {
            return App.AppVariables.g_dataprovider.GetItemsByCategory(genre, subType, detailType);
        }
    }
}
