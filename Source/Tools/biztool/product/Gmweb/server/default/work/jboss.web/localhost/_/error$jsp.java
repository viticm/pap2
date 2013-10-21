package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class error$jsp extends HttpJspBase {


    static {
    }
    public error$jsp( ) {
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
            			"", true, 8192, true);

            application = pageContext.getServletContext();
            config = pageContext.getServletConfig();
            session = pageContext.getSession();
            out = pageContext.getOut();

            // HTML // begin [file="/error.jsp";from=(0,67);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/error.jsp";from=(1,2);to=(10,0)]
                
                
                String path = request.getContextPath();
                String basePath = request.getScheme() + "://"
                		+ request.getServerName() + ":" + request.getServerPort()
                		+ path + "/";
                		String jumpPath = (String)request.getAttribute("jumpPath");
                		
                			
            // end
            // HTML // begin [file="/error.jsp";from=(10,2);to=(15,12)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>系统提示</title>\r\n<link href=\"");

            // end
            // begin [file="/error.jsp";from=(15,15);to=(15,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/error.jsp";from=(15,41);to=(17,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/error.jsp";from=(17,16);to=(17,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/error.jsp";from=(17,42);to=(19,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/error.jsp";from=(19,38);to=(19,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/error.jsp";from=(19,64);to=(41,6)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\n<!--\r\nfunction BackLastPage()\r\n{\r\n\tif(event)\r\n\t{\r\n\t\tvar keyCode = event.keyCode;\r\n\t\tif(keyCode==13)\r\n\t\t{\r\n\t\t\thistory.back();\r\n\t\t}\r\n\t}\r\n}\r\n\r\n//-->\r\n</script>\r\n</head>\r\n<body onkeydown=\"BackLastPage()\">\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">\r\n      ");

            // end
            // begin [file="/error.jsp";from=(41,8);to=(54,1)]
                  String errMsg = exception.getMessage();
                	  exception.printStackTrace();
                		if (errMsg == null)	{
                
                			errMsg = exception.toString()+"\n没有错误消息!";
                		}
                
                		if (errMsg.equals("unlogin")){
                
                			RequestDispatcher dispatcher =
                					getServletConfig().getServletContext().getRequestDispatcher(request.getContextPath()+"/cancel.jsp");
                			dispatcher.forward(request, response);
                		}
                	
            // end
            // HTML // begin [file="/error.jsp";from=(54,3);to=(64,91)]
                out.write("\r\n      <table width=\"1\" border=\"1\" align=\"center\" cellspacing=\"0\"  bordercolor=\"#000000\" class=\"tablelist\">\r\n        <tr>\r\n          <td> <table width=\"272\" border=\"0\" align=\"center\" cellspacing=\"0\" class=\"tablelist\">\r\n              <thead>\r\n                <tr>\r\n                  <td colspan=\"2\" align=\"left\">&nbsp;<font color=\"#FFFFFF\">系统提示</font></td>\r\n                </tr>\r\n              </thead>\r\n              <tr>\r\n                <td width=\"50\" align=\"center\" valign=\"top\" bgcolor=\"#F3FBFE\"><br><img src=\"");

            // end
            // begin [file="/error.jsp";from=(64,94);to=(64,118)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/error.jsp";from=(64,120);to=(67,30)]
                out.write("/images/warning.gif\" width=\"32\" height=\"32\"></td>\r\n                <td width=\"220\" align=\"center\" bgcolor=\"#F3FBFE\"> <table width=\"196\" height=\"23\" border=\"0\" cellspacing=\"0\">\r\n                    <tr>\r\n                      <td><br>");

            // end
            // begin [file="/error.jsp";from=(67,33);to=(67,39)]
                out.print(errMsg);
            // end
            // HTML // begin [file="/error.jsp";from=(67,41);to=(74,11)]
                out.write("</td>\r\n                    </tr>\r\n                  </table></td>\r\n              </tr>\r\n              <tr>\r\n                <td colspan=\"2\" align=\"center\" bgcolor=\"#F3FBFE\">\r\n                \r\n        \t\t\t");

            // end
            // begin [file="/error.jsp";from=(74,13);to=(76,17)]
                
                        				if(null!=jumpPath){
                                	
            // end
            // HTML // begin [file="/error.jsp";from=(76,19);to=(77,75)]
                out.write("\r\n                            <button onClick=\"javascript:document.location='");

            // end
            // begin [file="/error.jsp";from=(77,78);to=(77,95)]
                out.print(basePath+jumpPath);
            // end
            // HTML // begin [file="/error.jsp";from=(77,97);to=(78,14)]
                out.write("'\">返回</button>\r\n            \t\t");

            // end
            // begin [file="/error.jsp";from=(78,16);to=(81,11)]
                
                        					
                        				}else{
                        			
            // end
            // HTML // begin [file="/error.jsp";from=(81,13);to=(83,11)]
                out.write("\r\n                        <button onClick=\"javascript:history.back()\">返回</button>\r\n        \t\t\t");

            // end
            // begin [file="/error.jsp";from=(83,13);to=(85,11)]
                
                        				}
                        			
            // end
            // HTML // begin [file="/error.jsp";from=(85,13);to=(95,7)]
                out.write("    \r\n        \t\t\t\r\n                </td>\r\n              </tr>\r\n            </table></td>\r\n        </tr>\r\n      </table></td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>");

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
