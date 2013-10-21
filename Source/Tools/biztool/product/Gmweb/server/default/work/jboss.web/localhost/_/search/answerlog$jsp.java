package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.exception.ServiceException;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class answerlog$jsp extends HttpJspBase {

    // begin [file="/search/answerlog.jsp";from=(1,0);to=(1,107)]
    // end
    // begin [file="/search/answerlog.jsp";from=(2,0);to=(2,102)]
    // end

    static {
    }
    public answerlog$jsp( ) {
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

            // HTML // begin [file="/search/answerlog.jsp";from=(0,210);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/answerlog.jsp";from=(1,0);to=(1,107)]
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
            // begin [file="/search/answerlog.jsp";from=(1,0);to=(1,107)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(1,107);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/answerlog.jsp";from=(2,0);to=(2,102)]
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
            // begin [file="/search/answerlog.jsp";from=(2,0);to=(2,102)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(2,102);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(3,57);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(4,56);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/answerlog.jsp";from=(5,0);to=(5,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("answerlog");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/answerlog.jsp";from=(5,0);to=(5,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(5,43);to=(14,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>答复记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(14,42);to=(18,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/answerlog.jsp";from=(18,2);to=(23,0)]
                
                	if(null==session.getAttribute("userinfo"))
                		throw new ServiceException("请重新登录系统");
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(23,2);to=(129,70)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/search/answerlog.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/search/answerlog.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"gateway\");\r\n\tdocument.all(\"game_id\").value=objSel.options[objSel.selectedIndex].game_id;\r\n\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n}\r\n\r\nfunction Search()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"quiztime\")) strErr+=\"*提问时间不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tsetGatewayInfo();\r\n\t\tvar quiztime=document.all(\"quiztime\").value;\r\n\t\tvar usercode=document.all(\"usercode\").value;\r\n\t\tvar state=document.all(\"state\").value;\r\n\t\tvar game_id=document.all(\"game_id\").value;\r\n\t\tvar area=document.all(\"area\").value;\r\n\t\tvar group=document.all(\"group\").value;\r\n\t\tvar account=document.all(\"account\").value;\r\n\t\tvar role=document.all(\"role\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/answerlog.jsp?pageAction=servlet&step=&start=&quiztime=\"+quiztime+\r\n\t\t\t\t\"&usercode=\"+usercode+\"&state=\"+state+\r\n\t\t\t\t\"&game_id=\"+game_id+\"&area=\"+area+\"&group=\"+group+\"&account=\"+account+\"&role=\"+role;\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">问题答复记录</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td>&nbsp;&nbsp;&nbsp;&nbsp;提问时间\r\n                  <input name=\"quiztime\" type=\"text\" size=\"21\" value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(129,73);to=(129,126)]
                out.print(Util.setNullToEmpty(request.getParameter("quiztime")));
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(129,128);to=(131,70)]
                out.write("\">\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('quiztime')\" style=\"cursor:hand \">操作人\r\n                  <input name=\"usercode\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(131,73);to=(131,126)]
                out.print(Util.setNullToEmpty(request.getParameter("usercode")));
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(131,128);to=(141,58)]
                out.write("\">\r\n                  问题状态\r\n                  <select name=\"state\">\r\n                    <option value=\"all\">所有状态</option>\r\n                    <option value=\"0\">1.已答复</option>\r\n                    <option value=\"1\">2.已删除</option>\r\n                  </select>\r\n                </td>\r\n              </tr>\r\n              <tr>\r\n                <td nowrap>  &nbsp;&nbsp;&nbsp;&nbsp;所在区服 ");

            // end
            // begin [file="/search/answerlog.jsp";from=(141,58);to=(141,150)]
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
                        // HTML // begin [file="/search/answerlog.jsp";from=(141,150);to=(142,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/search/answerlog.jsp";from=(142,12);to=(142,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(142,30);to=(146,63)]
                out.write("            <input type=\"hidden\" name=\"game_id\">\r\n            <input type=\"hidden\" name=\"area\">\r\n            <input type=\"hidden\" name=\"group\">\r\n玩家帐号\r\n            <input name=\"account\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(146,66);to=(146,113)]
                out.print(Util.setNullToEmpty((String)hs1.get("account")));
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(146,115);to=(148,60)]
                out.write("\">\r\n            玩家角色\r\n            <input name=\"role\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(148,63);to=(148,107)]
                out.print(Util.setNullToEmpty((String)hs1.get("role")));
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(148,109);to=(172,16)]
                out.write("\">\r\n            <button onclick=\"Search()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">提问时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作人</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">问题状态</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">所在区服</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家帐号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家角色</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">问题内容</font></td>\r\n                      <td align=\"center\" nowrap ><font color=\"#FFFFFF\">答复内容</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/answerlog.jsp";from=(172,18);to=(221,8)]
                
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
                           se.setAttribute("answerlog", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("answerlog");
                       }
                
                       num = SearchHelperId.getAnswerLogNum(hs1);
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       if(num!=0){
                       lt = SearchHelperId.getAnswerLogList(hs1, curPageInfo[0], curPageInfo[2]);
                       }
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                         HashMap hs = (HashMap)lt.get(i);
                         String quiztime = (String)hs.get("quiztime");
                         String usercode = Util.setNullToEmpty((String)hs.get("usercode"));
                         String username = Util.setNullToEmpty((String)hs.get("username"));
                         String state=Util.setNullToEmpty((String)hs.get("state"));
                         if(state.equals("0")){
                           state="已答复";
                           }
                         else if(state.equals("1")){
                           state="已删除";
                           }
                         String answertime=Util.setNullToEmpty((String)hs.get("answertime"));
                         String gateway=Util.setNullToEmpty((String)hs.get("gateway"));
                         String account = Util.setNullToEmpty((String)hs.get("account"));
                         String role = Util.setNullToEmpty((String)hs.get("role"));
                         String question = Util.setNullToEmpty((String)hs.get("question"));
                         String answer = Util.setNullToEmpty((String)hs.get("answer"));
                        
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(221,10);to=(222,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/answerlog.jsp";from=(222,26);to=(222,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(222,39);to=(222,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(222,59);to=(222,60)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(222,62);to=(223,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(223,51);to=(223,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(223,56);to=(224,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/answerlog.jsp";from=(224,44);to=(224,52)]
                out.print(quiztime);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(224,54);to=(225,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/answerlog.jsp";from=(225,44);to=(225,54)]
                out.print(answertime);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(225,56);to=(226,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(226,51);to=(226,59)]
                out.print(usercode);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(226,61);to=(226,62)]
                out.write("(");

            // end
            // begin [file="/search/answerlog.jsp";from=(226,65);to=(226,73)]
                out.print(username);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(226,75);to=(227,48)]
                out.write(")</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(227,51);to=(227,56)]
                out.print(state);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(227,58);to=(228,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(228,51);to=(228,58)]
                out.print(gateway);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(228,60);to=(229,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(229,51);to=(229,58)]
                out.print(account);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(229,60);to=(230,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/answerlog.jsp";from=(230,51);to=(230,55)]
                out.print(role);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(230,57);to=(231,26)]
                out.write("</td>\r\n                      <td>");

            // end
            // begin [file="/search/answerlog.jsp";from=(231,29);to=(231,37)]
                out.print(question);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(231,39);to=(232,26)]
                out.write("</td>\r\n                      <td>");

            // end
            // begin [file="/search/answerlog.jsp";from=(232,29);to=(232,35)]
                out.print(answer);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(232,37);to=(234,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/answerlog.jsp";from=(234,22);to=(236,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(236,6);to=(247,7)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n       <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n       <tr>\r\n\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(247,9);to=(256,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(256,9);to=(258,35)]
                out.write("\r\n\r\n           <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/search/answerlog.jsp";from=(258,38);to=(258,41)]
                out.print(num);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(258,43);to=(258,57)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/search/answerlog.jsp";from=(258,60);to=(258,77)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(258,79);to=(258,80)]
                out.write("～");

            // end
            // begin [file="/search/answerlog.jsp";from=(258,83);to=(258,108)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(258,110);to=(259,7)]
                out.write("条</td>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(259,9);to=(262,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(262,17);to=(263,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(263,38);to=(263,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(263,66);to=(264,35)]
                out.write("/search/answerlog.jsp?pageAction=first&start=\">第一页</a></td>\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(264,38);to=(264,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(264,66);to=(264,114)]
                out.write("/search/answerlog.jsp?pageAction=previous&start=");

            // end
            // begin [file="/search/answerlog.jsp";from=(264,117);to=(264,140)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(264,142);to=(265,7)]
                out.write("\">上一页</a></td>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(265,9);to=(266,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(266,9);to=(269,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>第一页</u></td>\r\n            <td width=\"7%\"><u disabled>上一页</u></td>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(269,9);to=(272,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(272,16);to=(273,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(273,38);to=(273,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(273,66);to=(273,110)]
                out.write("/search/answerlog.jsp?pageAction=next&start=");

            // end
            // begin [file="/search/answerlog.jsp";from=(273,113);to=(273,128)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(273,130);to=(274,36)]
                out.write("\">下一页</a></td>\r\n           <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(274,39);to=(274,65)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(274,67);to=(274,111)]
                out.write("/search/answerlog.jsp?pageAction=last&start=");

            // end
            // begin [file="/search/answerlog.jsp";from=(274,114);to=(274,132)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(274,134);to=(275,7)]
                out.write("\">最后一页</a></td>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(275,9);to=(275,18)]
                 } else {
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(275,20);to=(278,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>下一页</u></td>\r\n            <td width=\"12%\"><u disabled>最后一页</u></td>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(278,9);to=(278,10)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(278,12);to=(281,7)]
                out.write("\r\n\r\n           <td width=\"10%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(281,9);to=(284,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(284,17);to=(285,15)]
                out.write("\r\n               ");

            // end
            // begin [file="/search/answerlog.jsp";from=(285,18);to=(285,32)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(285,34);to=(285,35)]
                out.write("/");

            // end
            // begin [file="/search/answerlog.jsp";from=(285,38);to=(285,51)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(285,53);to=(286,7)]
                out.write("\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(286,9);to=(286,10)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(286,12);to=(289,7)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(289,9);to=(292,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(292,17);to=(293,53)]
                out.write("\r\n           转到第<INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(293,56);to=(293,70)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(293,72);to=(293,131)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/search/answerlog.jsp";from=(293,134);to=(293,158)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(293,160);to=(293,175)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/answerlog.jsp";from=(293,178);to=(293,183)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(293,185);to=(294,55)]
                out.write(")\">页\r\n           <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/search/answerlog.jsp";from=(294,58);to=(294,82)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(294,84);to=(294,99)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/answerlog.jsp";from=(294,102);to=(294,107)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(294,109);to=(295,7)]
                out.write(")\"> GO </button>\r\n       ");

            // end
            // begin [file="/search/answerlog.jsp";from=(295,9);to=(295,10)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(295,12);to=(297,78)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>页长设置为<INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/search/answerlog.jsp";from=(297,81);to=(297,86)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(297,88);to=(297,149)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/search/answerlog.jsp";from=(297,152);to=(297,176)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(297,178);to=(297,193)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/answerlog.jsp";from=(297,196);to=(297,213)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(297,215);to=(298,54)]
                out.write(")\">\r\n           <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/search/answerlog.jsp";from=(298,57);to=(298,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(298,83);to=(298,98)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/answerlog.jsp";from=(298,101);to=(298,118)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(298,120);to=(311,6)]
                out.write(")\">确定</button>\r\n           </td>\r\n         </tr>\r\n            </table>\r\n</td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/answerlog.jsp";from=(311,8);to=(311,19)]
                if(num==0){
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(311,21);to=(315,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/answerlog.jsp";from=(315,8);to=(315,9)]
                }
            // end
            // HTML // begin [file="/search/answerlog.jsp";from=(315,11);to=(324,0)]
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
