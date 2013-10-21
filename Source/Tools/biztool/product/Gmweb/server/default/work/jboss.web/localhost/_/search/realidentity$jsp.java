package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class realidentity$jsp extends HttpJspBase {

    // begin [file="/search/realidentity.jsp";from=(1,0);to=(1,102)]
    // end

    static {
    }
    public realidentity$jsp( ) {
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

            // HTML // begin [file="/search/realidentity.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/realidentity.jsp";from=(1,0);to=(1,102)]
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
            // begin [file="/search/realidentity.jsp";from=(1,0);to=(1,102)]
                }
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(1,102);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/realidentity.jsp";from=(4,0);to=(4,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("realidentity");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/realidentity.jsp";from=(4,0);to=(4,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(4,46);to=(11,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>防沉迷信息查询</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/search/realidentity.jsp";from=(11,15);to=(11,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(11,41);to=(13,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/realidentity.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(13,42);to=(15,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/realidentity.jsp";from=(15,38);to=(15,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(15,64);to=(17,0)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/realidentity.jsp";from=(17,2);to=(21,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
                   String usertype = Util.setNullToEmpty((String)hs1.get("usertype"));
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(21,2);to=(72,120)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\n\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"uid\")&&IsEmpty(\"identitycard\")) strErr+=\"*帐号和证件号码不能为全空\\n\";\r\n\tif((!IsEmpty(\"uid\"))&&(!IsEmpty(\"identitycard\"))) strErr+=\"*只可以根据帐号或证件号码之一查找，请去掉一项输入条件\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar uid=document.all(\"uid\").value;\r\n\t\tvar identitycard=document.all(\"identitycard\").value;\r\n\t\tvar usertype=document.all(\"usertype\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/realidentity.jsp?usertype=\"+usertype+\"&identitycard=\"+identitycard+\"&uid=\"+uid;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">防沉迷信息查询</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\r\n\t<br>\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td  nowrap><LABEL id=\"lbluid\">帐号</LABEL>\r\n                  <input name=\"uid\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"30\" value=\"");

            // end
            // begin [file="/search/realidentity.jsp";from=(72,123);to=(72,166)]
                out.print(Util.setNullToEmpty((String)hs1.get("uid")));
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(72,168);to=(74,129)]
                out.write("\">&nbsp;&nbsp;\r\n\t\t\t\t  证件号码\r\n                  <input name=\"identitycard\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"20\" maxlength=\"30\" value=\"");

            // end
            // begin [file="/search/realidentity.jsp";from=(74,132);to=(74,184)]
                out.print(Util.setNullToEmpty((String)hs1.get("identitycard")));
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(74,186);to=(77,41)]
                out.write("\">&nbsp;&nbsp;\r\n\t\t\t\t  帐号类型\r\n                  <SELECT name=\"usertype\">\r\n                    <OPTION value=\"pass\" ");

            // end
            // begin [file="/search/realidentity.jsp";from=(77,43);to=(77,94)]
                if (usertype.equals("pass")) out.print("selected");
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(77,96);to=(78,39)]
                out.write(">1.Passport帐号</OPTION>\r\n                    <OPTION value=\"fs\" ");

            // end
            // begin [file="/search/realidentity.jsp";from=(78,41);to=(78,90)]
                if (usertype.equals("fs")) out.print("selected");
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(78,92);to=(79,39)]
                out.write(">2.封神帐号</OPTION>\r\n                    <OPTION value=\"jx\" ");

            // end
            // begin [file="/search/realidentity.jsp";from=(79,41);to=(79,90)]
                if (usertype.equals("jx")) out.print("selected");
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(79,92);to=(102,16)]
                out.write(">3.剑网帐号</OPTION>\r\n                  </SELECT>&nbsp;&nbsp;\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">账号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">真实姓名</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">证件类型</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">证件号码</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">生日</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">是否纳入防沉迷</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">是否经过验证</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">补充时间</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/realidentity.jsp";from=(102,18);to=(152,8)]
                
                				int viewList = 0;
                   ArrayList lt=new ArrayList();
                   HttpSession se = request.getSession();
                	String Uid="";
                	String RealName="";
                	String CardType="";
                	String CardTypes="";
                	String IdentityCard="";
                	String Birthday="";
                	String TimeLimitFlag="";
                	String ValidateFlag="";
                	String TimeLimitFlags="";
                	String ValidateFlags="";
                	String operationDate="";
                    HashMap hs = new HashMap();
                
                   if (hs1.containsKey("uid")) {
                       viewList = 1;
                       lt = SearchHelperId.getRealIdentityInfo(hs1);
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                		 hs = (HashMap)lt.get(i);
                		 Uid = Util.setNullToEmpty((String)hs.get("uid"));
                		 RealName = Util.setNullToEmpty((String)hs.get("realname"));
                		 CardType = Util.setNullToEmpty((String)hs.get("cardtype"));
                		 IdentityCard = Util.setNullToEmpty((String)hs.get("identitycard"));
                		 Birthday = Util.setNullToEmpty((String)hs.get("birthday"));
                		 TimeLimitFlag = Util.setNullToEmpty((String)hs.get("timelimitflag"));
                		 ValidateFlag = Util.setNullToEmpty((String)hs.get("validateflag"));
                		 operationDate = Util.setNullToEmpty((String)hs.get("operationdate"));
                		 TimeLimitFlags="不纳入";
                		 ValidateFlags="未验证";
                		 CardTypes="身份证";
                		 if (TimeLimitFlag.equals("1")){
                			 TimeLimitFlags = "纳入防沉迷";
                		 }
                		 if (ValidateFlag.equals("1")){
                			 ValidateFlags = "验证并且通过";
                		 } else if (ValidateFlag.equals("2")){
                			 ValidateFlags = "验证但没有通过";
                		 }
                		 if (CardType.equals("1")){
                			 CardTypes = "军人证";
                		 } else if (CardType.equals("2")){
                			 CardTypes = "护照";
                		 }
                
                
                        
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(152,10);to=(154,48)]
                out.write("\r\n                    <tr>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(154,51);to=(154,54)]
                out.print(Uid);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(154,56);to=(155,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(155,51);to=(155,59)]
                out.print(RealName);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(155,61);to=(156,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(156,51);to=(156,60)]
                out.print(CardTypes);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(156,62);to=(157,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(157,51);to=(157,63)]
                out.print(IdentityCard);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(157,65);to=(158,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(158,51);to=(158,59)]
                out.print(Birthday);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(158,61);to=(159,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(159,51);to=(159,65)]
                out.print(TimeLimitFlags);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(159,67);to=(160,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(160,51);to=(160,64)]
                out.print(ValidateFlags);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(160,66);to=(161,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/realidentity.jsp";from=(161,51);to=(161,64)]
                out.print(operationDate);
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(161,66);to=(163,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/realidentity.jsp";from=(163,22);to=(165,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(165,6);to=(177,6)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/realidentity.jsp";from=(177,8);to=(177,40)]
                if(lt.size()==0 && viewList==1){
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(177,42);to=(181,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/realidentity.jsp";from=(181,8);to=(181,9)]
                }
            // end
            // HTML // begin [file="/search/realidentity.jsp";from=(181,11);to=(190,0)]
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
