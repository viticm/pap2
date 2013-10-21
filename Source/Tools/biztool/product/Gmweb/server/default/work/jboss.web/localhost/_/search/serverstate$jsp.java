package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class serverstate$jsp extends HttpJspBase {


    static {
    }
    public serverstate$jsp( ) {
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

            // HTML // begin [file="/search/serverstate.jsp";from=(0,89);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/serverstate.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/serverstate.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/serverstate.jsp";from=(3,0);to=(3,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("serverstate");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/serverstate.jsp";from=(3,0);to=(3,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/serverstate.jsp";from=(3,45);to=(12,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>服务器状态</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/serverstate.jsp";from=(12,16);to=(12,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/serverstate.jsp";from=(12,42);to=(16,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/serverstate.jsp";from=(16,2);to=(20,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/search/serverstate.jsp";from=(20,2);to=(76,0)]
                out.write("\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">服务器状态</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" --><br>\r\n      <table width=\"704\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;剑网2服务器状态</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"300\" valign=\"top\"\"><iframe name=\"frmFS\" width=\"100%\" height=\"100%\" frameborder=0 scrolling=\"auto\" src=\"http://jx.kingsoft.com/serverstate/jx2state.html\" marginwidth=\"0\" marginheight=\"0\">\r\n          对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe></td>\r\n        </tr>\r\n        <tr >\r\n          <td height=\"31\" align=\"center\" nowrap><button onclick=\"window.location.reload();\">刷新服务器状态</button></td>\r\n        </tr>\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;封神榜服务器状态</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"300\" valign=\"top\"\"><iframe name=\"frmFS\" width=\"100%\" height=\"100%\" frameborder=0 scrolling=\"auto\" src=\"http://jx.kingsoft.com/serverstate/fsstate.html\" marginwidth=\"0\" marginheight=\"0\">\r\n          对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe></td>\r\n        </tr>\r\n        <tr >\r\n          <td height=\"31\" align=\"center\" nowrap><button onclick=\"window.location.reload();\">刷新服务器状态</button></td>\r\n        </tr>\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;剑网服务器状态</b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"300\" valign=\"top\"\"><iframe name=\"frmJX\" width=\"100%\" height=\"100%\" frameborder=0 scrolling=\"auto\" src=\"http://jx.kingsoft.com/serverstate/servernew.html\" marginwidth=\"0\" marginheight=\"0\">\r\n          对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe></td>\r\n        </tr>\r\n        <tr >\r\n          <td height=\"31\" align=\"center\" nowrap><button onclick=\"window.location.reload();\">刷新服务器状态</button></td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
