package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class roleresume$jsp extends HttpJspBase {

    // begin [file="/search/roleresume.jsp";from=(1,0);to=(1,107)]
    // end
    // begin [file="/search/roleresume.jsp";from=(2,0);to=(2,102)]
    // end

    static {
    }
    public roleresume$jsp( ) {
    }

    private static boolean _jspx_inited = false;

    public final void _jspx_init() throws org.apache.jasper.runtime.JspException {
    }

    public void _jspService(HttpServletRequest request, HttpServletResponse  response)
        throws java.io.IOException, ServletException {

        JspFactory _jspxFactory = null;
        PageContext pageContext = null;
        HttpSession session = null;
        Throwable exception = (Throwable) request.getAttribute("javax.servlet.jsp.jspException");
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

            // HTML // begin [file="/search/roleresume.jsp";from=(0,180);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/roleresume.jsp";from=(1,0);to=(1,107)]
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
            // begin [file="/search/roleresume.jsp";from=(1,0);to=(1,107)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(1,107);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/roleresume.jsp";from=(2,0);to=(2,102)]
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
            // begin [file="/search/roleresume.jsp";from=(2,0);to=(2,102)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(2,102);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(3,57);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(4,56);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/roleresume.jsp";from=(5,0);to=(5,44)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("roleresume");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/roleresume.jsp";from=(5,0);to=(5,44)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(5,44);to=(12,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>查询角色恢复记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(12,15);to=(12,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(12,41);to=(14,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(14,42);to=(16,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(16,38);to=(16,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(16,64);to=(18,0)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/roleresume.jsp";from=(18,2);to=(21,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(21,2);to=(189,16)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\nvar objChk=null;//当前被选中的\r\n/******************************\r\n选择\r\n******************************/\r\nfunction onSelect()\r\n{\r\n\tif(objChk) objChk.checked=false;\r\n\tvar objCurChk=event.srcElement;\r\n\tif(objCurChk.checked=true)\r\n\t{\r\n\t \tobjChk=objCurChk;\r\n\t\tdocument.all(\"iid\").value=objCurChk.iid;\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t\tdocument.all(\"operationflag\").disabled=false;\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjChk=null;\r\n\t\tdocument.all(\"iid\").value=\"\";\r\n\t\tdocument.all(\"btnModify\").disabled=true;\r\n\t\tdocument.all(\"operationflag\").disabled=true;\r\n\t}\r\n}\r\n\r\n/********************************\r\n修改状态\r\n********************************/\r\nfunction onModify()\r\n{\r\n\tif(objChk.operationflag==document.all(\"operationflag\").value)\r\n\t{\r\n\t\talert(\"新状态不能与原状态相同！\");\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"frmModifyState\").submit();\r\n\t}\r\n}\r\n\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction CheckNum(strNum)\r\n{\r\n var code = event.keyCode;\r\n if(!(47<event.keyCode&&event.keyCode<58))\r\n  {\r\n      event.keyCode=null;\r\n\r\n    }else {\r\n    var strValue = document.all(strNum).value\r\n    if (event.keyCode == 48){\r\n        if (strValue == \"\"){\r\n         event.keyCode=null;\r\n        }\r\n    }\r\n}\r\n}\r\n\r\nfunction goClick(pPath, pPage, pStep) {\r\n   var strPage = pPage.substring(0,1);\r\n   var code = event.keyCode;\r\n   if (strPage == \"0\"){\r\n\r\n       alert(\"请输入合法数据！\");\r\n       page.focus();\r\n       return;\r\n   }//输入合法数据\r\n      var pStart = (pPage*1 - 1) * pStep;\r\n      if (code == 13 || code == 0){\r\n      location.href = pPath+\"/search/roleresume.jsp?pageAction=find&ppage=\"+pPage+\"&start=\" + pStart;\r\n }}\r\n\r\nfunction stepClick(pPath, nStep, curPageStartIndex) {\r\n\r\n   var strStep = nStep.substring(0,1);\r\n   var code = event.keyCode;\r\n  if (strStep == \"0\"){\r\n\r\n      alert(\"请输入合法数据！\")\r\n      step.focus();\r\n      return;\r\n  }\r\n  if (code == 13 || code == 0){\r\n     location.href = pPath+\"/search/roleresume.jsp?pageAction=pageStep&step=\"+nStep+\"&start=\"+curPageStartIndex;\r\n}\r\n}\r\n\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsNull(document.all(\"iid_Search\"))&&IsNull(document.all(\"name\"))&&IsNull(document.all(\"commitedate\"))) strErr+=\"*服务编号、玩家帐号、查询期间不能全为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar iid=document.all(\"iid_Search\").value;\r\n\t\tvar game_id=document.all(\"gameid\").value;\r\n\t\tvar commitedate=document.all(\"commitedate\").value;\r\n\t\tvar name=document.all(\"name\").value;\r\n\t\tvar operationflag=document.all(\"selOperationFlag\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/roleresume.jsp?pageAction=servlet&step=&start=&commitedate=\"+commitedate+\r\n\t\t\t\t\"&name=\"+name+\"&game_id=\"+game_id+\"&iid=\"+iid+\"&operationflag=\"+operationflag;\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">角色恢复</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td width=\"10\">&nbsp;</td>\r\n                <td  nowrap>服务编号<input name=\"iid_Search\" type=\"text\" class=\"text\" size=\"8\" maxlength=\"20\" onmouseover=\"this.focus()\" ></td>\r\n                <td  nowrap>玩家帐号<input name=\"name\" type=\"text\" class=\"text\" size=\"13\" maxlength=\"50\" onmouseover=\"this.focus()\" >\r\n                </td>\r\n                <td  nowrap>游戏<select name=\"gameid\" onmouseover=\"this.focus()\" >\r\n                    <option value=\"0\" selected>剑网</option>\r\n                    <option value=\"1\">剑网IB</option>\r\n                  </select></td>\r\n                <td  nowrap>当前状态<select name=\"selOperationFlag\" onmouseover=\"this.focus()\" >\r\n\t\t\t\t\t<option value=\"all\" selected>所有状态</option>\r\n                    <option value=\"0\">已扣元宝,申请待理</option>\r\n                    <option value=\"1\">已扣元宝,恢复成功</option>\r\n                    <option value=\"2\">恢复失败,元宝未还</option>\r\n                    <option value=\"3\">恢复失败,元宝已还</option>\r\n                  </select></td>\r\n                <td  nowrap>查询期间\r\n                  <input name=\"commitedate\" type=\"text\" size=\"21\" readonly maxlength=\"21\" class=\"text\">\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('commitedate')\" style=\"cursor:hand \">\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                  <td  align=\"center\" nowrap>编号</td>\r\n                  <td   align=\"center\">提交时间</td>\r\n                  <td  align=\"center\" nowrap>游戏</td>\r\n                  <td  align=\"center\" nowrap>玩家真名</td>\r\n                  <td  align=\"center\" nowrap>玩家帐号</td>\r\n                  <td  align=\"center\" nowrap>所在区服</td>\r\n                  <td  align=\"center\" nowrap>角色名称</td>\r\n                  <td  align=\"center\" nowrap>删除时间</td>\r\n                  <td  align=\"center\" nowrap>联系电话</td>\r\n                  <td  align=\"center\" nowrap>当前状态</td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/roleresume.jsp";from=(189,18);to=(254,8)]
                
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
                
                   //ArrayList lt1 = new ArrayList();
                   //HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                	 String iid="";//编号
                	String game_id="";//提交时间
                	String gamename="";//提交时间
                	String commiteDate="";//提交时间
                	String account_name="";//玩家真名
                	String name="";//玩家帐号
                	String server_name="";//所在区服
                	String roler_name="";//角色名称
                	String deletedate="";//删除时间
                	String phone="";//联系电话
                	String operationFlag="";//当前状态
                	String operationFlagStr="";//当前状态
                    HashMap hs = new HashMap();
                
                   if (hs1.containsKey("pageAction")) {
                       String page1 = (String)hs1.get("pageAction");
                       if (page1.trim().equals("servlet")) {
                           se.setAttribute("filllog", hs1);
                       } else {
                           hs1 = (HashMap)se.getAttribute("filllog");
                       }
                
                       num = SearchHelperId.getRoleResumeNum(hs1);
                       curPageInfo = QueryPaginationId.setPageInfo(request, num); //分页信息
                       if(num!=0){
                       	lt = SearchHelperId.getRoleResumeList(hs1, curPageInfo[0], curPageInfo[2]);
                       }
                   }
                	   game_id = (String)hs1.get("game_id");
                	   if ("1".equals(game_id)){
                		   gamename="剑侠IB";
                	   } else {
                		   gamename="剑侠";
                	   }
                
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                		 hs = (HashMap)lt.get(i);
                		 iid = Util.setNullToEmpty((String)hs.get("iid"));
                		 commiteDate = Util.setNullToEmpty((String)hs.get("commitedate"));
                		 account_name = Util.setNullToEmpty((String)hs.get("account_name"));
                		 name = Util.setNullToEmpty((String)hs.get("name"));
                		 server_name = Util.setNullToEmpty((String)hs.get("server_name"));
                		 roler_name = Util.setNullToEmpty((String)hs.get("roler_name"));
                		 deletedate = Util.setNullToEmpty((String)hs.get("deletedate"));
                		 phone = Util.setNullToEmpty((String)hs.get("phone"));
                		 operationFlag = Util.setNullToEmpty((String)hs.get("operationflag"));
                		 operationFlagStr = Util.setNullToEmpty((String)hs.get("operationflagstr"));
                        
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(254,10);to=(255,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/roleresume.jsp";from=(255,26);to=(255,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(255,39);to=(255,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(255,59);to=(255,60)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(255,62);to=(256,80)]
                out.write(">\r\n                      <td align=\"center\" nowrap><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/search/roleresume.jsp";from=(256,83);to=(256,84)]
                out.print(i);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(256,86);to=(256,93)]
                out.write("\" iid=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(256,96);to=(256,99)]
                out.print(iid);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(256,101);to=(256,118)]
                out.write("\" operationflag=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(256,121);to=(256,134)]
                out.print(operationFlag);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(256,136);to=(257,20)]
                out.write("\" onClick=\"onSelect()\">\r\n                    ");

            // end
            // begin [file="/search/roleresume.jsp";from=(257,23);to=(257,26)]
                out.print(iid);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(257,28);to=(258,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/roleresume.jsp";from=(258,44);to=(258,55)]
                out.print(commiteDate);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(258,57);to=(259,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(259,51);to=(259,59)]
                out.print(gamename);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(259,61);to=(260,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(260,51);to=(260,63)]
                out.print(account_name);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(260,65);to=(261,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(261,51);to=(261,55)]
                out.print(name);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(261,57);to=(262,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(262,51);to=(262,62)]
                out.print(server_name);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(262,64);to=(263,48)]
                out.write("服</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(263,51);to=(263,61)]
                out.print(roler_name);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(263,63);to=(264,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(264,51);to=(264,61)]
                out.print(deletedate);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(264,63);to=(265,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(265,51);to=(265,56)]
                out.print(phone);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(265,58);to=(266,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/roleresume.jsp";from=(266,51);to=(266,67)]
                out.print(operationFlagStr);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(266,69);to=(268,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/roleresume.jsp";from=(268,22);to=(270,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(270,6);to=(277,44)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\">\r\n\t\t  \t\t  <form name=\"frmModifyState\" action=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(277,47);to=(277,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(277,73);to=(280,95)]
                out.write("/GMServlet\">\r\n\t\t  <table >\r\n              <tr>\r\n                <td><input name=\"iid\" type=\"hidden\"><input name=\"game_id\" type=\"hidden\" value=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(280,98);to=(280,105)]
                out.print(game_id);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(280,107);to=(299,7)]
                out.write("\"><input name=\"action\" type=\"hidden\" value=\"modifyroleresumestate\"><button name=\"btnModify\" onClick=\"onModify()\" disabled>状态修改为</button></td>\r\n                <td><select name=\"operationflag\" disabled>\r\n\t\t\t\t\t<option value=\"0\">已扣元宝,申请待理</option>\r\n\t\t\t\t\t<option value=\"1\" selected>已扣元宝,恢复成功</option>\r\n\t\t\t\t\t<option value=\"2\">恢复失败,元宝未还</option>\r\n\t\t\t\t\t<option value=\"3\">恢复失败,元宝已还</option>\r\n                  </select></td>\r\n              </tr>\r\n            </table>\r\n\t\t\t</form>\r\n\r\n\t\t  </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"center\">\r\n       <!-----------------------分页信息------------------------>\r\n       <table id=\"tablePage\" class=\"pagebar\" width=\"100%\">\r\n       <tr>\r\n\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(299,9);to=(308,7)]
                
                         nStep = curPageInfo[2];
                         curPageEndIndex = QueryPaginationId.getEndPageEndIndex();
                         sumPageNumber = curPageInfo[3];
                         lastPageStartIndex = curPageInfo[4];
                         curPageIndex = curPageInfo[5];
                         curPageStartIndex = curPageInfo[0];
                         isPrevious = curPageInfo[6];
                         isNext = curPageInfo[7];
                       
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(308,9);to=(310,35)]
                out.write("\r\n\r\n           <td width=\"15%\" nowrap>共");

            // end
            // begin [file="/search/roleresume.jsp";from=(310,38);to=(310,41)]
                out.print(num);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(310,43);to=(310,57)]
                out.write("条&nbsp;&nbsp;第");

            // end
            // begin [file="/search/roleresume.jsp";from=(310,60);to=(310,77)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(310,79);to=(310,80)]
                out.write("～");

            // end
            // begin [file="/search/roleresume.jsp";from=(310,83);to=(310,108)]
                out.print(curPageStartIndex + nStep);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(310,110);to=(311,7)]
                out.write("条</td>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(311,9);to=(314,15)]
                
                           if ((isPrevious == 1)&&(num > nStep))
                           {
                               
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(314,17);to=(315,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(315,38);to=(315,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(315,66);to=(316,35)]
                out.write("/search/roleresume.jsp?pageAction=first&start=\">第一页</a></td>\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(316,38);to=(316,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(316,66);to=(316,115)]
                out.write("/search/roleresume.jsp?pageAction=previous&start=");

            // end
            // begin [file="/search/roleresume.jsp";from=(316,118);to=(316,141)]
                out.print(curPageEndIndex-2*nStep);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(316,143);to=(317,7)]
                out.write("\">上一页</a></td>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(317,9);to=(318,7)]
                 } else {
                       
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(318,9);to=(321,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>第一页</u></td>\r\n            <td width=\"7%\"><u disabled>上一页</u></td>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(321,9);to=(324,14)]
                   }
                           if((isNext == 1) &&(num > nStep))
                           {
                              
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(324,16);to=(325,35)]
                out.write("\r\n           <td width=\"7%\"><a href=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(325,38);to=(325,64)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(325,66);to=(325,111)]
                out.write("/search/roleresume.jsp?pageAction=next&start=");

            // end
            // begin [file="/search/roleresume.jsp";from=(325,114);to=(325,129)]
                out.print(curPageEndIndex);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(325,131);to=(326,36)]
                out.write("\">下一页</a></td>\r\n           <td width=\"12%\"><a href=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(326,39);to=(326,65)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(326,67);to=(326,112)]
                out.write("/search/roleresume.jsp?pageAction=last&start=");

            // end
            // begin [file="/search/roleresume.jsp";from=(326,115);to=(326,133)]
                out.print(lastPageStartIndex);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(326,135);to=(327,7)]
                out.write("\">最后一页</a></td>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(327,9);to=(327,18)]
                 } else {
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(327,20);to=(330,7)]
                out.write("\r\n            <td width=\"7%\"><u disabled>下一页</u></td>\r\n            <td width=\"12%\"><u disabled>最后一页</u></td>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(330,9);to=(330,10)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(330,12);to=(333,7)]
                out.write("\r\n\r\n           <td width=\"10%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(333,9);to=(336,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(336,17);to=(337,15)]
                out.write("\r\n               ");

            // end
            // begin [file="/search/roleresume.jsp";from=(337,18);to=(337,32)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(337,34);to=(337,35)]
                out.write("/");

            // end
            // begin [file="/search/roleresume.jsp";from=(337,38);to=(337,51)]
                out.print(sumPageNumber);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(337,53);to=(338,7)]
                out.write("\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(338,9);to=(338,10)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(338,12);to=(341,7)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(341,9);to=(344,15)]
                
                           if(num > nStep)
                           {
                               
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(344,17);to=(345,53)]
                out.write("\r\n           转到第<INPUT TYPE=\"text\" NAME=\"page\"  value=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(345,56);to=(345,70)]
                out.print(curPageIndex+1);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(345,72);to=(345,131)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('page');goClick('");

            // end
            // begin [file="/search/roleresume.jsp";from=(345,134);to=(345,158)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(345,160);to=(345,175)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/roleresume.jsp";from=(345,178);to=(345,183)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(345,185);to=(346,55)]
                out.write(")\">页\r\n           <button name=\"searchBut\"  onClick=\"goClick('");

            // end
            // begin [file="/search/roleresume.jsp";from=(346,58);to=(346,82)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(346,84);to=(346,99)]
                out.write("', page.value, ");

            // end
            // begin [file="/search/roleresume.jsp";from=(346,102);to=(346,107)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(346,109);to=(347,7)]
                out.write(")\"> GO </button>\r\n       ");

            // end
            // begin [file="/search/roleresume.jsp";from=(347,9);to=(347,10)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(347,12);to=(349,78)]
                out.write("\r\n           </td>\r\n           <td width=\"20%\" nowrap>页长设置为<INPUT TYPE=\"text\" NAME=\"step\"  value=\"");

            // end
            // begin [file="/search/roleresume.jsp";from=(349,81);to=(349,86)]
                out.print(nStep);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(349,88);to=(349,149)]
                out.write("\" style=\"width:50px\" onKeyPress=\"CheckNum('step');stepClick('");

            // end
            // begin [file="/search/roleresume.jsp";from=(349,152);to=(349,176)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(349,178);to=(349,193)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/roleresume.jsp";from=(349,196);to=(349,213)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(349,215);to=(350,54)]
                out.write(")\">\r\n           <button name=\"stepBut\" onClick=\"stepClick('");

            // end
            // begin [file="/search/roleresume.jsp";from=(350,57);to=(350,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(350,83);to=(350,98)]
                out.write("', step.value, ");

            // end
            // begin [file="/search/roleresume.jsp";from=(350,101);to=(350,118)]
                out.print(curPageStartIndex);
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(350,120);to=(363,6)]
                out.write(")\">确定</button>\r\n           </td>\r\n         </tr>\r\n            </table>\r\n</td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/roleresume.jsp";from=(363,8);to=(363,19)]
                if(num==0){
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(363,21);to=(367,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/roleresume.jsp";from=(367,8);to=(367,9)]
                }
            // end
            // HTML // begin [file="/search/roleresume.jsp";from=(367,11);to=(376,0)]
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
