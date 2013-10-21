using System;
using System.Collections.Generic;
using System.Text;
using JX2Lib;
namespace JX2Lib
{
   public class JX2ItemDataInfo
    {
        public string itemGuid;
        public string itemName;
       public string item2Account;
       public string itemDescript;
       public DateTime logTime;
       public string eventName;
       public JX2ItemDataInfo()
        {
            itemGuid = "";
            itemName = "";
            item2Account = "";
            itemDescript = "";
            logTime = DateTime.Now;
            eventName = "";
        }
    }

}
