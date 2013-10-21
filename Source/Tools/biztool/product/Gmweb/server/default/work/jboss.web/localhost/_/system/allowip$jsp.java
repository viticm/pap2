package org.apache.jsp;

import java.sql.*;
import java.util.*;
import  com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class allowip$jsp extends HttpJspBase {

    // begin [file="/system/allowip.jsp";from=(3,0);to=(3,102)]
    // end
    // begin [file="/system/allowip.jsp";from=(4,0);to=(4,107)]
    // end

    static {
    }
    public allowip$jsp( ) {
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

            // HTML // begin [file="/system/allowip.jsp";from=(0,162);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/allowip.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/allowip.jsp";from=(2,0);to=(2,41)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("allowip");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/allowip.jsp";from=(2,0);to=(2,41)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(2,41);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/allowip.jsp";from=(3,0);to=(3,102)]
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
            // begin [file="/system/allowip.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/allowip.jsp";from=(4,0);to=(4,107)]
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
            // begin [file="/system/allowip.jsp";from=(4,0);to=(4,107)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(4,107);to=(13,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>可登录IP维护</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(13,42);to=(17,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/system/allowip.jsp";from=(17,2);to=(22,0)]
                
                    //this prevents caching in NS and IE
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(22,2);to=(205,101)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar objChk=null;//当前被选中的\r\nvar action=null;\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/system/allowip.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/system/allowip.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"ip\").value=objCurChk.ip;\r\n\t\tdocument.all(\"remark\").value=objCurChk.remark;\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"btnDel\").disabled=false;\r\n\t}\r\n}\r\n\r\n/******************************\r\ninflag: true，可编辑   false,不可编辑状态\r\n*******************************/\r\nfunction SetEditModel(inflag)\r\n{\r\n\tSetChkState(inflag);\r\n\tdocument.all(\"ip\").disabled=!inflag;\r\n\tdocument.all(\"remark\").disabled=!inflag;\r\n\tif(action==\"addallowip\")\r\n\t{\r\n\t\tdocument.all(\"ip\").value=\"\";\r\n\t\tdocument.all(\"remark\").value=\"\";\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=inflag;\r\n\tdocument.all(\"btnModify\").disabled=inflag;\r\n\tdocument.all(\"btnDel\").disabled=inflag;\r\n\tdocument.all(\"btnSave\").disabled=!inflag;\r\n\tdocument.all(\"btnCancel\").disabled=!inflag;\r\n\tif(objChk){\r\n\t\tobjChk.checked=true;\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n增加\r\n******************************/\r\nfunction onAdd()\r\n{\r\n\tif(objChk){\r\n\t\tobjChk.checked=false;\r\n\t}\r\n\taction=\"addallowip\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"ip\").focus();\r\n}\r\n\r\n/******************************\r\n修改\r\n*****************************/\r\nfunction onModify()\r\n{\r\n\taction=\"modifyallowip\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"ip\").focus();\r\n}\r\n\r\n/****************************\r\n删除\r\n****************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此可登录IP吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delallowip\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n保存\r\n*****************************/\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"ip\")) strErr+=\"*IP不能为空！\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/******************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetEditModel(false);\r\n\taction=\"\";\r\n\tif(document.all(\"action\").value==\"addallowip\")\r\n\t{\r\n\t\tdocument.all(\"remark\").value=\"\";\r\n\t\tdocument.all(\"ip\").value=\"\";\r\n\t}\r\n\telse if(objChk)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objChk.iid;\r\n\t\tdocument.all(\"remark\").value=objChk.remark;\r\n\t\tdocument.all(\"ip\").value=objChk.ip;\r\n\t}\r\n}\r\n\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">可登录IP维护</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n    <br>\r\n      <table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td height=\"200\" align=\"center\" valign=\"top\"> <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(205,104);to=(205,128)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(205,130);to=(214,16)]
                out.write("/SystemServlet\">\r\n              <table width=\"80%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n                <thead>\r\n                  <tr>\r\n                    <td  align=\"center\" nowrap width=\"30\"><font color=\"#FFFFFF\">序号</font></td>\r\n                    <td  align=\"center\"><font color=\"#FFFFFF\">IP</font></td>\r\n                    <td  align=\"center\"><font color=\"#FFFFFF\">备注</font></td>\r\n                  </tr>\r\n                </thead>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(214,18);to=(254,0)]
                
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
                           se.setAttribute("allowip", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("allowip");
                       }
                       num = SystemHelperId.getAllowIPListNum();
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       lt = SystemHelperId.getAllowIPListByPage(curPageInfo[0], curPageInfo[2]);
                   }
                //*********End***************************************************
                	String iid ="";
                	String remark = "";
                	String ip="";
                for (int i=0; i<lt.size(); i++)
                {
                	HashMap hs = (HashMap)lt.get(i);
                	iid = (String)hs.get("iid");
                	remark = (String)hs.get("remark");
                	ip= (String)hs.get("ip");
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(254,2);to=(255,21)]
                out.write("\r\n                <tr  ");

            // end
            // begin [file="/system/allowip.jsp";from=(255,23);to=(255,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(255,36);to=(255,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/allowip.jsp";from=(255,56);to=(255,57)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(255,59);to=(256,92)]
                out.write(">\r\n                  <td align=\"center\" valign=\"middle\" nowrap><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/system/allowip.jsp";from=(256,95);to=(256,96)]
                out.print(i);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(256,98);to=(256,104)]
                out.write("\" ip=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(256,107);to=(256,109)]
                out.print(ip);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(256,111);to=(256,122)]
                out.write("\"  remark=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(256,125);to=(256,131)]
                out.print(remark);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(256,133);to=(256,140)]
                out.write("\" iid=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(256,143);to=(256,146)]
                out.print(iid);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(256,148);to=(256,171)]
                out.write("\" onClick=\"onSelect()\">");

            // end
            // begin [file="/system/allowip.jsp";from=(256,174);to=(256,177)]
                out.print(i+1);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(256,179);to=(258,37)]
                out.write("\r\n                  </td>\r\n                  <td align=\"center\">");

            // end
            // begin [file="/system/allowip.jsp";from=(258,40);to=(258,42)]
                out.print(ip);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(258,44);to=(259,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/allowip.jsp";from=(259,47);to=(259,53)]
                out.print(remark);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(259,55);to=(261,16)]
                out.write("</td>\r\n                </tr>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(261,18);to=(261,19)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(261,21);to=(279,16)]
                out.write("\r\n                <tfoot>\r\n                  <tr>\r\n                    <td align=\"center\" valign=\"middle\">内容</td>\r\n                    <td><input name=\"ip\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"remark\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                  </tr>\r\n                </tfoot>\r\n              </table>\r\n              <input type=\"hidden\" name=\"action\">\r\n              <input type=\"hidden\" name=\"iid\">\r\n            </form></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n            <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n              <tr>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(279,18);to=(288,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(288,9);to=(289,40)]
                out.write("\r\n                <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/system/allowip.jsp";from=(289,43);to=(289,46)]
                out.print(num);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(289,48);to=(289,62)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/system/allowip.jsp";from=(289,65);to=(289,82)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(289,84);to=(289,85)]
                out.write("～");

            // end
            // begin [file="/system/allowip.jsp";from=(289,88);to=(289,113)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(289,115);to=(290,16)]
                out.write("条</td>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(290,18);to=(293,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(293,17);to=(294,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(294,43);to=(294,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(294,71);to=(295,40)]
                out.write("/system/allowip.jsp?pageAction=first&start=\">第一页</a></td>\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(295,43);to=(295,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(295,71);to=(295,117)]
                out.write("/system/allowip.jsp?pageAction=previous&start=");

            // end
            // begin [file="/system/allowip.jsp";from=(295,120);to=(295,143)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(295,145);to=(296,16)]
                out.write("\">上一页</a></td>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(296,18);to=(297,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(297,9);to=(300,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>第一页</u></td>\r\n                <td width=\"7%\"><u disabled>上一页</u></td>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(300,18);to=(303,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(303,16);to=(304,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(304,43);to=(304,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(304,71);to=(304,113)]
                out.write("/system/allowip.jsp?pageAction=next&start=");

            // end
            // begin [file="/system/allowip.jsp";from=(304,116);to=(304,131)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(304,133);to=(305,41)]
                out.write("\">下一页</a></td>\r\n                <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(305,44);to=(305,70)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(305,72);to=(305,114)]
                out.write("/system/allowip.jsp?pageAction=last&start=");

            // end
            // begin [file="/system/allowip.jsp";from=(305,117);to=(305,135)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(305,137);to=(306,16)]
                out.write("\">最后一页</a></td>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(306,18);to=(306,27)]
                 } else {
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(306,29);to=(310,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>下一页</u></td>\r\n                <td width=\"12%\"><u disabled>最后一页</u></td>\r\n                <td width=\"12%\">excel格式</td>\r\n                ");

            // end
            // begin [file="/system/allowip.jsp";from=(310,18);to=(310,19)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(310,21);to=(311,40)]
                out.write("\r\n                <td width=\"10%\" nowrap> ");

            // end
            // begin [file="/system/allowip.jsp";from=(311,42);to=(314,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(314,17);to=(314,18)]
                out.write(" ");

            // end
            // begin [file="/system/allowip.jsp";from=(314,21);to=(314,35)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(314,37);to=(314,38)]
                out.write("/");

            // end
            // begin [file="/system/allowip.jsp";from=(314,41);to=(314,54)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(314,56);to=(314,57)]
                out.write(" ");

            // end
            // begin [file="/system/allowip.jsp";from=(314,59);to=(314,60)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(314,62);to=(315,40)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap> ");

            // end
            // begin [file="/system/allowip.jsp";from=(315,42);to=(318,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(318,17);to=(320,57)]
                out.write("\r\n                  转到第\r\n                  <INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(320,60);to=(320,74)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(320,76);to=(320,135)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/system/allowip.jsp";from=(320,138);to=(320,162)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(320,164);to=(320,179)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/allowip.jsp";from=(320,182);to=(320,187)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(320,189);to=(322,62)]
                out.write(")\">\r\n                  页\r\n                  <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/system/allowip.jsp";from=(322,65);to=(322,89)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(322,91);to=(322,106)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/allowip.jsp";from=(322,109);to=(322,114)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(322,116);to=(324,18)]
                out.write(")\">\r\n                  GO </button>\r\n                  ");

            // end
            // begin [file="/system/allowip.jsp";from=(324,20);to=(324,21)]
                }
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(324,23);to=(326,57)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap>页长设置为\r\n                  <INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/system/allowip.jsp";from=(326,60);to=(326,65)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(326,67);to=(326,128)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/system/allowip.jsp";from=(326,131);to=(326,155)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(326,157);to=(326,172)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/allowip.jsp";from=(326,175);to=(326,192)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(326,194);to=(327,61)]
                out.write(")\">\r\n                  <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/system/allowip.jsp";from=(327,64);to=(327,88)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(327,90);to=(327,105)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/allowip.jsp";from=(327,108);to=(327,125)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/allowip.jsp";from=(327,127);to=(356,0)]
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
