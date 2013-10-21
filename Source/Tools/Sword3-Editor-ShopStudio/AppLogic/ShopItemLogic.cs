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
    class ShopItemLogic
    {
        static public ShopItem AddShopItem(ShopItem shop)
        {
            if (false == App.AppVariables.g_dataprovider.IsItemExistsInShop(shop.ItemIndex, shop.ShopItemID))
            {
                return App.AppVariables.g_dataprovider.AddShopItem(shop);
            }
            else
                return null;
        }
        static public bool UpdateShopItem(ShopItem shop)
        {
            return App.AppVariables.g_dataprovider.UpdateShopItem(shop);
        }
        static public bool DeleteShopItem(int shopid)
        {
            return App.AppVariables.g_dataprovider.DeleteShopItem(shopid);
        }
        static public int GetShopItemCount()
        {
            return App.AppVariables.g_dataprovider.GetShopItemCount();
        }
        static public ShopItem GetOneShopItem(int shopid)
        {
            return App.AppVariables.g_dataprovider.GetOneShopItem(shopid);
        }
        static public ShopItemCollection GetShopItemsBySql(string sql)
        {
            return App.AppVariables.g_dataprovider.GetShopItemsBySql(sql);
        }
        static public ShopItemCollection GetShopItemsBySql(string sql, int PageSize, int PageIndex)
        {
            return App.AppVariables.g_dataprovider.GetShopItemsBySql(sql, PageSize, PageIndex);
        }
        static public ShopItemCollection GetAllShopItems()
        {
            return App.AppVariables.g_dataprovider.GetAllShopItems();
        }
        static public ShopItemCollection GetAllShopItems(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            return App.AppVariables.g_dataprovider.GetAllShopItems(PageSize, PageIndex, SortField, DESC, out retCnt);
        }

        static public ShopItemCollection GetShopItemsByShop(int shopid)
        {
            return App.AppVariables.g_dataprovider.GetShopItemsByShop(shopid);
        }
        static public bool IsItemExistsInShop(int itemIndex, int shopid)
        {
            return App.AppVariables.g_dataprovider.IsItemExistsInShop(itemIndex, shopid);
        }
        static public bool DeleteShopItemsByShopTemplateID(int shoptemplateid)
        {
            return App.AppVariables.g_dataprovider.DeleteShopItemsByShopTemplateID(shoptemplateid);
        }
    }
}
