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
    class ShopListLogic
    {
        static public ShopList AddShopList(ShopList shoplist)
        {
            if (false == App.AppVariables.g_dataprovider.IsShopListExists(shoplist.ShopName))
            {
                return App.AppVariables.g_dataprovider.AddShopList(shoplist);
            }
            else
                return null;
        }
        static public bool UpdateShopList(ShopList shoplist)
        {
            return App.AppVariables.g_dataprovider.UpdateShopList(shoplist);
        }
        static public bool DeleteShopList(int shoplistid) 
        {
            return App.AppVariables.g_dataprovider.DeleteShopList(shoplistid);
        }
        static public int GetShopListCount()
        {
            return App.AppVariables.g_dataprovider.GetShopListCount();
        }
        static public ShopList GetOneShopList(int shoplistid)
        {
            return App.AppVariables.g_dataprovider.GetOneShopList(shoplistid);
        }
        static public ShopListCollection GetShopListsBySql(string sql)
        {
            return App.AppVariables.g_dataprovider.GetShopListsBySql(sql);
        }
        static public ShopListCollection GetShopListsBySql(string sql, int PageSize, int PageIndex)
        {
            return App.AppVariables.g_dataprovider.GetShopListsBySql(sql, PageSize, PageIndex);
        }
        static public ShopListCollection GetAllShopLists()
        {
            return App.AppVariables.g_dataprovider.GetAllShopLists();
        }
        static public ShopListCollection GetAllShopLists(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            return App.AppVariables.g_dataprovider.GetAllShopLists(PageSize, PageIndex, SortField, DESC, out retCnt);
        }
        static public ShopList GetOneShopListByShopName(string shopname)
        {
            return App.AppVariables.g_dataprovider.GetOneShopListByShopName(shopname);
        }
        static public bool IsShopListExists(string shopname)
        {
            return App.AppVariables.g_dataprovider.IsShopListExists(shopname);
        }
    }
}
