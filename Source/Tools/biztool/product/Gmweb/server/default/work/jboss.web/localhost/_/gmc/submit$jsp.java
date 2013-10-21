package org.apache.jsp;

import java.util.*;
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
            response.setContentType("text/html; charset=gb2312");
            pageContext = _jspxFactory.getPageContext(this, request, response,
            			"/error.jsp", true, 8192, true);

            application = pageContext.getServletContext();
            config = pageContext.getServletConfig();
            session = pageContext.getSession();
            out = pageContext.getOut();

            // HTML // begin [file="/gmc/submit.jsp";from=(0,148);to=(5,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\r\n<title>问题答复数据提交</title>\r\n");

            // end
            // begin [file="/gmc/submit.jsp";from=(5,2);to=(7,0)]
                
                	String info=Util.setNullToEmpty((String)request.getAttribute("info"));
            // end
            // HTML // begin [file="/gmc/submit.jsp";from=(7,2);to=(9,11)]
                out.write("\r\n<script language=\"javascript\">\r\n\tvar info=\"");

            // end
            // begin [file="/gmc/submit.jsp";from=(9,14);to=(9,18)]
                out.print(info);
            // end
            // HTML // begin [file="/gmc/submit.jsp";from=(9,20);to=(18,51)]
                out.write("\";\r\n\tif(info!=\"\")\r\n\t{\r\n\t\talert(info);\r\n\t}\r\n</script>\r\n</head>\r\n<body leftmargin=\"0\" topmargin=\"0\">\r\n问题答复数据提交\r\n      <form name=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/submit.jsp";from=(18,54);to=(18,80)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/submit.jsp";from=(18,82);to=(40,0)]
                out.write("/GMCServlet\">\r\n\t  <input type=\"hidden\" name=\"category\" value=\"R_N_CTRL\">\r\n\t  <input type=\"hidden\" name=\"command\" value=\"chatrole\">\r\n        <input type=\"hidden\" name=\"questionid\">\r\n        <input type=\"hidden\" name=\"quiztime\">\r\n        <input type=\"hidden\" name=\"gateway\">\r\n        <input type=\"hidden\" name=\"game_id\">\r\n        <input type=\"hidden\" name=\"area\">\r\n        <input type=\"hidden\" name=\"group\">\r\n        <input type=\"hidden\" name=\"account\">\r\n        <input type=\"hidden\" name=\"role\">\r\n        <input type=\"hidden\" name=\"message\">\r\n        <input type=\"hidden\" name=\"usercode\">\r\n        <input type=\"hidden\" name=\"question\">\r\n        <input type=\"hidden\" name=\"answer\">\r\n        <input type=\"hidden\" name=\"action\">\r\n        <input type=\"hidden\" name=\"allquestion\">\r\n        <input type=\"hidden\" name=\"term\">\r\n        <input type=\"hidden\" name=\"channel\" value=\"全部\">\r\n      </form>\r\n</body>\r\n</html>\r\n");

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
