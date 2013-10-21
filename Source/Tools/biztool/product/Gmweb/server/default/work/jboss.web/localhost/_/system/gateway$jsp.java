package org.apache.jsp;

import java.sql.*;
import java.util.*;
import  com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class gateway$jsp extends HttpJspBase {

    // begin [file="/system/gateway.jsp";from=(4,0);to=(4,102)]
    // end
    // begin [file="/system/gateway.jsp";from=(5,0);to=(5,107)]
    // end

    static {
    }
    public gateway$jsp( ) {
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

            // HTML // begin [file="/system/gateway.jsp";from=(0,162);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/gateway.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/gateway.jsp";from=(2,52);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/gateway.jsp";from=(3,0);to=(3,41)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("gateway");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/gateway.jsp";from=(3,0);to=(3,41)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(3,41);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/gateway.jsp";from=(4,0);to=(4,102)]
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
            // begin [file="/system/gateway.jsp";from=(4,0);to=(4,102)]
                }
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(4,102);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/gateway.jsp";from=(5,0);to=(5,107)]
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
            // begin [file="/system/gateway.jsp";from=(5,0);to=(5,107)]
                }
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(5,107);to=(14,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>游戏区服维护</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(14,42);to=(18,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/system/gateway.jsp";from=(18,2);to=(23,0)]
                
                    //this prevents caching in NS and IE
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(23,2);to=(278,101)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar objChk=null;//当前被选中的\r\nvar action=null;\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/system/gateway.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/system/gateway.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"area\").value=objCurChk.area;\r\n\t\tdocument.all(\"group\").value=objCurChk.group;\r\n\t\tdocument.all(\"region\").value=objCurChk.region;\r\n\t\tdocument.all(\"zonecode\").value=objCurChk.zonecode;\r\n\t\tdocument.all(\"iorder\").value=objCurChk.iorder;\r\n\t\tdocument.all(\"gateway_name\").value=objCurChk.gateway_name;\r\n\t\tdocument.all(\"nameserverdbip\").value=objChk.nameserverdbip;\r\n\t\tdocument.all(\"dbname\").value=objChk.dbname;\r\n\t\tdocument.all(\"dbuser\").value=objChk.dbuser;\r\n\t\tdocument.all(\"dbpassword\").value=objChk.dbpassword;\r\n\t\tSetSelValue(\"game_id\",objChk.game_id);\r\n\t\tonChangeGameId();\r\n\t\tSetSelValue(\"idc_id\",objChk.idc_id);\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"btnDel\").disabled=false;\r\n\t}\r\n}\r\n\r\n/******************************\r\ninflag: true，可编辑   false,不可编辑状态\r\n*******************************/\r\nfunction SetEditModel(inflag)\r\n{\r\n\tSetChkState(inflag);\r\n\tdocument.all(\"game_id\").disabled=!inflag;\r\n\tdocument.all(\"idc_id\").disabled=!inflag;\r\n\tdocument.all(\"area\").disabled=!inflag;\r\n\tdocument.all(\"group\").disabled=!inflag;\r\n\tdocument.all(\"region\").disabled=!inflag;\r\n\tdocument.all(\"zonecode\").disabled=!inflag;\r\n\tdocument.all(\"iorder\").disabled=!inflag;\r\n\tdocument.all(\"gateway_name\").disabled=!inflag;\r\n\t\r\n\tdocument.all(\"nameserverdbip\").disabled=!inflag;\r\n\tdocument.all(\"dbname\").disabled=!inflag;\r\n\tdocument.all(\"dbuser\").disabled=!inflag;\r\n\tdocument.all(\"dbpassword\").disabled=!inflag;\r\n\t\r\n\tif(action==\"addgatewayinfo\")\r\n\t{\r\n\t\tdocument.all(\"area\").value=\"\";\r\n\t\tdocument.all(\"group\").value=\"\";\r\n\t\tdocument.all(\"region\").value=\"\";\r\n\t\tdocument.all(\"zonecode\").value=\"\";\r\n\t\tdocument.all(\"iorder\").value=\"\";\r\n\t\tdocument.all(\"gateway_name\").value=\"\";\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t\tdocument.all(\"nameserverdbip\").value=\"\";\r\n\t\tdocument.all(\"dbname\").value=\"\";\r\n\t\tdocument.all(\"dbuser\").value=\"\";\r\n\t\tdocument.all(\"dbpassword\").value=\"\";\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=inflag;\r\n\tdocument.all(\"btnModify\").disabled=inflag;\r\n\tdocument.all(\"btnDel\").disabled=inflag;\r\n\tdocument.all(\"btnSave\").disabled=!inflag;\r\n\tdocument.all(\"btnCancel\").disabled=!inflag;\r\n\tif(objChk){\r\n\t\tobjChk.checked=true;\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n增加\r\n******************************/\r\nfunction onAdd()\r\n{\r\n\tif(objChk){\r\n\t\tobjChk.checked=false;\r\n\t}\r\n\taction=\"addgatewayinfo\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"gateway_name\").focus();\r\n}\r\n\r\n/******************************\r\n修改\r\n*****************************/\r\nfunction onModify()\r\n{\r\n\taction=\"modifygatewayinfo\";\r\n\tSetEditModel(true);\r\n\tdocument.all(\"gateway_name\").focus();\r\n}\r\n\r\n/****************************\r\n删除\r\n****************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此游戏区服吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delgatewayinfo\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\n/******************************\r\n保存\r\n*****************************/\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"gateway_name\")) strErr+=\"*区服名称不能为空！\\n\";\r\n\tif(IsEmpty(\"area\")) strErr+=\"*区为不能空！\\n\";\r\n\tif(IsEmpty(\"group\")) strErr+=\"*服为不能空！\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/******************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetEditModel(false);\r\n\taction=\"\";\r\n\tif(document.all(\"action\").value==\"addgatewayinfo\")\r\n\t{\r\n\t\tdocument.all(\"area\").value=\"\";\r\n\t\tdocument.all(\"group\").value=\"\";\r\n\t\tdocument.all(\"region\").value=\"\";\r\n\t\tdocument.all(\"zonecode\").value=\"\";\r\n\t\tdocument.all(\"iorder\").value=\"\";\r\n\t\tdocument.all(\"gateway_name\").value=\"\";\r\n\t\tdocument.all(\"nameserverdbip\").value=\"\";\r\n\t\tdocument.all(\"dbname\").value=\"\";\r\n\t\tdocument.all(\"dbuser\").value=\"\";\r\n\t\tdocument.all(\"dbpassword\").value=\"\";\r\n\t\t\r\n\t}\r\n\telse if(objChk)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objChk.iid;\r\n\t\tdocument.all(\"area\").value=objChk.area;\r\n\t\tdocument.all(\"group\").value=objChk.area;\r\n\t\tdocument.all(\"region\").value=objChk.region;\r\n\t\tdocument.all(\"zonecode\").value=objChk.zonecode;\r\n\t\tdocument.all(\"iorder\").value=objChk.iorder;\r\n\t\tdocument.all(\"gateway_name\").value=objChk.gateway_name;\r\n\t\tdocument.all(\"nameserverdbip\").value=objChk.nameserverdbip;\r\n\t\tdocument.all(\"dbname\").value=objChk.dbname;\r\n\t\tdocument.all(\"dbuser\").value=objChk.dbuser;\r\n\t\tdocument.all(\"dbpassword\").value=objChk.dbpassword;\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"idc_id\");\r\n\tvar objSelTemp=document.all(\"selIdcTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t}\r\n\t}\r\n}\r\n\r\nwindow.onload = function(){\r\n    onChangeGameId();   \r\n}   \r\n\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body >\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">游戏区服维护</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n    <br>\r\n      <table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td height=\"200\" align=\"center\" valign=\"top\"> <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(278,104);to=(278,128)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(278,130);to=(297,16)]
                out.write("/SystemServlet\">\r\n              <table width=\"95%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n                <thead>\r\n                  <tr>\r\n                    <td width=\"5%\" align=\"center\" nowrap width=\"30\"><font color=\"#FFFFFF\">序号</font></td>\r\n                    <td width=\"8%\" align=\"center\"><font color=\"#FFFFFF\">所属游戏</font></td>\r\n                    <td width=\"8%\" align=\"center\"><font color=\"#FFFFFF\">所在IDC</font></td>\r\n                    <td width=\"12%\" align=\"center\"><font color=\"#FFFFFF\">区服名称</font></td>\r\n                    <td width=\"5%\" align=\"center\"><font color=\"#FFFFFF\">区</font></td>\r\n                    <td width=\"5%\" align=\"center\"><font color=\"#FFFFFF\">服</font></td>\r\n                    <td width=\"6%\" align=\"center\"><font color=\"#FFFFFF\">大区编号</font></td>\r\n                    <td width=\"6%\" align=\"center\"><font color=\"#FFFFFF\">区服编号</font></td>\r\n                    <td width=\"12%\" align=\"center\"><font color=\"#FFFFFF\">角色库地址</font></td>\r\n                    <td width=\"8%\" align=\"center\"><font color=\"#FFFFFF\">库名称</font></td>\r\n                    <td width=\"10%\" align=\"center\"><font color=\"#FFFFFF\">用户名</font></td>\r\n                    <td width=\"10%\" align=\"center\"><font color=\"#FFFFFF\">密码</font></td>\r\n                    <td width=\"5%\" align=\"center\"><font color=\"#FFFFFF\">排序</font></td>\r\n                  </tr>\r\n                </thead>\r\n                ");

            // end
            // begin [file="/system/gateway.jsp";from=(297,18);to=(362,0)]
                
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
                           se.setAttribute("gateway", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("gateway");
                       }
                       num = SystemHelperId.getGatewayInfoListNum();
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       lt = SystemHelperId.getGatewayInfoListByPage(curPageInfo[0], curPageInfo[2]);
                   }
                //*********End***************************************************
                	String iid ="";
                	String game_id = "";
                	String game_name="";
                	String idc_id="";
                	String idc_name="";
                	String gateway_name="";
                	String area="";
                	String group="";
                	String region="";
                	String zonecode="";
                	String iorder="";
                	String nameserverdbip = null;
                	String dbname = null;
                	String dbuser = null;
                	String dbpassword = null;
                	HashMap hs = null;
                for (int i=0; i<lt.size(); i++)
                {
                	hs = (HashMap)lt.get(i);
                	iid = (String)hs.get("iid");
                	game_id = (String)hs.get("game_id");
                	game_name = (String)hs.get("game_name");
                	idc_id = (String)hs.get("idc_id");
                	idc_name = (String)hs.get("idc_name");
                	area= (String)hs.get("area");
                	group= (String)hs.get("group");
                	gateway_name= (String)hs.get("gateway_name");
                	region= (String)hs.get("region");
                	zonecode= (String)hs.get("zonecode");
                	iorder= (String)hs.get("iorder");
                	nameserverdbip= (String)hs.get("nameserverdbip");
                	dbname= (String)hs.get("dbname");
                	dbuser= (String)hs.get("dbuser");
                	dbpassword= (String)hs.get("dbpassword");
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(362,2);to=(363,21)]
                out.write("\r\n                <tr  ");

            // end
            // begin [file="/system/gateway.jsp";from=(363,23);to=(363,34)]
                if(i%2==0){
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(363,36);to=(363,54)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/system/gateway.jsp";from=(363,56);to=(363,57)]
                }
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(363,59);to=(364,92)]
                out.write(">\r\n                  <td align=\"center\" valign=\"middle\" nowrap><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/system/gateway.jsp";from=(364,95);to=(364,96)]
                out.print(i);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(364,98);to=(365,28)]
                out.write("\"\r\n                   game_id=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(365,31);to=(365,38)]
                out.print(game_id);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(365,40);to=(365,49)]
                out.write("\"  area=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(365,52);to=(365,56)]
                out.print(area);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(365,58);to=(365,67)]
                out.write("\" group=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(365,70);to=(365,75)]
                out.print(group);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(365,77);to=(365,93)]
                out.write("\" gateway_name=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(365,96);to=(365,108)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(365,110);to=(366,25)]
                out.write("\"\r\n                    iid=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(366,28);to=(366,31)]
                out.print(iid);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(366,33);to=(366,65)]
                out.write("\" onClick=\"onSelect()\"  region=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(366,68);to=(366,74)]
                out.print(region);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(366,76);to=(366,86)]
                out.write("\" iorder=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(366,89);to=(366,95)]
                out.print(iorder);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(366,97);to=(366,109)]
                out.write("\" zonecode=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(366,112);to=(366,120)]
                out.print(zonecode);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(366,122);to=(366,132)]
                out.write("\" idc_id=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(366,135);to=(366,141)]
                out.print(idc_id);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(366,143);to=(367,38)]
                out.write("\"\r\n                      nameserverdbip=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(367,41);to=(367,55)]
                out.print(nameserverdbip);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(367,57);to=(367,67)]
                out.write("\" dbname=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(367,70);to=(367,76)]
                out.print(dbname);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(367,78);to=(367,88)]
                out.write("\" dbuser=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(367,91);to=(367,97)]
                out.print(dbuser);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(367,99);to=(367,113)]
                out.write("\" dbpassword=\"");

            // end
            // begin [file="/system/gateway.jsp";from=(367,116);to=(367,126)]
                out.print(dbpassword);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(367,128);to=(367,130)]
                out.write("\">");

            // end
            // begin [file="/system/gateway.jsp";from=(367,133);to=(367,136)]
                out.print(i+1);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(367,138);to=(369,37)]
                out.write("\r\n                  </td>\r\n                  <td align=\"center\">");

            // end
            // begin [file="/system/gateway.jsp";from=(369,40);to=(369,49)]
                out.print(game_name);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(369,51);to=(370,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(370,47);to=(370,55)]
                out.print(idc_name);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(370,57);to=(371,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(371,47);to=(371,59)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(371,61);to=(372,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(372,47);to=(372,51)]
                out.print(area);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(372,53);to=(373,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(373,47);to=(373,52)]
                out.print(group);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(373,54);to=(374,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(374,47);to=(374,55)]
                out.print(zonecode);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(374,57);to=(375,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(375,47);to=(375,53)]
                out.print(region);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(375,55);to=(376,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(376,47);to=(376,61)]
                out.print(nameserverdbip);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(376,63);to=(377,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(377,47);to=(377,53)]
                out.print(dbname);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(377,55);to=(378,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(378,47);to=(378,53)]
                out.print(dbuser);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(378,55);to=(379,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(379,47);to=(379,57)]
                out.print(dbpassword);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(379,59);to=(380,44)]
                out.write("</td>\r\n                  <td align=\"center\" nowrap>");

            // end
            // begin [file="/system/gateway.jsp";from=(380,47);to=(380,53)]
                out.print(iorder);
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(380,55);to=(382,16)]
                out.write("</td>\r\n                </tr>\r\n                ");

            // end
            // begin [file="/system/gateway.jsp";from=(382,18);to=(382,19)]
                }
            // end
            // HTML // begin [file="/system/gateway.jsp";from=(382,21);to=(392,64)]
                out.write("\r\n                <tfoot>\r\n                  <tr>\r\n                    <td align=\"center\" valign=\"middle\">内容</td>\r\n                    <td><select name=\"game_id\" onChange=\"onChangeGameId()\" disabled>\r\n                     <option value=\"88\">剑网3</option>\r\n                      <option value=\"6\">剑网2</option>\r\n              <option value=\"66\">剑网2 IB</option>\r\n              <option value=\"77\">剑侠世界</option>\r\n                      </select></td>\r\n                    <td><select name=\"idc_id\" disabled></select>");

            // end
            // begin [file="/system/gateway.jsp";from=(392,64);to=(392,149)]
                /* ----  idc:selectidc ---- */
                com.kingsoft.gmsystem.web.taglib.IDCTag _jspx_th_idc_selectidc_0 = new com.kingsoft.gmsystem.web.taglib.IDCTag();
                _jspx_th_idc_selectidc_0.setPageContext(pageContext);
                _jspx_th_idc_selectidc_0.setParent(null);
                _jspx_th_idc_selectidc_0.setDisplayCode("select_idc");
                _jspx_th_idc_selectidc_0.setSelectName("selIdcTemp");
                _jspx_th_idc_selectidc_0.setStyle("display:none");
                try {
                    int _jspx_eval_idc_selectidc_0 = _jspx_th_idc_selectidc_0.doStartTag();
                    if (_jspx_eval_idc_selectidc_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.IDCTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                // end
                // begin [file="/system/gateway.jsp";from=(392,64);to=(392,165)]
                if (_jspx_th_idc_selectidc_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                    return;
            } finally {
                _jspx_th_idc_selectidc_0.release();
            }
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(392,165);to=(419,16)]
            out.write("</td>\r\n                    <td><input name=\"gateway_name\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"area\" type=\"text\"  style=\"width:100%\" onKeyPress=\"OnlyInputNumber()\"  disabled></td>\r\n                    <td><input name=\"group\" type=\"text\"  style=\"width:100%\"  onKeyPress=\"OnlyInputNumber()\"  disabled></td>\r\n                    <td>\r\n                      <input name=\"zonecode\" type=\"text\"  style=\"width:100%\"   disabled>                    </td>\r\n                    <td>\r\n                      <input name=\"region\" type=\"text\"  style=\"width:100%\"   disabled></td>\r\n                    <td><input name=\"nameserverdbip\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td><input name=\"dbname\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td>\r\n                      <input name=\"dbuser\" type=\"text\"  style=\"width:100%\" disabled>                    </td>\r\n                    <td>\r\n                      <input name=\"dbpassword\" type=\"text\"  style=\"width:100%\"  disabled></td>\r\n                    <td> <input name=\"iorder\" type=\"text\"  style=\"width:100%\"  onKeyPress=\"OnlyInputNumber()\" disabled></td>\r\n                  </tr>\r\n                </tfoot>\r\n              </table>\r\n              <input type=\"hidden\" name=\"action\">\r\n              <input type=\"hidden\" name=\"iid\">\r\n            </form></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n            <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n              <tr>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(419,18);to=(428,7)]
            
                     nStep = curPageInfo[2];
                     curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                     sumPageNumber = curPageInfo[3];
                     lastPageStartIndex = curPageInfo[4];
                     curPageIndex = curPageInfo[5];
                     curPageStartIndex = curPageInfo[0];
                     isPrevious = curPageInfo[6];
                     isNext = curPageInfo[7];
                   
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(428,9);to=(429,40)]
            out.write("\r\n                <td width=\"15%\" nowrap>共");

        // end
        // begin [file="/system/gateway.jsp";from=(429,43);to=(429,46)]
            out.print(num);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(429,48);to=(429,62)]
            out.write("条&nbsp;&nbsp;第");

        // end
        // begin [file="/system/gateway.jsp";from=(429,65);to=(429,82)]
            out.print(curPageStartIndex);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(429,84);to=(429,85)]
            out.write("～");

        // end
        // begin [file="/system/gateway.jsp";from=(429,88);to=(429,113)]
            out.print(curPageStartIndex + nStep);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(429,115);to=(430,16)]
            out.write("条</td>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(430,18);to=(433,15)]
            
                       if ((isPrevious == 1)&&(num > nStep))
                       {
                           
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(433,17);to=(434,40)]
            out.write("\r\n                <td width=\"7%\"><a href=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(434,43);to=(434,69)]
            out.print( request.getContextPath() );
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(434,71);to=(435,40)]
            out.write("/system/gateway.jsp?pageAction=first&start=\">第一页</a></td>\r\n                <td width=\"7%\"><a href=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(435,43);to=(435,69)]
            out.print( request.getContextPath() );
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(435,71);to=(435,117)]
            out.write("/system/gateway.jsp?pageAction=previous&start=");

        // end
        // begin [file="/system/gateway.jsp";from=(435,120);to=(435,143)]
            out.print(curPageEndIndex-2*nStep);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(435,145);to=(436,16)]
            out.write("\">上一页</a></td>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(436,18);to=(437,7)]
             } else {
                   
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(437,9);to=(440,16)]
            out.write("\r\n                <td width=\"7%\"><u disabled>第一页</u></td>\r\n                <td width=\"7%\"><u disabled>上一页</u></td>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(440,18);to=(443,14)]
               }
                       if((isNext == 1) &&(num > nStep))
                       {
                          
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(443,16);to=(444,40)]
            out.write("\r\n                <td width=\"7%\"><a href=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(444,43);to=(444,69)]
            out.print( request.getContextPath() );
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(444,71);to=(444,113)]
            out.write("/system/gateway.jsp?pageAction=next&start=");

        // end
        // begin [file="/system/gateway.jsp";from=(444,116);to=(444,131)]
            out.print(curPageEndIndex);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(444,133);to=(445,41)]
            out.write("\">下一页</a></td>\r\n                <td width=\"12%\"><a href=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(445,44);to=(445,70)]
            out.print( request.getContextPath() );
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(445,72);to=(445,114)]
            out.write("/system/gateway.jsp?pageAction=last&start=");

        // end
        // begin [file="/system/gateway.jsp";from=(445,117);to=(445,135)]
            out.print(lastPageStartIndex);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(445,137);to=(446,16)]
            out.write("\">最后一页</a></td>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(446,18);to=(446,27)]
             } else {
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(446,29);to=(450,16)]
            out.write("\r\n                <td width=\"7%\"><u disabled>下一页</u></td>\r\n                <td width=\"12%\"><u disabled>最后一页</u></td>\r\n                <td width=\"12%\"></td>\r\n                ");

        // end
        // begin [file="/system/gateway.jsp";from=(450,18);to=(450,19)]
            }
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(450,21);to=(451,40)]
            out.write("\r\n                <td width=\"10%\" nowrap> ");

        // end
        // begin [file="/system/gateway.jsp";from=(451,42);to=(454,15)]
            
                       if(num > nStep)
                       {
                           
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(454,17);to=(454,18)]
            out.write(" ");

        // end
        // begin [file="/system/gateway.jsp";from=(454,21);to=(454,35)]
            out.print(curPageIndex+1);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(454,37);to=(454,38)]
            out.write("/");

        // end
        // begin [file="/system/gateway.jsp";from=(454,41);to=(454,54)]
            out.print(sumPageNumber);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(454,56);to=(454,57)]
            out.write(" ");

        // end
        // begin [file="/system/gateway.jsp";from=(454,59);to=(454,60)]
            }
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(454,62);to=(455,40)]
            out.write(" </td>\r\n                <td width=\"20%\" nowrap> ");

        // end
        // begin [file="/system/gateway.jsp";from=(455,42);to=(458,15)]
            
                       if(num > nStep)
                       {
                           
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(458,17);to=(460,57)]
            out.write("\r\n                  转到第\r\n                  <INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(460,60);to=(460,74)]
            out.print(curPageIndex+1);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(460,76);to=(460,135)]
            out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

        // end
        // begin [file="/system/gateway.jsp";from=(460,138);to=(460,162)]
            out.print(request.getContextPath());
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(460,164);to=(460,179)]
            out.write("', page.value, ");

        // end
        // begin [file="/system/gateway.jsp";from=(460,182);to=(460,187)]
            out.print(nStep);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(460,189);to=(462,62)]
            out.write(")\">\r\n                  页\r\n                  <button name=\"searchBut\"  onClick=\"goClick('");

        // end
        // begin [file="/system/gateway.jsp";from=(462,65);to=(462,89)]
            out.print(request.getContextPath());
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(462,91);to=(462,106)]
            out.write("', page.value, ");

        // end
        // begin [file="/system/gateway.jsp";from=(462,109);to=(462,114)]
            out.print(nStep);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(462,116);to=(464,18)]
            out.write(")\">\r\n                  GO </button>\r\n                  ");

        // end
        // begin [file="/system/gateway.jsp";from=(464,20);to=(464,21)]
            }
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(464,23);to=(466,57)]
            out.write(" </td>\r\n                <td width=\"20%\" nowrap>页长设置为\r\n                  <INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

        // end
        // begin [file="/system/gateway.jsp";from=(466,60);to=(466,65)]
            out.print(nStep);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(466,67);to=(466,128)]
            out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

        // end
        // begin [file="/system/gateway.jsp";from=(466,131);to=(466,155)]
            out.print(request.getContextPath());
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(466,157);to=(466,172)]
            out.write("', step.value, ");

        // end
        // begin [file="/system/gateway.jsp";from=(466,175);to=(466,192)]
            out.print(curPageStartIndex);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(466,194);to=(467,61)]
            out.write(")\">\r\n                  <button name=\"stepBut\" onClick=\"stepClick('");

        // end
        // begin [file="/system/gateway.jsp";from=(467,64);to=(467,88)]
            out.print(request.getContextPath());
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(467,90);to=(467,105)]
            out.write("', step.value, ");

        // end
        // begin [file="/system/gateway.jsp";from=(467,108);to=(467,125)]
            out.print(curPageStartIndex);
        // end
        // HTML // begin [file="/system/gateway.jsp";from=(467,127);to=(496,0)]
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
