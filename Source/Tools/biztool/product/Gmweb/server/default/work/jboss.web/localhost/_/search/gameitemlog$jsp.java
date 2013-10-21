package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class gameitemlog$jsp extends HttpJspBase {

    // begin [file="/search/gameitemlog.jsp";from=(5,0);to=(5,100)]
    // end

    static {
    }
    public gameitemlog$jsp( ) {
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

            // HTML // begin [file="/search/gameitemlog.jsp";from=(0,175);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(4,60);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(5,0);to=(5,100)]
                com.kingsoft.gmsystem.service.SearchHelper SearchHelper = null;
                boolean _jspx_specialSearchHelper  = false;
                 synchronized (session) {
                    SearchHelper= (com.kingsoft.gmsystem.service.SearchHelper)
                    pageContext.getAttribute("SearchHelper",PageContext.SESSION_SCOPE);
                    if ( SearchHelper == null ) {
                        _jspx_specialSearchHelper = true;
                        try {
                            SearchHelper = (com.kingsoft.gmsystem.service.SearchHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SearchHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SearchHelper", exc);
                        }
                        pageContext.setAttribute("SearchHelper", SearchHelper, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSearchHelper == true) {
            // end
            // begin [file="/search/gameitemlog.jsp";from=(5,0);to=(5,100)]
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(5,100);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(6,0);to=(6,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("gameitemlog");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/gameitemlog.jsp";from=(6,0);to=(6,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(6,45);to=(13,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>玩家物品流向查询</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(13,2);to=(28,0)]
                
                    ArrayList lt1 = new ArrayList();
                    HashMap hs1 = Util.parseUploadData(request, lt1);
                    ArrayList logInfos = new ArrayList();
                    String game_id = Util.setNullToEmpty((String)hs1.get("game_id"));
                    String gateway_id = Util.setNullToEmpty((String)hs1.get("gateway_id"));
                    String idc_id = Util.setNullToEmpty((String)hs1.get("idc_id"));
                    String role = Util.setNullToEmpty((String)hs1.get("role"));
                    String account = Util.setNullToEmpty((String)hs1.get("account"));
                    String ddatetime = Util.setNullToEmpty((String)hs1.get("ddatetime"));
                    String itemname = Util.setNullToEmpty((String)hs1.get("itemname"));
                    String itemid = Util.setNullToEmpty((String)hs1.get("itemid"));
                    if((role!=null && !role.equals("")) || (account!=null && !account.equals("")) || (itemid!=null && !itemid.equals(""))){
                      logInfos=SearchHelper.getGameItemLogList(hs1);
                    }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(28,2);to=(30,13)]
                out.write("\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(30,16);to=(30,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(30,42);to=(31,10)]
                out.write("\";\r\nvar role=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(31,13);to=(31,17)]
                out.print(role);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(31,19);to=(32,13)]
                out.write("\";\r\nvar account=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(32,16);to=(32,23)]
                out.print(account);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(32,25);to=(33,13)]
                out.write("\";\r\nvar game_id=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(33,16);to=(33,23)]
                out.print(game_id);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(33,25);to=(34,16)]
                out.write("\";\r\nvar gateway_id=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(34,19);to=(34,29)]
                out.print(gateway_id);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(34,31);to=(35,15)]
                out.write("\";\r\nvar ddatetime=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(35,18);to=(35,27)]
                out.print(ddatetime);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(35,29);to=(36,14)]
                out.write("\";\r\nvar itemname=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(36,17);to=(36,25)]
                out.print(itemname);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(36,27);to=(37,12)]
                out.write("\";\r\nvar itemid=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(37,15);to=(37,21)]
                out.print(itemid);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(37,23);to=(147,34)]
                out.write("\";\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"account\")&&IsEmpty(\"role\")&&IsEmpty(\"itemid\")) strErr+=\"*玩家帐号/角色和物品ID不能都为空\\n\";\r\n\tif(IsEmpty(\"ddatetime\")) strErr+=\"*查询期间不能为空\\n\";\r\n\tif(!IsEmpty(\"itemid\")){\r\n\t\tif(!checkNumn(\"itemid\")){ \r\n\t\tstrErr+=\"*物品ID只能为整数\\n\";\r\n\t\t}\r\n\t}\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar role=document.all(\"role\").value;\r\n\t\tvar account=document.all(\"account\").value;\r\n\t\tvar objGateway=document.all(\"selGateway\");\r\n\t\tvar objOption=objGateway.options[objGateway.selectedIndex];\r\n\t\tvar game_id=objOption.game_id;\r\n\t\tvar gateway_id=objOption.value;\r\n\t\tvar gateway=objOption.gateway;\r\n\t\tvar idc_id=objOption.idc_id;\r\n\t\tvar ddatetime=document.all(\"ddatetime\").value;\r\n\t\tvar itemname=document.all(\"itemname\").value;\r\n\t\tvar itemid=document.all(\"itemid\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/gameitemlog.jsp?role=\"+role+\"&account=\"+account+\"&itemname=\"+itemname+\"&game_id=\"+game_id+\"&gateway_id=\"+gateway_id+\"&gateway=\"+gateway+\"&idc_id=\"+idc_id+\"&ddatetime=\"+ddatetime+\"&itemid=\"+itemid;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\t\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_code\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n\tonChangeZoneCode();\r\n\t\t\r\n\t/*var objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_code\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"gateway\",objOption.gateway);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"idc_id\",objOption.idc_id);\r\n\t\t}\t\t\r\n\t}*/\r\n}\r\n\r\nfunction initAreaGroup()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objOption;\r\n\tfor(i=0;i<objSel.length;i++)\r\n\t{\r\n\t\tobjOption=objSel.options[i];\r\n\t\tif(objOption.value==gateway_id)\r\n\t\t{\r\n\t\t \tobjSel.options[i].selected = true;\r\n\t\t}\t\t\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">玩家物品流向查询</span> </td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\r\n\t<br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\">\r\n\t\t  <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t    <tr>\r\n\t\t\t  <td width=\"10%\" align=\"right\">所属游戏：</td>\r\n\t\t\t  <td width=\"15%\" align=\"left\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(147,34);to=(147,146)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_code");
                _jspx_th_game_gameselect_0.setOnChange("onChangeGameId()");
                _jspx_th_game_gameselect_0.setStyle("width:90");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/gameitemlog.jsp";from=(147,34);to=(147,146)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(147,146);to=(151,8)]
                out.write("</td>\r\n\t\t\t  <td width=\"8%\" align=\"right\">所在区服：</td>\r\n\t\t\t  <td width=\"15%\" align=\"left\">\r\n                  <select name=\"zonecode\" style=\"width:200\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(151,8);to=(152,58)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_zone_all");
                _jspx_th_display_display_0.setProperty("zonecodeTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/search/gameitemlog.jsp";from=(152,58);to=(153,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/search/gameitemlog.jsp";from=(153,8);to=(153,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(153,26);to=(155,5)]
                out.write("\r\n\t\t\t  <select name=\"selGateway\"></select>\r\n\t\t\t  ");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(155,5);to=(155,106)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_gateway_idc");
                _jspx_th_display_display_1.setProperty("selGatewayTemp");
                _jspx_th_display_display_1.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/search/gameitemlog.jsp";from=(155,106);to=(156,5)]
                            out.write("\r\n\t\t\t  ");

                        // end
                        // begin [file="/search/gameitemlog.jsp";from=(156,5);to=(156,23)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(156,23);to=(158,124)]
                out.write("</td>\r\n\t\t\t  <td width=\"9%\" align=\"right\">查询期间：</td>\r\n\t\t\t  <td width=\"18%\" align=\"left\"><input name=\"ddatetime\" type=\"text\" size=\"21\" readonly maxlength=\"21\" class=\"text\" value=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(158,127);to=(158,176)]
                out.print(Util.setNullToEmpty((String)hs1.get("ddatetime")));
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(158,178);to=(159,80)]
                out.write("\">&nbsp;<IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('ddatetime')\" style=\"cursor:hand \"></td>\r\n\t\t\t  <td align=\"left\">物品ID：<input name=\"itemid\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(159,83);to=(159,89)]
                out.print(itemid);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(159,91);to=(162,76)]
                out.write("\" onmouseover=\"this.focus()\" size=\"12\" maxlength=\"50\" ></td></tr>\r\n\t\t    <tr>\r\n\t\t\t  <td align=\"right\">玩家帐号：</td>\r\n\t\t\t  <td align=\"left\"><input name=\"account\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(162,79);to=(162,86)]
                out.print(account);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(162,88);to=(164,73)]
                out.write("\" onmouseover=\"this.focus()\" size=\"12\" maxlength=\"50\" ></td>\r\n\t\t\t  <td align=\"right\">玩家角色：</td>\r\n\t\t\t  <td align=\"left\"><input name=\"role\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(164,76);to=(164,80)]
                out.print(role);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(164,82);to=(166,77)]
                out.write("\" onmouseover=\"this.focus()\" size=\"12\" maxlength=\"50\" ></td>\r\n\t\t\t  <td align=\"right\">物品名称：</td>\r\n\t\t\t  <td align=\"left\"><input name=\"itemname\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(166,80);to=(166,88)]
                out.print(itemname);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(166,90);to=(192,11)]
                out.write("\" onmouseover=\"this.focus()\" size=\"12\" maxlength=\"50\" ></td>\r\n\t\t\t  <td align=\"left\">&nbsp;&nbsp;&nbsp;&nbsp;<button name=\"btnSearch\" onClick=\"onSearch()\">&nbsp;&nbsp;查询&nbsp;&nbsp; </button></td></tr>\r\n\t\t  </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <table width=\"773\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n\t\t\t <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"30\"  colspan=\"6\"><font color=#ffffff><b>&nbsp;&#8226;&nbsp;玩家物品流向记录</b></font></td>\r\n              </tr>\r\n             <tr align=\"left\">\r\n               <td colspan=\"6\">\r\n                <table width=\"100%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                    <tr>\r\n                      <td width=\"4%\" align=\"center\" nowrap>序号</td>\r\n                      <td width=\"10%\" align=\"center\" nowrap>区服</td>\r\n                      <td width=\"10%\" align=\"center\" nowrap>帐号</td>\r\n                      <td width=\"10%\" align=\"center\" nowrap>角色</td>\r\n                      <td width=\"10%\" align=\"center\" nowrap>流向类型</td>\r\n                      <td width=\"12%\" align=\"center\" nowrap>物品ID</td>\r\n                      <td width=\"12%\" align=\"center\" nowrap>物品名称</td>\r\n                      <td width=\"8%\" align=\"center\" nowrap>价值量</td>\r\n                      <td width=\"15%\" align=\"center\" nowrap>时间</td>\r\n                      <td align=\"center\" nowrap>备注</td>\r\n                    </tr>\r\n\t\t         ");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(192,13);to=(214,8)]
                
                	String OperationTime="";
                	String OperationType="";
                	String GatewayName="";
                	String AccountName="";
                	String RoleName="";
                	String ItemID="";
                	String ItemName="";
                	String ItemValue="";
                	String Comment="";
                	HashMap hs = new HashMap();
                                 for(int i=0;i<logInfos.size();i++){
                		 hs = (HashMap)logInfos.get(i);
                		 OperationTime = Util.setNullToEmpty((String)hs.get("OperationTime"));
                		 OperationType = Util.setNullToEmpty((String)hs.get("OperationType"));
                		 GatewayName = Util.setNullToEmpty((String)hs.get("GatewayName"));
                		 AccountName = Util.setNullToEmpty((String)hs.get("AccountName"));
                		 RoleName = Util.setNullToEmpty((String)hs.get("RoleName"));
                		 ItemID = Util.setNullToEmpty((String)hs.get("ItemID"));
                		 ItemName = Util.setNullToEmpty((String)hs.get("ItemName"));
                		 ItemValue = Util.setNullToEmpty((String)hs.get("ItemValue"));
                		 Comment = Util.setNullToEmpty((String)hs.get("Comment"));
                        
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(214,10);to=(215,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(215,26);to=(215,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(215,39);to=(215,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(215,59);to=(215,60)]
                }
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(215,62);to=(216,41)]
                out.write(">\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(216,44);to=(216,47)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(216,49);to=(217,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(217,44);to=(217,55)]
                out.print(GatewayName);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(217,57);to=(218,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(218,44);to=(218,55)]
                out.print(AccountName);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(218,57);to=(219,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(219,44);to=(219,52)]
                out.print(RoleName);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(219,54);to=(220,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(220,44);to=(220,57)]
                out.print(OperationType);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(220,59);to=(221,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(221,44);to=(221,50)]
                out.print(ItemID);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(221,52);to=(222,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(222,44);to=(222,52)]
                out.print(ItemName);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(222,54);to=(223,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(223,44);to=(223,53)]
                out.print(ItemValue);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(223,55);to=(224,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(224,44);to=(224,57)]
                out.print(OperationTime);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(224,59);to=(225,41)]
                out.write("</td>\r\n                      <td align=\"center\">");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(225,44);to=(225,51)]
                out.print(Comment);
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(225,53);to=(227,10)]
                out.write("</td>\r\n                    </tr>\r\n          ");

            // end
            // begin [file="/search/gameitemlog.jsp";from=(227,12);to=(229,9)]
                
                         }
                         
            // end
            // HTML // begin [file="/search/gameitemlog.jsp";from=(229,11);to=(290,0)]
                out.write("\r\n                 </table>\r\n               </td>\r\n              </tr>\r\n           </table>\r\n\t\t   <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td colspan=\"2\">&nbsp;\r\n               </td>\r\n             </tr>\r\n             <tr>\r\n               <td>\r\n                 <button onclick=\"history.back()\">返回</button>\r\n               </td>\r\n               <td>\r\n                 <button onclick=\"window.print()\">打印</button>\r\n               </td>\r\n             </tr>\r\n           </table>\r\n          </td>\r\n        </tr>\r\n      </table>\r\n\t  <script>\r\n\t  if(game_id!=\"\"){\r\n\t\tSetSelValue(\"game_code\",game_id);\r\n\t  }\r\n\t  onChangeGameId();\r\n\t  if(game_id!=\"\"){\r\n\t     initAreaGroup()\r\n\t  }\r\n\t  \r\n\t  \r\ndocument.getElementById(\"zonecode\").onchange = function onChangeZoneCode(){\r\n\t\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_code\").value&&objOption.zonecode==document.all(\"zonecode\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"gateway\",objOption.gateway);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"idc_id\",objOption.idc_id);\r\n\t\t}\r\n\t}\r\n}\r\n\t</script>\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
