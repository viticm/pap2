package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class home$jsp extends HttpJspBase {


    static {
    }
    public home$jsp( ) {
    }

    private static boolean _jspx_inited = false;

    public final void _jspx_init() throws org.apache.jasper.runtime.JspException {
    }

    public void _jspService(HttpServletRequest request, HttpServletResponse  response)
        throws java.io.IOException, ServletException {

        JspFactory _jspxFactory = null;
        PageContext pageContext = null;
        HttpSession session = null;
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

            // HTML // begin [file="/home.jsp";from=(0,71);to=(11,16)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>您已登录金山GM系统-剑网3</title>\r\n</head>\r\n\r\n<frameset rows=\"53,*\" cols=\"*\" frameborder=\"NO\" border=\"0\" framespacing=\"0\">\r\n  <frame src=\"topbanner.jsp\" name=\"topFrame\" scrolling=\"NO\" noresize >\r\n  <frameset rows=\"*\" cols=\"175,*\" name=\"mainSet\" framespacing=\"0\" frameborder=\"NO\" border=\"0\">\r\n    <frame src=\"menutree.jsp\" name=\"leftFrame\" scrolling=\"NO\">\r\n    <frame src=\"");

            // end
            // begin [file="/home.jsp";from=(11,19);to=(11,43)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/home.jsp";from=(11,45);to=(18,0)]
                out.write("/gmc/integrate.jsp\" name=\"mainFrame\">\r\n  </frameset>\r\n</frameset>\r\n<noframes><body>\r\n</body></noframes>\r\n</html>\r\n\r\n");

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
