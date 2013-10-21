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

namespace Fireball.Plugins
{
    public abstract class PluginInfo
    {
        private string _Author = null;
        private string _Company = null;
        private string _Email = null;
        private string _WebSite = null;
        private Version _Version = null;

        public Version Version
        {
            get { return _Version; }
        }

        public string WebSite
        {
            get { return _WebSite; }
        }

        public string Email
        {
            get { return _Email; }
        }

        public string Company
        {
            get { return _Company; }
        }

        public string Author
        {
            get { return _Author; }
        }

        protected void SetAuthor(string author)
        {
            _Author = author;
        }

        protected void SetCompany(string company)
        {
            _Company = company;
        }

        protected void SetEmail(string email)
        {
            _Email = email;
        }

        protected void SetWebSite(string webSite)
        {
            _WebSite = webSite;
        }

        protected void SetVersion(Version version)
        {
            _Version = version;
        }

        protected virtual void ShowAboutBox()
        {

        }
    }
}
