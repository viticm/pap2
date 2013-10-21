using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using FSEye;

public partial class Common_GameServerFilter : System.Web.UI.UserControl
{
	IList _filterList;

	IList _filterOptionPanelList;

	NullFilter _nullFilter = new NullFilter();

	ConnectionStatusFilter _connectionStatusFilter = new ConnectionStatusFilter();

	GameServerGroupFilter _groupFilter = new GameServerGroupFilter(null);

	KeywordFilter _keywordFilter = new KeywordFilter();

	ServerTypeFilter _serverTypeFilter = new ServerTypeFilter();

	/// <summary>
	/// 过滤器改变事件
	/// </summary>
	public event EventHandler FilterChanged;

	/// <summary>
	/// 选中的过滤器
	/// </summary>
	public IGameServerFilter SelectedFilter
	{
		get
		{
			if (ListBoxFilter.SelectedIndex >= 0)
				return _filterList[ListBoxFilter.SelectedIndex] as IGameServerFilter;
			else
				return null;
		}
	}	

	/// <summary>
	/// 过滤服务器
	/// </summary>
	public IList FilterGameServer(IList serverList)
	{
		ArrayList matchServerList = new ArrayList();

		IGameServerFilter filter = SelectedFilter;
		if (filter != null)
		{
			for (int i = 0; i < serverList.Count; i++)
			{
				GameServer server = serverList[i] as GameServer;
				if (filter.IsMatch(server))
					matchServerList.Add(server);
			}
		}

		return matchServerList;
	}

	protected void Page_Init(object sender, EventArgs e)
	{
		_filterList = new ArrayList();
		_filterList.Add(_nullFilter);
		_filterList.Add(_serverTypeFilter);
		_filterList.Add(_connectionStatusFilter);
		_filterList.Add(_groupFilter);
		_filterList.Add(_keywordFilter);		

		_filterOptionPanelList = new ArrayList();
		_filterOptionPanelList.Add(PanelNullFilter);
		_filterOptionPanelList.Add(PanelServerTypeFilter);
		_filterOptionPanelList.Add(PanelConnectionStatusFilter);
		_filterOptionPanelList.Add(PanelGameServerGroupFilter);
		_filterOptionPanelList.Add(PanelKeywordFilter);		

		if (!IsPostBack)
		{
			ListFilters();			
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		_connectionStatusFilter.IsConnected = CheckBoxConnected.Checked;
		_groupFilter.Group = ServerGroupDropDownList.SelectedServerGroup;
		_keywordFilter.Keyword = TextBoxKeyword.Text;

		ShowFilterOptions();
	}

	private void ListFilters()
	{
		foreach (IGameServerFilter filter in _filterList)
		{
			ListBoxFilter.Items.Add(new ListItem(filter.Name));
		}
		ListBoxFilter.SelectedIndex = 0;
	}

	private void ShowFilterOptions()
	{
		foreach (Panel optionPanel in _filterOptionPanelList)
		{
			optionPanel.Visible = false;
		}

		if (ListBoxFilter.SelectedIndex >= 0)
		{
			(_filterOptionPanelList[ListBoxFilter.SelectedIndex] as Panel).Visible = true;
		}
	}

	protected void CheckBoxConnected_CheckedChanged(object sender, EventArgs e)
	{
		_connectionStatusFilter.IsConnected = CheckBoxConnected.Checked;

		DoFilterChangedEvent();
	}

	protected void ServerGroupDropDownList_SelectedServerGroupChanged(object sender, EventArgs e)
	{
		_groupFilter.Group = ServerGroupDropDownList.SelectedServerGroup;

		DoFilterChangedEvent();
	}

	protected void ListBoxFilter_SelectedIndexChanged(object sender, EventArgs e)
	{
		ShowFilterOptions();

		DoFilterChangedEvent();
	}

	protected void ButtonFilterByKeyword_Click(object sender, EventArgs e)
	{
		_keywordFilter.Keyword = TextBoxKeyword.Text;

		DoFilterChangedEvent();
	}

	protected void DropDownListServerType_SelectedIndexChanged(object sender, EventArgs e)
	{
		_serverTypeFilter.ServerType = (GameServer.ServerType)int.Parse(DropDownListServerType.SelectedValue);

		DoFilterChangedEvent();
	}

	private void DoFilterChangedEvent()
	{
		if (this.FilterChanged != null)
		{
			this.FilterChanged(this, null);
		}
	}
}
