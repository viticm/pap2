using System;
using System.Collections.Generic;
using System.Text;
using Fireball.IO.Compression.Zip;

namespace Fireball.IO
{
    public abstract class ZipHelperEntry
    {
        ZipHelper _owner;
        ZipEntry _zipEntry;
        string _name;
        string _path;
        //bool _isRoot;

        protected internal ZipHelperEntry(ZipHelper owner, string path)
        {
            _owner = owner;
            _path = path;

            if (path == "/" || path == "")
            {
                _zipEntry = null;
                //_isRoot = true;
                _name = "<root>";
            }
            else
            {
                _zipEntry = _owner.GetZipEntry(_path);
                _name = _owner.ExtractName(_path);
            }
        }

        internal ZipEntry InternalZipEntry
        {
            get
            {
                return _zipEntry;
            }
            set
            {
                _zipEntry = value;
            }
        }
        internal ZipHelper OwnerZip
        {
            get
            {
                return _owner;
            }
        }

        public string Name
        {
            get
            {
                return _name;
            }
        }
        public string Path
        {
            get
            {
                return _path;
            }
        }
    }
}
