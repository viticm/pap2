package org.apache.jsp;

import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class integrate$jsp extends HttpJspBase {


    static {
    }
    public integrate$jsp( ) {
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

            // HTML // begin [file="/gmc/integrate.jsp";from=(0,147);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/integrate.jsp";from=(3,0);to=(3,43)]
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
                        // begin [file="/gmc/integrate.jsp";from=(3,0);to=(3,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(3,43);to=(10,0)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>GM频道处理</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/integrate.jsp";from=(10,2);to=(15,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(15,2);to=(17,13)]
                out.write("\r\n<script language=\"Javascript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/integrate.jsp";from=(17,16);to=(17,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(17,42);to=(220,48)]
                out.write("\";\r\n\r\n//取得系统当前日期\r\n\r\nfunction CheckEnter()\r\n{\r\n\tif(event.ctrlKey && window.event.keyCode==10){\r\n\t\t onAnswer();\r\n\t}\r\n}\r\n\r\nfunction SetSubmitData()\r\n{\r\n\t\tfrmSubmit.document.all(\"questionid\").value=document.all(\"questionid\").value;\r\n\t\tfrmSubmit.document.all(\"quiztime\").value=document.all(\"quiztime\").value;\r\n\t\tfrmSubmit.document.all(\"gateway\").value=document.all(\"selGateway\").options[document.all(\"selGateway\").selectedIndex].value;\r\n\t\tfrmSubmit.document.all(\"game_id\").value=document.all(\"game_id\").value;\r\n\t\tfrmSubmit.document.all(\"area\").value=document.all(\"area\").value;\r\n\t\tfrmSubmit.document.all(\"group\").value=document.all(\"group\").value;\r\n\t\tfrmSubmit.document.all(\"account\").value=document.all(\"account\").value;\r\n\t\tfrmSubmit.document.all(\"role\").value=document.all(\"role\").value;\r\n\t\tfrmSubmit.document.all(\"question\").value=document.all(\"question\").value;\r\n\t\tfrmSubmit.document.all(\"answer\").value=document.all(\"answer\").value;\r\n\t\tfrmSubmit.document.all(\"usercode\").value=document.all(\"usercode\").value;\r\n\t\tfrmSubmit.document.all(\"term\").value=document.all(\"term\").value;\r\n\t\tfrmSubmit.document.all(\"allquestion\").value=document.all(\"allquestion\").checked?\"1\":\"0\";\r\n\t\tfrmSubmit.document.all(\"message\").value=document.all(\"answer\").value;\r\n}\r\n\r\n/***********************************************\r\n答复问题\r\n***********************************************/\r\n\r\nfunction onAnswer()\r\n{\r\n\t\tvar strErr=\"\";\r\n\t\tif(IsEmpty(\"answer\"))\r\n\t\t{\r\n\t\t\tstrErr+=\"答复内容不能为空！\\n\";\r\n\t\t}\r\n\t\telse if(document.all(\"answer\").value.length>106)\tstrErr+=\"答复内容已超过106个字符(53个汉字)，请缩短答复内容后再试！\\n\";\r\n\t\tif(strErr!=\"\")\r\n\t\t{\r\n\t\t\talert(strErr);\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tSetSubmitData();\r\n\t\t\tfrmSubmit.document.all(\"action\").value=\"answerquestion\";\r\n\t\t\tfrmSubmit.document.all(\"frmSubmit\").submit();\r\n\t\t\tfrmPlayerList.objOption.answer=document.all(\"answer\").value;\r\n\t\t\tSetQuestionState();\r\n\t\t}\r\n}\r\n\r\n/***********************************************\r\n删除问题\r\n***********************************************/\r\nfunction onDelete()\r\n{\r\n\t\tSetSubmitData();\r\n\t\tfrmSubmit.document.all(\"action\").value=\"deletequestion\";\r\n\t\tfrmSubmit.document.all(\"frmSubmit\").submit();\r\n\t\tfrmPlayerList.objOption.answer=\"此问题已删除！\";\r\n\t\tSetQuestionState();\r\n}\r\n\r\nfunction SetQuestionState()\r\n{\r\n\t\tfrmPlayerList.document.all(\"selPlayerList\").focus();\r\n\t\tfrmSubmit.src=AbsPath+\"/submit.jsp\";\r\n\t\tfrmPlayerList.objOption.className=\"haveanswer\";\r\n\t\tfrmPlayerList.intHaveAnswer++;\r\n\t\tdocument.all(\"lblHaveAnswer\").innerText=frmPlayerList.intHaveAnswer;\r\n\t\tdocument.all(\"answer\").disabled=true;\r\n\t\tdocument.all(\"selQuickanswer\").disabled=true;\r\n\t\tdocument.all(\"btnAnswer\").disabled=true;\r\n\t\tdocument.all(\"btnDelete\").disabled=true;\r\n\t\tif(frmPlayerList.intHaveAnswer!=frmPlayerList.intQuestion)\r\n\t\t{\r\n\t\t\tvar objSelPlayList=frmPlayerList.document.all(\"selPlayerList\");//玩家列表\r\n\t\t\tdo\r\n\t\t\t{\r\n\t\t\t \tfrmPlayerList.intCurrent++;\r\n\t\t\t\tif(frmPlayerList.intCurrent==frmPlayerList.intQuestion) frmPlayerList.intCurrent=0;\r\n\t\t\t}while(objSelPlayList.options[frmPlayerList.intCurrent].className==\"haveanswer\")\r\n\t\t\tobjSelPlayList.selectedIndex =frmPlayerList.intCurrent;\r\n\t\t\tfrmPlayerList.onSelectPlayer();\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tReloadPlayerList();\r\n\t\t}\r\n\t\tif(document.all(\"allquestion\").checked){\r\n\t\t\tReloadPlayerList();\r\n\t\t}\r\n\t\tdocument.all(\"allquestion\").checked=false;\r\n}\r\n\r\n/***********************************************\r\n刷新问题列表\r\n***********************************************/\r\nfunction ReloadPlayerList()\r\n{\r\n\tdocument.all(\"allquestion\").checked=false;\r\n\tfrmPlayerList.window.location.reload();\r\n}\r\n\r\n/***********************************************\r\n选择快速答复列表\r\n***********************************************/\r\nfunction onQuickanswer()\r\n{\r\n\tdocument.all(\"answer\").value=document.all(\"selQuickanswer\").value\r\n}\r\n\r\nfunction answerlog(){\r\n\twindow.open(AbsPath+\"/search/answerlog.jsp\",\"_blank\");\r\n}\r\n/**********************************\r\n执行GM操作\r\n***********************************/\r\nfunction onGMFunction()\r\n{\r\n\tvar objSel=document.all(\"selGMFunction\");\r\n\tvar strErr=\"\";\r\n\tif(objSel.value==\"\")\r\n\t{\r\n\t\tstrErr+=\"*请先选择GM操作\\n\";\r\n\t}\r\n\tif(IsEmpty(\"account\")&&IsEmpty(\"role\"))\r\n\t{\r\n\t\tstrErr+=\"*玩家帐号和角色不能同时为空\\n\";\r\n\t}\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tSetSubmitData();\r\n\t\tfrmSubmit.document.all(\"action\").value=\"quickgmfunction\";\r\n\t\tif(objSel.value==\"踢号\")\r\n\t\t{\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"卡号\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"R_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"kickaccount\";\r\n\t\t}\r\n\t\telse if(objSel.value==\"踢人\")\r\n\t\t{\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"卡号\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"R_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"kickrole\";\r\n\t\t}\r\n\t\telse if(objSel.value==\"禁言\")\r\n\t\t{\r\n\t\t\tSetSubmitData();\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"乱发言\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"R_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"forbidsay\";\r\n\t\t}\r\n\t\telse if(objSel.value==\"冻结帐号\")\r\n\t\t{\r\n\t\t\tSetSubmitData();\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"违规\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"A_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"freeze\";\r\n\t\t}\r\n\t\telse if(objSel.value==\"解除禁言\")\r\n\t\t{\r\n\t\t\tSetSubmitData();\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"改正以前违规行为\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"R_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"permitsay\";\r\n\t\t}\r\n\t\telse if(objSel.value==\"解冻帐号\")\r\n\t\t{\r\n\t\t\tfrmSubmit.document.all(\"message\").value=\"改正以前违规行为\";\r\n\t\t\tfrmSubmit.document.all(\"category\").value=\"A_N_CTRL\";\r\n\t\t\tfrmSubmit.document.all(\"command\").value=\"unfreeze\";\r\n\t\t}\r\n\t\tfrmSubmit.document.all(\"frmSubmit\").submit();\r\n\t\tdocument.all(\"allquestion\").checked=false;\r\n\t}\r\n}\r\n\tfunction setQuestionType(iid){\r\n\t\tvar objSelTemp = document.all(\"selQuestionTypeTemp\");\t\t\r\n\t\tvar objOption;\r\n\t\tdocument.all(\"typeDiv\").innerHTML = \"<button name=all onclick=changeQuestionType('0','all')>全部</button>\";\r\n\t\t\r\n\t\tvar i=0;\r\n\t\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\t\tobjOption=objSelTemp.options[i];\r\n\t\t\tif(objOption.fid==0){\r\n\t\t\t\tdocument.all(\"typeDiv\").innerHTML += \"&nbsp;&nbsp;<button name=\"+objOption.value+\" onclick=changeQuestionType('\"+objOption.iid+\"','\"+objOption.value+\"')>\"+objOption.text+\"</button>\";\t\t\r\n\t\t\t\t\r\n\t\t\t}\t\t\r\n\t\t}\r\n\t\t\r\n\t\t\r\n\t}\r\n\tvar preClickType = \"\";\r\n\tfunction changeQuestionType(iid,type){\r\n\t\tdocument.getElementById(\"frmPlayerList\").src=\"");

            // end
            // begin [file="/gmc/integrate.jsp";from=(220,51);to=(220,75)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(220,77);to=(263,31)]
                out.write("/GMCServlet?action=playerlist&type=\"+type+\"&iid=\"+iid;\r\n\t\t//ReloadPlayerList();\r\n\t\r\n\t\twindow.open(document.all.frmPlayerList.src,'frmPlayerList','');\r\n\t\t\r\n\t\tdocument.all(\"subTypeTd\").style.display = \"none\";\r\n\t\tvar objSel=document.all(\"subType\");\r\n\t\tvar objSelTemp=document.all(\"selQuestionTypeTemp\");\r\n\t\tvar objOption;\r\n\t\tobjSel.length=0;\r\n\t\tvar i=0;\r\n\t\tvar channel = \"全部\";\r\n\t\tfrmSubmit.document.all(\"channel\").value=channel;\r\n\t\t\r\n\t\tif(preClickType!=\"\"){\r\n\t\t\t\tdocument.getElementById(preClickType).className =\"\";\r\n\t\t}\r\n\t\tdocument.getElementById(type).className =\"ClickBUTTON\";\r\n\t\tpreClickType = type;\t\r\n\t\t\t\t\t\r\n\t\tif(iid == 0){\r\n\t\t\treturn;\r\n\t\t}\r\n\t\tfor(i=0;i<objSelTemp.length;i++){\r\n\t\t\tobjOption=objSelTemp.options[i];\r\n\t\t\t\r\n\t\t\tif(objOption.iid==iid){\t\t\r\n\t\t\t\tchannel = objOption.text;\r\n\t\t\t\tfrmSubmit.document.all(\"channel\").value=channel;\r\n\t\t\t}\t\t\r\n\t\t\tif(objOption.fid==iid){\r\n\t\t\t\tobjSel.options[objSel.length]=new Option(objOption.text,objOption.value);\r\n\t\t\t}\t\t\r\n\t\t}\r\n\t\tif(objSel.length>0){\r\n\t\t\tdocument.all(\"subTypeTd\").style.display = \"\";\r\n\t\t}\r\n\t\t\r\n\t\t\r\n\t\t\r\n\t}\r\n</script>\r\n</head>\r\n<body onload=\"setQuestionType('");

            // end
            // begin [file="/gmc/integrate.jsp";from=(263,34);to=(263,82)]
                out.print(Util.setNullToNull(request.getParameter("iid")) );
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(263,84);to=(281,104)]
                out.write("')\">\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">服务功能</span> &gt;&gt; <span class=\"title\">GM频道处理（基于GCD新协议）</span></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n      <table width=\"100%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" class=\"tablelist\">\r\n        <tbody>\r\n          <tr align=\"center\" height=\"50\">\r\n            <td background=\"../images/tab_bg.gif\"><div id=\"typeDiv\"></div><font color=\"#FFFFFF\">玩家问题列表(问题总数:</font><font color=\"#FF0000\">\r\n              <label id=\"lblNumber\">0</label>\r\n            </font><font color=\"#FFFFFF\">,已处理:</font><font color=\"#FF0000\">\r\n            <label id=\"lblHaveAnswer\">0</label>\r\n            </font>)</td>\r\n          </tr>\r\n          <tr align=\"center\">\r\n            <td height=\"380\">\r\n              <iframe name=\"frmPlayerList\" width=\"100%\" height=\"100%\" frameborder=0 scrolling=\"no\" src=\"");

            // end
            // begin [file="/gmc/integrate.jsp";from=(281,107);to=(281,131)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(281,133);to=(294,33)]
                out.write("/GMCServlet?action=playerlist&type=all&iid=0\" marginwidth=\"0\" marginheight=\"0\">\r\n              对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe>\r\n\t\t\t</td>\r\n          </tr>\r\n          <tr align=\"center\">\r\n            <td background=\"../images/tab_bg.gif\"><font color=\"#FFFFFF\">问题详情</font></td>\r\n          </tr>\r\n          <tr>\r\n            <td valign=\"top\">\r\n              <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n                <tr>\r\n                  <td nowrap>玩家帐号<input type=\"text\" name=\"account\" value=\"\" size=\"15\"></td>\r\n                  <td nowrap>玩家角色<input type=\"text\" name=\"role\" value=\"\" size=\"15\"></td>\r\n                  <td nowrap>所属区服");

            // end
            // begin [file="/gmc/integrate.jsp";from=(294,33);to=(294,121)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_gateway_person");
                _jspx_th_display_display_0.setProperty("selGateway");
                _jspx_th_display_display_0.setParameter("");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/integrate.jsp";from=(294,121);to=(294,131)]
                            out.write("          ");

                        // end
                        // begin [file="/gmc/integrate.jsp";from=(294,131);to=(294,149)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(294,149);to=(298,62)]
                out.write("\r\n\t\t\t\t  <input type=\"hidden\" name=\"game_id\">\r\n                  <input type=\"hidden\" name=\"area\">\r\n                  <input type=\"hidden\" name=\"group\"></td>\r\n                  <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/integrate.jsp";from=(298,65);to=(298,87)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(298,89);to=(318,139)]
                out.write("\">\r\n                  <input type=\"hidden\" name=\"questionid\"></td>\r\n                  <td nowrap id=\"subTypeTd\" style=\"display:none\">类型<select name=\"subType\"></select></td>\r\n                  <td nowrap>提问日期<input type=\"text\" name=\"quiztime\" value=\"\" readonly=\"true\"></td>\r\n                </tr>\r\n                <tr>\r\n                  <td colspan=\"6\"><textarea name=\"question\" style=\"width:100%\" rows=\"2\" readonly onMouseOver=\"this.focus()\" ></textarea></td>\r\n                </tr>\r\n              </table>\r\n            </td>\r\n          </tr>\r\n          <tr>\r\n            <td align=\"center\" background=\"../images/tab_bg.gif\"><font color=\"#FFFFFF\">答复内容</font></td>\r\n          </tr>\r\n          <tr>\r\n            <td>\r\n              <textarea name=\"answer\" style=\"width:100%\" rows=\"2\" disabled onKeyPress=\"CheckEnter()\"></textarea>\r\n            </td>\r\n          </tr>\r\n          <tr>\r\n            <td nowrap>快速答复<SELECT name='selQuickanswer'  onclick='onQuickanswer()' style='width:90%'  onmouseover='this.focus()'></SELECT>");

            // end
            // begin [file="/gmc/integrate.jsp";from=(318,139);to=(318,219)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_1 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_1.setPageContext(pageContext);
                _jspx_th_display_display_1.setParent(null);
                _jspx_th_display_display_1.setDisplayCode("select_quickanswer");
                _jspx_th_display_display_1.setProperty("selQuickanswerTemp");
                try {
                    int _jspx_eval_display_display_1 = _jspx_th_display_display_1.doStartTag();
                    if (_jspx_eval_display_display_1 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_1 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/integrate.jsp";from=(318,219);to=(319,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/gmc/integrate.jsp";from=(319,12);to=(319,30)]
                        } while (_jspx_th_display_display_1.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_1.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_1.release();
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(319,30);to=(333,5)]
                out.write("\r\n\t\t\t</td>\r\n          </tr>\r\n          <tr>\r\n            <td align=\"right\" nowrap>\r\n              <table width=\"700\" border=\"0\" cellspacing=\"0\">\r\n                <tr align=\"center\">\r\n                  <td><input type=\"checkbox\" name=\"allquestion\">\r\n                  对此玩家所有问题操作</td>\r\n                  <td><button name=\"btnAnswer\" style=\"width:80\" onClick=\"onAnswer()\" disabled>答复问题</button></td>\r\n                  <td><button name=\"btnDelete\" style=\"width:80\"  onClick=\"onDelete()\" disabled>删除问题</button></td>\r\n                  <td><button name=\"btnFresh\" style=\"width:80\"  onClick=\"ReloadPlayerList()\">刷新问题列表</button><button name=\"btnanswerlog\" style=\"width:80\"  onClick=\"answerlog()\">答复记录</button></td>\r\n                  <td>GM操作<select name=\"selGMFunction\">\r\n\t\t\t\t\t<option value=\"\">=请选择=</option>\r\n\t\t\t\t\t");

            // end
            // begin [file="/gmc/integrate.jsp";from=(333,7);to=(333,63)]
                if(userinfo.getUserpower().indexOf("kickplayer") != -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(333,65);to=(333,97)]
                out.write(" <option value=\"踢号\">踢号</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(333,99);to=(333,100)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(333,102);to=(334,5)]
                out.write("\r\n\t\t\t\t\t");

            // end
            // begin [file="/gmc/integrate.jsp";from=(334,7);to=(334,63)]
                if(userinfo.getUserpower().indexOf("kickplayer") != -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(334,65);to=(334,97)]
                out.write(" <option value=\"踢人\">踢人</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(334,99);to=(334,100)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(334,102);to=(335,5)]
                out.write("\r\n\t\t\t\t\t");

            // end
            // begin [file="/gmc/integrate.jsp";from=(335,7);to=(335,63)]
                if(userinfo.getUserpower().indexOf("rolebehave") != -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(335,65);to=(335,97)]
                out.write(" <option value=\"禁言\">禁言</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(335,99);to=(335,100)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(335,102);to=(336,5)]
                out.write("\r\n\t\t\t\t\t");

            // end
            // begin [file="/gmc/integrate.jsp";from=(336,7);to=(336,65)]
                if(userinfo.getUserpower().indexOf("accountnctrl") != -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(336,67);to=(336,103)]
                out.write(" <option value=\"冻结帐号\">冻结帐号</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(336,105);to=(336,106)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(336,108);to=(337,6)]
                out.write("\r\n\t\t\t\t\t ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(337,8);to=(337,63)]
                if(userinfo.getUserpower().indexOf("rolebehave")!= -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(337,65);to=(337,101)]
                out.write(" <option value=\"解除禁言\">解除禁言</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(337,103);to=(337,104)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(337,106);to=(338,5)]
                out.write("\r\n\t\t\t\t\t");

            // end
            // begin [file="/gmc/integrate.jsp";from=(338,7);to=(338,65)]
                if(userinfo.getUserpower().indexOf("accountnctrl") != -1){
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(338,67);to=(338,103)]
                out.write(" <option value=\"解冻帐号\">解冻帐号</option> ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(338,105);to=(338,106)]
                }
            // end
            // HTML // begin [file="/gmc/integrate.jsp";from=(338,108);to=(358,18)]
                out.write("\r\n                  </select>期限<select name=\"term\">\r\n              <option value=\"0\" selected>无限</option>\r\n              <option value=\"1\">1</option>\r\n              <option value=\"2\">2</option>\r\n              <option value=\"3\">3</option>\r\n              <option value=\"4\">4</option>\r\n              <option value=\"5\">5</option>\r\n              <option value=\"6\">6</option>\r\n              <option value=\"7\">7</option>\r\n              <option value=\"10\">10</option>\r\n              <option value=\"15\">15</option>\r\n              <option value=\"20\">20</option>\r\n              <option value=\"25\">25</option>\r\n              <option value=\"30\">30</option>\r\n              <option value=\"35\">35</option>\r\n              <option value=\"40\">40</option>\r\n              <option value=\"50\">50</option>\r\n              <option value=\"60\">60</option>                  </select>天\r\n                  <button onclick=\"onGMFunction()\">确定</button></td>\r\n                  ");

            // end
            // begin [file="/gmc/integrate.jsp";from=(358,18);to=(358,125)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_2 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_2.setPageContext(pageContext);
                _jspx_th_display_display_2.setParent(null);
                _jspx_th_display_display_2.setDisplayCode("select_questiontype");
                _jspx_th_display_display_2.setProperty("selQuestionTypeTemp");
                _jspx_th_display_display_2.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_2 = _jspx_th_display_display_2.doStartTag();
                    if (_jspx_eval_display_display_2 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                // end
                // begin [file="/gmc/integrate.jsp";from=(358,18);to=(358,143)]
                if (_jspx_th_display_display_2.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                    return;
            } finally {
                _jspx_th_display_display_2.release();
            }
        // end
        // HTML // begin [file="/gmc/integrate.jsp";from=(358,143);to=(376,0)]
            out.write("\r\n                </tr>\r\n              </table>\r\n\t\t\t</td>\r\n          </tr>\r\n        </tbody>\r\n      </table>\r\n      <iframe name=\"frmSubmit\" width=\"100%\" height=\"0\" frameborder=0 scrolling=\"auto\" src=\"submit.jsp\" marginwidth=\"0\" marginheight=\"0\">\r\n      对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe>\r\n\r\n\t</td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
