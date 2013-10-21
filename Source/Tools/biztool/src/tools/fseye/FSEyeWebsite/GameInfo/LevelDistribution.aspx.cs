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
using System.Drawing;
using ZedGraph;
using Resources;
using FSEye;
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_LevelDistribution : BasePage
{
    Hashtable _infoHashtable;
    int _total;
    protected void Page_Load(object sender, EventArgs e)
    {
        //权限检查
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticLevelDistribution, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            DateCalendar.SelectedDate = DateTime.Now;

            CreateCheckBoxListRoleType();
        }

        PanelResult.Visible = false;
        LabelOpMsg.Text = string.Empty;
    }

    private void CreateCheckBoxListRoleType()
    {
        ListItem item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.ProfessionalNotChoose), "0,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.XuanFeng), "0,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.XingTian), "0,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.ProfessionalNotChoose), "1,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.ZhenRen), "1,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.TianShi), "1,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.ProfessionalNotChoose), "2,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.ShouShi), "2,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.YiShi), "2,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void ButtonStatistic_Click(object sender, EventArgs e)
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
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone,StringDef.GameServer);
                return;
            }
            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }
            
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();            

            string roleClassText = string.Empty;
            StringBuilder roleTypeBuilder = new StringBuilder();
            foreach (ListItem item in CheckBoxListRoleType.Items)
            {
                if (item.Selected)
                {
                    string[] conValue = item.Value.Split(',');
                    roleTypeBuilder.AppendFormat("({0}='{1}' AND {2}='{3}') OR ", FS2TableString.RolesfirstFieldRoleType, conValue[0], FS2TableString.RolesfirstFieldSkillSeries, conValue[1]);
                }
            }
            if (roleTypeBuilder.Length != 0)
            {
                searchCondition.AppendFormat(" AND ({0}) ", roleTypeBuilder.ToString(0, roleTypeBuilder.Length - 3));
            }
            else
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.RoleClass);
                return;
            }

            int startLevel = int.Parse(TextBoxStartLevel.Text);
            if (startLevel > 1)
            {
                searchCondition.AppendFormat(" AND {0}>={1}", FS2TableString.RolesfirstFieldRoleLevel, startLevel);
            }

            int endLevel = int.Parse(TextBoxEndLevel.Text);
            if (endLevel < FS2GameDataManager.MaxLevel)
            {
                searchCondition.AppendFormat(" AND {0}<={1}", FS2TableString.RolesfirstFieldRoleLevel, endLevel);
            }

            switch (DropDownListSex.SelectedIndex)
            {
                case 1:
                    searchCondition.AppendFormat(" AND {0}={1}", FS2TableString.RolesfirstFieldRoleSex, (int)FS2RoleSex.Male);
                    break;
                case 2:
                    searchCondition.AppendFormat(" AND {0}={1}", FS2TableString.RolesfirstFieldRoleSex, (int)FS2RoleSex.Female);
                    break;
            }

            DateTime activeDate = DateCalendar.SelectedDate;
            if (DropDownListActivePlayer.SelectedIndex != 0 && activeDate == DateTime.MinValue)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgInputParamNotCorrect, StringDef.Date);
                return;
            }
            switch (DropDownListActivePlayer.SelectedIndex)
            {
                case 1:
                    //活跃玩家
                    searchCondition.AppendFormat(" AND {0}>='{1}' ", FS2TableString.RolesfirstFieldLastPlayingDate, activeDate.AddDays(-WebConfig.ActivePlayerDaySpan));
                    break;
                case 2:
                    //不活跃玩家
                    searchCondition.AppendFormat(" AND {0}<'{1}' ", FS2TableString.RolesfirstFieldLastPlayingDate, activeDate.AddDays(-WebConfig.ActivePlayerDaySpan));
                    break;
            }

            switch (DropDownListSocialRelation.SelectedIndex)
            {
                case 1:
                    searchCondition.AppendFormat(" AND ({0} IS NOT NULL AND {0}!='') ", FS2TableString.RolesfirstFieldTongGUID);
                    break;
                case 2:
                    searchCondition.AppendFormat(" AND ({0} IS NULL OR {0}='') ", FS2TableString.RolesfirstFieldTongGUID);
                    break;
            }

            //图表中的等级分组
            try
            {
                int group = int.Parse(TextBoxGroup.Text.Trim());
                if (group <= 0 || group > (endLevel - startLevel) + 1)
                {
                    LabelOpMsg.Text = StringDef.ParameterInputError;
                    return;
                }
            }
            catch(Exception)
            {
                LabelOpMsg.Text = StringDef.ParameterInputError;
                return;
            }

            if (searchCondition.Length > 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, " WHERE");
            }
                        
            string LevelQueryText = "SELECT RoleType,SkillSeries,Rolelevel,COUNT(*) FROM rolesfirst {0} GROUP BY RoleType,SkillSeries,RoleLevel ORDER BY roleType ASC,SkillSeries ASC,RoleLevel ASC;";

            SqlCommand cmd = new SqlCommand(string.Format(LevelQueryText, searchCondition.Length == 0 ? string.Empty : searchCondition.ToString()), string.Empty);
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(
                        new SqlDataType[] {
                        SqlDataType.SByte,
                        SqlDataType.SByte,
                        SqlDataType.Int32,
                        SqlDataType.Int32
                    });

                object[] record = null;
                _infoHashtable = new Hashtable();
                int totalNum = 0;                
                while ((record = result.ReadRecord()) != null)
                {
                    SByte roleClass = (SByte)record[0];
                    int skillSeries = (SByte)record[1];
                    FS2RoleType roleType = FS2RoleType.Jiashi;

                    switch (roleClass)
                    {
                        case (SByte)0:
                            if (skillSeries == 0) roleType = FS2RoleType.Xuanfeng;
                            else if (skillSeries == 1) roleType = FS2RoleType.Xingtian;
                            else roleType = FS2RoleType.Jiashi;
                            break;
                        case (SByte)1:
                            if (skillSeries == 0) roleType = FS2RoleType.Zhenren;
                            else if (skillSeries == 1) roleType = FS2RoleType.Tianshi;
                            else roleType = FS2RoleType.Daoshi;
                            break;
                        case (SByte)2:
                            if (skillSeries == 0) roleType = FS2RoleType.Shoushi;
                            else if (skillSeries == 1) roleType = FS2RoleType.Yishi;
                            else roleType = FS2RoleType.Yiren;
                            break;
                    }

                    if (_infoHashtable[roleType] == null)
                    {
                        _infoHashtable.Add(roleType, new ArrayList());
                    }
                    ArrayList roleTypeLevelList = _infoHashtable[roleType] as ArrayList;
                    if (roleTypeLevelList != null)
                    {
                        LevelInfo info = new LevelInfo();
                        info.Level = (int)record[2];
                        info.Num = (int)record[3];
                        roleTypeLevelList.Add(info);
                    }
                    totalNum += (int)record[3];
                }

                if (_infoHashtable != null && _infoHashtable.Count != 0)
                {
                    _total = totalNum;
                    PanelResult.Visible = true;
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name, server.Name,
                                            string.Format(StringDef.LabelRoleLevel, startLevel, endLevel),
                                            StringDef.LevelDistribution);
                }
                else
                {
                    PanelResult.Visible = false;
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

    protected void DropDownListActivePlayerSelectedIndexChanged(object sender, EventArgs e)
    {
        if (DropDownListActivePlayer.SelectedIndex != 0) DateCalendar.Enabled = true;
        else DateCalendar.Enabled = false;
    }    

    protected void ZedGraphWebLevelSuiStatistic_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
    {
        if (_infoHashtable != null)
        {
            //整理数据
            int startLevel = int.Parse(TextBoxStartLevel.Text.Trim());
            int endLevel = int.Parse(TextBoxEndLevel.Text.Trim());
            int groupCount = int.Parse(TextBoxGroup.Text.Trim());
            string[] levels = new string[(endLevel - startLevel) / groupCount + 1];
            double[] counts;
            Color[] roleTypeColor = new Color[]{
                Color.Blue,
                Color.Brown,
                Color.DarkGoldenrod,
                Color.Beige,
                Color.Yellow,
                Color.Coral,
                Color.Pink,
                Color.Green,
                Color.Gray
            };

            for (int index = 0; index != levels.Length; ++index)
            {
                levels[index] = string.Concat(startLevel + index * groupCount, '-', startLevel + (index + 1) * groupCount - 1);
            }           

            GraphPane graphPane = masterPane[0];

            //绘制图表
            graphPane.Title.Text = string.Format("{0}-{1} {2} {3}", startLevel, endLevel,
                StringDef.Role + StringDef.Total + StringDef.Colon, _total.ToString());
            graphPane.Title.IsVisible = true;
            graphPane.Title.FontSpec.Size = 14;

            graphPane.Fill = new Fill(WebConfig.GraphPaneBgColor);
            graphPane.Legend.FontSpec.Fill.IsVisible = false;
            graphPane.Legend.FontSpec.Size = 10.5f;
            graphPane.Legend.Fill.IsVisible = false;
            graphPane.Legend.Border.IsVisible = false;

            graphPane.YAxis.Title.Text = StringDef.Count;
            graphPane.YAxis.Title.FontSpec.IsBold = false;
            graphPane.YAxis.Title.FontSpec.Size = 10.5f;
            graphPane.YAxis.Scale.FontSpec.Size = 10.5f;
            graphPane.YAxis.MajorGrid.IsVisible = true;
            graphPane.YAxis.MajorGrid.DashOff = 0;
            graphPane.YAxis.MajorGrid.Color = Color.Gray;
            graphPane.YAxis.MinorGrid.IsVisible = true;
            graphPane.YAxis.MinorGrid.Color = Color.LightGray;
            graphPane.YAxis.MinorGrid.DashOff = 0;

            graphPane.XAxis.Title.Text = StringDef.Level;
            graphPane.XAxis.Title.FontSpec.IsBold = false;
            graphPane.XAxis.Title.FontSpec.Size = 10.5f;
            graphPane.XAxis.Scale.IsVisible = true;
            graphPane.XAxis.Type = AxisType.Text;
            graphPane.XAxis.Scale.TextLabels = levels;
            graphPane.XAxis.Scale.FontSpec.Size = 10.5f;

            switch (DropDownListBarType.SelectedItem.Text)
            {
                case "Stack":
                    graphPane.BarSettings.Type = BarType.Stack;
                    break;
                case "PercentStack":
                    graphPane.BarSettings.Type = BarType.PercentStack;
                    break;
                case "Cluster":
                    graphPane.BarSettings.Type = BarType.Cluster;
                    break;
                case "ClusterHiLow":
                    graphPane.BarSettings.Type = BarType.ClusterHiLow;
                    break;
                case "Overlay":
                    graphPane.BarSettings.Type = BarType.Overlay;
                    break;
                case "SortedOverlay":
                    graphPane.BarSettings.Type = BarType.SortedOverlay;
                    break;
            }            

            for (int roleType = 0; roleType != 10; ++roleType)
            {
                ArrayList infoList = _infoHashtable[(FS2RoleType)roleType] as ArrayList;
                if (infoList != null)
                {
                    counts = new double[(endLevel - startLevel) / groupCount + 1];
                    for (int index = 0; index != levels.Length; ++index)
                    {
                        foreach (LevelInfo info in (LevelInfo[])infoList.ToArray(typeof(LevelInfo)))
                        {
                            if (info.Level >= startLevel + index * groupCount && info.Level < startLevel + (index + 1) * groupCount)
                                counts[index] += info.Num;
                            else if (info.Level >= startLevel + (index + 1) * groupCount)
                                break;
                        }
                    }
                    string classDescription = string.Empty;
                    switch ((FS2RoleType)roleType)
                    {
                        case FS2RoleType.Jiashi:
                            classDescription = StringDef.Jiashi;
                            break;
                        case FS2RoleType.Xuanfeng:
                            classDescription = StringDef.XuanFeng;
                            break;
                        case FS2RoleType.Xingtian:
                            classDescription = StringDef.XingTian;
                            break;
                        case FS2RoleType.Daoshi:
                            classDescription = StringDef.Daoshi;
                            break;
                        case FS2RoleType.Zhenren:
                            classDescription = StringDef.ZhenRen;
                            break;
                        case FS2RoleType.Tianshi:
                            classDescription = StringDef.TianShi;
                            break;
                        case FS2RoleType.Yiren:
                            classDescription = StringDef.Yiren;
                            break;
                        case FS2RoleType.Shoushi:
                            classDescription = StringDef.ShouShi;
                            break;
                        case FS2RoleType.Yishi:
                            classDescription = StringDef.YiShi;
                            break;
                    }
                    BarItem bar = graphPane.AddBar(classDescription, null, counts, roleTypeColor[(int)roleType]);
                    bar.Bar.Fill = new Fill(roleTypeColor[(int)roleType]);
                }
            }

            masterPane.AxisChange(g);
            BarItem.CreateBarLabels(graphPane, true, string.Empty, TextObj.Default.FontFamily, 10.5f,
                TextObj.Default.FontColor, false, false, false);
        }
    }
}

public class LevelInfo
{
    public int Level;

    public int Num;

    public LevelInfo()
    {
        Level = 0;
        Num = 0;
    }

    public LevelInfo(int num,int level)
    {
        this.Level = level;
        this.Num = num;
    }
}