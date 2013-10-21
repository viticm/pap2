package org.apache.jsp;

import com.kingsoft.gmsystem.domain.UserInfo;
import com.kingsoft.gmsystem.service.GMCHelper;
import java.util.Iterator;
import java.util.HashMap;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class gmcfunction$jsp extends HttpJspBase {


    static {
    }
    public gmcfunction$jsp( ) {
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

            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(2,126);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(4,53);to=(5,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(5,55);to=(6,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(6,61);to=(7,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(7,64);to=(8,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(8,0);to=(8,45)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("gmcfunction");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(8,0);to=(8,45)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(8,45);to=(13,14)]
                out.write("\r\n<html>\r\n\t<head>\r\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n\t\t<title>GMC功能频道</title>\r\n\t\t<link href=\"");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(13,17);to=(13,41)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(13,43);to=(16,8)]
                out.write("/css/public.css\"\r\n\t\t\trel=\"stylesheet\" type=\"text/css\">\r\n\t\t<script language=\"JavaScript\"\r\n\t\t\tsrc=\"");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(16,11);to=(16,35)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(16,37);to=(17,2)]
                out.write("/js/public.js\"></script>\r\n\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(17,4);to=(25,2)]
                
                			response.addHeader("Pragma", "no-cache");
                			response.addHeader("Cache-Control", "no-cache");
                			response.addDateHeader("Expires", -1);
                			UserInfo userInfo = (UserInfo) session.getAttribute("userinfo");
                			GMCHelper gmcHelper = new GMCHelper();
                			//List<HashMap<String, String>> gmcOptFieldList =  gmcHelper.getGMCOptFieldList();
                			Iterator gmcOptFieldIt = gmcHelper.getGMCFunctionOptFieldList().iterator();
                		
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(25,4);to=(27,13)]
                out.write("\r\n\t\t<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(27,16);to=(27,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(27,42);to=(30,0)]
                out.write("\";\r\nvar optFieldAry = new Array();\r\n\r\n");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(30,2);to=(39,0)]
                
                
                	int cnt=0;
                	HashMap gmcOptField = null;
                	while (gmcOptFieldIt.hasNext()) {
                		gmcOptField = (HashMap)gmcOptFieldIt.next();		
                		out.println("optFieldAry["+ cnt++ +"] = new Array('" + gmcOptField.get("gmc_id")
                				+ "','" + gmcOptField.get("opt_prefix")+ "','" + gmcOptField.get("forder")+ "');");
                	}	
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(39,2);to=(240,14)]
                out.write("\r\nfunction onSubmit(){\r\n\t\r\n\t\r\n\tvar strErr=\"\";\r\n\t\r\n\t\r\n\tvar tabGMCFun=document.all(\"tabGMCFun\");\t\r\n\tvar rows=tabGMCFun.rows;\r\n\tfor(var i=0;i<rows.length;i++){\r\n\t\tif(rows[i].id!=\"\"&&\trows[i].style.display==\"\"){\r\n\t\t\tif((rows[i].cells[1].children[0].type==\"text\"||rows[i].cells[1].children[0].type==\"select-one\")&&rows[i].cells[1].children[0].value==\"\"){\r\n\t\t\t\tstrErr+=\"*\"+ rows[i].cells[0].innerHTML +\"不能为空！\\n\";\r\n\t\t\t}else if(rows[i].id==\"trGROUP\"){\r\n\t\t\t\tif(document.all(\"zonecode\").value==\"\"||document.all(\"selGateway\").value==\"\")\r\n\t\t\t\t\tstrErr+=\"*\"+ rows[i].cells[0].innerHTML +\"不能为空！\\n\";\r\n\t\t\t}\t\t\t\t\t\t\r\n\t\t} \r\n\t}\r\n\t\r\n\t\r\n\tif(strErr!=\"\"){\r\n\t\talert(strErr);\r\n\t\treturn false\r\n\t}else{\r\n\t\tvar selGMCFunction = document.all(\"gmcFunction\").options[document.all(\"gmcFunction\").selectedIndex];\r\n\t\tdocument.all(\"gmcCmd\").value =  selGMCFunction.getAttribute(\"category\") +\" \"+  selGMCFunction.getAttribute(\"command\") ;\r\n\t\t\r\n\t\tif(document.all(\"trTYPE\").style.display==\"none\")\r\n\t\t\tdocument.all(\"functionName\").value =  selGMCFunction.text;\r\n\t\telse{\t\t\r\n\t\t\tvar objSel = document.all(\"operationType\")\r\n\t\t\tvar objOption = objSel.options[objSel.selectedIndex];\r\n\t\t\tdocument.all(\"functionName\").value =  objOption.text;\t\t\r\n\t\t}\r\n\t\t\r\n\t\tvar objSel=document.all(\"selGateway\");\r\n\t\tvar objOption;\r\n\t\tobjOption = objSel.options[objSel.selectedIndex];\r\n\t\tif(objOption.getAttribute(\"group\")!=\"\"){\r\n\t\t\tdocument.all(\"GROUP\").value = objOption.getAttribute(\"area\") + \"-\" + objOption.getAttribute(\"group\");\t\r\n\t\t\tdocument.all(\"gateway\").value=objOption.text.substring(objOption.text.indexOf(\".\")+1,objOption.text.length) ;\t\r\n\t\t}\r\n\t\t\t\r\n\t\t//瞬移位置\r\n\t\tif(document.all(\"trDEST\").style.display==\"\"){\r\n\t\t\tobjSel = document.all(\"DEST\");\r\n\t\t\tobjOption = objSel.options[objSel.selectedIndex];\r\n\t\t\tdocument.all(\"positionname\").value=objOption.text;\r\n\t\t}else{\r\n\t\t\tdocument.all(\"DEST\").value=\"\";\r\n\t\t}\r\n\t\t\r\n\t\t//操作原因\r\n\t\tif(document.all(\"trT_DATA\").style.display==\"\"){\r\n\t\t\tdocument.all(\"message\").value=document.all(\"trT_DATA\").value;\t\t\r\n\t\t}else if(document.all(\"selReason\").value==\"other\"){\r\n\t\t\tdocument.all(\"message\").value=document.all(\"txtReason\").value;\r\n\t\t}else{\r\n\t\t\tdocument.all(\"message\").value=document.all(\"selReason\").value;\r\n\t\t}\r\n\t\treturn true;\r\n\t}\r\n\t\r\n}\r\n\r\nfunction onChangeGameId(){\r\n\t\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objSel2=document.all(\"DEST\");\r\n\tvar objSelTemp2=document.all(\"selPositionTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tobjSel2.length=0;\r\n\t\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value){\r\n\t\t \tif(j==0){\r\n\t\t\t\tobjSel.options[objSel.length]=new Option(j+\".所在区\",\"\");\r\n\t\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",\"\");\t\t\t \t\r\n\t\t \t}\r\n\t\t \tj++;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n\tj=0;\r\n\tfor(i=0;i<objSelTemp2.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp2.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tif(j==0){\r\n\t\t\t\tobjSel2.options[objSel2.length]=new Option(j+\".瞬移位置\",\"\");\r\n\t\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"game_id\",\"\");\t\r\n\t\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"positionname\",\"\");\t\r\n\t\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"game_id\",\"\");\t\t \t\r\n\t\t \t}\r\n\t\t \tj++;\r\n\t\t\tobjSel2.options[objSel2.length]=new Option(j+\".\"+objOption.positionname,objOption.value);\r\n\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"positionname\",objOption.positionname);\r\n\t\t\tobjSel2.options[objSel2.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\r\n\t}\r\n\tonChangeZoneCode();\r\n}\t\t\r\nfunction onChangeFunction(){\r\n\tvar gmc_id = document.all(\"gmcFunction\").value; \r\n\tvar curGMCFunction = new Array();\r\n\tvar cnt=0;\r\n\tvar tmpOptField = \"\";\r\n\tsetDisplayStyle(\"none\");\r\n\tfor(cnt=0;cnt<optFieldAry.length;cnt++){\r\n\t\tif(optFieldAry[cnt][0]==gmc_id&&document.all(\"tr\"+optFieldAry[cnt][1])!=undefined){\r\n\t\t\tdocument.all(\"tr\"+optFieldAry[cnt][1]).style.display=\"\"; \r\n\t\t}\r\n\t}\r\n\t\r\n\t//根据所选功能设置部分特殊控件显示效果\r\n\tvar objSel = document.all(\"gmcFunction\");\r\n\tif(objSel==undefined||objSel.length==0){\r\n\t\tdocument.all(\"submitBtn\").disabled=true;\r\n\t\treturn;\r\n\t}\r\n\tvar objOption = objSel.options[objSel.selectedIndex];\r\n\tif(objOption.getAttribute(\"category\")==\"R_N_CTRL\"&&objOption.getAttribute(\"command\")==\"CHAT\"){\t\r\n\t\tdocument.all(\"trGM\").style.display=\"none\"; \r\n\t\tdocument.all(\"trReason\").style.display=\"none\";\r\n\t\tdocument.all(\"trTerm\").style.display=\"none\"; \t\t\r\n\t}else if(objOption.getAttribute(\"category\")==\"G_M_CTRL\"&&objOption.getAttribute(\"command\")==\"FLY\"){\t\r\n\t\tdocument.all(\"trReason\").style.display=\"none\";\r\n\t\tdocument.all(\"trTerm\").style.display=\"none\"; \t\t\r\n\t}else if(objOption.getAttribute(\"command\")==\"FREEZE\"){\t\r\n\t\tdocument.all(\"trReason\").style.display=\"\";\r\n\t\tdocument.all(\"trTerm\").style.display=\"\"; \t\r\n\t}else{\t\r\n\t\tdocument.all(\"trReason\").style.display=\"\";\r\n\t}\r\n\t\t\r\n}\r\nfunction setDisplayStyle(state){\r\n\t\r\n\tvar tabGMCFun=document.all(\"tabGMCFun\");\t\r\n\tvar rows=tabGMCFun.rows;\r\n\tfor(var i=0;i<rows.length;i++){\r\n\t\tif(rows[i].id!=\"\"){\r\n\t\t\trows[i].style.display=state;\r\n\t\t} \r\n\t}\r\n\t\r\n}\r\nfunction onChangeCommandType(){\r\n\tvar objSel = document.all(\"operationType\")\r\n\tvar objOption = objSel.options[objSel.selectedIndex];\r\n\tdocument.all(\"TYPE\").value=objOption.getAttribute(\"type\");\r\n\tdocument.all(\"OPT\").value=objOption.getAttribute(\"option\");\r\n}\r\n\r\nfunction onChangeReason()\r\n{\r\n\tvar objSel=document.all(\"selReason\");\r\n\tvar objTr=document.all(\"trTxtReason\");\r\n\tif(objSel.value==\"other\")\r\n\t{\r\n\t\tobjTr.style.display=\"\";\r\n\t\tdocument.all(\"txtReason\").focus();\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjTr.style.display=\"none\";\r\n\t}\r\n}\r\n</script>\r\n\t</head>\r\n\t<body onload=\"onChangeFunction(),onChangeGameId(),onChangeCommandType()\">\r\n\t\t<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\"\r\n\t\t\tcellspacing=\"0\">\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"20\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\t&nbsp;&nbsp;当前位置：\r\n\t\t\t\t\t<span class=\"title\">服务功能</span> &gt;&gt;\r\n\t\t\t\t\t<span class=\"title\">GMC功能频道</span>\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td align=\"center\" valign=\"top\">\r\n\t\t\t\t\t<div id=\"divPrompt\"\r\n\t\t\t\t\t\tstyle=\"width: 350px; position: absolute; display: none; left: 240px; top: 230px; height: 44px; z-index: 1000\">\r\n\t\t\t\t\t\t<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\"\r\n\t\t\t\t\t\t\theight=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n\t\t\t\t\t</div>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<form id=\"frmSubmit\" method=\"post\"\r\n\t\t\t\t\t\taction=\"");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(240,17);to=(240,41)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(240,43);to=(243,50)]
                out.write("/GMCFunctionServlet\"\r\n\t\t\t\t\t\tonsubmit=\"return onSubmit()\">\r\n\t\t\t\t\t\t<input name=\"action\" type=\"hidden\" value=\"gmcfunction\">\r\n\t\t\t\t\t\t<input name=\"usercode\" type=\"hidden\" value=\"");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(243,53);to=(243,76)]
                out.print(userInfo.getUsercode() );
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(243,78);to=(266,9)]
                out.write("\"/>\r\n\t\t\t\t\t\t<input name=\"gmcCmd\" type=\"hidden\" value=\"\"/>\r\n\t\t\t\t\t\t<input name=\"message\" type=\"hidden\" value=\"\"/>\r\n\t\t\t\t\t\t<input name=\"gateway\" type=\"hidden\" value=\"\"/>\r\n\t\t\t\t\t\t<input name=\"GROUP\" type=\"hidden\" value=\"\"/>\r\n\t\t\t\t\t\t<input name=\"positionname\" type=\"hidden\" value=\"\"/>\r\n\t\t\t\t\t\t\r\n\r\n\t\t\t\t\t\t<table id=\"tabGMCFun\" width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\tclass=\"tablelist\">\r\n\t\t\t\t\t\t\t<tr align=\"left\" bgcolor=\"#419ADC\">\r\n\t\t\t\t\t\t\t\t<td height=\"31\" colspan=\"2\" nowrap>\r\n\t\t\t\t\t\t\t\t\t<font color=#ffffff><b>&nbsp;&#8226;&nbsp;GMC功能频道</b>\r\n\t\t\t\t\t\t\t\t\t</font>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\r\n\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t所属游戏\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(266,9);to=(267,30)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_id");
                _jspx_th_game_gameselect_0.setStyle("width:160");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(266,9);to=(267,30)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(267,30);to=(275,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\tGM功能\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"292\">\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(275,9);to=(277,30)]
                /* ----  gmcfunction:gmcfunctionselect ---- */
                com.kingsoft.gmsystem.web.taglib.GMCFunctionTag _jspx_th_gmcfunction_gmcfunctionselect_0 = new com.kingsoft.gmsystem.web.taglib.GMCFunctionTag();
                _jspx_th_gmcfunction_gmcfunctionselect_0.setPageContext(pageContext);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setParent(null);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setDisplayCode("select_function");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setSelectName("gmcFunction");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setOnChange("onChangeFunction()");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setStyle("width:160");
                try {
                    int _jspx_eval_gmcfunction_gmcfunctionselect_0 = _jspx_th_gmcfunction_gmcfunctionselect_0.doStartTag();
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GMCFunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(275,9);to=(277,30)]
                        } while (_jspx_th_gmcfunction_gmcfunctionselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_gmcfunction_gmcfunctionselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_gmcfunction_gmcfunctionselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(277,30);to=(286,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t<input type=\"hidden\" name=\"functionName\"/>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trTYPE\">\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t操作类型\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(286,9);to=(287,76)]
                /* ----  function:functionSelect ---- */
                com.kingsoft.gmsystem.web.taglib.FunctionTag _jspx_th_function_functionSelect_0 = new com.kingsoft.gmsystem.web.taglib.FunctionTag();
                _jspx_th_function_functionSelect_0.setPageContext(pageContext);
                _jspx_th_function_functionSelect_0.setParent(null);
                _jspx_th_function_functionSelect_0.setDisplayCode("select_r_n_ctrl");
                _jspx_th_function_functionSelect_0.setSelectName("operationType");
                _jspx_th_function_functionSelect_0.setParameter("behave");
                _jspx_th_function_functionSelect_0.setStyle("width:160");
                try {
                    int _jspx_eval_function_functionSelect_0 = _jspx_th_function_functionSelect_0.doStartTag();
                    if (_jspx_eval_function_functionSelect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.FunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_function_functionSelect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(286,9);to=(287,76)]
                        } while (_jspx_th_function_functionSelect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_function_functionSelect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_function_functionSelect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(287,76);to=(298,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t<input type=\"hidden\" name=\"OPT\" value=\"\"/>\r\n\t\t\t\t\t\t\t\t\t<input type=\"hidden\" name=\"TYPE\" value=\"\"/>\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trGROUP\">\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t所在区服\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"292\">\r\n\t\t\t\t\t\t\t\t\t<select name=\"zonecode\"></select>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(298,9);to=(299,59)]
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
                        // HTML // begin [file="/gmc/gmcfunction.jsp";from=(299,59);to=(300,9)]
                            out.write("\r\n\t\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(300,9);to=(300,27)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(300,27);to=(302,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t<select name=\"selGateway\"></select>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(302,9);to=(303,61)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_gateway");
                _jspx_th_display_display_1.setProperty("selGatewayTemp");
                _jspx_th_display_display_1.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/gmcfunction.jsp";from=(303,61);to=(304,9)]
                            out.write("\r\n\t\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(304,9);to=(304,27)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(304,27);to=(329,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trACC\">\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t玩家帐号\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"292\">\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"ACC\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trROLE\">\r\n\t\t\t\t\t\t\t\t<td width=\"132\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t角色\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"292\">\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"ROLE\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\r\n\t\t\t\t\t\t\t<tr id=\"trReason\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\">\r\n\t\t\t\t\t\t\t\t\t操作原因\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(329,9);to=(330,50)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_2 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_2.setPageContext(pageContext);
                _jspx_th_display_display_2.setParent(null);
                _jspx_th_display_display_2.setDisplayCode("select_reason");
                _jspx_th_display_display_2.setProperty("selReason");
                _jspx_th_display_display_2.setParameter("freeze");
                try {
                    int _jspx_eval_display_display_2 = _jspx_th_display_display_2.doStartTag();
                    if (_jspx_eval_display_display_2 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_2 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/gmcfunction.jsp";from=(330,50);to=(331,9)]
                            out.write("\r\n\t\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(331,9);to=(331,27)]
                        } while (_jspx_th_display_display_2.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_2.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_2.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(331,27);to=(366,9)]
                out.write("\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trTxtReason\" style=\"display: none\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\">\r\n\t\t\t\t\t\t\t\t\t请输入原因\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input name=\"txtReason\" type=\"text\" class=\"text\" size=\"40\"\r\n\t\t\t\t\t\t\t\t\t\tmaxlength=\"121\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\r\n\t\t\t\t\t\t\t<tr id=\"trGM\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\tGM角色\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"284\">\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"GM\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trTARGET\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t目标角色\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"TARGET\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trDEST\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\">\r\n\t\t\t\t\t\t\t\t\t瞬移位置\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<select name=\"DEST\" style='width: 200'></select>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/gmcfunction.jsp";from=(366,9);to=(367,62)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_3 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_3.setPageContext(pageContext);
                _jspx_th_display_display_3.setParent(null);
                _jspx_th_display_display_3.setDisplayCode("select_position");
                _jspx_th_display_display_3.setProperty("selPositionTemp");
                _jspx_th_display_display_3.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_3 = _jspx_th_display_display_3.doStartTag();
                    if (_jspx_eval_display_display_3 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_3 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/gmcfunction.jsp";from=(367,62);to=(368,9)]
                            out.write("\r\n\t\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/gmcfunction.jsp";from=(368,9);to=(368,27)]
                        } while (_jspx_th_display_display_3.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_3.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_3.release();
                }
            // end
            // HTML // begin [file="/gmc/gmcfunction.jsp";from=(368,27);to=(504,0)]
                out.write("\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\r\n\t\t\t\t\t\t\t<tr id=\"trT_DATA\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\">\r\n\t\t\t\t\t\t\t\t\t消息内容\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input name=\"T_DATA\" type=\"text\" class=\"text\" size=\"40\"\r\n\t\t\t\t\t\t\t\t\t\tmaxlength=\"121\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr id=\"trTerm\">\r\n\t\t\t\t\t\t\t\t<td height=\"30\" align=\"right\">\r\n\t\t\t\t\t\t\t\t\t冻结角色期限\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<select name=\"term\">\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"0\" selected>\r\n\t\t\t\t\t\t\t\t\t\t\t无限\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"1\">\r\n\t\t\t\t\t\t\t\t\t\t\t1\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"2\">\r\n\t\t\t\t\t\t\t\t\t\t\t2\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"3\">\r\n\t\t\t\t\t\t\t\t\t\t\t3\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"4\">\r\n\t\t\t\t\t\t\t\t\t\t\t4\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"5\">\r\n\t\t\t\t\t\t\t\t\t\t\t5\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"6\">\r\n\t\t\t\t\t\t\t\t\t\t\t6\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"7\">\r\n\t\t\t\t\t\t\t\t\t\t\t7\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"10\">\r\n\t\t\t\t\t\t\t\t\t\t\t10\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"15\">\r\n\t\t\t\t\t\t\t\t\t\t\t15\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"20\">\r\n\t\t\t\t\t\t\t\t\t\t\t20\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"25\">\r\n\t\t\t\t\t\t\t\t\t\t\t25\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"30\">\r\n\t\t\t\t\t\t\t\t\t\t\t30\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"35\">\r\n\t\t\t\t\t\t\t\t\t\t\t35\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"40\">\r\n\t\t\t\t\t\t\t\t\t\t\t40\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"50\">\r\n\t\t\t\t\t\t\t\t\t\t\t50\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t\t<option value=\"60\">\r\n\t\t\t\t\t\t\t\t\t\t\t60\r\n\t\t\t\t\t\t\t\t\t\t</option>\r\n\t\t\t\t\t\t\t\t\t</select>\r\n\t\t\t\t\t\t\t\t\t天\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t<br>\r\n\t\t\t\t\t\t<table width=\"200\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n\t\t\t\t\t\t\t<tr align=\"center\">\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"submit\" name=\"submitBtn\" value=\"确定\" />\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"reset\" value=\"取消\" />\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t</table>\r\n\t\t\t\t\t</form>\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\tCopyright &copy; 2005 Kingsoft Corp,All Rights Reserved\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t</table>\r\n\t</body>\r\n\t<script type=\"text/javascript\">\r\n\r\ndocument.getElementById(\"zonecode\").onchange = function onChangeZoneCode(){\r\n\t\r\n\tvar objSel=document.all(\"selGateway\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tvar txtGateway = \"\";\r\n\tobjSel.options[objSel.length]=new Option(j+\".所在服\",\"\");\t \t\r\n\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",\"\");\r\n\tobjSel.options[objSel.length-1].setAttribute(\"area\",\"\");\r\n\tobjSel.options[objSel.length-1].setAttribute(\"group\",\"\");\r\n\t\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value&&objOption.zonecode==document.all(\"zonecode\").value)\r\n\t\t{\r\n\t\t \tif(j==0){\r\n\t\t\t\tobjSel.length=0;\r\n\t\t\t\tobjSel.options[objSel.length]=new Option(j+\".所在服\",\"\");\t \t\r\n\t\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",\"\");\r\n\t\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",\"\");\r\n\t\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",\"\");\r\n\t\t \t}\r\n\t\t \tj++;\r\n\t\t \ttxtGateway=objOption.text;\r\n\t\t \ttxtGateway = txtGateway.substring(txtGateway.indexOf(\".\"),txtGateway.length);\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+txtGateway,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t}\r\n\t}\r\n\t\r\n}\r\n</script>\r\n</html>\r\n");

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
