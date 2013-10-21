package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class kickplayer$jsp extends HttpJspBase {


    static {
    }
    public kickplayer$jsp( ) {
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

            // HTML // begin [file="/gmc/kickplayer.jsp";from=(0,176);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(4,62);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(5,0);to=(5,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("6kickplayer");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/kickplayer.jsp";from=(5,0);to=(5,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(5,45);to=(12,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>强制下线（踢号）</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(12,2);to=(24,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
                	HashMap requestMap = (HashMap)session.getAttribute("requestMap");
                	String command = "";
                	String game_id = "";
                	if (requestMap != null){
                		command = Util.setNullToEmpty((String)requestMap.get("command"));
                		game_id = Util.setNullToEmpty((String)requestMap.get("game_id"));
                	}
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(24,2);to=(26,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(26,16);to=(26,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(26,42);to=(115,16)]
                out.write("\";\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit()\r\n{\r\n\tvar strErr=\"\";\r\n\tvar objSelCommandType=document.all(\"command\");\r\n\tif (objSelCommandType.value == \"kickaccount\"){\r\n\t\tif(IsEmpty(\"account\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\t} else if (objSelCommandType.value == \"kickrole\"){\r\n\t\tif(IsEmpty(\"role\")) strErr+=\"*玩家角色不能为空\\n\";\r\n\t\tif(IsEmpty(\"selGateway\")) strErr+=\"*区服不能为空\\n\";\r\n\t}\r\n\tif(document.all(\"selReason\").value==\"other\"&& IsEmpty(\"othermessage\"))  strErr+=\"*操作原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tif(document.all(\"selReason\").value!=\"other\")\r\n\t\t{\r\n\t\t\tdocument.all(\"message\").value=document.all(\"selReason\").value;\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tdocument.all(\"message\").value=document.all(\"othermessage\").value;\r\n\t\t}\r\n\t\tif (objSelCommandType.value == \"kickrole\"){\r\n\t\t\tsetGatewayInfo();\r\n\t\t}\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tif(objSel.selectedIndex!=-1){\r\n\t\tdocument.all(\"gateway\").value=objSel.options[objSel.selectedIndex].value;\r\n\t\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\t\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t}\r\n\t}\r\n}\r\n\r\nfunction onChangeKickType()\r\n{\r\n\tvar objSelKickType=document.all(\"command\");\r\n\tif (objSelKickType.value == \"kickaccount\"){\r\n\t\tdocument.all(\"role\").value = \"\";\r\n\t\tdocument.all(\"trAccount\").style.display = \"\";\r\n\t\tdocument.all(\"trGateway\").style.display = \"none\";\r\n\t\tdocument.all(\"trRole\").style.display = \"none\";\r\n\t} else if (objSelKickType.value == \"kickrole\"){\r\n\t\tdocument.all(\"account\").value = \"\";\r\n\t\tdocument.all(\"trAccount\").style.display = \"none\";\r\n\t\tdocument.all(\"trGateway\").style.display = \"\";\r\n\t\tdocument.all(\"trRole\").style.display = \"\";\r\n\t}\r\n\tonChangeGameId();\r\n}\r\n\r\nfunction initInputField(){\r\n\tvar command = \"");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(115,19);to=(115,26)]
                out.print(command);
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(115,28);to=(116,16)]
                out.write("\";\r\n\tvar game_id = \"");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(116,19);to=(116,26)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(116,28);to=(156,44)]
                out.write("\";\r\n\tif (command != \"\"){\r\n\t\tvar commandSel = document.all(\"command\");\r\n\t\tfor (i=0; i<commandSel.length; i++){\r\n\t\t\tif (command==commandSel.options[i].value){\r\n\t\t\t\tcommandSel.options[i].selected = true;\r\n\t\t\t\tbreak;\r\n\t\t\t}\r\n\t\t}\r\n\t\tdocument.all(\"game_id\").value = game_id;\r\n\t}\r\n}\r\n\r\nfunction onChangeReason()\r\n{\r\n\tvar objSel=document.all(\"selReason\");\r\n\tvar objTr=document.all(\"trReason\");\r\n\tif(objSel.value==\"other\")\r\n\t{\r\n\t\tobjTr.style.display=\"\";\r\n\t\tdocument.all(\"othermessage\").focus();\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjTr.style.display=\"none\";\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">强制下线踢人（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div><br><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(156,47);to=(156,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(156,73);to=(157,45)]
                out.write("/GMCServlet\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(157,48);to=(157,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(157,72);to=(169,14)]
                out.write("\">\r\n\t<input type=\"hidden\" name=\"category\" value=\"R_N_CTRL\">\r\n\t<input type=\"hidden\" name=\"message\">\r\n\t<input type=\"hidden\" name=\"gateway\">\r\n\t<input type=\"hidden\" name=\"area\">\r\n\t<input type=\"hidden\" name=\"group\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;强制下线（踢人）</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>操作类型：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(169,14);to=(169,126)]
                /* ----  function:functionSelect ---- */
                com.kingsoft.gmsystem.web.taglib.FunctionTag _jspx_th_function_functionSelect_0 = new com.kingsoft.gmsystem.web.taglib.FunctionTag();
                _jspx_th_function_functionSelect_0.setPageContext(pageContext);
                _jspx_th_function_functionSelect_0.setParent(null);
                _jspx_th_function_functionSelect_0.setDisplayCode("select_r_n_ctrl");
                _jspx_th_function_functionSelect_0.setSelectName("command");
                _jspx_th_function_functionSelect_0.setParameter("kick");
                _jspx_th_function_functionSelect_0.setStyle("width:160");
                try {
                    int _jspx_eval_function_functionSelect_0 = _jspx_th_function_functionSelect_0.doStartTag();
                    if (_jspx_eval_function_functionSelect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.FunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_function_functionSelect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/kickplayer.jsp";from=(169,14);to=(169,126)]
                        } while (_jspx_th_function_functionSelect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_function_functionSelect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_function_functionSelect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(169,126);to=(173,14)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(173,14);to=(173,125)]
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
                        // begin [file="/gmc/kickplayer.jsp";from=(173,14);to=(173,125)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(173,125);to=(186,12)]
                out.write("</td>\r\n        </tr>\r\n        <tr id=\"trAccount\">\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>玩家帐号：</td>\r\n          <td width=\"292\"> <input type=\"text\" name=\"account\" class=\"text\"> </td>\r\n        </tr>\r\n        <tr id=\"trRole\">\r\n          <td height=\"30\"  align=\"right\" nowrap><font color=\"#FF0000\">*</font>玩家角色：</td>\r\n          <td> <input type=\"text\" name=\"role\" class=\"text\"> </td>\r\n        </tr>\r\n        <tr id=\"trGateway\">\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所在区服：</td>\r\n          <td><select name=\"selGateway\" size='15' style='width:200'></select>\r\n            ");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(186,12);to=(186,116)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_gateway_person");
                _jspx_th_display_display_0.setProperty("selGatewayTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/kickplayer.jsp";from=(186,116);to=(187,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/kickplayer.jsp";from=(187,12);to=(187,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(187,30);to=(191,14)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>操作原因：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/kickplayer.jsp";from=(191,14);to=(191,97)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_reason");
                _jspx_th_display_display_1.setProperty("selReason");
                _jspx_th_display_display_1.setParameter("kick");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/kickplayer.jsp";from=(191,97);to=(192,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/kickplayer.jsp";from=(192,12);to=(192,30)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/kickplayer.jsp";from=(192,30);to=(219,0)]
                out.write("\r\n            </td>\r\n        </tr>\r\n        <tr id=\"trReason\" style=\"display:none\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>请输入原因：</td>\r\n          <td><input name=\"othermessage\" type=\"text\" class=\"text\" size=\"40\" maxlength=\"121\"></td>\r\n        </tr>\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onSubmit()\">确定</button></td>\r\n    <td><button onClick=\"javascript:history.back()\">取消</button></td>\r\n  </tr>\r\n</table><br><br>\r\n<script language=\"javascript\">\r\n\tinitInputField();\r\n\tonChangeKickType();\r\n</script>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
