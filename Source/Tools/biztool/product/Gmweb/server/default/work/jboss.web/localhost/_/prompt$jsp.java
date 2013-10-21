package org.apache.jsp;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class prompt$jsp extends HttpJspBase {


    static {
    }
    public prompt$jsp( ) {
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

            // HTML // begin [file="/prompt.jsp";from=(0,67);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/prompt.jsp";from=(1,2);to=(10,0)]
                
                
                String path = request.getContextPath();
                String basePath = request.getScheme() + "://"
                		+ request.getServerName() + ":" + request.getServerPort()
                		+ path + "/";
                		String jumpPath = (String)request.getAttribute("jumpPath");
                		
                			
            // end
            // HTML // begin [file="/prompt.jsp";from=(10,2);to=(17,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>系统提示</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/prompt.jsp";from=(17,15);to=(17,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/prompt.jsp";from=(17,41);to=(19,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/prompt.jsp";from=(19,16);to=(19,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/prompt.jsp";from=(19,42);to=(21,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/prompt.jsp";from=(21,38);to=(21,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/prompt.jsp";from=(21,64);to=(43,97)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" --><!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统提示</span><!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n      <table width=\"100%\" height=\"100\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\" valign=\"middle\"><br><br><br><br><br><br><br><br><br>\r\n            <table width=\"1\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#000000\" class=\"tablelist\">\r\n              <tr>\r\n                <td>\r\n                  <table width=\"272\" border=\"0\" align=\"center\" cellspacing=\"0\" class=\"tablelist\">\r\n                    <thead>\r\n\t\t\t\t\t<tr>\r\n                      <td colspan=\"2\" align=\"left\">&nbsp;<font color=\"#FFFFFF\">系统提示</font></td>\r\n                    </tr></thead>\r\n                    <tr>\r\n                      <td width=\"50\" align=\"center\" valign=\"top\" bgcolor=\"#F3FBFE\"><br><img src=\"");

            // end
            // begin [file="/prompt.jsp";from=(43,100);to=(43,124)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/prompt.jsp";from=(43,126);to=(47,36)]
                out.write("/images/info.gif\" width=\"32\" height=\"32\"></td>\r\n                      <td width=\"220\" align=\"center\" bgcolor=\"#F3FBFE\">\r\n                        <table width=\"196\" height=\"23\" border=\"0\" cellspacing=\"0\">\r\n                          <tr>\r\n                            <td><br>");

            // end
            // begin [file="/prompt.jsp";from=(47,39);to=(47,78)]
                out.print((String)request.getAttribute("message"));
            // end
            // HTML // begin [file="/prompt.jsp";from=(47,80);to=(55,11)]
                out.write("</td>\r\n                          </tr>\r\n                        </table>\r\n                      </td>\r\n                    </tr>\r\n                    \r\n                    <tr>\r\n                      <td colspan=\"2\" align=\"center\" bgcolor=\"#F3FBFE\">\r\n        \t\t\t");

            // end
            // begin [file="/prompt.jsp";from=(55,13);to=(57,17)]
                
                        				if(null!=jumpPath){
                                	
            // end
            // HTML // begin [file="/prompt.jsp";from=(57,19);to=(58,75)]
                out.write("\r\n                            <button onClick=\"javascript:document.location='");

            // end
            // begin [file="/prompt.jsp";from=(58,78);to=(58,95)]
                out.print(basePath+jumpPath);
            // end
            // HTML // begin [file="/prompt.jsp";from=(58,97);to=(59,14)]
                out.write("'\">返回</button>\r\n            \t\t");

            // end
            // begin [file="/prompt.jsp";from=(59,16);to=(62,11)]
                
                        					
                        				}else{
                        			
            // end
            // HTML // begin [file="/prompt.jsp";from=(62,13);to=(64,11)]
                out.write("\r\n                        <button onClick=\"javascript:history.back()\">返回</button>\r\n        \t\t\t");

            // end
            // begin [file="/prompt.jsp";from=(64,13);to=(66,11)]
                
                        				}
                        			
            // end
            // HTML // begin [file="/prompt.jsp";from=(66,13);to=(85,0)]
                out.write("              \r\n                        \r\n                      </td>\r\n                    </tr>\r\n                  </table>\r\n                </td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n      </table>\r\n      <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
