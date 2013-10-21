package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class flygm$jsp extends HttpJspBase {


    static {
    }
    public flygm$jsp( ) {
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

            // HTML // begin [file="/gmc/flygm.jsp";from=(0,176);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(4,60);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/flygm.jsp";from=(5,0);to=(5,40)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("8flygm");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/flygm.jsp";from=(5,0);to=(5,40)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(5,40);to=(12,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>瞬移</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/flygm.jsp";from=(12,2);to=(24,0)]
                
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
            // HTML // begin [file="/gmc/flygm.jsp";from=(24,2);to=(26,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/flygm.jsp";from=(26,16);to=(26,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(26,42);to=(137,16)]
                out.write("\";\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit()\r\n{\r\n\tvar strErr=\"\";\r\n\tvar objSelG=document.all(\"selGateway\");\r\n\tvar objSel=document.all(\"newposition\");\r\n\tif(IsEmpty(\"role\")) strErr+=\"*游戏角色不能为空\\n\";\r\n\tif(IsEmpty(\"selGateway\"))\r\n\t{\r\n\t strErr+=\"*所在区服不能为空\\n\";\r\n\t }\r\n\tif(document.all(\"command\").value==\"flytodest\")\r\n\t{\r\n\t\tif (IsEmpty(\"newposition\")) strErr+=\"*瞬移位置不能为空\\n\";\r\n\t\tif (!IsEmpty(\"selGateway\")){\r\n\t\tif(objSelG.options[objSelG.selectedIndex].game_id != objSel.options[objSel.selectedIndex].game_id) strErr+=\"*所在区服与瞬移位置不匹配\\n\";;\r\n\t\t}\r\n\t}\r\n\telse if (document.all(\"command\").value==\"flytotarget\")\r\n\t{\r\n\t\tif (IsEmpty(\"dest_role\")) strErr+=\"*玩家角色不能为空\\n\";\r\n\t}\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tsetGatewayInfo();\r\n\t\tif(document.all(\"command\").value==\"flytodest\")\r\n\t\t{\r\n\t\t\tdocument.all(\"positionname\").value=objSel.options[objSel.selectedIndex].positionname;\r\n\t\t}\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction setGatewayInfo()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tif(objSel.selectedIndex!=-1){\r\n\t\tdocument.all(\"gateway\").value=objSel.options[objSel.selectedIndex].value;\r\n\t\tdocument.all(\"area\").value=objSel.options[objSel.selectedIndex].area;\r\n\t\tdocument.all(\"group\").value=objSel.options[objSel.selectedIndex].group;\r\n\t}\r\n}\r\n\r\n/*****************************\r\n改变瞬移类型\r\n******************************/\r\nfunction onChangeAction()\r\n{\r\n\tif(document.all(\"command\").value==\"flytodest\")\r\n\t{\r\n\t\tdocument.all(\"dest_role\").value=\"\";\r\n\t\tdocument.all(\"trDestRole\").style.display=\"none\";\r\n\t\tdocument.all(\"trDestPosi\").style.display=\"\";\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"trDestRole\").style.display=\"\";\r\n\t\tdocument.all(\"trDestPosi\").style.display=\"none\";\r\n\t}\r\n}\r\n\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objSel2=document.all(\"newposition\");\r\n\tvar objSelTemp2=document.all(\"selectPositionTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tobjSel2.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\r\n\t}\r\n\tj=0;\r\n\tfor(i=0;i<objSelTemp2.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp2.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel2.options[objSel2.length]=new Option(j+\".\"+objOption.positionname,objOption.value);\r\n\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"positionname\",objOption.positionname);\r\n\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\r\n\t}\r\n}\r\n\r\nfunction initInputField(){\r\n\tvar command = \"");

            // end
            // begin [file="/gmc/flygm.jsp";from=(137,19);to=(137,26)]
                out.print(command);
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(137,28);to=(138,16)]
                out.write("\";\r\n\tvar game_id = \"");

            // end
            // begin [file="/gmc/flygm.jsp";from=(138,19);to=(138,26)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(138,28);to=(163,44)]
                out.write("\";\r\n\tif (command != \"\"){\r\n\t\tvar commandSel = document.all(\"command\");\r\n\t\tfor (i=0; i<commandSel.length; i++){\r\n\t\t\tif (command==commandSel.options[i].value){\r\n\t\t\t\tcommandSel.options[i].selected = true;\r\n\t\t\t\tbreak;\r\n\t\t\t}\r\n\t\t}\r\n\t\tdocument.all(\"game_id\").value = game_id;\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">瞬移（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/flygm.jsp";from=(163,47);to=(163,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(163,73);to=(164,45)]
                out.write("/GMCServlet\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/flygm.jsp";from=(164,48);to=(164,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(164,72);to=(176,14)]
                out.write("\">\r\n\t<input type=\"hidden\" name=\"category\" value=\"G_M_CTRL\">\r\n\t<input type=\"hidden\" name=\"gateway\">\r\n\t<input type=\"hidden\" name=\"positionname\">\r\n\t<input type=\"hidden\" name=\"area\">\r\n\t<input type=\"hidden\" name=\"group\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;瞬移GM</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" width=\"140\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>瞬移类型：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/flygm.jsp";from=(176,14);to=(176,94)]
                /* ----  flytype:type ---- */
                com.kingsoft.gmsystem.web.taglib.FlyTypeTag _jspx_th_flytype_type_0 = new com.kingsoft.gmsystem.web.taglib.FlyTypeTag();
                _jspx_th_flytype_type_0.setPageContext(pageContext);
                _jspx_th_flytype_type_0.setParent(null);
                _jspx_th_flytype_type_0.setDisplayCode("select_type");
                _jspx_th_flytype_type_0.setSelectName("command");
                _jspx_th_flytype_type_0.setStyle("width:160");
                try {
                    int _jspx_eval_flytype_type_0 = _jspx_th_flytype_type_0.doStartTag();
                    if (_jspx_eval_flytype_type_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.FlyTypeTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_flytype_type_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/flygm.jsp";from=(176,14);to=(176,94)]
                        } while (_jspx_th_flytype_type_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_flytype_type_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_flytype_type_0.release();
                }
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(176,94);to=(180,14)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/flygm.jsp";from=(180,14);to=(180,125)]
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
                        // begin [file="/gmc/flygm.jsp";from=(180,14);to=(180,125)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(180,125);to=(185,12)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所在区服：</td>\r\n          <td><select name=\"selGateway\" size='12' style='width:200'></select>\r\n            ");

            // end
            // begin [file="/gmc/flygm.jsp";from=(185,12);to=(185,116)]
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
                        // HTML // begin [file="/gmc/flygm.jsp";from=(185,116);to=(186,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/flygm.jsp";from=(186,12);to=(186,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(186,30);to=(199,12)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>GM角色：</td>\r\n          <td width=\"284\"> <input type=\"text\" name=\"role\" class=\"text\"> </td>\r\n        </tr>\r\n        <tr id=\"trDestRole\" style=\"display:none\">\r\n          <td height=\"30\"  align=\"right\" nowrap><font color=\"#FF0000\">*</font>目标角色：</td>\r\n          <td> <input type=\"text\" name=\"dest_role\" class=\"text\"> </td>\r\n        </tr>\r\n        <tr id=\"trDestPosi\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>瞬移位置：</td>\r\n          <td><select name=\"newposition\" size='10' style='width:200'></select>\r\n            ");

            // end
            // begin [file="/gmc/flygm.jsp";from=(199,12);to=(199,114)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_position");
                _jspx_th_display_display_1.setProperty("selectPositionTemp");
                _jspx_th_display_display_1.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/flygm.jsp";from=(199,114);to=(200,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/flygm.jsp";from=(200,12);to=(200,30)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/flygm.jsp";from=(200,30);to=(223,0)]
                out.write(" </td>\r\n        </tr>\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onSubmit()\">确定</button></td>\r\n    <td><button onClick=\"javascript:history.back()\">取消</button></td>\r\n  </tr>\r\n</table>\r\n<script language=\"javascript\">\r\n\tinitInputField();\r\n\tonChangeGameId();\r\n\tonChangeAction();\r\n</script>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
