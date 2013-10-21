package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class affichenew$jsp extends HttpJspBase {

    // begin [file="/gmc/affichenew.jsp";from=(1,0);to=(1,93)]
    // end

    static {
    }
    public affichenew$jsp( ) {
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

            // HTML // begin [file="/gmc/affichenew.jsp";from=(0,142);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(1,0);to=(1,93)]
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
            // begin [file="/gmc/affichenew.jsp";from=(1,0);to=(1,93)]
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(1,93);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(4,54);to=(5,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(5,64);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(6,0);to=(6,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("3affichenew");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/affichenew.jsp";from=(6,0);to=(6,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(6,45);to=(15,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>发布公告</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<link href=\"../css/calendar.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n<script language=\"JavaScript\" src=\"../js/calendar.js\"></script>\r\n");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(15,2);to=(20,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(20,2);to=(22,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(22,16);to=(22,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(22,42);to=(317,44)]
                out.write("\";\r\nvar objSelectTr;//所选择的行\r\nvar action;\r\n/*************************\r\n增加\r\n**************************/\r\nfunction onAdd()\r\n{\r\n\taction=\"addautoaffiche\";\r\n\tSetEnable();\r\n\tdocument.all(\"term\").value=\"\";\r\n\tdocument.all(\"endtime\").value=\"\";\r\n\tdocument.all(\"message\").value=\"\";\r\n\tdocument.all(\"message\").focus();\r\n}\r\n\r\nfunction onModify()\r\n{\r\n\taction=\"modifyautoaffiche\";\r\n\tSetEnable();\r\n\tdocument.all(\"message\").focus();\r\n}\r\n\r\n\r\n/*************************\r\n删除\r\n**************************/\r\nfunction onDel()\r\n{\r\n\tif(confirm(\"你确定要删除此循环公告吗？\"))\r\n\t{\r\n\t\tdocument.all(\"action\").value=\"delautoaffiche\";\r\n\t\tsetButtonState(true);\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/*************************\r\n发布\r\n*************************/\r\nfunction onPublish()\r\n{\r\n\tvar strErr=\"\";\r\n\tvar tmpGateway = \"\";\r\n\t//var objSel=document.all(\"selGateway\");\r\n\tif(!IsEmpty(\"term\") && document.all(\"term\").value==\"0\") strErr+=\"*循环周期不能为0\\n\";\r\n\tif(!IsEmpty(\"term\") && IsEmpty(\"endtime\")) strErr+=\"*循环公告必须设置截止时间\\n\";\r\n\tif(action==\"modifyautoaffiche\" && IsEmpty(\"term\")) strErr+=\"*修改循环公告时循环周期不能为空\\n\";\r\n\tif(IsEmpty(\"message\"))\r\n\t{\r\n\t strErr+=\"*公告内容不能为空\\n\";\r\n\t }\r\n\t else if(document.all(\"message\").value.length>106)\tstrErr+=\"公告内容已超过106个字符(53个汉字)，请缩短公告内容后再试！\\n\";\r\n\r\n\t\r\n\t\r\n\tvar objDiv = document.all(\"gatewayDiv\");\r\n\tvar aryCheck = objDiv.all.tags(\"INPUT\");\r\n\tvar intCheckLength = aryCheck.length;\r\n\tfor (i = 0; i < intCheckLength; i++) {\t\r\n\t\tif (aryCheck[i].checked) {\r\n\t\t\ttmpGateway= aryCheck[i].name.split(\"#\");\r\n\t\t}\r\n\t}\r\n\tif(tmpGateway==\"\") strErr+=\"*发布区服不能为空\\n\";\r\n\t\t\t\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tdocument.all(\"action\").value=action;\r\n\t\tsetButtonState(true);\r\n\t\tif(action==\"addautoaffiche\")\r\n\t\t{\r\n\t\t\tdocument.all(\"gateway\").value=getGatewayInfo();\r\n\t\t}\r\n\t\telse if(action==\"modifyautoaffiche\")\r\n\t\t{\t\t\t\t\t\r\n\t\t\tdocument.all(\"area\").value=tmpGateway[1];\r\n\t\t\tdocument.all(\"group\").value=tmpGateway[2];\r\n\t\t\tdocument.all(\"gateway\").value=tmpGateway[3].substring(0,tmpGateway[3].length-1);\r\n\t\t}\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n/***************************\r\n取消\r\n*****************************/\r\nfunction onCancel()\r\n{\r\n\tSetDisable();\r\n\taction=\"\";\r\n\tif(objSelectTr)\r\n\t{\r\n\t\tdocument.all(\"iid\").value=objSelectTr.cells[0].iid;\r\n\t\tdocument.all(\"term\").value=objSelectTr.cells[0].term;\r\n\t\tdocument.all(\"endtime\").value=objSelectTr.cells[0].endtime;\r\n\t\tdocument.all(\"message\").value=objSelectTr.cells[0].message;\r\n\t\tvar typecode=objSelectTr.cells[0].typecode;\r\n\t\tdocument.all(\"typecode\")[0].checked=false;\r\n\t\tdocument.all(\"typecode\")[1].checked=false;\r\n\t\tif(typecode.indexOf(\"normal\")!=-1){\r\n\t\t\tdocument.all(\"typecode\")[0].selected=true;\r\n\t\t}\r\n\t\telse if(typecode.indexOf(\"important\")!=-1)\r\n\t\t{\r\n\t\t\tdocument.all(\"typecode\")[1].selected=true;\r\n\t\t}\r\n\t\telse if(typecode.indexOf(\"news\")!=-1)\r\n\t\t{\r\n\t\t\tdocument.all(\"typecode\")[2].selected=true;\r\n\t\t}\r\n\t\tgame_id=objSelectTr.cells[0].game_id;\r\n\t\tarea=objSelectTr.cells[0].area;\r\n\t\tgroup=objSelectTr.cells[0].group;\r\n\t\tSetGateway(game_id,area,group);\r\n\t\tdocument.all(\"btnModify\").disabled=false;\r\n\t}\r\n}\r\n\r\n/****************************************\r\n移动Mouse时改变Color\r\n*****************************************/\r\nfunction ChangeColor(objTr)\r\n{\r\n\tif(objSelectTr)\r\n\t{\r\n\t\tobjSelectTr.bgColor=\"\";\r\n\t}\r\n\tobjTr.bgColor=\"#00CCFF\";\r\n\tobjSelectTr=objTr;\r\n}\r\n\r\n/***************************\r\n选择公告\r\n***************************/\r\nfunction onSelAffiche()\r\n{\r\n\tvar objTr=event.srcElement;\r\n\twhile(objTr.tagName.toUpperCase()!=\"TR\") objTr=objTr.parentElement;\r\n\ttry{\r\n\t\tChangeColor(objTr);\r\n\t\tdocument.all(\"frmSubmit\").reset();\r\n\t\tdocument.all(\"iid\").value=objTr.cells[0].iid;\r\n\t\tdocument.all(\"term\").value=objTr.cells[0].term;\r\n\t\tdocument.all(\"endtime\").value=objTr.cells[0].endtime;\r\n\t\tdocument.all(\"message\").value=objTr.cells[0].message;\r\n\t\tdocument.all(\"typecode\").value=objTr.cells[0].typecode;\r\n\t\tgame_id=objTr.cells[0].game_id;\r\n\t\tarea=objTr.cells[0].area;\r\n\t\tgroup=objTr.cells[0].group;\r\n\t\tdocument.all(\"game_id\").value=game_id;\r\n\t\tonChangeGameId();\r\n\t\tSetAreaGateway(game_id,area,group);\r\n\t}\r\n\tcatch(e)\r\n\t{\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=false;\r\n\tdocument.all(\"btnModify\").disabled=false;\r\n\tdocument.all(\"btnDel\").disabled=false;\r\n\tdocument.all(\"btnPublish\").disabled=true;\r\n\tdocument.all(\"btnCancel\").disabled=true;\r\n}\r\n\r\n/****************************\r\n设置发布区服\r\n*************************/\r\nfunction SetAreaGateway(game_id,area,group)\r\n{\t\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==game_id&& objOption.area==area && objOption.group==group)\r\n\t\t{\r\n\t\t\tSetSelValue(\"zonecode\",objOption.zonecode);\r\n\t\t\tonChangeZoneCode();\r\n\t\t\t\r\n\t\t\tvar objDiv = document.all(\"gatewayDiv\");\r\n\t\t\tvar aryCheck = objDiv.all.tags(\"INPUT\");\r\n\t\t\tvar intCheckLength = aryCheck.length;\r\n\t\t\tvar gateway = \"\";\r\n\t\t\tfor (i = 0; i < intCheckLength; i++) {\t\r\n\t\t\t\tif(aryCheck[i].name==game_id+\"#\"+area+\"#\"+group+\"#\"+objOption.value+\",\"){\r\n\t\t\t\t\taryCheck[i].checked=true;\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t\tbreak;\t\t\r\n\t\t}\r\n\t}\r\n}\r\n\r\n/**************************\r\n\t设置为可填写内容状态\r\n**************************/\r\nfunction SetEnable()\r\n{\r\n\tdocument.all(\"divAfficheList\").disabled=true;\r\n\tdocument.all(\"tbdAfficheList\").disabled=true;\r\n\tdocument.all(\"zonecode\").disabled=false;\r\n\tdocument.all(\"typecode\").disabled=false;\r\n\tdocument.all(\"term\").disabled=false;\r\n\tdocument.all(\"endtime\").disabled=false;\r\n\tdocument.all(\"message\").disabled=false;\r\n\tdocument.all(\"btnAdd\").disabled=true;\r\n\tdocument.all(\"btnModify\").disabled=true;\r\n\tdocument.all(\"btnDel\").disabled=true;\r\n\tdocument.all(\"btnPublish\").disabled=false;\r\n\tdocument.all(\"btnCancel\").disabled=false;\r\n}\r\n\r\n/**************************\r\n\t设置为不可填写内容状态\r\n**************************/\r\nfunction SetDisable()\r\n{\r\n\tdocument.all(\"divAfficheList\").disabled=false;\r\n\tdocument.all(\"tbdAfficheList\").disabled=false;\r\n\tdocument.all(\"zonecode\").disabled=true;\r\n\tdocument.all(\"typecode\").disabled=true;\r\n\tdocument.all(\"term\").disabled=true;\r\n\tdocument.all(\"endtime\").disabled=true;\r\n\tdocument.all(\"message\").disabled=true;\r\n\tdocument.all(\"btnAdd\").disabled=false;\r\n\tdocument.all(\"btnModify\").disabled=true;\r\n\tdocument.all(\"btnDel\").disabled=true;\r\n\tdocument.all(\"btnPublish\").disabled=true;\r\n\tdocument.all(\"btnCancel\").disabled=true;\r\n}\r\n\r\n/***************************\r\n选择区服\r\n**************************/\r\nfunction getGatewayInfo()\r\n{\r\n\tvar objDiv = document.all(\"gatewayDiv\");\r\n\tvar aryCheck = objDiv.all.tags(\"INPUT\");\r\n\tvar intCheckLength = aryCheck.length;\r\n\tvar gateway = \"\";\r\n\tfor (i = 0; i < intCheckLength; i++) {\t\r\n\t\tif (aryCheck[i].checked) {\r\n\t\t\tgateway += aryCheck[i].name ;\r\n\t\t}\r\n\t}\r\n\tif(gateway!=\"\")\r\n\t{\r\n\t\tgateway=gateway.substring(0,gateway.length-1);\r\n\t}\r\n\treturn gateway;\r\n}\r\n\r\n\r\nfunction onChangeGameId()\r\n{\r\n\t\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t\r\n\t\t \tj+=1;\r\n\t\t \tif(j==1){\r\n\t\t\tobjSel.options[objSel.length]=new Option(0+\".所有区\",'all');\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t \t}\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n\tonChangeZoneCode();\r\n\t\t\r\n\t\r\n}\r\n</script>\r\n\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">发布公告（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><br>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(317,47);to=(317,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(317,73);to=(337,18)]
                out.write("/GMCServlet\">\r\n\t  <table width=\"95%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#000000\"  class=\"tablelist\">\r\n        <tr>\r\n          <td align=\"center\" background=\"../images/tab_bg.gif\"><font color=\"#FFFFFF\">系统中正在发布的循环公告</font></td>\r\n        </tr>\r\n        <tr>\r\n          <td valign=\"top\"><div id=\"divAfficheList\" style=\"overflow:auto;height:320;width:100%;background:#ffffff\">\r\n              <table width=\"100%\"  border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" class=\"tablelist\">\r\n                <tr align=\"center\">\r\n                  <td align=\"center\" width=\"10\">№</td>\r\n                  <td align=\"center\" >公告类型</td>\r\n                  <td>发布人</td>\r\n                  <td>公告内容</td>\r\n                  <td nowrap>发布区服</td>\r\n                  <td>开始时间</td>\r\n                  <td>截止时间</td>\r\n                  <td width=\"28\" nowrap>周期</td>\r\n                  <td width=\"40\" nowrap>已发次</td>\r\n                </tr>\r\n                <tbody id=\"tbdAfficheList\" onclick=\"onSelAffiche()\" style=\"cursor:hand;\">\r\n                  ");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(337,20);to=(369,18)]
                
                                  ArrayList ltAffiche = GMHelperId.getAutoGMCAffiche("all");
                                  String iid="";
                                  String typecode="";
                                  String typename="";
                                  String usercode="";
                				  String username="";
                                  String area="";
                                  String game_id="";
                                  String group="";
                                  String message="";
                                  String term="";
                                  String starttime="";
                                  String times="";
                                  String gateway="";
                                  String endtime="";
                                  for(int i=0;i<ltAffiche.size();i++){
                                    HashMap hs=(HashMap)ltAffiche.get(i);
                                    iid=(String)hs.get("iid");
                                    typecode=(String)hs.get("typecode");
                                    typename=(String)hs.get("typename");
                                    usercode=(String)hs.get("usercode");
                					username=(String)hs.get("username");
                                    message=(String)hs.get("message");
                                    gateway=(String)hs.get("gateway");
                                    starttime=(String)hs.get("starttime");
                                    endtime=(String)hs.get("endtime");
                                    term=(String)hs.get("term");
                                    times=(String)hs.get("times");
                					game_id=(String)hs.get("game_id");
                					area=(String)hs.get("area");
                					group=(String)hs.get("group");
                                  
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(369,20);to=(371,44)]
                out.write("\r\n                  <tr>\r\n                    <td align=\"center\" iid=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,47);to=(371,50)]
                out.print(iid);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,52);to=(371,63)]
                out.write("\" endtime=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,66);to=(371,111)]
                out.print(endtime.substring(0,endtime.lastIndexOf(":")));
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,113);to=(371,125)]
                out.write("\" typecode=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,128);to=(371,136)]
                out.print(typecode);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,138);to=(371,146)]
                out.write("\" term=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,149);to=(371,153)]
                out.print(term);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,155);to=(371,166)]
                out.write("\" message=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,169);to=(371,176)]
                out.print(message);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,178);to=(371,189)]
                out.write("\" game_id=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,192);to=(371,199)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,201);to=(371,209)]
                out.write("\" area=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,212);to=(371,216)]
                out.print(area);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,218);to=(371,227)]
                out.write("\" group=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,230);to=(371,235)]
                out.print(group);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,237);to=(371,239)]
                out.write("\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(371,242);to=(371,245)]
                out.print(i+1);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(371,247);to=(372,24)]
                out.write("</td>\r\n                    <td>");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(372,27);to=(372,35)]
                out.print(typename);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(372,37);to=(373,39)]
                out.write("</td>\r\n                    <td align=\"center\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(373,42);to=(373,50)]
                out.print(usercode);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(373,52);to=(373,53)]
                out.write("(");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(373,56);to=(373,64)]
                out.print(username);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(373,66);to=(374,24)]
                out.write(")</td>\r\n                    <td>");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(374,27);to=(374,34)]
                out.print(message);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(374,36);to=(375,24)]
                out.write("</td>\r\n                    <td>");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(375,27);to=(375,34)]
                out.print(gateway);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(375,36);to=(376,39)]
                out.write("</td>\r\n                    <td align=\"center\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(376,42);to=(376,51)]
                out.print(starttime);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(376,53);to=(377,39)]
                out.write("</td>\r\n                    <td align=\"center\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(377,42);to=(377,49)]
                out.print(endtime);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(377,51);to=(378,38)]
                out.write("</td>\r\n                    <td align=\"right\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(378,41);to=(378,45)]
                out.print(term);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(378,47);to=(379,39)]
                out.write("</td>\r\n                    <td align=\"right\" >");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(379,42);to=(379,47)]
                out.print(times);
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(379,49);to=(381,18)]
                out.write("</td>\r\n                  </tr>\r\n                  ");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(381,20);to=(381,21)]
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(381,23);to=(394,34)]
                out.write("\r\n                </tbody>\r\n              </table>\r\n          </div></td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\"  background=\"../images/tab_bg.gif\"><font color=\"#FFFFFF\">公告详细信息</font></td>\r\n        </tr>\r\n        <tr>\r\n          <td valign=\"top\">\r\n              <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n                <tr>\r\n                  <td align=\"right\" nowrap>公告类型：</td>\r\n                  <td colspan=\"5\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(394,34);to=(394,115)]
                /* ----  affiche:type ---- */
                com.kingsoft.gmsystem.web.taglib.AfficheTypeTag _jspx_th_affiche_type_0 = new com.kingsoft.gmsystem.web.taglib.AfficheTypeTag();
                _jspx_th_affiche_type_0.setPageContext(pageContext);
                _jspx_th_affiche_type_0.setParent(null);
                _jspx_th_affiche_type_0.setDisplayCode("select_type");
                _jspx_th_affiche_type_0.setSelectName("typecode");
                _jspx_th_affiche_type_0.setStyle("width:160");
                try {
                    int _jspx_eval_affiche_type_0 = _jspx_th_affiche_type_0.doStartTag();
                    if (_jspx_eval_affiche_type_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.AfficheTypeTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_affiche_type_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/affichenew.jsp";from=(394,34);to=(394,115)]
                        } while (_jspx_th_affiche_type_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_affiche_type_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_affiche_type_0.release();
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(394,115);to=(406,34)]
                out.write(" &nbsp;&nbsp;&nbsp;&nbsp;循环周期：\r\n<input name=\"term\" type=\"text\" size=\"5\" maxlength=\"5\" onKeyPress=\"OnlyInputNumber()\" disabled>\r\n分钟<font color=\"#666666\">(为空则不循环)</font>&nbsp;&nbsp;&nbsp;&nbsp;截止时间：\r\n<input name=\"endtime\" id=\"endtime\" type=\"text\" style=\"width:120\" disabled>\r\n<IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" name=\"bgntime\" onClick=\"setday(this,document.all('endtime'))\" style=\"cursor:hand \"> <font color=\"#666666\">(包括此日)</font>\r\n\r\n</td>\r\n                </tr>\r\n                <tr>\r\n                  <td width=\"71\" rowspan=\"2\" align=\"right\">公告内容： </td>\r\n                  <td width=\"120\" rowspan=\"2\" ><textarea name=\"message\" cols=\"50\" rows=\"7\" disabled></textarea></td>\r\n                  <td width=\"60\" align=\"left\">所属游戏:</td>\r\n                  <td width=\"160\">");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(406,34);to=(406,145)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_id");
                _jspx_th_game_gameselect_0.setOnChange("onChangeGameId()");
                _jspx_th_game_gameselect_0.setStyle("width:160");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/affichenew.jsp";from=(406,34);to=(406,145)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(406,145);to=(410,64)]
                out.write("</td>\r\n                  <td width=\"60\" align=\"left\">发布区服：\r\n                    <input type=\"hidden\" name=\"script\">\r\n                    <input type=\"hidden\" name=\"action\">\r\n                    <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(410,67);to=(410,89)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(410,91);to=(418,8)]
                out.write("\">\r\n                    <input type=\"hidden\" name=\"iid\">\r\n                    <input type=\"hidden\" name=\"area\">\r\n                    <input type=\"hidden\" name=\"group\">\r\n                    <input type=\"hidden\" name=\"gateway\">\r\n                  </td>\r\n                  <td width=\"150\" align=\"left\">\r\n                  <select name=\"zonecode\" style=\"width:150\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(418,8);to=(419,58)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_zone_all");
                _jspx_th_display_display_0.setProperty("zonecodeTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/affichenew.jsp";from=(419,58);to=(420,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/affichenew.jsp";from=(420,8);to=(420,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(420,26);to=(426,6)]
                out.write("\r\n\t\t\t\t</td>\r\n                </tr>\r\n                <tr>\r\n\t\t\t\t<td colspan=\"4\">\r\n\t\t\t\t  <div id=\"gatewayDiv\"></div>\r\n\t\t\t\t  ");

            // end
            // begin [file="/gmc/affichenew.jsp";from=(426,6);to=(426,110)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_gateway_person");
                _jspx_th_display_display_1.setProperty("selGatewayTemp");
                _jspx_th_display_display_1.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/affichenew.jsp";from=(426,110);to=(427,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/affichenew.jsp";from=(427,12);to=(427,30)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/affichenew.jsp";from=(427,30);to=(499,0)]
                out.write("</td>\r\n                </tr>\r\n              </table>\r\n          </td>\r\n        </tr>\r\n      </table>\r\n\t  </form><table width=\"450\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td> <button onClick=\"onAdd()\" name=\"btnAdd\">增加</button></td>\r\n    <td> <button onClick=\"onModify()\" name=\"btnModify\" disabled>修改</button></td>\r\n    <td> <button onClick=\"onDel()\"name=\"btnDel\" disabled>删除</button></td>\r\n    <td> <button onClick=\"onPublish()\" name=\"btnPublish\" disabled>发布</button></td>\r\n    <td><button onClick=\"onCancel()\" name=\"btnCancel\" disabled>取消</button></td>\r\n  </tr>\r\n</table><br>\r\n<script language=\"javascript\">\t\r\ndocument.getElementById(\"zonecode\").onchange = function onChangeZoneCode(){\r\n\r\n\r\n\tvar objSel = document.getElementById(\"zonecode\");\r\n\tvar objOpt = objSel.options[objSel.selectedIndex];\t\r\n\tvar tmp = \"\";\r\n\t\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tvar j=0;\r\n\tvar game_id = document.all(\"game_id\").value;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==game_id&&objOption.zonecode==document.all(\"zonecode\").value)\r\n\t\t{\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+objOption.game_id+\"#\"+objOption.area+\"#\"\r\n\t\t\t+objOption.group+\"#\"+objOption.value+\",\"+\"'>\"\r\n\t\t\t+objOption.value;\t\t\t\r\n\t\t}\r\n\t}\r\n\t\r\n\tif(tmp == \"\"){\r\n\t\tif(game_id==\"6\")\r\n\t\t{\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+game_id+\"#0#0#剑网2所有区服,\"+\"'>\"\r\n\t\t\t+\"剑网2所有区服\";\t\t\t\r\n\t\t}\r\n\t    else if(game_id==\"66\")\r\n\t\t{\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+game_id+\"#0#0#剑网2IB所有区服,\"+\"'>\"\r\n\t\t\t+\"剑网2IB所有区服\";\t\t\t\r\n\t\t}\r\n\t    else if(game_id==\"77\")\r\n\t\t{\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+game_id+\"#0#0#剑侠世界所有区服,\"+\"'>\"\r\n\t\t\t+\"剑侠世界所有区服\";\t\t\t\r\n\t\t}\r\n\t    else if(game_id==\"88\")\r\n\t\t{\r\n\t\t\ttmp +=\" <input type='checkbox' name='\"+game_id+\"#0#0#剑网3所有区服,\"+\"'>\"\r\n\t\t\t+\"剑网3所有区服\";\t\t\t\r\n\t\t}\r\n\t}\r\n\tdocument.getElementById(\"gatewayDiv\").innerHTML = tmp;\r\n}\r\n\tonChangeGameId();\r\n</script>\r\n</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
