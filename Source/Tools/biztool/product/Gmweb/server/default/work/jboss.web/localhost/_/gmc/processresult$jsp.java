package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import com.kingsoft.gmsystem.socket.gmc.GMCCommandResult;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class processresult$jsp extends HttpJspBase {

    // begin [file="/gmc/processresult.jsp";from=(4,0);to=(4,94)]
    // end

    static {
    }
    public processresult$jsp( ) {
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

            // HTML // begin [file="/gmc/processresult.jsp";from=(0,225);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/processresult.jsp";from=(4,0);to=(4,94)]
                com.kingsoft.gmsystem.service.GMHelper GMHelperId = null;
                boolean _jspx_specialGMHelperId  = false;
                 synchronized (session) {
                    GMHelperId= (com.kingsoft.gmsystem.service.GMHelper)
                    pageContext.getAttribute("GMHelperId",PageContext.SESSION_SCOPE);
                    if ( GMHelperId == null ) {
                        _jspx_specialGMHelperId = true;
                        try {
                            GMHelperId = (com.kingsoft.gmsystem.service.GMHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.GMHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.GMHelper", exc);
                        }
                        pageContext.setAttribute("GMHelperId", GMHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialGMHelperId == true) {
            // end
            // begin [file="/gmc/processresult.jsp";from=(4,0);to=(4,94)]
                }
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(4,94);to=(10,35)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>多区服操作执行情况汇总</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/gmc/processresult.jsp";from=(10,38);to=(10,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(10,64);to=(12,13)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/processresult.jsp";from=(12,16);to=(12,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(12,42);to=(14,0)]
                out.write("\";\r\n</script>\r\n");

            // end
            // begin [file="/gmc/processresult.jsp";from=(14,2);to=(24,0)]
                
                String seltype=(String)request.getParameter("seltype");
                String iid=(String)request.getParameter("iid");
                String sn=(String)request.getParameter("sn");
                CommandResult commandResult = GMHelperId.getCommandResult(iid);
                if (seltype == null || seltype.equals("iid")){
                	commandResult = GMHelperId.getCommandResult(iid);
                } else {
                	commandResult = GMHelperId.getLastCommandResult(sn);
                }
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(24,2);to=(43,62)]
                out.write("\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">多区服操作执行情况汇总（基于GCD新协议）</span> </td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n\t<br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <table width=\"715\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"31\" colspan=\"6\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;多区服操作执行情况汇总</b></font></td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"15%\" height=\"25\" align=\"right\">操作类型：</td>\r\n               <td width=\"35%\" height=\"25\" align=\"left\">&nbsp;");

            // end
            // begin [file="/gmc/processresult.jsp";from=(43,65);to=(43,95)]
                out.print(commandResult.getOperateType());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(43,97);to=(45,62)]
                out.write("</td>\r\n               <td width=\"15%\" height=\"25\" align=\"right\">操作人：</td>\r\n               <td width=\"35%\" height=\"25\" align=\"left\">&nbsp;");

            // end
            // begin [file="/gmc/processresult.jsp";from=(45,65);to=(45,92)]
                out.print(commandResult.getOperator());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(45,94);to=(49,62)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"18%\" height=\"25\" align=\"right\">操作流水号：</td>\r\n               <td width=\"32%\" height=\"25\" align=\"left\">&nbsp;");

            // end
            // begin [file="/gmc/processresult.jsp";from=(49,65);to=(49,86)]
                out.print(commandResult.getSn());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(49,88);to=(51,62)]
                out.write("</td>\r\n               <td width=\"18%\" height=\"25\" align=\"right\">操作时间：</td>\r\n               <td width=\"32%\" height=\"25\" align=\"left\">&nbsp;");

            // end
            // begin [file="/gmc/processresult.jsp";from=(51,65);to=(51,95)]
                out.print(commandResult.getOperateTime());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(51,97);to=(53,4)]
                out.write("</td>\r\n             </tr>\r\n\t\t\t ");

            // end
            // begin [file="/gmc/processresult.jsp";from=(53,6);to=(62,3)]
                
                        Iterator resultIterator = commandResult.getCommandResults().iterator();
                        int index = 0;
                		GMCCommandResult groupResult;
                        while (resultIterator.hasNext()){
                            groupResult = (GMCCommandResult) resultIterator.next();
                            if (index % 2 == 0){
                                out.println("<tr>");
                            }
                			
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(62,5);to=(63,65)]
                out.write("\r\n               <td width=\"18%\" height=\"25\"  align=\"right\" nowrap>");

            // end
            // begin [file="/gmc/processresult.jsp";from=(63,68);to=(63,90)]
                out.print(groupResult.getGroup());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(63,92);to=(64,37)]
                out.write("：</td>\r\n               <td width=\"15%\">&nbsp;");

            // end
            // begin [file="/gmc/processresult.jsp";from=(64,40);to=(64,63)]
                out.print(groupResult.getResult());
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(64,65);to=(65,3)]
                out.write("</td>\r\n\t\t\t");

            // end
            // begin [file="/gmc/processresult.jsp";from=(65,5);to=(76,4)]
                
                            if (index % 2 == 1){
                                out.println("</tr>");
                            }
                            index++;
                        }
                            if (index % 2 != 0){
                				out.println("<td colspan='2'/>");
                                out.println("</tr>");
                            }
                
                			 
            // end
            // HTML // begin [file="/gmc/processresult.jsp";from=(76,6);to=(97,0)]
                out.write("\r\n           </table>\r\n           <br>\r\n\t\t  </td>\r\n        </tr>\r\n      </table>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
