
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
    public  class XmlNodeExtended : XmlElement, IXmlLineInfo
    {
        int lineNumber = 0;
        int linePosition = 0;

        internal XmlNodeExtended(string prefix, string localname, string nsURI, XmlDocument doc)
            : base(prefix, localname, nsURI, doc)
        {
            ((XmlDocumentExtended)doc).IncrementElementCount();
        }

        public void SetLineInfo(int linenum, int linepos)
        {
            lineNumber = linenum;
            linePosition = linepos;
        }


        /// <summary>
        /// Ottiene il numero corrente di riga.
        /// </summary>
        /// <value></value>
        /// <returns>Numero della riga corrente o 0 se non sono disponibili informazioni sulla riga: <see cref="M:System.Xml.IXmlLineInfo.HasLineInfo"></see>, ad esempio, restituisce false.</returns>
        public int LineNumber
        {
            get
            {
                return lineNumber;
            }
        }
        /// <summary>
        /// Ottiene la posizione corrente di riga.
        /// </summary>
        /// <value></value>
        /// <returns>Posizione della riga corrente o 0 se non sono disponibili informazioni sulla riga: <see cref="M:System.Xml.IXmlLineInfo.HasLineInfo"></see>, ad esempio, restituisce false.</returns>
        public int LinePosition
        {
            get
            {
                return linePosition;
            }
        }

        /// <summary>
        /// Ottiene un valore che indica se la classe può restituire informazioni sulla riga.
        /// </summary>
        /// <returns>
        /// true se è possibile specificare la <see cref="P:System.Xml.IXmlLineInfo.LineNumber"></see> e <see cref="P:System.Xml.IXmlLineInfo.LinePosition"></see>; in caso contrario false.
        /// </returns>
        public bool HasLineInfo()
        {
            return true;
        }

        /// <summary>
        /// Selects the single node ex.
        /// </summary>
        /// <param name="xpath">The xpath.</param>
        /// <returns></returns>
        public XmlNodeExtended SelectSingleNodeEx(string xpath)
        {
            return (XmlNodeExtended)this.SelectSingleNode(xpath);
        }

        /// <summary>
        /// Gets the owner document ex.
        /// </summary>
        /// <value>The owner document ex.</value>
        public XmlDocumentExtended OwnerDocumentEx
        {
            get
            {
                if (this.OwnerDocument is XmlDocumentExtended)
                    return (XmlDocumentExtended)this.OwnerDocument;
                return null;
            }
        }
    }// End LineInfoElement class.
}
