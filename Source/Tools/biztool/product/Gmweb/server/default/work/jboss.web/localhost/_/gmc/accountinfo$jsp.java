package org.apache.jsp;

import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class accountinfo$jsp extends HttpJspBase {

    // begin [file="/gmc/accountinfo.jsp";from=(4,0);to=(4,102)]
    // end

    static {
    }
    public accountinfo$jsp( ) {
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

            // HTML // begin [file="/gmc/accountinfo.jsp";from=(0,163);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(4,0);to=(4,102)]
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
            // begin [file="/gmc/accountinfo.jsp";from=(4,0);to=(4,102)]
                }
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(4,102);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(5,0);to=(5,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("1accountinfo");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/accountinfo.jsp";from=(5,0);to=(5,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(5,46);to=(11,35)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>玩家帐号信息</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(11,38);to=(11,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(11,64);to=(13,0)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\" src=\"../js/public.js\"></script>\r\n");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(13,2);to=(23,0)]
                
                    UserInfo userinfo = (UserInfo) request.getSession().getAttribute("userinfo");
                    AccountInfo accinfo=new AccountInfo();
                    String account=new String(Util.setNullToEmpty(request.getParameter("account")).getBytes("ISO-8859-1"));//要查询的
                    String game_code=request.getParameter("game_code");//所属游戏专区
                    String zonecode=request.getParameter("zonecode");//所属游戏专区
                	String rolelist=request.getParameter("rolelist");//是否现实角色列表
                    if(account!=null && !account.equals("")){
                      accinfo=SystemHelperId.getGMCAccountInfo(game_code,zonecode,account,userinfo.getUsercode(),rolelist,userinfo);
                      }
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(23,2);to=(25,13)]
                out.write("\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(25,16);to=(25,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(25,42);to=(26,15)]
                out.write("\";\r\nvar game_code=\"");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(26,18);to=(26,27)]
                out.print(game_code);
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(26,29);to=(27,14)]
                out.write("\";\r\nvar zonecode=\"");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(27,17);to=(27,25)]
                out.print(zonecode);
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(27,27);to=(99,34)]
                out.write("\";\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"account\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar account=document.all(\"account\").value;\r\n\t\tvar rolelist=\"0\";\r\n\t\tvar rolelistcheck=document.all(\"roleListcheck\");\r\n\t\tif (rolelistcheck.checked){\r\n\t\t\trolelist=\"1\";\r\n\t\t}\r\n\t\tvar strUrl=AbsPath+\"/gmc/accountinfo.jsp?account=\"+account+\"&game_code=\"+document.all(\"game_code\").value+\"&zonecode=\"+document.all(\"zonecode\").value+\"&rolelist=\"+rolelist;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\t\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_code\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t}\t\t\r\n\t}\r\n}\r\n\r\nfunction toProcessResult(sn)\r\n{\r\n\tstrUrl=AbsPath+\"/gmc/processresult.jsp?seltype=sn&sn=\"+sn;\r\n\twindow.navigate(strUrl);\r\n}\r\n\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">玩家帐号信息（基于GCD新协议）</span> </td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\r\n\t<br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n        <tr>\r\n          <td align=\"center\">\r\n\t\t  <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t    <tr>\r\n\t\t\t  <td width=\"10%\" align=\"right\">所属游戏：</td>\r\n\t\t\t  <td width=\"12%\" align=\"left\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(99,34);to=(99,147)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_code");
                _jspx_th_game_gameselect_0.setOnChange("onChangeGameId()");
                _jspx_th_game_gameselect_0.setStyle("width:100");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/accountinfo.jsp";from=(99,34);to=(99,147)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(99,147);to=(103,8)]
                out.write("</td>\r\n\t\t\t  <td width=\"10%\" align=\"right\">所在区：</td>\r\n\t\t\t  <td width=\"18%\" align=\"left\">\r\n\t\t\t\t\t\t\t\t<select name=\"zonecode\"></select>\r\n\t\t\t\t\t\t\t\t");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(103,8);to=(104,58)]
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
                        // HTML // begin [file="/gmc/accountinfo.jsp";from=(104,58);to=(105,8)]
                            out.write("\r\n\t\t\t\t\t\t\t\t");

                        // end
                        // begin [file="/gmc/accountinfo.jsp";from=(105,8);to=(105,26)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(105,26);to=(108,88)]
                out.write("\r\n\t\t\t  </td>\r\n\t\t\t  <td width=\"10%\" align=\"right\"><font color=\"#FF0000\">*</font>玩家帐号：</td>\r\n\t\t\t  <td width=\"12%\" align=\"left\"><input name=\"account\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(108,91);to=(108,98)]
                out.print(account);
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(108,100);to=(109,116)]
                out.write("\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"30\" ></td>\r\n\t\t\t  <td width=\"15%\" align=\"right\"><font color=\"#FF0000\">*</font>查询角色列表<input type=\"checkbox\" name=\"roleListcheck\"  ");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(109,118);to=(109,164)]
                if("1".equals(rolelist)) out.print("checked");
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(109,166);to=(122,31)]
                out.write("></td>\r\n\t\t\t  <td align=\"left\"><button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td></tr>\r\n\t\t  </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <table width=\"773\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"31\" colspan=\"6\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;玩家帐号基本信息</b></font></td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"15%\" height=\"25\"  align=\"right\" nowrap>所属游戏：</td>\r\n               <td width=\"17%\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(122,34);to=(122,55)]
                out.print(accinfo.getGameName());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(122,57);to=(124,31)]
                out.write("&nbsp;</td>\r\n               <td width=\"18%\" height=\"25\"  align=\"right\" nowrap>玩家帐号：</td>\r\n               <td width=\"17%\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(124,34);to=(124,54)]
                out.print(accinfo.getAccName());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(124,56);to=(130,19)]
                out.write("&nbsp;</td>\r\n               <td width=\"18%\" height=\"25\"  align=\"right\" nowrap></td>\r\n               <td width=\"15%\"></td>\r\n              </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">玩家姓名：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(130,22);to=(130,43)]
                out.print(accinfo.getRealName());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(130,45);to=(132,19)]
                out.write("</td>\r\n               <td align=\"right\">玩家性别：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(132,22);to=(132,38)]
                out.print(accinfo.getSex());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(132,40);to=(134,31)]
                out.write("</td>\r\n               <td width=\"15%\" align=\"right\">金山通行证：</td>\r\n               <td width=\"18%\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(134,34);to=(134,55)]
                out.print(accinfo.getPassport());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(134,57);to=(138,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\" align=\"right\">玩家生日：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(138,22);to=(138,43)]
                out.print(accinfo.getBirthday());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(138,45);to=(140,19)]
                out.write("</td>\r\n               <td align=\"right\">身份证号：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(140,22);to=(140,40)]
                out.print(accinfo.getIDNum());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(140,42);to=(142,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">申请密码保护：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(142,22);to=(142,45)]
                out.print(accinfo.getPwdProtect());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(142,47);to=(146,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">手机：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(146,22);to=(146,44)]
                out.print(accinfo.getHandPhone());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(146,46);to=(148,19)]
                out.write("</td>\r\n               <td  align=\"right\">电话：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(148,22);to=(148,40)]
                out.print(accinfo.getPhone());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(148,42);to=(150,19)]
                out.write("</td>\r\n               <td  align=\"right\">Email：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(150,22);to=(150,40)]
                out.print(accinfo.getEmail());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(150,42);to=(154,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">上次登入时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(154,22);to=(154,44)]
                out.print(accinfo.getLoginTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(154,46);to=(156,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">首次登入时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(156,22);to=(156,49)]
                out.print(accinfo.getFirstLoginTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(156,51);to=(158,19)]
                out.write("</td>\r\n               <td  align=\"right\">提示问题：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(158,22);to=(158,43)]
                out.print(accinfo.getQuestion());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(158,45);to=(162,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">上次登入IP：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(162,22);to=(162,42)]
                out.print(accinfo.getLoginIP());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(162,44);to=(164,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">首次登录IP：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(164,22);to=(164,47)]
                out.print(accinfo.getFirstLoginIP());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(164,49);to=(166,19)]
                out.write("</td>\r\n               <td  align=\"right\">答案：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(166,22);to=(166,41)]
                out.print(accinfo.getAnswer());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(166,43);to=(170,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">上次退出时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(170,22);to=(170,45)]
                out.print(accinfo.getLogoutTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(170,47);to=(172,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">帐号注册时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(172,22);to=(172,42)]
                out.print(accinfo.getRegTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(172,44);to=(174,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">帐号注册IP：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(174,22);to=(174,40)]
                out.print(accinfo.getRegIP());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(174,42);to=(178,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\" nowrap>帐号状态：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(178,22);to=(178,43)]
                out.print(accinfo.getAccState());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(178,45);to=(180,43)]
                out.write("</td>\r\n               <td height=\"25\" align=\"right\">玩家地址：</td>\r\n               <td height=\"25\" colspan=\"3\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(180,46);to=(180,66)]
                out.print(accinfo.getAddress());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(180,68);to=(187,19)]
                out.write("</td>\r\n              </tr>\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"30\"  colspan=\"6\"><font color=#ffffff><b>&nbsp;&#8226;&nbsp;游戏时间信息</b></font></td>\r\n              </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">剩余点数：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(187,22);to=(187,45)]
                out.print(accinfo.getLeftSecond());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(187,47);to=(189,19)]
                out.write("</td>\r\n               <td align=\"right\">包时结束时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(189,22);to=(189,42)]
                out.print(accinfo.getEndTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(189,44);to=(195,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\"></td>\r\n               <td></td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">免费剩余点数：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(195,22);to=(195,49)]
                out.print(accinfo.getFreeLeftSecond());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(195,51);to=(197,19)]
                out.write("</td>\r\n               <td align=\"right\">免费包时结束时间：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(197,22);to=(197,46)]
                out.print(accinfo.getFreeEndTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(197,48);to=(205,19)]
                out.write("</td>\r\n               <td align=\"right\"></td>\r\n               <td></td>\r\n             </tr>\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"30\"  colspan=\"6\"><font color=#ffffff><b>&nbsp;&#8226;&nbsp;未领取物品信息</b></font></td>\r\n              </tr><tr>\r\n               <td height=\"25\"  align=\"right\">扩展点：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(205,22);to=(205,44)]
                out.print(accinfo.getNExtpoint());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(205,46);to=(207,19)]
                out.write("</td>\r\n               <td align=\"right\">扩展点1：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(207,22);to=(207,45)]
                out.print(accinfo.getNExtpoint1());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(207,47);to=(209,19)]
                out.write("</td>\r\n               <td height=\"25\"  align=\"right\">扩展点2：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(209,22);to=(209,45)]
                out.print(accinfo.getNExtpoint2());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(209,47);to=(215,19)]
                out.write("</td>\r\n              </tr>\r\n             <tr>\r\n               <td align=\"right\">扩展点3：</td>\r\n               <td>&nbsp;</td>\r\n               <td height=\"25\"  align=\"right\">扩展点4：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(215,22);to=(215,45)]
                out.print(accinfo.getNExtpoint4());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(215,47);to=(217,19)]
                out.write("</td>\r\n               <td align=\"right\">扩展点5：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(217,22);to=(217,45)]
                out.print(accinfo.getNExtpoint5());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(217,47);to=(221,19)]
                out.write("</td>\r\n             </tr>\r\n             <tr>\r\n               <td height=\"25\"  align=\"right\">扩展点6：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(221,22);to=(221,45)]
                out.print(accinfo.getNExtpoint6());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(221,47);to=(223,19)]
                out.write("</td>\r\n               <td align=\"right\">扩展点7：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(223,22);to=(223,45)]
                out.print(accinfo.getNExtpoint7());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(223,47);to=(225,19)]
                out.write("</td>\r\n               <td align=\"right\">剩余金币：</td>\r\n               <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(225,22);to=(225,44)]
                out.print(accinfo.getLeftMoney());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(225,46);to=(240,3)]
                out.write("</td>\r\n             </tr>\r\n\t\t\t <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"30\"  colspan=\"6\"><font color=#ffffff><b>&nbsp;&#8226;&nbsp;帐号相关角色信息</b></font></td>\r\n              </tr>\r\n             <tr>\r\n               <td height=\"25\" colspan=\"6\">\r\n                 <table width=\"100%\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n                   <tr align=\"center\">\r\n                     <td height=\"25\">所在区</td>\r\n                     <td>角色名</td>\r\n                     <td>创建时间</td>\r\n                     <td>目前等级</td>\r\n                     <td>当前状态</td>\r\n                   </tr>\r\n\t\t ");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(240,5);to=(244,19)]
                
                                 if(accinfo.getRoles()!=null){
                                  for(int i=0;i<accinfo.getRoles().size();i++){
                                   RoleInfo roleinfo=(RoleInfo)accinfo.getRoles().get(i);
                                   
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(244,21);to=(246,37)]
                out.write("\r\n                   <tr align=\"center\">\r\n                     <td height=\"25\">");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(246,40);to=(246,65)]
                out.print(roleinfo.getGatewayName());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(246,67);to=(247,25)]
                out.write("</td>\r\n                     <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(247,28);to=(247,50)]
                out.print(roleinfo.getRoleName());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(247,52);to=(248,25)]
                out.write("</td>\r\n                     <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(248,28);to=(248,52)]
                out.print(roleinfo.getCreateTime());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(248,54);to=(249,25)]
                out.write("</td>\r\n                     <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(249,28);to=(249,47)]
                out.print(roleinfo.getLevel());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(249,49);to=(250,25)]
                out.write("</td>\r\n                     <td>");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(250,28);to=(250,47)]
                out.print(roleinfo.getState());
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(250,49);to=(252,18)]
                out.write("</td>\r\n                   </tr>\r\n                  ");

            // end
            // begin [file="/gmc/accountinfo.jsp";from=(252,20);to=(252,22)]
                }}
            // end
            // HTML // begin [file="/gmc/accountinfo.jsp";from=(252,24);to=(288,0)]
                out.write("\r\n                 </table>\r\n               </td>\r\n              </tr>\r\n           </table>\r\n\t\t   <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td>\r\n                 <button onclick=\"history.back()\">返回</button>\r\n               </td>\r\n               <td>\r\n                 <button onclick=\"window.print()\">打印</button>\r\n               </td>\r\n             </tr>\r\n           </table>\r\n          </td>\r\n        </tr>\r\n      </table>\r\n\t  <script>\r\n\t  \r\n\t  onChangeGameId();\r\n\t  if(game_code!=null){\r\n\t  \r\n\t\tSetSelValue(\"game_code\",game_code);\r\n\t\tSetSelValue(\"zonecode\",zonecode);\r\n\t  }\r\n\t  \r\n\t</script>\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
