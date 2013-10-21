package org.apache.jsp;

import java.sql.*;
import java.util.*;
import com.kingsoft.gmsystem.common.Util;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class fillgame$jsp extends HttpJspBase {

    // begin [file="/search/fillgame.jsp";from=(1,0);to=(1,102)]
    // end

    static {
    }
    public fillgame$jsp( ) {
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

            // HTML // begin [file="/search/fillgame.jsp";from=(0,161);to=(1,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/fillgame.jsp";from=(1,0);to=(1,102)]
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
            // begin [file="/search/fillgame.jsp";from=(1,0);to=(1,102)]
                }
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(1,102);to=(2,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(2,57);to=(3,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(3,56);to=(4,0)]
                out.write("\r\n");

            // end
            // begin [file="/search/fillgame.jsp";from=(4,0);to=(4,43)]
                /* ----  power:checkpower ---- */
                com.kingsoft.gmsystem.web.taglib.PowerTag _jspx_th_power_checkpower_0 = new com.kingsoft.gmsystem.web.taglib.PowerTag();
                _jspx_th_power_checkpower_0.setPageContext(pageContext);
                _jspx_th_power_checkpower_0.setParent(null);
                _jspx_th_power_checkpower_0.setModulecode("fillagame");
                try {
                    int _jspx_eval_power_checkpower_0 = _jspx_th_power_checkpower_0.doStartTag();
                    if (_jspx_eval_power_checkpower_0 == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_BUFFERED)
                        throw new JspTagException("Since tag handler class com.kingsoft.gmsystem.web.taglib.PowerTag does not implement BodyTag, it can't return BodyTag.EVAL_BODY_TAG");
                    if (_jspx_eval_power_checkpower_0 != javax.servlet.jsp.tagext.Tag.SKIP_BODY) {
                        do {
                        // end
                        // begin [file="/search/fillgame.jsp";from=(4,0);to=(4,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(4,43);to=(11,12)]
                out.write("\r\n<html><!-- InstanceBegin template=\"/Templates/normal.dwt\" codeOutsideHTMLIsLocked=\"false\" -->\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<!-- InstanceBeginEditable name=\"doctitle\" -->\r\n<title>充值游戏记录</title>\r\n<!-- InstanceEndEditable -->\r\n<link href=\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(11,15);to=(11,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(11,41);to=(13,13)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(13,16);to=(13,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(13,42);to=(15,35)]
                out.write("\";\r\n</script>\r\n<script language=\"JavaScript\" src=\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(15,38);to=(15,62)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(15,64);to=(17,0)]
                out.write("/js/public.js\"></script>\r\n<!-- InstanceBeginEditable name=\"head\" -->\r\n");

            // end
            // begin [file="/search/fillgame.jsp";from=(17,2);to=(21,0)]
                
                   ArrayList lt1 = new ArrayList();
                   HashMap hs1 = Util.parseUploadData(request, lt1);
                   String channel = Util.setNullToEmpty((String)hs1.get("channel"));
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(21,2);to=(73,125)]
                out.write("\r\n<script language=\"JavaScript\" type=\"text/JavaScript\">\r\n<!--\r\n/********************************\r\n开始查询\r\n********************************/\r\n\r\nfunction onSearch()\r\n{\r\n\tvar strErr=\"\";\r\n\tif(IsEmpty(\"caccname\")) strErr+=\"*玩家帐号不能为空\\n\";\r\n\tif(IsEmpty(\"ddatetime\")) strErr+=\"*查询期间不能为空\\n\";\r\n\tif(strErr!=\"\")\r\n\t{\r\n\t\talert(strErr);\r\n\t}\r\n\telse\r\n\t{\r\n\t\tvar ddatetime=document.all(\"ddatetime\").value;\r\n\t\tvar account=document.all(\"caccname\").value;\r\n        var channel=document.all(\"cchannel\").value;\r\n\t\tvar strUrl=AbsPath+\"/search/fillgame.jsp?ddatetime=\"+ddatetime+\r\n\t\t\t\t\t\t\t\t\"&caccname=\"+account+\"&channel=\"+channel;\r\n\t\tdocument.all(\"divPrompt\").style.display=\"\";\r\n\t\twindow.navigate(strUrl);\r\n\t}\r\n}\r\n\r\n\r\n//-->\r\n</script>\r\n<!-- InstanceEndEditable -->\r\n</head>\r\n<body>\r\n<table width=\"100%\" height=\"100%\" border=\"0\" align=\"center\" cellspacing=\"0\">\r\n  <tr>\r\n    <td height=\"20\" bgcolor=\"#BBDDFD\">&nbsp;&nbsp;当前位置：<!-- InstanceBeginEditable name=\"position\" --><span class=\"title\">查询功能</span> &gt;&gt; <span class=\"title\">游戏帐号充值记录</span> <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td align=\"center\" valign=\"top\"><!-- InstanceBeginEditable name=\"main\" -->\r\n<div id=\"divPrompt\" style=\"width:250px; position:absolute; display:none; left: 340px; top: 300px; height: 44px;\">\r\n<table width=\"100%\" height=\"100%\" border=\"1\" align=\"center\" cellspacing=\"0\" bordercolor=\"#0000FF\" bgcolor=\"#FFFFFF\">\r\n  <tr>\r\n    <td align=\"center\" valign=\"middle\">正在查询和等待返回结果，请稍候......</td>\r\n  </tr>\r\n</table></div>\r\n\t<br>\r\n      <table width=\"98%\" border=\"0\" cellspacing=\"0\" >\r\n        <tr>\r\n          <td align=\"center\" valign=\"bottom\" nowrap><table width=\"100%\"  border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\r\n              <tr>\r\n                <td  nowrap><LABEL id=\"lblcAccName\">玩家帐号</LABEL>\r\n                  <input name=\"caccname\" type=\"text\" class=\"text\" onmouseover=\"this.focus()\" size=\"15\" maxlength=\"30\" value=\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(73,128);to=(73,176)]
                out.print(Util.setNullToEmpty((String)hs1.get("caccname")));
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(73,178);to=(76,48)]
                out.write("\">&nbsp;&nbsp;\r\n                  充值渠道\r\n                  <SELECT name=\"cchannel\">\r\n                    <OPTION value=\"fillgame2jx\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(76,50);to=(76,107)]
                if (channel.equals("fillgame2jx")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(76,109);to=(77,48)]
                out.write(">2.剑网充值</OPTION>\r\n                    <OPTION value=\"fillgame2fs\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(77,50);to=(77,107)]
                if (channel.equals("fillgame2fs")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(77,109);to=(78,49)]
                out.write(">3.封神充值</OPTION>\r\n                    <OPTION value=\"fillgame2jx2\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(78,51);to=(78,109)]
                if (channel.equals("fillgame2jx2")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(78,111);to=(79,49)]
                out.write(">4.剑网2充值</OPTION>\r\n                    <OPTION value=\"fillgame2xl2\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(79,51);to=(79,109)]
                if (channel.equals("fillgame2xl2")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(79,111);to=(80,50)]
                out.write(">5.仙侣2充值</OPTION>\r\n                    <OPTION value=\"fillgame2jxib\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(80,52);to=(80,111)]
                if (channel.equals("fillgame2jxib")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(80,113);to=(81,52)]
                out.write(">6.剑侠IB充值</OPTION>\r\n                    <OPTION value=\"fillgame2jxiblh\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(81,54);to=(81,115)]
                if (channel.equals("fillgame2jxiblh")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(81,117);to=(82,50)]
                out.write(">7.剑侠IB联合充值</OPTION>\r\n                    <OPTION value=\"fillgame2shib\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(82,52);to=(82,111)]
                if (channel.equals("fillgame2shib")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(82,113);to=(83,50)]
                out.write(">8.水浒IB充值</OPTION>\r\n                    <OPTION value=\"fillgame2fsib\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(83,52);to=(83,111)]
                if (channel.equals("fillgame2fsib")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(83,113);to=(84,51)]
                out.write(">9.封神IB充值</OPTION>\r\n                    <OPTION value=\"fillgame2jx2ib\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(84,53);to=(84,113)]
                if (channel.equals("fillgame2jx2ib")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(84,115);to=(85,48)]
                out.write(">10.剑网2IB充值</OPTION>\r\n                    <OPTION value=\"fillgame2cq\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(85,50);to=(85,107)]
                if (channel.equals("fillgame2cq")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(85,109);to=(86,50)]
                out.write(">11.春秋Q传充值</OPTION>\r\n                    <OPTION value=\"fillgame2jxsj\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(86,52);to=(86,111)]
                if (channel.equals("fillgame2jxsj")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(86,113);to=(87,49)]
                out.write(">12.剑侠世界充值</OPTION>\r\n                    <OPTION value=\"fillgame2jx3\" ");

            // end
            // begin [file="/search/fillgame.jsp";from=(87,51);to=(87,109)]
                if (channel.equals("fillgame2jx3")) out.print("selected");
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(87,111);to=(90,108)]
                out.write(">13.剑网3充值</OPTION>\r\n                  </SELECT>&nbsp;&nbsp;\r\n                  查询期间\r\n                  <input name=\"ddatetime\" type=\"text\" size=\"21\" readonly maxlength=\"21\" class=\"text\" value=\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(90,111);to=(90,160)]
                out.print(Util.setNullToEmpty((String)hs1.get("ddatetime")));
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(90,162);to=(117,16)]
                out.write("\">&nbsp;&nbsp;\r\n                  <IMG src=\"../images/icon_time.gif\" width=\"16\" height=\"16\" onClick=\"GetPeriod('ddatetime')\" style=\"cursor:hand \">\r\n                  <button name=\"btnSearch\" onClick=\"onSearch()\">查询</button></td>\r\n              </tr>\r\n            </table>\r\n          </td>\r\n        </tr>\r\n        <tr>\r\n          <td height=\"350\" align=\"center\" valign=\"top\"><br>\r\n                <table width=\"98%\" border=\"1\" cellspacing=\"0\" bordercolor=\"#000000\" bgcolor=\"#FFFFFF\" id=\"tabStock\"  class=\"tablelist\">\r\n                  <thead>\r\n                    <tr>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">序号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值时间</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">玩家帐号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">通行证</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡号</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值IP</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡面额</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">卡数量</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值渠道</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值方式</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值区</font></td>\r\n                      <td align=\"center\" nowrap><font color=\"#FFFFFF\">充值系统</font></td>\r\n                    </tr>\r\n                  </thead>\r\n                  <tbody>\r\n                ");

            // end
            // begin [file="/search/fillgame.jsp";from=(117,18);to=(156,8)]
                
                				int viewList = 0;
                   String strPageAction="";
                   ArrayList lt=new ArrayList();
                   //ArrayList lt1 = new ArrayList();
                   //HashMap hs1 = Util.parseUploadData(request, lt1);
                   HttpSession se = request.getSession();
                	 String dDateTime="";//充值时间
                	String cChannel="";//充值渠道
                	String cAccName="";//玩家帐号
                	String cPassport="";//Passport
                	String cCardCode="";//一卡通卡号
                	String cClientIP="";//充值IP
                	String cCardType="";//卡类型
                	String iCardNumber="";//卡数量
                	String cGateway="";
                	String cServerIP="";
                	String cFillModel="";//充值方式
                	String cFillSys="";//充值系统
                    HashMap hs = new HashMap();
                
                   if (hs1.containsKey("caccname")) {
                       viewList = 1;
                       lt = SearchHelperId.getFillLogGame(hs1);
                   }
                       for(int i = 0;i<lt.size();i++) //显示表体一行
                       {
                		 hs = (HashMap)lt.get(i);
                		 dDateTime = Util.setNullToEmpty((String)hs.get("ddatetime"));
                		 cChannel = Util.setNullToEmpty((String)hs.get("cchannel"));
                		 cAccName = Util.setNullToEmpty((String)hs.get("caccname"));
                		 cPassport = Util.setNullToEmpty((String)hs.get("cpassport"));
                		 cCardCode = Util.setNullToEmpty((String)hs.get("ccardcode"));
                		 cClientIP = Util.setNullToEmpty((String)hs.get("cclientip"));
                		 cCardType = Util.setNullToEmpty((String)hs.get("ccardtype"));
                		 iCardNumber = Util.setNullToEmpty((String)hs.get("icardnumber"));
                		 cGateway = Util.setNullToEmpty((String)hs.get("cgateway"));
                		 cServerIP = Util.setNullToEmpty((String)hs.get("cserverip"));
                		 cFillModel = Util.setNullToEmpty((String)hs.get("cplusmodel"));
                        
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(156,10);to=(157,24)]
                out.write("\r\n                    <tr ");

            // end
            // begin [file="/search/fillgame.jsp";from=(157,26);to=(157,37)]
                if(i%2==0){
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(157,39);to=(157,57)]
                out.write(" bgcolor=\"#F3FBFE\"");

            // end
            // begin [file="/search/fillgame.jsp";from=(157,59);to=(157,60)]
                }
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(157,62);to=(158,48)]
                out.write(">\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(158,51);to=(158,54)]
                out.print(i+1);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(158,56);to=(159,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(159,51);to=(159,60)]
                out.print(dDateTime);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(159,62);to=(160,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(160,51);to=(160,59)]
                out.print(cAccName);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(160,61);to=(161,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(161,51);to=(161,60)]
                out.print(cPassport);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(161,62);to=(162,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(162,51);to=(162,60)]
                out.print(cCardCode);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(162,62);to=(163,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(163,51);to=(163,60)]
                out.print(cClientIP);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(163,62);to=(164,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(164,51);to=(164,60)]
                out.print(cCardType);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(164,62);to=(165,48)]
                out.write("元</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(165,51);to=(165,62)]
                out.print(iCardNumber);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(165,64);to=(166,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(166,51);to=(166,59)]
                out.print(cChannel);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(166,61);to=(167,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(167,51);to=(167,61)]
                out.print(cFillModel);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(167,63);to=(168,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(168,51);to=(168,59)]
                out.print(cGateway);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(168,61);to=(169,48)]
                out.write("</td>\r\n                      <td align=\"center\" nowrap>");

            // end
            // begin [file="/search/fillgame.jsp";from=(169,51);to=(169,60)]
                out.print(cServerIP);
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(169,62);to=(171,20)]
                out.write("</td>\r\n                    </tr>\r\n                    ");

            // end
            // begin [file="/search/fillgame.jsp";from=(171,22);to=(173,4)]
                
                       }
                    
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(173,6);to=(185,6)]
                out.write("\r\n                  </tbody>\r\n                </table>\r\n          </td>\r\n        </tr>\r\n      </table><br>\r\n\t  <table width=\"166\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n             <tr>\r\n               <td><button onclick=\"history.back()\">返回</button></td>\r\n               <td><button onclick=\"window.print()\">打印</button></td>\r\n             </tr>\r\n           </table>\r\n      ");

            // end
            // begin [file="/search/fillgame.jsp";from=(185,8);to=(185,40)]
                if(lt.size()==0 && viewList==1){
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(185,42);to=(189,6)]
                out.write("\r\n      <script language=\"javascript\">\r\n      \talert(\"查询到0条记录。\");\r\n      </script>\r\n      ");

            // end
            // begin [file="/search/fillgame.jsp";from=(189,8);to=(189,9)]
                }
            // end
            // HTML // begin [file="/search/fillgame.jsp";from=(189,11);to=(198,0)]
                out.write("\r\n    <!-- InstanceEndEditable --></td>\r\n  </tr>\r\n  <tr>\r\n    <td height=\"16\" align=\"center\" bgcolor=\"#BBDDFD\">Copyright &copy; 2005 Kingsoft Corp,All Rights Reserved</td>\r\n  </tr>\r\n</table>\r\n</body>\r\n<!-- InstanceEnd --></html>\r\n");

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
