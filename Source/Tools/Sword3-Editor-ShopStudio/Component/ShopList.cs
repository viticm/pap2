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
    class ShopList
    {
        private int m_recid = -1;
        private int m_shopid = -1;
        private int m_shopTemplateID = -1;
        private string m_shopName = string.Empty;
        private string m_configFile = string.Empty;
        private bool m_canRepair = false;

        public int RECID
        {
            get { return m_recid; }
            set { m_recid = value; }
        }
        public int ShopID
        {
            get { return m_shopid; }
            set { m_shopid = value; }
        }
        public int ShopTemplateID
        {
            get { return m_shopTemplateID; }
            set { m_shopTemplateID = value; }
        }
        public string ShopName
        {
            get { return m_shopName; }
            set { m_shopName = value; }
        }
        public string ConfigFile
        {
            get { return m_configFile; }
            set { m_configFile = value; }
        }
        public bool CanRepair
        {
            get { return m_canRepair; }
            set { m_canRepair = value; }
        }
    }
}
