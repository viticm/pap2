package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class realidentityfm$jsp extends HttpJspBase {

    // begin [file="/service/realidentityfm.jsp";from=(3,0);to=(3,102)]
    // end

    static {
    }
    public realidentityfm$jsp( ) {
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

            // HTML // begin [file="/service/realidentityfm.jsp";from=(0,175);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(3,0);to=(3,102)]
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
            // begin [file="/service/realidentityfm.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(4,0);to=(4,48)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("realidentityfm");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/service/realidentityfm.jsp";from=(4,0);to=(4,48)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(4,48);to=(12,35)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>修改防沉迷信息</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(12,38);to=(12,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(12,64);to=(14,13)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(14,42);to=(18,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(18,2);to=(46,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    ArrayList lt1 = new ArrayList();
                    HashMap hs1 = Util.parseUploadData(request, lt1);
                    UserInfo userinfo = (UserInfo) request.getSession().getAttribute("userinfo");
                	String uid = Util.setNullToEmpty((String)hs1.get("uid"));
                	String usertype = Util.setNullToEmpty((String)hs1.get("usertype"));
                    HashMap hs=new HashMap();
                	String RealName="";
                	String CardType="";
                	String IdentityCard="";
                	String TimeLimitFlag="";
                	String ValidateFlag="";
                	String operationDate="";
                    if(uid!=null && !uid.equals("")){
                		ArrayList lists = SearchHelperId.getRealIdentityInfo(hs1);
                		if(lists.size() > 0){
                			hs = (HashMap)lists.get(0);
                		    RealName = Util.setNullToEmpty((String)hs.get("realname"));
                		    CardType = Util.setNullToEmpty((String)hs.get("cardtype"));
                		    IdentityCard = Util.setNullToEmpty((String)hs.get("identitycard"));
                		    TimeLimitFlag = Util.setNullToEmpty((String)hs.get("timelimitflag"));
                		    ValidateFlag = Util.setNullToEmpty((String)hs.get("validateflag"));
                		    operationDate = Util.setNullToEmpty((String)hs.get("operationdate"));
                		}
                    }
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(46,2);to=(125,120)]
                out.write("\r\n<script language=\"JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"uid\")) strErr+=\"*帐号不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar uid=document.all(\"uid\").value;\r\n\t\tvar usertype=document.all(\"usertype\").value;\r\n\t\tvar strUrl=AbsPath+\"/service/realidentityfm.jsp?uid=\"+uid+\"&usertype=\"+usertype+\"&identitycard=\";\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\nfunction onModify()\r\n{\r\n\tdocument.all(\"identityCard\").disabled = false;\r\n\tdocument.all(\"realName\").disabled = false;\r\n\tdocument.all(\"btnModify\").disabled = true;\r\n\tdocument.all(\"btnSave\").disabled = false;\r\n    document.all(\"btnCancel\").disabled = false;\r\n}\r\n\r\nfunction onSave()\r\n{\r\n\tvar strErr=\"\";\r\n    if (document.all(\"cardType\").value!=\"2\" && !checkChineseName(document.all(\"realName\"),\"真实姓名\"))\r\n      return false;\r\n\tif(IsEmpty(\"realName\")) strErr+=\"*真实姓名不能为空\\n\";\r\n\tif(IsEmpty(\"identityCard\")) strErr+=\"*证件号码不能为空\\n\";\r\n\tif(IsEmpty(\"message\")) strErr+=\"*操作原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\nfunction onCancel()\r\n{\r\n\tdocument.all(\"identityCard\").disabled = true;\r\n\tdocument.all(\"realName\").disabled = true;\r\n\tdocument.all(\"btnModify\").disabled = false;\r\n\tdocument.all(\"btnSave\").disabled = true;\r\n    document.all(\"btnCancel\").disabled = true;\r\n}\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">防沉迷信息修改</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\t\t<br><br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\">输入帐号\r\n                  <input name=\"uid\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"32\" maxlength=\"50\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(125,123);to=(125,126)]
                out.print(uid);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(125,128);to=(128,41)]
                out.write("\">&nbsp;&nbsp;\r\n\t\t\t\t  帐号类型\r\n                  <SELECT name=\"usertype\">\r\n                    <OPTION value=\"pass\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(128,43);to=(128,94)]
                if (usertype.equals("pass")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(128,96);to=(129,39)]
                out.write(">1.Passport帐号</OPTION>\r\n                    <OPTION value=\"fs\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(129,41);to=(129,90)]
                if (usertype.equals("fs")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(129,92);to=(130,39)]
                out.write(">2.封神帐号</OPTION>\r\n                    <OPTION value=\"jx\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(130,41);to=(130,90)]
                if (usertype.equals("jx")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(130,92);to=(137,54)]
                out.write(">3.剑网帐号</OPTION>\r\n                  </SELECT>&nbsp;&nbsp;\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(137,57);to=(137,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(137,83);to=(144,31)]
                out.write("/GMServlet\">\r\n           <table width=\"667\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"31\" colspan=\"4\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;防沉迷信息</b></font></td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"25%\" height=\"25\" align=\"right\" nowrap>帐号：</td>\r\n               <td width=\"25%\">");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(144,34);to=(144,37)]
                out.print(uid);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(144,39);to=(146,84)]
                out.write(" </td>\r\n               <td width=\"25%\" align=\"right\">真实姓名： </td>\r\n               <td width=\"25%\"><input name=\"realName\" disabled  class=\"text\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(146,87);to=(146,95)]
                out.print(RealName);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(146,97);to=(151,38)]
                out.write("\" maxlength=\"12\"/><font color=\"#FF0000\">*</font></td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">证件类型(剑侠用户只能使用身份证)：</td>\r\n               <td><SELECT name=\"cardType\">\r\n                    <OPTION value=\"0\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(151,40);to=(151,88)]
                if (CardType.equals("0")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(151,90);to=(152,38)]
                out.write(">1.身份证</OPTION>\r\n                    <OPTION value=\"1\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(152,40);to=(152,88)]
                if (CardType.equals("1")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(152,90);to=(153,38)]
                out.write(">2.军人证</OPTION>\r\n                    <OPTION value=\"2\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(153,40);to=(153,88)]
                if (CardType.equals("2")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(153,90);to=(156,76)]
                out.write(">3.护照</OPTION>\r\n                  </SELECT><font color=\"#FF0000\">*</font></td>\r\n               <td align=\"right\">证件号码：</td>\r\n               <td><input name=\"identityCard\" disabled  class=\"text\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(156,79);to=(156,91)]
                out.print(IdentityCard);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(156,93);to=(161,38)]
                out.write("\" maxlength=\"20\"/><font color=\"#FF0000\">*</font></td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">纳入防沉迷：</td>\r\n               <td><SELECT name=\"timeLimitFlag\">\r\n                    <OPTION value=\"0\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(161,40);to=(161,93)]
                if (TimeLimitFlag.equals("0")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(161,95);to=(162,38)]
                out.write(">1.不纳入防沉迷</OPTION>\r\n                    <OPTION value=\"1\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(162,40);to=(162,93)]
                if (TimeLimitFlag.equals("1")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(162,95);to=(166,38)]
                out.write(">2.纳入防沉迷</OPTION>\r\n                  </SELECT><font color=\"#FF0000\">*</font></td>\r\n               <td align=\"right\">是否经过验证： </td>\r\n               <td><SELECT name=\"validateFlag\">\r\n                    <OPTION value=\"0\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(166,40);to=(166,92)]
                if (ValidateFlag.equals("0")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(166,94);to=(167,38)]
                out.write(">1.没有经过验证</OPTION>\r\n                    <OPTION value=\"1\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(167,40);to=(167,92)]
                if (ValidateFlag.equals("1")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(167,94);to=(168,38)]
                out.write(">2.经过验证并且验证通过</OPTION>\r\n                    <OPTION value=\"2\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(168,40);to=(168,92)]
                if (ValidateFlag.equals("2")) out.print("selected");
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(168,94);to=(173,85)]
                out.write(">3.经过验证并且验证没有通过</OPTION>\r\n                  </SELECT><font color=\"#FF0000\">*</font></td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">操作原因：</td>\r\n               <td colspan=\"3\"><input name=\"message\" class=\"text\" size=\"40\" value=\"\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(173,87);to=(173,129)]
                if(RealName==null || RealName.equals("")){
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(173,131);to=(173,141)]
                out.write(" disabled ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(173,143);to=(173,144)]
                }
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(173,146);to=(177,53)]
                out.write("/><font color=\"#FF0000\">*</font></td>\r\n             </tr>\r\n           </table>\r\n          <input type=\"hidden\" name=\"action\" value=\"modifyrealidentity\"/>\r\n          <input type=\"hidden\" name=\"account\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(177,56);to=(177,59)]
                out.print(uid);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(177,61);to=(178,54)]
                out.write("\"/>\r\n          <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(178,57);to=(178,79)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(178,81);to=(179,53)]
                out.write("\"/>\r\n          <input type=\"hidden\" name=\"game_id\" value=\"");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(179,56);to=(179,64)]
                out.print(usertype);
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(179,66);to=(183,65)]
                out.write("\"/>\r\n          </form>\r\n          \t\t<table width=\"420\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n            <tr align=\"center\">\r\n               <td><button name=\"btnModify\" onClick=\"onModify()\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(183,67);to=(183,109)]
                if(RealName==null || RealName.equals("")){
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(183,111);to=(183,132)]
                out.write(" disabled=\"disabled\" ");

            // end
            // begin [file="/service/realidentityfm.jsp";from=(183,134);to=(183,135)]
                }
            // end
            // HTML // begin [file="/service/realidentityfm.jsp";from=(183,137);to=(202,0)]
                out.write(">修改防沉迷信息</button></td>\r\n               <td><button name=\"btnSave\" onClick=\"onSave()\" disabled>保存</button></td>\r\n               <td><button name=\"btnCancel\" onClick=\"onCancel()\" disabled>取消</button></td>\r\n               <td><button name=\"Return\" onClick=\"javascript:history.back()\">返回</button></td>\r\n             </tr>\r\n           </table>\r\n\r\n          <br><br>\r\n\t\t  </td>\r\n        </tr>\r\n      </table>\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body><!-- InstanceEnd -->\r\n</html>\r\n");

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
