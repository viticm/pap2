package org.apache.jsp;

import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class submit$jsp extends HttpJspBase {


    static {
    }
    public submit$jsp( ) {
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

            // HTML // begin [file="/system/submit.jsp";from=(0,134);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/submit.jsp";from=(1,2);to=(5,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/system/submit.jsp";from=(5,2);to=(10,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>提交用户权限管理数据</title>\r\n");

            // end
            // begin [file="/system/submit.jsp";from=(10,2);to=(12,0)]
                
                	String info=Util.setNullToEmpty((String)request.getAttribute("info"));
            // end
            // HTML // begin [file="/system/submit.jsp";from=(12,2);to=(14,11)]
                out.write("\r\n<script language=\"javascript\">\r\n\tvar info=\"");

            // end
            // begin [file="/system/submit.jsp";from=(14,14);to=(14,18)]
                out.print(info);
            // end
            // HTML // begin [file="/system/submit.jsp";from=(14,20);to=(23,35)]
                out.write("\";\r\n\tif(info!=\"\")\r\n\t{\r\n\t\talert(info);\r\n\t\tparent.window.location.reload();\r\n\t}\r\n</script>\r\n</head>\r\n<body leftmargin=\"0\" topmargin=\"0\">数据提交页\r\n      <form name=\"frmData\" action=\"");

            // end
            // begin [file="/system/submit.jsp";from=(23,38);to=(23,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/submit.jsp";from=(23,64);to=(41,0)]
                out.write("/SystemServlet\" method=\"post\">\r\n        <input type=\"hidden\" name=\"action\">\r\n        <input type=\"hidden\" name=\"iid\">\r\n        <input type=\"hidden\" name=\"usercode\">\r\n        <input type=\"hidden\" name=\"username\">\r\n        <input type=\"hidden\" name=\"mobile\">\r\n\t    <input type=\"hidden\" name=\"ip\">\r\n        <input type=\"hidden\" name=\"groupid\">\r\n        <input type=\"hidden\" name=\"groupname\">\r\n        <input type=\"hidden\" name=\"power\">\r\n        <input type=\"hidden\" name=\"oldgroupid\">\r\n        \r\n        <input type=\"hidden\" name=\"oldgroupname\">\r\n        <input type=\"hidden\" name=\"delChks\">\r\n        <input type=\"hidden\" name=\"addChks\">\r\n</form>\r\n</body>\r\n</html>\r\n");

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
