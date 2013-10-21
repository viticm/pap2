<%
response.buffer = false
%>
<!--#include file="conn.asp"-->
<%
id = request.querystring("id")
if id = 0 then
	'no ducument
	response.write "ID error~"
	response.end
end if
set rs = conn.execute("select * from sys_notes where id=" & id)
if rs.eof then
	'no ducument
	response.write "No ducument exist~"
	response.end
end if
out_string = "" & rs("htmlcode")
%>
<style type="text/css">
<!--
body {
	background-color: #8EB3E7;
}
table {
	left: 0px;
	top: 0px;
}
-->
</style>
<script language="JavaScript" type="text/JavaScript">
<!--
function MM_reloadPage(init) {  //reloads the window if Nav4 resized
  if (init==true) with (navigator) {if ((appName=="Netscape")&&(parseInt(appVersion)==4)) {
    document.MM_pgW=innerWidth; document.MM_pgH=innerHeight; onresize=MM_reloadPage; }}
  else if (innerWidth!=document.MM_pgW || innerHeight!=document.MM_pgH) location.reload();
}
MM_reloadPage(true);
//-->
</script>

<div id="Layer1" style="position:absolute; left:4px; top:4px; width:764px; height:33px; z-index:1">
  <table align="center">
    <tr align="center">
      <td align="center"><b>作者：</b><%=rs("author")%>&nbsp;&nbsp; <b>创建时间：</b><%=rs("createtime")%>&nbsp;&nbsp; <b>修订：</b><%=rs("modifyby")%>&nbsp;&nbsp; <b>修订时间：</b><%=rs("modifytime")%>&nbsp;&nbsp;
          <input type="button" onclick="parent.location='editdocument.asp?id=' + <%=id%>" value="编辑">
      </td>
  </table>
</div>
