using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Collections.Generic;
using FSEye.FS2Lib;
using Resources;

/// <summary>
/// Summary description for FS2GameData
/// </summary>
[WebService(Namespace = "fseye.blaze.kingsoft")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class FS2GameData : System.Web.Services.WebService
{

    public FS2GameData()
    {

        //Uncomment the following line if using designed components 
        //InitializeComponent(); 
    }

    [WebMethod]
    public string[] GetItemNameList(string prefixText, int count)
    {
        if (count == 0)
        {
            count = 10;
        }

        List<string> items = new List<string>(count);

        Hashtable itemHashtable = FS2GameDataManager.TheInstance.ItemHashtable;
        if (itemHashtable != null)
        {
            foreach (FS2ItemData itemData in itemHashtable.Values)
            {
                if (items.Count >= count) break;
                if (itemData != null)
                {
                    if (itemData.Name.Contains(prefixText))
                    {
                        items.Add(itemData.Name);
                    }
                }
            }
        }

        if (items.Count == 0)
            items.Add(StringDef.NoInfo);

        return items.ToArray();
    }

    [WebMethod]
    public string[] GetNpcNameList(string prefixText, int count)
    {
        if (count == 0)
        {
            count = 10;
        }

        List<string> items = new List<string>(count);

        FS2NpcData[] npcDataList = FS2GameDataManager.TheInstance.NpcData;
        if (npcDataList != null)
        {
            foreach (FS2NpcData npcData in npcDataList)
            {
                if (items.Count >= count) break;
                if (npcData != null)
                {
                    if (npcData.Name.Contains(prefixText))
                    {
                        items.Add(npcData.Name);
                    }
                }
            }
        }

        if (items.Count == 0)
            items.Add(StringDef.NoInfo);

        return items.ToArray();
    }
    

}

