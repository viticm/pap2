package org.apache.jsp;

import com.kingsoft.gmsystem.service.GMHelper;
import com.kingsoft.gmsystem.common.Util;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class accountroleinfo$jsp extends HttpJspBase {


    static {
    }
    public accountroleinfo$jsp( ) {
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

            // HTML // begin [file="/search/accountroleinfo.jsp";from=(2,96);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(4,55);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(5,0);to=(5,49)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("accountroleinfo");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/accountroleinfo.jsp";from=(5,0);to=(5,49)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(5,49);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(6,2);to=(19,0)]
                
                	String account=new String(Util.setNullToEmpty(request.getParameter("account")).getBytes("ISO-8859-1"));
                	String roleName=new String(Util.setNullToEmpty(request.getParameter("roleName")).getBytes("ISO-8859-1"));
                	String zonecode=request.getParameter("zonecode2");
                	String game_code=request.getParameter("game_code");
                	String gatewayName=request.getParameter("gatewayName");
                	
                	GMHelper gh = new GMHelper();
                	List gatewayList = gh.getGatewayList();
                	Iterator it = gatewayList.iterator();
                	Iterator accountRoleIterator = null;
                	if(null != request.getAttribute("accountRoleList"))
                		accountRoleIterator = ((ArrayList)request.getAttribute("accountRoleList")).iterator();
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(19,2);to=(27,14)]
                out.write("\r\n<html>\r\n\t<!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n\t<head>\r\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n\t\t<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n\t\t<title>玩家帐号角色互查</title>\r\n\t\t<!-- InstanceEndEditable -->\r\n\t\t<link href=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(27,17);to=(27,41)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(27,43);to=(29,8)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n\t\t<script language=\"JavaScript\"\r\n\t\t\tsrc=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(29,11);to=(29,35)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(29,37);to=(31,16)]
                out.write("/js/public.js\"></script>\r\n\t\t<script language=\"JavaScript\">\r\n\t\t\tvar AbsPath=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(31,19);to=(31,43)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(31,45);to=(34,2)]
                out.write("\";   \r\n\t\t</script>\r\n\t\t<!-- InstanceBeginEditable name=\"head\" -->\r\n\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(34,4);to=(34,4)]
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(34,6);to=(36,15)]
                out.write("\r\n<script language=\"JavaScript\">\r\nvar game_code=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(36,18);to=(36,48)]
                out.print(Util.setNullToEmpty(game_code));
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(36,50);to=(37,14)]
                out.write("\";\r\nvar zonecode=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(37,17);to=(37,46)]
                out.print(Util.setNullToEmpty(zonecode));
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(37,48);to=(38,18)]
                out.write("\";\r\nvar gatewayNames=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(38,21);to=(38,53)]
                out.print(Util.setNullToEmpty(gatewayName));
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(38,55);to=(41,0)]
                out.write("\";\r\nvar gatewayAry = new Array();\r\n\r\n");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(41,2);to=(49,0)]
                
                	HashMap hm = null;
                	int cnt = 0;
                	while(it.hasNext()){
                		hm = (HashMap)it.next();
                		out.println("gatewayAry[" + cnt++ + "] =new Array('" + hm.get("game_id")+"','"+ hm.get("zonecode")+"','"  
                				+ hm.get("gateway_name")+"','" + hm.get("area")+"','"+ hm.get("group")+"','"+ hm.get("region")+"'); ");
                	}
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(49,2);to=(126,19)]
                out.write("\r\n\r\nfunction checkValue()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"account\")&&IsEmpty(\"roleName\")) strErr+=\"*玩家帐号或角色信息不能同时为空\\n\";\r\n\tif(strErr!=\"\"){\r\n\t\talert(strErr);\t\r\n\t\treturn false;\r\n\t}else{\t\t\r\n\t\tvar objDiv = document.all(\"gateway\");\r\n\t\tvar aryCheck = objDiv.all.tags(\"INPUT\");\r\n\t\tvar intCheckLength = aryCheck.length;\r\n\t\tvar gateways = \"\";\r\n\t\tfor (i = 0; i < intCheckLength; i++) {\t\r\n\t\t\tif (aryCheck[i].checked) {\r\n\t\t\t\tgateways += aryCheck[i].name + \",\";\r\n\t\t\t}\r\n\t\t}\r\n\t\tdocument.all(\"gatewayName\").value = gateways;\r\n\t\treturn true;\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"zonecode2\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_code\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t \tif(j==1){\r\n\t\t\tobjSel.options[objSel.length]=new Option(0+\".所有区\",'all');\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t \t}\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n}\r\n</script>\r\n\t</head>\r\n\t<!-- InstanceEndEditable -->\r\n\r\n\t<body>\r\n\t\t<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\"\r\n\t\t\tcellspacing=\"0\">\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"20\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\t&nbsp;&nbsp;当前位置：\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"position\" -->\r\n\t\t\t\t\t<span class=\"title\">查询功能</span> &gt;&gt;\r\n\t\t\t\t\t<span class=\"title\">玩家帐号角色互查</span>\r\n\t\t\t\t\t<!-- InstanceEndEditable -->\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td align=\"center\" valign=\"top\">\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"main\" -->\r\n\t\t\t\t\t<div id=\"divPrompt\"\r\n\t\t\t\t\t\tstyle=\"width: 250px; position: absolute; display: none; left: 340px; top: 300px; height: 44px;\">\r\n\t\t\t\t\t\t<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\"\r\n\t\t\t\t\t\t\tcellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td align=\"center\" valign=\"middle\">\r\n\t\t\t\t\t\t\t\t\t正在查询和等待返回结果，请稍候......\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t</table>\r\n\t\t\t\t\t</div>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<form action=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(126,22);to=(126,46)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(126,48);to=(139,8)]
                out.write("/GMCServlet?action=accountroleinfo\" method=\"post\" onsubmit=\"return checkValue()\">\r\n\t\t\t\t\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t所属游戏\r\n\t\t\t\t\t\t\t\t<select id=\"selectGamename\" name=\"game_code\"\r\n\t\t\t\t\t\t\t\t\tonmouseover=\"this.focus()\" onchange=\"onChangeGameId()\">\r\n\t\t\t\t\t\t\t\t\t<OPTION value='88'>\r\n\t\t\t\t\t\t\t\t\t\t剑网3\r\n\t\t\t\t\t\t\t\t\t</OPTION>\r\n\t\t\t\t\t\t\t\t</select>\r\n\t\t\t\t\t\t\t\t所在区\r\n\t\t\t\t\t\t\t\t<select name=\"zonecode2\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(139,8);to=(140,58)]
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
                        // HTML // begin [file="/search/accountroleinfo.jsp";from=(140,58);to=(141,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/search/accountroleinfo.jsp";from=(141,8);to=(141,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(141,26);to=(149,16)]
                out.write("\r\n\t\t\t\t\t\t\t\t<input type=\"hidden\" name=\"gatewayName\"/>\r\n\t\t\t\t\t\t\t\t所在服\r\n\t\t\t\t\t\t\t\t<div id=\"gateway\">\r\n\t\t\t\t\t\t\t\t</div>\r\n\r\n\t\t\t\t\t\t\t\t玩家帐号\r\n\t\t\t\t\t\t\t\t<input name=\"account\" type=\"text\" class=\"text\"\r\n\t\t\t\t\t\t\t\t\tvalue=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(149,19);to=(149,26)]
                out.print(account);
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(149,28);to=(152,63)]
                out.write("\" onmouseover=\"this.focus()\" size=\"20\"\r\n\t\t\t\t\t\t\t\t\tmaxlength=\"30\">\r\n\t\t\t\t\t\t\t\t玩家角色\r\n\t\t\t\t\t\t\t\t<input type=\"text\" name=\"roleName\" class=\"text\" value=\"");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(152,66);to=(152,74)]
                out.print(roleName);
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(152,76);to=(154,45)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\tonmouseover=\"this.focus()\" size=\"20\" maxlength=\"30\">\r\n\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"isLike\" ");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(154,48);to=(154,128)]
                out.print(Util.setNullToEmpty(request.getParameter("isLike")).equals("") ? "" : "checked" );
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(154,130);to=(183,9)]
                out.write(">模糊查询\r\n\t\t\t\t\t\t\t\t<input type=\"submit\" name=\"btnSearch\" value=\"查询\"/>\t\t\t\t\t\t\t\t\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t<br>\r\n\t\t\t\t\t\t\t\t<table width=\"773\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\t\t\tclass=\"tablelist\">\r\n\t\t\t\t\t\t\t\t\t<tr align=\"left\" bgcolor=\"#419ADC\">\r\n\t\t\t\t\t\t\t\t\t\t<td height=\"31\" colspan=\"6\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t<font color=#ffffff><b>&nbsp;&#8226;&nbsp;玩家帐号基本信息</b>\r\n\t\t\t\t\t\t\t\t\t\t\t</font>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t玩家帐号\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t玩家角色\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t所在区\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t所在服\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(183,11);to=(187,9)]
                
                										if(null != accountRoleIterator){
                											while(accountRoleIterator.hasNext()){
                												hm = (HashMap)accountRoleIterator.next();
                									
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(187,11);to=(191,11)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\r\n\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(191,14);to=(191,32)]
                out.print(hm.get("account") );
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(191,34);to=(194,11)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(194,14);to=(194,33)]
                out.print(hm.get("RoleName") );
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(194,35);to=(197,11)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(197,14);to=(197,33)]
                out.print(hm.get("ZoneName") );
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(197,35);to=(200,11)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td width=\"25%\" height=\"25\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(200,14);to=(200,36)]
                out.print(hm.get("GatewayName") );
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(200,38);to=(203,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/search/accountroleinfo.jsp";from=(203,11);to=(206,9)]
                
                											}
                										}
                									
            // end
            // HTML // begin [file="/search/accountroleinfo.jsp";from=(206,11);to=(260,0)]
                out.write("\r\n\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t</table>\r\n\t\t\t\t\t</form>\r\n\t\t\t\t\t<script language=\"JavaScript\">\r\ndocument.getElementById(\"zonecode2\").onchange = function onChangeZoneCode(){\r\n\tvar objSel = document.getElementById(\"zonecode2\");\r\n\tvar objOpt = objSel.options[objSel.selectedIndex];\r\n\tvar tmp = \"\";\r\n\tfor(var cnt=0;cnt<gatewayAry.length;cnt++){\r\n\t\tif(gatewayAry[cnt][0]==objOpt.game_id&&gatewayAry[cnt][1]==objOpt.value){\t\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+gatewayAry[cnt][5]+\"'>\"+gatewayAry[cnt][2];\r\n\t\t}\r\n\t}\r\n\tdocument.getElementById(\"gateway\").innerHTML = tmp;\r\n}\r\n\r\n\t  if(game_code!=\"\"){\r\n\t\tSetSelValue(\"game_code\",game_code);\r\n        onChangeGameId();\r\n        SetSelValue(\"zonecode2\",zonecode);\r\n\t  }else{\r\n          onChangeGameId();\r\n       }\r\n      if(zonecode!=\"\"){\r\n\t\tonChangeZoneCode();\r\n\t\tif(gatewayNames!=\"\"){\r\n\t\t\tvar objDiv = document.all(\"gateway\");\r\n\t\t\tvar aryCheck = objDiv.all.tags(\"INPUT\");\r\n\t\t\tvar intCheckLength = aryCheck.length;\r\n\t\t\tvar gateways = \"\";\r\n\t\t\tfor (i = 0; i < intCheckLength; i++) {\t\r\n\t\t\t\tif (gatewayNames.indexOf(aryCheck[i].name+\",\")>-1) {\r\n\t\t\t\t\taryCheck[i].checked=true;\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t}\t\t\r\n\t\t\r\n   \t  }\r\n\t</script>\r\n\t\t\t\t\t<!-- InstanceEndEditable -->\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\tCopyright &copy; 2005 Kingsoft Corp,All Rights Reserved\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t</table>\r\n\t</body>\r\n\t<!-- InstanceEnd -->\r\n</html>\r\n");

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
