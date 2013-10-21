<%@ control language="C#" autoeventwireup="true" inherits="Common_DateCalendar, App_Web_1ceayx6e" %>

<asp:Table ID="TableTest" runat="server" Width="0">
    <asp:TableRow VerticalAlign="Top">
        <asp:TableCell Width="100" VerticalAlign="Middle">
            <asp:Label ID="DateLabel" runat="server" Visible="true"></asp:Label>
            <asp:CheckBox ID="DateCheck" runat="server" OnCheckedChanged="Check_Click" AutoPostBack="true"></asp:CheckBox>
        </asp:TableCell>
        <asp:TableCell>
            <asp:TextBox ID="DateTextBox" runat="server" Height="15" Width="80" EnableTheming="true"/>
        </asp:TableCell>
        <asp:TableCell HorizontalAlign="Left">
            <asp:Button ID="ButtonSelect" runat="server" OnClick="Select" Text="<%$ Resources:StringDef,SelectDate %>" />
        </asp:TableCell>
    </asp:TableRow>
    <asp:TableRow>
        <asp:TableCell></asp:TableCell>
        <asp:TableCell></asp:TableCell>
        <asp:TableCell>
            <asp:Calendar ID="DateCalendarSelect" runat="server" BackColor="White" BorderColor="White"
                BorderWidth="1px" Font-Names="Verdana" Font-Size="9pt" ForeColor="Black" Height="150px"
                NextPrevFormat="FullMonth" ShowGridLines="True" Width="200px" Visible="false"
                OnSelectionChanged="SelectedChanged">
                <SelectedDayStyle BackColor="#333399" ForeColor="White" />
                <TodayDayStyle BackColor="#CCCCCC" />
                <OtherMonthDayStyle ForeColor="#999999" />
                <NextPrevStyle Font-Bold="True" Font-Size="8pt" ForeColor="#333333" VerticalAlign="Bottom" />
                <DayHeaderStyle Font-Bold="True" Font-Size="8pt" />
                <TitleStyle BackColor="White" BorderColor="Black" BorderWidth="4px" Font-Bold="True"
                    Font-Size="12pt" ForeColor="#333399" />
            </asp:Calendar>
        </asp:TableCell>
    </asp:TableRow>
</asp:Table>
