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

public partial class GameInfo_ItemExchange : BasePage
{
    DateTime _start;
    DateTime _end;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterItemExchange, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);

            StartDate.SelectedDate = DateTime.Today.AddMonths(-1);
            EndDate.SelectedDate = DateTime.Now;
        }
        LabelOpMsg.Text = string.Empty;
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    private ICollection CreateDataSource(IList exchangeRecordList, Hashtable guidHashTable)
    {
        if (exchangeRecordList == null || exchangeRecordList.Count == 0) return null;
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;
            dataTable.Columns.Add(new DataColumn("LogTime", typeof(DateTime)));
            dataTable.Columns.Add(new DataColumn("GiverId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("GiverName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ReceiverId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("ReceiverName", typeof(String)));            
            dataTable.Columns.Add(new DataColumn("ServerId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("ItemName", typeof(String)));

            foreach (ExchangeRecord exchangeRecord in exchangeRecordList)
            {
                dataRow = dataTable.NewRow();

                dataRow[0] = exchangeRecord.time;
                dataRow[1] = ((FS2RoleDataInfo)guidHashTable[exchangeRecord.giverGuid]).RoleId;
                dataRow[2] = ((FS2RoleDataInfo)guidHashTable[exchangeRecord.giverGuid]).RoleName;
                dataRow[3] = ((FS2RoleDataInfo)guidHashTable[exchangeRecord.receiverGuid]).RoleId;
                dataRow[4] = ((FS2RoleDataInfo)guidHashTable[exchangeRecord.receiverGuid]).RoleName;
                dataRow[5] = ServerDropDownList.SelectedServerId;
                dataRow[6] = exchangeRecord.itemName;

                dataTable.Rows.Add(dataRow);
            }

            DataView dataView = new DataView(dataTable);
            return dataView;
        }
        catch (Exception)
        {
            return null;
        }
    }

    protected void LinkButtonQuery_Click(object sender, EventArgs e)
    {
        Query();
    }

    void Query()
    {
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();
            searchCondition.AppendFormat("WHERE {0}={1}",FS2TableString.LogFieldLogEvent,LogEvent.ExchangeItem);

            string itemName = TextBoxInputItemName.Text.Trim();
            if (itemName.Length != 0)
            {
                searchCondition.AppendFormat(" AND {0} LIKE '?'", FS2TableString.LogFieldLogData);
                if (CheckBoxItemName.Checked) 
                    paramList.Add(string.Format("%“{0}”%",itemName));
                else 
                    paramList.Add(string.Format("%“%{0}%”%",itemName));                        
            }

            string giverName = TextBoxInputGiver.Text.Trim();
            if (giverName.Length != 0)
            {
                if (CheckBoxGiver.Checked)
                {
                    searchCondition.AppendFormat(" AND {0} IN ({1})", FS2TableString.LogFieldLogKey1,
                        string.Format("SELECT {0} FROM {1} WHERE {2}='?'", FS2TableString.RolesfirstFieldGUID,
                        FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldRoleName));
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0} IN ({1})", FS2TableString.LogFieldLogKey1,
                        string.Format("SELECT {0} FROM {1} WHERE {2} LIKE '%?%'", FS2TableString.RolesfirstFieldGUID,
                        FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldRoleName));
                }
                paramList.Add(giverName);
            }

            string receiverName = TextBoxInputReceiver.Text.Trim();
            if (receiverName.Length != 0)
            {
                if (CheckBoxReceiver.Checked)
                {
                    searchCondition.AppendFormat(" AND {0} IN ({1})", FS2TableString.LogFieldLogKey3,
                        string.Format("SELECT {0} FROM {1} WHERE {2} = '?'", FS2TableString.RolesfirstFieldGUID,
                        FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldRoleName));
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0} IN ({1})", FS2TableString.LogFieldLogKey3,
                        string.Format("SELECT {0} FROM {1} WHERE {2} LIKE '%?%'", FS2TableString.RolesfirstFieldGUID,
                        FS2TableString.RolesfirstTableName, FS2TableString.RolesfirstFieldRoleName));
                }
                paramList.Add(receiverName);
            }

            _start = StartDate.SelectedDate;
            if (_start == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }
            _end = EndDate.SelectedDate;
            if (_end == DateTime.MinValue)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }
            searchCondition.Append(string.Format(" AND {0}>='{1}' AND {0}<'{2}'", FS2TableString.LogFieldLogTime, _start.ToString("yyyy-MM-dd HH:mm:ss"), _end.ToString("yyyy-MM-dd HH:mm:ss")));

            string baseCmdString = string.Format("SELECT LogKey1,LogKey3,LogData,LogTime FROM {0} {1}",
                "{0}", searchCondition.ToString());
            string addTableCmdString;
            WebUtil.AddTableNameToCmd(CurrentUser.Id, server, baseCmdString, paramList, _start, _end, out addTableCmdString, out paramList);
            if (addTableCmdString.Length == 0)
            {
                //
                return;
            }

            string cmdString = "SELECT LogKey1,LogKey3,LogData,LogTime FROM ({0}) AS A ORDER BY LogTime";
            SqlCommand cmd = new SqlCommand(string.Format(cmdString, addTableCmdString), paramList.ToArray());
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String,
                                    SqlDataType.String,
                                    SqlDataType.Blob,
                                    SqlDataType.DateTime
                                    });

                object[] record = null;
                ArrayList infos = new ArrayList();
                ArrayList roleGuidList = new ArrayList();

                while ((record = result.ReadRecord()) != null)
                {
                    ExchangeRecord info = new ExchangeRecord();

                    info.giverGuid = record[0] as string;
                    if (info.giverGuid != null && !roleGuidList.Contains(info.giverGuid))
                        roleGuidList.Add(info.giverGuid);
                    info.receiverGuid = record[1] as string;
                    if (info.receiverGuid != null && !roleGuidList.Contains(info.receiverGuid))
                        roleGuidList.Add(info.receiverGuid);
                    info.time = (DateTime)record[3];

                    string blobData = Encoding.Default.GetString((byte[])record[2]);
                    blobData = blobData.Replace("“", "\"");
                    blobData = blobData.Replace("”", "\"");
                    string[] blobDataSplit = blobData.Split('\"');
                    info.itemName = blobDataSplit[3];

                    infos.Add(info);
                }

                if (infos.Count == 0)
                {
                    //没有查询到信息
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                    PanelResult.Visible = false;
                }
                else
                {
                    PanelResult.Visible = true;
                    DataGridResult.DataSource = CreateDataSource(infos, GetRoleGuid(roleGuidList));
                    DataGridResult.DataBind();
                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
                PanelResult.Visible = false;
            }            
        }
        catch (Exception ex)
        {
            PanelResult.Visible = false;
            LabelOpMsg.Text = ex.Message;
        }
    }


    private Hashtable GetRoleGuid(ArrayList guidList)
    {
        GameServer server = ServerDropDownList.SelectedGameServer;
        if (server == null) return null;
        if (guidList != null && guidList.Count != 0)
        {
            string sqlCmdText = "SELECT {0},{1},{2} FROM {3} WHERE {4}";
            StringBuilder guidBuilder = new StringBuilder();
            foreach (string guid in guidList)
            {
                guidBuilder.AppendFormat("{0}='{1}' OR ", FS2TableString.RolesfirstFieldGUID, guid);
            }
            SqlCommand sqlCmd = new SqlCommand(string.Format(sqlCmdText, FS2TableString.RolesfirstFieldId,
                FS2TableString.RolesfirstFieldRoleName, FS2TableString.RolesfirstFieldGUID, FS2TableString.RolesfirstTableName, guidBuilder.ToString(0, guidBuilder.Length - 3)), null);
            if (!server.IsConnected)
            {
                LabelOpMsg.Visible = true;
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return null;
            }
            SqlResult sqlResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, sqlCmd);
            if (sqlResult != null && sqlResult.Success)
            {
                Hashtable guidHashTable = new Hashtable();
                sqlResult.SetFieldType(new SqlDataType[]{
                                        SqlDataType.Int32,
                                        SqlDataType.String,
                                        SqlDataType.String
                                        });

                object[] record = null;
                ArrayList roleInfos = new ArrayList();
                while ((record = sqlResult.ReadRecord()) != null)
                {
                    FS2RoleDataInfo role = new FS2RoleDataInfo();
                    role.RoleId = (int)record[0];
                    role.RoleName = record[1] as string;
                    role.RoleGuid = record[2] as string;
                    if (role.RoleGuid != null && role.RoleGuid.Length != 0) guidHashTable[role.RoleGuid] = role;
                }
                return guidHashTable;
            }
        }
        return null;
    }
}

public class ExchangeRecord
{
    public string receiverGuid = string.Empty;

    public string giverGuid = string.Empty;

    public string itemName = string.Empty;

    public DateTime time;

    public ExchangeRecord()
    {
    }
}