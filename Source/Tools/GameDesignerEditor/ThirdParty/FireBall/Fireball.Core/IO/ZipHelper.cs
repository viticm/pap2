using System;
using System.Collections.Generic;
using System.Text;
using Fireball.IO.Compression.Zip;
using Fireball.Collections.Specialized;
using System.IO;

namespace Fireball.IO
{
    public class ZipHelper : IDisposable
    {
        private string _filename;
        private ZipFile _zip;
        private ZipHelperDir _currDir;
        private string _currPath;
        private bool _isOpen;
        private StringCollection _dirs;
        private StringCollection _files;

        // ------------------------------------------------------------------- //

        public ZipHelper(string filename)
        {
            _filename = filename;
            _currDir = new ZipHelperDir(this, "/");
            _currPath = _currDir.Path;
            _isOpen = false;
        }

        // ------------------------------------------------------------------- //

        ~ZipHelper()
        {
            this.Dispose();
        }
        public void Dispose()
        {
            if (_isOpen) this.Close();
        }

        // ------------------------------------------------------------------- //

        public ZipHelperDir CurrentDir
        {
            get
            {
                return _currDir;
            }
        }
        public bool IsOpen
        {
            get
            {
                return _isOpen;
            }
        }
        public string Filename
        {
            get
            {
                return _filename;
            }
        }

        // ------------------------------------------------------------------- //

        public bool Open()
        {
            _zip = new ZipFile(_filename);
            if (_zip == null)
            {
                _isOpen = false;
                return false;
            }

            ReadZip();
            _isOpen = true;
            return true;
        }
        public void Close()
        {
            if (_isOpen)
            {
                _zip.Close();
            }
            _isOpen = false;
        }
        public bool MoveUp()
        {
            if (_currPath == "/")
            {
                return false;
            }
            return SetCurrentDir(UpperPath(_currPath));
        }
        public bool MoveTo(string path)
        {
            if (!IsValidPath(path)) return false;
            return SetCurrentDir(path);
        }
        public string[] GetAllDirs()
        {
            //StringCollection sc = new StringCollection();
            //sc.Add("/");
            //foreach (ZipEntry ze in _zip)
            //{
            //    if (ze.IsDirectory)
            //    {
            //        sc.Add("/" + ze.Name);
            //    }
            //}
            //sc.Sort();
            //return sc.GetItems();

            return _dirs.GetItems();
        }
        public string[] GetAllFiles()
        {
            //StringCollection sc = new StringCollection();
            //foreach (ZipEntry ze in _zip)
            //{
            //    if (ze.IsFile)
            //    {
            //        sc.Add("/" + ze.Name);
            //    }
            //}
            //sc.Sort();
            //return sc.GetItems();

            return _files.GetItems();
        }
        public ZipHelperDir GetDir(string path)
        {
            if (!IsValidPath(path)) return null;

            return new ZipHelperDir(this, path);
        }
        public ZipHelperFile GetFile(string path)
        {
            if (!IsValidPath(path)) return null;

            return new ZipHelperFile(this, path);
        }
        public string[] GetDirs()
        {
            return GetDirs("/");
        }
        public string[] GetFiles()
        {
            return GetFiles("/");
        }
        public string[] GetDirs(string path)
        {
            if (!IsValidPath(path)) return new string[0];

            int pathsCount = path.Split('/').Length;
            if (path == "/") pathsCount = 1;

            StringCollection result = new StringCollection();
            for (int i = 0; i < _dirs.Count; i++)
            {
                if (_dirs[i].StartsWith(path) && (_dirs[i].Split('/').Length == (pathsCount + 1)) && (_dirs[i] != "/"))
                {
                    result.Add(_dirs[i]);
                }
            }
            return result.GetItems();
        }
        public string[] GetFiles(string path)
        {
            if (!IsValidPath(path)) return new string[0];

            int pathsCount = path.Split('/').Length;
            if (path == "/") pathsCount = 1;

            StringCollection result = new StringCollection();
            for (int i = 0; i < _files.Count; i++)
            {
                if (_files[i].StartsWith(path) && _files[i].Split('/').Length == (pathsCount + 1))
                {
                    result.Add(_files[i]);
                }
            }
            return result.GetItems();
        }
        public void Extract(string path)
        {
            ZipInputStream s = new ZipInputStream(File.OpenRead(_filename));
            ZipEntry ze;

            while ((ze = s.GetNextEntry()) != null)
            {
                string dirName = Path.GetDirectoryName(ze.Name);
                string fileName = Path.GetFileName(ze.Name);

                if (ze.IsFile)
                {
                    if (!Directory.Exists(path + "\\" + dirName))
                    {
                        Directory.CreateDirectory(path + "\\" + dirName);
                    }

                    FileStream streamWriter = File.Create(path + "\\" + dirName + "\\" + fileName);

                    if (ze.Size > 0)
                    {
                        int size = 2048;
                        byte[] data = new byte[size];

                        while (true)
                        {
                            size = s.Read(data, 0, data.Length);
                            if (size > 0)
                            {
                                streamWriter.Write(data, 0, size);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    streamWriter.Close();
                }
            }

            s.Close();
        }

        // ------------------------------------------------------------------- //

        private void ReadZip()
        {
            _dirs = new StringCollection();
            _dirs.Add("/");

            _files = new StringCollection();

            foreach(ZipEntry ze in _zip)
            {
                if (ze.IsDirectory)
                {
                    _dirs.Add("/" + ze.Name.TrimEnd('/'));
                }
                else if (ze.IsFile)
                {
                    _files.Add("/" + ze.Name.TrimEnd('/'));
                }
            }

            _dirs.Sort();
            _files.Sort();
        }
        private string FormatToInternalPath(string path)
        {
            path = path.Replace('\\', '/');
            path = path.TrimStart('/');
            path = path.TrimEnd('/');
            return path;
        }

        // ------------------------------------------------------------------- //

        internal string UpperPath(string path)
        {
            path = path.Replace("\\", "/");
            path = path.TrimEnd('/');
            string[] names = path.Split('/');
            StringBuilder sb = new StringBuilder();
            for (int i = 1; i < names.Length - 1; i++)
            {
                sb.Append("/");
                sb.Append(names[i]);
            }

            if (sb.Length == 0) sb.Append("/");
            return sb.ToString();
        }
        internal bool IsValidPath(string path)
        {
            return _dirs.Contains(path) || _files.Contains(path);
        }
        internal string ExtractName(string path)
        {
            path = path.Replace("\\", "/");
            if (path == "/") return string.Empty;
            path = path.TrimEnd('/');
            string[] names = path.Split('/');
            return names[names.Length - 1];
        }
        internal bool SetCurrentDir(string path)
        {
            _currPath = path;
            _currDir = new ZipHelperDir(this, _currPath);
            return true;
        }
        internal ZipEntry GetZipEntry(string path)
        {
            path = FormatToInternalPath(path);
            if (path == "" || path == "/") return null;
            return _zip.GetEntry(path);
        }

        // ------------------------------------------------------------------- //

        //public bool MoveNext()
        //{
        //    throw new NotImplementedException();
        //}
        //public bool MoveToFirstChildDir()
        //{
        //    throw new NotImplementedException();
        //}
        //public bool MakeDir(string name)
        //{
        //    throw new NotImplementedException();
        //}
    }
}
