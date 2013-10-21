<frameset rows="40,*" frameborder="NO" border="0" framespacing="0">
  <frame src="top.asp?id=<%=request("id")%>" name="topFrame" scrolling="NO" noresize>
  <frame src="viewdocument.asp?id=<%=request("id")%>" name="mainFrame" scrolling="auto">
</frameset>