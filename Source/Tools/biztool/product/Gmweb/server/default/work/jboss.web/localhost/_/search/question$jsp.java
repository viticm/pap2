package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class question$jsp extends HttpJspBase {

    // begin [file="/search/question.jsp";from=(1,0);to=(1,107)]
    // end
    // begin [file="/search/question.jsp";from=(2,0);to=(2,102)]
    // end

    static {
    }
    public question$jsp( ) {
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

            // HTML // begin [file="/search/question.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/question.jsp";from=(1,0);to=(1,107)]
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
            // begin [file="/search/question.jsp";from=(1,0);to=(1,107)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(1,107);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/question.jsp";from=(2,0);to=(2,102)]
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
            // begin [file="/search/question.jsp";from=(2,0);to=(2,102)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(2,102);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/question.jsp";from=(3,57);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/question.jsp";from=(4,56);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/question.jsp";from=(5,0);to=(5,42)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("question");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/question.jsp";from=(5,0);to=(5,42)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(5,42);to=(14,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>未答复问题查询</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/question.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/question.jsp";from=(14,42);to=(18,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/question.jsp";from=(18,2);to=(21,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/search/question.jsp";from=(21,2);to=(148,86)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\nvar game_id='';\r\nvar area='';\r\nvar group='';\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/search/question.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/search/question.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tif(objSel.selectedIndex!=-1){\r\n\t\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\t\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t}\r\n\t}\r\n}\r\n\r\nfunction Search()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"quiztime\")) strErr+=\"*提问时间不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tsetGatewayInfo();\r\n\t\tvar quiztime=document.all(\"quiztime\").value;\r\n\t\tvar keyword=document.all(\"keyword\").value;\r\n\t\tvar game_id=document.all(\"game_id\").value;\r\n\t\tvar area=document.all(\"area\").value;\r\n\t\tvar group=document.all(\"group\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/question.jsp?pageAction=servlet&step=&start=&quiztime=\"+quiztime+\r\n\t\t\t\t\"&keyword=\"+keyword+\"&game_id=\"+game_id+\"&area=\"+area+\"&group=\"+group;\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">未答复问题查询</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap>\r\n\t\t  <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t    <tr>\r\n\t\t\t  <td width=\"13%\" align=\"right\"><font color=\"#FF0000\">*</font>提问时间：</td>\r\n\t\t\t  <td width=\"25%\" align=\"left\"><input name=\"quiztime\" type=\"text\" size=\"21\" value=\"");

            // end
            // begin [file="/search/question.jsp";from=(148,89);to=(148,142)]
                out.print(Util.setNullToEmpty(request.getParameter("quiztime")));
            // end
            // HTML // begin [file="/search/question.jsp";from=(148,144);to=(151,85)]
                out.write("\">\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('quiztime')\" style=\"cursor:hand \"></td>\r\n\t\t\t  <td width=\"13%\" align=\"right\">关键字：</td>\r\n\t\t\t  <td width=\"25%\" align=\"left\"><input name=\"keyword\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/search/question.jsp";from=(151,88);to=(151,135)]
                out.print(Util.setNullToEmpty((String)hs1.get("keyword")));
            // end
            // HTML // begin [file="/search/question.jsp";from=(151,137);to=(162,12)]
                out.write("\"></td>\r\n\t\t\t  <td width=\"13%\" align=\"right\"></td>\r\n\t\t\t  <td align=\"left\"></td></tr>\r\n\t\t    <tr>\r\n\t\t\t  <td align=\"right\">所属游戏：</td>\r\n\t\t\t  <td align=\"left\"><select name=\"game_id\" onchange=\"onChangeGameId()\" style=\"width:160\">\r\n\t\t\t        <option value=\"77\">7.剑侠世界</option>\r\n\t\t\t        <option value=\"88\">8.剑网3</option>\r\n                  </select></td>\r\n\t\t\t  <td align=\"right\">所在区服：</td>\r\n\t\t\t  <td align=\"left\"><select name=\"selGateway\" style='width:180'></select>\r\n            ");

            // end
            // begin [file="/search/question.jsp";from=(162,12);to=(162,120)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_gateway_person_all");
                _jspx_th_display_display_0.setProperty("selGatewayTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/search/question.jsp";from=(162,120);to=(163,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/search/question.jsp";from=(163,12);to=(163,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(163,30);to=(184,16)]
                out.write("</td>\r\n            <input type=\"hidden\" name=\"area\">\r\n            <input type=\"hidden\" name=\"group\">\r\n\t\t\t  <td colspan=\"2\" align=\"left\"><button onclick=\"Search()\">查询</button></td></tr>\r\n\t\t  </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" width=\"7%\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" width=\"20%\" nowrap><font color=\"#FFFFFF\">提问时间</font></td>\r\n                      <td align=\"center\" width=\"13%\" nowrap><font color=\"#FFFFFF\">所在区服</font></td>\r\n                      <td align=\"center\" width=\"12%\" nowrap><font color=\"#FFFFFF\">玩家帐号</font></td>\r\n                      <td align=\"center\" width=\"13%\" nowrap><font color=\"#FFFFFF\">玩家角色</font></td>\r\n                      <td align=\"center\" width=\"35%\" nowrap><font color=\"#FFFFFF\">问题内容</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/question.jsp";from=(184,18);to=(222,8)]
                
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
                           se.setAttribute("question", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("question");
                       }
                
                       num = SearchHelperId.getQuestionNum(hs1);
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       if(num!=0){
                       lt = SearchHelperId.getQuestionList(hs1, curPageInfo[0], curPageInfo[2]);
                       }
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                         HashMap hs = (HashMap)lt.get(i);
                         String quiztime = (String)hs.get("quiztime");
                         String gateway=Util.setNullToEmpty((String)hs.get("gateway"));
                         String account = Util.setNullToEmpty((String)hs.get("account"));
                         String role = Util.setNullToEmpty((String)hs.get("role"));
                         String question = Util.setNullToEmpty((String)hs.get("question"));
                        
            // end
            // HTML // begin [file="/search/question.jsp";from=(222,10);to=(223,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/question.jsp";from=(223,26);to=(223,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/question.jsp";from=(223,39);to=(223,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/question.jsp";from=(223,59);to=(223,60)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(223,62);to=(224,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/question.jsp";from=(224,51);to=(224,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/question.jsp";from=(224,56);to=(225,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/question.jsp";from=(225,44);to=(225,52)]
                out.print(quiztime);
            // end
            // HTML // begin [file="/search/question.jsp";from=(225,54);to=(226,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/question.jsp";from=(226,51);to=(226,58)]
                out.print(gateway);
            // end
            // HTML // begin [file="/search/question.jsp";from=(226,60);to=(227,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/question.jsp";from=(227,51);to=(227,58)]
                out.print(account);
            // end
            // HTML // begin [file="/search/question.jsp";from=(227,60);to=(228,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/question.jsp";from=(228,51);to=(228,55)]
                out.print(role);
            // end
            // HTML // begin [file="/search/question.jsp";from=(228,57);to=(229,26)]
                out.write("</td>\r\n                      <td>");

            // end
            // begin [file="/search/question.jsp";from=(229,29);to=(229,37)]
                out.print(question);
            // end
            // HTML // begin [file="/search/question.jsp";from=(229,39);to=(231,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/question.jsp";from=(231,22);to=(233,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/question.jsp";from=(233,6);to=(244,7)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n       <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n       <tr>\r\n\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(244,9);to=(253,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/search/question.jsp";from=(253,9);to=(255,35)]
                out.write("\r\n\r\n           <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/search/question.jsp";from=(255,38);to=(255,41)]
                out.print(num);
            // end
            // HTML // begin [file="/search/question.jsp";from=(255,43);to=(255,57)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/search/question.jsp";from=(255,60);to=(255,77)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/question.jsp";from=(255,79);to=(255,80)]
                out.write("～");

            // end
            // begin [file="/search/question.jsp";from=(255,83);to=(255,108)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/search/question.jsp";from=(255,110);to=(256,7)]
                out.write("条</td>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(256,9);to=(259,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/search/question.jsp";from=(259,17);to=(260,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/question.jsp";from=(260,38);to=(260,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/question.jsp";from=(260,66);to=(261,35)]
                out.write("/search/question.jsp?pageAction=first&start=\">第一页</a></td>\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/question.jsp";from=(261,38);to=(261,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/question.jsp";from=(261,66);to=(261,113)]
                out.write("/search/question.jsp?pageAction=previous&start=");

            // end
            // begin [file="/search/question.jsp";from=(261,116);to=(261,139)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/search/question.jsp";from=(261,141);to=(262,7)]
                out.write("\">上一页</a></td>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(262,9);to=(263,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/search/question.jsp";from=(263,9);to=(266,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>第一页</u></td>\r\n            <td width=\"7%\"><u disabled>上一页</u></td>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(266,9);to=(269,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/search/question.jsp";from=(269,16);to=(270,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/question.jsp";from=(270,38);to=(270,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/question.jsp";from=(270,66);to=(270,109)]
                out.write("/search/question.jsp?pageAction=next&start=");

            // end
            // begin [file="/search/question.jsp";from=(270,112);to=(270,127)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/search/question.jsp";from=(270,129);to=(271,36)]
                out.write("\">下一页</a></td>\r\n           <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/search/question.jsp";from=(271,39);to=(271,65)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/question.jsp";from=(271,67);to=(271,110)]
                out.write("/search/question.jsp?pageAction=last&start=");

            // end
            // begin [file="/search/question.jsp";from=(271,113);to=(271,131)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/search/question.jsp";from=(271,133);to=(272,7)]
                out.write("\">最后一页</a></td>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(272,9);to=(272,18)]
                 } else {
            // end
            // HTML // begin [file="/search/question.jsp";from=(272,20);to=(275,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>下一页</u></td>\r\n            <td width=\"12%\"><u disabled>最后一页</u></td>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(275,9);to=(275,10)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(275,12);to=(278,7)]
                out.write("\r\n\r\n           <td width=\"10%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(278,9);to=(281,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/question.jsp";from=(281,17);to=(282,15)]
                out.write("\r\n               ");

            // end
            // begin [file="/search/question.jsp";from=(282,18);to=(282,32)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/question.jsp";from=(282,34);to=(282,35)]
                out.write("/");

            // end
            // begin [file="/search/question.jsp";from=(282,38);to=(282,51)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/search/question.jsp";from=(282,53);to=(283,7)]
                out.write("\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(283,9);to=(283,10)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(283,12);to=(286,7)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(286,9);to=(289,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/question.jsp";from=(289,17);to=(290,53)]
                out.write("\r\n           转到第<INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/search/question.jsp";from=(290,56);to=(290,70)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/question.jsp";from=(290,72);to=(290,131)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/search/question.jsp";from=(290,134);to=(290,158)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/question.jsp";from=(290,160);to=(290,175)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/question.jsp";from=(290,178);to=(290,183)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/question.jsp";from=(290,185);to=(291,55)]
                out.write(")\">页\r\n           <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/search/question.jsp";from=(291,58);to=(291,82)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/question.jsp";from=(291,84);to=(291,99)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/question.jsp";from=(291,102);to=(291,107)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/question.jsp";from=(291,109);to=(292,7)]
                out.write(")\"> GO </button>\r\n       ");

            // end
            // begin [file="/search/question.jsp";from=(292,9);to=(292,10)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(292,12);to=(294,78)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>页长设置为<INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/search/question.jsp";from=(294,81);to=(294,86)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/question.jsp";from=(294,88);to=(294,149)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/search/question.jsp";from=(294,152);to=(294,176)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/question.jsp";from=(294,178);to=(294,193)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/question.jsp";from=(294,196);to=(294,213)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/question.jsp";from=(294,215);to=(295,54)]
                out.write(")\">\r\n           <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/search/question.jsp";from=(295,57);to=(295,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/question.jsp";from=(295,83);to=(295,98)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/question.jsp";from=(295,101);to=(295,118)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/question.jsp";from=(295,120);to=(308,6)]
                out.write(")\">确定</button>\r\n           </td>\r\n         </tr>\r\n            </table>\r\n</td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/question.jsp";from=(308,8);to=(308,19)]
                if(num==0){
            // end
            // HTML // begin [file="/search/question.jsp";from=(308,21);to=(312,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/question.jsp";from=(312,8);to=(312,9)]
                }
            // end
            // HTML // begin [file="/search/question.jsp";from=(312,11);to=(321,10)]
                out.write("\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n<script language=\"javascript\">\r\n\r\n\tgame_id='");

            // end
            // begin [file="/search/question.jsp";from=(321,13);to=(321,60)]
                out.print(Util.setNullToEmpty((String)hs1.get("game_id")));
            // end
            // HTML // begin [file="/search/question.jsp";from=(321,62);to=(322,7)]
                out.write("';\r\n\tarea='");

            // end
            // begin [file="/search/question.jsp";from=(322,10);to=(322,54)]
                out.print(Util.setNullToEmpty((String)hs1.get("area")));
            // end
            // HTML // begin [file="/search/question.jsp";from=(322,56);to=(323,8)]
                out.write("';\r\n\tgroup='");

            // end
            // begin [file="/search/question.jsp";from=(323,11);to=(323,56)]
                out.print(Util.setNullToEmpty((String)hs1.get("group")));
            // end
            // HTML // begin [file="/search/question.jsp";from=(323,58);to=(342,0)]
                out.write("';\r\n\tif(game_id!=''){\r\n\tdocument.all(\"game_id\").value=game_id;\r\n\tonChangeGameId();\r\n\tvar objOption2;\r\n\tfor(var k=0;k<document.all(\"selGateway\").length;k++)\r\n\t{\r\n\t\tobjOption2=document.all(\"selGateway\").options[k];\r\n\t\tif(objOption2.area==area&&objOption2.group==group)\r\n\t\t{\r\n\t\t\tobjOption2.selected=true;\r\n\t\t}\r\n\t}\r\n\t}else{\r\n\tonChangeGameId();\r\n\t}\r\n</script>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
