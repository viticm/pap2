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
<%=out_string%>

