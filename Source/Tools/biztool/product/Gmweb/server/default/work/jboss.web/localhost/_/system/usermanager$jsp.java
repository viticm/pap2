package org.apache.jsp;

import java.util.ArrayList;
import com.kingsoft.gmsystem.domain.GroupInfo;
import com.kingsoft.gmsystem.domain.UserInfo;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class usermanager$jsp extends HttpJspBase {

    // begin [file="/system/usermanager.jsp";from=(3,0);to=(4,54)]
    // end

    static {
    }
    public usermanager$jsp( ) {
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

            // HTML // begin [file="/system/usermanager.jsp";from=(2,108);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/usermanager.jsp";from=(3,0);to=(4,54)]
                com.kingsoft.gmsystem.service.SystemHelper systemhelper = null;
                boolean _jspx_specialsystemhelper  = false;
                 synchronized (session) {
                    systemhelper= (com.kingsoft.gmsystem.service.SystemHelper)
                    pageContext.getAttribute("systemhelper",PageContext.SESSION_SCOPE);
                    if ( systemhelper == null ) {
                        _jspx_specialsystemhelper = true;
                        try {
                            systemhelper = (com.kingsoft.gmsystem.service.SystemHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SystemHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SystemHelper", exc);
                        }
                        pageContext.setAttribute("systemhelper", systemhelper, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialsystemhelper == true) {
            // end
            // begin [file="/system/usermanager.jsp";from=(3,0);to=(4,54)]
                }
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(4,54);to=(5,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(5,55);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/system/usermanager.jsp";from=(6,0);to=(6,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("usermanager");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/system/usermanager.jsp";from=(6,0);to=(6,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(6,45);to=(16,13)]
                out.write("\r\n<html>\r\n\t<!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n\t<head>\r\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n\t\t<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n\t\t<title>人员权限管理</title>\r\n\t\t<!-- InstanceEndEditable -->\r\n\t\t<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n\t\t<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(16,16);to=(16,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(16,42);to=(20,2)]
                out.write("\";\r\n</script>\r\n\t\t<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n\t\t<!-- InstanceBeginEditable name=\"head\" -->\r\n\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(20,4);to=(28,2)]
                
                			response.addHeader("Pragma", "no-cache");
                			response.addHeader("Cache-Control", "no-cache");
                			response.addDateHeader("Expires", -1);
                			ArrayList ltGroup = systemhelper.getGroupList();//所有组
                			ArrayList ltUser = systemhelper.getUserList();//所有用户
                			GroupInfo groupinfo = null;//组对象
                			UserInfo userinfo = null;//用户对象
                		
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(28,4);to=(40,8)]
                out.write("\r\n\t\t<style>\r\noption.group {\r\n\tbackground-color: #BBDDFD\r\n}\r\n</style>\r\n\t\t<script language=\"JavaScript\">\r\n\tvar action=\"\";\r\n\t\r\n\t\r\n</script>\r\n\t\t<script language=\"JavaScript\"\r\n\t\t\tsrc=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(40,11);to=(40,35)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(40,37);to=(82,10)]
                out.write("/js/usermanager.js\"></script>\r\n\t\t<!-- InstanceEndEditable -->\r\n\t</head>\r\n\t<body>\r\n\t\t<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\"\r\n\t\t\tcellspacing=\"0\">\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"20\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\t&nbsp;&nbsp;当前位置：\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"position\" -->\r\n\t\t\t\t\t<span class=\"title\">系统功能</span> &gt;&gt;\r\n\t\t\t\t\t<span class=\"title\">人员权限管理</span>\r\n\t\t\t\t\t<!-- InstanceEndEditable -->\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td align=\"center\" valign=\"top\">\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"main\" -->\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<form name=\"frmUserManager\">\r\n\t\t\t\t\t\t<input name=\"delChks\" type=\"hidden\">\r\n\t\t\t\t\t\t<input name=\"addChks\" type=\"hidden\">\r\n\t\t\t\t\t\t<table width=\"98%\" border=\"1\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\tbordercolor=\"#000000\" class=\"tablelist\">\r\n\t\t\t\t\t\t\t<thead>\r\n\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t<td width=\"15%\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">用户列表</font>\r\n\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t<td width=\"85%\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">用户信息</font>\r\n\t\t\t\t\t\t\t\t\t\t<input name=\"iid\" type=\"hidden\">\r\n\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t</thead>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td rowspan=\"4\" valign=\"top\">\r\n\t\t\t\t\t\t\t\t\t<select name=\"selUserList\" size=\"27\" style=\"width: 150\"\r\n\t\t\t\t\t\t\t\t\t\tonChange=\"onSelectUser()\">\r\n\t\t\t\t\t\t\t\t\t\t<option type=\"allgroup\" class=\"group\">\r\n\t\t\t\t\t\t\t\t\t\t\t所有组\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(82,12);to=(85,10)]
                
                											for (int i = 0; i < ltGroup.size(); i++) {//组环
                												groupinfo = (GroupInfo) ltGroup.get(i);
                										
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(85,12);to=(86,38)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t<option type=\"group\" value=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(86,41);to=(86,65)]
                out.print(groupinfo.getGroupname());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(86,67);to=(87,34)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tclass=\"group\" groupid=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(87,37);to=(87,59)]
                out.print(groupinfo.getGroupid());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(87,61);to=(88,18)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tpower=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(88,21);to=(88,46)]
                out.print(groupinfo.getGroupPower());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(88,48);to=(88,50)]
                out.write("\">");

            // end
            // begin [file="/system/usermanager.jsp";from=(88,53);to=(88,77)]
                out.print(groupinfo.getGroupname());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(88,79);to=(89,10)]
                out.write("</option>\r\n\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(89,12);to=(93,10)]
                
                											for (int j = 0; j < ltUser.size(); j++) {//用户环
                													userinfo = (UserInfo) ltUser.get(j);
                													if (userinfo.getGroupid().equals(groupinfo.getGroupid())) {//是此组用户
                										
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(93,12);to=(94,37)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t<option type=\"user\" value=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(94,40);to=(94,62)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(94,64);to=(95,16)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tiid=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(95,19);to=(95,36)]
                out.print(userinfo.getIid());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(95,38);to=(96,21)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tusercode=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(96,24);to=(96,46)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(96,48);to=(97,21)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tusername=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(97,24);to=(97,46)]
                out.print(userinfo.getUsername());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(97,48);to=(98,19)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tmobile=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(98,22);to=(98,42)]
                out.print(userinfo.getMobile());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(98,44);to=(99,20)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tgroupid=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(99,23);to=(99,44)]
                out.print(userinfo.getGroupid());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(99,46);to=(100,15)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\tip=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(100,18);to=(100,34)]
                out.print(userinfo.getIp());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(100,36);to=(100,45)]
                out.write("\" power=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(100,48);to=(100,73)]
                out.print(groupinfo.getGroupPower());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(100,75);to=(101,18)]
                out.write("\">\r\n\t\t\t\t\t\t\t\t\t\t\t&nbsp;┣");

            // end
            // begin [file="/system/usermanager.jsp";from=(101,21);to=(101,43)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(101,45);to=(102,10)]
                out.write("</option>\r\n\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(102,12);to=(106,10)]
                 	
                											}
                												}
                											}
                										
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(106,12);to=(129,10)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t</select>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<font color=\"#FF0000\">*</font>用户登录ID\r\n\t\t\t\t\t\t\t\t\t<input name=\"usercode\" type=\"text\" size=\"15\" maxlength=\"20\"\r\n\t\t\t\t\t\t\t\t\t\tdisabled class=\"text\">\r\n\t\t\t\t\t\t\t\t\t姓名\r\n\t\t\t\t\t\t\t\t\t<input name=\"username\" type=\"text\" disabled size=\"10\"\r\n\t\t\t\t\t\t\t\t\t\tmaxlength=\"50\" class=\"text\">\r\n\t\t\t\t\t\t\t\t\t手机\r\n\t\t\t\t\t\t\t\t\t<input name=\"mobile\" type=\"text\" disabled size=\"12\"\r\n\t\t\t\t\t\t\t\t\t\tmaxlength=\"12\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t登录IP\r\n\t\t\t\t\t\t\t\t\t<input name=\"ip\" type=\"text\" disabled size=\"16\" maxlength=\"100\"\r\n\t\t\t\t\t\t\t\t\t\tclass=\"text\">\r\n\t\t\t\t\t\t\t\t\t(多个“,”隔开)&nbsp;&nbsp;&nbsp;\r\n\t\t\t\t\t\t\t\t\t<font color=\"#FF0000\">*</font>用户分组\r\n\t\t\t\t\t\t\t\t\t<select name=\"groupid\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(129,12);to=(132,10)]
                
                											for (int i = 0; i < ltGroup.size(); i++) {
                												groupinfo = (GroupInfo) ltGroup.get(i);
                										
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(132,12);to=(133,25)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"");

            // end
            // begin [file="/system/usermanager.jsp";from=(133,28);to=(133,50)]
                out.print(groupinfo.getGroupid());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(133,52);to=(133,54)]
                out.write("\">");

            // end
            // begin [file="/system/usermanager.jsp";from=(133,57);to=(133,81)]
                out.print(groupinfo.getGroupname());
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(133,83);to=(134,10)]
                out.write("</option>\r\n\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/system/usermanager.jsp";from=(134,12);to=(136,10)]
                
                											}
                										
            // end
            // HTML // begin [file="/system/usermanager.jsp";from=(136,12);to=(523,0)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t</select>\r\n\t\t\t\t\t\t\t\t\t新组名\r\n\t\t\t\t\t\t\t\t\t<input name=\"groupname\" type=\"text\" disabled size=\"23\"\r\n\t\t\t\t\t\t\t\t\t\tmaxlength=\"20\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td align=\"center\" background=\"../images/tab_bg.gif\">\r\n\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">权限信息</font>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td align=\"center\" valign=\"top\" id=\"tdPower\">\r\n\t\t\t\t\t\t\t\t\t<fieldset style=\"width: 98%\">\r\n\t\t\t\t\t\t\t\t\t\t<legend>\r\n\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"chkGMCService\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetChildCheck('chkGMCService')\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t<b><font color=\"#3577FF\">GMC服务功能</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</b>\r\n\t\t\t\t\t\t\t\t\t\t</legend>\r\n\t\t\t\t\t\t\t\t\t\t<table width=\"100%\" border=\"0\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\t\t\t\t\tid=\"tabGMCService\">\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"4accountnctrl\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t冻结解冻帐号\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"6kickplayer\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t强制下线(踢人)\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"5rolebehave\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t角色行为控制\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"7chat\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t发送密聊信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"3affichenew\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t发布游戏公告\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"8flygm\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tGM 瞬移\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"exchangepassword\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t临时置换帐号密码\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"batchfreeze\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t批量帐号冻结\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"batchfreezerole\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t上传文件批量角色冻结\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"batchfreezeaccount\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t上传文件批量帐号冻结\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"gmcfunction\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkGMCService',this.name)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tdisabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tGMC功能频道\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\r\n\t\t\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t\t\t</fieldset>\r\n\t\t\t\t\t\t\t\t\t<fieldset style=\"width: 98%\">\r\n\t\t\t\t\t\t\t\t\t\t<legend>\r\n\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"chkSearch\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetChildCheck('chkSearch')\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t<b><font color=\"#3577FF\">查询功能</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</b>\r\n\t\t\t\t\t\t\t\t\t\t</legend>\r\n\t\t\t\t\t\t\t\t\t\t<table width=\"100%\" border=\"0\" cellspacing=\"0\" id=\"tabSearch\">\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"1accountinfo\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t帐号信息查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"passportinfo\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t查询通行证基本信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"fillagame\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t游戏帐号充值\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"fillbpass\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t通行证充值\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"fillccard\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t一卡通充值\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"gameitemlog\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t物品流向查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"gameactionlog\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t行为历程查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"itempurchase\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tIB物品查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"realidentity\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t防沉迷信息查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"realidentityfm\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t修改防沉迷信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"gmfunctionlog\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t操作记录\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"answerlog\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t问题答复记录\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"cardinfo\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t查看通行证证件号码\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"viewbind\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t查看通行证绑定信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"modifybind\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t修改通行证绑定信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"accountcard\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t查询玩家证件号码\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"accountclew\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t查询帐号提示问题和答案\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"freezepassport\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t冻结通行证交易\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"unfreezepassport\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t解冻通行证交易\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"dubainfo\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t毒霸绑定信息\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"roleresume\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t角色恢复\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"unpassprotect\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t取消密码保护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"question\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t未答复问题查询\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"accountroleinfo\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSearch',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t帐号角色互查\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap></td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap></td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t\t\t</fieldset>\r\n\t\t\t\t\t\t\t\t\t<fieldset style=\"width: 98%\">\r\n\t\t\t\t\t\t\t\t\t\t<legend>\r\n\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"chkSystem\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetChildCheck('chkSystem')\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t<b><font color=\"#3577FF\">系统维护</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</b>\r\n\t\t\t\t\t\t\t\t\t\t</legend>\r\n\t\t\t\t\t\t\t\t\t\t<table width=\"100%\" border=\"0\" cellspacing=\"0\" id=\"tabSystem\">\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"usermanager\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t人员权限管理\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"allowip\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t可登录IP维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"gateway\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t游戏区服维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"position\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t瞬移位置维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td width=\"20%\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"commonquestion\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t常见问题维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"reason\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t操作原因维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t&nbsp;\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"idc\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t游戏IDC维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<input type=\"checkbox\" name=\"managegmcfunction\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tonClick=\"SetParentCheck('chkSystem',this.name)\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tGMC功能维护\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t&nbsp;\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<td nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t&nbsp;\r\n\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t\t\t</fieldset>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t</table>\r\n\t\t\t\t\t</form>\r\n\t\t\t\t\t<table width=\"637\" height=\"16\" border=\"0\" cellspacing=\"0\">\r\n\t\t\t\t\t\t<tr align=\"center\">\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnAdd\" onClick=\"onAdd()\">\r\n\t\t\t\t\t\t\t\t\t增加用户\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnModify\" onClick=\"onModify()\" disabled>\r\n\t\t\t\t\t\t\t\t\t修改\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnDelete\" onClick=\"onDelete()\" disabled>\r\n\t\t\t\t\t\t\t\t\t删除\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnSave\" onClick=\"onSave()\" disabled>\r\n\t\t\t\t\t\t\t\t\t保存\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnCancel\" onClick=\"onCancel()\" disabled>\r\n\t\t\t\t\t\t\t\t\t取消\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnAddGroup\" onClick=\"onAddGroup()\">\r\n\t\t\t\t\t\t\t\t\t增加组\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnModifyGroup\" onClick=\"onModifyGroup()\" disabled>\r\n\t\t\t\t\t\t\t\t\t修改组\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnDelGroup\" onClick=\"onDelGroup()\" disabled>\r\n\t\t\t\t\t\t\t\t\t删除组\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnAddPower\" onClick=\"onAddPower()\" disabled>\r\n\t\t\t\t\t\t\t\t\t增加所有组权限\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t<td width=\"65\">\r\n\t\t\t\t\t\t\t\t<button name=\"btnDelPower\" onClick=\"onDelPower()\" disabled>\r\n\t\t\t\t\t\t\t\t\t删除所有组权限\r\n\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t</table>\r\n\t\t\t\t\t<iframe name=\"frmSubmit\" width=\"0\" height=\"0\" frameborder=0\r\n\t\t\t\t\t\tscrolling=\"no\" src=\"submit.jsp?info=null\" marginwidth=\"0\"\r\n\t\t\t\t\t\tmarginheight=\"0\">\r\n\t\t\t\t\t\t对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。\r\n\t\t\t\t\t</iframe>\r\n\r\n\t\t\t\t\t<!-- InstanceEndEditable -->\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\tCopyright &copy; 2005 Kingsoft Corp,All Rights Reserved\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t</table>\r\n\t</body>\r\n\t<!-- InstanceEnd -->\r\n</html>\r\n");

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
