package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class gmfunctionlog$jsp extends HttpJspBase {

    // begin [file="/search/gmfunctionlog.jsp";from=(1,0);to=(1,107)]
    // end
    // begin [file="/search/gmfunctionlog.jsp";from=(2,0);to=(2,102)]
    // end

    static {
    }
    public gmfunctionlog$jsp( ) {
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

            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(1,0);to=(1,107)]
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
            // begin [file="/search/gmfunctionlog.jsp";from=(1,0);to=(1,107)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(1,107);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(2,0);to=(2,102)]
                com.kingsoft.gmsystem.service.SearchHelper SearchHelperId = null;
                boolean _jspx_specialSearchHelperId  = false;
                 synchronized (session) {
                    SearchHelperId= (com.kingsoft.gmsystem.service.SearchHelper)
                    pageContext.getAttribute("SearchHelperId",PageContext.SESSION_SCOPE);
                    if ( SearchHelperId == null ) {
                        _jspx_specialSearchHelperId = true;
                        try {
                            SearchHelperId = (com.kingsoft.gmsystem.service.SearchHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SearchHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SearchHelper", exc);
                        }
                        pageContext.setAttribute("SearchHelperId", SearchHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSearchHelperId == true) {
            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(2,0);to=(2,102)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(2,102);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(3,57);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(4,56);to=(5,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(5,64);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(6,0);to=(6,47)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("gmfunctionlog");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/gmfunctionlog.jsp";from=(6,0);to=(6,47)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(6,47);to=(15,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>操作记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(15,16);to=(15,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(15,42);to=(19,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(19,2);to=(22,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(22,2);to=(139,73)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/search/gmfunctionlog.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/search/gmfunctionlog.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"gateway\");\r\n\tdocument.all(\"game_id\").value=objSel.options[objSel.selectedIndex].game_id;\r\n\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n}\r\n\r\nfunction Search()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"operatetime\")) strErr+=\"*操作时间不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tsetGatewayInfo();\r\n\t\tvar operatetime=document.all(\"operatetime\").value;\r\n\t\tvar usercode=document.all(\"usercode\").value;\r\n\t\tvar script=document.all(\"script\").value;\r\n\t\tvar game_id=document.all(\"game_id\").value;\r\n\t\tvar area=document.all(\"area\").value;\r\n\t\tvar group=document.all(\"group\").value;\r\n\t\tvar account=document.all(\"account\").value;\r\n\t\tvar role=document.all(\"role\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/gmfunctionlog.jsp?pageAction=servlet&step=&start=&operatetime=\"+operatetime+\r\n\t\t\t\t\"&usercode=\"+usercode+\"&script=\"+script+\r\n\t\t\t\t\"&game_id=\"+game_id+\"&area=\"+area+\"&group=\"+group+\"&account=\"+account+\"&role=\"+role;\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\nfunction addScript(){\r\n\t\r\n\tvar objSel=document.all(\"script\");\r\n\tvar objSelTemp=document.all(\"gmcFunction\");\r\n\tvar objOption;\r\n\t\r\n\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tobjSel.options[objSel.length]=new Option((objSel.length-5) +\".\"+objOption.text,objOption.text);\r\n\t}\r\n}\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body onload=\"addScript()\">\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">操作记录</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td>&nbsp;&nbsp;&nbsp;&nbsp;操作时间\r\n                  <input name=\"operatetime\" type=\"text\" size=\"21\" value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(139,76);to=(139,132)]
                out.print(Util.setNullToEmpty(request.getParameter("operatetime")));
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(139,134);to=(141,70)]
                out.write("\">\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('operatetime')\" style=\"cursor:hand \">操作人\r\n                  <input name=\"usercode\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(141,73);to=(141,121)]
                out.print(Util.setNullToEmpty((String)hs1.get("usercode")));
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(141,123);to=(172,4)]
                out.write("\">\r\n                  GM操作\r\n                  <select name=\"script\">\r\n                    <option value=\"all\">所有操作</option>\r\n                    <option value=\"所有公告\">1.所有公告</option>\r\n                    <option value=\"发布全区普通公告\">&nbsp;&nbsp;┣发布全区普通公告</option>\r\n                    <option value=\"发布某区普通公告\">&nbsp;&nbsp;┣发布某区普通公告</option>\r\n                    <option value=\"发布全区重要公告\">&nbsp;&nbsp;┣发布全区重要公告</option>\r\n                    <option value=\"发布某区重要公告\">&nbsp;&nbsp;┗发布某区重要公告</option>\r\n                    <option value=\"密玩家\">2.密玩家</option>\r\n                    <option value=\"禁言\">3.禁言</option>\r\n                    <option value=\"解除禁言\">4.解除禁言</option>\r\n                    <option value=\"强制帐号下线\">5.强制帐号下线</option>\r\n                    <option value=\"强制角色下线\">6.强制角色下线</option>\r\n                    <option value=\"冻结帐号\">6.冻结帐号</option>\r\n                    <option value=\"解冻帐号\">7.解冻帐号</option>\r\n                    <option value=\"冻结玩家\">8.冻结玩家</option>\r\n                    <option value=\"解冻玩家\">9.解冻玩家</option>\r\n                    <option value=\"关入天牢\">10.关入天牢</option>\r\n                    <option value=\"解禁天牢\">11.解禁天牢</option>\r\n                    <option value=\"瞬移到指定位置\">12.瞬移到指定位置</option>\r\n                    <option value=\"修改通行证绑定信息\">13.修改通行证绑定信息</option>\r\n                    <option value=\"临时置换帐号密码\">14.临时置换帐号密码</option>\r\n                    <option value=\"恢复临时置换密码\">15.恢复临时置换密码</option>\r\n                    <option value=\"重置帐号密码锁\">16.重置帐号密码锁</option>\r\n                    <option value=\"给指定帐号密言\">17.给指定帐号密言</option>\r\n                    <option value=\"给指定角色密言\">18.给指定角色密言</option>\r\n                    <option value=\"关键查询操作\">19.关键查询操作</option>\r\n                    <option value=\"修改用户防沉迷信息\">20.修改用户防沉迷信息</option>\r\n                  </select>\r\n                  \r\n\t\t\t\t");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(172,4);to=(174,43)]
                /* ----  gmcfunction:gmcfunctionselect ---- */
                com.kingsoft.gmsystem.web.taglib.GMCFunctionTag _jspx_th_gmcfunction_gmcfunctionselect_0 = new com.kingsoft.gmsystem.web.taglib.GMCFunctionTag();
                _jspx_th_gmcfunction_gmcfunctionselect_0.setPageContext(pageContext);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setParent(null);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setDisplayCode("select_function");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setSelectName("gmcFunction");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setOnChange("onChangeFunction()");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setStyle("width:160;display:none");
                try {
                    int _jspx_eval_gmcfunction_gmcfunctionselect_0 = _jspx_th_gmcfunction_gmcfunctionselect_0.doStartTag();
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GMCFunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/gmfunctionlog.jsp";from=(172,4);to=(174,43)]
                        } while (_jspx_th_gmcfunction_gmcfunctionselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_gmcfunction_gmcfunctionselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_gmcfunction_gmcfunctionselect_0.release();
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(174,43);to=(178,58)]
                out.write("\r\n                </td>\r\n              </tr>\r\n              <tr>\r\n                <td nowrap>  &nbsp;&nbsp;&nbsp;&nbsp;操作区服 ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(178,58);to=(178,150)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_gateway_system");
                _jspx_th_display_display_0.setProperty("gateway");
                _jspx_th_display_display_0.setParameter("haveall");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/search/gmfunctionlog.jsp";from=(178,150);to=(179,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/search/gmfunctionlog.jsp";from=(179,12);to=(179,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(179,30);to=(183,63)]
                out.write(" <input type=\"hidden\" name=\"game_id\">\r\n            <input type=\"hidden\" name=\"area\">\r\n            <input type=\"hidden\" name=\"group\">\r\n玩家帐号\r\n            <input name=\"account\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(183,66);to=(183,113)]
                out.print(Util.setNullToEmpty((String)hs1.get("account")));
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(183,115);to=(185,60)]
                out.write("\">\r\n            玩家角色\r\n            <input name=\"role\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(185,63);to=(185,107)]
                out.print(Util.setNullToEmpty((String)hs1.get("role")));
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(185,109);to=(211,16)]
                out.write("\">\r\n            <button onclick=\"Search()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作人</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">GM操作</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">期限/周期</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">游戏</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">区服</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家帐号/通行证</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家角色</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作内容/原因/公告内容</font></td>\r\n                      <td align=\"center\" nowrap ><font color=\"#FFFFFF\">瞬移位置</font></td>\r\n                      <td align=\"center\" nowrap ><font color=\"#FFFFFF\">备注</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(211,18);to=(277,8)]
                
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
                   int num=-1;
                
                   HttpSession se = request.getSession();
                
                   if (hs1.containsKey("pageAction")) {
                       String page1 = (String)hs1.get("pageAction");
                       if (page1.trim().equals("servlet")) {
                           se.setAttribute("gmfunctionlog", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("gmfunctionlog");
                       }
                       num = SearchHelperId.getGMFunctionLogNum(hs1);
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                  	if(num!=0){
                       lt = SearchHelperId.getGMFunctionLogList(hs1, curPageInfo[0], curPageInfo[2]);
                       }
                   }
                       String operatetime = "";
                         String usercode = "";
                         String username = "";
                         String script="";
                         String term="";
                         String gateway="";
                         String account = "";
                         String role = "";
                         String message = "";
                         String positionname = "";
                		 String remark="";
                		 String game_name="";
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                         HashMap hs = (HashMap)lt.get(i);
                         operatetime = (String)hs.get("operatetime");
                         usercode = Util.setNullToEmpty((String)hs.get("usercode"));
                         username = Util.setNullToEmpty((String)hs.get("username"));
                         script=Util.setNullToEmpty((String)hs.get("script"));
                         term=Util.setNullToEmpty((String)hs.get("term"));
                		 if(term.equals("0")){ term="";}
                		 if(!term.equals("")&&!term.equals("0")){
                		 	if(script.indexOf("公告")==-1){
                		 		term=term+"天";
                			}
                			else
                			{
                		 		term=term+"分钟";
                			}
                		 }
                         gateway=Util.setNullToEmpty((String)hs.get("gateway"));
                         game_name=Util.setNullToEmpty((String)hs.get("game_name"));
                         account = Util.setNullToEmpty((String)hs.get("account"));
                         role = Util.setNullToEmpty((String)hs.get("role"));
                         message = Util.setNullToEmpty((String)hs.get("message"));
                         positionname = Util.setNullToEmpty((String)hs.get("positionname"));
                         remark = Util.setNullToEmpty((String)hs.get("remark"));
                        
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(277,10);to=(278,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(278,26);to=(278,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(278,39);to=(278,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(278,59);to=(278,60)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(278,62);to=(279,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(279,51);to=(279,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(279,56);to=(280,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(280,44);to=(280,55)]
                out.print(operatetime);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(280,57);to=(281,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(281,51);to=(281,59)]
                out.print(usercode);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(281,61);to=(281,62)]
                out.write("(");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(281,65);to=(281,73)]
                out.print(username);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(281,75);to=(282,48)]
                out.write(")</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(282,51);to=(282,57)]
                out.print(script);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(282,59);to=(283,47)]
                out.write("</td>\r\n                      <td align=\"right\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(283,50);to=(283,54)]
                out.print(term);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(283,56);to=(284,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(284,51);to=(284,60)]
                out.print(game_name);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(284,62);to=(285,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(285,51);to=(285,58)]
                out.print(gateway);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(285,60);to=(286,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(286,51);to=(286,58)]
                out.print(account);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(286,60);to=(287,26)]
                out.write("</td>\r\n                      <td>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(287,29);to=(287,33)]
                out.print(role);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(287,35);to=(288,26)]
                out.write("</td>\r\n                      <td>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(288,29);to=(288,36)]
                out.print(message);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(288,38);to=(289,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(289,51);to=(289,63)]
                out.print(positionname);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(289,65);to=(290,33)]
                out.write("</td>\r\n                      <td nowrap>");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(290,36);to=(290,42)]
                out.print(remark);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(290,44);to=(292,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(292,22);to=(294,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(294,6);to=(305,7)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n       <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n       <tr>\r\n\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(305,9);to=(314,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(314,9);to=(316,35)]
                out.write("\r\n\r\n           <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(316,38);to=(316,41)]
                out.print(num);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(316,43);to=(316,57)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(316,60);to=(316,77)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(316,79);to=(316,80)]
                out.write("～");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(316,83);to=(316,108)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(316,110);to=(317,7)]
                out.write("条</td>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(317,9);to=(320,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(320,17);to=(321,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(321,38);to=(321,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(321,66);to=(322,35)]
                out.write("/search/gmfunctionlog.jsp?pageAction=first&start=\">第一页</a></td>\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(322,38);to=(322,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(322,66);to=(322,118)]
                out.write("/search/gmfunctionlog.jsp?pageAction=previous&start=");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(322,121);to=(322,144)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(322,146);to=(323,7)]
                out.write("\">上一页</a></td>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(323,9);to=(324,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(324,9);to=(327,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>第一页</u></td>\r\n            <td width=\"7%\"><u disabled>上一页</u></td>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(327,9);to=(330,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(330,16);to=(331,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(331,38);to=(331,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(331,66);to=(331,114)]
                out.write("/search/gmfunctionlog.jsp?pageAction=next&start=");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(331,117);to=(331,132)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(331,134);to=(332,36)]
                out.write("\">下一页</a></td>\r\n           <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(332,39);to=(332,65)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(332,67);to=(332,115)]
                out.write("/search/gmfunctionlog.jsp?pageAction=last&start=");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(332,118);to=(332,136)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(332,138);to=(333,7)]
                out.write("\">最后一页</a></td>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(333,9);to=(333,18)]
                 } else {
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(333,20);to=(336,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>下一页</u></td>\r\n            <td width=\"12%\"><u disabled>最后一页</u></td>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(336,9);to=(336,10)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(336,12);to=(339,7)]
                out.write("\r\n\r\n           <td width=\"10%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(339,9);to=(342,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(342,17);to=(343,15)]
                out.write("\r\n               ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(343,18);to=(343,32)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(343,34);to=(343,35)]
                out.write("/");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(343,38);to=(343,51)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(343,53);to=(344,7)]
                out.write("\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(344,9);to=(344,10)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(344,12);to=(347,7)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(347,9);to=(350,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(350,17);to=(351,53)]
                out.write("\r\n           转到第<INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(351,56);to=(351,70)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(351,72);to=(351,131)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(351,134);to=(351,158)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(351,160);to=(351,175)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(351,178);to=(351,183)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(351,185);to=(352,55)]
                out.write(")\">页\r\n           <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(352,58);to=(352,82)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(352,84);to=(352,99)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(352,102);to=(352,107)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(352,109);to=(353,7)]
                out.write(")\"> GO </button>\r\n       ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(353,9);to=(353,10)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(353,12);to=(355,78)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>页长设置为<INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(355,81);to=(355,86)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(355,88);to=(355,149)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(355,152);to=(355,176)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(355,178);to=(355,193)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(355,196);to=(355,213)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(355,215);to=(356,54)]
                out.write(")\">\r\n           <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(356,57);to=(356,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(356,83);to=(356,98)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(356,101);to=(356,118)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(356,120);to=(369,6)]
                out.write(")\">确定</button>\r\n           </td>\r\n         </tr>\r\n            </table>\r\n</td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(369,8);to=(369,19)]
                if(num==0){
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(369,21);to=(373,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/gmfunctionlog.jsp";from=(373,8);to=(373,9)]
                }
            // end
            // HTML // begin [file="/search/gmfunctionlog.jsp";from=(373,11);to=(382,0)]
                out.write("\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
