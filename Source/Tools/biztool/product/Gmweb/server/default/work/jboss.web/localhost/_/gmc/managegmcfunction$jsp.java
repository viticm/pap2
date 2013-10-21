package org.apache.jsp;

import com.kingsoft.gmsystem.service.GMCHelper;
import java.util.Iterator;
import java.util.HashMap;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class managegmcfunction$jsp extends HttpJspBase {


    static {
    }
    public managegmcfunction$jsp( ) {
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

            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(2,88);to=(4,0)]
                out.write("\r\n\r\n");

            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(4,64);to=(5,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(5,55);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(6,0);to=(6,51)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("managegmcfunction");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/managegmcfunction.jsp";from=(6,0);to=(6,51)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(6,51);to=(16,13)]
                out.write("\r\n<html>\r\n\t<!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n\t<head>\r\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n\t\t<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n\t\t<title>GMC功能维护</title>\r\n\t\t<!-- InstanceEndEditable -->\r\n\t\t<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n\t\t<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(16,16);to=(16,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(16,42);to=(20,2)]
                out.write("\";\r\n</script>\r\n\t\t<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n\t\t<!-- InstanceBeginEditable name=\"head\" -->\r\n\t\t");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(20,4);to=(32,2)]
                
                			//this prevents caching in NS and IE
                			response.addHeader("Pragma", "no-cache");
                			response.addHeader("Cache-Control", "no-cache");
                			response.addDateHeader("Expires", -1);
                			GMCHelper gmcHelper = new GMCHelper();
                			//List<HashMap<String, String>> gmcOptFieldList =  gmcHelper.getGMCOptFieldList();
                			Iterator gmcFunctionIt = gmcHelper.getGMCFunctionList().iterator();
                			Iterator gmcFunOptFieldIt = gmcHelper.getGMCFunctionOptFieldList()
                					.iterator();
                			int cnt = 0;
                			HashMap hm = null;
                		
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(32,4);to=(38,0)]
                out.write("\r\n\t\t<script language=\"Javascript\">\r\nvar objChked=null;//当前被选中的\r\nvar action=null;\r\nvar optFunFieldAry = new Array();\r\n\r\n");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(38,2);to=(46,0)]
                
                
                	HashMap gmcOptField = null;
                	while (gmcFunOptFieldIt.hasNext()) {
                		gmcOptField = (HashMap)gmcFunOptFieldIt.next();		
                		out.println("optFunFieldAry["+ cnt++ +"] = new Array('" + gmcOptField.get("gmc_id")
                				+ "','" + gmcOptField.get("opt_prefix")+ "','" + gmcOptField.get("opt_id")+ "');");
                	}	
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(46,2);to=(233,9)]
                out.write("\r\n\r\n\r\nfunction onClickGMCFunction(objGMCFunction){\r\n\tobjChked = objGMCFunction;\r\n\tvar gmcCmdID = \"\";\r\n\tgmcCmdID = objChked.value;\r\n\tdocument.all(\"gmc_id\").value=objChked.value;  \r\n\tdocument.all(\"category\").value=objChked.getAttribute(\"category\");\r\n\tdocument.all(\"command\").value=objChked.getAttribute(\"command\");\r\n\tdocument.all(\"cmdName\").value=objChked.getAttribute(\"cmdName\");  \r\n\t\r\n\tvar objSel1=document.all(\"selArrangedOptField\");\r\n\tvar objSel2=document.all(\"selChkOptField\");\r\n\tvar objSelTemp=document.all(\"tmpOptionalField\");\r\n\t\r\n\tvar objOption;\r\n\tobjSel1.length=0;\r\n\tobjSel2.length=0;\r\n\t\r\n\t\r\n\tfor(j=0;j<optFunFieldAry.length;j++){\r\n\t\tif(optFunFieldAry[j][0]==gmcCmdID){\r\n\t\t\tobjSel2.options[objSel2.length]=new Option(optFunFieldAry[j][1],optFunFieldAry[j][2]);\t\r\n\t\t}\r\n\t}\r\n\tvar isChk = \"0\";\r\n\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\tisChk = \"0\";\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tfor(j=0;j<objSel2.length;j++){\r\n\t\t\tif(objOption.text==objSel2.options[j].text){\r\n\t\t\t\tisChk = \"1\";\r\n\t\t\t\tbreak;\r\n\t\t\t}\r\n\t\t}\t\t\r\n\t\tif(isChk==\"0\")\r\n\t\t\tobjSel1.options[objSel1.length]=new Option(objOption.text,objOption.value);\t\t\t\r\n\t}\t\r\n\t\r\n\tdocument.all(\"btnModify\").disabled=false;\r\n\tdocument.all(\"btnDel\").disabled=false;\r\n}\r\n\r\n\r\nfunction setIsDisable(state){\r\n\tdocument.all(\"category\").disabled=state;\r\n\tdocument.all(\"command\").disabled=state;\r\n\tdocument.all(\"cmdName\").disabled=state;  \r\n\t\r\n\tvar radioTmp = document.all(\"gmcCmdID\");  \r\n\tif(radioTmp!=undefined){\r\n\t\tfor (i=0;i<radioTmp.length;i++){  \r\n\t\t\tif(document.all(\"action\").value!=\"\"){\r\n\t\t\t     radioTmp[i].disabled=true;  \r\n\t\t\t}else {\r\n\t\t\t     radioTmp[i].disabled=false;  \r\n\t\t\t} \r\n\t\t}\t\r\n\t}\r\n\tif(objChked!=null){\r\n\t\tdocument.all(\"btnModify\").disabled=!state;\r\n\t\tdocument.all(\"btnDel\").disabled=!state;\r\n\t}\r\n\tdocument.all(\"btnAdd\").disabled=!state;\r\n\tdocument.all(\"btnSave\").disabled=state;\r\n\tdocument.all(\"btnCancel\").disabled=state;\r\n\t\r\n\tdocument.all(\"selArrangedOptField\").disabled=state;\r\n\tdocument.all(\"selChkOptField\").disabled=state;  \t\r\n}\r\n\r\n\r\nfunction onAdd(){\t\t\r\n\tdocument.all(\"action\").value=\"addGMCFunction\";\r\n\tresetValue();\r\n\tvar objSel=document.all(\"selArrangedOptField\");\r\n\tvar objSelTemp=document.all(\"tmpOptionalField\");\r\n\t\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\t\r\n\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tobjSel.options[objSel.length]=new Option(objOption.text,objOption.value);\t\t\t\r\n\t}\t\r\n\tsetIsDisable(false);\r\n\t\r\n}\r\nfunction onModify(){\t\r\n\tdocument.all(\"action\").value=\"modifyGMCFunction\";\r\n\tresetValue();\r\n\tsetIsDisable(false);\r\n}\r\n\r\nfunction onDel(){\r\n\tif(confirm(\"你确定要删除此GMC功能吗？\")){\r\n\t\tdocument.all(\"action\").value=\"delGMCFunction\";\r\n\t\tdocument.all(\"frmSubmit\").submit();\r\n\t}\r\n}\r\n\r\n\r\nfunction onSave(){\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"category\")) strErr+=\"*类别不能为空！\\n\";\r\n\tif(IsEmpty(\"command\")) strErr+=\"*操作命令不能空！\\n\";\r\n\tif(IsEmpty(\"cmdName\")) strErr+=\"*命令名称不能空！\\n\";\r\n\tif(document.all(\"selChkOptField\").length==0) strErr+=\"*已选字段不能空！\\n\";\r\n\tif(document.all(\"selChkOptField\").length>14) strErr+=\"*已选字段不能超过14个！\\n\";\r\n\t\r\n\tif(strErr!=\"\"){\r\n\t\talert(strErr);\r\n\t}else{\r\n\t\tvar objSel=document.all(\"selChkOptField\");\r\n\t\tvar optFields = \"\";\r\n\t\tfor(i=0;i<objSel.length;i++){\r\n\t\t\toptFields += objSel.options[i].value+\",\";\t\t\r\n\t\t}\t\r\n\t\toptFields = optFields.substring(0,optFields.length-1);\r\n\t\t\r\n\t\tdocument.all(\"optFields\").value=optFields;\r\n\t\tdocument.all(\"frmSubmit\").submit();\t\r\n\t}\r\n}\r\nfunction onCancel(){\r\n\tresetValue();\r\n\tdocument.all(\"action\").value=\"\";\r\n\tsetIsDisable(true);\r\n}\r\nfunction resetValue(){\t\r\n\tif(document.all(\"action\").value==\"addGMCFunction\"){\r\n\t\tdocument.all(\"category\").value=\"\";\r\n\t\tdocument.all(\"command\").value=\"\";\r\n\t\tdocument.all(\"cmdName\").value=\"\";\r\n\t\t\r\n\t\tdocument.all(\"selArrangedOptField\").length=0;\r\n\t\tdocument.all(\"selChkOptField\").length=0;\t\r\n\t\tif(objChked!=null){\r\n\t\t\tobjChked.blur();\r\n\t\t}\r\n\t}\r\n\telse if(document.all(\"action\").value==\"modifyGMCFunction\"){\r\n\t\tonClickGMCFunction(objChked);\r\n\t}\r\n}\r\n\r\n</script>\r\n\t\t<!-- InstanceEndEditable -->\r\n\t</head>\r\n\t<body onload=\"setIsDisable(true)\">\r\n\t\t<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\"\r\n\t\t\tcellspacing=\"0\">\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"20\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\t&nbsp;&nbsp;当前位置：\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"position\" -->\r\n\t\t\t\t\t<span class=\"title\">系统功能</span> &gt;&gt;\r\n\t\t\t\t\t<span class=\"title\">GMC功能维护</span>\r\n\t\t\t\t\t<!-- InstanceEndEditable -->\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td align=\"center\" valign=\"top\">\r\n\t\t\t\t\t<!-- InstanceBeginEditable name=\"main\" -->\r\n\t\t\t\t\t<br>\r\n\t\t\t\t\t<table width=\"95%\" border=\"0\" cellspacing=\"0\">\r\n\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t<td height=\"200\" align=\"center\" valign=\"top\">\r\n\t\t\t\t\t\t\t\t<table width=\"80%\" border=\"1\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\t\t\tbordercolor=\"#000000\" class=\"tablelist\">\r\n\t\t\t\t\t\t\t\t\t<thead>\r\n\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"7%\" align=\"center\" nowrap width=\"30\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">序号</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"10%\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">类别</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"17%\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">操作命令</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"15%\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<font color=\"#FFFFFF\">命令名称</font>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t</thead>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(233,11);to=(237,9)]
                
                										cnt = 0;
                										while (gmcFunctionIt.hasNext()) {
                											hm = (HashMap) gmcFunctionIt.next();
                									
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(237,11);to=(238,13)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t<tr ");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(238,15);to=(238,28)]
                if(cnt%2==0){
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(238,30);to=(238,49)]
                out.write(" bgcolor=\"#F3FBFE\" ");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(238,51);to=(238,52)]
                }
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(238,54);to=(242,19)]
                out.write(">\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\" valign=\"middle\" nowrap>\r\n\t\t\t\t\t\t\t\t\t\t\t<input type=\"radio\" name=\"gmcCmdID\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tonclick=\"onClickGMCFunction(this)\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tvalue=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(242,22);to=(242,35)]
                out.print(hm.get("iid"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(242,37);to=(243,22)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tcategory=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(243,25);to=(243,43)]
                out.print(hm.get("category"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(243,45);to=(244,21)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tcommand=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(244,24);to=(244,41)]
                out.print(hm.get("command"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(244,43);to=(245,21)]
                out.write("\"\r\n\t\t\t\t\t\t\t\t\t\t\t\tcmdname=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(245,24);to=(245,41)]
                out.print(hm.get("cmdname"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(245,43);to=(245,45)]
                out.write("\">");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(245,48);to=(245,53)]
                out.print(++cnt);
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(245,55);to=(247,29)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(247,32);to=(247,50)]
                out.print(hm.get("category"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(247,52);to=(248,29)]
                out.write("</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(248,32);to=(248,49)]
                out.print(hm.get("command"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(248,51);to=(249,29)]
                out.write("</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(249,32);to=(249,49)]
                out.print(hm.get("cmdname"));
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(249,51);to=(251,9)]
                out.write("</td>\r\n\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(251,11);to=(253,9)]
                
                										}
                									
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(253,11);to=(292,14)]
                out.write("\r\n\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t<td height=\"30\" align=\"center\">\r\n\t\t\t\t\t\t\t\t<table width=\"328\" height=\"15\" border=\"0\" cellspacing=\"0\">\r\n\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t<button name=\"btnAdd\" onClick=\"onAdd()\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t增加\r\n\t\t\t\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t<button name=\"btnModify\" onClick=\"onModify()\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t修改\r\n\t\t\t\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t<button name=\"btnDel\" onClick=\"onDel()\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t删除\r\n\t\t\t\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t<button name=\"btnSave\" onClick=\"onSave()\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t保存\r\n\t\t\t\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t<td align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t<button name=\"btnCancel\" onClick=\"onCancel()\" disabled>\r\n\t\t\t\t\t\t\t\t\t\t\t\t取消\r\n\t\t\t\t\t\t\t\t\t\t\t</button>\r\n\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t</table>\r\n\t\t\t\t\t<form id=\"frmSubmit\" method=\"post\"\r\n\t\t\t\t\t\taction=\"");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(292,17);to=(292,41)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(292,43);to=(346,12)]
                out.write("/GMCFunctionServlet\"\r\n\t\t\t\t\t\tonsubmit=\"return onSubmit()\">\r\n\t\t\t\t\t\t<input type=\"hidden\" name=\"action\" />\r\n\t\t\t\t\t\t<input type=\"hidden\" name=\"optFields\" />\r\n\t\t\t\t\t\t<input type=\"hidden\" name=\"gmc_id\" />\r\n\r\n\t\t\t\t\t\t<table id=\"tabGMCFun\" width=\"430\" border=\"1\" cellpadding=\"0\"\r\n\t\t\t\t\t\t\tcellspacing=\"0\" class=\"tablelist\">\r\n\r\n\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td width=\"80\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t类别：\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"category\" maxlength=\"8\" class=\"text\"\r\n\t\t\t\t\t\t\t\t\t\tonkeyup=\"value=value.replace(/[\\W]/g,'')\" onchange=\"value=value.replace(/[\\W]/g,'')\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"80\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t操作命令：\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"command\" maxlength=\"11\" class=\"text\"\r\n\t\t\t\t\t\t\t\t\t\tonkeyup=\"value=value.replace(/[\\W]/g,'')\" onchange=\"value=value.replace(/[\\W]/g,'')\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td width=\"80\" height=\"30\" align=\"right\" nowrap>\r\n\t\t\t\t\t\t\t\t\t命令名称：\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t<input type=\"text\" name=\"cmdName\" maxlength=\"25\" class=\"text\">\r\n\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t<td colspan=\"6\">\r\n\t\t\t\t\t\t\t\t\t<table width=\"700\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\t\t\t\tclass=\"tablelist\">\r\n\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"340\" height=\"30\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t备选字段\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"20\" valign=\"middle\" rowspan=\"2\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<a\r\n\t\t\t\t\t\t\t\t\t\t\t\t\thref=\"javascript:selItemMove('selChkOptField','selArrangedOptField')\">&larr;</a>&nbsp;&nbsp;&nbsp;\r\n\t\t\t\t\t\t\t\t\t\t\t\t<a\r\n\t\t\t\t\t\t\t\t\t\t\t\t\thref=\"javascript:selItemMove('selArrangedOptField','selChkOptField')\">&rarr;</a>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"340\" height=\"30\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t已选字段\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t<tr>\r\n\t\t\t\t\t\t\t\t\t\t\t<td width=\"340\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t<select name=\"selArrangedOptField\" size=\"12\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tstyle=\"width: 200\" multiple=\"multiple\"></select>\r\n\t\t\t\t\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/managegmcfunction.jsp";from=(346,12);to=(349,36)]
                /* ----  gmcfunction:gmcfunctionselect ---- */
                com.kingsoft.gmsystem.web.taglib.GMCFunctionTag _jspx_th_gmcfunction_gmcfunctionselect_0 = new com.kingsoft.gmsystem.web.taglib.GMCFunctionTag();
                _jspx_th_gmcfunction_gmcfunctionselect_0.setPageContext(pageContext);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setParent(null);
                _jspx_th_gmcfunction_gmcfunctionselect_0.setDisplayCode("select_optionalField");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setSelectName("tmpOptionalField");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setOnChange("onChangeFunction()");
                _jspx_th_gmcfunction_gmcfunctionselect_0.setStyle("display:none");
                try {
                    int _jspx_eval_gmcfunction_gmcfunctionselect_0 = _jspx_th_gmcfunction_gmcfunctionselect_0.doStartTag();
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GMCFunctionTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_gmcfunction_gmcfunctionselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/managegmcfunction.jsp";from=(346,12);to=(349,36)]
                        } while (_jspx_th_gmcfunction_gmcfunctionselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_gmcfunction_gmcfunctionselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_gmcfunction_gmcfunctionselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/managegmcfunction.jsp";from=(349,36);to=(382,0)]
                out.write("\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\tclass=\"tablelist\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t<tr width=\"340\" align=\"center\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<select name=\"selChkOptField\" size=\"12\" style=\"width: 200\"\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tmultiple=\"multiple\"></select>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t<td valign=\"middle\">\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<a href=\"javascript:selItemUp('selChkOptField')\">&uarr;</a>&nbsp;&nbsp;&nbsp;\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t<a href=\"javascript:selItemDown('selChkOptField')\">&darr;</a>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t\t\t\t\t</td>\r\n\t\t\t\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t\t\t\t</table>\r\n\t\t\t\t\t\t\t\t<td>\r\n\t\t\t\t\t\t\t</tr>\r\n\t\t\t\t\t\t</table>\r\n\t\t\t\t\t</form>\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t\t<tr>\r\n\t\t\t\t<td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">\r\n\t\t\t\t\tCopyright &copy; 2005 Kingsoft Corp,All Rights Reserved\r\n\t\t\t\t</td>\r\n\t\t\t</tr>\r\n\t\t</table>\r\n\t</body>\r\n</html>\r\n");

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
