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
using FSEye.Security;
using FSEye.FS2Lib;
using JX2Lib;
using FSEye;
using FSEye.PlugIn;

public partial class GameMaster_QueryRole : BasePage
{
    const int DefaultRecordPerPage = 50;

    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterQueryRole, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            CreateLevelList();
            ListBoxRecordPerPage.SelectedIndex = 1;
            _recordPerPage = DefaultRecordPerPage;
            LinkButtonRoleFreeze.Text = StringDef.Role + StringDef.Freeze;
            LinkButtonRoleFreeze.Attributes.Add("onclick",
                    string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Freeze + StringDef.Role)));
            LinkButtonRoleUnFreeze.Text = StringDef.Role + StringDef.Unfreeze;
            LinkButtonRoleUnFreeze.Attributes.Add("onclick",
                    string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Unfreeze + StringDef.Role)));
            LinkButtonRoleAccountFreeze.Text = StringDef.Role + StringDef.Account + StringDef.Freeze;
            LinkButtonRoleAccountFreeze.Attributes.Add("onclick",
                    string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Freeze + StringDef.Account)));
            LinkButtonRoleAccountUnFreeze.Text = StringDef.Role + StringDef.Account + StringDef.Unfreeze;
            LinkButtonRoleAccountUnFreeze.Attributes.Add("onclick",
                    string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.Unfreeze + StringDef.Account)));
            //CreateCheckBoxListbySeries();
            CreateCheckBoxListRoleType();
           // CreateTime_start.SelectedDate = DateTime.MinValue;
            //CreateTime_end.SelectedDate = DateTime.MaxValue;
            //LastLoginTime_start.SelectedDate = DateTime.MinValue;
            //LastLoginTime_end.SelectedDate = DateTime.MaxValue;
        }
        else
        {
            try
            {
                _recordPerPage = int.Parse(ListBoxRecordPerPage.SelectedValue);
            }
            catch (Exception)
            {
                _recordPerPage = DefaultRecordPerPage;
            }
        }

        LabelOpMsg.Text = string.Empty;
    }

    private void CreateCheckBoxListRoleType()
    {
        ListItem item = new ListItem(string.Format("{0}[{1}]", StringDef.Shaolin, StringDef.NoRote), "1,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Shaolin, StringDef.Daoshaolin), "1,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Shaolin, StringDef.Gunshaolin), "1,2");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianwang, StringDef.NoRote), "2,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianwang, StringDef.Qiantianwang), "2,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianwang, StringDef.Cuitianwang), "2,2");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tangmen, StringDef.NoRote), "3,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tangmen, StringDef.Xianjintangmen), "3,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tangmen, StringDef.Xiujiantangmen), "3,2");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudu, StringDef.NoRote), "4,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudu, StringDef.Daowudu), "4,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudu, StringDef.Zhangwudu), "4,2");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Emei, StringDef.NoRote), "5,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Emei, StringDef.Zhangemei), "5,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Emei, StringDef.Fuzhuemei), "5,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Cuiyan, StringDef.NoRote), "6,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Cuiyan, StringDef.Jiancuiyan), "6,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Cuiyan, StringDef.Daocuiyan), "6,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Gaibang, StringDef.NoRote), "7,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Gaibang, StringDef.Zhanggaibang), "7,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Gaibang, StringDef.Gungaibang), "7,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianren, StringDef.NoRote), "8,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianren, StringDef.Zhangtianren), "8,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Tianren, StringDef.Motianren), "8,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudang, StringDef.NoRote), "9,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudang, StringDef.Qiwudang), "9,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Wudang, StringDef.Jianwudang), "9,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Kunlun, StringDef.NoRote), "10,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Kunlun, StringDef.Daokunlun), "10,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Kunlun, StringDef.Jiankunlun), "10,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Mingjiao, StringDef.NoRote), "11,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Mingjiao, StringDef.Cuimingjiao), "11,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Mingjiao, StringDef.Jianmingjiao), "11,2");
        item.Selected = true;

        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daliduanshi, StringDef.NoRote), "12,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daliduanshi, StringDef.Zhiduanshi), "12,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daliduanshi, StringDef.Qiduanshi), "12,2");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.NoFaction, StringDef.NewPlayer), "0,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
    }
    protected void LinkButtonChooseAll_Click(object sender, EventArgs e)
        
    {   
        foreach(ListItem item in  CheckBoxListRoleType.Items)
        {
            item.Selected = true;
        }
    }
    protected void LinkButtonNotChoose_Click(object sender, EventArgs e)
    {
        foreach (ListItem item in CheckBoxListRoleType.Items)
        {
            item.Selected = false;
        }
    }
/*
    //五行的CreateCheck
    private void CreateCheckBoxListbySeries()
    {
        ListItem item = new ListItem(string.Format("{0}", StringDef.gold), "1");
        item.Selected = true;
        CheckBoxListbySeries.Items.Add(item);
        item = new ListItem(string.Format("{0}", StringDef.wood), "2");
        item.Selected = true;
        CheckBoxListbySeries.Items.Add(item);
        item = new ListItem(string.Format("{0}", StringDef.water), "3");
        item.Selected = true;
        CheckBoxListbySeries.Items.Add(item);
        item = new ListItem(string.Format("{0}", StringDef.fire), "4");
        item.Selected = true;
        CheckBoxListbySeries.Items.Add(item);
        item = new ListItem(string.Format("{0}", StringDef.earth), "5");
        item.Selected = true;
        CheckBoxListbySeries.Items.Add(item);
    }

    */

    ICollection CreateDataSource(IList roleList)
    {
        if (roleList == null || roleList.Count == 0) return null;
        try
        {
            DataTable dataTable = new DataTable();
            DataRow dataRow;            
            dataTable.Columns.Add(new DataColumn("cAccountName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("cRoleName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("iSex", typeof(String)));
            dataTable.Columns.Add(new DataColumn("iLevel", typeof(int)));
           // dataTable.Columns.Add(new DataColumn("iCreateTime", typeof(int)));
            //dataTable.Columns.Add(new DataColumn("iLastLoginTime", typeof(int)));
            dataTable.Columns.Add(new DataColumn("iGameTime", typeof(int)));
            dataTable.Columns.Add(new DataColumn("RoleClass", typeof(String)));
            dataTable.Columns.Add(new DataColumn("iMoney", typeof(Int64)));
            dataTable.Columns.Add(new DataColumn("dCreateTime", typeof(DateTime)));
            dataTable.Columns.Add(new DataColumn("dLastLoginTime", typeof(DateTime)));
            dataTable.Columns.Add(new DataColumn("cRoleID", typeof(String)));
            dataTable.Columns.Add(new DataColumn("serverID", typeof(String)));
            dataTable.Columns.Add(new DataColumn("cRoleNameUrlEncode", typeof(String)));
            foreach (JX2RoleDataInfo roleInfo in roleList)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = roleInfo.cAccountName;
                dataRow[1] = roleInfo.cRoleName;
                dataRow[2] = roleInfo.iSex == JX2RoleSex.Male ? StringDef.Male : StringDef.Female;
                dataRow[3] = roleInfo.iLevel;
               // dataRow[4] = roleInfo.dCreateTime;
               // dataRow[5] = roleInfo.dLastLoginTime;
                dataRow[4] = roleInfo.iGameTime/3600;
                string classDescription = string.Empty;
                switch (roleInfo.RoleType)
                {
                    case JxRoleType.Wumenpai:
                        classDescription = StringDef.NoFaction;
                        break;
                    case JxRoleType.Shaolin:
                        classDescription = StringDef.Shaolin;
                        break;
                    case JxRoleType.Daoshaolin:
                        classDescription = StringDef.Daoshaolin;
                        break;
                    case JxRoleType.Gunshaolin:
                        classDescription = StringDef.Gunshaolin;
                        break;
                    case JxRoleType.Tianwang:
                        classDescription = StringDef.Tianwang;
                        break;
                    case JxRoleType.Qiantianwang:
                        classDescription = StringDef.Qiantianwang;
                        break;
                    case JxRoleType.Cuitianwang:
                        classDescription = StringDef.Cuitianwang;
                        break;
                    case JxRoleType.Tangmen:
                        classDescription = StringDef.Tangmen;
                        break;
                    case JxRoleType.Xiujiantangmen:
                        classDescription = StringDef.Xiujiantangmen;
                        break;
                    case JxRoleType.Xianjintangmen:
                        classDescription = StringDef.Xianjintangmen;
                        break;
                    case JxRoleType.Wudu:
                        classDescription = StringDef.Wudu;
                        break;
                    case JxRoleType.Daowudu:
                        classDescription = StringDef.Daowudu;
                        break;
                    case JxRoleType.Zhangwudu:
                        classDescription = StringDef.Zhangwudu;
                        break;
                    case JxRoleType.Emei:
                        classDescription = StringDef.Emei;
                        break;
                    case JxRoleType.Fuzhuemei:
                        classDescription = StringDef.Fuzhuemei;
                        break;
                    case JxRoleType.Zhangemei:
                        classDescription = StringDef.Zhangemei;
                        break;
                    case JxRoleType.Cuiyan:
                        classDescription = StringDef.Cuiyan;
                        break;
                    case JxRoleType.Jiancuiyan:
                        classDescription = StringDef.Jiancuiyan;
                        break;
                    case JxRoleType.Daocuiyan:
                        classDescription = StringDef.Daocuiyan;
                        break;
                    case JxRoleType.Gaibang:
                        classDescription = StringDef.Gaibang;
                        break;
                    case JxRoleType.Gungaibang:
                        classDescription = StringDef.Gungaibang;
                        break;
                    case JxRoleType.Zhanggaibang:
                        classDescription = StringDef.Zhanggaibang;
                        break;
                    case JxRoleType.Tianren:
                        classDescription = StringDef.Tianren;
                        break;
                    case JxRoleType.Zhangtianren:
                        classDescription = StringDef.Zhangtianren;
                        break;
                    case JxRoleType.Motianren:
                        classDescription = StringDef.Motianren;
                        break;
                    case JxRoleType.Wudang:
                        classDescription = StringDef.Wudang;
                        break;
                    case JxRoleType.Jianwudang:
                        classDescription = StringDef.Jianwudang;
                        break;
                    case JxRoleType.Qiwudang:
                        classDescription = StringDef.Qiwudang;
                        break;
                    case JxRoleType.Kunlun:
                        classDescription = StringDef.Kunlun;
                        break;
                    case JxRoleType.Daokunlun:
                        classDescription = StringDef.Daokunlun;
                        break;
                    case JxRoleType.Jiankunlun:
                        classDescription = StringDef.Jiankunlun;
                        break;
                    case JxRoleType.Mingjiao:
                        classDescription = StringDef.Mingjiao;
                        break;
                    case JxRoleType.Cuimingjiao:
                        classDescription = StringDef.Cuimingjiao;
                        break;
                    case JxRoleType.Jianmingjiao:
                        classDescription = StringDef.Jianmingjiao;
                        break;
                    case JxRoleType.Daliduanshi:
                        classDescription = StringDef.Daliduanshi;
                        break;
                    case JxRoleType.Zhiduanshi:
                        classDescription = StringDef.Zhiduanshi;
                        break;
                    case JxRoleType.Qiduanshi:
                        classDescription = StringDef.Qiduanshi;
                        break;
                    case JxRoleType.ShaolinWuluxian:
                        classDescription = StringDef.Shaolin+StringDef.NoRote;
                        break;
                    case JxRoleType.TianwangWuluxian:
                        classDescription = StringDef.Tianwang+StringDef.NoRote;
                        break;
                    case JxRoleType.TianrenWuluxian:
                        classDescription = StringDef.Tianren+StringDef.NoRote;
                        break;
                    case JxRoleType.CuiyanWuluxian:
                        classDescription = StringDef.Cuiyan+StringDef.NoRote;
                        break;
                    case JxRoleType.TangmenWuluxian:
                        classDescription = StringDef.Tangmen+StringDef.NoRote;
                        break;
                    case JxRoleType.WudangWuluxian:
                        classDescription = StringDef.Wudang+StringDef.NoRote;
                        break;
                    case JxRoleType.DaliduanshiWuluxian:
                        classDescription = StringDef.Daliduanshi+StringDef.NoRote;
                        break;
                    case JxRoleType.WuduWuluxian:
                        classDescription = StringDef.Wudu+StringDef.NoRote;
                        break;
                    case JxRoleType.KunlunWuluxian:
                        classDescription = StringDef.Kunlun+StringDef.NoRote;
                        break;
                    case JxRoleType.MingjiaoWuluxian:
                        classDescription = StringDef.Mingjiao+StringDef.NoRote;
                        break;
                    case JxRoleType.EmeiWuluxian:
                        classDescription = StringDef.Emei+StringDef.NoRote;
                        break;
                    case JxRoleType.GaibangWuluxian:
                        classDescription = StringDef.Gaibang+StringDef.NoRote;
                        break;

                }
                dataRow[5] = classDescription;
                dataRow[6] = roleInfo.iMoney;
                dataRow[7] = roleInfo.dCreateTime;
                dataRow[8] = roleInfo.dLastLoginTime;
                dataRow[9] = roleInfo.cRoleID;
                dataRow[10] = serverGroupDropDownList.SelectedValue;
                dataRow[11] = Server.UrlEncode(roleInfo.cRoleName);
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

    void CreateLevelList()
    {
        for (int level = 1; level <= FS2GameDataManager.MaxLevel; level++)
        {
            ListBoxStartLevel.Items.Add(new ListItem(level.ToString(), level.ToString()));
            ListBoxEndLevel.Items.Add(new ListItem(level.ToString(), level.ToString()));
        }

        ListBoxStartLevel.SelectedIndex = 0;
        ListBoxEndLevel.SelectedIndex = ListBoxEndLevel.Items.Count - 1;
    }

    protected void LinkButtonSearchRole_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    void Query(int offset)
    {
        try
        {
            PanelResult.Visible = false;

            GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(int.Parse(serverGroupDropDownList.SelectedValue));
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            //读权限
            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return;
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();

            string cRoleName = TextBoxcRoleName.Text;
            string cAccountName = TextBoxcAccountName.Text;
            WebUtil.ValidateValueString(cRoleName);
            WebUtil.ValidateValueString(cAccountName);
            //角色名
            if (cRoleName.Length > 0)
            {
                if (CheckBoxcRoleName.Checked)
                {
                    searchCondition.AppendFormat(" AND {0}='?'", JX2TableString.cRoleName);
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0} LIKE '%?%'", JX2TableString.cRoleName);
                }
                paramList.Add(cRoleName);
            }
            //帐号名
            if (cAccountName.Length > 0)
            {
                if (CheckBoxcAccountName.Checked)
                {
                    searchCondition.Append(string.Format(" AND {0}='?'", JX2TableString.cAccountName));
                }
                else
                {
                    searchCondition.Append(string.Format(" AND {0} LIKE '%?%'", JX2TableString.cAccountName));
                }
                paramList.Add(cAccountName);
            }


            //性别
            int sex = ListBoxSex.SelectedIndex - 1;
            if (sex >= 0)
            {
                searchCondition.Append(string.Format(" AND {0}={1}", JX2TableString.iSex, sex));
            }
            //总在线时间
            string gametime_start_str = TextBoxdwGameTime_start.Text;
            string gametime_end_str = TextBoxdwGameTime_end.Text;
            if(gametime_start_str!=""&&gametime_end_str!=""){
                int GameTime_start = Convert.ToInt32(TextBoxdwGameTime_start.Text);
                int GameTime_end = Convert.ToInt32(TextBoxdwGameTime_end.Text);

                if (GameTime_start > GameTime_end)
                {
                    LabelOpMsg.Text = StringDef.EndTimesmallerThanStartTime;
                    return;
                }
                else
                {
                    GameTime_start = GameTime_start * 3600;
                    GameTime_end = GameTime_end * 3600;
                    searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<='{2}'", JX2TableString.iGameTime,
                    GameTime_start, GameTime_end);
                }
            }
          
           


            //金钱
            string Money_start_str = TextBoxMoney_start.Text;
            string Money_end_str = TextBoxMoney_end.Text;
            if(Money_start_str!=""&&Money_end_str!=""){
                int Money_start = Convert.ToInt32(TextBoxMoney_start.Text);
                int Money_end = Convert.ToInt32(TextBoxMoney_end.Text);
                if (Money_start > Money_end)
                {
                    LabelOpMsg.Text = StringDef.EndTimesmallerThanStartTime;
                    return;
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<='{2}'", JX2TableString.iMoney,
                    Money_start, Money_end);
                }
            }
            
         
            
            //创建时间
            DateTime createtime_start = CreateTime_start.SelectedDate;
            DateTime createtime_end = CreateTime_end.SelectedDate;
            if (createtime_start != DateTime.MinValue && createtime_end == DateTime.MinValue){
                searchCondition.AppendFormat(" AND {0}>='{1}'", JX2TableString.dCreateTime,
                    createtime_start.ToString("yyyy-MM-dd HH:mm:ss"));
            }
            else if (createtime_start == DateTime.MinValue && createtime_end != DateTime.MinValue){
                searchCondition.AppendFormat(" AND {0}<='{1}'", JX2TableString.dCreateTime,
                   createtime_end.ToString("yyyy-MM-dd HH:mm:ss"));
            }
            else if (createtime_start != DateTime.MinValue && createtime_end != DateTime.MinValue)
            {
            if (createtime_start > createtime_end)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }
            else
            {
                searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<='{2}'", JX2TableString.dCreateTime,
                    createtime_start.ToString("yyyy-MM-dd HH:mm:ss"), createtime_end.ToString("yyyy-MM-dd HH:mm:ss"));
            }
        }


            //最后一次登陆时间
            DateTime lastlogintime_start = LastLoginTime_start.SelectedDate;
            DateTime lastlogintime_end = LastLoginTime_end.SelectedDate;
            if (lastlogintime_start != DateTime.MinValue && lastlogintime_end == DateTime.MinValue){
                searchCondition.AppendFormat(" AND {0}>='{1}'", JX2TableString.dLastLoginTime,
                 lastlogintime_start.ToString("yyyy-MM-dd HH:mm:ss"));
            }
            else if (lastlogintime_start == DateTime.MinValue && lastlogintime_end != DateTime.MinValue){
                searchCondition.AppendFormat(" AND {0}<='{1}'", JX2TableString.dLastLoginTime,
                  lastlogintime_end.ToString("yyyy-MM-dd HH:mm:ss"));
            }
            if (lastlogintime_start !=DateTime.MinValue && lastlogintime_end != DateTime.MinValue)
            {
              
                if (lastlogintime_start > lastlogintime_end)
                {
                    LabelOpMsg.Text = StringDef.EndTimesmallerThanStartTime;
                    return;
                }
                else
                {
                    searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<='{2}'", JX2TableString.dLastLoginTime,
                  lastlogintime_start.ToString("yyyy-MM-dd HH:mm:ss"), lastlogintime_end.ToString("yyyy-MM-dd HH:mm:ss"));
                }

            }

            //角色
            StringBuilder roleTypeBuilder = new StringBuilder();
            foreach (ListItem item in CheckBoxListRoleType.Items)
            {
                if (item.Selected)
                {
                    string[] conValue = item.Value.Split(',');
                    roleTypeBuilder.AppendFormat("({0}='{1}' AND {2}='{3}') OR ", JX2TableString.iFaction, conValue[0], JX2TableString.iFactionRoute, conValue[1]);    //门派，与门派路线                    
                }
            }
            if (roleTypeBuilder.Length != 0)
            {
                searchCondition.AppendFormat(" AND ({0}) ", roleTypeBuilder.ToString(0, roleTypeBuilder.Length - 3));
            }
            else
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.RoleClass);
                LabelOpMsg.Visible = true;
                return;
            }

            //等级排序
            int startLevel = int.Parse(ListBoxStartLevel.SelectedValue);
            if (startLevel > 1)
            {
                searchCondition.Append(string.Format(" AND {0}>={1}", JX2TableString.iLevel, startLevel));
            }

            int endLevel = int.Parse(ListBoxEndLevel.SelectedValue);
            if (endLevel < FS2GameDataManager.MaxLevel)
            {
                searchCondition.Append(string.Format(" AND {0}<={1}", JX2TableString.iLevel, endLevel));
            }

            if (searchCondition.Length > 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, " WHERE");
            }
            
            //排序
            string orderByType = string.Empty;
            switch (ListBoxOrderByType.SelectedIndex)
            {
                case 0:
                    orderByType = "ASC";
                    break;
                case 1:
                    orderByType = "DESC";
                    break;
            }

            string orderByStatement = string.Empty;
            switch (ListBoxOrderBy.SelectedIndex)
            {

                case 0:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", JX2TableString.iLevel, orderByType);
                    break;
                case 1:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", JX2TableString.iFaction, orderByType);
                    break;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount + 1);

            SqlCommand cmd = new SqlCommand("SELECT cAccountName,cRoleName,iSex,iLevel,iGameTime,iFaction,iFactionRoute,iMoney, dCreateTime,dLastLoginTime,cRoleID FROM trolebaseinfo" + searchCondition.ToString() + orderByStatement + limitStatement, paramList.ToArray());

          SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
           
            if (result != null && result.Success)
            {
                ViewState[WebConfig.SessionQueryLogOffset] = offset;

                result.SetFieldType(
                        new SqlDataType[] {                        
						SqlDataType.String,
						SqlDataType.String,
						SqlDataType.SByte,
						SqlDataType.Int32,//等级
                        SqlDataType.Int32,//在线时间
                        SqlDataType.Int32,
                        SqlDataType.Int32,
                        SqlDataType.Int64,
                        SqlDataType.DateTime,
                        SqlDataType.DateTime,
                        SqlDataType.String
                        });
                //做结果列表了
                ArrayList roles = new ArrayList();
                object[] record = null;
                //Encoding utf8 = Encoding.UTF8;
                //Encoding gb2312 = Encoding.GetEncoding("GB2312");
                while ((record = result.ReadRecord()) != null)
                {
                   
                    JX2RoleDataInfo info = new JX2RoleDataInfo();
                    info.cAccountName = (string)record[0];
                    //string strRoleName = (string)record[1];
                    //info.cRoleName = utf8.GetString(gb2312.GetBytes(strRoleName));
                    info.cRoleName = (string)record[1];
                    info.iSex = (JX2RoleSex)(SByte)record[2];
                    info.iLevel = (int)record[3];
                    //在线时间
                    info.iGameTime = (int)record[4];
                    //这个是门派字段
                    info.iFaction = (int)record[5];
                    //这个是门派路线字段
                    info.iFactionRoute = (int)record[6];
                    //这个是职业字段
                    info.RoleType = (JxRoleType)(info.iFaction * 100 + info.iFactionRoute);//RoleType等于iFaction乘100与iFactionRoute相加，为了与JxRoleType批配
                    info.iMoney = (Int64)record[7];
                    if (record[8] != null)
                         info.dCreateTime = (DateTime)record[8];
                     //游戏新建角色，没登陆显示的上次登陆时间为1970-1-1 10:00:00，处理：上次登陆时间小与创建时间的，上次登陆时间置为创建时间
                     if (record[9] != null && (DateTime)record[9] >(DateTime)record[8]){
                         info.dLastLoginTime = (DateTime)record[9];
                     }
                     else{
                         info.dLastLoginTime = (DateTime)record[8];
                     }
                     info.cRoleID = (String)record[10];
                    roles.Add(info);
                }
               

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (roles.Count > limitCount);
                DataGridResult.DataSource = CreateDataSource(roles);
                DataGridResult.DataBind();

                if (roles != null && roles.Count != 0)
                    PanelResult.Visible = true;
                else
                    PanelResult.Visible = false;

                if (!PanelResult.Visible)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                }
            }
            else
            {
                if (result == null)
                    LabelOpMsg.Text = StringDef.QueryTimeOut;
                else
                    LabelOpMsg.Text = StringDef.OperationFail;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            PanelResult.Visible = false;
        }
    }

    //private bool CreateSearchRoleResultList(int serverId, RoleBasicInfo[] roles)
    //{
    //    if (roles != null && roles.Length > 0)
    //    {
    //        RoleBasicInfo info = null;
    //        for (int i = 0; i < roles.Length; i++)
    //        {
    //            info = roles[i];
    //            TableRow row = new TableRow();

    //            TableCell cell = new TableCell();
    //            CheckBox checkBox = new CheckBox();
    //            checkBox.Text = info.Id.ToString();
    //            cell.Controls.Add(checkBox);
    //            //cell.Text = info.Id.ToString();
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            HyperLink link = new HyperLink();
    //            link.Target = "_blank";
    //            link.NavigateUrl = "RoleDetail.aspx?serverId=" + serverId + "&roleId=" + info.Id;				
    //            link.Text = info.Name;
    //            link.SkinID = "PlainText";
    //            cell.Controls.Add(link);
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            cell.Text = info.Account;
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            if (info.Tong.Length > 0)
    //            {
    //                cell.Text = info.Tong;
    //            }
    //            else
    //            {
    //                cell.Text = StringDef.None;
    //            }
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            cell.Font.Bold = true;
    //            cell.Text = info.Level.ToString();
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            Image sexImage = new Image();
    //            if (info.IsMale)
    //            {
    //                sexImage.ToolTip = StringDef.Male;
    //                sexImage.ImageUrl = "~/Image/male.gif";
    //            }
    //            else
    //            {
    //                sexImage.ToolTip = StringDef.Female;
    //                sexImage.ImageUrl = "~/Image/female.gif";
    //            }
    //            cell.Controls.Add(sexImage);
    //            row.Cells.Add(cell);

    //            cell = new TableCell();
    //            switch (info.Class)
    //            {
    //                case 0:
    //                    cell.Text = StringDef.Jiashi;
    //                    break;
    //                case 1:
    //                    cell.Text = StringDef.Daoshi;
    //                    break;
    //                case 2:
    //                    cell.Text = StringDef.Yiren;
    //                    break;
    //            }
    //            row.Cells.Add(cell);

    //            TableSearchRoleList.Rows.Add(row);
    //        }
    //        return true;
    //    }
    //    return false;
    //}

    protected void ButtonFirstPage_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    protected void ButtonPreviousPage_Click(object sender, EventArgs e)
    {
        int offset = (int)ViewState[WebConfig.SessionQueryLogOffset];
        if (offset > 0)
        {
            offset -= _recordPerPage;
        }
        if (offset < 0)
            offset = 0;

        Query(offset);
    }

    protected void ButtonNextPage_Click(object sender, EventArgs e)
    {
        int offset = (int)ViewState[WebConfig.SessionQueryLogOffset];
        offset += _recordPerPage;

        Query(offset);
    }

    protected void LinkButtonFreeze_Click(object sender, EventArgs e)
    {
        Label labelServerId = DataGridResult.Items[1].FindControl("LabelServerId") as Label;
        GameServer server = TheAdminServer.GameServerManager.GetGameServer(int.Parse(labelServerId.Text));
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }

        ArrayList selectedRoleIdList = new ArrayList();
        ArrayList selectedRoleName = new ArrayList();
        foreach (DataGridItem item in DataGridResult.Items)
        {
            CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
            if (selection != null)
            {
                if (selection.Checked)
                {
                    selectedRoleIdList.Add(selection.Text);

                    HyperLink link = (HyperLink)item.FindControl("LinkRoleName");
                    if (link != null) selectedRoleName.Add(link.Text);
                }
            }
        }
        if (selectedRoleIdList.Count == 0) return;
        string cmdText = "UPDATE {0} SET {1}='{2}' WHERE {3}";
        StringBuilder searchCondition = new StringBuilder();
        foreach (string selectedRoleId in selectedRoleIdList)
        {
            searchCondition.AppendFormat("{0}='{1}' OR ", FS2TableString.RolesfirstFieldId, selectedRoleId);
        }
        SqlCommand cmd = new SqlCommand(string.Format(cmdText, FS2TableString.RolesfirstTableName,
            FS2TableString.RolesfirstFieldNoLoginIn, 1, searchCondition.ToString(0, searchCondition.Length - 3), null));

        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
        if (result != null & result.Success)
        {
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Freeze + StringDef.Role + selectedRoleIdList.Count + StringDef.Unit;

            //发GM指令 记日志            
            foreach (string roleName in selectedRoleName)
            {
                string gmCmd = WebConfig.GMCommandKickPlayer;
                server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, gmCmd);

                FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(CurrentUser.Id,
                (int)GMUtil.GMOperation.FreezeRole,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                              CurrentUser.RealName,
                              server.Name,
                              roleName,
                              StringDef.Unfreeze + StringDef.Role),
                (int)GMUtil.GMOpTargetType.Role);
            }
        }
        else
        {
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Freeze + StringDef.Role + selectedRoleIdList.Count + StringDef.Unit;
        }

    }

    protected void LinkButtonUnFreeze_Click(object sender, EventArgs e)
    {
        ArrayList selectedRoleIdList = new ArrayList();
        ArrayList selectedRoleName = new ArrayList();
        foreach (DataGridItem item in DataGridResult.Items)
        {
            CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
            if (selection != null)
            {
                if (selection.Checked)
                {
                    selectedRoleIdList.Add(selection.Text);
                    HyperLink link = (HyperLink)item.FindControl("LinkRoleName");
                    if (link != null) selectedRoleName.Add(link.Text);
                }
            }
        }
        if (selectedRoleIdList.Count == 0) return;
        string cmdText = "UPDATE {0} SET {1}='{2}' WHERE {3}";
        StringBuilder searchCondition = new StringBuilder();
        foreach (string selectedRoleId in selectedRoleIdList)
        {
            searchCondition.AppendFormat("{0}='{1}' OR ", FS2TableString.RolesfirstFieldId, selectedRoleId);
        }
        SqlCommand cmd = new SqlCommand(string.Format(cmdText, FS2TableString.RolesfirstTableName,
            FS2TableString.RolesfirstFieldNoLoginIn, 0, searchCondition.ToString(0, searchCondition.Length - 3), null));

        Label labelServerId = DataGridResult.Items[1].FindControl("LabelServerId") as Label;
        GameServer server = TheAdminServer.GameServerManager.GetGameServer(int.Parse(labelServerId.Text));
        if (server == null)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }

        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);

        if (result != null & result.Success)
        {
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Unfreeze + StringDef.Role + selectedRoleIdList.Count + StringDef.Unit;
            //记日志
            foreach (string roleName in selectedRoleName)
            {
                FSEye.Security.LogSystem.TheInstance.WriteGMOperationLog(CurrentUser.Id,
                (int)GMUtil.GMOperation.UnfreezeRole,
                roleName,
                server.Id,
                string.Format(StringDef.GMMessageLog,
                              CurrentUser.RealName,
                              server.Name,
                              roleName,
                              StringDef.Unfreeze + StringDef.Role),
                (int)GMUtil.GMOpTargetType.Role);
            }
        }
        else
        {
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Unfreeze + StringDef.Role + selectedRoleIdList.Count + StringDef.Unit;
        }
    }

    protected void LinkButtonRoleAccountFreeze_Click(object sender, EventArgs e)
    {
        int num = 0;
        foreach (DataGridItem item in DataGridResult.Items)
        {
            CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
            if (selection != null && selection.Checked)
            {
                //对选择的角色进行操作
                string accountName = item.Cells[2].Text;
                FSEye.PaySysLib.OperationResult result = GMUtil.SetAccountFrozen(CurrentUser.Id, accountName, true);

                //如果不成功则不进行后面发GM指今操作
                if (result != FSEye.PaySysLib.OperationResult.Success) continue;

                ++num;

                string gatewayInfo = GMUtil.GetGatewayByAccount(CurrentUser.Id, accountName);

                //如果Gateway信息不是Offline则得到其serverId然后对其上角色进行强制退出操作
                if (gatewayInfo != null && !gatewayInfo.Equals("Offline", StringComparison.OrdinalIgnoreCase))
                {
                    GameServer server = GMUtil.GetAccountOnlineServer(CurrentUser.Id, accountName);
                    if (server == null)
                    {
                        LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                        return;
                    }
                    IList roleNameList = GMUtil.GetRoleNameByAccount(CurrentUser.Id, server, accountName, true);
                    if (roleNameList.Count != 0)
                    {
                        foreach (string roleName in roleNameList)
                        {
                            server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, roleName, WebConfig.GMCommandKickPlayer);
                        }
                    }
                }
            }
        }
        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Unfreeze + StringDef.Role + StringDef.Account + num + StringDef.Unit;
    }

    protected void LinkButtonRoleAccountUnFreeze_Click(object sender, EventArgs e)
    {
        int num = 0;
        foreach (DataGridItem item in DataGridResult.Items)
        {
            CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
            if (selection != null && selection.Checked)
            {
                ++num;
                //对选择的角色进行操作
                string accountName = item.Cells[2].Text;
                TheAdminServer.PaySysAgent.UnfreezeAccount(accountName);
            }
        }
        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.Unfreeze + StringDef.Role + StringDef.Account + num + StringDef.Unit;
    }

    protected void CheckBoxPanelAdvancedQuery_Click(object sender, EventArgs e)
    {
        PanelAdvancedQuery.Visible = CheckBoxPanelAdvancedQuery.Checked;
    }


    //protected void LinkButtonExecuteGMCommand_Click(object sender, EventArgs e)
    //{
    //    foreach (DataGridItem item in DataGridResult.Items)
    //    {
    //        CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
    //        if (selection != null && selection.Checked)
    //        {
    //            Label labelServerId = DataGridResult.Items[1].FindControl("LabelServerId") as Label;
    //            HyperLink link = (HyperLink)item.FindControl("LinkRoleName");
    //            if (link != null && labelServerId!=null) 
    //            {
    //                string roleName = link.Text;
    //                if(roleName!=null&&roleName.Length!=0)
    //                    Response.Redirect(string.Format("~/GameMaster/GMCommand.aspx?{0}={1}&{2}={3}", WebConfig.ParamServerId, labelServerId.Text, WebConfig.ParamRoleName, Server.UrlEncode(roleName)), true);
    //            }
    //        }
    //    }        
    //}

    //protected void LinkButtonExpStatistic_Click(object sender,EventArgs e)
    //{
    //    foreach (DataGridItem item in DataGridResult.Items)
    //    {
    //        CheckBox selection = (CheckBox)item.FindControl("CheckBoxSelect");
    //        if (selection != null && selection.Checked)
    //        {
    //            Label labelServerId = DataGridResult.Items[1].FindControl("LabelServerId") as Label;
    //            HyperLink link = (HyperLink)item.FindControl("LinkRoleName");
    //            if (link != null && labelServerId != null)
    //            {
    //                string roleName = link.Text;
    //                if (roleName != null && roleName.Length != 0)
    //                    Response.Redirect(string.Format("~/GameInfo/ExpStatistic.aspx?{0}={1}&{2}={3}", WebConfig.ParamServerId, labelServerId.Text, WebConfig.ParamRoleName, Server.UrlEncode(roleName)), true);
    //            }
    //        }
    //    }
    //}
}