<%@ Application Language="C#" %>
<%@ Import Namespace="FSEye" %>
<%@ Import Namespace="System.Resources" %>
<%@ Import Namespace="System.IO" %>

<script runat="server">

    void Application_Start(Object sender, EventArgs E)
    {
		//初始化根路径	
		WebConfig.WebsiteRootPath = Server.MapPath("~/");
		CreateTempGraphDir();

		try
		{
			using (FileStream configStream = new FileStream(WebConfig.WebsiteRootPath + WebConfig.SystemConfigFile, FileMode.Open, FileAccess.Read))
			{
				SystemConfig.Current = SystemConfig.LoadFromXml(configStream, true);

				Util.DebugLog("Application_Start: Begin.");
				Util.DebugLog("Application_Start: LoadSystemConfig Done.");
				
				Application["AdminServer"] = AdminServer.TheInstance;
				Util.DebugLog("Application_Start: Instance AdminServer Done.");
				
				AdminServer.TheInstance.Initialize();
				Util.DebugLog("Application_Start: Initialize AdminServer Done.");
				
				FSEye.ShellScriptManager.TheInstance.ScriptRootPath = WebConfig.WebsiteRootPath + WebConfig.ShellScriptDir;

				AdminServer.TheInstance.StartService(null);
				Util.DebugLog("Application_Start: Start AdminServer Done.");

				Util.DebugLog("Application_Start: Completed.");
			}
		}
		catch (Exception ex)
		{
			Util.DebugLog("Application_Start: Exception" + ex.ToString());
		}
    }

    void Application_End(Object sender, EventArgs E)
    {
		Cleanup();
		Util.DebugLog("App_End");
    }

    void Session_Start(Object sender, EventArgs E)
    {
        Util.DebugLog("Session Start - SessionID : " + Context.Session.SessionID + " UserHostAddress : " + Context.Request.UserHostAddress);
    }

    void Session_End(Object sender, EventArgs E)
    {
		UserInfo user = (UserInfo)Session["CurrentUser"];
		if (user != null)
		{
			AdminServer.TheInstance.SecurityManager.Logout(user.Id);
		}
		Util.DebugLog("Session End");
    }

    void Application_BeginRequest(Object sender, EventArgs E)
    {
    }

    void Application_EndRequest(Object sender, EventArgs E)
    {
    }

    void Application_Error(Object sender, EventArgs E)
    {
		try
		{
			Util.DebugLog(Context.Error.ToString());
		}
		catch (Exception)
		{
		}
    }

	/// <summary>
	/// 清理
	/// </summary>
	void Cleanup()
	{
		//CleanTempGraph();
	}

	/// <summary>
	/// 清理临时图片目录
	/// </summary>
	void CleanTempGraph()
	{
		DirectoryInfo dir = new DirectoryInfo(WebConfig.WebsiteRootPath + WebConfig.TempGraphPath);
		if (dir.Exists)
		{
			FileInfo[] files = dir.GetFiles();
			foreach (FileInfo file in files)
			{
				file.Delete();
			}
		}
	}

	/// <summary>
	/// 建立临时图片目录
	/// </summary>
	void CreateTempGraphDir()
	{
		DirectoryInfo dir = new DirectoryInfo(WebConfig.WebsiteRootPath + WebConfig.TempGraphPath);
		if (!dir.Exists)
		{
			dir.Create();
		}
	}
 
</script>
