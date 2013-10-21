package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class setgw$jsp extends HttpJspBase {

    // begin [file="/individual/setgw.jsp";from=(1,0);to=(1,94)]
    // end

    static {
    }
    public setgw$jsp( ) {
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

            // HTML // begin [file="/individual/setgw.jsp";from=(0,142);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/individual/setgw.jsp";from=(1,0);to=(1,94)]
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
            // begin [file="/individual/setgw.jsp";from=(1,0);to=(1,94)]
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(1,94);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/individual/setgw.jsp";from=(4,0);to=(4,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("havelogin");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/individual/setgw.jsp";from=(4,0);to=(4,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(4,43);to=(13,13)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>设置工作范围</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(13,42);to=(17,0)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/individual/setgw.jsp";from=(17,2);to=(23,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
                    String usergateway=GMHelperId.getUserGateway(userinfo.getUsercode());
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(23,2);to=(139,17)]
                out.write("\r\n<script language=\"javascript\">\r\n/**************************\r\n\t设置子框\r\n**************************/\r\nfunction SetChildCheck(chkName,tabName)\r\n{\r\n\tvar objChk=document.all(chkName);\r\n\tvar objTable=document.all(tabName);\r\n\tvar aryCheck=objTable.all.tags(\"INPUT\");\r\n\tvar intCheckLength=aryCheck.length;\r\n\tvar flag=false,i;\r\n\tif(objChk.checked) flag=true;\r\n\tfor(i=0;i<intCheckLength;i++) aryCheck[i].checked=flag;\r\n}\r\n\r\n/**************************\r\n\t设置父框\r\n**************************/\r\nfunction SetParentCheck(parentChkName,tabName)\r\n{\r\n\tvar objParentChk=document.all(parentChkName);\r\n\tvar objTable=document.all(tabName);\r\n\tvar aryCheck=objTable.all.tags(\"INPUT\");\r\n\tvar intCheckLength=aryCheck.length;\r\n\tvar i,intChecked=0;\r\n\tfor(i=0;i<intCheckLength;i++)\r\n\t{\r\n\t\tif(aryCheck[i].checked) intChecked++;\r\n\t}\r\n\tobjParentChk.checked=(intCheckLength==intChecked);\r\n}\r\n\r\n\r\n/***********************************\r\n取得工作范围\r\n***********************************/\r\nfunction getUserGateway(strTableName)\r\n{\r\n\tvar objTable=document.all(strTableName);\r\n\tvar strUserGateway=\"\";\r\n\tvar i;\r\n\tvar aryChk=objTable.all.tags(\"INPUT\");\r\n\tfor(i=0;i<aryChk.length;i++)\r\n\t{\r\n\t\tif(aryChk[i].checked)\r\n\t\t{\r\n\t\t\tstrUserGateway=strUserGateway+aryChk[i].value+\",\";\r\n\t\t}\r\n\t}\r\n\tif(strUserGateway!=\"\")\r\n\t{\r\n\t\tstrUserGateway=strUserGateway.substring(0,strUserGateway.length-1);\r\n\t}\r\n\treturn strUserGateway;\r\n}\r\n\r\n/**************************\r\n\t设置工作范围\r\n**************************/\r\nfunction setUserGateway(inData)\r\n{\r\n\tvar aryGwCodeInfo=inData.split(\",\");\r\n\tvar i,objCheck,strObjName;\r\n\tfor(i=0;i<aryGwCodeInfo.length;i++)\r\n\t{\r\n\t\tstrObjName=\"chk\"+aryGwCodeInfo[i];\r\n\t\tobjCheck = document.all(strObjName);\r\n\t\tif(objCheck !=null && typeof(objCheck)== \"object\")\r\n\t\t{\r\n\t\t\tobjCheck.checked = true;\r\n\t\t}\r\n\t}\r\n}\r\n\r\n/**************************\r\n\t按确定\r\n**************************/\r\nfunction onSubmit()\r\n{\r\n\tvar jx2Gateway=getUserGateway(\"tabJX2\");\r\n    var jx2ibGateway=getUserGateway(\"tabJX2IB\")\r\n    var jxsjGateway=getUserGateway(\"tabJXSJ\")\r\n\tvar jx3Gateway=getUserGateway(\"tabJX3\");\r\n\tvar allGateway=\"\";\r\n\tif(jx2Gateway!=\"\")\r\n\t{\r\n\t\tallGateway=allGateway+\",\"+jx2Gateway;\r\n\t}\r\n\tif(jx2ibGateway!=\"\")\r\n\t{\r\n\t\tallGateway=allGateway+\",\"+jx2ibGateway;\r\n\t}\r\n\tif(jxsjGateway!=\"\")\r\n\t{\r\n\t\tallGateway=allGateway+\",\"+jxsjGateway;\r\n\t}\r\n\tif(jx3Gateway!=\"\")\r\n\t{\r\n\t\tallGateway=allGateway+\",\"+jx3Gateway;\r\n\t}\r\n\tif (allGateway!=\"\")\r\n\t{\r\n\t\tallGateway=allGateway.substring(1);\r\n\t}\r\n\r\n\tdocument.all(\"usergateway\").value=allGateway;\r\n\tdocument.all(\"frmSubmit\").submit();\r\n}\r\n\r\n/**************************\r\n\t按还原\r\n**************************/\r\nfunction onReturn()\r\n{\r\n\tClearAllChk();\r\n\tsetUserGateway(\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(139,20);to=(139,31)]
                out.print(usergateway);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(139,33);to=(154,44)]
                out.write("\");\r\n\tSetParentCheck(\"chkJX2\",\"tabJX2\");\r\n\tSetParentCheck(\"chkJX2IB\",\"tabJX2IB\");\r\n\tSetParentCheck(\"chkJXSJ\",\"tabJXSJ\");\r\n}\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">系统功能</span> &gt;&gt; <span class=\"title\">设置工作范围</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" --><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(154,47);to=(154,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(154,73);to=(159,56)]
                out.write("/SystemServlet\">\r\n      <table width=\"98%\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\"  nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;设置工作范围\r\n            <input type=\"hidden\" name=\"action\" value=\"setusergateway\">\r\n            <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(159,59);to=(159,81)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(159,83);to=(169,8)]
                out.write("\">\r\n            <input type=\"hidden\" name=\"usergateway\" >\r\n          </b></font></td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"148\"  nowrap>\r\n\t\t  <input type=\"checkbox\" name=\"chkJX2\" value=\"checkbox\" onClick=\"SetChildCheck('chkJX2','tabJX2')\">\r\n                《剑网2》所有区服：\r\n        <table id=\"tabJX2\" width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n        <tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(169,10);to=(181,8)]
                
                        ArrayList lt=GMHelperId.getGatewayList(6);
                        String gateway_name = null;
                        String game_id = null;
                        String area = null;
                        String group = null;
                        for (int i=0,k=0;i<lt.size(); i++) {
                          gateway_name = (String) ( (HashMap) lt.get(i)).get("gateway_name");
                          game_id = (String) ( (HashMap) lt.get(i)).get("game_id");
                          area = (String) ( (HashMap) lt.get(i)).get("area");
                          group = (String) ( (HashMap) lt.get(i)).get("group");
                        if(k%5==0){
                        
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(181,10);to=(183,8)]
                out.write("\r\n        </tr><tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(183,10);to=(183,11)]
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(183,13);to=(184,44)]
                out.write("\r\n        <td><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,47);to=(184,54)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,56);to=(184,57)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,60);to=(184,64)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,66);to=(184,67)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,70);to=(184,75)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,77);to=(184,86)]
                out.write("\" value=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,89);to=(184,96)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,98);to=(184,99)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,102);to=(184,106)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,108);to=(184,109)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(184,112);to=(184,117)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(184,119);to=(185,8)]
                out.write("\" onClick=\"SetParentCheck('chkJX2','tabJX2')\">\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(185,11);to=(185,23)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(185,25);to=(186,9)]
                out.write("</td>\r\n         ");

            // end
            // begin [file="/individual/setgw.jsp";from=(186,11);to=(188,10)]
                
                         k++;
                         }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(188,12);to=(197,8)]
                out.write("\r\n         </tr>\r\n          </table>\r\n           <tr>\r\n          <td height=\"148\"  nowrap>\r\n\t\t  <input type=\"checkbox\" name=\"chkJX2IB\" value=\"checkbox\" onClick=\"SetChildCheck('chkJX2IB','tabJX2IB')\">\r\n                《剑网2IB》所有区服：\r\n        <table id=\"tabJX2IB\" width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n        <tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(197,10);to=(205,8)]
                
                        lt=GMHelperId.getGatewayList(66);
                        for (int i=0,k=0;i<lt.size(); i++) {
                          gateway_name = (String) ( (HashMap) lt.get(i)).get("gateway_name");
                          game_id = (String) ( (HashMap) lt.get(i)).get("game_id");
                          area = (String) ( (HashMap) lt.get(i)).get("area");
                          group = (String) ( (HashMap) lt.get(i)).get("group");
                        if(k%7==0){
                        
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(205,10);to=(207,8)]
                out.write("\r\n        </tr><tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(207,10);to=(207,11)]
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(207,13);to=(208,44)]
                out.write("\r\n        <td><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,47);to=(208,54)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,56);to=(208,57)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,60);to=(208,64)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,66);to=(208,67)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,70);to=(208,75)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,77);to=(208,86)]
                out.write("\" value=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,89);to=(208,96)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,98);to=(208,99)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,102);to=(208,106)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,108);to=(208,109)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(208,112);to=(208,117)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(208,119);to=(209,8)]
                out.write("\" onClick=\"SetParentCheck('chkJX2IB','tabJX2IB')\">\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(209,11);to=(209,23)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(209,25);to=(210,9)]
                out.write("</td>\r\n         ");

            // end
            // begin [file="/individual/setgw.jsp";from=(210,11);to=(212,10)]
                
                         k++;
                         }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(212,12);to=(221,8)]
                out.write("\r\n         </tr>\r\n          </table>\r\n           <tr>\r\n          <td height=\"148\"  nowrap>\r\n\t\t  <input type=\"checkbox\" name=\"chkJXSJ\" value=\"checkbox\" onClick=\"SetChildCheck('chkJXSJ','tabJXSJ')\">\r\n                《剑侠世界》所有区服：\r\n        <table id=\"tabJXSJ\" width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n        <tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(221,10);to=(229,8)]
                
                        lt=GMHelperId.getGatewayList(77);
                        for (int i=0,k=0;i<lt.size(); i++) {
                          gateway_name = (String) ( (HashMap) lt.get(i)).get("gateway_name");
                          game_id = (String) ( (HashMap) lt.get(i)).get("game_id");
                          area = (String) ( (HashMap) lt.get(i)).get("area");
                          group = (String) ( (HashMap) lt.get(i)).get("group");
                        if(k%7==0){
                        
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(229,10);to=(231,8)]
                out.write("\r\n        </tr><tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(231,10);to=(231,11)]
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(231,13);to=(232,44)]
                out.write("\r\n        <td><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,47);to=(232,54)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,56);to=(232,57)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,60);to=(232,64)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,66);to=(232,67)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,70);to=(232,75)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,77);to=(232,86)]
                out.write("\" value=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,89);to=(232,96)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,98);to=(232,99)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,102);to=(232,106)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,108);to=(232,109)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(232,112);to=(232,117)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(232,119);to=(233,8)]
                out.write("\" onClick=\"SetParentCheck('chkJXSJ','tabJXSJ')\">\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(233,11);to=(233,23)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(233,25);to=(234,9)]
                out.write("</td>\r\n         ");

            // end
            // begin [file="/individual/setgw.jsp";from=(234,11);to=(236,10)]
                
                         k++;
                         }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(236,12);to=(248,8)]
                out.write("\r\n         </tr>\r\n          </table>\r\n          </td>\r\n          </tr>\r\n\r\n        <tr>\r\n          <td height=\"148\"  nowrap>\r\n\t\t  <input type=\"checkbox\" name=\"chkJX3\" value=\"checkbox\" onClick=\"SetChildCheck('chkJX3','tabJX3')\">\r\n                《剑网3》所有区服：\r\n        <table id=\"tabJX3\" width=\"100%\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n        <tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(248,10);to=(256,8)]
                
                        lt=GMHelperId.getGatewayList(88);
                        for (int i=0,k=0;i<lt.size(); i++) {
                          gateway_name = (String) ( (HashMap) lt.get(i)).get("gateway_name");
                          game_id = (String) ( (HashMap) lt.get(i)).get("game_id");
                          area = (String) ( (HashMap) lt.get(i)).get("area");
                          group = (String) ( (HashMap) lt.get(i)).get("group");
                        if(k%5==0){
                        
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(256,10);to=(258,8)]
                out.write("\r\n        </tr><tr>\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(258,10);to=(258,11)]
                }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(258,13);to=(259,44)]
                out.write("\r\n        <td><input type=\"checkbox\" name=\"chk");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,47);to=(259,54)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,56);to=(259,57)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,60);to=(259,64)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,66);to=(259,67)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,70);to=(259,75)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,77);to=(259,86)]
                out.write("\" value=\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,89);to=(259,96)]
                out.print(game_id);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,98);to=(259,99)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,102);to=(259,106)]
                out.print(area);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,108);to=(259,109)]
                out.write("#");

            // end
            // begin [file="/individual/setgw.jsp";from=(259,112);to=(259,117)]
                out.print(group);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(259,119);to=(260,8)]
                out.write("\" onClick=\"SetParentCheck('chkJX3','tabJX3')\">\r\n        ");

            // end
            // begin [file="/individual/setgw.jsp";from=(260,11);to=(260,23)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(260,25);to=(261,9)]
                out.write("</td>\r\n         ");

            // end
            // begin [file="/individual/setgw.jsp";from=(261,11);to=(263,10)]
                
                         k++;
                         }
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(263,12);to=(276,17)]
                out.write("\r\n         </tr>\r\n          </table>\r\n         </tr>\r\n      </table>\r\n\t</form>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onSubmit()\">确定</button></td>\r\n    <td><button onClick=\"javascript:history.back()\">取消</button></td>\r\n  </tr>\r\n</table>\r\n<script language=\"Javascript\">\r\n\tsetUserGateway(\"");

            // end
            // begin [file="/individual/setgw.jsp";from=(276,20);to=(276,31)]
                out.print(usergateway);
            // end
            // HTML // begin [file="/individual/setgw.jsp";from=(276,33);to=(290,0)]
                out.write("\");\r\n\tSetParentCheck(\"chkJX2\",\"tabJX2\");\r\n    SetParentCheck(\"chkJX2IB\",\"tabJX2IB\");\r\n    SetParentCheck(\"chkJXSJ\",\"tabJXSJ\");\r\n\tSetParentCheck(\"chkJX3\",\"tabJX3\");\r\n</script>\r\n<!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
