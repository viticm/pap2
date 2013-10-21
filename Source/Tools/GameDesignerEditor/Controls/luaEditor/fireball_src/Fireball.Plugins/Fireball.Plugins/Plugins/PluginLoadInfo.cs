//    Copyright (C) 2005  Sebastian Faltoni
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

namespace Fireball.Plugins
{
    public class PluginLoadInfo
    {
        private bool _Loaded = false;
        private string _Filename = null;
        private string _ClassName = null;

        public PluginLoadInfo(bool loaded,
            string filename, string className)
        {
            _Loaded = loaded;
            _Filename = filename;
            _ClassName = className;
        }

        public string ClassName
        {
            get
            {
                return _ClassName;
            }
            set
            {
                _ClassName = value;
            }
        }
        public bool Loaded
        {
            get { return _Loaded; }
            set { _Loaded = value; }
        }
        public string Filename
        {
            get
            {
                return _Filename;
            }
            set
            {
                _Filename = value;
            }
        }
    }
}
