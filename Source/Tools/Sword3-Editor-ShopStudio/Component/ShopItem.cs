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
    class ShopItem
    {
        private int m_recid = -1;
        private int m_shopItemID = -1;
        private int m_shopTemplateID = -1;
        private string m_tabType = string.Empty;
        private int m_itemIndex = -1;
        private string m_randSeed = string.Empty;
        private string m_countLimit = string.Empty;
        private string m_reputeLevel = string.Empty;
        private int m_price = -1;
        private int m_sortIndex = -1;

        public int RECID
        {
            get { return m_recid; }
            set { m_recid = value; }
        }
        public int ShopItemID
        {
            get { return m_shopItemID; }
            set { m_shopItemID = value; }
        }
        public int ShopTemplateID
        {
            get { return m_shopTemplateID; }
            set { m_shopTemplateID = value; }
        }
        public string TabType
        {
            get { return m_tabType; }
            set { m_tabType = value; }
        }
        public int ItemIndex
        {
            get { return m_itemIndex; }
            set { m_itemIndex = value; }
        }
        public string RandSeed
        {
            get { return m_randSeed; }
            set { m_randSeed = value; }
        }
        public string CountLimit
        {
            get { return m_countLimit; }
            set { m_countLimit = value; }
        }
        public string ReputeLevel
        {
            get { return m_reputeLevel; }
            set { m_reputeLevel = value; }
        }
        public int Price
        {
            get { return m_price; }
            set { m_price = value; }
        }
        public int SortIndex
        {
            get { return m_sortIndex; }
            set { m_sortIndex = value; }
        }
        public ShopItem Clone()
        {
            ShopItem shop = new ShopItem();
            shop.ShopItemID = m_shopItemID;
            shop.CountLimit = m_countLimit;
            shop.ItemIndex = m_itemIndex;
            shop.Price = m_price;
            shop.RandSeed = m_randSeed;
            shop.RECID= m_recid;
            shop.ReputeLevel = m_reputeLevel;
            shop.ShopTemplateID = m_shopTemplateID;
            shop.TabType = m_tabType;
            shop.SortIndex = m_sortIndex;
            return shop;
        }
    }
}
