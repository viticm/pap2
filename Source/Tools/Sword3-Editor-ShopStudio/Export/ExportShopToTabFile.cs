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
using System.IO;
using XSJ.Sword3.Editor.Studio.Component;
using XSJ.Sword3.Editor.Studio.Data;
using XSJ.Sword3.Editor.Studio.AppLogic;
using XSJ.Sword3.Editor.Studio;

namespace XSJ.Sword3.Editor.Studio.Export
{
    class ExportShopToTabFile
    {
        static public void Export(string filename,ref ShopItemCollection shopitems)
        {
            if (filename == "" && null == shopitems && shopitems.Count <= 0)
            {
                return;
            }
            string str = string.Empty;
            StreamWriter sw = new StreamWriter(filename,false,Encoding.ASCII);
            str = "TabType\tItemIndex\tRandSeed\tCountLimit\tReputeLevel\tPrice";
            sw.WriteLine(str);
            foreach (ShopItem item in shopitems)
            {
                str = item.TabType + "\t"
                        + item.ItemIndex + "\t"
                        + item.RandSeed + "\t"
                        + item.CountLimit + "\t"
                        + item.ReputeLevel + "\t"
                        + item.Price;
                sw.WriteLine(str);
            }
            sw.Flush();
            sw.Close();
        }
    }
}
