<!--#include file="../conn.asp"-->
<%
id = request.querystring("id")
set rs = conn.execute("select * from sys_upload where id=" & id)
response.ContentType = rs("content_type")
response.BinaryWrite rs("data").GetChunk(7500000)
%>
