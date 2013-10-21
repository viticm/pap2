package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class gmcresultlog$jsp extends HttpJspBase {

    // begin [file="/gmc/gmcresultlog.jsp";from=(1,0);to=(1,107)]
    // end
    // begin [file="/gmc/gmcresultlog.jsp";from=(2,0);to=(2,102)]
    // end

    static {
    }
    public gmcresultlog$jsp( ) {
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

            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(1,0);to=(1,107)]
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
            // begin [file="/gmc/gmcresultlog.jsp";from=(1,0);to=(1,107)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(1,107);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(2,0);to=(2,102)]
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
            // begin [file="/gmc/gmcresultlog.jsp";from=(2,0);to=(2,102)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(2,102);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(3,57);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(4,56);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(5,0);to=(5,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("gmcresultlog");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/gmcresultlog.jsp";from=(5,0);to=(5,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(5,46);to=(12,13)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>多区服操作执行情况日志记录</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(12,16);to=(12,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(12,42);to=(15,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(15,2);to=(18,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(18,2);to=(107,73)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/gmc/gmcresultlog.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/gmc/gmcresultlog.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\n\r\nfunction Search()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"operatetime\")) strErr+=\"*操作时间不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar operatetime=document.all(\"operatetime\").value;\r\n\t\tvar usercode=document.all(\"usercode\").value;\r\n\t\tvar strUrl=AbsPath+\"/gmc/gmcresultlog.jsp?pageAction=servlet&step=&start=&operatetime=\"+operatetime+\"&usercode=\"+usercode;\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">多区服操作执行情况日志记录</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n\t<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n\t<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n\t</div>\r\n\t<br>\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td>&nbsp;&nbsp;&nbsp;&nbsp;操作时间\r\n                  <input name=\"operatetime\" type=\"text\" size=\"21\" value=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(107,76);to=(107,132)]
                out.print(Util.setNullToEmpty(request.getParameter("operatetime")));
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(107,134);to=(109,70)]
                out.write("\">\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('operatetime')\" style=\"cursor:hand \">操作人\r\n                  <input name=\"usercode\" type=\"text\" size=\"20\" value=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(109,73);to=(109,121)]
                out.print(Util.setNullToEmpty((String)hs1.get("usercode")));
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(109,123);to=(127,16)]
                out.write("\">\r\n            <button onclick=\"Search()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作类型</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作人</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">操作时间</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(127,18);to=(169,8)]
                
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
                           se.setAttribute("gmcresultlog", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("gmcresultlog");
                       }
                       num = SearchHelperId.getGMCCommandResultNum(hs1);
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                  	if(num!=0){
                       lt = SearchHelperId.getGMCCommandResultList(hs1, curPageInfo[0], curPageInfo[2]);
                       }
                   }
                         String iid = "";
                         String operatetime = "";
                         String operator = "";
                         String serialnumber = "";
                         String operatetype="";
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                         HashMap hs = (HashMap)lt.get(i);
                         iid = (String)hs.get("iid");
                         operatetime = (String)hs.get("operatetime");
                         operator = Util.setNullToEmpty((String)hs.get("operator"));
                         serialnumber = Util.setNullToEmpty((String)hs.get("serialnumber"));
                         operatetype=Util.setNullToEmpty((String)hs.get("operatetype"));
                        
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(169,10);to=(170,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(170,26);to=(170,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(170,39);to=(170,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(170,59);to=(170,60)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(170,62);to=(171,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(171,51);to=(171,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(171,56);to=(172,96)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap><a href=\"/gmc/processresult.jsp?seltype=iid&iid=");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(172,99);to=(172,102)]
                out.print(iid);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(172,104);to=(172,106)]
                out.write("\">");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(172,109);to=(172,120)]
                out.print(operatetype);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(172,122);to=(173,48)]
                out.write("</a></td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(173,51);to=(173,59)]
                out.print(operator);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(173,61);to=(174,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(174,51);to=(174,62)]
                out.print(operatetime);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(174,64);to=(176,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(176,22);to=(178,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(178,6);to=(188,7)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n       <tr>\r\n\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(188,9);to=(197,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(197,9);to=(199,35)]
                out.write("\r\n\r\n           <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(199,38);to=(199,41)]
                out.print(num);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(199,43);to=(199,57)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(199,60);to=(199,77)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(199,79);to=(199,80)]
                out.write("～");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(199,83);to=(199,108)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(199,110);to=(200,7)]
                out.write("条</td>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(200,9);to=(203,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(203,17);to=(204,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(204,38);to=(204,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(204,66);to=(205,35)]
                out.write("/gmc/gmcresultlog.jsp?pageAction=first&start=\">第一页</a></td>\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(205,38);to=(205,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(205,66);to=(205,114)]
                out.write("/gmc/gmcresultlog.jsp?pageAction=previous&start=");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(205,117);to=(205,140)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(205,142);to=(206,7)]
                out.write("\">上一页</a></td>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(206,9);to=(207,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(207,9);to=(210,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>第一页</u></td>\r\n            <td width=\"7%\"><u disabled>上一页</u></td>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(210,9);to=(213,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(213,16);to=(214,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(214,38);to=(214,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(214,66);to=(214,110)]
                out.write("/gmc/gmcresultlog.jsp?pageAction=next&start=");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(214,113);to=(214,128)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(214,130);to=(215,36)]
                out.write("\">下一页</a></td>\r\n           <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(215,39);to=(215,65)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(215,67);to=(215,111)]
                out.write("/gmc/gmcresultlog.jsp?pageAction=last&start=");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(215,114);to=(215,132)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(215,134);to=(216,7)]
                out.write("\">最后一页</a></td>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(216,9);to=(216,18)]
                 } else {
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(216,20);to=(219,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>下一页</u></td>\r\n            <td width=\"12%\"><u disabled>最后一页</u></td>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(219,9);to=(219,10)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(219,12);to=(222,7)]
                out.write("\r\n\r\n           <td width=\"10%\" nowrap>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(222,9);to=(225,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(225,17);to=(226,15)]
                out.write("\r\n               ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(226,18);to=(226,32)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(226,34);to=(226,35)]
                out.write("/");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(226,38);to=(226,51)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(226,53);to=(227,7)]
                out.write("\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(227,9);to=(227,10)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(227,12);to=(230,7)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(230,9);to=(233,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(233,17);to=(234,53)]
                out.write("\r\n           转到第<INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(234,56);to=(234,70)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(234,72);to=(234,131)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(234,134);to=(234,158)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(234,160);to=(234,175)]
                out.write("', page.value, ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(234,178);to=(234,183)]
                out.print(nStep);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(234,185);to=(235,55)]
                out.write(")\">页\r\n           <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(235,58);to=(235,82)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(235,84);to=(235,99)]
                out.write("', page.value, ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(235,102);to=(235,107)]
                out.print(nStep);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(235,109);to=(236,7)]
                out.write(")\"> GO </button>\r\n       ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(236,9);to=(236,10)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(236,12);to=(238,78)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>页长设置为<INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(238,81);to=(238,86)]
                out.print(nStep);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(238,88);to=(238,149)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(238,152);to=(238,176)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(238,178);to=(238,193)]
                out.write("', step.value, ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(238,196);to=(238,213)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(238,215);to=(239,54)]
                out.write(")\">\r\n           <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(239,57);to=(239,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(239,83);to=(239,98)]
                out.write("', step.value, ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(239,101);to=(239,118)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(239,120);to=(252,6)]
                out.write(")\">确定</button>\r\n           </td>\r\n         </tr>\r\n            </table>\r\n</td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(252,8);to=(252,19)]
                if(num==0){
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(252,21);to=(256,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/gmc/gmcresultlog.jsp";from=(256,8);to=(256,9)]
                }
            // end
            // HTML // begin [file="/gmc/gmcresultlog.jsp";from=(256,11);to=(265,0)]
                out.write("\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
