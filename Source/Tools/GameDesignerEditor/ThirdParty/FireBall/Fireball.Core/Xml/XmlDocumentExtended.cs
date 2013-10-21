
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Fireball.Xml
{
    public class XmlDocumentExtended : XmlDocument
    {
        private int m_ElementCount;

        private XmlTextReader m_Reader = null;

        private string m_RealFileName;

        public string RealFileName
        {
            get { return m_RealFileName; }
        }

        public XmlDocumentExtended()
            : base()
        {
            m_ElementCount = 0;
        }

        public override XmlElement CreateElement(string prefix, string localname, string nsURI)
        {
            XmlNodeExtended elem = new XmlNodeExtended(prefix, localname, nsURI, this);

            elem.SetLineInfo(m_Reader.LineNumber, m_Reader.LinePosition);


            return elem;
        }

        public XmlNodeExtended SelectSingleNodeEx(string xpath)
        {
            return (XmlNodeExtended)this.SelectSingleNode(xpath);
        }

        public override void Load(string filename)
        {
            m_RealFileName = filename;

            m_Reader = new XmlTextReader(filename);

            base.Load(m_Reader);

            m_Reader.Close();
        }

        public override void Load(System.IO.Stream inStream)
        {
            m_Reader = new XmlTextReader(inStream);

            base.Load(m_Reader);

            m_Reader.Close();
        }

        public override void Load(System.IO.TextReader txtReader)
        {
            m_Reader = new XmlTextReader(txtReader);

            base.Load(m_Reader);

            m_Reader.Close();
        }

        public override void Load(XmlReader reader)
        {
            string xml = reader.ReadOuterXml();

            m_Reader = new XmlTextReader(xml, XmlNodeType.Document, null);

            base.Load(m_Reader);

            m_Reader.Close();
        }

        public override void LoadXml(string xml)
        {
            m_Reader = new XmlTextReader(xml, XmlNodeType.Document, null);

            base.Load(m_Reader);

            m_Reader.Close();
        }

        public void IncrementElementCount()
        {
            m_ElementCount++;
        }

        public int GetCount()
        {
            return m_ElementCount;
        }

    }
}
