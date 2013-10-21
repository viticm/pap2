package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class fillcard$jsp extends HttpJspBase {

    // begin [file="/search/fillcard.jsp";from=(1,0);to=(1,102)]
    // end

    static {
    }
    public fillcard$jsp( ) {
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

            // HTML // begin [file="/search/fillcard.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/fillcard.jsp";from=(1,0);to=(1,102)]
                com.kingsoft.gmsystem.service.SearchHelper SearchHelperId = null;
                boolean _jspx_specialSearchHelperId  = false;
                 synchronized (session) {
                    SearchHelperId= (com.kingsoft.gmsystem.service.SearchHelper)
                    pageContext.getAttribute("SearchHelperId",PageContext.SESSION_SCOPE);
                    if ( SearchHelperId == null ) {
                        _jspx_specialSearchHelperId = true;
                        try {
                            SearchHelperId = (com.kingsoft.gmsystem.service.SearchHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SearchHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SearchHelper", exc);
                        }
                        pageContext.setAttribute("SearchHelperId", SearchHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSearchHelperId == true) {
            // end
            // begin [file="/search/fillcard.jsp";from=(1,0);to=(1,102)]
                }
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(1,102);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/fillcard.jsp";from=(4,0);to=(4,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("fillccard");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/fillcard.jsp";from=(4,0);to=(4,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(4,43);to=(11,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>一卡通充值记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/search/fillcard.jsp";from=(11,15);to=(11,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(11,41);to=(13,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/fillcard.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(13,42);to=(15,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/fillcard.jsp";from=(15,38);to=(15,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(15,64);to=(17,0)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/fillcard.jsp";from=(17,2);to=(20,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(20,2);to=(68,126)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\n\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"ccardcode\")) strErr+=\"*一卡通帐号不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar ccardcode=document.all(\"ccardcode\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/fillcard.jsp?ccardcode=\"+ccardcode;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">一卡通充值记录</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\r\n\t<br>\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td  nowrap><LABEL id=\"lblcAccName\">一卡通帐号</LABEL>\r\n                  <input name=\"ccardcode\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"30\" value=\"");

            // end
            // begin [file="/search/fillcard.jsp";from=(68,129);to=(68,178)]
                out.print(Util.setNullToEmpty((String)hs1.get("ccardcode")));
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(68,180);to=(93,16)]
                out.write("\">&nbsp;&nbsp;\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家帐号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">通行证</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值IP</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡面额</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡数量</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值渠道</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值方式</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值系统</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/fillcard.jsp";from=(93,18);to=(130,8)]
                
                				int viewList = 0;
                   String strPageAction="";
                   ArrayList lt=new ArrayList();
                   //ArrayList lt1 = new ArrayList();
                   //HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                	 String dDateTime="";//充值时间
                	String cChannel="";//充值渠道
                	String cAccName="";//玩家帐号
                	String cPassport="";//Passport
                	String cCardCode="";//一卡通卡号
                	String cClientIP="";//充值IP
                	String cCardType="";//卡类型
                	String iCardNumber="";//卡数量
                	String cServerIP="";
                	String cFillModel="";//充值方式
                	String cFillSys="";//充值系统
                    HashMap hs = new HashMap();
                
                   if (hs1.containsKey("ccardcode")) {
                       viewList = 1;
                       lt = SearchHelperId.getOneCardFillLog(hs1);
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                		 hs = (HashMap)lt.get(i);
                		 dDateTime = Util.setNullToEmpty((String)hs.get("ddatetime"));
                		 cChannel = Util.setNullToEmpty((String)hs.get("cchannel"));
                		 cAccName = Util.setNullToEmpty((String)hs.get("caccname"));
                		 cPassport = Util.setNullToEmpty((String)hs.get("cpassport"));
                		 cCardCode = Util.setNullToEmpty((String)hs.get("ccardcode"));
                		 cClientIP = Util.setNullToEmpty((String)hs.get("cclientip"));
                		 cCardType = Util.setNullToEmpty((String)hs.get("ccardtype"));
                		 iCardNumber = Util.setNullToEmpty((String)hs.get("icardnumber"));
                		 cServerIP = Util.setNullToEmpty((String)hs.get("cserverip"));
                		 cFillModel = Util.setNullToEmpty((String)hs.get("cplusmodel"));
                        
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(130,10);to=(131,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/fillcard.jsp";from=(131,26);to=(131,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(131,39);to=(131,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/fillcard.jsp";from=(131,59);to=(131,60)]
                }
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(131,62);to=(132,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(132,51);to=(132,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(132,56);to=(133,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(133,51);to=(133,60)]
                out.print(dDateTime);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(133,62);to=(134,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(134,51);to=(134,59)]
                out.print(cAccName);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(134,61);to=(135,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(135,51);to=(135,60)]
                out.print(cPassport);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(135,62);to=(136,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(136,51);to=(136,60)]
                out.print(cCardCode);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(136,62);to=(137,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(137,51);to=(137,60)]
                out.print(cClientIP);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(137,62);to=(138,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(138,51);to=(138,60)]
                out.print(cCardType);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(138,62);to=(139,48)]
                out.write("元</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(139,51);to=(139,62)]
                out.print(iCardNumber);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(139,64);to=(140,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(140,51);to=(140,59)]
                out.print(cChannel);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(140,61);to=(141,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(141,51);to=(141,61)]
                out.print(cFillModel);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(141,63);to=(142,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillcard.jsp";from=(142,51);to=(142,60)]
                out.print(cServerIP);
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(142,62);to=(144,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/fillcard.jsp";from=(144,22);to=(146,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(146,6);to=(158,6)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/fillcard.jsp";from=(158,8);to=(158,40)]
                if(lt.size()==0 && viewList==1){
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(158,42);to=(162,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/fillcard.jsp";from=(162,8);to=(162,9)]
                }
            // end
            // HTML // begin [file="/search/fillcard.jsp";from=(162,11);to=(171,0)]
                out.write("\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
