package org.apache.jsp;

import java.sql.*;
import java.util.*;
import  com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class commonquestion$jsp extends HttpJspBase {

    // begin [file="/system/commonquestion.jsp";from=(3,0);to=(3,102)]
    // end
    // begin [file="/system/commonquestion.jsp";from=(4,0);to=(4,107)]
    // end

    static {
    }
    public commonquestion$jsp( ) {
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

            // HTML // begin [file="/system/commonquestion.jsp";from=(0,162);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/commonquestion.jsp";from=(2,0);to=(2,48)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("commonquestion");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/commonquestion.jsp";from=(2,0);to=(2,48)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(2,48);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/commonquestion.jsp";from=(3,0);to=(3,102)]
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
            // begin [file="/system/commonquestion.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/commonquestion.jsp";from=(4,0);to=(4,107)]
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
            // begin [file="/system/commonquestion.jsp";from=(4,0);to=(4,107)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(4,107);to=(13,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>常见问题维护</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(13,42);to=(17,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/system/commonquestion.jsp";from=(17,2);to=(22,0)]
                
                    //this prevents caching in NS and IE
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(22,2);to=(265,101)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar objChk=null;//当前被选中的\r\nvar action=null;\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/system/commonquestion.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/system/commonquestion.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"answer\").value=objCurChk.answer;\r\n\t\tdocument.all(\"question\").value=objCurChk.question;\r\n\t\tdocument.all(\"keyword\").value=objCurChk.keyword;\r\n\t\tdocument.all(\"chkjx\").checked=(objCurChk.jx!=0)?true:false;\r\n\t\tdocument.all(\"chkjxvnet\").checked=(objCurChk.jxvnet!=0)?true:false;\r\n\t\tdocument.all(\"chkfs\").checked=(objCurChk.fs!=0)?true:false;\r\n\t\tdocument.all(\"chkjx2\").checked=(objCurChk.jx2!=0)?true:false;\r\n\t\tdocument.all(\"chkcq\").checked=(objCurChk.cq!=0)?true:false;\r\n\t\tdocument.all(\"chkquickanswer\").checked=(objCurChk.quickanswer!=0)?true:false;\r\n\t\tdocument.all(\"chkquickdeal\").checked=(objCurChk.quickdeal!=0)?true:false;\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"btnDel\").disabled=false;\r\n\t}\r\n}\r\n\r\n/******************************\r\ninflag: true，可编辑   false,不可编辑状态\r\n*******************************/\r\nfunction SetEditModel(inflag)\r\n{\r\n\tSetChkState(inflag);\r\n\tdocument.all(\"question\").disabled=!inflag;\r\n\tdocument.all(\"answer\").disabled=!inflag;\r\n\tdocument.all(\"keyword\").disabled=!inflag;\r\n\tdocument.all(\"chkjx\").disabled=!inflag;\r\n\tdocument.all(\"chkjxvnet\").disabled=!inflag;\r\n\tdocument.all(\"chkfs\").disabled=!inflag;\r\n\tdocument.all(\"chkjx2\").disabled=!inflag;\r\n\tdocument.all(\"chkcq\").disabled=!inflag;\r\n\tdocument.all(\"chkquickanswer\").disabled=!inflag;\r\n\tdocument.all(\"chkquickdeal\").disabled=!inflag;\r\n\tif(action==\"addcommonquestion\")\r\n\t{\r\n\t\tdocument.all(\"keyword\").value=\"\";\r\n\t\tdocument.all(\"question\").value=\"\";\r\n\t\tdocument.all(\"answer\").value=\"\";\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t\tdocument.all(\"chkjx\").checked=false;\r\n\t\tdocument.all(\"chkjxvnet\").checked=false;\r\n\t\tdocument.all(\"chkfs\").checked=false;\r\n\t\tdocument.all(\"chkjx2\").checked=false;\r\n\t\tdocument.all(\"chkcq\").checked=false;\r\n\t\tdocument.all(\"chkquickanswer\").checked=false;\r\n\t\tdocument.all(\"chkquickdeal\").checked=false;\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=inflag;\r\n\tdocument.all(\"btnModify\").disabled=inflag;\r\n\tdocument.all(\"btnDel\").disabled=inflag;\r\n\tdocument.all(\"btnSave\").disabled=!inflag;\r\n\tdocument.all(\"btnCancel\").disabled=!inflag;\r\n\tif(objChk){\r\n\t\tobjChk.checked=true;\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n增加\r\n******************************/\r\nfunction onAdd()\r\n{\r\n\tif(objChk){\r\n\t\tobjChk.checked=false;\r\n\t}\r\n\taction=\"addcommonquestion\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"question\").focus();\r\n}\r\n\r\n/******************************\r\n修改\r\n*****************************/\r\nfunction onModify()\r\n{\r\n\taction=\"modifycommonquestion\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"question\").focus();\r\n}\r\n\r\n/****************************\r\n删除\r\n****************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此常见问题吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delcommonquestion\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n保存\r\n*****************************/\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"keyword\")) strErr+=\"*关键字不能为空！\\n\";\r\n\tif(IsEmpty(\"question\")) strErr+=\"*问题内容不能为空！\\n\";\r\n\tif(IsEmpty(\"answer\")) strErr+=\"*答案不能为空!\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"jx\").value=document.all(\"chkjx\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"jxvnet\").value=document.all(\"chkjxvnet\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"fs\").value=document.all(\"chkfs\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"jx2\").value=document.all(\"chkjx2\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"cq\").value=document.all(\"chkcq\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"quickanswer\").value=document.all(\"chkquickanswer\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"quickdeal\").value=document.all(\"chkquickdeal\").checked?\"1\":\"0\";\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/******************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetEditModel(false);\r\n\taction=\"\";\r\n\tif(document.all(\"action\").value==\"addcommonquestion\")\r\n\t{\r\n\t\tdocument.all(\"question\").value=\"\";\r\n\t\tdocument.all(\"answer\").value=\"\";\r\n\t\tdocument.all(\"keyword\").value=\"\";\r\n\t\tdocument.all(\"chkjx\").checked=false;\r\n\t\tdocument.all(\"chkjxvnet\").checked=false;\r\n\t\tdocument.all(\"chkfs\").checked=false;\r\n\t\tdocument.all(\"chkjx2\").checked=false;\r\n\t\tdocument.all(\"chkcq\").checked=false;\r\n\t\tdocument.all(\"chkquickanswer\").checked=false;\r\n\t}\r\n\telse if(objChk)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objChk.iid;\r\n\t\tdocument.all(\"answer\").value=objChk.answer;\r\n\t\tdocument.all(\"question\").value=objChk.question;\r\n\t\tdocument.all(\"keyword\").value=objChk.keyword;\r\n\t\tdocument.all(\"chkjx\").checked=(objChk.jx!=0)?true:false;\r\n\t\tdocument.all(\"chkjxvnet\").checked=(objChk.jxvnet!=0)?true:false;\r\n\t\tdocument.all(\"chkfs\").checked=(objChk.fs!=0)?true:false;\r\n\t\tdocument.all(\"chkjx2\").checked=(objChk.jx2!=0)?true:false;\r\n\t\tdocument.all(\"chkcq\").checked=(objChk.cq!=0)?true:false;\r\n\t\tdocument.all(\"chkquickanswer\").checked=(objChk.quickanswer!=0)?true:false;\r\n\t\tdocument.all(\"chkquickdeal\").checked=(objChk.quickdeal!=0)?true:false;\r\n\t}\r\n}\r\n\r\n/******************************\r\n再次匹配\r\n*****************************/\r\nfunction onMatch()\r\n{\r\n\tif(confirm(\"你确定要重新匹配待答复问题吗？\"))\r\n\t{\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";//显示提示信息\r\n\t\tdocument.all(\"action\").value=\"rematch\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">常见问题维护</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <br>\r\n      <table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td height=\"200\" align=\"center\" valign=\"top\"> <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(265,104);to=(265,128)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(265,130);to=(275,16)]
                out.write("/SystemServlet\">\r\n              <table width=\"100%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n                <thead>\r\n                  <tr>\r\n                    <td width=\"30\"  align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                    <td width=\"45\" align=\"center\"><font color=\"#FFFFFF\">项目</font></td>\r\n                    <td  align=\"center\"><font color=\"#FFFFFF\">内容</font></td>\r\n                    <td colspan=\"3\"  align=\"center\"><font color=\"#FFFFFF\">适应范围</font></td>\r\n                  </tr>\r\n                </thead>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(275,18);to=(331,0)]
                
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
                           se.setAttribute("commonquestion", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("commonquestion");
                       }
                       num = SystemHelperId.getCommonQuestionListNum();
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       ltcom = SystemHelperId.getCommonQuestionListByPage(curPageInfo[0], curPageInfo[2]);
                   }
                //*********End***************************************************
                	String iid ="";
                	String question = "";
                	String answer ="";
                	String keyword="";
                	String jx="";
                	String jxvnet="";
                	String fs="";
                	String cq="";
                	String jx2="";
                	String quickanswer="";
                	String quickdeal="";
                for (int i=0; i<ltcom.size(); i++)
                {
                	HashMap hscom = (HashMap)ltcom.get(i);
                	iid = (String)hscom.get("iid");
                	answer = (String)hscom.get("answer");
                	question = (String)hscom.get("question");
                	keyword= (String)hscom.get("keyword");
                	jx=(String)hscom.get("jx");
                	jxvnet=(String)hscom.get("jxvnet");
                	fs=(String)hscom.get("fs");
                	cq=(String)hscom.get("cq");
                	jx2=(String)hscom.get("jx2");
                	quickanswer=(String)hscom.get("quickanswer");
                	quickdeal=(String)hscom.get("quickdeal");
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(331,2);to=(332,21)]
                out.write("\r\n                <tr  ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(332,23);to=(332,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(332,36);to=(332,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(332,56);to=(332,57)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(332,59);to=(333,97)]
                out.write(">\r\n                  <td rowspan=\"3\" align=\"center\" valign=\"middle\"><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,100);to=(333,101)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,103);to=(333,114)]
                out.write("\" keyword=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,117);to=(333,124)]
                out.print(keyword);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,126);to=(333,136)]
                out.write("\" answer=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,139);to=(333,145)]
                out.print(answer);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,147);to=(333,161)]
                out.write("\"  quickdeal=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,164);to=(333,173)]
                out.print(quickdeal);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,175);to=(333,191)]
                out.write("\"  quickanswer=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,194);to=(333,205)]
                out.print(quickanswer);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,207);to=(333,219)]
                out.write("\" question=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,222);to=(333,230)]
                out.print(question);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,232);to=(333,239)]
                out.write("\" iid=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,242);to=(333,245)]
                out.print(iid);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,247);to=(333,253)]
                out.write("\" jx=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,256);to=(333,258)]
                out.print(jx);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,260);to=(333,291)]
                out.write("\" onClick=\"onSelect()\" jxvnet=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,294);to=(333,300)]
                out.print(jxvnet);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,302);to=(333,308)]
                out.write("\" fs=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,311);to=(333,313)]
                out.print(fs);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,315);to=(333,321)]
                out.write("\" cq=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,324);to=(333,326)]
                out.print(cq);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,328);to=(333,335)]
                out.write("\" jx2=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,338);to=(333,341)]
                out.print(jx2);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,343);to=(333,349)]
                out.write("\"><br>");

            // end
            // begin [file="/system/commonquestion.jsp";from=(333,352);to=(333,355)]
                out.print(i+1);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(333,357);to=(336,22)]
                out.write("\r\n                  </td>\r\n                  <td align=\"center\" nowrap>关键字</td>\r\n                  <td>");

            // end
            // begin [file="/system/commonquestion.jsp";from=(336,25);to=(336,32)]
                out.print(keyword);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(336,34);to=(338,35)]
                out.write("</td>\r\n                  <td nowrap>\r\n<input type=\"checkbox\" name=\"chkjx_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(338,38);to=(338,39)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(338,41);to=(338,43)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(338,45);to=(338,64)]
                if(jx.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(338,66);to=(338,75)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(338,77);to=(338,78)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(338,80);to=(339,68)]
                out.write(" >剑网官方专区</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkjxvnet_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(339,71);to=(339,72)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(339,74);to=(339,76)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(339,78);to=(339,101)]
                if(jxvnet.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(339,103);to=(339,112)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(339,114);to=(339,115)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(339,117);to=(341,65)]
                out.write(" >\r\n                  剑网互联星空专区</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkjx2_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(341,68);to=(341,69)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(341,71);to=(341,73)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(341,75);to=(341,95)]
                if(jx2.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(341,97);to=(341,106)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(341,108);to=(341,109)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(341,111);to=(344,21)]
                out.write(" >\r\n剑网2</td>\r\n                </tr>\r\n                <tr  ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(344,23);to=(344,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(344,36);to=(344,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(344,56);to=(344,57)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(344,59);to=(346,22)]
                out.write(">\r\n                  <td align=\"center\" nowrap>问题</td>\r\n                  <td>");

            // end
            // begin [file="/system/commonquestion.jsp";from=(346,25);to=(346,33)]
                out.print(question);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(346,35);to=(347,64)]
                out.write("</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkfs_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(347,67);to=(347,68)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(347,70);to=(347,72)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(347,74);to=(347,93)]
                if(fs.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(347,95);to=(347,104)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(347,106);to=(347,107)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(347,109);to=(349,64)]
                out.write(" >\r\n封神榜</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkcq_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(349,67);to=(349,68)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(349,70);to=(349,72)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(349,74);to=(349,93)]
                if(cq.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(349,95);to=(349,104)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(349,106);to=(349,107)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(349,109);to=(351,73)]
                out.write(" >\r\n幻想春秋</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkquickanswer_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(351,76);to=(351,77)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(351,79);to=(351,81)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(351,83);to=(351,111)]
                if(quickanswer.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(351,113);to=(351,122)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(351,124);to=(351,125)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(351,127);to=(354,21)]
                out.write(">\r\n快速答复</td>\r\n                </tr>\r\n                <tr  ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(354,23);to=(354,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(354,36);to=(354,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(354,56);to=(354,57)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(354,59);to=(356,22)]
                out.write(">\r\n                  <td align=\"center\">答案</td>\r\n                  <td>");

            // end
            // begin [file="/system/commonquestion.jsp";from=(356,25);to=(356,31)]
                out.print(answer);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(356,33);to=(358,71)]
                out.write("</td>\r\n                  <td nowrap>&nbsp;</td>\r\n                  <td nowrap><input type=\"checkbox\" name=\"chkquickdeal_");

            // end
            // begin [file="/system/commonquestion.jsp";from=(358,74);to=(358,75)]
                out.print(i);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(358,77);to=(358,79)]
                out.write("\" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(358,81);to=(358,107)]
                if(quickdeal.equals("1")){
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(358,109);to=(358,118)]
                out.write(" checked ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(358,120);to=(358,121)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(358,123);to=(362,16)]
                out.write(">\r\n公聊监控快速处理</td>\r\n                  <td nowrap>&nbsp;</td>\r\n                </tr>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(362,18);to=(362,19)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(362,21);to=(410,16)]
                out.write("\r\n                <tfoot>\r\n                  <tr>\r\n                    <td rowspan=\"3\" align=\"center\" valign=\"middle\">内容</td>\r\n                    <td align=\"center\">关键字</td>\r\n                    <td><input name=\"keyword\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input type=\"checkbox\" name=\"chkjx\" disabled>剑网官方专区</td>\r\n                    <td><input type=\"checkbox\" name=\"chkjxvnet\" disabled>\r\n剑网互联星空专区</td>\r\n                    <td><input type=\"checkbox\" name=\"chkjx2\" disabled>\r\n剑网2</td>\r\n                  </tr>\r\n                  <tr>\r\n                    <td align=\"center\">问题</td>\r\n                    <td><input name=\"question\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input type=\"checkbox\" name=\"chkfs\" disabled>\r\n封神榜</td>\r\n                    <td><input type=\"checkbox\" name=\"chkcq\" disabled>\r\n                      幻想春秋</td>\r\n                    <td><input type=\"checkbox\" name=\"chkquickanswer\" disabled>\r\n快速答复</td>\r\n                  </tr>\r\n                  <tr>\r\n                    <td align=\"center\">答案</td>\r\n                    <td><input name=\"answer\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td>&nbsp;</td>\r\n                    <td><input type=\"checkbox\" name=\"chkquickdeal\" disabled>\r\n公聊监控快速处理</td>\r\n                    <td>&nbsp;</td>\r\n                  </tr>\r\n                </tfoot>\r\n              </table>\r\n              <input type=\"hidden\" name=\"action\">\r\n              <input type=\"hidden\" name=\"iid\">\r\n              <input type=\"hidden\" name=\"jx\">\r\n              <input type=\"hidden\" name=\"jxvnet\">\r\n              <input type=\"hidden\" name=\"fs\">\r\n              <input type=\"hidden\" name=\"jx2\">\r\n              <input type=\"hidden\" name=\"cq\">\r\n              <input type=\"hidden\" name=\"quickanswer\">\r\n              <input type=\"hidden\" name=\"quickdeal\">\r\n            </form></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n            <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n              <tr>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(410,18);to=(419,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(419,9);to=(420,40)]
                out.write("\r\n                <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/system/commonquestion.jsp";from=(420,43);to=(420,46)]
                out.print(num);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(420,48);to=(420,62)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/system/commonquestion.jsp";from=(420,65);to=(420,82)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(420,84);to=(420,85)]
                out.write("～");

            // end
            // begin [file="/system/commonquestion.jsp";from=(420,88);to=(420,113)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(420,115);to=(421,16)]
                out.write("条</td>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(421,18);to=(424,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(424,17);to=(425,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(425,43);to=(425,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(425,71);to=(426,40)]
                out.write("/system/commonquestion.jsp?pageAction=first&start=\">第一页</a></td>\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(426,43);to=(426,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(426,71);to=(426,124)]
                out.write("/system/commonquestion.jsp?pageAction=previous&start=");

            // end
            // begin [file="/system/commonquestion.jsp";from=(426,127);to=(426,150)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(426,152);to=(427,16)]
                out.write("\">上一页</a></td>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(427,18);to=(428,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(428,9);to=(431,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>第一页</u></td>\r\n                <td width=\"7%\"><u disabled>上一页</u></td>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(431,18);to=(434,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(434,16);to=(435,40)]
                out.write("\r\n                <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(435,43);to=(435,69)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(435,71);to=(435,120)]
                out.write("/system/commonquestion.jsp?pageAction=next&start=");

            // end
            // begin [file="/system/commonquestion.jsp";from=(435,123);to=(435,138)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(435,140);to=(436,41)]
                out.write("\">下一页</a></td>\r\n                <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(436,44);to=(436,70)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(436,72);to=(436,121)]
                out.write("/system/commonquestion.jsp?pageAction=last&start=");

            // end
            // begin [file="/system/commonquestion.jsp";from=(436,124);to=(436,142)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(436,144);to=(437,16)]
                out.write("\">最后一页</a></td>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(437,18);to=(437,27)]
                 } else {
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(437,29);to=(441,16)]
                out.write("\r\n                <td width=\"7%\"><u disabled>下一页</u></td>\r\n                <td width=\"12%\"><u disabled>最后一页</u></td>\r\n                <td width=\"12%\">excel格式</td>\r\n                ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(441,18);to=(441,19)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(441,21);to=(442,40)]
                out.write("\r\n                <td width=\"10%\" nowrap> ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(442,42);to=(445,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(445,17);to=(445,18)]
                out.write(" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(445,21);to=(445,35)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(445,37);to=(445,38)]
                out.write("/");

            // end
            // begin [file="/system/commonquestion.jsp";from=(445,41);to=(445,54)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(445,56);to=(445,57)]
                out.write(" ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(445,59);to=(445,60)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(445,62);to=(446,40)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap> ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(446,42);to=(449,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(449,17);to=(451,57)]
                out.write("\r\n                  转到第\r\n                  <INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(451,60);to=(451,74)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(451,76);to=(451,135)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/system/commonquestion.jsp";from=(451,138);to=(451,162)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(451,164);to=(451,179)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(451,182);to=(451,187)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(451,189);to=(453,62)]
                out.write(")\">\r\n                  页\r\n                  <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/system/commonquestion.jsp";from=(453,65);to=(453,89)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(453,91);to=(453,106)]
                out.write("', page.value, ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(453,109);to=(453,114)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(453,116);to=(455,18)]
                out.write(")\">\r\n                  GO </button>\r\n                  ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(455,20);to=(455,21)]
                }
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(455,23);to=(457,57)]
                out.write(" </td>\r\n                <td width=\"20%\" nowrap>页长设置为\r\n                  <INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/system/commonquestion.jsp";from=(457,60);to=(457,65)]
                out.print(nStep);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(457,67);to=(457,128)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/system/commonquestion.jsp";from=(457,131);to=(457,155)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(457,157);to=(457,172)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(457,175);to=(457,192)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(457,194);to=(458,61)]
                out.write(")\">\r\n                  <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/system/commonquestion.jsp";from=(458,64);to=(458,88)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(458,90);to=(458,105)]
                out.write("', step.value, ");

            // end
            // begin [file="/system/commonquestion.jsp";from=(458,108);to=(458,125)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/system/commonquestion.jsp";from=(458,127);to=(486,0)]
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
