package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class roleinfo$jsp extends HttpJspBase {

    // begin [file="/gmc/roleinfo.jsp";from=(5,0);to=(5,94)]
    // end

    static {
    }
    public roleinfo$jsp( ) {
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

            // HTML // begin [file="/gmc/roleinfo.jsp";from=(0,175);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(1,57);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(2,56);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(3,54);to=(4,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(4,68);to=(5,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(5,0);to=(5,94)]
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
            // begin [file="/gmc/roleinfo.jsp";from=(5,0);to=(5,94)]
                }
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(5,94);to=(6,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(6,0);to=(6,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("2roleinfo");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/roleinfo.jsp";from=(6,0);to=(6,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(6,43);to=(12,35)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>玩家角色信息</title>\r\n<link href=\"../css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(12,38);to=(12,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(12,64);to=(14,13)]
                out.write("/js/public.js\"></script>\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(14,16);to=(14,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(14,42);to=(16,0)]
                out.write("\";\r\n</script>\r\n");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(16,2);to=(37,0)]
                
                    UserInfo userinfo = (UserInfo) session.getAttribute("userinfo");
                    	String role="";
                    	String area="";
                    	String group="";
                    	String dblock="";
                    	String zonecode="";
                	  	String game_id="";
                	  	HashMap hs=(HashMap)request.getAttribute("searchinfo");
                	  	HashMap roleMap=(HashMap)request.getAttribute("roleinfo");
                		if(hs==null){
                            hs=new HashMap();
                			roleMap=new HashMap();
                        }else{
                			role=Util.setNullToEmpty((String)hs.get("role"));
                			area=Util.setNullToEmpty((String)hs.get("area"));
                			group=Util.setNullToEmpty((String)hs.get("group"));
                			dblock=Util.setNullToEmpty((String)hs.get("dblock"));
                			zonecode=Util.setNullToEmpty((String)hs.get("zonecode"));
                			game_id=Util.setNullToEmpty((String)hs.get("game_id"));
                		}
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(37,2);to=(39,13)]
                out.write("\r\n<script language=\"JavaScript\">\r\nvar game_id=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(39,16);to=(39,23)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(39,25);to=(40,12)]
                out.write("\";\r\nvar dblock=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(40,15);to=(40,21)]
                out.print(dblock);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(40,23);to=(41,14)]
                out.write("\";\r\nvar zonecode=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(41,17);to=(41,25)]
                out.print(zonecode);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(41,27);to=(95,13)]
                out.write("\";\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"role\")) strErr+=\"*玩家角色不能为空\\n\";\r\n\tif(IsEmpty(\"message\")) strErr+=\"*查询原因不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar objGateway=document.all(\"zonecode\");\r\n\t\tvar objOption=objGateway.options[objGateway.selectedIndex];\r\n\t\tdocument.all(\"area\").value=objOption.area;\r\n\t\tdocument.all(\"group\").value=objOption.group;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\tdocument.all(\"frmRoleinfo\").submit();\r\n\t}\r\n}\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"zonecode\");\r\n\tvar objSelTemp=document.all(\"selGatewayTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"game_id\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.value,objOption.value);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t}\t\t\r\n\t}\r\n}\r\n</script>\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">玩家角色信息（基于GCD新协议）</span> </td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\">\r\n<div id=\"divPrompt\" style=\"width:350px; position:absolute; display:none; left: 240px; top: 230px; height: 44px; z-index:1000\">\r\n<Iframe src=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(95,16);to=(95,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(95,42);to=(99,37)]
                out.write("/gmc/processPrompt.html\" id=\"test\" width=\"100%\" height=\"44px\" scrolling=\"no\" frameborder=\"0\"></iframe>\r\n</div>\r\n\t<br>\r\n\t<table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t  <form name=\"frmRoleinfo\" action=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(99,40);to=(99,64)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(99,66);to=(105,48)]
                out.write("/GMCServlet\" method=\"post\">\r\n\t\t  <input type=\"hidden\" name=\"action\" value=\"getroleinfo\">\r\n\t\t  <input type=\"hidden\" name=\"category\" value=\"ROL_INFO\">\r\n\t\t  <input type=\"hidden\" name=\"command\" value=\"queryblock\">\r\n\t\t  <input type=\"hidden\" name=\"area\">\r\n\t\t  <input type=\"hidden\" name=\"group\">\r\n\t\t  <input type=\"hidden\" name=\"usercode\" value=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(105,51);to=(105,73)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(105,75);to=(111,34)]
                out.write("\"/>\r\n        <tr>\r\n          <td align=\"center\">\r\n\t\t  <table width=\"100%\" border=\"0\" cellspacing=\"0\">\r\n\t\t    <tr>\r\n\t\t\t  <td width=\"13%\" align=\"right\">所属游戏：</td>\r\n\t\t\t  <td width=\"20%\" align=\"left\">");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(111,34);to=(111,145)]
                /* ----  game:gameselect ---- */
                com.kingsoft.gmsystem.web.taglib.GameTag _jspx_th_game_gameselect_0 = new com.kingsoft.gmsystem.web.taglib.GameTag();
                _jspx_th_game_gameselect_0.setPageContext(pageContext);
                _jspx_th_game_gameselect_0.setParent(null);
                _jspx_th_game_gameselect_0.setDisplayCode("select_game");
                _jspx_th_game_gameselect_0.setSelectName("game_id");
                _jspx_th_game_gameselect_0.setOnChange("onChangeGameId()");
                _jspx_th_game_gameselect_0.setStyle("width:100");
                try {
                    int _jspx_eval_game_gameselect_0 = _jspx_th_game_gameselect_0.doStartTag();
                    if (_jspx_eval_game_gameselect_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.GameTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_game_gameselect_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/roleinfo.jsp";from=(111,34);to=(111,145)]
                        } while (_jspx_th_game_gameselect_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_game_gameselect_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_game_gameselect_0.release();
                }
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(111,145);to=(114,5)]
                out.write("</td>\r\n\t\t\t  <td width=\"10%\" align=\"right\">所在区：</td>\r\n\t\t\t  <td width=\"20%\" align=\"left\"><select name=\"zonecode\"></select>\r\n\t\t\t  ");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(114,5);to=(114,102)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_gateway");
                _jspx_th_display_display_0.setProperty("selGatewayTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/gmc/roleinfo.jsp";from=(114,102);to=(115,5)]
                            out.write("\r\n\t\t\t  ");

                        // end
                        // begin [file="/gmc/roleinfo.jsp";from=(115,5);to=(115,23)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(115,23);to=(117,73)]
                out.write("</td>\r\n\t\t\t  <td width=\"10%\" align=\"right\"><font color=\"#FF0000\">*</font>玩家角色：</td>\r\n\t\t\t  <td align=\"left\"><input name=\"role\" type=\"text\" class=\"text\" value=\"");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(117,76);to=(117,80)]
                out.print(role);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(117,82);to=(122,34)]
                out.write("\" onmouseover=\"this.focus()\" size=\"20\" maxlength=\"50\" ></td></tr>\r\n\t\t    <tr>\r\n\t\t\t  <td align=\"right\"><font color=\"#FF0000\">*</font>查询原因：</td>\r\n\t\t\t  <td align=\"left\"><input name=\"message\" class=\"text\" size=\"20\" value=\"\"/></td>\r\n\t\t\t  <td align=\"right\">信息类别：</td>\r\n\t\t\t  <td colspan=\"2\" align=\"left\">");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(122,34);to=(122,115)]
                /* ----  roleBlock:type ---- */
                com.kingsoft.gmsystem.web.taglib.RoleBlockTypeTag _jspx_th_roleBlock_type_0 = new com.kingsoft.gmsystem.web.taglib.RoleBlockTypeTag();
                _jspx_th_roleBlock_type_0.setPageContext(pageContext);
                _jspx_th_roleBlock_type_0.setParent(null);
                _jspx_th_roleBlock_type_0.setDisplayCode("select_type");
                _jspx_th_roleBlock_type_0.setSelectName("dblock");
                _jspx_th_roleBlock_type_0.setStyle("width:160");
                try {
                    int _jspx_eval_roleBlock_type_0 = _jspx_th_roleBlock_type_0.doStartTag();
                    if (_jspx_eval_roleBlock_type_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.RoleBlockTypeTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_roleBlock_type_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/gmc/roleinfo.jsp";from=(122,34);to=(122,115)]
                        } while (_jspx_th_roleBlock_type_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_roleBlock_type_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_roleBlock_type_0.release();
                }
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(122,115);to=(136,43)]
                out.write("</td>\r\n\t\t\t  <td align=\"left\"><button name=\"btnSearch\" onClick=\"onSearch()\"> 查询 </button></td></tr>\r\n\t\t  </table>\r\n          </td>\r\n        </tr>\r\n\t\t</form>\r\n        <tr>\r\n          <td align=\"center\"><br>\r\n           <table width=\"715\" border=\"1\" cellpadding=\"0\" cellspacing=\"0\" class=\"tablelist\">\r\n             <tr align=\"left\" bgcolor=\"#419ADC\">\r\n               <td height=\"31\" colspan=\"6\" nowrap><font color=#ffffff><b>&nbsp;&#8226;&nbsp;玩家角色信息</b></font></td>\r\n             </tr>\r\n             <tr>\r\n               <td width=\"18%\" height=\"25\"  align=\"right\">全部返回值：</td>\r\n               <td height=\"25\" colspan=\"5\">");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(136,46);to=(136,95)]
                out.print(Util.setNullToEmpty((String)roleMap.get("所有返回值")));
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(136,97);to=(138,4)]
                out.write("</td>\r\n             </tr>\r\n\t\t\t ");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(138,6);to=(150,3)]
                
                			 roleMap.remove("所有返回值");
                        Iterator keyIterator = roleMap.keySet().iterator();
                        int index = 0;
                        String keyword = null;
                        String keyvalue = null;
                        while (keyIterator.hasNext()){
                            keyword = (String) keyIterator.next();
                            keyvalue = Util.setNullToEmpty((String)roleMap.get(keyword));
                            if (index % 3 == 0){
                                out.println("<tr>");
                            }
                			
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(150,5);to=(151,65)]
                out.write("\r\n               <td width=\"18%\" height=\"25\"  align=\"right\" nowrap>");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(151,68);to=(151,75)]
                out.print(keyword);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(151,77);to=(152,31)]
                out.write("：</td>\r\n               <td width=\"15%\">");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(152,34);to=(152,42)]
                out.print(keyvalue);
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(152,44);to=(153,3)]
                out.write("</td>\r\n\t\t\t");

            // end
            // begin [file="/gmc/roleinfo.jsp";from=(153,5);to=(165,4)]
                
                            if (index % 3 == 2){
                                out.println("</tr>");
                            }
                            index++;
                        }
                            if (index % 3 != 0){
                				int cols = 3-(index % 3);
                				out.println("<td colspan='" + cols + "'/>");
                                out.println("</tr>");
                            }
                
                			 
            // end
            // HTML // begin [file="/gmc/roleinfo.jsp";from=(165,6);to=(190,0)]
                out.write("\r\n           </table>\r\n           <br>\r\n\t\t  </td>\r\n        </tr>\r\n      </table>\r\n\t  <script>\r\n\t  if(game_id!=\"\"){\r\n\t\tSetSelValue(\"game_id\",game_id);\r\n\t\tSetSelValue(\"dblock\",dblock);\r\n        SetSelValue(\"zonecode\",zonecode);\r\n\t  }\r\n          onChangeGameId();\r\n\t</script>\r\n      <iframe name=\"frmSubmit\" width=\"100%\" height=\"0\" frameborder=0 scrolling=\"auto\" src=\"../service/submit.jsp\" marginwidth=\"0\" marginheight=\"0\">\r\n      对不起，你的浏览器不支持嵌入式框架。请升级到更高的版本。 </iframe>\r\n\r\n    </td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n</html>\r\n");

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
