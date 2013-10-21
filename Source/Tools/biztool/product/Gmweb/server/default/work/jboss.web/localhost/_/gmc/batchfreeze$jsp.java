package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class batchfreeze$jsp extends HttpJspBase {


    static {
    }
    public batchfreeze$jsp( ) {
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

            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(0,176);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(4,62);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(5,0);to=(5,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("batchfreeze");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/batchfreeze.jsp";from=(5,0);to=(5,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(5,45);to=(12,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>批量帐号冻结</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(12,2);to=(24,0)]
                
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
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(24,2);to=(26,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(26,16);to=(26,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(26,42);to=(57,16)]
                out.write("\";\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"accounts\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\tif(document.all(\"selReason\").value==\"other\"&& IsEmpty(\"othermessage\"))  strErr+=\"*冻结帐号原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tif(document.all(\"selReason\").value!=\"other\")\r\n\t\t{\r\n\t\t\tdocument.all(\"message\").value=document.all(\"selReason\").value;\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tdocument.all(\"message\").value=document.all(\"othermessage\").value;\r\n\t\t}\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\nfunction initInputField(){\r\n\tvar game_id = \"");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(57,19);to=(57,26)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(57,28);to=(107,44)]
                out.write("\";\r\n\tif (game_id != \"\"){\r\n\t\tdocument.all(\"game_id\").value = game_id;\r\n\t}\r\n\r\n\tvar objSelReason=document.all(\"selReason\");\r\n\tvar objSelReasonTemp=document.all(\"selReasonTemp\");\r\n\tvar objOption;\r\n\tobjSelReason.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelReasonTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelReasonTemp.options[i];\r\n\t\tif(objOption.commandType==\"freeze\")\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSelReason.options[objSelReason.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t}\r\n\t}\r\n\tobjSelReason.options[objSelReason.length]=new Option((j+1)+\".输入原因...\",\"other\");\r\n\tonChangeReason();\r\n}\r\n\r\nfunction onChangeReason()\r\n{\r\n\tvar objSel=document.all(\"selReason\");\r\n\tvar objTr=document.all(\"trReason\");\r\n\tif(objSel.value==\"other\")\r\n\t{\r\n\t\tobjTr.style.display=\"\";\r\n\t\tdocument.all(\"othermessage\").focus();\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjTr.style.display=\"none\";\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">批量帐号冻结（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\t\t<br><br><br><br><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(107,47);to=(107,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(107,73);to=(109,45)]
                out.write("/GMCServlet\">\r\n\t<input type=\"hidden\" name=\"action\" value=\"batchfreeze\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(109,48);to=(109,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(109,72);to=(117,14)]
                out.write("\">\r\n\t<input name=\"message\" type=\"hidden\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;批量帐号冻结</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(117,14);to=(117,97)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_id");
                _jspx_th_game_gameselect_0.setStyle("width:160");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/batchfreeze.jsp";from=(117,14);to=(117,97)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(117,97);to=(126,6)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>玩家帐号：<br>(以半角分号\";\"分割)</td>\r\n          <td width=\"292\"> <textarea name=\"accounts\" cols=\"50\" rows=\"5\"></textarea></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>操作原因：</td>\r\n          <td><select name=\"selReason\" onChange='onChangeReason()'></select>\r\n\t\t    ");

            // end
            // begin [file="/gmc/batchfreeze.jsp";from=(126,6);to=(126,105)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_reason_all");
                _jspx_th_display_display_0.setProperty("selReasonTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/batchfreeze.jsp";from=(126,105);to=(127,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/batchfreeze.jsp";from=(127,12);to=(127,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreeze.jsp";from=(127,30);to=(176,0)]
                out.write("\r\n            </td>\r\n        </tr>\r\n        <tr id=\"trReason\" style=\"display:none\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>请输入原因：</td>\r\n          <td><input name=\"othermessage\" type=\"text\" class=\"text\" size=\"40\" maxlength=\"121\"></td>\r\n        </tr>\r\n        <tr id=\"trTerm\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>冻结帐号期限：</td>\r\n          <td><select name=\"term\">\r\n            <option value=\"0\" selected>无限</option>\r\n            <option value=\"1\">1</option>\r\n            <option value=\"2\">2</option>\r\n            <option value=\"3\">3</option>\r\n            <option value=\"4\">4</option>\r\n            <option value=\"5\">5</option>\r\n            <option value=\"6\">6</option>\r\n            <option value=\"7\">7</option>\r\n            <option value=\"10\">10</option>\r\n            <option value=\"15\">15</option>\r\n            <option value=\"20\">20</option>\r\n            <option value=\"25\">25</option>\r\n            <option value=\"30\">30</option>\r\n            <option value=\"35\">35</option>\r\n            <option value=\"40\">40</option>\r\n            <option value=\"50\">50</option>\r\n            <option value=\"60\">60</option>\r\n          </select>\r\n            天</td>\r\n        </tr>\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onSubmit()\">确定</button></td>\r\n    <td><button onClick=\"javascript:history.back()\">取消</button></td>\r\n  </tr>\r\n</table>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n<script language=\"javascript\">\r\n\tinitInputField();\r\n</script>\r\n</body>\r\n</html>\r\n");

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
