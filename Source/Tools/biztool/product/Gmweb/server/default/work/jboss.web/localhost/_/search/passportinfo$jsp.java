package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class passportinfo$jsp extends HttpJspBase {

    // begin [file="/search/passportinfo.jsp";from=(3,0);to=(3,102)]
    // end

    static {
    }
    public passportinfo$jsp( ) {
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

            // HTML // begin [file="/search/passportinfo.jsp";from=(0,175);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/passportinfo.jsp";from=(3,0);to=(3,102)]
                com.kingsoft.gmsystem.service.SystemHelper SystemHelperId = null;
                boolean _jspx_specialSystemHelperId  = false;
                 synchronized (session) {
                    SystemHelperId= (com.kingsoft.gmsystem.service.SystemHelper)
                    pageContext.getAttribute("SystemHelperId",PageContext.SESSION_SCOPE);
                    if ( SystemHelperId == null ) {
                        _jspx_specialSystemHelperId = true;
                        try {
                            SystemHelperId = (com.kingsoft.gmsystem.service.SystemHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SystemHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SystemHelper", exc);
                        }
                        pageContext.setAttribute("SystemHelperId", SystemHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSystemHelperId == true) {
            // end
            // begin [file="/search/passportinfo.jsp";from=(3,0);to=(3,102)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(3,102);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/passportinfo.jsp";from=(4,0);to=(4,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("passportinfo");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/passportinfo.jsp";from=(4,0);to=(4,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(4,46);to=(12,35)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>通行证信息</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(12,38);to=(12,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(12,64);to=(14,13)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(14,42);to=(18,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/passportinfo.jsp";from=(18,2);to=(28,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo = (UserInfo) request.getSession().getAttribute("userinfo");
                    String cPassport=new String(Util.setNullToEmpty(request.getParameter("cpassport")).getBytes("ISO-8859-1"));//要查询的通行证
                    PassportInfo pinfo=new PassportInfo();
                    if(cPassport!=null && !cPassport.equals("")){
                    	pinfo=SystemHelperId.getPassportInfo(cPassport,userinfo);
                    }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(28,2);to=(103,126)]
                out.write("\r\n<script language=\"JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"cpassport\")) strErr+=\"*通行证不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar cpassport=document.all(\"cpassport\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/passportinfo.jsp?cpassport=\"+cpassport;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\nfunction onModify()\r\n{\r\n\tdocument.all(\"mail\").disabled = false;\r\n        document.all(\"mobile\").disabled = false;\r\n\tdocument.all(\"btnModify\").disabled = true;\r\n\tdocument.all(\"btnSave\").disabled = false;\r\n        document.all(\"btnCancel\").disabled = false;\r\n}\r\n\r\nfunction onSave()\r\n{\r\n\tdocument.all(\"frmSubmit\").submit();\r\n}\r\n\r\nfunction onCancel(mail, mobile)\r\n{\r\n  \tdocument.all(\"mail\").value = mail;\r\n        document.all(\"mobile\").value = mobile;\r\n\tdocument.all(\"btnModify\").disabled = false;\r\n\tdocument.all(\"btnSave\").disabled = true;\r\n        document.all(\"btnCancel\").disabled = true;\r\n}\r\nfunction onFreeze()\r\n{\r\n\tdocument.all(\"action\").value=\"freezepassport\";\r\n\tdocument.all(\"frmSubmit\").submit();\r\n}\r\nfunction onUnFreeze()\r\n{\r\n\tdocument.all(\"action\").value=\"unfreezepassport\";\r\n\tdocument.all(\"frmSubmit\").submit();\r\n}\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">通行证信息</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\t\t<br><br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\">输入通行证\r\n                  <input name=\"cpassport\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"32\" maxlength=\"50\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(103,129);to=(103,148)]
                out.print(pinfo.getPassport());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(103,150);to=(109,54)]
                out.write("\">\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(109,57);to=(109,81)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(109,83);to=(116,37)]
                out.write("/GMServlet\">\r\n           <table width=\"667\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"31\" colspan=\"4\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;通行证信息</b></font></td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"25%\" height=\"25\" align=\"right\" nowrap>金山通行证：</td>\r\n               <td width=\"25%\">&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(116,40);to=(116,59)]
                out.print(pinfo.getPassport());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(116,61);to=(118,37)]
                out.write(" </td>\r\n               <td width=\"25%\" align=\"right\"> 证件类型： </td>\r\n               <td width=\"25%\">&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(118,40);to=(118,58)]
                out.print(pinfo.getId_type());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(118,60);to=(122,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\" nowrap>真实姓名：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(122,28);to=(122,47)]
                out.print(pinfo.getRealname());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(122,49);to=(125,25)]
                out.write("\r\n               </td>\r\n               <td align=\"right\">证件号码：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(125,28);to=(125,45)]
                out.print(pinfo.getId_num());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(125,47);to=(129,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">已冻结交易：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(129,28);to=(129,47)]
                out.print(pinfo.getUserType());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(129,49);to=(131,25)]
                out.write("</td>\r\n               <td align=\"right\">积分：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(131,28);to=(131,44)]
                out.print(pinfo.getPoint());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(131,46);to=(135,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">余额：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(135,28);to=(135,44)]
                out.print(pinfo.getMoney());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(135,46);to=(137,25)]
                out.write(" </td>\r\n               <td align=\"right\">QQ号码： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(137,28);to=(137,41)]
                out.print(pinfo.getQq());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(137,43);to=(141,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">昵称： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(141,28);to=(141,47)]
                out.print(pinfo.getNickname());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(141,49);to=(143,25)]
                out.write("</td>\r\n               <td align=\"right\"> 生日： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(143,28);to=(143,44)]
                out.print(pinfo.getBirth());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(143,46);to=(147,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">性别： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(147,28);to=(147,45)]
                out.print(pinfo.getGender());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(147,47);to=(149,25)]
                out.write("</td>\r\n               <td align=\"right\">用户单位： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(149,28);to=(149,43)]
                out.print(pinfo.getUnit());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(149,45);to=(153,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 所在国家： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(153,28);to=(153,46)]
                out.print(pinfo.getCountry());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(153,48);to=(155,25)]
                out.write("</td>\r\n               <td align=\"right\"> 邮政编码： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(155,28);to=(155,42)]
                out.print(pinfo.getZip());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(155,44);to=(159,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 省份自治区： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(159,28);to=(159,47)]
                out.print(pinfo.getProvince());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(159,49);to=(161,25)]
                out.write("</td>\r\n               <td align=\"right\"> 联系电话： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(161,28);to=(161,48)]
                out.print(pinfo.getTelephone());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(161,50);to=(165,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 所在城市： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(165,28);to=(165,43)]
                out.print(pinfo.getArea());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(165,45);to=(167,25)]
                out.write("</td>\r\n               <td align=\"right\">传真：</td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(167,28);to=(167,42)]
                out.print(pinfo.getFax());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(167,44);to=(171,37)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">联系地址： </td>\r\n               <td colspan=\"3\">&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(171,40);to=(171,58)]
                out.print(pinfo.getAddress());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(171,60);to=(175,25)]
                out.write("</td>\r\n               </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 星座： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(175,28);to=(175,52)]
                out.print(pinfo.getConstellation());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(175,54);to=(177,25)]
                out.write("</td>\r\n               <td align=\"right\">职业： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(177,28);to=(177,49)]
                out.print(pinfo.getProfession());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(177,51);to=(181,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 收入状况： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(181,28);to=(181,45)]
                out.print(pinfo.getIncome());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(181,47);to=(183,25)]
                out.write("</td>\r\n               <td align=\"right\"> 学历： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(183,28);to=(183,45)]
                out.print(pinfo.getDegree());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(183,47);to=(187,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 毒霸结束时间： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(187,28);to=(187,48)]
                out.print(pinfo.getDbenddate());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(187,50);to=(189,25)]
                out.write("</td>\r\n               <td align=\"right\"> 网镖结束时间： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(189,28);to=(189,48)]
                out.print(pinfo.getWbenddate());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(189,50);to=(193,25)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\"> 木马结束时间： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(193,28);to=(193,48)]
                out.print(pinfo.getMmenddate());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(193,50);to=(195,25)]
                out.write("</td>\r\n               <td align=\"right\"> 漏洞结束时间： </td>\r\n               <td>&nbsp;");

            // end
            // begin [file="/search/passportinfo.jsp";from=(195,28);to=(195,48)]
                out.print(pinfo.getLdenddate());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(195,50);to=(199,68)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">绑定邮箱：</td>\r\n               <td><input name=\"mail\" disabled  class=\"text\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(199,71);to=(199,90)]
                out.print(pinfo.getBindMail());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(199,92);to=(202,99)]
                out.write("\"/></td>\r\n               <td align=\"right\">绑定手机：</td>\r\n               <td>\r\n                 <input name=\"mobile\" disabled  class=\"text\" onKeyPress=\"OnlyInputNumber()\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(202,102);to=(202,123)]
                out.print(pinfo.getBindMobile());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(202,125);to=(207,85)]
                out.write("\" maxlength=\"12\"/>\r\n               </td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">操作原因：</td>\r\n               <td colspan=\"3\"><input name=\"message\" class=\"text\" size=\"40\" value=\"\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(207,87);to=(207,131)]
                if(cPassport==null || cPassport.equals("")){
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(207,133);to=(207,143)]
                out.write(" disabled ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(207,145);to=(207,146)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(207,148);to=(211,54)]
                out.write("/></td>\r\n             </tr>\r\n           </table>\r\n          <input type=\"hidden\" name=\"action\" value=\"modifybind\"/>\r\n          <input type=\"hidden\" name=\"passport\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(211,57);to=(211,76)]
                out.print(pinfo.getPassport());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(211,78);to=(212,54)]
                out.write("\"/>\r\n          <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(212,57);to=(212,79)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(212,81);to=(213,53)]
                out.write("\"/>\r\n          <input type=\"hidden\" name=\"oldmail\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(213,56);to=(213,75)]
                out.print(pinfo.getBindMail());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(213,77);to=(214,55)]
                out.write("\"/>\r\n          <input type=\"hidden\" name=\"oldmobile\" value=\"");

            // end
            // begin [file="/search/passportinfo.jsp";from=(214,58);to=(214,79)]
                out.print(pinfo.getBindMobile());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(214,81);to=(218,14)]
                out.write("\"/>\r\n          </form>\r\n          \t\t<table width=\"420\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n            <tr align=\"center\">\r\n              ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(218,16);to=(218,101)]
                 if(SystemHelperId.isCanModifyBind(userinfo.getUserpower(),userinfo.getUsercode())) {
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(218,103);to=(219,65)]
                out.write("\r\n               <td><button name=\"btnModify\" onClick=\"onModify()\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(219,67);to=(219,111)]
                if(cPassport==null || cPassport.equals("")){
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(219,113);to=(219,134)]
                out.write(" disabled=\"disabled\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(219,136);to=(219,137)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(219,139);to=(221,63)]
                out.write(">修改绑定</button></td>\r\n               <td><button name=\"btnSave\" onClick=\"onSave()\" disabled>保存</button></td>\r\n               <td><button name=\"btnCancel\" onClick=\"onCancel('");

            // end
            // begin [file="/search/passportinfo.jsp";from=(221,66);to=(221,85)]
                out.print(pinfo.getBindMail());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(221,87);to=(221,90)]
                out.write("','");

            // end
            // begin [file="/search/passportinfo.jsp";from=(221,93);to=(221,114)]
                out.print(pinfo.getBindMobile());
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(221,116);to=(222,16)]
                out.write("')\" disabled>取消</button></td>\r\n                ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(222,18);to=(222,83)]
                 }  if(SystemHelperId.isHaveThePower(request,"freezepassport")) {
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(222,85);to=(223,65)]
                out.write("\r\n               <td><button name=\"btnFreeze\" onClick=\"onFreeze()\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(223,67);to=(223,111)]
                if(cPassport==null || cPassport.equals("")){
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(223,113);to=(223,134)]
                out.write(" disabled=\"disabled\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(223,136);to=(223,137)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(223,139);to=(224,15)]
                out.write(">冻结</button></td>\r\n               ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(224,17);to=(224,82)]
                } if(SystemHelperId.isHaveThePower(request,"unfreezepassport")) {
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(224,84);to=(225,69)]
                out.write("\r\n               <td><button name=\"brnUnFreeze\" onClick=\"onUnFreeze()\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(225,71);to=(225,115)]
                if(cPassport==null || cPassport.equals("")){
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(225,117);to=(225,138)]
                out.write(" disabled=\"disabled\" ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(225,140);to=(225,141)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(225,143);to=(226,15)]
                out.write(">解冻</button></td>\r\n               ");

            // end
            // begin [file="/search/passportinfo.jsp";from=(226,17);to=(226,18)]
                }
            // end
            // HTML // begin [file="/search/passportinfo.jsp";from=(226,20);to=(243,0)]
                out.write("\r\n               <td><button name=\"Return\" onClick=\"javascript:history.back()\">返回</button></td>\r\n             </tr>\r\n           </table>\r\n\r\n          <br><br>\r\n\t\t  </td>\r\n        </tr>\r\n      </table>\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body><!-- InstanceEnd -->\r\n</html>\r\n");

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
