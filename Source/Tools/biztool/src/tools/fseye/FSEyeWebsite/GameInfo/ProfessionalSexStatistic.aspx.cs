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

public partial class GameInfo_ProfessionalSexStatistic : BasePage
{
    FS2RoleDataInfo[] _statInfo;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticProfessionalSexStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
            DateCalendar.SelectedDate = DateTime.Now;
        }

        LabelOpMsg.Text = string.Empty;
        PanelResult.Visible = false;
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

            string RoleSexQueryText = "SELECT RoleType, RoleSex,TongGUID,SkillSeries FROM rolesfirst {0}";

            int startLevel = int.Parse(TextBoxStartLevel.Text);
            if (startLevel > 1)
            {
                searchCondition.Append(string.Format(" AND {0}>={1}", FS2TableString.RolesfirstFieldRoleLevel, startLevel));
            }

            int endLevel = int.Parse(TextBoxEndLevel.Text);
            if (endLevel < FS2GameDataManager.MaxLevel)
            {
                searchCondition.Append(string.Format(" AND {0}<={1}", FS2TableString.RolesfirstFieldRoleLevel, endLevel));
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

            if (searchCondition.Length > 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, " WHERE");
            }

            SqlCommand cmd = new SqlCommand(string.Format(RoleSexQueryText, searchCondition.Length == 0 ? string.Empty : searchCondition.ToString()), string.Empty);
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                result.SetFieldType(
                        new SqlDataType[] {
                        SqlDataType.SByte,
                        SqlDataType.SByte,
                        SqlDataType.String,
                        SqlDataType.Int32
                    });

                object[] record = null;
                RoleSexTotalInfo totalInfo = new RoleSexTotalInfo();
                ArrayList infos = new ArrayList();
                while ((record = result.ReadRecord()) != null)
                {
                    //RoleType, RoleSex,TongGUID,SkillSeries
                    FS2RoleDataInfo info = new FS2RoleDataInfo();
                    info.SkillSeries = (int)record[3];
                    switch ((SByte)record[0])
                    {
                        case (SByte)0:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Xuanfeng;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Xingtian;
                            else info.RoleType = FS2RoleType.Jiashi;
                            break;
                        case (SByte)1:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Zhenren;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Tianshi;
                            else info.RoleType = FS2RoleType.Daoshi;
                            break;
                        case (SByte)2:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Shoushi;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Yishi;
                            else info.RoleType = FS2RoleType.Yiren;
                            break;
                    }

                    info.RoleSex = (FS2RoleSex)((SByte)record[1]);
                    info.TongName = record[2] as string;

                    switch (DropDownListSocialRelation.SelectedIndex)
                    {
                        case 0:
                            infos.Add(info);
                            break;
                        case 1:
                            if (info.TongName != null && info.TongName != string.Empty) infos.Add(info);
                            break;
                        case 2:
                            if (info.TongName == null|| info.TongName==string.Empty) infos.Add(info);
                            break;
                        //case 3:
                        //    if (IsInZhuhou(info)) infos.Add(info);
                        //    break;
                        //case 4:
                        //    if (IsInNation(info)) infos.Add(info);
                        //    break;
                    }
                }

                if (infos.Count != 0)
                {
                    PanelResult.Visible = true;
                    _statInfo = (FS2RoleDataInfo[])infos.ToArray(typeof(FS2RoleDataInfo));
                    CreateSearchRoleSexList((FS2RoleDataInfo[])infos.ToArray(typeof(FS2RoleDataInfo)));
                                        LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name,server.Name,
                                        string.Format(StringDef.LabelRoleLevel, startLevel, endLevel),
                                        StringDef.ProfessionalSex);
                }
                else
                {
                    PanelResult.Visible = false;
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                }
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

    bool CreateSearchRoleSexList(FS2RoleDataInfo[] infos)
    {
        RoleSexTotalInfo totalInfo = null;
        if (infos != null)
        {
            totalInfo = new RoleSexTotalInfo();
            foreach (FS2RoleDataInfo info in infos)
            {
                totalInfo.infos[Convert.ToInt16(info.RoleType)].num[(int)info.RoleSex]++;
            }
        }

        if (totalInfo != null)
        {
            int totalMale = 0;
            int totalFemale = 0;
            int totalMaleJia = 0;
            int totalFemaleJia = 0;
            int totalMaleDao = 0;
            int totalFemaleDao = 0;
            int totalMaleYi = 0;
            int totalFemaleYi = 0;

            int rowNum = 0;
            foreach (RoleSexStatisticInfo info in totalInfo.infos)
            {
                ++rowNum;
                totalMale += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                totalFemale += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                TableRow row = new TableRow();

                //三类职业行标头
                TableCell headerCell = new TableCell();
                headerCell.RowSpan = 3;                
                switch(rowNum) 
                {   
                    case 1:
                        headerCell.Text = StringDef.Jiashi;
                        row.Cells.Add(headerCell);
                        break;
                    case 4:
                        headerCell.Text = StringDef.Daoshi;
                        row.Cells.Add(headerCell);
                        break;
                    case 7:
                        headerCell.Text = StringDef.Yiren;
                        row.Cells.Add(headerCell);
                        break;                    
                }

                TableCell cell1 = new TableCell();

                string classDescription = string.Empty;
                switch (info.roleType)
                {
                    case FS2RoleType.Jiashi:
                        classDescription = StringDef.Jiashi + string.Format("[{0}]", StringDef.ProfessionalNotChoose);
                        totalMaleJia += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleJia += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Xuanfeng:
                        classDescription = StringDef.XuanFeng;
                        totalMaleJia += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleJia += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Xingtian:
                        classDescription = StringDef.XingTian;
                        totalMaleJia += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleJia += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Daoshi:
                        classDescription = StringDef.Daoshi + string.Format("[{0}]", StringDef.ProfessionalNotChoose);
                        totalMaleDao += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleDao += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Zhenren:
                        classDescription = StringDef.ZhenRen;
                        totalMaleDao += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleDao += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Tianshi:
                        classDescription = StringDef.TianShi;
                        totalMaleDao += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleDao += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Yiren:
                        classDescription = StringDef.Yiren + string.Format("[{0}]", StringDef.ProfessionalNotChoose);
                        totalMaleYi += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleYi += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Shoushi:
                        classDescription = StringDef.ShouShi;
                        totalMaleYi += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleYi += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                    case FS2RoleType.Yishi:
                        classDescription = StringDef.YiShi;
                        totalMaleYi += info.num[Convert.ToInt16(FS2RoleSex.Male)];
                        totalFemaleYi += info.num[Convert.ToInt16(FS2RoleSex.Female)];
                        break;
                }
                cell1.Text = classDescription;
                TableCell cell2 = new TableCell();
                if (info.num[Convert.ToInt16(FS2RoleSex.Male)] != 0)
                {
                    cell2.Text = string.Format("{0} ({1}%)",
                                               info.num[Convert.ToInt16(FS2RoleSex.Male)].ToString(),
                                               Decimal.Round((decimal)info.num[Convert.ToInt16(FS2RoleSex.Male)] / info.TotalNum * 100, 2).ToString());
                }
                else
                { cell2.Text = "0"; }
                TableCell cell3 = new TableCell();
                if (info.num[Convert.ToInt16(FS2RoleSex.Female)] != 0)
                {
                    cell3.Text = string.Format("{0} ({1}%)",
                                              info.num[Convert.ToInt16(FS2RoleSex.Female)].ToString(),
                                              Decimal.Round((decimal)info.num[Convert.ToInt16(FS2RoleSex.Female)] / info.TotalNum * 100, 2).ToString());
                }
                else
                { cell3.Text = "0"; }
                TableCell cell4 = new TableCell();
                //cell4.Text = string.Format("{0} (100%)", info.TotalNum);
                row.Cells.Add(cell1);
                row.Cells.Add(cell2);
                row.Cells.Add(cell3);
                row.Cells.Add(cell4);

                TableCell tailCell = new TableCell();
                tailCell.RowSpan = 3;                
                switch (rowNum)
                {
                    case 1:
                    case 4:
                    case 7:
                        row.Cells.Add(tailCell);
                        break;
                }

                TableSearchRoleSexStatistic.Rows.Add(row);
            }

            //九种职业统计
            for (int totalInfoIndex = 0; totalInfoIndex != totalInfo.infos.Length; ++totalInfoIndex)
            {
                RoleSexStatisticInfo info = totalInfo.infos[totalInfoIndex] as RoleSexStatisticInfo;
                if (info != null)
                {
                    switch (info.roleType)
                    {
                        case FS2RoleType.Jiashi:
                            if ((totalMaleJia + totalFemaleJia) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleJia + totalFemaleJia) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = "0";
                            }
                            break;
                        case FS2RoleType.Xuanfeng:
                        case FS2RoleType.Xingtian:
                            if ((totalMaleJia + totalFemaleJia) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleJia + totalFemaleJia) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = "0";
                            }
                            break;
                        case FS2RoleType.Daoshi:
                            if ((totalMaleDao + totalFemaleDao) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleDao + totalFemaleDao) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = "0";
                            }
                            break;
                        case FS2RoleType.Zhenren:
                        case FS2RoleType.Tianshi:
                            if ((totalMaleDao + totalFemaleDao) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleDao + totalFemaleDao) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = "0 (0%)";
                            }
                            break;
                        case FS2RoleType.Yiren:
                            if ((totalMaleYi + totalFemaleYi) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleYi + totalFemaleYi) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[4].Text = "0 (0%)";
                            }
                            break;
                        case FS2RoleType.Shoushi:
                        case FS2RoleType.Yishi:
                            if ((totalMaleYi + totalFemaleYi) != 0)
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = string.Format("{0} ({1}%)",
                                    info.TotalNum,
                                    Decimal.Round((decimal)info.TotalNum / (totalMaleYi + totalFemaleYi) * 100, 2).ToString());
                            }
                            else
                            {
                                TableSearchRoleSexStatistic.Rows[totalInfoIndex + 1].Cells[3].Text = "0 (0%)";
                            }
                            break;
                    }                    
                }
            }

            //三类职业总统计
            string typeStat = "{0}<br/>" + StringDef.Male + StringDef.Colon + "{1} {2}<br/>" + StringDef.Female + StringDef.Colon + "{3} {4}<br/>"+StringDef.Count+StringDef.Colon+" {5}";
            TableSearchRoleSexStatistic.Rows[1].Cells[5].Text = string.Format(typeStat,
                                    StringDef.Jiashi,
                                    totalMaleJia,
                                    totalMaleJia + totalFemaleJia == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalMaleJia / (totalMaleJia + totalFemaleJia) * 100, 2)),
                                    totalFemaleJia,
                                    totalMaleJia + totalFemaleJia == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalFemaleJia / (totalMaleJia + totalFemaleJia) * 100, 2)),
                                    totalMaleJia + totalFemaleJia);
            
            TableSearchRoleSexStatistic.Rows[4].Cells[5].Text = string.Format(typeStat,
                                    StringDef.Daoshi,
                                    totalMaleDao,
                                    totalMaleDao + totalFemaleDao == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalMaleDao / (totalMaleDao + totalFemaleDao) * 100, 2)),
                                    totalFemaleDao,
                                    totalMaleDao + totalFemaleDao == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalFemaleDao / (totalMaleDao + totalFemaleDao) * 100, 2)),
                                    totalMaleDao + totalFemaleDao);

            TableSearchRoleSexStatistic.Rows[7].Cells[5].Text = string.Format(typeStat,
                                    StringDef.Yiren,
                                    totalMaleYi,
                                    totalMaleYi + totalFemaleYi == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalMaleYi / (totalMaleYi + totalFemaleYi) * 100, 2)),
                                    totalFemaleYi,
                                    totalMaleYi + totalFemaleYi == 0 ? string.Empty : string.Format("({0}%)", Decimal.Round((decimal)totalFemaleYi / (totalMaleYi + totalFemaleYi) * 100, 2)),
                                    totalMaleYi + totalFemaleYi);

            TableRow rowTotal = new TableRow();
            rowTotal.Font.Bold = true;
            TableCell cell = new TableCell();
            cell.ColumnSpan = 2;
            cell.Text = StringDef.Total;
            rowTotal.Cells.Add(cell);
            cell = new TableCell();
            if(totalMale!=0)
            {
                cell.Text = string.Format("{0} ({1}%)", totalMale, Decimal.Round((decimal)totalMale / (totalMale + totalFemale) * 100, 2));
            }
            else 
            {cell.Text = "0";}
            rowTotal.Cells.Add(cell);
            cell = new TableCell();
            if (totalFemale != 0)
            {
                cell.Text = string.Format("{0} ({1}%)", totalFemale, Decimal.Round((decimal)totalFemale / (totalMale + totalFemale) * 100, 2));
            }
            else
            { cell.Text = "0"; }
            rowTotal.Cells.Add(cell);
            cell = new TableCell();
            cell.ColumnSpan = 2;
            if ((totalMale + totalFemale) != 0)
            {
                cell.Text = string.Format("{0} (100%)", totalMale + totalFemale);
            }
            else { cell.Text = "0"; }
            rowTotal.Cells.Add(cell);
            TableSearchRoleSexStatistic.Rows.Add(rowTotal);
        }
        return true;
    }
        
    protected void ZedGraphWebProfsStat_RenderGraph(ZedGraph.Web.ZedGraphWeb webObject, Graphics g, MasterPane masterPane)
    {
        //if (_statInfo != null)
        //{
        //    GraphPane myPane = masterPane[0];

        //    myPane.Fill = new Fill(Color.White, Color.Goldenrod, 45.0f);
        //    myPane.Chart.Fill.Type = FillType.None;

        //    myPane.Legend.Position = LegendPos.Float;
        //    myPane.Legend.Location = new Location(0.95f, 0.15f, CoordType.PaneFraction, AlignH.Right, AlignV.Top);
        //    myPane.Legend.IsHStack = false;

        //    // Add some pie slices
        //    for (int index = 0; index != param.Length; index += 2)
        //    {
        //        double count = Convert.ToDouble(param[index]);
        //        string label = param[index + 1] as string;
        //        PieItem segment = myPane.AddPieSlice((double)count, Color.Navy, Color.White, (float)(count / totalCount), 0, label);
        //    }
        //}
    }
}


/// <summary>
/// 职业性别统计信息
/// </summary>
public class RoleSexStatisticInfo
{
    public FS2RoleType roleType;

    public FS2RoleSex roleSex;

    public int[] num;

    public int TotalNum
    {
        get { return num[Convert.ToInt16(FS2RoleSex.Male)] + num[Convert.ToInt16(FS2RoleSex.Female)]; }
    }

    public RoleSexStatisticInfo()
    {
        num = new int[Enum.GetValues(typeof(FS2RoleSex)).Length];
        for (int i = 0; i != Enum.GetValues(typeof(FS2RoleSex)).Length; ++i)
            num[i] = 0;
    }
}

public class RoleSexTotalInfo
{
    public RoleSexStatisticInfo[] infos;

    public RoleSexTotalInfo()
    {
        infos = new RoleSexStatisticInfo[Enum.GetValues(typeof(FS2RoleType)).Length];
        for (int i = 0; i != Enum.GetValues(typeof(FS2RoleType)).Length; ++i)
        {
            infos[i] = new RoleSexStatisticInfo();
            infos[i].roleType = (FS2RoleType)i;
        }
    }
}