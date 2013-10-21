package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class itempurchase$jsp extends HttpJspBase {

    // begin [file="/search/itempurchase.jsp";from=(1,0);to=(1,102)]
    // end

    static {
    }
    public itempurchase$jsp( ) {
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

            // HTML // begin [file="/search/itempurchase.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/itempurchase.jsp";from=(1,0);to=(1,102)]
                com.kingsoft.gmsystem.service.SearchHelper SearchHelperId = null;
                boolean _jspx_specialSearchHelperId  = false;
                 synchronized (session) {
                    SearchHelperId= (com.kingsoft.gmsystem.service.SearchHelper)
                    pageContext.getAttribute("SearchHelperId",PageContext.SESSION_SCOPE);
                    if ( SearchHelperId == null ) {
                        _jspx_specialSearchHelperId = true;
                        try {
                            SearchHelperId = (com.kingsoft.gmsystem.service.SearchHelper) java.beans.Beans.instantiate(this.getClass().getClassLoader(), "com.kingsoft.gmsystem.service.SearchHelper");
                        } catch (ClassNotFoundException exc) {
                             throw new InstantiationException(exc.getMessage());
                        } catch (Exception exc) {
                             throw new ServletException (" Cannot create bean of class "+"com.kingsoft.gmsystem.service.SearchHelper", exc);
                        }
                        pageContext.setAttribute("SearchHelperId", SearchHelperId, PageContext.SESSION_SCOPE);
                    }
                 } 
                if(_jspx_specialSearchHelperId == true) {
            // end
            // begin [file="/search/itempurchase.jsp";from=(1,0);to=(1,102)]
                }
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(1,102);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/itempurchase.jsp";from=(4,0);to=(4,46)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("itempurchase");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/itempurchase.jsp";from=(4,0);to=(4,46)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(4,46);to=(11,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>物品购买记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(11,15);to=(11,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(11,41);to=(13,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(13,42);to=(15,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(15,38);to=(15,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(15,64);to=(17,0)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/itempurchase.jsp";from=(17,2);to=(22,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
                   String channel = Util.setNullToEmpty((String)hs1.get("channel"));
                   System.out.println("--------------channel=" + channel);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(22,2);to=(104,125)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\n\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"caccname\")&&IsEmpty(\"itemId\")) strErr+=\"*玩家帐号/物品编号不能都为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar ddatetime=document.all(\"ddatetime\").value;\r\n\t\tvar account=document.all(\"caccname\").value;\r\n\t\tvar itemId=document.all(\"itemId\").value;\r\n        var zonecode2=document.all(\"zonecode2\").value;\r\n        var channel=document.all(\"cchannel\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/itempurchase.jsp?ddatetime=\"+ddatetime+\r\n\t\t\t\t\t\t\t\t\"&caccname=\"+account+\"&channel=\"+channel+\"&zonecode2=\"+zonecode2+\r\n\t\t\t\t\t\t\t\t\"&itemId=\"+itemId;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\nfunction onChangeGameId()\r\n{\r\n\tvar objSel=document.all(\"zonecode2\");\r\n\tvar objSelTemp=document.all(\"zonecodeTemp\");\r\n\tvar objOption;\r\n\tobjSel.length=0;\r\n\tvar j=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id==document.all(\"cchannel\").value)\r\n\t\t{\r\n\t\t \tj+=1;\r\n\t\t\tobjSel.options[objSel.length]=new Option(j+\".\"+objOption.text,objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"game_id\",objOption.game_id);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"area\",objOption.area);\r\n\t\t\tobjSel.options[objSel.length-1].setAttribute(\"group\",objOption.group);\r\n\t\t}\t\t\r\n\t}\r\n\tif(document.all(\"cchannel\").value==\"15\"||document.all(\"cchannel\").value==\"22\"||document.all(\"cchannel\").value==\"66\" )\r\n\t{\r\n\t\tobjSel.disabled=false;\r\n\t}\r\n\telse\r\n\t\tobjSel.disabled=true;\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">物品购买记录</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\r\n\t<br>\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td  nowrap><LABEL id=\"lblcAccName\">玩家帐号</LABEL>\r\n                  <input name=\"caccname\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"30\" value=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(104,128);to=(104,176)]
                out.print(Util.setNullToEmpty((String)hs1.get("caccname")));
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(104,178);to=(107,39)]
                out.write("\">&nbsp;&nbsp;\r\n                  游戏类型\r\n                  <SELECT name=\"cchannel\" onchange=\"onChangeGameId()\">\r\n                    <OPTION value=\"21\" ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(107,41);to=(107,89)]
                if (channel.equals("21")) out.print("selected");
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(107,91);to=(108,40)]
                out.write(">1.剑侠IB</OPTION>\r\n                    <OPTION value=\"211\" ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(108,42);to=(108,91)]
                if (channel.equals("211")) out.print("selected");
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(108,93);to=(109,39)]
                out.write(">2.剑侠IB联合</OPTION>\r\n                    <OPTION value=\"15\" ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(109,41);to=(109,89)]
                if (channel.equals("15")) out.print("selected");
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(109,91);to=(110,39)]
                out.write(">3.水浒</OPTION>\r\n                    <OPTION value=\"22\" ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(110,41);to=(110,89)]
                if (channel.equals("22")) out.print("selected");
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(110,91);to=(111,39)]
                out.write(">4.封神榜IB</OPTION>\r\n                    <OPTION value=\"66\" ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(111,41);to=(111,89)]
                if (channel.equals("66")) out.print("selected");
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(111,91);to=(114,12)]
                out.write(">5.剑网2IB</OPTION>\r\n                  </SELECT>&nbsp;&nbsp;\r\n\t\t\t\t  所在区<select name=\"zonecode2\"></select>\r\n            ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(114,12);to=(114,110)]
                /* ----  display:display ---- */
                com.kingsoft.gmsystem.web.taglib.UserTag _jspx_th_display_display_0 = new com.kingsoft.gmsystem.web.taglib.UserTag();
                _jspx_th_display_display_0.setPageContext(pageContext);
                _jspx_th_display_display_0.setParent(null);
                _jspx_th_display_display_0.setDisplayCode("select_region_all");
                _jspx_th_display_display_0.setProperty("zonecodeTemp");
                _jspx_th_display_display_0.setParameter("display:none");
                try {
                    int _jspx_eval_display_display_0 = _jspx_th_display_display_0.doStartTag();
                    if (_jspx_eval_display_display_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.UserTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_display_display_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // HTML // begin [file="/search/itempurchase.jsp";from=(114,110);to=(115,12)]
                            out.write("\r\n            ");

                        // end
                        // begin [file="/search/itempurchase.jsp";from=(115,12);to=(115,30)]
                        } while (_jspx_th_display_display_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_display_display_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_display_display_0.release();
                }
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(115,30);to=(117,123)]
                out.write("<br>\r\n\t\t\t\t  物品编号\r\n                  <input name=\"itemId\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"50\" value=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(117,126);to=(117,172)]
                out.print(Util.setNullToEmpty((String)hs1.get("itemId")));
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(117,174);to=(119,108)]
                out.write("\">&nbsp;&nbsp;\r\n                  查询期间\r\n                  <input name=\"ddatetime\" type=\"text\" size=\"21\" readonly maxlength=\"21\" class=\"text\" value=\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(119,111);to=(119,160)]
                out.print(Util.setNullToEmpty((String)hs1.get("ddatetime")));
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(119,162);to=(144,16)]
                out.write("\">&nbsp;&nbsp;\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('ddatetime')\" style=\"cursor:hand \">\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品编号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品名称</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品类型</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品使用方式</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品所在区服</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品价格</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家账号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">购买时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品过期时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">物品消耗时间</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(144,18);to=(180,8)]
                
                				int viewList = 0;
                   String strPageAction="";
                   ArrayList lt=new ArrayList();
                   //ArrayList lt1 = new ArrayList();
                   //HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                	 String ItemGUID="";//充值时间
                	 String ItemID="";//充值时间
                	String ItemType="";//充值渠道
                	String UseType="";//玩家帐号
                	String ClientID="";//Passport
                	String Price="";//一卡通卡号
                	String PurchaseAccount="";//充值IP
                	String PurchaseTime="";//卡类型
                	String OverdueTime="";//卡数量
                	String ConsumeTime="";
                    HashMap hs = new HashMap();
                
                   if (hs1.containsKey("caccname")) {
                       viewList = 1;
                       lt = SearchHelperId.getIbItemList(hs1);
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                		 hs = (HashMap)lt.get(i);
                		 ItemGUID = Util.setNullToEmpty((String)hs.get("itemguid"));
                		 ItemID = Util.setNullToEmpty((String)hs.get("itemid"));
                		 ItemType = Util.setNullToEmpty((String)hs.get("itemtype"));
                		 UseType = Util.setNullToEmpty((String)hs.get("usetype"));
                		 ClientID = Util.setNullToEmpty((String)hs.get("clientid"));
                		 Price = Util.setNullToEmpty((String)hs.get("price"));
                		 PurchaseAccount = Util.setNullToEmpty((String)hs.get("purchaseaccount"));
                		 PurchaseTime = Util.setNullToEmpty((String)hs.get("purchasetime"));
                		 OverdueTime = Util.setNullToEmpty((String)hs.get("overduetime"));
                		 ConsumeTime = Util.setNullToEmpty((String)hs.get("consumetime"));
                        
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(180,10);to=(181,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(181,26);to=(181,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(181,39);to=(181,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/itempurchase.jsp";from=(181,59);to=(181,60)]
                }
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(181,62);to=(182,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(182,51);to=(182,59)]
                out.print(ItemGUID);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(182,61);to=(183,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(183,51);to=(183,57)]
                out.print(ItemID);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(183,59);to=(184,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(184,51);to=(184,59)]
                out.print(ItemType);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(184,61);to=(185,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(185,51);to=(185,58)]
                out.print(UseType);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(185,60);to=(186,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(186,51);to=(186,59)]
                out.print(ClientID);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(186,61);to=(187,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(187,51);to=(187,56)]
                out.print(Price);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(187,58);to=(188,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(188,51);to=(188,66)]
                out.print(PurchaseAccount);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(188,68);to=(189,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(189,51);to=(189,63)]
                out.print(PurchaseTime);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(189,65);to=(190,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(190,51);to=(190,62)]
                out.print(OverdueTime);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(190,64);to=(191,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/itempurchase.jsp";from=(191,51);to=(191,62)]
                out.print(ConsumeTime);
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(191,64);to=(193,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(193,22);to=(195,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(195,6);to=(207,6)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(207,8);to=(207,40)]
                if(lt.size()==0 && viewList==1){
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(207,42);to=(211,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/itempurchase.jsp";from=(211,8);to=(211,9)]
                }
            // end
            // HTML // begin [file="/search/itempurchase.jsp";from=(211,11);to=(225,0)]
                out.write("\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n\t  <script>\r\n\t    var objArea2=document.all(\"zonecode2\");\r\n\t\tobjArea2.disabled=true;\r\n          onChangeGameId();\r\n\t</script>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
