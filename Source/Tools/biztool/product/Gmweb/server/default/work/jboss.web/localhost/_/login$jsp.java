package org.apache.jsp;

import com.kingsoft.gmsystem.common.ServiceConst;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class login$jsp extends HttpJspBase {


    static {
    }
    public login$jsp( ) {
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

            // HTML // begin [file="/login.jsp";from=(0,123);to=(9,13)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>登录金山GM系统</title>\r\n<link href=\"css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"Javascript\" src=\"js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\n<!--\r\nvar AbsPath=\"");

            // end
            // begin [file="/login.jsp";from=(9,16);to=(9,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/login.jsp";from=(9,42);to=(28,16)]
                out.write("\";\r\nfunction login()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"usercode\")) strErr+=\"*用户名称不能为空\\n\";\r\n\tif(IsEmpty(\"password\")) strErr+=\"*密码不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t\talert(strErr);\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"frmLogin\").submit();\r\n\t}\r\n}\r\n\r\nfunction CheckEnter()\r\n{\r\n\tif(event.keyCode==13) login();\r\n}\r\n\r\nwindow.status=\" ");

            // end
            // begin [file="/login.jsp";from=(28,19);to=(28,39)]
                out.print(ServiceConst.VERINFO);
            // end
            // HTML // begin [file="/login.jsp";from=(28,41);to=(46,49)]
                out.write("\";\r\n\r\n//-->\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"48\" background=\"images/logo.gif\"></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"20\" background=\"images/top_td_bg.gif\"></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\"></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">\r\n      <form  name=\"frmLogin\" method=POST action=\"");

            // end
            // begin [file="/login.jsp";from=(46,52);to=(46,78)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/login.jsp";from=(46,80);to=(84,0)]
                out.write("/SystemServlet\">\r\n              <table width=\"322\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n                <tr align=\"left\" bgcolor=\"#F3FBFE\">\r\n                  <td height=\"31\" colspan=\"2\" nowrap>&nbsp;&nbsp;<img src=\"images/user.gif\" width=\"16\" height=\"16\"><font color=#0662B6><b>&nbsp;&nbsp;欢迎登陆GM——剑网3\r\n                    <input name=\"action\" type=\"hidden\" value=\"login\">\r\n                  </b></font></td>\r\n                </tr>\r\n                <tr>\r\n                  <td width=\"90\" height=\"1\" align=\"right\" nowrap></td>\r\n                  <td width=\"232\"></td>\r\n                </tr>\r\n                <tr>\r\n                  <td height=\"30\" align=\"right\" nowrap>用户名称：</td>\r\n                  <td><input name=\"usercode\" type=\"text\" size=\"25\" maxlength=\"20\" class=\"text\" onMouseOver=\"this.focus()\" >\r\n                  </td>\r\n                </tr>\r\n                <tr>\r\n                  <td height=\"30\" align=\"right\" nowrap>密&nbsp;&nbsp;&nbsp;&nbsp;码：</td>\r\n                  <td><input name=\"password\" type=\"password\" size=\"20\" maxlength=\"20\" class=\"text\" onMouseOver=\"this.focus()\" onKeyPress=\"CheckEnter()\">\r\n                  <button onClick=\"login()\">登录</button></td>\r\n                </tr>\r\n                <tr align=\"left\" bgcolor=\"#FFFFFF\">\r\n                  <td height=\"51\" colspan=\"2\" nowrap>&nbsp;&nbsp;其它登陆：<a href=\"http://61.152.111.74:8080/index.html\">剑网2 GM系统</a>&nbsp;&nbsp;<a href=\"http://61.152.111.36/index.html\">水浒 GM系统</a>&nbsp;&nbsp;<a href=\"http://61.152.111.51/index.html\">封神榜 GM系统</a></td>\r\n                </tr>\r\n              </table>\r\n      </form>\r\n<script language=\"JavaScript\">\r\n\tdocument.all(\"usercode\").focus();\r\n</script>\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" valign=\"middle\" bgcolor=\"#BBDDFD\">Copyright\r\n      &copy;  2005 Kingsoft Corp,All Rights Reserved </td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

            // end
            // begin [file="/login.jsp";from=(84,2);to=(90,0)]
                
                  String action = request.getParameter("action");
                  if (action != null && action.equals("exit")) {
                    HttpSession sess = request.getSession();
                    sess.invalidate();
                  }
            // end
            // HTML // begin [file="/login.jsp";from=(90,2);to=(92,0)]
                out.write("\r\n\r\n");

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
