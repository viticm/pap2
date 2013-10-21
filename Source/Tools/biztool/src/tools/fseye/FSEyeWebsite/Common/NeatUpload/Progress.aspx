<%@ Page language="c#" AutoEventWireup="false" Inherits="Brettle.Web.NeatUpload.ProgressPage" %>
<%@ Register TagPrefix="Upload" Namespace="Brettle.Web.NeatUpload" Assembly="Brettle.Web.NeatUpload" %>
<%--
NeatUpload - an HttpModule and User Controls for uploading large files
Copyright (C) 2005  Dean Brettle

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>
<html>
	<head runat="server">
		<title>Upload Progress</title>
		<link rel="stylesheet" type="text/css" title="default" href="default.css" />		
		<style type="text/css">
<!--
		body, form, table, tr, td {
			margin: 0px;
			border: 0px none;
			padding: 0px;
            font-family: ËÎÌו;
	        font-size: 12px;
	        color: #000000;
		}

		#progressDisplayCenterer {
			vertical-align: middle;
			padding:2px;
			width: 100%;
			height: 100%;
		}
		
		#progressDisplay {
			vertical-align: middle;
			width: 100%;
		}
		
		#barTd {
			width: 80%;
		}
		
		#statusDiv {
			border-width: 1px;
			border-style: solid;
			padding: 0px;
			position: relative;
			width: 100%;
			text-align: center;
			z-index: 1; 
		}
		
		#barDiv,#barDetailsDiv {
			border: 0px none ; 
			margin: 0px; 
			padding: 0px; 
			position: absolute; 
			top: 0pt; 
			left: 0pt; 
			z-index: -1; 
			height: 100%;
			width: 75%;
		}
-->
		</style>
	</head>
	<body>
		<form id="dummyForm" runat="server">
		<table id="progressDisplayCenterer">
		<tr>
		<td>
		<table id="progressDisplay" class="ProgressDisplay">
		<tr>
        <td valign="middle">
			<span id="label" runat="server" class="Label"><asp:Label runat="server" Text="<%$ Resources:StringDef,Progress %>" SkinID="SmallTitle" /></span>
		</td>
		<td id="barTd" valign="middle" >		    
			<div id="statusDiv" runat="server" class="StatusMessage">
			    <Upload:DetailsSpan id="DetailsSpan1" runat="server" WhenStatus="NormalInProgress" style="font-weight: normal; white-space: nowrap;">
					<%# String.Format("{0:0%}", FractionComplete) %>
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="DetailsSpan2" runat="server" WhenStatus="Cancelled">
					<%# String.Format("{0:0%}", FractionComplete) %>
				</Upload:DetailsSpan>
				<Upload:DetailsDiv id="barDetailsDiv" runat="server" UseHtml4="true"
					 Width='<%# Unit.Percentage(Math.Floor(100*FractionComplete)) %>' class="ProgressBar">
                </Upload:DetailsDiv>	
			</div>
		</td>
		<td valign="middle" >
			<asp:HyperLink id="cancel" runat="server" Visible='<%# CancelVisible %>' NavigateUrl='<%# CancelUrl %>' title="<%$ Resources:StringDef,Cancel %>" SkinID="plainText" ><asp:Literal runat="server" Text="<%$ Resources:StringDef,Cancel %>" /></asp:HyperLink>
			<asp:HyperLink id="refresh" runat="server" Visible='<%# StartRefreshVisible %>' NavigateUrl='<%# StartRefreshUrl %>' Text="<%$ Resources:StringDef,Refresh %>" SkinID="PlainText" ><asp:Literal runat="server" Text="<%$ Resources:StringDef,Refresh %>" /></asp:HyperLink>
			<asp:HyperLink id="stopRefresh" runat="server" Visible='<%# StopRefreshVisible %>' NavigateUrl='<%# StopRefreshUrl %>' Text="<%$ Resources:StringDef,StopRefreshing %>" SkinID="PlainText" >StopRefresh</asp:HyperLink>
		</td>
		</tr>
		<tr>
		    <td colspan="3">
		        <Upload:DetailsSpan id="normalInProgress" runat="server" WhenStatus="NormalInProgress" style="font-weight: normal; white-space: nowrap;">
					<%# FormatCount(BytesRead) %>/<%# FormatCount(BytesTotal) %> <%# CountUnits %>
					(<%# String.Format("{0:0%}", FractionComplete) %>) at <%# FormatRate(BytesPerSec) %>
					- <%# FormatTimeSpan(TimeRemaining) %> <asp:Literal ID="Literal1" runat="server" Text="<%$ Resources:StringDef,LeftSecond %>" />
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="chunkedInProgress" runat="server" WhenStatus="ChunkedInProgress" style="font-weight: normal; white-space: nowrap;">
					<%# FormatCount(BytesRead) %> <%# CountUnits %>
					at <%# FormatRate(BytesPerSec) %>
					- <%# FormatTimeSpan(TimeElapsed) %> elapsed
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="completed" runat="server" WhenStatus="Completed">
					Complete: <%# FormatCount(BytesRead) %> <%# CountUnits %>
					at <%# FormatRate(BytesPerSec) %>
					took <%# FormatTimeSpan(TimeElapsed) %>
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="cancelled" runat="server" WhenStatus="Cancelled">
					<asp:Literal ID="Literal2" runat="server" Text="<%$ Resources:StringDef,Cancelled %>" />
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="rejected" runat="server" WhenStatus="Rejected">
					<asp:Literal ID="Literal3" runat="server" Text="<%$ Resources:StringDef,Rejected %>" /> <%# Rejection != null ? Rejection.Message : "" %>
				</Upload:DetailsSpan>
				<Upload:DetailsSpan id="error" runat="server" WhenStatus="Failed">
					<asp:Literal ID="Literal4" runat="server" Text="<%$ Resources:StringDef,Error %>" /> <%# Failure != null ? Failure.Message : "" %>
				</Upload:DetailsSpan>
		    </td>		
		</tr>
		</table>
		</td>
		</tr>
		</table>
		</form>
	</body>
</html>
