using System;
using System.Collections.Generic;
using System.Text;

using System.Xml;
using System.Collections;

namespace FlowChartVarialbe
{
    public class FCVarialbes
    {
        string m_strTypeId;//变量类型

        
        XmlDataDocument m_XmlResult;//多个变量集合的并集结果，也是要返回的结果
        XmlDataDocument m_XmlTemp;//为求得m_XmlResult而建立的辅助


        
        public FCVarialbes(string strtypeid, string strEventTag)
        {
            m_XmlResult = new XmlDataDocument();
            m_XmlTemp = new XmlDataDocument();

            SetInnerXml(m_XmlResult, strEventTag);
            m_strTypeId = strtypeid;
        }

        /// <summary>
        /// 把下一个事件节点的tag加进来
        /// </summary>
        /// <param name="strEventTag"></param>
        public void AddNextEventTag(string strEventTag)
        {
            if(strEventTag == "")
            {
                return;
            }
            SetInnerXml(m_XmlTemp, strEventTag);
            Union2Xml();
        }

        /// <summary>
        /// 设置Xmldoc的内容为strInnerXml
        /// </summary>
        /// <param name="Xmldoc"></param>
        /// <param name="strInnerXml">为空时初始化Xmldoc</param>
        protected void SetInnerXml(XmlDataDocument Xmldoc, string strInnerXml)
        {
            if (strInnerXml == "")
            {
                strInnerXml = @"<variables></variables>";
            }
            if (Xmldoc.HasChildNodes)
            {
                Xmldoc.RemoveAll();
            }
            Xmldoc.InnerXml = strInnerXml;
        }

        /// <summary>
        /// 取m_XmlResult, m_XmlTemp并集.结果放到m_XmlResult
        /// </summary>
        protected void Union2Xml()
        {
            XmlNodeList nodelist = m_XmlTemp.DocumentElement.SelectNodes("variable");
            foreach (XmlNode node in nodelist)
            {
                if (!HasNode(node))
                {
                    AddNode(node);
                }
            }

        }
        /// <summary>
        /// 判断m_XmlResult是否含有节点node1
        /// </summary>
        /// <param name="node1"></param>
        /// <returns></returns>
        protected bool HasNode(XmlNode node1)
        {
            if (null == node1)
            {
                return true;
            }
            string strname = node1.SelectSingleNode("name").FirstChild.Value;
            string strtypeid = node1.SelectSingleNode("typeid").FirstChild.Value;
            string strpath = string.Format("//variable[name='{0}'][typeid='{1}']", strname, strtypeid);
            XmlNode root = m_XmlResult.DocumentElement.SelectSingleNode(strpath);
            return (root != null);
        }
        /// <summary>
        /// 向m_XmlResult添加节点node1
        /// </summary>
        /// <param name="node1"></param>
        protected void AddNode(XmlNode node1)
        {
            XmlElement root = m_XmlResult.DocumentElement;
            XmlElement newVariable = m_XmlResult.CreateElement("variable");
            newVariable.InnerXml = node1.InnerXml;
            root.AppendChild(newVariable);
        }


        public string Show(ArrayList UsedVariablesList, string strOldValue, string strTypeName)
        {
            string strValue = "";
            VariableForm myform = new VariableForm(m_strTypeId, m_XmlResult, UsedVariablesList,strOldValue,strTypeName);
            if (myform.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                m_XmlResult = myform.RtnResultXml();
                strValue = myform.RtnChoosedName();
            }
            return strValue;
            
        }

        /// <summary>
        /// 返回m_XmlResult的InnerXml
        /// </summary>
        /// <returns></returns>
        public string RtnResultXml()
        {
            return m_XmlResult.InnerXml;
        }
    }
}
