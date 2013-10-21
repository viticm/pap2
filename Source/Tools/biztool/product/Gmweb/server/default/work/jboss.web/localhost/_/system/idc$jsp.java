package org.apache.jsp;

import java.sql.*;
import java.util.*;
import  com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class idc$jsp extends HttpJspBase {

    // begin [file="/system/idc.jsp";from=(3,0);to=(3,102)]
    // end
    // begin [file="/system/idc.jsp";from=(4,0);to=(4,107)]
    // end

    static {
    }
    public idc$jsp( ) {
    }

    private static boolean _jspx_inited = false;

    public final void _jspx_init() throws org.apache.jasper.runtime.JspException {
    }

    public void _jspService(HttpServletRequest request, HttpServletResponse  response)
        throws java.io.IOException, ServletException {

        JspFactory _jspxFactory = null;
        PageContext pageContext = null;
        HttpSession session = null;
        ServletContext application = null;
        ServletConfig config = null;
        JspWriter out = null;
        Object page = this;
        String  _value = null;
        try {

            if (_jspx_inited == false) {
                synchronized (this) {
                    if (_jspx_inited == false) {
                        _jspx_init();
                        _jspx_inited = true;
                    }
                }
            }
            _jspxFactory = JspFactory.getDefaultFactory();
            response.setContentType("text/html; charset=GBK");
            pageContext = _jspxFactory.getPageContext(this, request, response,
            			"/error.jsp", true, 8192, true);

            application = pageContext.getServletContext();
            config = pageContext.getServletConfig();
            session = pageContext.getSession();
            out = pageContext.getOut();

            // HTML // begin [file="/system/idc.jsp";from=(0,162);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/idc.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/idc.jsp";from=(2,0);to=(2,37)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("idc");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/idc.jsp";from=(2,0);to=(2,37)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(2,37);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/idc.jsp";from=(3,0);to=(3,102)]
                com.kingsoft.gmsystem.service.SystemHelper SystemHelperId = null;
                boolean _jspx_specialSystemHelperId  = false;
                 synchronized (session) {
                    SystemHelperId= (com.kingsoft.gmsystem.service.SystemHelper)
                    pageContext.getAttribute("SystemHelperId",PageContext.SESSION_SCOPE);
                    if ( SystemHelperId == null ) {
                        _jspx_specialSystemHelperId = true;
                        try {
                            SystemHelperId = (com.kingsoft.gmsystem.service.SystemHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SystemHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SystemHelper", exc);
                        }
                        pageContext.setAttribute("SystemHelperId", SystemHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSystemHelperId == true) {
            // end
            // begin [file="/system/idc.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/idc.jsp";from=(4,0);to=(4,107)]
                com.kingsoft.gmsystem.common.QueryPagination QueryPaginationId = null;
                boolean _jspx_specialQueryPaginationId  = false;
                 synchronized (session) {
                    QueryPaginationId= (com.kingsoft.gmsystem.common.QueryPagination)
                    pageContext.getAttribute("QueryPaginationId",PageContext.SESSION_SCOPE);
                    if ( QueryPaginationId == null ) {
                        _jspx_specialQueryPaginationId = true;
                        try {
                            QueryPaginationId = (com.kingsoft.gmsystem.common.QueryPagination) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.common.QueryPagination");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.common.QueryPagination", exc);
                        }
                        pageContext.setAttribute("QueryPaginationId", QueryPaginationId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialQueryPaginationId == true) {
            // end
            // begin [file="/system/idc.jsp";from=(4,0);to=(4,107)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(4,107);to=(13,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>游戏IDC维护</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/idc.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(13,42);to=(17,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/system/idc.jsp";from=(17,2);to=(22,0)]
                
                    //this prevents caching in NS and IE
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(22,2);to=(226,101)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar objChk=null;//当前被选中的\r\nvar action=null;\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/system/idc.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/system/idc.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"idc_name\").value=objCurChk.idc_name;\r\n\t\tdocument.all(\"idc_ip\").value=objCurChk.idc_ip;\r\n\t\tdocument.all(\"db_name\").value=objCurChk.db_name;\r\n\t\tdocument.all(\"username\").value=objCurChk.username;\r\n\t\tdocument.all(\"password\").value=objCurChk.password;\r\n\t\tSetSelValue(\"game_id\",objChk.game_id);\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"btnDel\").disabled=false;\r\n\t}\r\n}\r\n\r\n/******************************\r\ninflag: true，可编辑   false,不可编辑状态\r\n*******************************/\r\nfunction SetEditModel(inflag)\r\n{\r\n\tSetChkState(inflag);\r\n\tdocument.all(\"game_id\").disabled=!inflag;\r\n\tdocument.all(\"idc_name\").disabled=!inflag;\r\n\tdocument.all(\"idc_ip\").disabled=!inflag;\r\n\tdocument.all(\"db_name\").disabled=!inflag;\r\n\tdocument.all(\"username\").disabled=!inflag;\r\n\tdocument.all(\"password\").disabled=!inflag;\r\n\tif(action==\"addidcinfo\")\r\n\t{\r\n\t\tdocument.all(\"idc_name\").value=\"\";\r\n\t\tdocument.all(\"idc_ip\").value=\"\";\r\n\t\tdocument.all(\"db_name\").value=\"\";\r\n\t\tdocument.all(\"username\").value=\"\";\r\n\t\tdocument.all(\"password\").value=\"\";\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=inflag;\r\n\tdocument.all(\"btnModify\").disabled=inflag;\r\n\tdocument.all(\"btnDel\").disabled=inflag;\r\n\tdocument.all(\"btnSave\").disabled=!inflag;\r\n\tdocument.all(\"btnCancel\").disabled=!inflag;\r\n\tif(objChk){\r\n\t\tobjChk.checked=true;\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n增加\r\n******************************/\r\nfunction onAdd()\r\n{\r\n\tif(objChk){\r\n\t\tobjChk.checked=false;\r\n\t}\r\n\taction=\"addidcinfo\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"idc_name\").focus();\r\n}\r\n\r\n/******************************\r\n修改\r\n*****************************/\r\nfunction onModify()\r\n{\r\n\taction=\"modifyidcinfo\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"idc_name\").focus();\r\n}\r\n\r\n/****************************\r\n删除\r\n****************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此游戏IDC吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delidcinfo\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n保存\r\n*****************************/\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"idc_name\")) strErr+=\"*IDC名称不能为空！\\n\";\r\n\tif(IsEmpty(\"idc_ip\")) strErr+=\"*IDC—IP不能空！\\n\";\r\n\tif(IsEmpty(\"db_name\")) strErr+=\"*数据库名不能空！\\n\";\r\n\tif(IsEmpty(\"username\")) strErr+=\"*用户名不能空！\\n\";\r\n\tif(IsEmpty(\"password\")) strErr+=\"*登录密码不能空！\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/******************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetEditModel(false);\r\n\taction=\"\";\r\n\tif(document.all(\"action\").value==\"addidcinfo\")\r\n\t{\r\n\t\tdocument.all(\"idc_name\").value=\"\";\r\n\t\tdocument.all(\"idc_ip\").value=\"\";\r\n\t\tdocument.all(\"db_name\").value=\"\";\r\n\t\tdocument.all(\"username\").value=\"\";\r\n\t\tdocument.all(\"password\").value=\"\";\r\n\t}\r\n\telse if(objChk)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objChk.iid;\r\n\t\tdocument.all(\"idc_name\").value=objChk.idc_name;\r\n\t\tdocument.all(\"idc_ip\").value=objChk.idc_ip;\r\n\t\tdocument.all(\"db_name\").value=objChk.db_name;\r\n\t\tdocument.all(\"username\").value=objChk.username;\r\n\t\tdocument.all(\"password\").value=objChk.password;\r\n\t}\r\n}\r\n\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">游戏IDC维护</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n    <br>\r\n      <table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td height=\"200\" align=\"center\" valign=\"top\"> <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/system/idc.jsp";from=(226,104);to=(226,128)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(226,130);to=(239,16)]
                out.write("/SystemServlet\">\r\n              <table width=\"80%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n                <thead>\r\n                  <tr>\r\n                    <td width=\"8%\" align=\"center\" nowrap width=\"30\"><font color=\"#FFFFFF\">序号</font></td>\r\n                    <td width=\"10%\" align=\"center\"><font color=\"#FFFFFF\">所属游戏</font></td>\r\n                    <td width=\"15%\" align=\"center\"><font color=\"#FFFFFF\">IDC名称</font></td>\r\n                    <td width=\"15%\" align=\"center\"><font color=\"#FFFFFF\">IDC-IP</font></td>\r\n                    <td width=\"12%\" align=\"center\"><font color=\"#FFFFFF\">数据库名</font></td>\r\n                    <td width=\"12%\" align=\"center\"><font color=\"#FFFFFF\">用户名</font></td>\r\n                    <td width=\"12%\" align=\"center\"><font color=\"#FFFFFF\">登录密码</font></td>\r\n                  </tr>\r\n                </thead>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(239,18);to=(292,0)]
                
                //*******Begin******************************************************************
                   int curPageEndIndex=0;// 当前页 的尾索引
                   int curPageStartIndex=0;// 当前页 的索引
                   int nStep=24;
                   int lastPageStartIndex=0;//last page startindex
                   int isNext=0; //1:有下一页  2：无下一页
                   int isPrevious=0;  // false:无上一页
                   int sumPageNumber=0;//公有页数
                   int curPageIndex=0;//第几页
                   String strPageAction="";
                   ArrayList lt=new ArrayList();
                   int[] curPageInfo = new int[8];
                   int num=0;
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                
                   if (hs1.containsKey("pageAction")) {
                       String page1 = (String)hs1.get("pageAction");
                       if (page1.trim().equals("servlet")) {
                           se.setAttribute("idc", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("idc");
                       }
                       num = SystemHelperId.getIDCInfoListNum();
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       lt = SystemHelperId.getIDCInfoListByPage(curPageInfo[0], curPageInfo[2]);
                   }
                //*********End***************************************************
                	String iid ="";
                	String game_id = "";
                	String game_name="";
                	String idc_name="";
                	String idc_ip="";
                	String db_name="";
                	String username="";
                	String password="";
                for (int i=0; i<lt.size(); i++)
                {
                	HashMap hs = (HashMap)lt.get(i);
                	iid = (String)hs.get("iid");
                	game_id = (String)hs.get("game_id");
                	game_name = (String)hs.get("game_name");
                	idc_name= (String)hs.get("idc_name");
                	idc_ip= (String)hs.get("idc_ip");
                	db_name= (String)hs.get("db_name");
                	username= (String)hs.get("username");
                	password= (String)hs.get("password");
                	username = Util.getStars(username.length());
                	password = Util.getStars(password.length());
                
            // end
            // HTML // begin [file="/system/idc.jsp";from=(292,2);to=(293,21)]
                out.write("\r\n                <tr  ");

            // end
            // begin [file="/system/idc.jsp";from=(293,23);to=(293,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/idc.jsp";from=(293,36);to=(293,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/idc.jsp";from=(293,56);to=(293,57)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(293,59);to=(294,92)]
                out.write(">\r\n                  <td align=\"center\" valign=\"middle\" nowrap><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/system/idc.jsp";from=(294,95);to=(294,96)]
                out.print(i);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,98);to=(294,109)]
                out.write("\" game_id=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,112);to=(294,119)]
                out.print(game_id);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,121);to=(294,133)]
                out.write("\" idc_name=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,136);to=(294,144)]
                out.print(idc_name);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,146);to=(294,156)]
                out.write("\" idc_ip=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,159);to=(294,165)]
                out.print(idc_ip);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,167);to=(294,174)]
                out.write("\" iid=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,177);to=(294,180)]
                out.print(iid);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,182);to=(294,193)]
                out.write("\" db_name=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,196);to=(294,203)]
                out.print(db_name);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,205);to=(294,217)]
                out.write("\" username=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,220);to=(294,228)]
                out.print(username);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,230);to=(294,242)]
                out.write("\" password=\"");

            // end
            // begin [file="/system/idc.jsp";from=(294,245);to=(294,253)]
                out.print(password);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,255);to=(294,279)]
                out.write("\" onClick=\"onSelect()\" >");

            // end
            // begin [file="/system/idc.jsp";from=(294,282);to=(294,285)]
                out.print(i+1);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(294,287);to=(296,37)]
                out.write("\r\n                  </td>\r\n                  <td align=\"center\">");

            // end
            // begin [file="/system/idc.jsp";from=(296,40);to=(296,49)]
                out.print(game_name);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(296,51);to=(297,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/idc.jsp";from=(297,47);to=(297,55)]
                out.print(idc_name);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(297,57);to=(298,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/idc.jsp";from=(298,47);to=(298,53)]
                out.print(idc_ip);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(298,55);to=(299,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/idc.jsp";from=(299,47);to=(299,54)]
                out.print(db_name);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(299,56);to=(300,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/idc.jsp";from=(300,47);to=(300,55)]
                out.print(username);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(300,57);to=(301,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/idc.jsp";from=(301,47);to=(301,55)]
                out.print(password);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(301,57);to=(303,16)]
                out.write("</td>\r\n                </tr>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(303,18);to=(303,19)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(303,21);to=(330,16)]
                out.write("\r\n                <tfoot>\r\n                  <tr>\r\n                    <td align=\"center\" valign=\"middle\">内容</td>\r\n                    <td><select name=\"game_id\" disabled>\r\n              <option value=\"6\">剑网2</option>\r\n              <option value=\"66\">剑网2IB</option>\r\n              <option value=\"77\">剑侠世界</option>\r\n                     <option value=\"88\">剑网3</option>\r\n                      </select></td>\r\n                    <td><input name=\"idc_name\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"idc_ip\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"db_name\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"username\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"password\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                  </tr>\r\n                </tfoot>\r\n              </table>\r\n              <input type=\"hidden\" name=\"action\">\r\n              <input type=\"hidden\" name=\"iid\">\r\n            </form></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n            <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n              <tr>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(330,18);to=(339,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/system/idc.jsp";from=(339,9);to=(340,40)]
                out.write("\r\n                <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/system/idc.jsp";from=(340,43);to=(340,46)]
                out.print(num);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(340,48);to=(340,62)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/system/idc.jsp";from=(340,65);to=(340,82)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(340,84);to=(340,85)]
                out.write("～");

            // end
            // begin [file="/system/idc.jsp";from=(340,88);to=(340,113)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(340,115);to=(341,16)]
                out.write("条</td>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(341,18);to=(344,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/system/idc.jsp";from=(344,17);to=(345,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/idc.jsp";from=(345,43);to=(345,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/idc.jsp";from=(345,71);to=(346,40)]
                out.write("/system/idc.jsp?pageAction=first&start=\">第一页</a></td>\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/idc.jsp";from=(346,43);to=(346,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/idc.jsp";from=(346,71);to=(346,113)]
                out.write("/system/idc.jsp?pageAction=previous&start=");

            // end
            // begin [file="/system/idc.jsp";from=(346,116);to=(346,139)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(346,141);to=(347,16)]
                out.write("\">上一页</a></td>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(347,18);to=(348,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/system/idc.jsp";from=(348,9);to=(351,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>第一页</u></td>\r\n                <td width=\"7%\"><u disabled>上一页</u></td>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(351,18);to=(354,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/system/idc.jsp";from=(354,16);to=(355,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/idc.jsp";from=(355,43);to=(355,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/idc.jsp";from=(355,71);to=(355,109)]
                out.write("/system/idc.jsp?pageAction=next&start=");

            // end
            // begin [file="/system/idc.jsp";from=(355,112);to=(355,127)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(355,129);to=(356,41)]
                out.write("\">下一页</a></td>\r\n                <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/system/idc.jsp";from=(356,44);to=(356,70)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/idc.jsp";from=(356,72);to=(356,110)]
                out.write("/system/idc.jsp?pageAction=last&start=");

            // end
            // begin [file="/system/idc.jsp";from=(356,113);to=(356,131)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(356,133);to=(357,16)]
                out.write("\">最后一页</a></td>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(357,18);to=(357,27)]
                 } else {
            // end
            // HTML // begin [file="/system/idc.jsp";from=(357,29);to=(361,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>下一页</u></td>\r\n                <td width=\"12%\"><u disabled>最后一页</u></td>\r\n                <td width=\"12%\"></td>\r\n                ");

            // end
            // begin [file="/system/idc.jsp";from=(361,18);to=(361,19)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(361,21);to=(362,40)]
                out.write("\r\n                <td width=\"10%\" nowrap> ");

            // end
            // begin [file="/system/idc.jsp";from=(362,42);to=(365,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/idc.jsp";from=(365,17);to=(365,18)]
                out.write(" ");

            // end
            // begin [file="/system/idc.jsp";from=(365,21);to=(365,35)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(365,37);to=(365,38)]
                out.write("/");

            // end
            // begin [file="/system/idc.jsp";from=(365,41);to=(365,54)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(365,56);to=(365,57)]
                out.write(" ");

            // end
            // begin [file="/system/idc.jsp";from=(365,59);to=(365,60)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(365,62);to=(366,40)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap> ");

            // end
            // begin [file="/system/idc.jsp";from=(366,42);to=(369,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/idc.jsp";from=(369,17);to=(371,57)]
                out.write("\r\n                  转到第\r\n                  <INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/system/idc.jsp";from=(371,60);to=(371,74)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(371,76);to=(371,135)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/system/idc.jsp";from=(371,138);to=(371,162)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(371,164);to=(371,179)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/idc.jsp";from=(371,182);to=(371,187)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(371,189);to=(373,62)]
                out.write(")\">\r\n                  页\r\n                  <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/system/idc.jsp";from=(373,65);to=(373,89)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(373,91);to=(373,106)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/idc.jsp";from=(373,109);to=(373,114)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(373,116);to=(375,18)]
                out.write(")\">\r\n                  GO </button>\r\n                  ");

            // end
            // begin [file="/system/idc.jsp";from=(375,20);to=(375,21)]
                }
            // end
            // HTML // begin [file="/system/idc.jsp";from=(375,23);to=(377,57)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap>页长设置为\r\n                  <INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/system/idc.jsp";from=(377,60);to=(377,65)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(377,67);to=(377,128)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/system/idc.jsp";from=(377,131);to=(377,155)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(377,157);to=(377,172)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/idc.jsp";from=(377,175);to=(377,192)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(377,194);to=(378,61)]
                out.write(")\">\r\n                  <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/system/idc.jsp";from=(378,64);to=(378,88)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/idc.jsp";from=(378,90);to=(378,105)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/idc.jsp";from=(378,108);to=(378,125)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/idc.jsp";from=(378,127);to=(407,0)]
                out.write(")\">确定</button></td>\r\n              </tr>\r\n            </table>\r\n\r\n\t\t  </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n            <table width=\"328\" height=\"15\" border=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td align=\"center\"> <button name=\"btnAdd\" onClick=\"onAdd()\"   >增加</button></td>\r\n                <td align=\"center\"> <button name=\"btnModify\" onClick=\"onModify()\"   disabled>修改</button></td>\r\n                <td align=\"center\"> <button name=\"btnDel\" onClick=\"onDel()\"   disabled>删除</button></td>\r\n                <td align=\"center\"> <button name=\"btnSave\" onClick=\"onSave()\"   disabled>保存</button></td>\r\n                <td align=\"center\"> <button name=\"btnCancel\" onClick=\"onCancel()\"   disabled>取消</button></td>\r\n                <td align=\"center\" style=\"display:none\"> <button name=\"btnMatch\" onClick=\"onMatch()\"  >再次匹配</button></td>\r\n              </tr>\r\n            </table></td>\r\n        </tr>\r\n      </table>\r\n\r\n\t<!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

            // end

        } catch (Throwable t) {
            if (out != null && out.getBufferSize() != 0)
                out.clearBuffer();
            if (pageContext != null) pageContext.handlePageException(t);
        } finally {
            if (_jspxFactory != null) _jspxFactory.releasePageContext(pageContext);
        }
    }
}
