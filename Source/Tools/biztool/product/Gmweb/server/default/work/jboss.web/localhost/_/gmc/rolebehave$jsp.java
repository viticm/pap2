package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class rolebehave$jsp extends HttpJspBase {


    static {
    }
    public rolebehave$jsp( ) {
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

            // HTML // begin [file="/gmc/rolebehave.jsp";from=(0,176);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(4,62);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(5,0);to=(5,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("5rolebehave");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(5,0);to=(5,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(5,45);to=(12,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>角色行为控制</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(12,2);to=(24,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
                	HashMap requestMap = (HashMap)session.getAttribute("requestMap");
                	String rolebehave = "";
                	String game_id = "";
                	if (requestMap != null){
                		rolebehave = Util.setNullToEmpty((String)requestMap.get("rolebehave"));
                		game_id = Util.setNullToEmpty((String)requestMap.get("game_id"));
                	}
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(24,2);to=(163,19)]
                out.write("\r\n<script language=\"javascript\">\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit() {\r\n\tvar strErr=\"\";\r\n\tif(document.all(\"trAccount\").style.display==\"\"&&document.all(\"trRole\").style.display==\"\"){\t\r\n\t\tif(IsEmpty(\"account\")&&IsEmpty(\"role\")) strErr+=\"*玩家帐号和玩家角色不能同时为空\\n\";\r\n\t}else if(document.all(\"trAccount\").style.display==\"\"&&IsEmpty(\"account\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\telse if(document.all(\"trRole\").style.display==\"\"&&IsEmpty(\"role\")) strErr+=\"*玩家角色不能为空\\n\";\r\n\tif(!IsEmpty(\"role\")&&document.all(\"type\").value!='FREEZE'&&IsEmpty(\"selGateway\")) strErr+=\"*请选择所在区服\\n\";\r\n\tif(document.all(\"selReason\").value==\"other\"&& IsEmpty(\"othermessage\"))  strErr+=\"*操作原因不能为空\\n\";\r\n\tif(strErr!=\"\"){\r\n\t\talert(strErr);\r\n\t\treturn false;\r\n\t}\r\n\tif(document.all(\"selReason\").value!=\"other\")\r\n\t{\r\n\t\tdocument.all(\"message\").value=document.all(\"selReason\").value;\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"message\").value=document.all(\"othermessage\").value;\r\n\t}\r\n\tif (!IsEmpty(\"role\")){\r\n\t\tsetGatewayInfo();\r\n\t}\r\n\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\tsetButtonState(true);\r\n\tdocument.all(\"frmSubmit\").submit();\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tif(objSel.selectedIndex!=-1){\r\n\t\tdocument.all(\"gateway\").value=objSel.options[objSel.selectedIndex].value;\r\n\t\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\t\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n\t\tdocument.all(\"zonecode2\").value=objSel.options[objSel.selectedIndex].zonecode;\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"zonecode\",objOption.zonecode);\r\n\t\t}\r\n\t}\r\n}\r\n\r\nfunction onChangeCommandType()\r\n{\r\n\tvar objSelCommandType=document.all(\"rolebehave\");\r\n\tvar objTermTr=document.all(\"trTerm\");\r\n\tif (objSelCommandType.options[objSelCommandType.selectedIndex].term == \"1\"){\r\n\t\tobjTermTr.style.display=\"\";\r\n\t} else {\r\n\t\tobjTermTr.style.display=\"none\";\r\n\t}\r\n\tdocument.all(\"type\").value=objSelCommandType.options[objSelCommandType.selectedIndex].type;\r\n\tdocument.all(\"option\").value=objSelCommandType.options[objSelCommandType.selectedIndex].option;\r\n\tvar objSelReason=document.all(\"selReason\");\r\n\tvar objSelReasonTemp=document.all(\"selReasonTemp\");\r\n\tvar objOption;\r\n\tobjSelReason.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelReasonTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelReasonTemp.options[i];\r\n\t\tif(objOption.commandType==objSelCommandType.options[objSelCommandType.selectedIndex].reasonKey)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSelReason.options[objSelReason.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t}\r\n\t}\r\n\tif(document.all(\"type\").value=='RESET_LOCK'||document.all(\"type\").value=='FREEZE'){\r\n\t\tdocument.all(\"command\").value = document.all(\"type\").value;\r\n\t\t\r\n\t\tvar objSel=document.all(\"zonecode\");\r\n\t\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\t\tvar objOption;\r\n\t\tobjSel.length=0;\r\n\t\tvar j=0;\r\n\t\tfor(i=0;i<objSelTemp.length;i++)\r\n\t\t{\r\n\t\t\tobjOption=objSelTemp.options[i];\r\n\t\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t\t{\r\n\t\t\t \tj+=1;\r\n\t\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\t}\t\t\r\n\t\t}\r\n\t\tonChangeZoneCode();\r\n\t\ttrZone.style.display=\"\";\t\t\r\n\t\ttrGateway.style.display=\"none\";\r\n\t\tif(document.all(\"type\").value=='RESET_LOCK')\r\n\t\t\ttrRole.style.display=\"none\";\r\n\t\telse\r\n\t\t\ttrRole.style.display=\"\";\r\n\t}else{\r\n\t\tdocument.all(\"command\").value = 'rolebehave';\r\n\t\ttrZone.style.display=\"none\";\r\n\t\ttrGateway.style.display=\"\";\r\n\t\ttrRole.style.display=\"\";\t\t\t\r\n\t}\r\n\tobjSelReason.options[objSelReason.length]=new Option((j+1)+\".输入原因...\",\"other\");\r\n\tonChangeReason();\r\n\tonChangeGameId();\r\n}\r\n\r\nfunction focusAccount(){\r\n\tdocument.all(\"role\").value = \"\";\r\n}\r\n\r\nfunction focusRole(){\r\n\tdocument.all(\"account\").value = \"\";\r\n}\r\n\r\nfunction initInputField(){\r\n\tvar rolebehave = \"");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(163,22);to=(163,32)]
                out.print(rolebehave);
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(163,34);to=(164,16)]
                out.write("\";\r\n\tvar game_id = \"");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(164,19);to=(164,26)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(164,28);to=(204,44)]
                out.write("\";\r\n\tif (rolebehave != \"\"){\r\n\t\tvar commandSel = document.all(\"rolebehave\");\r\n\t\tfor (i=0; i<commandSel.length; i++){\r\n\t\t\tif (rolebehave==commandSel.options[i].value){\r\n\t\t\t\tcommandSel.options[i].selected = true;\r\n\t\t\t\tbreak;\r\n\t\t\t}\r\n\t\t}\r\n\t\tdocument.all(\"game_id\").value = game_id;\r\n\t}\r\n}\r\n\r\nfunction onChangeReason()\r\n{\r\n\tvar objSel=document.all(\"selReason\");\r\n\tvar objTr=document.all(\"trReason\");\r\n\tif(objSel.value==\"other\")\r\n\t{\r\n\t\tobjTr.style.display=\"\";\r\n\t\tdocument.all(\"othermessage\").focus();\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjTr.style.display=\"none\";\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">角色行为控制（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\t\t<br><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(204,47);to=(204,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(204,73);to=(205,45)]
                out.write("/GMCServlet\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(205,48);to=(205,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(205,72);to=(221,14)]
                out.write("\">\r\n\t<input type=\"hidden\" name=\"category\" value=\"R_N_CTRL\">\r\n\t<input type=\"hidden\" name=\"command\" value=\"rolebehave\">\r\n\t<input type=\"hidden\" name=\"type\">\r\n\t<input type=\"hidden\" name=\"option\">\r\n\t<input type=\"hidden\" name=\"gateway\">\r\n\t<input type=\"hidden\" name=\"area\">\r\n\t<input type=\"hidden\" name=\"group\">\r\n\t<input type=\"hidden\" name=\"zonecode2\">\t\r\n\t<input name=\"message\" type=\"hidden\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;角色行为控制:冻结/解冻、禁（解除）言、关（解禁）天牢等</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>操作类型：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(221,14);to=(221,131)]
                /* ----  function:functionSelect ---- */
                com.kingsoft.gmsystem.web.taglib.FunctionTag _jspx_th_function_functionSelect_0 = new com.kingsoft.gmsystem.web.taglib.FunctionTag();
                _jspx_th_function_functionSelect_0.setPageContext(pageContext);
                _jspx_th_function_functionSelect_0.setParent(null);
                _jspx_th_function_functionSelect_0.setDisplayCode("select_r_n_ctrl");
                _jspx_th_function_functionSelect_0.setSelectName("rolebehave");
                _jspx_th_function_functionSelect_0.setParameter("behave");
                _jspx_th_function_functionSelect_0.setStyle("width:160");
                try {
                    int _jspx_eval_function_functionSelect_0 = _jspx_th_function_functionSelect_0.doStartTag();
                    if (_jspx_eval_function_functionSelect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.FunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_function_functionSelect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(221,14);to=(221,131)]
                        } while (_jspx_th_function_functionSelect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_function_functionSelect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_function_functionSelect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(221,131);to=(225,14)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(225,14);to=(225,125)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_id");
                _jspx_th_game_gameselect_0.setOnChange("onChangeGameId()");
                _jspx_th_game_gameselect_0.setStyle("width:160");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(225,14);to=(225,125)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(225,125);to=(238,8)]
                out.write("</td>\r\n        </tr>\r\n        <tr id=\"trAccount\">\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap>玩家帐号：</td>\r\n          <td width=\"292\"> <input type=\"text\" name=\"account\" class=\"text\" onfocus=\"javascript:focusAccount();\">&nbsp;&nbsp;<font color=\"#FF0000\">作用于帐号下所有角色</font></td>\r\n        </tr>\r\n        <tr id=\"trRole\">\r\n          <td height=\"30\"  align=\"right\" nowrap>玩家角色：</td>\r\n          <td> <input type=\"text\" name=\"role\" class=\"text\" onfocus=\"javascript:focusRole();\"> </td>\r\n        </tr>\r\n        <tr id=\"trZone\">\r\n          <td height=\"30\"  align=\"right\" nowrap>所在区：</td>\r\n          <td><select name=\"zonecode\" style=\"width:200\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(238,8);to=(239,58)]
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
                        // HTML // begin [file="/gmc/rolebehave.jsp";from=(239,58);to=(240,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(240,8);to=(240,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(240,26);to=(247,12)]
                out.write("\r\n\t\t\t</td>\r\n        </tr>\r\n\t\t\t\t\t\t\t\t\r\n        <tr id=\"trGateway\">\r\n          <td height=\"30\" align=\"right\" nowrap>所在区服：</td>\r\n          <td><select name=\"selGateway\" size='15' style='width:200'></select>\r\n            ");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(247,12);to=(247,116)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_gateway_person");
                _jspx_th_display_display_1.setProperty("selGatewayTemp");
                _jspx_th_display_display_1.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/rolebehave.jsp";from=(247,116);to=(248,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(248,12);to=(248,30)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(248,30);to=(253,6)]
                out.write("\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>操作原因：</td>\r\n          <td><select name=\"selReason\" onChange='onChangeReason()'></select>\r\n\t\t    ");

            // end
            // begin [file="/gmc/rolebehave.jsp";from=(253,6);to=(253,105)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_2 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_2.setPageContext(pageContext);
                _jspx_th_display_display_2.setParent(null);
                _jspx_th_display_display_2.setDisplayCode("select_reason_all");
                _jspx_th_display_display_2.setProperty("selReasonTemp");
                _jspx_th_display_display_2.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_2 = _jspx_th_display_display_2.doStartTag();
                    if (_jspx_eval_display_display_2 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_2 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/rolebehave.jsp";from=(253,105);to=(254,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/rolebehave.jsp";from=(254,12);to=(254,30)]
                        } while (_jspx_th_display_display_2.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_2.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_2.release();
                }
            // end
            // HTML // begin [file="/gmc/rolebehave.jsp";from=(254,30);to=(330,0)]
                out.write("\r\n            </td>\r\n        </tr>\r\n        <tr id=\"trReason\" style=\"display:none\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>请输入原因：</td>\r\n          <td><input name=\"othermessage\" type=\"text\" class=\"text\" size=\"40\" maxlength=\"121\"></td>\r\n        </tr>\r\n        <tr id=\"trTerm\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>行为控制期限：</td>\r\n          <td><select name=\"term\">\r\n            <option value=\"0\" selected>无限</option>\r\n            <option value=\"1\">1</option>\r\n            <option value=\"2\">2</option>\r\n            <option value=\"3\">3</option>\r\n            <option value=\"4\">4</option>\r\n            <option value=\"5\">5</option>\r\n            <option value=\"6\">6</option>\r\n            <option value=\"7\">7</option>\r\n            <option value=\"10\">10</option>\r\n            <option value=\"15\">15</option>\r\n            <option value=\"20\">20</option>\r\n            <option value=\"25\">25</option>\r\n            <option value=\"30\">30</option>\r\n            <option value=\"35\">35</option>\r\n            <option value=\"40\">40</option>\r\n            <option value=\"50\">50</option>\r\n            <option value=\"60\">60</option>\r\n          </select>\r\n            天</td>\r\n        </tr>\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onSubmit()\">确定</button></td>\r\n    <td><button onClick=\"javascript:history.back()\">取消</button></td>\r\n  </tr>\r\n</table><br><br>\r\n提示：您可输入(“玩家角色”+“所在区服”)或“玩家帐号”来对角色行为进行控制。\r\n<script language=\"javascript\">\t\r\n\t\r\ndocument.getElementById(\"zonecode\").onchange = function onChangeZoneCode(){\r\n\t\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value&&objOption.zonecode==document.all(\"zonecode\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t \tif(j==1){\t\t\t\t\r\n\t\t\t\tdocument.all(\"area\").value=objOption.area;\r\n\t \t\t}\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"zonecode\",objOption.zonecode);\r\n\t\t}\r\n\t}\r\n}\r\n\tinitInputField();\r\n\tonChangeCommandType();\r\n</script>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
