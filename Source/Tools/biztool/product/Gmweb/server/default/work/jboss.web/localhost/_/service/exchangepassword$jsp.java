package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class exchangepassword$jsp extends HttpJspBase {


    static {
    }
    public exchangepassword$jsp( ) {
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

            // HTML // begin [file="/service/exchangepassword.jsp";from=(0,142);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(3,0);to=(3,50)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("exchangepassword");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/service/exchangepassword.jsp";from=(3,0);to=(3,50)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(3,50);to=(12,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>临时置换密码</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(12,16);to=(12,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(12,42);to=(16,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(16,2);to=(21,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(21,2);to=(97,44)]
                out.write("\r\n<script language=\"javascript\">\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onExchange()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"account\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar objGameid=document.all(\"game_id\");\r\n\t\tvar game_name=objGameid.options[objGameid.selectedIndex].text;\r\n\t\tvar account=document.all(\"account\").value;\r\n\t\tif(confirm(\"你确定要临时置换\"+game_name+\"玩家\"+account+\"的一级(二级)密码吗？\"))\r\n\t\t{\r\n\t\t\tdocument.all(\"action\").value=\"exchangepassword\";\r\n\t\t\tsetButtonState(true);\r\n\t\t\tdocument.all(\"frmSubmit\").submit();\r\n\t\t}\r\n\t}\r\n}\r\n\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onResume()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"account\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\tif(IsEmpty(\"message\")) strErr+=\"*置换原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"resumepassword\";\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\nfunction onChangeGameId()\r\n{\r\n\t\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n}\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">临时置换密码</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<br><br><br><br><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(97,47);to=(97,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(97,73);to=(110,56)]
                out.write("/GMServlet\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;临时置换密码</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>\r\n            <select name=\"game_id\" onmouseover=\"this.focus()\" >\r\n                     <option value=\"88\">2.剑网3</option>\r\n                     <option value=\"77\">1.剑侠世界</option>\r\n                  </select>\r\n            <input type=\"hidden\" name=\"action\" value=\"\">\r\n            <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(110,59);to=(110,81)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(110,83);to=(115,8)]
                out.write("\">\r\n</td>\r\n        <tr>\r\n          <td height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所在区：</td>\r\n          <td><select name=\"zonecode\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/service/exchangepassword.jsp";from=(115,8);to=(116,58)]
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
                        // HTML // begin [file="/service/exchangepassword.jsp";from=(116,58);to=(117,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/service/exchangepassword.jsp";from=(117,8);to=(117,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/service/exchangepassword.jsp";from=(117,26);to=(148,0)]
                out.write("</td>\r\n        </tr>\r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>玩家帐号：</td>\r\n          <td width=\"292\"> <input type=\"text\" name=\"account\" class=\"text\"> </td>\r\n        </tr>\r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>置换原因：</td>\r\n          <td width=\"292\"> <input type=\"text\" name=\"message\" class=\"text\"> </td>\r\n        </tr>\r\n\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onExchange()\">置换密码</button></td>\r\n    <td><button onClick=\"onResume()\">恢复密码</button></td>\r\n  </tr>\r\n</table><br><br>\r\n提示：在临时置换帐号密码后，若在30分钟内没有被手工恢复密码，则系统会自动恢复玩家帐号的密码。恢复密码操作不用选择玩家所在区。\r\n\t<!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<script type=\"text/javascript\">\r\nonChangeGameId();\r\n</script>\r\n<!-- InstanceEnd --></html>\r\n");

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
