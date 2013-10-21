package org.apache.jsp;

import com.kingsoft.gmsystem.domain.UserInfo;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class freezeidcard$jsp extends HttpJspBase {


    static {
    }
    public freezeidcard$jsp( ) {
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

            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(0,137);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(2,0);to=(2,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("freezeidcard");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/freezeidcard.jsp";from=(2,0);to=(2,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(2,46);to=(7,12)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>帐号一般控制</title>\r\n<link href=\"");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(7,15);to=(7,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(7,41);to=(8,35)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(8,38);to=(8,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(8,64);to=(9,0)]
                out.write("/js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(9,2);to=(14,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(14,2);to=(16,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(16,16);to=(16,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(16,42);to=(63,44)]
                out.write("\";\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit()\r\n{\r\n\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"idcard\")) strErr+=\"*玩家证件不能为空\\n\";\r\n\t\r\n\tvar cardType = document.all(\"cardType\").value;\r\n\tvar checkResult = \"\";\r\n\tif(cardType==\"0\"){\r\n\t\tcheckResult = checkIdcard(document.all(\"idcard\").value);\r\n\t\tif(checkResult!=true)\r\n\t\t\tstrErr+=\"*\"+checkResult+\"\\n\";\r\n\t}else if(cardType==\"1\"){\r\n\t\tcheckResult = checkMilitaryCard(document.all(\"idcard\").value);\r\n\t\tif(checkResult!=true)\r\n\t\t\tstrErr+=\"*\"+checkResult+\"\\n\";\r\n\t}else if(cardType==\"2\"){\r\n\t\tcheckResult = checkPassport(document.all(\"idcard\").value);\r\n\t\tif(checkResult!=true)\r\n\t\t\tstrErr+=\"*\"+checkResult+\"\\n\";\r\n\t}\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t\treturn false;\r\n\t}else{\r\n\t\treturn true;\r\n\t}\r\n}\r\n\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">按证件号冻结帐号</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\t\t<br><br><br><br><br><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(63,47);to=(63,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(63,73);to=(64,45)]
                out.write("/GMCServlet\" onsubmit=\"return onSubmit()\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/freezeidcard.jsp";from=(64,48);to=(64,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/freezeidcard.jsp";from=(64,72);to=(101,0)]
                out.write("\">\r\n\t<input name=\"action\" type=\"hidden\" value=\"freezeByIdcard\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;按证件号冻结帐号</b></font></td>\r\n        </tr>\r\n        \r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>证件类型：</td>\r\n          <td width=\"292\"><SELECT name=\"cardType\">\r\n                    <OPTION value=\"0\">1.身份证</OPTION>\r\n                    <OPTION value=\"1\">2.军人证</OPTION>\r\n                    <OPTION value=\"2\">3.护照</OPTION>\r\n                  </SELECT><font color=\"#FF0000\">*</font></td>\r\n        </tr>\r\n             \r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>玩家证件号：</td>\r\n          <td width=\"292\"><input type=\"text\" name=\"idCard\" class=\"text\"></td>\r\n        </tr>\r\n\r\n      </table>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td><input type=\"submit\" value=\"确定\"/></td>\r\n    <td><input type=\"reset\" value=\"取消\"/></td>\r\n  </tr>\r\n</table>\r\n\t</form>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
