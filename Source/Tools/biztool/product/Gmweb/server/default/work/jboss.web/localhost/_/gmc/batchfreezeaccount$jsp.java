package org.apache.jsp;

import java.util.HashMap;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.UserInfo;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class batchfreezeaccount$jsp extends HttpJspBase {


    static {
    }
    public batchfreezeaccount$jsp( ) {
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

            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(0,189);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(2,54);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(4,0);to=(4,52)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("batchfreezeaccount");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/batchfreezeaccount.jsp";from=(4,0);to=(4,52)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(4,52);to=(9,12)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>帐号一般控制</title>\r\n<link href=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(9,15);to=(9,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(9,41);to=(10,35)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(10,38);to=(10,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(10,64);to=(11,0)]
                out.write("/js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(11,2);to=(24,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                
                	String path = request.getContextPath();
                	String basePath = request.getScheme() + "://"
                			+ request.getServerName() + ":" + request.getServerPort()
                			+ path + "/";
                			
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
                    
                    
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(24,2);to=(26,13)]
                out.write("\r\n<script language=\"javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(26,16);to=(26,24)]
                out.print(basePath);
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(26,26);to=(27,21)]
                out.write("\";\r\nvar accountFileName=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(27,24);to=(27,92)]
                out.print(Util.setNullToEmpty((String)request.getAttribute("accountFileName")));
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(27,94);to=(28,13)]
                out.write("\";\r\nvar message=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(28,16);to=(28,76)]
                out.print(Util.setNullToEmpty((String)request.getAttribute("message")));
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(28,78);to=(94,50)]
                out.write("\";\r\nvar flag=\"\";\r\nfunction freezeByTable(){\r\n\tif(\"\"!=accountFileName&&\"\"!=message){\r\n\t\tflag = window.confirm(\"文件已经上传成功，请确认是否冻结此批帐号？\");\r\n\t\t\r\n\t\tif(flag==1){\r\n\t\t\tdocument.all(\"frmFreezeSubmit\").submit();\r\n\t\t}\r\n\t}\r\n}\r\n/*************************\r\n确定\r\n*************************/\r\nfunction onSubmit()\r\n{\r\n\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"accountFile\")) strErr+=\"*帐号文件不能为空\\n\";\r\n\tvar accountFile = document.all(\"accountFile\").value;\r\n\tvar extName = accountFile.substring(accountFile.length-4,accountFile.length);\r\n\tif(extName!=\".txt\") strErr+=\"*帐号文件只能是txt格式\\n\";\r\n\tif(document.all(\"selReason\").value==\"other\"&& IsEmpty(\"txtReason\"))  strErr+=\"*冻结帐号原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t\treturn false;\r\n\t}else{\r\n\t\tdocument.all(\"frmSubmit\").action += \"&term=\"+document.all(\"selTerm\").value;\r\n\t\tif(document.all(\"selReason\").value!=\"other\")\r\n\t\t\tdocument.all(\"frmSubmit\").action += \"&message=\"+document.all(\"selReason\").value;\r\n\t\telse\r\n\t\t\tdocument.all(\"frmSubmit\").action += \"&message=\"+document.all(\"txtReason\").value;\r\n\t\treturn true;\r\n\t}\r\n}\r\n\r\nfunction onChangeReason()\r\n{\r\n\tvar objSel=document.all(\"selReason\");\r\n\tvar objTr=document.all(\"trReason\");\r\n\tif(objSel.value==\"other\")\r\n\t{\r\n\t\tobjTr.style.display=\"\";\r\n\t\tdocument.all(\"txtReason\").focus();\r\n\t}\r\n\telse\r\n\t{\r\n\t\tobjTr.style.display=\"none\";\r\n\t}\r\n}\r\n\r\n\r\n</script>\r\n</head>\r\n<body onload=\"freezeByTable()\">\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">批量冻结帐号</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\t\t<br><br><br><br><br><br>\r\n\r\n\t<form id=\"frmFreezeSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(94,53);to=(94,77)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(94,79);to=(95,45)]
                out.write("/GMCServlet?action=batchFreezeByTable\" method=\"post\">\r\n\t\t<input type=\"hidden\" name=\"message\" value=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(95,48);to=(95,108)]
                out.print(Util.setNullToEmpty((String)request.getAttribute("message")));
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(95,110);to=(96,53)]
                out.write("\">\r\n\t\t<input type=\"hidden\" name=\"accountFileName\" value=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(96,56);to=(96,124)]
                out.print(Util.setNullToEmpty((String)request.getAttribute("accountFileName")));
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(96,126);to=(97,42)]
                out.write("\">\r\n\t\t<input type=\"hidden\" name=\"term\" value=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(97,45);to=(97,102)]
                out.print(Util.setNullToEmpty((String)request.getParameter("term")));
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(97,104);to=(99,44)]
                out.write("\">\r\n\t</form>\r\n\t<form id=\"frmSubmit\" method=\"post\" action=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(99,47);to=(99,71)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(99,73);to=(101,45)]
                out.write("/GMCServlet?action=uploadAccountFile\" onsubmit=\"return onSubmit()\"\r\n\t\t\tenctype=\"multipart/form-data\" method=\"post\">\r\n\t<input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(101,48);to=(101,70)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(101,72);to=(109,14)]
                out.write("\">\r\n      <table width=\"430\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n        <tr align=\"left\" bgcolor=\"#419ADC\">\r\n          <td height=\"31\" colspan=\"2\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;批量冻结帐号</b></font></td>\r\n        </tr>\r\n        \r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>所属游戏：</td>\r\n          <td>");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(109,14);to=(109,97)]
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
                        // begin [file="/gmc/batchfreezeaccount.jsp";from=(109,14);to=(109,97)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(109,97);to=(121,6)]
                out.write("</td>\r\n        </tr>\r\n        \r\n        <tr>\r\n          <td width=\"132\" height=\"30\" align=\"right\" nowrap><font color=\"#FF0000\">*</font>帐号文件：</td>\r\n          <td width=\"292\">\r\n\t\t\t<input type=\"file\" name=\"accountFile\"></td>\r\n        </tr>\r\n        \r\n        <tr>\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>操作原因：</td>\r\n          <td>\r\n\t\t    ");

            // end
            // begin [file="/gmc/batchfreezeaccount.jsp";from=(121,6);to=(121,91)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_reason");
                _jspx_th_display_display_0.setProperty("selReason");
                _jspx_th_display_display_0.setParameter("freeze");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(121,91);to=(122,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/batchfreezeaccount.jsp";from=(122,12);to=(122,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/batchfreezeaccount.jsp";from=(122,30);to=(168,0)]
                out.write("\r\n            </td>\r\n        </tr>\r\n        <tr id=\"trReason\" style=\"display:none\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>请输入原因：</td>\r\n          <td><input name=\"txtReason\" type=\"text\" class=\"text\" size=\"40\" maxlength=\"121\"></td>\r\n        </tr>\r\n        <tr id=\"trTerm\">\r\n          <td height=\"30\" align=\"right\"><font color=\"#FF0000\">*</font>冻结帐号期限：</td>\r\n          <td><select name=\"selTerm\">\r\n            <option value=\"0\" selected>无限</option>\r\n            <option value=\"1\">1</option>\r\n            <option value=\"2\">2</option>\r\n            <option value=\"3\">3</option>\r\n            <option value=\"4\">4</option>\r\n            <option value=\"5\">5</option>\r\n            <option value=\"6\">6</option>\r\n            <option value=\"7\">7</option>\r\n            <option value=\"10\">10</option>\r\n            <option value=\"15\">15</option>\r\n            <option value=\"20\">20</option>\r\n            <option value=\"25\">25</option>\r\n            <option value=\"30\">30</option>\r\n            <option value=\"35\">35</option>\r\n            <option value=\"40\">40</option>\r\n            <option value=\"50\">50</option>\r\n            <option value=\"60\">60</option>\r\n          </select>\r\n            天</td>\r\n        </tr>\r\n      </table>\r\n\t  <br><table width=\"200\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\r\n  <tr align=\"center\">\r\n    <td><input type=\"submit\" value=\"确定\"/></td>\r\n    <td><input type=\"reset\" value=\"取消\"/></td>\r\n  </tr>\r\n</table>\r\n\t</form>\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
