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
    class ItemCategoryLogic
    {
        static public ItemCategory AddItemCategory(ItemCategory itemcategory)
        {
            return App.AppVariables.g_dataprovider.AddItemCategory(itemcategory);
        }
        static public bool UpdateItemCategory(ItemCategory itemcategory)
        {
            return App.AppVariables.g_dataprovider.UpdateItemCategory(itemcategory);
        }
        static public bool DeleteItemCategory(int itemcategoryid)
        {
            return App.AppVariables.g_dataprovider.DeleteItemCategory(itemcategoryid);
        }
        static public int GetItemCategoryCount()
        {
            return App.AppVariables.g_dataprovider.GetItemCategoryCount();
        }
        static public ItemCategory GetOneItemCategory(int itemcategoryid)
        {
            return App.AppVariables.g_dataprovider.GetOneItemCategory(itemcategoryid);
        }
        static public ItemCategoryCollection GetItemCategoriesBySql(string sql)
        {
            return App.AppVariables.g_dataprovider.GetItemCategoriesBySql(sql);
        }
        static public ItemCategoryCollection GetItemCategoriesBySql(string sql, int PageSize, int PageIndex)
        {
            return App.AppVariables.g_dataprovider.GetItemCategoriesBySql(sql, PageSize, PageIndex);
        }
        static public ItemCategoryCollection GetAllItemCategories()
        {
            return App.AppVariables.g_dataprovider.GetAllItemCategories();
        }
        static public ItemCategoryCollection GetItemCategories(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            return App.AppVariables.g_dataprovider.GetItemCategories(PageSize, PageIndex, SortField, DESC, out retCnt);
        }
    }
}
