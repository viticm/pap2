package org.apache.jsp;

import com.kingsoft.gmsystem.domain.UserInfo;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class topbanner$jsp extends HttpJspBase {


    static {
    }
    public topbanner$jsp( ) {
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

            // HTML // begin [file="/topbanner.jsp";from=(0,136);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/topbanner.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/topbanner.jsp";from=(2,0);to=(2,43)]
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
                        // begin [file="/topbanner.jsp";from=(2,0);to=(2,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/topbanner.jsp";from=(2,43);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/topbanner.jsp";from=(3,2);to=(8,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
            // end
            // HTML // begin [file="/topbanner.jsp";from=(8,2);to=(18,107)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>topbanner</title>\r\n<link href=\"css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"75\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"48\" align=\"right\" valign=\"bottom\" background=\"images/logo.gif\"><font color=\"#FFFFFF\"><b>用户：");

            // end
            // begin [file="/topbanner.jsp";from=(18,110);to=(18,160)]
                out.print(userinfo.getUsercode()+"  "+userinfo.getUsername());
            // end
            // HTML // begin [file="/topbanner.jsp";from=(18,162);to=(26,0)]
                out.write("</b></font>&nbsp;&nbsp;<a href=\"login.jsp?action=exit\" target=\"_top\"><b><font color=\"#FFFFFF\">[退出系统]</font></b></a>&nbsp;&nbsp;</td>\r\n  </tr>\r\n  <tr>\r\n    <td><img src=\"images/top_td_bg.gif\" alt=\"金山GM系统\" width=\"100%\" height=\"31\" border=\"0\"></td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
