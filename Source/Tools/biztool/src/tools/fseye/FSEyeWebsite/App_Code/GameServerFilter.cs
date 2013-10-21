using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using Resources;
using FSEye;

/// <summary>
/// 游戏服务器过滤器
/// </summary>
public interface IGameServerFilter
{
	string Name
	{
		get;
	}

	bool IsMatch(GameServer server);
}

/// <summary>
/// 空过滤器
/// </summary>
public class NullFilter : IGameServerFilter
{
	public string Name
	{
		get { return "无过滤"; }
	}

	public NullFilter()
	{
	}

	public bool IsMatch(GameServer server)
	{
		return true;
	}
}

/// <summary>
/// 连接状态过滤器
/// </summary>
public class ConnectionStatusFilter : IGameServerFilter
{
	bool _isConnected;

	public string Name
	{
		get { return "按连接状态过滤"; }
	}

	public bool IsConnected
	{
		get { return _isConnected; }
		set { _isConnected = value; }
	}

	public ConnectionStatusFilter()
	{
		_isConnected = true;
	}

	public bool IsMatch(GameServer server)
	{
		if (server.IsConnected == _isConnected)
			return true;
		else
			return false;
	}
}

/// <summary>
/// 服务器组过滤器
/// </summary>
public class GameServerGroupFilter : IGameServerFilter
{
	ServerGroup _group;

	public string Name
	{
		get { return "按服务器组过滤"; }
	}

	public ServerGroup Group
	{
		get { return _group; }
		set { _group = value; }
	}

	public GameServerGroupFilter(ServerGroup group)
	{
		_group = group;
	}

	public bool IsMatch(GameServer server)
	{
		if (_group != null)
		{
			return _group.List.Contains(server);
		}

		return false;
	}
}

/// <summary>
/// 关键字过滤器
/// </summary>
public class KeywordFilter : IGameServerFilter
{
	string _keyword;

	public string Name
	{
		get { return "按关键字过滤"; }
	}

	public string Keyword
	{
		get { return _keyword; }
		set { _keyword = value; }
	}

	public KeywordFilter() : this(string.Empty)
	{
	}

	public KeywordFilter(string keyword)
	{
		_keyword = keyword;
	}

	public bool IsMatch(GameServer server)
	{
		if (_keyword != null)
		{
			string lowerCaseKeyword = _keyword.ToLower();
			if (server.Name.ToLower().Contains(lowerCaseKeyword) ||
				server.Comment.ToLower().Contains(lowerCaseKeyword) ||
				(server.Group != null &&
					(server.Group.Name.ToLower().Contains(lowerCaseKeyword) || server.Group.Comment.ToLower().Contains(lowerCaseKeyword))
				))
			{
				return true;
			}
		}

		return false;
	}
}

/// <summary>
/// 服务器类型过滤器
/// </summary>
public class ServerTypeFilter : IGameServerFilter
{
	GameServer.ServerType _serverType;

	public string Name
	{
		get { return "按服务器类型过滤"; }
	}

	public GameServer.ServerType ServerType
	{
		get { return _serverType; }
		set { _serverType = value; }
	}

	public ServerTypeFilter()
	{
	}

	public bool IsMatch(GameServer server)
	{
		return (server.Type == _serverType);
	}
}


