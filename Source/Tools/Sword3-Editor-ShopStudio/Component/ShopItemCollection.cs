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
    class ShopItemCollection : ArrayList
    {
        public ShopItemCollection() : base() { }
        public ShopItemCollection(ICollection c) : base(c) { }
        public ShopItem GetItem(int itemidx)
        {
            ShopItem retobj = null;
            if (this.Count > 0)
            {
                foreach (ShopItem shop in this)
                {
                    if (shop.ItemIndex == itemidx)
                    {
                        retobj = shop;
                        break;
                    }
                }
            }
            return retobj;
        }
        public void UpdateItem(ShopItem newitem)
        {
            if (this.Count <= 0) return;
            ShopItem shop = null;
            for (int i = 0; i < this.Count; i++)
            {
                shop = (ShopItem)this[i];
                if (shop.ItemIndex == newitem.ItemIndex)
                {
                    shop.ShopItemID = newitem.ShopItemID;
                    shop.CountLimit = newitem.CountLimit;
                    shop.Price = newitem.Price;
                    shop.RandSeed = newitem.RandSeed;
                    shop.ReputeLevel = newitem.ReputeLevel;
                    shop.ShopTemplateID = newitem.ShopTemplateID;
                    shop.TabType = newitem.TabType;
                    shop.SortIndex = newitem.SortIndex;
                    this[i] = shop;
                    break;
                }
            }
        }
    }
}
