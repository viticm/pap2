package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class modifypwd$jsp extends HttpJspBase {


    static {
    }
    public modifypwd$jsp( ) {
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

            // HTML // begin [file="/individual/modifypwd.jsp";from=(0,89);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/individual/modifypwd.jsp";from=(3,0);to=(3,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("havelogin");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/individual/modifypwd.jsp";from=(3,0);to=(3,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(3,43);to=(12,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"../Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>修改密码</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/individual/modifypwd.jsp";from=(12,16);to=(12,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(12,42);to=(16,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/individual/modifypwd.jsp";from=(16,2);to=(20,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(20,2);to=(54,20)]
                out.write("\r\n<script language=\"JavaScript\">\r\nfunction CheckField()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"oldpassword\")) strErr=strErr+\"*原登录密码不能为空\\n\";\r\n\tif(IsEmpty(\"newpassword\"))\r\n\t\tstrErr=strErr+\"*新登录密码不能为空\\n\";\r\n\telse\r\n\t{\r\n\t\tif(document.all(\"newpassword\").value.length<6)\r\n\t\t\tstrErr=strErr+\"*新登录密码长度不能少于6位\\n\";\r\n\t\telse\r\n\t\t\tif(document.all(\"newpassword\").value!=document.all(\"newpassword2\").value)\r\n\t\t\t\tstrErr=strErr+\"*新登录密码与确认新密码不一致\\n\";\r\n\t}\r\n\tif(strErr==\"\")\r\n\t{\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmModifyPwd\").submit();\r\n\t\t}\r\n\telse\r\n\t\talert(strErr);\r\n}\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统维护</span> &gt;&gt; <span class=\"title\">修改密码</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <form action=\"");

            // end
            // begin [file="/individual/modifypwd.jsp";from=(54,23);to=(54,47)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/individual/modifypwd.jsp";from=(54,49);to=(96,27)]
                out.write("/SystemServlet\" method=\"post\" name=\"frmModifyPwd\">\r\n\t    <br>\r\n\t    <br>\r\n\t    <br>\r\n\t    <br>\r\n\t    <br>\r\n\t    <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n          <tr align=\"left\" bgcolor=\"#419ADC\">\r\n            <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;修改密码\r\n              <input type=\"hidden\" name=\"action\" value=\"modifypwd\"/>\r\n            </b></font></td>\r\n          </tr>\r\n          <tr>\r\n            <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>原登录密码：</td>\r\n            <td width=\"292\"><input type=\"password\" maxlength=\"20\" name=\"oldpassword\" size=\"20\"/>\r\n(6-20位)            </td>\r\n          </tr>\r\n          <tr>\r\n            <td height=\"30\"  align=\"right\" nowrap><font color=\"#FF0000\">*</font>新登录密码：</td>\r\n            <td><input type=\"password\" maxlength=\"20\" name=\"newpassword\" size=\"20\"/>\r\n(6-20位)            </td>\r\n          </tr>\r\n          <tr>\r\n            <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>确认新密码：</td>\r\n            <td><input type=\"password\" maxlength=\"20\" name=\"newpassword2\" size=\"20\"/>\r\n(6-20位)            </td>\r\n          </tr>\r\n        </table>\r\n      </form>\r\n              <table width=\"208\" height=\"15\" border=\"0\" cellspacing=\"0\">\r\n                <tr>\r\n                  <td align=\"center\"><button onClick=\"CheckField()\"  >确定</button></td>\r\n                  <td align=\"center\"><button onClick=\"javascript:history.back()\"  >取消</button></td>\r\n                </tr>\r\n            </table>\t\r\n\t<!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>");

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
