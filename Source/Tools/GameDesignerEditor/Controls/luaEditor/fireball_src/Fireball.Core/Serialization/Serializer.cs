
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
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Runtime;
using System.Reflection;
using System.Xml;

namespace Fireball.Serialization
{
    public class Serializer
    {

        public static string Serialize(object obj)
        {
           object[] attrs = obj.GetType().GetCustomAttributes(typeof(SerializerAttribute), false);

           if (attrs.Length == 0)
           {
               throw new InvalidOperationException("The obj is not marked with SerializerAttribute");
           }


           PropertyInfo[] infos = obj.GetType().GetProperties(BindingFlags.Public | BindingFlags.NonPublic);

           StringBuilder sbXml = new StringBuilder();

           XmlTextWriter xwr = new XmlTextWriter(new StringWriter(sbXml), Encoding.UTF8);

           xwr.Formatting = Formatting.Indented;

           xwr.WriteStartElement("Object");
           xwr.WriteAttributeString("Assembly", obj.GetType().Assembly.FullName);
           xwr.WriteAttributeString("Type", obj.GetType().FullName);

           foreach (PropertyInfo current in infos)
           {
               if (!current.CanWrite)
                   throw new Exception("The property " + current.Name + " don't have a set method!");
               xwr.WriteStartElement("Property");
               xwr.WriteAttributeString("Name", current.Name);
               xwr.WriteAttributeString("Type", current.PropertyType);

               xwr.WriteEndElement();
           }

           xwr.WriteEndElement();

           xwr.Flush();

           xwr.Close();

           return sbXml.ToString();
        }
    }
}
