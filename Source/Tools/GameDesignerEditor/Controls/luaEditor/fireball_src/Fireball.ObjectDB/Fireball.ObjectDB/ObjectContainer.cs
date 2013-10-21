#region License

//Copyright (C) <2005>  <Sebastian Faltoni> sebastian(dot)faltoni(at)gmail(dot)com

//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#endregion License

using System;
using System.Collections.Generic;
using System.Text;
using com.db4o;

namespace Fireball.ObjectDB
{
    public class ObjectDB
    {
        private ObjectContainer _Container = null;

        public ObjectDB(string dbFilename)
        {
            _Container = Db4o.OpenFile(dbFilename);
        }

        public void StoreObject(DBObjectBase obj)
        {
            _Container.Set(obj);
        }

        public IList<T> Query<T>(Predicate<T> predicate) where T : DBObjectBase
        {
            return _Container.Query<T>(predicate); 
        }

        public IList<T> QueryAll<T>() where T : DBObjectBase
        {
            return _Container.Query<T>();
        }

        public bool Close()
        {
            return _Container.Close();
        }
    }
}
