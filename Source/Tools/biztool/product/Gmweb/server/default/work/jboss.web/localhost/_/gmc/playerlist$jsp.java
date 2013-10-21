package org.apache.jsp;

import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import org.apache.jasper.runtime.*;


public class playerlist$jsp extends HttpJspBase {


    static {
    }
    public playerlist$jsp( ) {
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

            // HTML // begin [file="/gmc/playerlist.jsp";from=(0,111);to=(1,0)]
                out.write("\r\n");

            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(1,56);to=(2,0)]
                out.write("\r\n");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(2,0);to=(2,43)]
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
                        // begin [file="/gmc/playerlist.jsp";from=(2,0);to=(2,43)]
                        } while (_jspx_th_power_checkpower_0.doAfterBody() == javax.servlet.jsp.tagext.BodyTag.EVAL_BODY_AGAIN);
                    }
                    if (_jspx_th_power_checkpower_0.doEndTag() == javax.servlet.jsp.tagext.Tag.SKIP_PAGE)
                        return;
                } finally {
                    _jspx_th_power_checkpower_0.release();
                }
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(2,43);to=(6,12)]
                out.write("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=GBK\">\r\n<link href=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(6,15);to=(6,41)]
                out.print( request.getContextPath() );
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(6,43);to=(8,0)]
                out.write("/css/public.css\" rel=\"stylesheet\" type=\"text/css\">\r\n<title>玩家问题列表</title>\r\n");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(8,2);to=(12,0)]
                
                    response.addHeader("Pragma","no-cache") ;
                    response.addHeader("Cache-Control","no-cache") ;
                    response.addDateHeader("Expires",-1);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(12,2);to=(20,13)]
                out.write("\r\n<style>\r\n\toption.haveanswer{background-color:#999999}\r\n\toption.green{background-color:#E8FFF3}\r\n\toption.blue{background-color:#DFE9FF}\r\n\toption.red{background-color:#FFE6E6}\r\n</style>\r\n<script language=\"JavaScript\">\r\nvar AbsPath=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(20,16);to=(20,40)]
                out.print(request.getContextPath());
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(20,42);to=(120,2)]
                out.write("\";\r\nvar objOption;\r\nvar intHaveAnswer=0;\r\nvar intQuestion;\r\nvar intCurrent;\r\nfunction onSelectPlayer()\r\n{\r\n\tvar objSel=document.all(\"selPlayerList\");\r\n\tif(objSel.selectedIndex==-1) return;\r\n\tobjOption=objSel.options(objSel.selectedIndex);\r\n\t\tvar account=objSel.value;\r\n\t\tvar question=objOption.question;\r\n\t\tvar area=objOption.area;\r\n\t\tvar group=objOption.group;\r\n\t\tvar quiztime=objOption.quiztime;\r\n\t\tvar iid=objOption.iid;\r\n\t\tvar role=objOption.role;\r\n\t\tvar game_id=objOption.game_id;\r\n\t\tvar subtype=objOption.subtype;\r\n\t\tSetSubType(subtype);\r\n\t\tparent.document.all(\"questionid\").value=iid;\r\n\t\tparent.document.all(\"account\").value=account;\r\n\t\tSetParentGwcode(game_id,area,group);\r\n\t\tSetParentQuickanswer(game_id);\r\n\t\tparent.document.all(\"game_id\").value=game_id;\r\n\t\tparent.document.all(\"area\").value=area;\r\n\t\tparent.document.all(\"group\").value=group;\r\n\t\tparent.document.all(\"role\").value=role;\r\n\t\tparent.document.all(\"quiztime\").value=quiztime;\r\n\t\tparent.document.all(\"question\").value=question;\r\n\r\n\t\tparent.document.all(\"answer\").disabled=false;\r\n\t\tparent.document.all(\"answer\").value=objOption.answer;\r\n\t\tparent.document.all(\"selQuickanswer\").disabled=false;\r\n\t\tparent.document.all(\"btnAnswer\").disabled=false;\r\n\t\tparent.document.all(\"btnDelete\").disabled=false;\r\n\tif(objOption.className==\"haveanswer\")\r\n\t{\r\n\t\tparent.document.all(\"answer\").disabled=true;\r\n\t\tparent.document.all(\"selQuickanswer\").disabled=true;\r\n\t\tparent.document.all(\"btnAnswer\").disabled=true;\r\n\t\tparent.document.all(\"btnDelete\").disabled=true;\r\n\t}\r\n\tintCurrent=Number(objOption.serial);\r\n\tparent.document.all(\"selQuickanswer\").selectedIndex=-1;\r\n}\r\n\r\n/*********************************************\r\n\r\n***********************************************/\r\nfunction SetParentGwcode(game_id,area,group)\r\n{\r\n\tvar objSel=parent.document.all(\"selGateway\");\r\n\tvar objOption,i;\r\n\tfor(i=0;i<objSel.options.length;i++)\r\n\t{\r\n\t\tobjOption=objSel.options[i];\r\n\t\tobjOption.selected=false;\r\n\t\tif(objOption.game_id==game_id && objOption.area==area && objOption.group==group)\r\n\t\t{\r\n\t\t\tobjOption.selected=true;\r\n\t\t\tbreak;\r\n\t\t}\r\n\t}\r\n}\r\n\r\nfunction SetSubType(subtype)\r\n{\r\n\tvar objSel=parent.document.all(\"subType\");\r\n\tvar objOption,i;\r\n\tfor(i=0;i<objSel.options.length;i++)\r\n\t{\r\n\t\tobjOption=objSel.options[i];\r\n\t\tobjOption.selected=false;\r\n\t\tif(objOption.value==subtype)\r\n\t\t{\r\n\t\t\tobjOption.selected=true;\r\n\t\t\tbreak;\r\n\t\t}\r\n\t}\r\n}\r\n/************************\r\n*************************/\r\nfunction SetParentQuickanswer(game_id)\r\n{\r\n\tvar objSel=parent.document.all(\"selQuickanswer\"),objOption;\r\n\tvar objSelTemp=parent.document.all(\"selQuickanswerTemp\");\r\n\tobjSel.length=0;\r\n\tfor(i=0;i<objSelTemp.length;i++)\r\n\t{\r\n\t\tobjOption=objSelTemp.options[i];\r\n\t\tif(objOption.game_id.indexOf(\"'\"+game_id+\"'\")!=-1) objSel.options[objSel.length]=new Option(objOption.value,objOption.value);\r\n\t}\r\n}\r\n\r\n</script>\r\n</head>\r\n<body leftmargin=\"0\" topmargin=\"0\">\r\n\r\n<select name=\"selPlayerList\" size=\"25\" style=\"width:100%\"  onChange=\"onSelectPlayer()\" onmouseover=\"this.focus()\" disabled>\r\n  ");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(120,4);to=(158,0)]
                
                
                  ArrayList lt = (ArrayList)session.getAttribute("playerlist");
                  //session.removeAttribute("playerlist");
                      HashMap hs = new HashMap();
                      String iid = "";
                      String quiztime = "";
                      String game_id = "";
                      String area = "";
                      String group = "";
                      String role = "";
                      String account="";
                      String question = "";
                      String gateway_name = "";
                	  String color="red";
                      String subtype="";
                	  int delay=0;
                  for (int i=0, n=lt.size(); i<n; i++) {
                  	  color="red";
                      hs = (HashMap)lt.get(i);
                      iid = (String)hs.get("iid");
                      quiztime = (String)hs.get("quiztime");
                      game_id = (String)hs.get("game_id");
                      area = (String)hs.get("area");
                      group = (String)hs.get("group");
                      role = (String)hs.get("role"); 
                      account=(String)hs.get("account");
                      question = (String)hs.get("question");
                      gateway_name = (String)hs.get("gateway_name");
                      subtype = (String)hs.get("questionsubtype");
                      
                      question=question.replace('"','\'');
                	  delay= Integer.parseInt((String)hs.get("delay"));
                	  if(delay<10){
                	  	color="green";
                	  }else if(delay<30){
                	  	color="blue";
                	  }
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(158,2);to=(159,17)]
                out.write("\r\n  <option class=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,20);to=(159,25)]
                out.print(color);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,27);to=(159,38)]
                out.write("\" subtype=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,41);to=(159,48)]
                out.print(subtype);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,50);to=(159,60)]
                out.write("\" serial=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,63);to=(159,64)]
                out.print(i);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,66);to=(159,75)]
                out.write("\" value=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,78);to=(159,85)]
                out.print(account);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,87);to=(159,96)]
                out.write("\" iid = \"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,99);to=(159,102)]
                out.print(iid);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,104);to=(159,116)]
                out.write("\" question=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,119);to=(159,127)]
                out.print(question);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,129);to=(159,151)]
                out.write("\" answer=\"\" quiztime=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,154);to=(159,162)]
                out.print(quiztime);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,164);to=(159,172)]
                out.write("\" area=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,175);to=(159,179)]
                out.print(area);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,181);to=(159,190)]
                out.write("\" group=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,193);to=(159,198)]
                out.print(group);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,200);to=(159,208)]
                out.write("\" role=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,211);to=(159,215)]
                out.print(role);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,217);to=(159,228)]
                out.write("\" game_id=\"");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,231);to=(159,238)]
                out.print(game_id);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,240);to=(159,242)]
                out.write("\">");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,245);to=(159,250)]
                out.print(delay);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,252);to=(159,265)]
                out.write("&nbsp;|&nbsp;");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,268);to=(159,276)]
                out.print(quiztime);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,278);to=(159,291)]
                out.write("&nbsp;|&nbsp;");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,294);to=(159,306)]
                out.print(gateway_name);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,308);to=(159,321)]
                out.write("&nbsp;|&nbsp;");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,324);to=(159,328)]
                out.print(role);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,330);to=(159,343)]
                out.write("&nbsp;|&nbsp;");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(159,346);to=(159,354)]
                out.print(question);
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(159,356);to=(160,2)]
                out.write("</option>\r\n  ");

            // end
            // begin [file="/gmc/playerlist.jsp";from=(160,4);to=(162,0)]
                
                  }
            // end
            // HTML // begin [file="/gmc/playerlist.jsp";from=(162,2);to=(179,0)]
                out.write("\r\n\r\n</select>\r\n<script language=\"JavaScript\">\r\n\tdocument.all(\"selPlayerList\").disabled=false;\r\n\tintQuestion=document.all(\"selPlayerList\").options.length;\r\n\tparent.document.all(\"lblNumber\").innerText=intQuestion;\r\n\tparent.document.all(\"lblHaveAnswer\").innerText=intHaveAnswer;\r\n\tif(intQuestion!=0)\r\n\t{\r\n\t\tdocument.all(\"selPlayerList\").selectedIndex=0;\r\n\t\tonSelectPlayer();\r\n\t}\r\n</script>\r\n\r\n</body>\r\n</html>\r\n");

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
