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
using System.Text;
using Resources;
using FSEye.FS2Lib;
using FSEye;
using FSEye.Security;

public partial class GameMaster_RoleItemInfo : BasePage
{
    int _serverId;
    int _roleId;

    protected void Page_Init(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterRoleItemInfo, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (Request.Params[WebConfig.ParamServerId] != null)
        {
            _serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
            if (Request.Params[WebConfig.ParamRoleId] != null)
            {
                _roleId = int.Parse(Request.Params[WebConfig.ParamRoleId]);
            }

            if (!IsPostBack)
            {
                GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(_serverId);
                if (server == null) return;
                FS2RoleDataInfo info = Query(server, _roleId);
                if (info != null)
                {
                    LabelTitle.Text += string.Format("[{0}]", info.RoleName) + StringDef.ItemInfo;
                    
                    RenderAll(info);
                    Session["RoleDataInfo"] = info;
                }
            }
        }
        else
        {
            Response.Redirect("QueryRole.aspx");
        }
    }

    protected FS2RoleDataInfo Query(GameServer server, int roleId)
    {
        FS2RoleDataInfo roleInfo = null;
        ArrayList paramList = new ArrayList();
        paramList.Add(roleId);
        string selectedFiled = string.Format("{0},{1},{2},{3},{4}",
            FS2TableString.RolesfirstFieldMoney,FS2TableString.RolesfirstFieldMoneyInBox,
            FS2TableString.RolesfirstFieldItemList,FS2TableString.RolesfirstFieldRoleName,
            FS2TableString.RolesfirstFieldId);
        string sqlCmdText = string.Format("SELECT {0} FROM {1} WHERE {2}='?';" ,selectedFiled,
            FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldId);
        SqlCommand cmd = new SqlCommand(sqlCmdText, paramList.ToArray());
        if (!server.IsConnected)
        {
            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return null;
        }
        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id,server,cmd);
        if (result != null && result.Success)
        {
            result.SetFieldType(new SqlDataType[]{
                SqlDataType.UInt32,
                SqlDataType.UInt32,
                SqlDataType.Blob,
                SqlDataType.String,
                SqlDataType.Int32
                });
            object[] record;
            byte[] itemData;

            roleInfo = new FS2RoleDataInfo();
            while ((record = result.ReadRecord())!=null)
            {
                if (record != null)
                {
                    roleInfo.Money = (uint)record[0];
                    roleInfo.MoneyInBox = (uint)record[1];                    
                    itemData = (byte[])record[2];
                    if (itemData != null)
                    {
                        roleInfo.ItemList = FS2ItemDataInfo.Parse(itemData);
                        //if (info != null) ItemList.Add(info);
                    }
                    roleInfo.RoleName = record[3] as string;
                    roleInfo.RoleId = (int)record[4];
                }
            }
            //RenderView(roleInfo);
        }
        return roleInfo;
    }

    private ArrayList GetItemList(FS2RoleDataInfo roleInfo, ItemPosition type)
    {
        //LabelTitle.Text += string.Format("[{0}]物品信息", roleInfo.RoleName);

        ArrayList itemList = new ArrayList();

        foreach (FS2ItemDataInfo itemInfo in roleInfo.ItemList)
        {
            if(itemInfo.Local.Equals((byte)type))
            {
                itemList.Add(itemInfo);                
            }
        }
        //if (renderRoleSate) RenderRoleState((FS2ItemDataInfo[])itemInBody.ToArray(typeof(FS2ItemDataInfo)));
        return itemList;
    }

    private void RenderAll(FS2RoleDataInfo roleInfo)
    {
        ArrayList itemInBody = new ArrayList();
        ArrayList itemInBag = new ArrayList();
        ArrayList itemInStorage = new ArrayList();
        if (roleInfo.ItemList == null || roleInfo.ItemList.Count == 0) return;
        foreach (FS2ItemDataInfo itemInfo in roleInfo.ItemList)
        {
            switch (itemInfo.Local)
            {
                case (byte)ItemPosition.PosEquip:
                    itemInBody.Add(itemInfo);
                    break;
                case (byte)ItemPosition.PosEquiproom:
                    itemInBag.Add(itemInfo);
                    break;
                case (byte)ItemPosition.PosRepositoryroom:
                    itemInStorage.Add(itemInfo);
                    break;
            }
        }
        RenderEquipedItem((FS2ItemDataInfo[])itemInBody.ToArray(typeof(FS2ItemDataInfo)));
        RenderItemInfo((FS2ItemDataInfo[])itemInBag.ToArray(typeof(FS2ItemDataInfo)), 1,DivRoleItem);
        RenderItemInfo((FS2ItemDataInfo[])itemInStorage.ToArray(typeof(FS2ItemDataInfo)), 1, DivItemStorage);
    }

    private void RenderEquipedItem(FS2ItemDataInfo[] itemInfos)
    {
        int rowNum = 3;
        int colNum = 3;
        FS2ItemDataInfo[] equipedSequence = new FS2ItemDataInfo[10];
        foreach (FS2ItemDataInfo item in itemInfos)
        {
            switch ((ItemPart)item.X)
            {
                case ItemPart.Amulet:
                    equipedSequence[0] = item;
                    break;
                case ItemPart.Helm:
                    equipedSequence[1] = item;
                    break;
                case ItemPart.Shoulder:
                    equipedSequence[2] = item;
                    break;
                case ItemPart.Weapon:
                    equipedSequence[3] = item;
                    break;
                case ItemPart.Armor:
                    equipedSequence[4] = item;
                    break;
                case ItemPart.Pendant:
                    equipedSequence[5] = item;
                    break;
                case ItemPart.Ring:
                    equipedSequence[6] = item;
                    break;
                case ItemPart.Boots:
                    equipedSequence[7] = item;
                    break;
                case ItemPart.Cuff:
                    equipedSequence[8] = item;
                    break;
                case ItemPart.Talisman:
                    equipedSequence[9] = item;
                    break;
            }
        }

        StringBuilder content = new StringBuilder();

        content.Append("<table cellspacing='5px' cellpadding='0' border='0' >");
        content.Append("<tr><td>");

        //法宝
        content.Append("<table cellspacing='0px' cellpadding='0' border='0' background='../Resources/Images/FS2Objects/itemCell.gif'>");
        content.Append("<tr height='38px'>");
        
        if (equipedSequence[9] != null)
        {
            content.Append(GameDataRender.RenderItemCell("1", string.Empty, string.Format("updateRoleItemInfo(\"{0}\")", equipedSequence[9].TemplateId), string.Empty));
        }
        else
        {
            content.Append("<td width='38px'></td>");
        }
        content.Append("</tr>");
        content.Append("</table>");


        content.Append("</td><td>");
        //装备
        content.Append("<table cellspacing='0' cellpadding='0' border='0' background='../Resources/Images/FS2Objects/itemCell.gif'>");
        for (int rowIndex = 0; rowIndex != rowNum; ++rowIndex)
        {
            content.Append("<tr height='38px'>");
            for (int colIndex = 0; colIndex != colNum; ++colIndex)
            {
                FS2ItemDataInfo itemSeq = equipedSequence[rowIndex * colNum + colIndex];
                if (itemSeq != null)
                {
                    FS2ItemData itemData = FS2GameDataManager.TheInstance.GetItemData(itemSeq.TemplateId);
                    content.Append(GameDataRender.RenderItemCell("1", itemData == null ? string.Empty : (itemData.Image + "_man_small_normal.jpg") , string.Format("updateRoleItemInfo(\"{0}\")", itemSeq.TemplateId), string.Empty));
                    //content.Append(GameDataRender.RenderItemCell("1",itemData == null ? string.Empty : itemData.Image, string.Format("updateRoleItemInfo(\"{0}\")", itemSeq.TemplateId),string.Empty));
                }
                else
                {
                    content.Append("<td width='38px' ></td>");
                }
            }
        }

        content.Append("</table>");

        content.Append("</td></tr></table>");

        DivRoleState.InnerHtml = content.ToString();
    }

    private void RenderItemInfo(FS2ItemDataInfo[] itemInfos, int pageNum, HtmlGenericControl div)
    {
        StringBuilder content = new StringBuilder();
        content.Append("<table border='0' cellspacing='0' cellpadding='0' background='../Resources/Images/FS2Objects/itemCell.gif'>");        
        for (int rowIndex = 0; rowIndex != FS2MacroDef.ItemRowNumPerBag; ++rowIndex)
        {
            content.Append("<tr height='38px'>");
            for (int colIndex = 0; colIndex != FS2MacroDef.ItemColNumPerBag; ++colIndex)
            {
                bool hasItem = false;
                foreach (FS2ItemDataInfo item in itemInfos)
                {
                    //如果找到
                    if (item.Y == rowIndex + (pageNum-1) * FS2MacroDef.ItemRowNumPerBag && item.X == colIndex)
                    {
                        FS2ItemData itemData = FS2GameDataManager.TheInstance.GetItemData(item.TemplateId);
                        //没有资源，先显示默认图标。						
						content.Append(GameDataRender.RenderItemCell(item.ItemCount.ToString(), itemData == null ? string.Empty : (itemData.Image + "_man_small_normal.jpg"), 
                                string.Format("updateRoleItemInfo(\"{0}\")", item.TemplateId),
                                string.Format("onclick='window.location=\"../GameMaster/ItemFlow.aspx?{0}={1}&{2}={3}\"'", WebConfig.ParamServerId, _serverId, WebConfig.ParamRoleGuid, item.GUID)));
                        hasItem = true;
                        break;
                    }
                }
                if (!hasItem)
                {
                    content.Append("<td width='38px'></td>");
                }
            }
            content.Append("</tr>");
        }
        content.Append("</table>");

        div.InnerHtml = content.ToString();
    }

    protected void LinkButtonBag_Click(object sender, EventArgs e)
    {
        LinkButton linkButton = sender as LinkButton;
        if (linkButton == null) return;
        FS2RoleDataInfo roleDataInfo = Session["RoleDataInfo"] as FS2RoleDataInfo;
        if (roleDataInfo.RoleId != _roleId)
        {
            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(_serverId);
            if (server == null) return;
            roleDataInfo = Query(server, _roleId);
            Session["RoleDataInfo"] = roleDataInfo;
        }
        if (roleDataInfo != null)
        {
            RenderAll(roleDataInfo);
            RenderItemInfo((FS2ItemDataInfo[])GetItemList(roleDataInfo, ItemPosition.PosEquiproom).ToArray(typeof(FS2ItemDataInfo)), int.Parse(linkButton.ID.Substring(linkButton.ID.Length - 1)), DivRoleItem);
        }
    }

    protected void LinkButtonStorage_Click(object sender, EventArgs e)
    {
        LinkButton linkButton = sender as LinkButton;
        if (linkButton == null) return;
        FS2RoleDataInfo roleDataInfo = Session["RoleDataInfo"] as FS2RoleDataInfo;
        if(roleDataInfo.RoleId != _roleId)
        {
            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(_serverId);
            if (server == null) return;
            roleDataInfo = Query(server, _roleId);
            Session["RoleDataInfo"] = roleDataInfo;
        }
        if (roleDataInfo != null)
        {
            RenderAll(roleDataInfo);
            RenderItemInfo((FS2ItemDataInfo[])GetItemList(roleDataInfo, ItemPosition.PosRepositoryroom).ToArray(typeof(FS2ItemDataInfo)), int.Parse(linkButton.ID.Substring(linkButton.ID.Length - 1)), DivItemStorage);
        }
    }
}