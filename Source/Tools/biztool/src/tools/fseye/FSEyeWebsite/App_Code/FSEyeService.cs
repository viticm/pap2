using System;
using System.Web;
using System.Collections;
using System.Collections.Generic;
using System.Web.Services;
using System.Web.Services.Protocols;
using FSEye;
using FSEye.Security;

/// <summary>
/// Summary description for FSEyeService
/// </summary>
[WebService(Namespace = "fseye.blaze.kingsoft")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class FSEyeService : System.Web.Services.WebService
{

	public FSEyeService()
	{
	}

	[WebMethod]	
	public string[] GetUserGroupNames(string prefixText, int count)
	{
		Group[] groups = AdminServer.TheInstance.SecurityManager.GetAllGroups();
		List<string> matchingGroupNames = new List<string>();
		foreach (Group group in groups)
		{
			if (group.SecurityObject.Name.Contains(prefixText))
			{
				matchingGroupNames.Add(group.SecurityObject.Name);
			}
		}

		return matchingGroupNames.ToArray();
	}
}

