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
    class Item
    {
        private int m_recid = -1;
        private int m_id = -1;
        private string m_name = string.Empty;
        private int m_UiID = -1;
        private int m_gener = -1;
        private int m_subType = -1;
        private int m_detailType = -1;
        private string m_quality = string.Empty;
        private int m_price = -1;
        private string m_bindType = string.Empty;
        private string m_maxExistTime = string.Empty;
        private string m_maxExistAmount = string.Empty;
        private string m_maxDurability = string.Empty;
        private bool m_canStack = false;
        private bool m_canConsume = false;
        private bool m_canTrade = false;
        private bool m_canDestroy = false;
        private string m_scriptName = string.Empty;
        private int m_skillID = -1;
        private string m_skillLevel = string.Empty;

        public int RECID
        {
            get { return m_recid; }
            set { m_recid = value; }
        }
        public int ID
        {
            get { return m_id; }
            set { m_id = value; }
        }
        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        public int UiID
        {
            get { return m_UiID; }
            set { m_UiID = value; }
        }
        public int Gener
        {
            get { return m_gener; }
            set { m_gener = value; }
        }
        public int SubType
        {
            get { return m_subType; }
            set { m_subType = value; }
        }
        public int DetailType
        {
            get { return m_detailType; }
            set { m_detailType = value; }
        }
        public string Quality
        {
            get { return m_quality; }
            set { m_quality = value; }
        }
        public int Price
        {
            get { return m_price; }
            set { m_price = value; }
        }
        public string BindType
        {
            get { return m_bindType; }
            set { m_bindType = value; }
        }
        public string MaxExistTime
        {
            get { return m_maxExistTime; }
            set { m_maxExistTime = value; }
        }
        public string MaxExistAmount
        {
            get { return m_maxExistAmount; }
            set { m_maxExistAmount = value; }
        }
        public string MaxDurability
        {
            get { return m_maxDurability; }
            set { m_maxDurability = value; }
        }
        public bool CanStack
        {
            get { return m_canStack; }
            set { m_canStack = value; }
        }
        public bool CanConsume
        {
            get { return m_canConsume; }
            set { m_canConsume = value; }
        }
        public bool CanTrade
        {
            get { return m_canTrade; }
            set { m_canTrade = value; }
        }
        public bool CanDestroy
        {
            get { return m_canDestroy; }
            set { m_canDestroy = value; }
        }
        public string ScriptName
        {
            get { return m_scriptName; }
            set { m_scriptName = value; }
        }
        public int SkillID
        {
            get { return m_skillID; }
            set { m_skillID = value; }
        }
        public string SkillLevel
        {
            get { return m_skillLevel; }
            set { m_skillLevel = value; }
        }
    }
}
