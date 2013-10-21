/* -----------------------------------------------------------------------*\
 * 文件名     :  IDataProvider.cs
 * 创建者     :  魏伟存 WilsonWei
 * 创建时间   :  2006-1-26
 * 功能描述   :  数据库操作接口
 * 注意事项   :  
 * 版权所有   :  
 *                 Kingsoft Corporation.
 *                
 * -----------------------------------------------------------------------*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Collections.Specialized;
using XSJ.Sword3.Editor.Studio;
using XSJ.Sword3.Editor.Studio.Component;
using XSJ.Sword3.Editor.Studio.Data;

namespace XSJ.Sword3.Editor.Studio.Data
{
	class SqlDataProvider : IDataProviderBase
    {
        private string m_itemTablename = "Items";
        private string m_equipmentTablename = "Equipment";
        private string m_shopItemTablename = "NpcShopItems";
        private string m_shopListTablename = "NpcShopList";
        private string m_itemCategoryTablename = "ItemCategory";

        protected Database m_db = new Database();
        protected SqlParameter[] m_itemParams4Add = new SqlParameter[21];
        protected SqlParameter[] m_equipParams4Add = new SqlParameter[7];
        protected SqlParameter[] m_shopitemParams4Add = new SqlParameter[9];
        protected SqlParameter[] m_shoplistParams4Add = new SqlParameter[5];
        protected SqlParameter[] m_itemCategoryParams4Add = new SqlParameter[2];

        protected SqlParameter[] m_itemParams4Update = new SqlParameter[21];
        protected SqlParameter[] m_equipParams4Update = new SqlParameter[7];
        protected SqlParameter[] m_shopitemParams4Update = new SqlParameter[9];
        protected SqlParameter[] m_shoplistParams4Update = new SqlParameter[5];
        protected SqlParameter[] m_itemCategoryParams4Update = new SqlParameter[2];

        protected string m_Proc_AddItem = "shopstudio_additem";
        protected string m_Proc_AddEquipment = "shopstudio_addequipment";
        protected string m_Proc_AddShopItem = "shopstudio_addshopitem";
        protected string m_Proc_AddShopList = "shopstudio_addshoplist";
        protected string m_Proc_AddItemCategory = "shopstudio_additemcategory";

        protected string m_Proc_UpdateItem = "shopstudio_updateitem";
        protected string m_Proc_UpdateEquipment = "shopstudio_updatequipment";
        protected string m_Proc_UpdateShopItem = "shopstudio_updateshopitem";
        protected string m_Proc_UpdateShopList = "shopstudio_updateshoplist";
        protected string m_Proc_UpdateItemCategory = "shopstudio_updateitemcategory";

        #region IDataProviderBase Members
        /// <summary>
        /// 尝试链接
        /// </summary>
        /// <param name="msg">错误信息</param>
        /// <param name="stackinfo">堆栈信息</param>
        /// <returns>如果可以连接成功,则返回true,否则返回false.</returns>
        public bool TryConnect(out string msg,out string stackinfo)
        {
            return m_db.TryConnect(out msg,out stackinfo);
        }
        public string ItemsTableName
        {
            get { return m_itemTablename; }
        }

        public string ShopItemTableName
        {
            get { return m_shopItemTablename; }
        }

        public string ShopListTableName
        {
            get { return m_shopListTablename; }
        }

        public string EquipmentTableName
        {
            get { return m_equipmentTablename; }
        }
        public string ItemCategoryTableName
        {
            get { return m_itemCategoryTablename; }
        }


        public void Init(string connectionString)
        {
            m_db.SqlConnectionString = connectionString;
            MakeAllParameters();
        }

        private void MakeAllParameters()
        {
            _makeItemParams();
            _makeEquipmentParams();
            _makeShopItemParams();
            _makeShopListParams();
            _makeItemCategoryParams();
        }

        public int GetRecordCount(string sql)
        {
            return m_db.GetRecordCount(sql);
        }

        public int RunSql(string strSqlName, bool UseTransaction)
        {
            return m_db.RunSql(strSqlName, UseTransaction);
        }

        public int RunSql(string strSqlName, SqlParameter[] prams, bool UseTransaction)
        {
            return m_db.RunSql(strSqlName, prams, UseTransaction);
        }

        public void RunSql(string strSqlName, out SqlDataReader dataReader)
        {
            m_db.RunSql(strSqlName, out dataReader);
        }

        public void RunSql(string strSqlName, SqlParameter[] prams, out SqlDataReader dataReader)
        {
            m_db.RunSql(strSqlName, prams, out dataReader);
        }

        public int RunSql2(string strSqlName, SqlParameter[] prams, bool UseTransaction)
        {
            return m_db.RunSql2(strSqlName, prams, UseTransaction);
        }

        public System.Data.DataTable GetDataTableBySql(string sql)
        {
            return m_db.GetTable(sql);
        }

        public Item AddItem(Item item)
        {
            Item obj = null;
            this._setItemParamValues(ref m_itemParams4Add, item);
            int iret = m_db.RunProc2(this.m_Proc_AddItem, this.m_itemParams4Add, true);
            obj = GetOneItem(iret);
            return obj;
        }

        public bool UpdateItem(XSJ.Sword3.Editor.Studio.Component.Item item)
        {
            this._setItemParamValues(ref m_itemParams4Update,item);
            int iret = m_db.RunProc2(this.m_Proc_UpdateItem, this.m_itemParams4Update, true);
            return (iret == 0);
        }

        public bool DeleteItem(int itemid)
        {
            string sql = "delete from " + this.m_itemTablename
                + " where ID=" + itemid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public int GetItemCount()
        {
            string sql = "select count(ID) from " + this.m_itemTablename;
            return m_db.GetRecordCount(sql);
        }

        public Item GetOneItem(int itemid)
        {
            Item obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + m_itemTablename+ " where ID=" + itemid.ToString();
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildItem(ref sdr);
            sdr.Close();
            return obj;
        }

        public ItemCollection GetItemsBySql(string sql)
        {
            return this._getItems(sql);
        }

        public ItemCollection GetItemsBySql(string sql, int PageSize, int PageIndex)
        {
            return this._getItems(sql, PageIndex, PageSize);
        }

        public ItemCollection GetAllItems()
        {
            string sql = "select * from " + this.m_itemTablename;
            return this._getItems(sql);
        }

        public ItemCollection GetAllItems(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            ItemCollection retobjs = null;
            string _DESC = "DESC";
            if (DESC) { _DESC = "DESC"; }
            else { _DESC = "ASC"; }

            string sql = "select * from " + this.m_itemTablename
                + " order by " + SortField + " " + _DESC;

            retobjs = this._getItems(sql, PageIndex, PageSize);
            //try
            //{
            sql = sql.Replace("*", "count(*)");
            sql = sql.Replace("order", "");
            sql = sql.Replace("by", "");
            sql = sql.Replace(_DESC, "");
            sql = sql.Replace(SortField, "");
            retCnt = this.m_db.GetRecordCount(sql);
            //}
            //catch
            //{
            retCnt = 0;
            //}
            return retobjs;
        }
        public ItemCollection GetItemsByCategory(int genre, int subType, int detailType)
        {
            string sql = "select * from " + m_itemTablename
            + " where Genre=" + genre.ToString() + " and SubType=" + subType.ToString()
            + " and DetailType=" + detailType.ToString();
            return GetItemsBySql(sql);
        }
        public Equipment AddEquipment(Equipment equip)
        {
            Equipment obj = null;
            this._setEquipmentParamValues(ref m_equipParams4Add, equip);
            int iret = m_db.RunProc2(this.m_Proc_AddEquipment, this.m_equipParams4Add, true);
            obj = GetOneEquipment(iret);
            return obj;
        }

        public bool UpdateEquipment(Equipment equip)
        {
            this._setEquipmentParamValues(ref this.m_equipParams4Update, equip);
            int iret = m_db.RunProc2(this.m_Proc_UpdateEquipment, this.m_shopitemParams4Update, true);
            return (iret == 0);
        }

        public bool DeleteEquipment(int equipid)
        {
            string sql = "delete from " + this.m_equipmentTablename
            + " where ID=" + equipid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public int GetEquipmentCount()
        {
            string sql = "select count(ID) from " + this.m_equipmentTablename;
            return m_db.GetRecordCount(sql);
        }

        public Equipment GetOneEquipment(int equipid)
        {
            Equipment obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + this.m_equipmentTablename+ " where ID=" + equipid.ToString();
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildEquipment(ref sdr);
            sdr.Close();
            return obj;
        }

        public EquipmentCollection GetEquipmentsBySql(string sql)
        {
            return this._getEquipments(sql);
        }

        public EquipmentCollection GetEquipmentsBySql(string sql, int PageSize, int PageIndex)
        {
            return this._getEquipments(sql, PageIndex, PageSize);
        }

        public EquipmentCollection GetAllEquipments()
        {
            string sql = "select * from " + this.m_equipmentTablename;
            return this._getEquipments(sql);
        }

        public EquipmentCollection GetAllEquipments(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            EquipmentCollection retobjs = null;
            string _DESC = "DESC";
            if (DESC) { _DESC = "DESC"; }
            else { _DESC = "ASC"; }

            string sql = "select * from " + this.m_equipmentTablename
                + " order by " + SortField + " " + _DESC;

            retobjs = this._getEquipments(sql, PageIndex, PageSize);
            //try
            //{
            sql = sql.Replace("*", "count(*)");
            sql = sql.Replace("order", "");
            sql = sql.Replace("by", "");
            sql = sql.Replace(_DESC, "");
            sql = sql.Replace(SortField, "");
            retCnt = this.m_db.GetRecordCount(sql);
            //}
            //catch
            //{
            retCnt = 0;
            //}
            return retobjs;
        }

        public EquipmentCollection GetEquipmentsByParentID(int parentid)
        {
            string sql = "select * from " + this.m_equipmentTablename
                + " where ParentID=" + parentid.ToString();
            return this._getEquipments(sql);
        }

        public ShopItem AddShopItem(ShopItem shop)
        {
            ShopItem obj = null;
            this._setShopItemParamValues(ref m_shopitemParams4Add, shop);
            int iret = m_db.RunProc2(this.m_Proc_AddShopItem, this.m_shopitemParams4Add, true);
            obj = GetOneShopItem(iret);
            return obj;
        }

        public bool UpdateShopItem(ShopItem shop)
        {
            this._setShopItemParamValues(ref this.m_shopitemParams4Update, shop);
            int iret = m_db.RunProc2(this.m_Proc_UpdateShopItem, this.m_shopitemParams4Update, false);
            //int iret = m_db.RunSql("update " + this.m_shopItemTablename 
            //    + " set ShopTemplateID=" +shop.ShopTemplateID
            //    +" TabType='" +shop.TabType +"'"
            //    +" ItemIndex=" +shop.ItemIndex
            //    +" RandSeed='" +shop.RandSeed +"'"
            //    +" CountLimit='" +shop.CountLimit +"'"
            //    +" ReputeLevel='" +shop.ReputeLevel +"'"
            //    +" Price=" +shop.Price
            //    +" where ShopItemID=" +shop.ShopItemID,false);
            return (iret == 0);
        }

        public bool DeleteShopItem(int shopid)
        {
            string sql = "delete from " + this.m_shopItemTablename
            + " where ShopItemID=" + shopid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public int GetShopItemCount()
        {
            string sql = "select count(ID) from " + this.m_shopItemTablename;
            return m_db.GetRecordCount(sql);
        }

        public ShopItem GetOneShopItem(int shopid)
        {
            ShopItem obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + this.m_shopItemTablename + " where ShopTemplateID=" + shopid.ToString();
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildShopItem(ref sdr);
            sdr.Close();
            return obj;
        }

        public ShopItemCollection GetShopItemsBySql(string sql)
        {
            return this._getShopItems(sql);
        }

        public ShopItemCollection GetShopItemsBySql(string sql, int PageSize, int PageIndex)
        {
            return this._getShopItems(sql, PageIndex, PageSize);
        }

        public ShopItemCollection GetAllShopItems()
        {
            string sql = "select * from " + this.m_shopItemTablename
                    + " order by SortIndex asc";
            return this._getShopItems(sql);
        }

        public ShopItemCollection GetAllShopItems(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            ShopItemCollection retobjs = null;
            string _DESC = "DESC";
            if (DESC) { _DESC = "DESC"; }
            else { _DESC = "ASC"; }

            string sql = "select * from " + this.m_shopItemTablename
                + " order by " + SortField + " " + _DESC;

            retobjs = this._getShopItems(sql, PageIndex, PageSize);
            //try
            //{
            sql = sql.Replace("*", "count(*)");
            sql = sql.Replace("order", "");
            sql = sql.Replace("by", "");
            sql = sql.Replace(_DESC, "");
            sql = sql.Replace(SortField, "");
            retCnt = this.m_db.GetRecordCount(sql);
            //}
            //catch
            //{
            retCnt = 0;
            //}
            return retobjs;
        }
        public ShopItemCollection GetShopItemsByShop(int shopid)
        {
            string sql = "select * from " + m_shopItemTablename
            + " where ShopTemplateID=" + shopid.ToString()
            + " order by SortIndex asc";
            return this.GetShopItemsBySql(sql);
        }
        public bool IsItemExistsInShop(int itemIndex, int shopid)
        {
            string sql = "select RECID from " + m_shopItemTablename
            + " where ItemIndex=" + itemIndex.ToString()
            + " and ShopTemplateID=" + shopid.ToString();
            int iret = m_db.RunSql2(sql);
            return ( iret >0);
        }
        public bool DeleteShopItemsByShopTemplateID(int shoptemplateid)
        {
            string sql = "delete from " + this.m_shopItemTablename
            + " where ShopTemplateID=" + shoptemplateid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public ShopList AddShopList(ShopList shoplist)
        {
            ShopList obj = null;
            this._setShopListParamValues(ref m_shoplistParams4Add, shoplist);
            int iret = m_db.RunProc2(this.m_Proc_AddShopList,this.m_shoplistParams4Add, true);
            obj = GetOneShopList(iret);
            return obj;
        }

        public bool UpdateShopList(ShopList shoplist)
        {
            this._setShopListParamValues(ref this.m_shoplistParams4Update, shoplist);
            int iret = m_db.RunProc2(this.m_Proc_UpdateShopList, this.m_shoplistParams4Update, true);
            return (iret == 0);
        }

        public bool DeleteShopList(int shoplistid)
        {
            string sql = "delete from " + this.m_shopListTablename
            + " where ShopID=" + shoplistid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public int GetShopListCount()
        {
            string sql = "select count(ShopTemplateID) from " + this.m_shopListTablename;
            return m_db.GetRecordCount(sql);
        }

        public ShopList GetOneShopList(int shoplistid)
        {
            ShopList obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + this.m_shopListTablename 
                + " where ShopID=" + shoplistid.ToString();
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildShopList(ref sdr);
            sdr.Close();
            return obj;
        }
        public ShopList GetOneShopListByShopName(string shopname)
        {
            ShopList obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + this.m_shopListTablename
                + " where ShopName='" + shopname + "'";
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildShopList(ref sdr);
            sdr.Close();
            return obj;
        }
        public bool IsShopListExists(string shopname)
        {
            string sql = "select count(ShopTemplateID) from " + this.m_shopListTablename
            + " where ShopName='" + shopname + "'";
            return (m_db.GetRecordCount(sql) >0);
        }
        public ShopListCollection GetShopListsBySql(string sql)
        {
            return this._getShopLists(sql);
        }

        public ShopListCollection GetShopListsBySql(string sql, int PageSize, int PageIndex)
        {
            return this._getShopLists(sql, PageIndex, PageSize);
        }

        public ShopListCollection GetAllShopLists()
        {
            string sql = "select * from " + this.m_shopListTablename;
            return this._getShopLists(sql);
        }

        public ShopListCollection GetAllShopLists(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            ShopListCollection retobjs = null;
            string _DESC = "DESC";
            if (DESC) { _DESC = "DESC"; }
            else { _DESC = "ASC"; }

            string sql = "select * from " + this.m_shopListTablename
                + " order by " + SortField + " " + _DESC;

            retobjs = this._getShopLists(sql, PageIndex, PageSize);
            //try
            //{
            sql = sql.Replace("*", "count(*)");
            sql = sql.Replace("order", "");
            sql = sql.Replace("by", "");
            sql = sql.Replace(_DESC, "");
            sql = sql.Replace(SortField, "");
            retCnt = this.m_db.GetRecordCount(sql);
            //}
            //catch
            //{
            retCnt = 0;
            //}
            return retobjs;
        }


        public ItemCategory AddItemCategory(ItemCategory itemcategory)
        {
            ItemCategory obj = null;
            this._setItemCategoryParamValues(ref m_itemCategoryParams4Add, itemcategory);
            int iret = m_db.RunProc2(this.m_Proc_AddItemCategory, this.m_itemCategoryParams4Add, true);
            obj = GetOneItemCategory(iret);
            return obj;
        }

        public bool UpdateItemCategory(ItemCategory itemcategory)
        {
            this._setItemCategoryParamValues(ref this.m_itemCategoryParams4Update, itemcategory);
            int iret = m_db.RunProc2(this.m_Proc_UpdateItemCategory, this.m_itemCategoryParams4Update, true);
            return (iret == 0);
        }

        public bool DeleteItemCategory(int itemcategoryid)
        {
            string sql = "delete from " + this.m_itemCategoryTablename
           + " where RECID=" + itemcategoryid.ToString();
            int iret = m_db.RunSql(sql, true);
            return (iret == 0);
        }

        public int GetItemCategoryCount()
        {
            string sql = "select count(RECID) from " + this.ItemCategoryTableName;
            return m_db.GetRecordCount(sql);
        }

        public ItemCategory GetOneItemCategory(int itemcategoryid)
        {
            ItemCategory obj = null;
            SqlDataReader sdr = null;
            string sql = "select * from " + this.ItemCategoryTableName + " where RECID=" + itemcategoryid.ToString();
            this.m_db.RunSql(sql, out sdr);
            sdr.Read();
            obj = this._buildItemCategory(ref sdr);
            sdr.Close();
            return obj;
        }

        public ItemCategoryCollection GetItemCategoriesBySql(string sql)
        {
            return this._getItemCategories(sql);
        }

        public ItemCategoryCollection GetItemCategoriesBySql(string sql, int PageSize, int PageIndex)
        {
            return this._getItemCategories(sql, PageIndex, PageSize);
        }

        public ItemCategoryCollection GetAllItemCategories()
        {
            string sql = "select * from " + this.m_itemCategoryTablename;
            return this._getItemCategories(sql);
        }

        public ItemCategoryCollection GetItemCategories(int PageSize, int PageIndex, string SortField, bool DESC, out int retCnt)
        {
            ItemCategoryCollection retobjs = null;
            string _DESC = "DESC";
            if (DESC) { _DESC = "DESC"; }
            else { _DESC = "ASC"; }

            string sql = "select * from " + this.m_itemCategoryTablename
                + " order by " + SortField + " " + _DESC;

            retobjs = this._getItemCategories(sql, PageIndex, PageSize);
            //try
            //{
            sql = sql.Replace("*", "count(*)");
            sql = sql.Replace("order", "");
            sql = sql.Replace("by", "");
            sql = sql.Replace(_DESC, "");
            sql = sql.Replace(SortField, "");
            retCnt = this.m_db.GetRecordCount(sql);
            //}
            //catch
            //{
            retCnt = 0;
            //}
            return retobjs;
        }

        #endregion

        #region private methods
        private void _makeItemParams()
        {
            SqlParameter sp = null;
            sp = m_db.MakeInParam("ID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[0] = sp;
            sp = m_db.MakeInParam("Name", System.Data.SqlDbType.VarChar, 500, null);
            m_itemParams4Add[1] = sp;
            sp = m_db.MakeInParam("UiID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[2] = sp;
            sp = m_db.MakeInParam("Genre", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[3] = sp;
            sp = m_db.MakeInParam("SubType", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[4] = sp;
            sp = m_db.MakeInParam("DetailType", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[5] = sp;
            sp = m_db.MakeInParam("Quality", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[6] = sp;
            sp = m_db.MakeInParam("Price", System.Data.SqlDbType.Money, 8, null);
            m_itemParams4Add[7] = sp;
            sp = m_db.MakeInParam("BindType", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[8] = sp;
            sp = m_db.MakeInParam("MaxExistTime", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[9] = sp;
            sp = m_db.MakeInParam("MaxExistAmount", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[10] = sp;
            sp = m_db.MakeInParam("MaxDurability", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[11] = sp;
            sp = m_db.MakeInParam("CanStack", System.Data.SqlDbType.TinyInt, 1, null);
            m_itemParams4Add[12] = sp;
            sp = m_db.MakeInParam("CanConsume", System.Data.SqlDbType.TinyInt,1, null);
            m_itemParams4Add[13] = sp;
            sp = m_db.MakeInParam("CanTrade", System.Data.SqlDbType.TinyInt,1,null);
            m_itemParams4Add[14] = sp;
            sp = m_db.MakeInParam("CanDestroy", System.Data.SqlDbType.TinyInt,1, null);
            m_itemParams4Add[15] = sp;
            sp = m_db.MakeInParam("ScriptName", System.Data.SqlDbType.VarChar, 8000, null);
            m_itemParams4Add[16] = sp;
            sp = m_db.MakeInParam("SkillID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Add[17] = sp;
            sp = m_db.MakeInParam("SkillLevel", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Add[18] = sp;

            sp = m_db.MakeInParam("ID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[0] = sp;
            sp = m_db.MakeInParam("Name", System.Data.SqlDbType.VarChar, 500, null);
            m_itemParams4Update[1] = sp;
            sp = m_db.MakeInParam("UiID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[2] = sp;
            sp = m_db.MakeInParam("Genre", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[3] = sp;
            sp = m_db.MakeInParam("SubType", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[4] = sp;
            sp = m_db.MakeInParam("DetailType", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[5] = sp;
            sp = m_db.MakeInParam("Quality", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[6] = sp;
            sp = m_db.MakeInParam("Price", System.Data.SqlDbType.Money, 8, null);
            m_itemParams4Update[7] = sp;
            sp = m_db.MakeInParam("BindType", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[8] = sp;
            sp = m_db.MakeInParam("MaxExistTime", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[9] = sp;
            sp = m_db.MakeInParam("MaxExistAmount", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[10] = sp;
            sp = m_db.MakeInParam("MaxDurability", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[11] = sp;
            sp = m_db.MakeInParam("CanStack", System.Data.SqlDbType.TinyInt, 1, null);
            m_itemParams4Update[12] = sp;
            sp = m_db.MakeInParam("CanConsume", System.Data.SqlDbType.TinyInt,1, null);
            m_itemParams4Update[13] = sp;
            sp = m_db.MakeInParam("CanTrade", System.Data.SqlDbType.TinyInt,1, null);
            m_itemParams4Update[14] = sp;
            sp = m_db.MakeInParam("CanDestroy", System.Data.SqlDbType.TinyInt, 1, null);
            m_itemParams4Update[15] = sp;
            sp = m_db.MakeInParam("ScriptName", System.Data.SqlDbType.VarChar, 8000, null);
            m_itemParams4Update[18] = sp;
            sp = m_db.MakeInParam("SkillID", System.Data.SqlDbType.Int, 4, null);
            m_itemParams4Update[19] = sp;
            sp = m_db.MakeInParam("SkillLevel", System.Data.SqlDbType.VarChar, 50, null);
            m_itemParams4Update[20] = sp;
        }
        private void _makeEquipmentParams()
        {
            SqlParameter sp = null;
            sp = m_db.MakeInParam("ID", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Add[0] = sp;
            sp = m_db.MakeInParam("ParentID", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Add[1] = sp;
            sp = m_db.MakeInParam("Name", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Add[2] = sp;
            sp = m_db.MakeInParam("Comment", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Add[3] = sp;
            sp = m_db.MakeInParam("Genre", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Add[4] = sp;
            sp = m_db.MakeInParam("SubType", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Add[5] = sp;
            sp = m_db.MakeInParam("DetailType", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Add[6] = sp;

            sp = m_db.MakeInParam("ID", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Update[0] = sp;
            sp = m_db.MakeInParam("ParentID", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Update[1] = sp;
            sp = m_db.MakeInParam("Name", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Update[2] = sp;
            sp = m_db.MakeInParam("Comment", System.Data.SqlDbType.VarChar, 50, null);
            m_equipParams4Update[3] = sp;
            sp = m_db.MakeInParam("Genre", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Update[4] = sp;
            sp = m_db.MakeInParam("SubType", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Update[5] = sp;
            sp = m_db.MakeInParam("DetailType", System.Data.SqlDbType.Int, 4, null);
            m_equipParams4Update[6] = sp;
        }
        private void _makeShopItemParams()
        {
            SqlParameter sp = null;
            sp = m_db.MakeInParam("ShopItemID", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Add[0] = sp;
            sp = m_db.MakeInParam("ShopTemplateID", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Add[1] = sp;
            sp = m_db.MakeInParam("TabType", System.Data.SqlDbType.VarChar, 500, null);
            m_shopitemParams4Add[2] = sp;
            sp = m_db.MakeInParam("ItemIndex", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Add[3] = sp;
            sp = m_db.MakeInParam("RandSeed", System.Data.SqlDbType.VarChar,50, null);
            m_shopitemParams4Add[4] = sp;
            sp = m_db.MakeInParam("CountLimit", System.Data.SqlDbType.VarChar,50, null);
            m_shopitemParams4Add[5] = sp;
            sp = m_db.MakeInParam("ReputeLevel", System.Data.SqlDbType.VarChar, 50, null);
            m_shopitemParams4Add[6] = sp;
            sp = m_db.MakeInParam("Price", System.Data.SqlDbType.Money, 8, null);
            m_shopitemParams4Add[7] = sp;
            sp = m_db.MakeInParam("SortIndex", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Add[8] = sp;

            sp = m_db.MakeInParam("ShopItemID", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Update[0] = sp;
            sp = m_db.MakeInParam("ShopTemplateID", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Update[1] = sp;
            sp = m_db.MakeInParam("TabType", System.Data.SqlDbType.VarChar, 500, null);
            m_shopitemParams4Update[2] = sp;
            sp = m_db.MakeInParam("ItemIndex", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Update[3] = sp;
            sp = m_db.MakeInParam("RandSeed", System.Data.SqlDbType.VarChar, 50, null);
            m_shopitemParams4Update[4] = sp;
            sp = m_db.MakeInParam("CountLimit", System.Data.SqlDbType.VarChar, 50, null);
            m_shopitemParams4Update[5] = sp;
            sp = m_db.MakeInParam("ReputeLevel", System.Data.SqlDbType.VarChar, 50, null);
            m_shopitemParams4Update[6] = sp;
            sp = m_db.MakeInParam("Price", System.Data.SqlDbType.Money, 8, null);
            m_shopitemParams4Update[7] = sp;
            sp = m_db.MakeInParam("SortIndex", System.Data.SqlDbType.Int, 4, null);
            m_shopitemParams4Update[8] = sp;
        }
        private void _makeShopListParams()
        {
            SqlParameter sp = null;
            sp = m_db.MakeInParam("ShopID", System.Data.SqlDbType.Int, 4, null);
            m_shoplistParams4Add[0] = sp;
            sp = m_db.MakeInParam("ShopTemplateID", System.Data.SqlDbType.Int, 4, null);
            m_shoplistParams4Add[1] = sp;
            sp = m_db.MakeInParam("ShopName", System.Data.SqlDbType.VarChar, 500, null);
            m_shoplistParams4Add[2] = sp;
            sp = m_db.MakeInParam("ConfigFile", System.Data.SqlDbType.VarChar, 500, null);
            m_shoplistParams4Add[3] = sp;
            sp = m_db.MakeInParam("CanRepair", System.Data.SqlDbType.TinyInt, 1, null);
            m_shoplistParams4Add[4] = sp;

            sp = m_db.MakeInParam("ShopID", System.Data.SqlDbType.Int, 4, null);
            m_shoplistParams4Update[0] = sp;
            sp = m_db.MakeInParam("ShopTemplateID", System.Data.SqlDbType.Int, 4, null);
            m_shoplistParams4Update[1] = sp;
            sp = m_db.MakeInParam("ShopName", System.Data.SqlDbType.VarChar, 500, null);
            m_shoplistParams4Update[2] = sp;
            sp = m_db.MakeInParam("ConfigFile", System.Data.SqlDbType.VarChar, 500, null);
            m_shoplistParams4Update[3] = sp;
            sp = m_db.MakeInParam("CanRepair", System.Data.SqlDbType.TinyInt, 1, null);
            m_shoplistParams4Update[4] = sp;
        }
        private void _makeItemCategoryParams()
        {
            //SqlParameter sp = null;
            //sp = m_db.MakeInParam("RECID", System.Data.SqlDbType.Int, 4, null);
            
        }

        #endregion

        #region Build Object
        private Item _buildItem(ref SqlDataReader sdr)
        {
            if (null == sdr || false == sdr.HasRows) return null;
            Item item = new Item();
            item.RECID = Convert.ToInt32(sdr["RECID"]);
            item.ID = Convert.ToInt32(sdr["ID"]);
            item.BindType = Convert.ToString(sdr["BindType"]);
            item.CanConsume = Convert.ToBoolean(sdr["CanConsume"]);
            item.CanDestroy = Convert.ToBoolean(sdr["CanDestroy"]);
            item.CanStack = Convert.ToBoolean(sdr["CanStack"]);
            item.CanTrade = Convert.ToBoolean(sdr["CanTrade"]);
            item.DetailType = Convert.ToInt32(sdr["DetailType"]);
            item.Gener = Convert.ToInt32(sdr["Genre"]);
            item.MaxDurability = Convert.ToString(sdr["MaxDurability"]);
            item.MaxExistAmount = Convert.ToString(sdr["MaxExistAmount"]);
            item.MaxExistTime = Convert.ToString(sdr["MaxExistTime"]);
            item.Name = Convert.ToString(sdr["Name"]);
            item.Price = Convert.ToInt32(sdr["Price"]);
            item.Quality = Convert.ToString(sdr["Quality"]);
            item.ScriptName = Convert.ToString(sdr["ScriptName"]);
            if(DBNull.Value != sdr["SkillID"])
                item.SkillID = Convert.ToInt32(sdr["SkillID"]);
            item.SkillLevel = Convert.ToString(sdr["SkillLevel"]);
            item.SubType = Convert.ToInt32(sdr["SubType"]);
            item.UiID = Convert.ToInt32(sdr["UiID"]);
            return item;
        }
        private Equipment _buildEquipment(ref SqlDataReader sdr)
        {
            if (null == sdr || false == sdr.HasRows) return null;
            Equipment equip = new Equipment();
            equip.RECID = Convert.ToInt32(sdr["RECID"]);
            equip.ID = Convert.ToInt32(sdr["ID"]);
            equip.Comment = Convert.ToString(sdr["Comment"]);
            equip.DetailType = Convert.ToInt32(sdr["DetailType"]);
            equip.Gener = Convert.ToInt32(sdr["Genre"]);
            equip.Name = Convert.ToString(sdr["Name"]);
            equip.ParentID = Convert.ToString(sdr["ParentID"]);
            equip.SubType = Convert.ToInt32(sdr["SubType"]);
            return equip;
        }
        private ShopItem _buildShopItem(ref SqlDataReader sdr)
        {
            if (null == sdr || false == sdr.HasRows) return null;
            ShopItem shopitem = new ShopItem();
            shopitem.RECID = Convert.ToInt32(sdr["RECID"]);
            shopitem.ShopItemID = Convert.ToInt32(sdr["ShopItemID"]);
            shopitem.ShopTemplateID = Convert.ToInt32(sdr["ShopTemplateID"]);
            shopitem.CountLimit = Convert.ToString(sdr["CountLimit"]);
            shopitem.ItemIndex = Convert.ToInt32(sdr["ItemIndex"]);
            shopitem.Price = Convert.ToInt32(sdr["Price"]);
            shopitem.RandSeed = Convert.ToString(sdr["RandSeed"]);
            shopitem.ReputeLevel = Convert.ToString(sdr["ReputeLevel"]);
            shopitem.TabType = Convert.ToString(sdr["TabType"]);
            shopitem.SortIndex = Convert.ToInt32(sdr["SortIndex"]);
            return shopitem;
        }
        private ShopList _buildShopList(ref SqlDataReader sdr)
        {
            if (null == sdr || false == sdr.HasRows) return null;
            ShopList shoplist = new ShopList();
            shoplist.RECID = Convert.ToInt32(sdr["RECID"]);
            shoplist.ShopID = Convert.ToInt32(sdr["ShopID"]);
            shoplist.ShopTemplateID = Convert.ToInt32(sdr["ShopTemplateID"]);
            shoplist.CanRepair = Convert.ToBoolean(sdr["CanRepair"]);
            shoplist.ConfigFile = Convert.ToString(sdr["ConfigFile"]);
            shoplist.ShopName = Convert.ToString(sdr["ShopName"]);
            return shoplist;
        }
        private ItemCategory _buildItemCategory(ref SqlDataReader sdr)
        {
            if (null == sdr || false == sdr.HasRows) return null;
            ItemCategory itemcategory = new ItemCategory();
            itemcategory.RECID = Convert.ToInt32(sdr["RECID"]);
            itemcategory.CategoryName = Convert.ToString(sdr["CategoryName"]);
            itemcategory.Filename = Convert.ToString(sdr["Filename"]);
            return itemcategory;
        }
        #endregion

        #region Get many records
        private ItemCollection _getItems(string sql)
        {
            ItemCollection retobjs = new ItemCollection();
            SqlDataReader sdr = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            while (sdr.Read())
            {
                retobjs.Add(this._buildItem(ref sdr ));
            }
            sdr.Close();
            return retobjs;
        }
        private ItemCollection _getItems(string sql, int pPageIndex, int pPageSize)
        {
            ItemCollection retobjs = new ItemCollection();
            SqlDataReader sdr = null;
            Item obj = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            int _curRecPos = 0;
            //计算页记录///
            long _startpos = 0;
            long _endpos = 0;
            if (pPageIndex > -1)
            {
                _startpos = pPageIndex * pPageSize;
                _endpos = _startpos + pPageSize;
            }
            while (sdr.Read())
            {
                if (_curRecPos > _endpos) break;
                if (_curRecPos >= _startpos && _curRecPos < _endpos)
                {
                    obj = this._buildItem(ref sdr);
                    if (null != obj)
                    {
                        retobjs.Add(obj);
                    }
                }
                _curRecPos++;
            }
            sdr.Close();
            if (retobjs.Count > 0)
            {
                return retobjs;
            }
            else
            {
                return null;
            }
        }

        private EquipmentCollection _getEquipments(string sql)
        {
            EquipmentCollection retobjs = new EquipmentCollection();
            SqlDataReader sdr = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            while (sdr.Read())
            {
                retobjs.Add(this._buildEquipment(ref sdr));
            }
            sdr.Close();
            return retobjs;
        }
        private EquipmentCollection _getEquipments(string sql, int pPageIndex, int pPageSize)
        {
            EquipmentCollection retobjs = new EquipmentCollection();
            SqlDataReader sdr = null;
            Equipment obj = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            int _curRecPos = 0;
            //计算页记录///
            long _startpos = 0;
            long _endpos = 0;
            if (pPageIndex > -1)
            {
                _startpos = pPageIndex * pPageSize;
                _endpos = _startpos + pPageSize;
            }
            while (sdr.Read())
            {
                if (_curRecPos > _endpos) break;
                if (_curRecPos >= _startpos && _curRecPos < _endpos)
                {
                    obj = this._buildEquipment(ref sdr);
                    if (null != obj)
                    {
                        retobjs.Add(obj);
                    }
                }
                _curRecPos++;
            }
            sdr.Close();
            if (retobjs.Count > 0)
            {
                return retobjs;
            }
            else
            {
                return null;
            }
        }

        private ShopItemCollection _getShopItems(string sql)
        {
            ShopItemCollection retobjs = new ShopItemCollection();
            SqlDataReader sdr = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            while (sdr.Read())
            {
                retobjs.Add(this._buildShopItem(ref sdr));
            }
            sdr.Close();
            return retobjs;
        }
        private ShopItemCollection _getShopItems(string sql, int pPageIndex, int pPageSize)
        {
            ShopItemCollection retobjs = new ShopItemCollection();
            SqlDataReader sdr = null;
            ShopItem obj = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            int _curRecPos = 0;
            //计算页记录///
            long _startpos = 0;
            long _endpos = 0;
            if (pPageIndex > -1)
            {
                _startpos = pPageIndex * pPageSize;
                _endpos = _startpos + pPageSize;
            }
            while (sdr.Read())
            {
                if (_curRecPos > _endpos) break;
                if (_curRecPos >= _startpos && _curRecPos < _endpos)
                {
                    obj = this._buildShopItem(ref sdr);
                    if (null != obj)
                    {
                        retobjs.Add(obj);
                    }
                }
                _curRecPos++;
            }
            sdr.Close();
            if (retobjs.Count > 0)
            {
                return retobjs;
            }
            else
            {
                return null;
            }
        }
        private ShopListCollection _getShopLists(string sql)
        {
            ShopListCollection retobjs = new ShopListCollection();
            SqlDataReader sdr = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            while (sdr.Read())
            {
                retobjs.Add(this._buildShopList(ref sdr));
            }
            sdr.Close();
            return retobjs;
        }
        private ShopListCollection _getShopLists(string sql, int pPageIndex, int pPageSize)
        {
            ShopListCollection retobjs = new ShopListCollection();
            SqlDataReader sdr = null;
            ShopList obj = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            int _curRecPos = 0;
            //计算页记录///
            long _startpos = 0;
            long _endpos = 0;
            if (pPageIndex > -1)
            {
                _startpos = pPageIndex * pPageSize;
                _endpos = _startpos + pPageSize;
            }
            while (sdr.Read())
            {
                if (_curRecPos > _endpos) break;
                if (_curRecPos >= _startpos && _curRecPos < _endpos)
                {
                    obj = this._buildShopList(ref sdr);
                    if (null != obj)
                    {
                        retobjs.Add(obj);
                    }
                }
                _curRecPos++;
            }
            sdr.Close();
            if (retobjs.Count > 0)
            {
                return retobjs;
            }
            else
            {
                return null;
            }
        }
        private ItemCategoryCollection _getItemCategories(string sql)
        {
            ItemCategoryCollection retobjs = new ItemCategoryCollection();
            SqlDataReader sdr = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            while (sdr.Read())
            {
                retobjs.Add(this._buildItemCategory(ref sdr));
            }
            sdr.Close();
            return retobjs;
        }
        private ItemCategoryCollection _getItemCategories(string sql, int pPageIndex, int pPageSize)
        {
            ItemCategoryCollection retobjs = new ItemCategoryCollection();
            SqlDataReader sdr = null;
            ItemCategory obj = null;
            m_db.RunSql(sql, out sdr);
            if (null == sdr) return null;
            int _curRecPos = 0;
            //计算页记录///
            long _startpos = 0;
            long _endpos = 0;
            if (pPageIndex > -1)
            {
                _startpos = pPageIndex * pPageSize;
                _endpos = _startpos + pPageSize;
            }
            while (sdr.Read())
            {
                if (_curRecPos > _endpos) break;
                if (_curRecPos >= _startpos && _curRecPos < _endpos)
                {
                    obj = this._buildItemCategory(ref sdr);
                    if (null != obj)
                    {
                        retobjs.Add(obj);
                    }
                }
                _curRecPos++;
            }
            sdr.Close();
            if (retobjs.Count > 0)
            {
                return retobjs;
            }
            else
            {
                return null;
            }
        }
        #endregion

        #region Set Parameters' value
        private void _setItemParamValues(ref SqlParameter[] parm, Item item)
        {
            parm[0].Value = item.ID;
            parm[1].Value = item.Name;
            parm[2].Value = item.UiID;
            parm[3].Value = item.Gener;
            parm[4].Value = item.SubType;
            parm[5].Value = item.DetailType;
            parm[6].Value = item.Quality;
            parm[7].Value = item.Price;
            parm[8].Value = item.BindType;
            parm[9].Value = item.MaxExistTime;
            parm[10].Value = item.MaxExistAmount;
            parm[11].Value = item.MaxDurability;
            parm[12].Value = item.CanStack;
            parm[13].Value = item.CanConsume;
            parm[14].Value = item.CanTrade;
            parm[15].Value = item.CanDestroy;
            parm[16].Value = item.ScriptName;
            parm[17].Value = item.SkillID;
            parm[18].Value = item.SkillLevel;
        }
        private void _setEquipmentParamValues(ref SqlParameter[] parm, Equipment equip)
        {
            parm[0].Value = equip.ID;
            parm[1].Value = equip.ParentID;
            parm[2].Value = equip.Name;
            parm[3].Value = equip.Comment;
            parm[4].Value = equip.Gener;
            parm[5].Value = equip.SubType;
            parm[6].Value = equip.DetailType;
        }
        private void _setShopItemParamValues(ref SqlParameter[] parm, ShopItem shopitem)
        {
            parm[0].Value = shopitem.ShopItemID;
            parm[1].Value = shopitem.ShopTemplateID;
            parm[2].Value = shopitem.TabType;
            parm[3].Value = shopitem.ItemIndex;
            parm[4].Value = shopitem.RandSeed;
            parm[5].Value = shopitem.CountLimit;
            parm[6].Value = shopitem.ReputeLevel;
            parm[7].Value = shopitem.Price;
            parm[8].Value = shopitem.SortIndex;
        }
        private void _setShopListParamValues(ref SqlParameter[] parm, ShopList shoplist)
        {
            parm[0].Value = shoplist.ShopID;
            parm[1].Value = shoplist.ShopTemplateID;
            parm[2].Value = shoplist.ShopName;
            parm[3].Value = shoplist.ConfigFile;
            parm[4].Value = shoplist.CanRepair;
        }
        private void _setItemCategoryParamValues(ref SqlParameter[] parm, ItemCategory itemcategory)
        {
            parm[0].Value = itemcategory.CategoryName;
            parm[1].Value = itemcategory.Filename;
        }
        #endregion

    }
}
