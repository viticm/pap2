package org.apache.jsp;

import java.util.*;
import com.kingsoft.gmsystem.domain.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class menutree$jsp extends HttpJspBase {


    static {
    }
    public menutree$jsp( ) {
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

            // HTML // begin [file="/menutree.jsp";from=(0,141);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/menutree.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/menutree.jsp";from=(2,0);to=(2,43)]
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
                        // begin [file="/menutree.jsp";from=(2,0);to=(2,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/menutree.jsp";from=(2,43);to=(3,0)]
                out.write("\r\n");

            // end
            // begin [file="/menutree.jsp";from=(3,2);to=(10,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
                    UserInfo userinfo=(UserInfo)session.getAttribute("userinfo");
                    List ltMenu=userinfo.getUsermenu();
                    PowerModule powermodule=null;
            // end
            // HTML // begin [file="/menutree.jsp";from=(10,2);to=(47,23)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<title>左菜单</title>\r\n<link href=\"css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<script src=\"js/alai_tree.js\"></script>\r\n<script src=\"js/alai_tree_win2k.js\"></script>\r\n</head>\r\n<body bgcolor=\"#BBDDFD\">\r\n<table width=\"170\" height=\"100%\"  border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n  <tr>\r\n    <td width=\"165\" id=\"tdTree\">\r\n\t\t<table width=\"100%\" height=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n\t\t  <tr>\r\n\t\t\t<td height=\"20\" align=\"center\"><a href=\"javascript:tree1.expandAll(true)\">全部展开</a>&nbsp;|&nbsp;<a href=\"javascript:tree1.expandAll(false)\">全部收缩</a></td>\r\n\t\t\t</tr>\r\n\t\t  <tr>\r\n\t\t\t<td bgcolor=\"#FFFFFF\"><div id=\"divTree\" style=\"overflow:auto;height:100%;width:165;\"></div></td>\r\n\t\t\t</tr>\r\n\t\t  <tr>\r\n\t\t\t<td height=\"16\">&nbsp;</td>\r\n\t\t\t</tr>\r\n\t\t</table>\r\n\t</td>\r\n    <td width=\"10\">\r\n\t\t<table height=\"30\" border=0 cellpadding=0 cellspacing=0  style=\"cursor:hand \">\r\n\t\t  <tbody>\r\n\t\t\t<tr>\r\n\t\t\t  <td onClick=switchSysBar() bgcolor=\"#096ACC\"><span class=navPoint id=switchPoint title=关闭/打开左栏>3</span></td>\r\n\t\t\t</tr>\r\n\t\t  </tbody>\r\n\t\t</table>\r\n\t</td>\r\n  </tr>\r\n</table>\r\n<script language=\"Javascript\">\r\nwindow.status=\" 当前登录用户：");

            // end
            // begin [file="/menutree.jsp";from=(47,26);to=(47,48)]
                out.print(userinfo.getUsercode());
            // end
            // HTML // begin [file="/menutree.jsp";from=(47,50);to=(47,62)]
                out.write("       登录时间：");

            // end
            // begin [file="/menutree.jsp";from=(47,65);to=(47,88)]
                out.print(userinfo.getLogintime());
            // end
            // HTML // begin [file="/menutree.jsp";from=(47,90);to=(73,12)]
                out.write("\";\r\n/****************************\r\n缩放\r\n***************************/\r\nfunction switchSysBar()\r\n{\r\n\tif (switchPoint.innerText==3){\r\n\t\tswitchPoint.innerText=4\r\n\t\t//收缩左边\r\n\t\tdocument.all(\"tdTree\").style.display=\"none\";\r\n\t\ttop.mainSet.cols=\"10,*\";\r\n\t}\r\n\telse\r\n\t{\r\n\t\tswitchPoint.innerText=3\r\n\t\t//显示左边\r\n\t\tdocument.all(\"tdTree\").style.display=\"\";\r\n\t\ttop.mainSet.cols=\"175,*\";\r\n\t}\r\n}\r\n\r\n //win2k风格模板\r\n //建立ImageList控件，并添加相关图标\r\n\r\nvar icons=new alai_imagelist()\r\n\r\nicons.path=\"");

            // end
            // begin [file="/menutree.jsp";from=(73,15);to=(73,39)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/menutree.jsp";from=(73,41);to=(75,11)]
                out.write("/images/\"\r\nicons.type=\"gif\"\r\nicons.add(\"");

            // end
            // begin [file="/menutree.jsp";from=(75,14);to=(75,38)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/menutree.jsp";from=(75,40);to=(86,1)]
                out.write("/images/fold_xp.gif\",\"default\")\r\nicons.add(\"plus.gif\",\"expand\")\r\nicons.add(\"minus\",\"collapse\")\r\nicons.add(\"html\",\"link\")\r\n\r\n\r\n/*开始构建目录树*/\r\nvar tree1=new alai_tree(icons,18,divTree)\r\nvar root=tree1.root\r\nvar n1=root.add(\" 服务功能 \",\"default\")\r\n      n1.add(\"GM频道处理\",\"file.gif\",\"js\",\"top.mainFrame.location.href='gmc/integrate.jsp'\")\r\n\t");

            // end
            // begin [file="/menutree.jsp";from=(86,3);to=(90,12)]
                
                        for(int i=0;i<ltMenu.size();i++){
                          powermodule=(PowerModule)ltMenu.get(i);
                          if(powermodule.getUpgrade().equals("Service")){
                            
            // end
            // HTML // begin [file="/menutree.jsp";from=(90,14);to=(91,14)]
                out.write("\r\n      n1.add(\"");

            // end
            // begin [file="/menutree.jsp";from=(91,17);to=(91,44)]
                out.print(powermodule.getModulename());
            // end
            // HTML // begin [file="/menutree.jsp";from=(91,46);to=(91,94)]
                out.write("\",\"file.gif\",\"js\",\"top.mainFrame.location.href='");

            // end
            // begin [file="/menutree.jsp";from=(91,97);to=(91,117)]
                out.print(powermodule.getUrl());
            // end
            // HTML // begin [file="/menutree.jsp";from=(91,119);to=(92,12)]
                out.write("'\")\r\n            ");

            // end
            // begin [file="/menutree.jsp";from=(92,14);to=(95,8)]
                
                            }
                          }
                        
            // end
            // HTML // begin [file="/menutree.jsp";from=(95,10);to=(98,1)]
                out.write("\r\n\r\nvar n2=root.add(\" 查询功能 \",\"default\")\r\n\t");

            // end
            // begin [file="/menutree.jsp";from=(98,3);to=(102,12)]
                
                        for(int i=0;i<ltMenu.size();i++){
                          powermodule=(PowerModule)ltMenu.get(i);
                          if(powermodule.getUpgrade().equals("Search")){
                            
            // end
            // HTML // begin [file="/menutree.jsp";from=(102,14);to=(103,14)]
                out.write("\r\n      n2.add(\"");

            // end
            // begin [file="/menutree.jsp";from=(103,17);to=(103,44)]
                out.print(powermodule.getModulename());
            // end
            // HTML // begin [file="/menutree.jsp";from=(103,46);to=(103,94)]
                out.write("\",\"file.gif\",\"js\",\"top.mainFrame.location.href='");

            // end
            // begin [file="/menutree.jsp";from=(103,97);to=(103,117)]
                out.print(powermodule.getUrl());
            // end
            // HTML // begin [file="/menutree.jsp";from=(103,119);to=(104,12)]
                out.write("'\")\r\n            ");

            // end
            // begin [file="/menutree.jsp";from=(104,14);to=(107,8)]
                
                            }
                          }
                        
            // end
            // HTML // begin [file="/menutree.jsp";from=(107,10);to=(114,1)]
                out.write("\r\n\r\nvar n4=root.add(\" 个人设置 \",\"default\")\r\n      n4.add(\"设置工作范围\",\"file.gif\",\"js\",\"top.mainFrame.location.href='individual/setgw.jsp'\")\r\n      n4.add(\"修改登录密码\",\"file.gif\",\"js\",\"top.mainFrame.location.href='individual/modifypwd.jsp'\")\r\n\r\nvar n5=root.add(\" 系统维护 \",\"default\")\r\n\t");

            // end
            // begin [file="/menutree.jsp";from=(114,3);to=(118,12)]
                
                        for(int i=0;i<ltMenu.size();i++){
                          powermodule=(PowerModule)ltMenu.get(i);
                          if(powermodule.getUpgrade().equals("System")){
                            
            // end
            // HTML // begin [file="/menutree.jsp";from=(118,14);to=(119,14)]
                out.write("\r\n      n5.add(\"");

            // end
            // begin [file="/menutree.jsp";from=(119,17);to=(119,44)]
                out.print(powermodule.getModulename());
            // end
            // HTML // begin [file="/menutree.jsp";from=(119,46);to=(119,94)]
                out.write("\",\"file.gif\",\"js\",\"top.mainFrame.location.href='");

            // end
            // begin [file="/menutree.jsp";from=(119,97);to=(119,117)]
                out.print(powermodule.getUrl());
            // end
            // HTML // begin [file="/menutree.jsp";from=(119,119);to=(120,12)]
                out.write("'\")\r\n            ");

            // end
            // begin [file="/menutree.jsp";from=(120,14);to=(123,8)]
                
                            }
                          }
                        
            // end
            // HTML // begin [file="/menutree.jsp";from=(123,10);to=(151,0)]
                out.write("\r\n\r\nvar n6=root.add(\"退出系统\",\"fold_xp.gif\",\"js\",\"top.window.navigate('login.jsp?action=exit')\");\r\n\r\ntree1.target=\"_blank\"\r\n\r\n/*收缩目录树*/\r\n//tree1.expandToTier(1)\r\ntree1.expandAll(true);\r\n\r\n/*简单的目录树事件处理*/\r\ntree1.onmouseover=function(node)\r\n{\r\n\tnode.label.style.textDecoration=\"underline\"\r\n}\r\n\r\ntree1.onmouseout=function(node)\r\n{\r\n\tnode.label.style.textDecoration=\"none\"\r\n}\r\n\r\ntree1.onblur=tree1.onfocus=function(node)\r\n{\r\n\r\n}\r\n</script>\r\n</body>\r\n</html>\r\n");

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
