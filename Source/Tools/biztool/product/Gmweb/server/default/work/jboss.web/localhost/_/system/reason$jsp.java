package org.apache.jsp;

import java.sql.*;
import java.util.*;
import  com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class reason$jsp extends HttpJspBase {

    // begin [file="/system/reason.jsp";from=(3,0);to=(3,102)]
    // end
    // begin [file="/system/reason.jsp";from=(4,0);to=(4,107)]
    // end

    static {
    }
    public reason$jsp( ) {
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

            // HTML // begin [file="/system/reason.jsp";from=(0,162);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/reason.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/reason.jsp";from=(2,0);to=(2,40)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("reason");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/reason.jsp";from=(2,0);to=(2,40)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(2,40);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/reason.jsp";from=(3,0);to=(3,102)]
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
            // begin [file="/system/reason.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/reason.jsp";from=(4,0);to=(4,107)]
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
            // begin [file="/system/reason.jsp";from=(4,0);to=(4,107)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(4,107);to=(13,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>操作原因维护</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/reason.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(13,42);to=(17,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/system/reason.jsp";from=(17,2);to=(22,0)]
                
                    //this prevents caching in NS and IE
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(22,2);to=(247,101)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar objChk=null;//当前被选中的\r\nvar action=null;\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/system/reason.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/system/reason.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"reason\").value=objCurChk.reason;\r\n\t\tdocument.all(\"chkforbid\").checked=(objCurChk.forbid!=0)?true:false;\r\n\t\tdocument.all(\"chkkick\").checked=(objCurChk.kick!=0)?true:false;\r\n\t\tdocument.all(\"chkkickaccount\").checked=(objCurChk.kickaccount!=0)?true:false;\r\n\t\tdocument.all(\"chkrelease\").checked=(objCurChk.release!=0)?true:false;\r\n\t\tdocument.all(\"chkpermit\").checked=(objCurChk.permit!=0)?true:false;\r\n\t\tdocument.all(\"chkarrent\").checked=(objCurChk.arrent!=0)?true:false;\r\n\t\tdocument.all(\"chkfreeze\").checked=(objCurChk.freeze!=0)?true:false;\r\n\t\tdocument.all(\"chkunfreeze\").checked=(objCurChk.unfreeze!=0)?true:false;\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"btnDel\").disabled=false;\r\n\t}\r\n}\r\n\r\n/******************************\r\ninflag: true，可编辑   false,不可编辑状态\r\n*******************************/\r\nfunction SetEditModel(inflag)\r\n{\r\n\tSetChkState(inflag);\r\n\tdocument.all(\"reason\").disabled=!inflag;\r\n\tdocument.all(\"chkforbid\").disabled=!inflag;\r\n\tdocument.all(\"chkkick\").disabled=!inflag;\r\n\tdocument.all(\"chkkickaccount\").disabled=!inflag;\r\n\tdocument.all(\"chkrelease\").disabled=!inflag;\r\n\tdocument.all(\"chkpermit\").disabled=!inflag;\r\n\tdocument.all(\"chkarrent\").disabled=!inflag;\r\n\tdocument.all(\"chkfreeze\").disabled=!inflag;\r\n\tdocument.all(\"chkunfreeze\").disabled=!inflag;\r\n\tif(action==\"addreason\")\r\n\t{\r\n\t\tdocument.all(\"reason\").value=\"\";\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t\tdocument.all(\"chkforbid\").checked=false;\r\n\t\tdocument.all(\"chkkick\").checked=false;\r\n\t\tdocument.all(\"chkkickaccount\").checked=false;\r\n\t\tdocument.all(\"chkrelease\").checked=false;\r\n\t\tdocument.all(\"chkpermit\").checked=false;\r\n\t\tdocument.all(\"chkarrent\").checked=false;\r\n\t\tdocument.all(\"chkfreeze\").checked=false;\r\n\t\tdocument.all(\"chkunfreeze\").checked=false;\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=inflag;\r\n\tdocument.all(\"btnModify\").disabled=inflag;\r\n\tdocument.all(\"btnDel\").disabled=inflag;\r\n\tdocument.all(\"btnSave\").disabled=!inflag;\r\n\tdocument.all(\"btnCancel\").disabled=!inflag;\r\n\tif(objChk){\r\n\t\tobjChk.checked=true;\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n增加\r\n******************************/\r\nfunction onAdd()\r\n{\r\n\tif(objChk){\r\n\t\tobjChk.checked=false;\r\n\t}\r\n\taction=\"addreason\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"reason\").focus();\r\n}\r\n\r\n/******************************\r\n修改\r\n*****************************/\r\nfunction onModify()\r\n{\r\n\taction=\"modifyreason\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"reason\").focus();\r\n}\r\n\r\n/****************************\r\n删除\r\n****************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此操作原因吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delreason\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n保存\r\n*****************************/\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"reason\")) strErr+=\"*操作原因不能为空！\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"forbid\").value=document.all(\"chkforbid\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"kick\").value=document.all(\"chkkick\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"kickaccount\").value=document.all(\"chkkickaccount\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"release\").value=document.all(\"chkrelease\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"permit\").value=document.all(\"chkpermit\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"arrent\").value=document.all(\"chkarrent\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"freeze\").value=document.all(\"chkfreeze\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"unfreeze\").value=document.all(\"chkunfreeze\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/******************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetEditModel(false);\r\n\taction=\"\";\r\n\tif(document.all(\"action\").value==\"addreason\")\r\n\t{\r\n\t\tdocument.all(\"reason\").value=\"\";\r\n\t\tdocument.all(\"chkforbid\").checked=false;\r\n\t\tdocument.all(\"chkkick\").checked=false;\r\n\t\tdocument.all(\"chkkickaccount\").checked=false;\r\n\t\tdocument.all(\"chkrelease\").checked=false;\r\n\t\tdocument.all(\"chkpermit\").checked=false;\r\n\t\tdocument.all(\"chkarrent\").checked=false;\r\n\t\tdocument.all(\"chkfreeze\").checked=false;\r\n\t\tdocument.all(\"chkunfreeze\").checked=false;\r\n\t}\r\n\telse if(objChk)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objChk.iid;\r\n\t\tdocument.all(\"reason\").value=objChk.reason;\r\n\t\tdocument.all(\"chkforbid\").checked=(objChk.forbid!=0)?true:false;\r\n\t\tdocument.all(\"chkkick\").checked=(objChk.kick!=0)?true:false;\r\n\t\tdocument.all(\"chkkickaccount\").checked=(objChk.kickaccount!=0)?true:false;\r\n\t\tdocument.all(\"chkrelease\").checked=(objChk.release!=0)?true:false;\r\n\t\tdocument.all(\"chkpermit\").checked=(objChk.permit!=0)?true:false;\r\n\t\tdocument.all(\"chkarrent\").checked=(objChk.arrent!=0)?true:false;\r\n\t\tdocument.all(\"chkfreeze\").checked=(objChk.freeze!=0)?true:false;\r\n\t\tdocument.all(\"chkunfreeze\").checked=(objChk.unfreeze!=0)?true:false;\r\n\t}\r\n}\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">操作原因维护</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <br>\r\n      <table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td height=\"200\" align=\"center\" valign=\"top\"> <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/system/reason.jsp";from=(247,104);to=(247,128)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(247,130);to=(256,16)]
                out.write("/SystemServlet\">\r\n              <table width=\"100%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n                <thead>\r\n                  <tr>\r\n                    <td width=\"30\"  align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                    <td  align=\"center\"><font color=\"#FFFFFF\">操作原因</font></td>\r\n                    <td colspan=\"8\"  align=\"center\"><font color=\"#FFFFFF\">适应范围</font></td>\r\n                  </tr>\r\n                </thead>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(256,18);to=(310,0)]
                
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
                   ArrayList ltcom=new ArrayList();
                   int[] curPageInfo = new int[8];
                   int num=0;
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                
                   if (hs1.containsKey("pageAction")) {
                       String page1 = (String)hs1.get("pageAction");
                       if (page1.trim().equals("servlet")) {
                           se.setAttribute("reason", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("reason");
                       }
                       num = SystemHelperId.getReasonListNum();
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       ltcom = SystemHelperId.getReasonListByPage(curPageInfo[0], curPageInfo[2]);
                   }
                //*********End***************************************************
                	String iid ="";
                	String reason="";
                	String forbid="";
                	String kick="";
                	String kickaccount="";
                	String release="";
                	String arrent="";
                	String permit="";
                	String freeze="";
                	String unfreeze="";
                for (int i=0; i<ltcom.size(); i++)
                {
                	HashMap hscom = (HashMap)ltcom.get(i);
                	iid = (String)hscom.get("iid");
                	reason= (String)hscom.get("reason");
                	forbid=(String)hscom.get("forbid");
                	kick=(String)hscom.get("kick");
                	kickaccount=(String)hscom.get("kickaccount");
                	release=(String)hscom.get("release");
                	arrent=(String)hscom.get("arrent");
                	permit=(String)hscom.get("permit");
                	freeze=(String)hscom.get("freeze");
                	unfreeze=(String)hscom.get("unfreeze");
            // end
            // HTML // begin [file="/system/reason.jsp";from=(310,2);to=(311,21)]
                out.write("\r\n                <tr  ");

            // end
            // begin [file="/system/reason.jsp";from=(311,23);to=(311,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(311,36);to=(311,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/reason.jsp";from=(311,56);to=(311,57)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(311,59);to=(312,85)]
                out.write(">\r\n                  <td align=\"center\" valign=\"middle\"><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/system/reason.jsp";from=(312,88);to=(312,89)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,91);to=(312,101)]
                out.write("\" reason=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,104);to=(312,110)]
                out.print(reason);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,112);to=(312,122)]
                out.write("\" freeze=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,125);to=(312,131)]
                out.print(freeze);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,133);to=(312,145)]
                out.write("\" unfreeze=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,148);to=(312,156)]
                out.print(unfreeze);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,158);to=(312,165)]
                out.write("\" iid=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,168);to=(312,171)]
                out.print(iid);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,173);to=(312,183)]
                out.write("\" forbid=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,186);to=(312,192)]
                out.print(forbid);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,194);to=(312,223)]
                out.write("\" onClick=\"onSelect()\" kick=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,226);to=(312,230)]
                out.print(kick);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,232);to=(312,247)]
                out.write("\" kickaccount=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,250);to=(312,261)]
                out.print(kickaccount);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,263);to=(312,274)]
                out.write("\" release=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,277);to=(312,284)]
                out.print(release);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,286);to=(312,296)]
                out.write("\" arrent=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,299);to=(312,305)]
                out.print(arrent);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,307);to=(312,317)]
                out.write("\" permit=\"");

            // end
            // begin [file="/system/reason.jsp";from=(312,320);to=(312,326)]
                out.print(permit);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,328);to=(312,330)]
                out.write("\">");

            // end
            // begin [file="/system/reason.jsp";from=(312,333);to=(312,336)]
                out.print(i+1);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(312,338);to=(314,22)]
                out.write("\r\n                  </td>\r\n                  <td>");

            // end
            // begin [file="/system/reason.jsp";from=(314,25);to=(314,31)]
                out.print(reason);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(314,33);to=(316,39)]
                out.write("</td>\r\n                  <td nowrap>\r\n<input type=\"checkbox\" name=\"chkforbid_");

            // end
            // begin [file="/system/reason.jsp";from=(316,42);to=(316,43)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(316,45);to=(316,47)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(316,49);to=(316,72)]
                if(forbid.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(316,74);to=(316,83)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(316,85);to=(316,86)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(316,88);to=(317,68)]
                out.write(" >禁言</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkpermit_");

            // end
            // begin [file="/system/reason.jsp";from=(317,71);to=(317,72)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(317,74);to=(317,76)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(317,78);to=(317,101)]
                if(permit.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(317,103);to=(317,112)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(317,114);to=(317,115)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(317,117);to=(319,66)]
                out.write(" >\r\n                  解除禁言</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkkick_");

            // end
            // begin [file="/system/reason.jsp";from=(319,69);to=(319,70)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(319,72);to=(319,74)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(319,76);to=(319,97)]
                if(kick.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(319,99);to=(319,108)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(319,110);to=(319,111)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(319,113);to=(321,68)]
                out.write(" >\r\n                  踢人</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkarrent_");

            // end
            // begin [file="/system/reason.jsp";from=(321,71);to=(321,72)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(321,74);to=(321,76)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(321,78);to=(321,101)]
                if(arrent.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(321,103);to=(321,112)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(321,114);to=(321,115)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(321,117);to=(323,69)]
                out.write(" >\r\n关天牢</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkrelease_");

            // end
            // begin [file="/system/reason.jsp";from=(323,72);to=(323,73)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(323,75);to=(323,77)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(323,79);to=(323,103)]
                if(release.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(323,105);to=(323,114)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(323,116);to=(323,117)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(323,119);to=(325,68)]
                out.write(" >\r\n解封天牢</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkfreeze_");

            // end
            // begin [file="/system/reason.jsp";from=(325,71);to=(325,72)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(325,74);to=(325,76)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(325,78);to=(325,101)]
                if(freeze.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(325,103);to=(325,112)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(325,114);to=(325,115)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(325,117);to=(327,70)]
                out.write(">\r\n冻结帐号</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkunfreeze_");

            // end
            // begin [file="/system/reason.jsp";from=(327,73);to=(327,74)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(327,76);to=(327,78)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(327,80);to=(327,105)]
                if(unfreeze.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(327,107);to=(327,116)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(327,118);to=(327,119)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(327,121);to=(330,64)]
                out.write(">\r\n解冻帐号</td>\r\n                  <td nowrap>\r\n                    <input type=\"checkbox\" name=\"chkkickaccount_");

            // end
            // begin [file="/system/reason.jsp";from=(330,67);to=(330,68)]
                out.print(i);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(330,70);to=(330,72)]
                out.write("\" ");

            // end
            // begin [file="/system/reason.jsp";from=(330,74);to=(330,102)]
                if(kickaccount.equals("1")){
            // end
            // HTML // begin [file="/system/reason.jsp";from=(330,104);to=(330,113)]
                out.write(" checked ");

            // end
            // begin [file="/system/reason.jsp";from=(330,115);to=(330,116)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(330,118);to=(333,16)]
                out.write(" >\r\n  踢号</td>\r\n                </tr>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(333,18);to=(333,19)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(333,21);to=(374,16)]
                out.write("\r\n                <tfoot>\r\n                  <tr>\r\n                    <td align=\"center\" valign=\"middle\">内容</td>\r\n                    <td><input name=\"reason\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input type=\"checkbox\" name=\"chkforbid\" disabled>禁言</td>\r\n                    <td><input type=\"checkbox\" name=\"chkpermit\" disabled>\r\n\t\t\t\t\t解除禁言</td>\r\n                    <td><input type=\"checkbox\" name=\"chkkick\" disabled>\r\n                    踢人</td>\r\n                    <td><input type=\"checkbox\" name=\"chkarrent\" disabled>\r\n关天牢</td>\r\n                    <td><input type=\"checkbox\" name=\"chkrelease\" disabled>\r\n                    解封天牢</td>\r\n                    <td><input type=\"checkbox\" name=\"chkfreeze\" disabled>\r\n冻结帐号</td>\r\n                    <td><input type=\"checkbox\" name=\"chkunfreeze\" disabled>\r\n解冻帐号</td>\r\n                    <td>\r\n                      <input type=\"checkbox\" name=\"chkkickaccount\" disabled>\r\n  踢号</td>\r\n                  </tr>\r\n                </tfoot>\r\n              </table>\r\n              <input type=\"hidden\" name=\"action\">\r\n              <input type=\"hidden\" name=\"iid\">\r\n              <input type=\"hidden\" name=\"forbid\">\r\n              <input type=\"hidden\" name=\"kick\">\r\n              <input type=\"hidden\" name=\"kickaccount\">\r\n              <input type=\"hidden\" name=\"release\">\r\n              <input type=\"hidden\" name=\"permit\">\r\n              <input type=\"hidden\" name=\"arrent\">\r\n              <input type=\"hidden\" name=\"freeze\">\r\n              <input type=\"hidden\" name=\"unfreeze\">\r\n            </form></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n            <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n              <tr>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(374,18);to=(383,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/system/reason.jsp";from=(383,9);to=(384,40)]
                out.write("\r\n                <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/system/reason.jsp";from=(384,43);to=(384,46)]
                out.print(num);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(384,48);to=(384,62)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/system/reason.jsp";from=(384,65);to=(384,82)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(384,84);to=(384,85)]
                out.write("～");

            // end
            // begin [file="/system/reason.jsp";from=(384,88);to=(384,113)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(384,115);to=(385,16)]
                out.write("条</td>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(385,18);to=(388,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/system/reason.jsp";from=(388,17);to=(389,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/reason.jsp";from=(389,43);to=(389,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/reason.jsp";from=(389,71);to=(390,40)]
                out.write("/system/reason.jsp?pageAction=first&start=\">第一页</a></td>\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/reason.jsp";from=(390,43);to=(390,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/reason.jsp";from=(390,71);to=(390,116)]
                out.write("/system/reason.jsp?pageAction=previous&start=");

            // end
            // begin [file="/system/reason.jsp";from=(390,119);to=(390,142)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(390,144);to=(391,16)]
                out.write("\">上一页</a></td>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(391,18);to=(392,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/system/reason.jsp";from=(392,9);to=(395,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>第一页</u></td>\r\n                <td width=\"7%\"><u disabled>上一页</u></td>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(395,18);to=(398,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/system/reason.jsp";from=(398,16);to=(399,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/reason.jsp";from=(399,43);to=(399,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/reason.jsp";from=(399,71);to=(399,112)]
                out.write("/system/reason.jsp?pageAction=next&start=");

            // end
            // begin [file="/system/reason.jsp";from=(399,115);to=(399,130)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(399,132);to=(400,41)]
                out.write("\">下一页</a></td>\r\n                <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/system/reason.jsp";from=(400,44);to=(400,70)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/reason.jsp";from=(400,72);to=(400,113)]
                out.write("/system/reason.jsp?pageAction=last&start=");

            // end
            // begin [file="/system/reason.jsp";from=(400,116);to=(400,134)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(400,136);to=(401,16)]
                out.write("\">最后一页</a></td>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(401,18);to=(401,27)]
                 } else {
            // end
            // HTML // begin [file="/system/reason.jsp";from=(401,29);to=(405,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>下一页</u></td>\r\n                <td width=\"12%\"><u disabled>最后一页</u></td>\r\n                <td width=\"12%\">excel格式</td>\r\n                ");

            // end
            // begin [file="/system/reason.jsp";from=(405,18);to=(405,19)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(405,21);to=(406,40)]
                out.write("\r\n                <td width=\"10%\" nowrap> ");

            // end
            // begin [file="/system/reason.jsp";from=(406,42);to=(409,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/reason.jsp";from=(409,17);to=(409,18)]
                out.write(" ");

            // end
            // begin [file="/system/reason.jsp";from=(409,21);to=(409,35)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(409,37);to=(409,38)]
                out.write("/");

            // end
            // begin [file="/system/reason.jsp";from=(409,41);to=(409,54)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(409,56);to=(409,57)]
                out.write(" ");

            // end
            // begin [file="/system/reason.jsp";from=(409,59);to=(409,60)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(409,62);to=(410,40)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap> ");

            // end
            // begin [file="/system/reason.jsp";from=(410,42);to=(413,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/reason.jsp";from=(413,17);to=(415,57)]
                out.write("\r\n                  转到第\r\n                  <INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/system/reason.jsp";from=(415,60);to=(415,74)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(415,76);to=(415,135)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/system/reason.jsp";from=(415,138);to=(415,162)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(415,164);to=(415,179)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/reason.jsp";from=(415,182);to=(415,187)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(415,189);to=(417,62)]
                out.write(")\">\r\n                  页\r\n                  <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/system/reason.jsp";from=(417,65);to=(417,89)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(417,91);to=(417,106)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/reason.jsp";from=(417,109);to=(417,114)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(417,116);to=(419,18)]
                out.write(")\">\r\n                  GO </button>\r\n                  ");

            // end
            // begin [file="/system/reason.jsp";from=(419,20);to=(419,21)]
                }
            // end
            // HTML // begin [file="/system/reason.jsp";from=(419,23);to=(421,57)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap>页长设置为\r\n                  <INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/system/reason.jsp";from=(421,60);to=(421,65)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(421,67);to=(421,128)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/system/reason.jsp";from=(421,131);to=(421,155)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(421,157);to=(421,172)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/reason.jsp";from=(421,175);to=(421,192)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(421,194);to=(422,61)]
                out.write(")\">\r\n                  <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/system/reason.jsp";from=(422,64);to=(422,88)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/reason.jsp";from=(422,90);to=(422,105)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/reason.jsp";from=(422,108);to=(422,125)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/reason.jsp";from=(422,127);to=(450,0)]
                out.write(")\">确定</button></td>\r\n              </tr>\r\n            </table>\r\n\r\n\t\t  </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n            <table width=\"328\" height=\"15\" border=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td align=\"center\"> <button name=\"btnAdd\" onClick=\"onAdd()\"   >增加</button></td>\r\n                <td align=\"center\"> <button name=\"btnModify\" onClick=\"onModify()\"   disabled>修改</button></td>\r\n                <td align=\"center\"> <button name=\"btnDel\" onClick=\"onDel()\"   disabled>删除</button></td>\r\n                <td align=\"center\"> <button name=\"btnSave\" onClick=\"onSave()\"   disabled>保存</button></td>\r\n                <td align=\"center\"> <button name=\"btnCancel\" onClick=\"onCancel()\"   disabled>取消</button></td>\r\n              </tr>\r\n            </table></td>\r\n        </tr>\r\n      </table>\r\n\r\n\t<!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
