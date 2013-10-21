using System;
using System.Collections.Generic;
using System.Text;
using Fireball.IO.Compression.Zip;

namespace Fireball.IO
{
    public class ZipHelperDir : ZipHelperEntry
    {

        internal ZipHelperDir(ZipHelper owner,string path)
            :base(owner,path)
        {
        }

        public string[] GetDirs()
        {
            return OwnerZip.GetDirs(this.Path);
        }
        public string[] GetFiles()
        {
            return OwnerZip.GetFiles(this.Path);
        }
    }
}
