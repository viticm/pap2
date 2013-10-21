/////////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : UpdateManager
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 更新管理,用于上传更新文件到相应的更新服务器等功能
//
/////////////////////////////////////////////////////////////////////////////

using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using FSEye.DAL;
using FSEye;
using System.IO;
using System.Net;
using FSEye.Security;

namespace FSEye
{
    public class FTPClient
    {
        IList _FTPServerList;

        public const string SecurityObjectSystem = "System";
        public const string SecurityObjectUpdateManagement = "UpdateManagement";
        public const string SecurityObjectUpdateServer = "UpdateServer";
        
        public IList FTPServerList
        {
            get{return _FTPServerList;}
        }

        public FTPClient()
        {
            _FTPServerList = new ArrayList();
        }

        public bool Startup()
        {
            try
            {
                LoadUpdateServer();
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }

        public bool Shutdown()
        {
            try
            {
                foreach (FTPServer server in _FTPServerList)
                {
                    server.Close();
                }
                _FTPServerList.Clear();
                return true;
            }
            catch (Exception e)
            {
                throw e;
            }
        }

        public void Dispose()
        {            
            _FTPServerList.Clear();
        }

        public bool LoadUpdateServer()
        {
            lock (this)
            {
                IBlazeDatabase db = null;
                try
                {
                    db = DbFactory.GetDatabase();
                    IBlazeTable table = db.GetTable(TableString.UpdateServerTableName);
                                        
                    DataSet data = new DataSet();
                    table.Get(data);
                    DataRowCollection rows = data.Tables[TableString.UpdateServerTableName].Rows;
                    foreach (DataRow row in rows)
                    {                        
                        int id = (int)row[TableString.UpdateServerFieldId];
                        string address = row[TableString.UpdateServerFieldAddress] as string;
                        int port = (int)row[TableString.UpdateServerFieldPort];
                        string username = row[TableString.UpdateServerFieldUsername] as string;
                        string password = row[TableString.UpdateServerFieldPassword] as string;
                        string literal = row[TableString.UpdateServerFieldLiteral] as string;
                        FTPServer.FTPServerType type = (FTPServer.FTPServerType)Enum.Parse(typeof(FTPServer.FTPServerType), row[TableString.UpdateServerFieldUpdateServerType].ToString());
                        FTPServer server = new FTPServer(id, address, port, username, password, literal, type);
                        server.SecurityObject = AdminServer.TheInstance.SecurityManager.Get(id);
                        _FTPServerList.Add(server);
                    }
                }
                catch (Exception)
                {
                    return false;
                }
                finally
                {
                    if (db != null)
                    {
                        db.Close();
                    }
                }
            }
            return true;
        }
        
        #region 数据库操作
        public bool AddUpdateServer(FTPServer server)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                db.BeginTrans();

                SecurityManager sm = AdminServer.TheInstance.SecurityManager;

                //构造相关安全对象
                string newUpdateServerObjPath = sm.Root.FullName + SecurityManager.ObjectPathDelimiter + SecurityObjectSystem + SecurityManager.ObjectPathDelimiter + SecurityObjectUpdateManagement + SecurityManager.ObjectPathDelimiter
                        + SecurityObjectUpdateServer + SecurityManager.ObjectPathDelimiter + server.Literal;
                FSEyeObject newUpdateServerObj = sm.Get(newUpdateServerObjPath, db);
                sm.Set(newUpdateServerObj);

                AddFatherAce(newUpdateServerObj.Parent, newUpdateServerObj, true);

                //IBlazeTable table = db.GetTable(TableString.UpdateServerTableName);
                //DataSet taskData = new DataSet();
                //table.Get(taskData);
                //DataTable taskTable = taskData.Tables[TableString.UpdateServerTableName];

                //DataRow addRow = taskTable.Rows.Add(newUpdateServerObj.Id,
                //                                    server.Address,
                //                                    server.Port,
                //                                    server.UserName,
                //                                    server.Password,
                //                                    server.Literal);
                //table.Set(taskTable);

                //插入数据到服务器表
                string[] fields = new string[] { 
                    TableString.UpdateServerFieldId,
                    TableString.UpdateServerFieldAddress,
                    TableString.UpdateServerFieldPort,
                    TableString.UpdateServerFieldUsername,
                    TableString.UpdateServerFieldPassword,
                    TableString.UpdateServerFieldLiteral,
                    TableString.UpdateServerFieldUpdateServerType
                };
                object[] values = new object[] {
                    newUpdateServerObj.Id,
                    server.Address, 
                    server.Port,
                    server.UserName,
                    server.Password,
                    server.Literal,
                    (Int16)server.FtpServerType
                };

                IBlazeTable serverTable = db.GetTable(TableString.UpdateServerTableName);
                serverTable.Add(fields, values);

                db.CommitTrans();

                server.SecurityObject = newUpdateServerObj;
                this._FTPServerList.Add(server);

                return true;
            }
            catch (Exception ex)
            {
                Util.DebugLog(ex.ToString());

                if (db != null)
                    db.RollbackTrans();
            }
            finally
            {
                if (db != null)
                    db.Close();
            }
            return false;            
        }

        /// <summary>
        /// 为新增UpdateServer设置权限时默认设置为其直接父亲的权限
        /// </summary>
        /// <param name="srcObj">源</param>
        /// <param name="targetObj">目标</param>
        /// <param name="overWrite">是否覆盖(由AccessControlEntry的AddAce方法决定)</param>
        void AddFatherAce(FSEyeObject srcObj, FSEyeObject targetObj, bool overWrite)
        {
            foreach (AccessControlEntry ace in srcObj.Aces)
            {
                targetObj.AddAce(ace, true);
            }
            AdminServer.TheInstance.SecurityManager.Set(targetObj);
            foreach (FSEyeObject obj in targetObj.Children)
            {
                AddFatherAce(srcObj, obj, overWrite);
            }
        }


        public bool EditUpdateServer(FTPServer server)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                db.BeginTrans();

                IBlazeTable table = db.GetTable(TableString.UpdateServerTableName);
                DataSet taskData = new DataSet();
                table.Get(taskData);
                DataTable taskTable = taskData.Tables[TableString.UpdateServerTableName];

                foreach (DataRow row in taskTable.Rows)
                {
                    if ((int)row[TableString.UpdateServerFieldId] == server.ID)
                    {
                        row.BeginEdit();
                        row[TableString.UpdateServerFieldAddress] = server.Address;
                        row[TableString.UpdateServerFieldPort] = server.Port;
                        row[TableString.UpdateServerFieldUsername] = server.UserName;
                        row[TableString.UpdateServerFieldPassword] = server.Password;
                        row[TableString.UpdateServerFieldLiteral] = server.Literal;
                        row.EndEdit();
                        break;
                    }
                }
                table.Set(taskTable);
                db.CommitTrans();

                return true;
            }
            catch (Exception)
            {
                if (db != null)
                    db.RollbackTrans();
            }
            finally
            {
                if (db != null)
                    db.Close();
            }
            return false; 
        }

        public bool DeleteUpdateServer(int serverId)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                db.BeginTrans();

                IBlazeTable table = db.GetTable(TableString.UpdateServerTableName);
                DataSet taskData = new DataSet();
                table.Get(taskData);
                DataTable taskTable = taskData.Tables[TableString.UpdateServerTableName];

                foreach (DataRow row in taskTable.Rows)
                {
                    if ((int)row[TableString.UpdateServerFieldId] == serverId)
                    {
                        row.Delete();
                        break;
                    }
                }
                table.Set(taskTable);

                AdminServer.TheInstance.SecurityManager.Delete(serverId, db);

                db.CommitTrans();

                this._FTPServerList.Remove(GetFTPServer(serverId));

                return true;
            }
            catch (Exception)
            {
                if (db != null)
                    db.RollbackTrans();
            }
            finally
            {
                if (db != null)
                    db.Close();
            }
            return false;

        }

        public bool DeleteUpdateServer(FTPServer server)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                db.BeginTrans();

                IBlazeTable table = db.GetTable(TableString.UpdateServerTableName);
                DataSet taskData = new DataSet();
                table.Get(taskData);
                DataTable taskTable = taskData.Tables[TableString.UpdateServerTableName];

                foreach (DataRow row in taskTable.Rows)
                {
                    if ((int)row[TableString.UpdateServerFieldId] == server.ID)
                    {
                        row.Delete();
                        break;
                    }
                }
                table.Set(taskTable);

                AdminServer.TheInstance.SecurityManager.Delete(server.ID, db);

                db.CommitTrans();

                this._FTPServerList.Remove(server);

                return true;
            }
            catch (Exception)
            {
                if (db != null)
                    db.RollbackTrans();
            }
            finally
            {
                if (db != null)
                    db.Close();
            }
            return false;

        }
        #endregion

        public FTPServer GetFTPServer(int serverId)
        {
            foreach(FTPServer server in _FTPServerList)
            {
                if (server.ID == serverId) return server;
            }
            return null;
        }        
    }

    /// <summary>
    /// FTPServer类,保存相关信息
    /// </summary>
    [Serializable]
    public class FTPServer:ISecurity, IDisposable
    {
        /// <summary>
        /// FTP服务器类型，对FTPServer进行分类
        /// </summary>
        public enum FTPServerType
        {
            ServerUpdate = 1,       //用于服务器端程序的更新
            ClientUpdate,           //用于客户端程序的更新
            ClientServerListFile    //用于客户端服务器列表文件的更新
        }
        
        int _id;
        int _port;
        string _address;
        string _username;
        string _password;
        string _literal;
        FTPServerType _ftpservertype;
        public string FTP_URL_PREFIX;

        FSEyeObject _securityObject;

        IList _uploadFiles;
        /// <summary>
        /// 保存上传文件状态 key为FileInfo,value为bool型指示是否覆盖(overcast)
        /// </summary>
        Hashtable _uploadFilesStateTable;
        IList _serverFiles;
        
        //本批任务中已上传的文件
        IList _uploadedFiles;
        //保存上传文件状态 key为FileInfo,value为bool型指示是否覆盖(overcast)
        Hashtable _uploadedFilesStateTable;

        string _result;
        long _currLength;
        bool _abort;
        bool _listFileRequestFinished;
        bool _deleteFileFinished;

        FtpWebRequest _uploadRequest;
        FtpWebResponse _uploadResponse;
        //FtpWebRequest _listRequest;
        //FtpWebRequest _deleteRequest;
        FileStream _fileStream;
        Stream _requestStream;

        string _currentPath = string.Empty;

        public string CurrentPath
        {
            get { return _currentPath; }
        }

        public FSEyeObject SecurityObject
		{
			get { return _securityObject; }
			set { _securityObject = value; }
		}        

        public int ID
        {
            set { _securityObject.Id = value; }
            get { return _securityObject.Id; }
        }

        public string Address
        {
            set { _address = value; }
            get { return _address; }
        }

        public string UserName
        {
            set { _username = value; }
            get { return _username; }
        }

        public string Password
        {
            set { _password = value; }
            get { return _password; }
        }

        public int Port
        {
            set { _port = value; }
            get { return _port; }
        }
        
        public string Literal
        {
            set { _literal = value; }
            get { return _literal; }
        }

        public FTPServerType FtpServerType
        {
            set { _ftpservertype = value; }
            get { return _ftpservertype; }
        }

        public IList UploadFiles
        {
            //set { _uploadFiles = value; }
            get { return _uploadFiles; }
        }

        public string Result
        {
            set { _result = value; }
            get { return _result; }
        }

        public IList ServerFiles
        {
            set { _serverFiles = value; }
            get { return _serverFiles; }
        }

        public IList UploadedFiles
        {
            set { _uploadedFiles = value; }
            get { return _uploadedFiles; }
        }

        public double CurrentProgress
        {
            get 
            {
                if (_uploadFiles.Count != 0)
                {
                    FileInfo file = _uploadFiles[0] as FileInfo;
                    return (double)_currLength / (double)file.Length;
                }
                else return 0;
            }        
        }

        public bool Abort
        {
            set { _abort = value; }
            get { return _abort; }
        }

        public Hashtable UploadFilesStateTable
        {
            get { return _uploadFilesStateTable; }
        }

        public Hashtable UploadedFilesStateTable
        {
            get { return _uploadedFilesStateTable; }
        }

        public string RootUri
        {
            get { return FTP_URL_PREFIX; }
        }

        public FTPServer(int id, string address,int port, string username, string password,string literal, FTPServerType type)
        {
            _id = id;
            _address = address;
            _port = port;
            _username = username;
            _password = password;
            _literal = literal;
            _ftpservertype = type;
            Init();
        }

        /// <summary>
        /// 没有ID的构造函数,用于添加操作
        /// </summary>
        /// <param name="address"></param>
        /// <param name="port"></param>
        /// <param name="username"></param>
        /// <param name="password"></param>
        /// <param name="literal"></param>
        public FTPServer(string address, int port, string username, string password, string literal, FTPServerType type)
        {
            _address = address;
            _port = port;
            _username = username;
            _password = password;
            _literal = literal;
            _ftpservertype = type;
            Init();            
        }

        private void Init()
        {
            _uploadFiles = new ArrayList();
            _serverFiles = new ArrayList();
            _uploadedFiles = new ArrayList();
            _uploadFilesStateTable = new Hashtable();
            _uploadedFilesStateTable = new Hashtable();
            _uploadResponse = null;
            _uploadRequest = null;
            _fileStream = null;
            _requestStream = null;
            //_listRequest = null;
            _currLength = 0;
            _abort = false;
            _listFileRequestFinished = true;
            _deleteFileFinished = true;
            _result = string.Empty;
            FTP_URL_PREFIX = @"ftp://" + this.Address + ":" + this.Port + "/";
        }

        public void Dispose()
        {
        }

        public void AddToUploadFileList(IList fileList, bool overWrite)
        {
            foreach (FileInfo fileToAdd in fileList)
            {
                int index = 0;
                for (; index != _uploadFiles.Count; ++index)
                {
                    FileInfo file = _uploadFiles[index] as FileInfo;
                    if (file.Name.Equals(fileToAdd.Name)) break;
                }
                if (index == _uploadFiles.Count)
                {
                    _uploadFiles.Add(fileToAdd);
                    _uploadFilesStateTable.Add(fileToAdd, overWrite);
                    _uploadedFilesStateTable.Add(fileToAdd, overWrite);
                }
            }
        }

        /// <summary>
        /// 列出根目录信息
        /// </summary>
        public void ListFile()
        {
            ListFtpFile("");
        }


        class ListFileAsyncState
        {
            public FtpWebRequest Request = null;
            public string Path = string.Empty;
        }


        /// <summary>
        /// 读取特定目录信息
        /// </summary>
        /// <param name="strPath"></param>
        public void ListFile(string strPath)
        {
            ListFtpFile(strPath);
        }

        private void ListFtpFile(string strPath)
        {
            FtpWebRequest _listRequest = null;
            try
            {
                if (!(_listFileRequestFinished && _deleteFileFinished)) return;
                _listRequest = (FtpWebRequest)WebRequest.Create(FTP_URL_PREFIX + strPath + @"/");
                _listRequest.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                _listRequest.Credentials = new NetworkCredential(this.UserName, this.Password);
                _listRequest.Proxy = null;
                _listRequest.KeepAlive = false;
                _listFileRequestFinished = false;

                ListFileAsyncState stateObj = new ListFileAsyncState();
                stateObj.Request = _listRequest;
                stateObj.Path = strPath;

                _listRequest.BeginGetResponse(ListFinish, stateObj);
            }
            catch (Exception ex)
            {
                _result = ex.Message;
            }
        }

        public void ListFinish(IAsyncResult ar)
        {
            FtpWebResponse response = null;

            try
            {
                ListFileAsyncState stateObj = ar.AsyncState as ListFileAsyncState;
                FtpWebRequest request = stateObj.Request;
                _currentPath = stateObj.Path;
                response = (FtpWebResponse)request.EndGetResponse(ar);
                _result = response.StatusDescription;
                ServerFiles = ServerFileInfo.Parse(response.GetResponseStream());
            }
            catch (Exception e)
            {
                if (response != null)
                    _result = response.StatusDescription;
                else
                {
                    _result = e.Message;
                }
            }
            finally
            {   
                if (response != null) response.Close();
                _listFileRequestFinished = true;
            }
        }

        public bool UploadFile(IList files)
        {
            try
            {
                //区分第一次
                if (files != null) _uploadFiles = files;
                _abort = false;
                if (_uploadFiles.Count != 0)
                {
                    FileInfo file = _uploadFiles[0] as FileInfo;                    
                    //把'#'替换为'_'
                    ServerFileInfo serverFile = SearchFile(file.Name.Replace('#','_'));
                    if (serverFile != null) _currLength = serverFile.Size;
                    else _currLength = 0;
                    string uploadUrl = FTP_URL_PREFIX + file.Name.Replace('#', '_');
                    _uploadRequest = (FtpWebRequest)WebRequest.Create(uploadUrl);
                    if ((bool)_uploadFilesStateTable[file])
                    {//覆盖
                        _uploadRequest.Method = WebRequestMethods.Ftp.UploadFile;
                        _currLength = 0;
                    }
                    else _uploadRequest.Method = _currLength == 0 ? WebRequestMethods.Ftp.UploadFile : WebRequestMethods.Ftp.AppendFile; 
                    _uploadRequest.Credentials = new NetworkCredential(this.UserName, this.Password);
                    _uploadRequest.Proxy = null;
                    _uploadRequest.KeepAlive = false;
                    _uploadRequest.BeginGetRequestStream(new AsyncCallback(TransFile), file);
                }
                return true;
            }
            catch (Exception e)
            {
                _result = e.Message;
                return false;
            }
        }

        public void TransFile(IAsyncResult ar)
        {
            FileInfo file = ar.AsyncState as FileInfo;
            try
            {
                _requestStream = _uploadRequest.EndGetRequestStream(ar);                
                _fileStream = File.OpenRead(file.FullName);
                byte[] buffer = new byte[1024];
                int bytesRead;                
                if (_currLength < file.Length)
                {
                    //设置Position
                    _fileStream.Seek(_currLength,SeekOrigin.Begin);                    
                    while (true)
                    {
                        if (_abort)
                        {
                            _uploadRequest.Abort();
                            break;
                        }
                        bytesRead = _fileStream.Read(buffer, 0, buffer.Length);
                        if (bytesRead == 0) break;
                        _requestStream.Write(buffer, 0, bytesRead);
                        _currLength += bytesRead;
                    }
                }
                RomoveFile(file);

                //添加到已完成列表中
                if (!_abort)
                    _uploadedFiles.Add(file);

                if (!_abort) _uploadRequest.BeginGetResponse(new AsyncCallback(UploadNextFile), null);
                else UploadFile(null);
            }
            catch (Exception e)
            {
                _result = e.Message;
                RomoveFile(file);
                UploadFile(null);
            }
        }

        private void UploadNextFile(IAsyncResult ar)
        {
            //清空
            _currLength = 0;
            _abort = false;
            if (_uploadResponse != null) _uploadResponse.Close();
            if (_fileStream != null) _fileStream.Close();
            if (_requestStream != null) _requestStream.Close();
            _uploadResponse = (FtpWebResponse)_uploadRequest.EndGetResponse(ar);
            //_result = "文件上传成功";
            try
            {
                if (_uploadFiles.Count != 0)
                {
                    FileInfo file = _uploadFiles[0] as FileInfo;
                    ServerFileInfo serverFile = SearchFile(file.Name.Replace('#', '_'));
                    if (serverFile != null) _currLength = serverFile.Size;
                    else _currLength = 0;
                    string uploadUrl = @"ftp://" + this.Address + ":" + this.Port + "/" + file.Name.Replace('#', '_');
                    _uploadRequest = (FtpWebRequest)WebRequest.Create(uploadUrl);
                    if ((bool)_uploadFilesStateTable[file])
                    {//覆盖
                        _uploadRequest.Method = WebRequestMethods.Ftp.UploadFile;
                        _currLength = 0;
                    }
                    else _uploadRequest.Method = _currLength == 0 ? WebRequestMethods.Ftp.UploadFile : WebRequestMethods.Ftp.AppendFile; 
                    _uploadRequest.Credentials = new NetworkCredential(this.UserName, this.Password);
                    _uploadRequest.Proxy = null;
                    _uploadRequest.KeepAlive = false;
                    _uploadRequest.BeginGetRequestStream(new AsyncCallback(TransFile), file);
                }
            }
            catch (Exception e)
            {
                _result = e.Message;
            }
        }

        /// <summary>
        /// 删除服务器上的文件
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns></returns>
        public bool DeleteFile(string fileName)
        {
            try
            {
                _deleteFileFinished = false;
                string deleteUri = FTP_URL_PREFIX + fileName;
                if (fileName.IndexOf('#') != -1) throw new Exception();
                FtpWebRequest _deleteRequest = (FtpWebRequest)WebRequest.Create(deleteUri);
                _deleteRequest.Method = WebRequestMethods.Ftp.DeleteFile;
                _deleteRequest.Credentials = new NetworkCredential(this.UserName, this.Password);
                _deleteRequest.BeginGetResponse(new AsyncCallback(DeleteFinish), _deleteRequest);
                return true;
            }
            catch (Exception e)
            {
                _result = e.Message;
                _deleteFileFinished = true;
                return false;
            }
        }
        private void DeleteFinish(IAsyncResult ar)
        {
            _deleteFileFinished = true;
            FtpWebRequest _deleteRequest = ar.AsyncState as FtpWebRequest;
            try
            {
                FtpWebResponse response = (FtpWebResponse)_deleteRequest.EndGetResponse(ar);
                _result = response.StatusDescription;
                response.Close();
            }
            catch (Exception e)
            {
                _result = e.Message;
            }
        }

        /// <summary>
        /// 删除目录
        /// </summary>
        /// <param name="dirName">目录名(含路径)</param>
        /// <returns></returns>
        public bool RemoveDir(string url)
        {
            try
            {
                _deleteFileFinished = false;
                FtpWebRequest request = (FtpWebRequest)WebRequest.Create(url);
                request.Credentials = new NetworkCredential(this.UserName, this.Password);
                request.Method = WebRequestMethods.Ftp.ListDirectoryDetails;
                FtpWebResponse response = request.GetResponse() as FtpWebResponse;
                IList fileList = ServerFileInfo.Parse(response.GetResponseStream());
                foreach (ServerFileInfo file in fileList)
                {
                    if (file.Name.Equals(".") || file.Name.Equals("..")) continue;
                    if (file.Directory)
                    {
                        RemoveDir(url + file.Name + "/");
                    }
                    else
                    {
                        if (file.Name.IndexOf('#') != -1) throw new Exception();
                        FtpWebRequest delete = (FtpWebRequest)WebRequest.Create(url + file.Name);
                        delete.Credentials = new NetworkCredential(this.UserName, this.Password);
                        delete.Method = WebRequestMethods.Ftp.DeleteFile;
                        Console.WriteLine(((FtpWebResponse)delete.GetResponse()).StatusDescription);
                    }
                }
                request = (FtpWebRequest)WebRequest.Create(url.Substring(0, url.Length - 1));
                request.Credentials = new NetworkCredential(this.UserName, this.Password);
                request.Method = WebRequestMethods.Ftp.RemoveDirectory;
                request.KeepAlive = false;
                response = request.GetResponse() as FtpWebResponse;
                this.Result = response.StatusDescription;
                response.Close();
            }
            catch (Exception e)
            {
                this.Result = e.Message;
                return false;
            }
            finally
            {
                _deleteFileFinished = true;
            }
            return true;
        }
        
        private void RomoveFile(FileInfo file)
        {
            _uploadFiles.Remove(file);
            _uploadFilesStateTable.Remove(file);
        }

        public double GetFileProcess(FileInfo uploadFile)
        {
            ServerFileInfo serverFile = SearchFile(uploadFile.Name);
            if (serverFile != null)
            {
                return (double)serverFile.Size / (double)uploadFile.Length;
            }
            else
            {
                return 0;
            }
        }

        /// <summary>
        /// 找一下上传文件是否在服务器上
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns></returns>
        public ServerFileInfo SearchFile(string fileName)
        {
            foreach (ServerFileInfo file in this.ServerFiles)
            {
                if(fileName.Equals(file.Name))return file;
            }
            return null;
        }

        public void Close()
        {
            if (_uploadRequest != null) _uploadRequest.Abort();
            if (_uploadResponse != null) _uploadResponse.Close();
            //if (_listRequest != null) _listRequest.Abort();
            //if (_deleteRequest != null) _deleteRequest.Abort();
            _uploadFiles.Clear();
            _uploadFilesStateTable.Clear();
            _uploadedFilesStateTable.Clear();
            _uploadedFiles.Clear();
            _result = string.Empty;
        }

        public ServerFileInfo GetServerFileInfo(string file)
        {
            foreach(ServerFileInfo serverFile in _serverFiles)
            {
                if (serverFile.Name.Equals(file)) return serverFile;
            }
            return null;
        }
    }
    

    /// <summary>
    /// 主要是解析list命令传来的stream为文件信息
    /// </summary>
    public class ServerFileInfo
    {
        private string _name;
        private long _size;
        private string _owner;
        private string _modifyTime;
        private bool _directory;
        private string _attribute;

        #region 属性
        public string Name
        {
            set { _name = value; }
            get { return _name; }
        }

        public long Size
        {
            set { _size = value; }
            get { return _size; }
        }

        public string Owner
        {
            set { _owner = value; }
            get { return _owner; }
        }

        public string ModifyTime
        {
            set { _modifyTime = value; }
            get { return _modifyTime; }
        }

        public bool Directory
        {
            set { _directory = value; }
            get { return _directory; }
        }

        public string Attribute
        {
            set { _attribute = value; }
            get { return _attribute; }
        }
        #endregion

        public ServerFileInfo(string name, long size, string owner, string modifyTime, bool directory,string attribute)
        {
            _name = name;
            _size = size;
            _owner = owner;
            _modifyTime = modifyTime;
            _directory = directory;
            _attribute = attribute;
        }

        public static IList Parse(Stream listCommand)
        {
            StreamReader reader = null;
            ArrayList files;
            try
            {
                reader = new StreamReader(listCommand, Encoding.Default);
                char[] num = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
                files = new ArrayList();
                string fileInfo;
                while ((fileInfo = reader.ReadLine()) != null)
                {
                    bool directory = fileInfo[0].Equals('d') ? true : false;
                    string attribute = fileInfo.Substring(0, 10);
                    string group = fileInfo.Substring(fileInfo.IndexOf(' ') + 3, fileInfo.IndexOf("group") - fileInfo.IndexOf(' ') + 2);
                    string owner = group.Split(' ')[1];
                    int sizeStart = fileInfo.IndexOfAny(num, fileInfo.IndexOf("group"));
                    int sizeEnd = sizeStart;
                    while (!fileInfo[sizeEnd].Equals(' ')) ++sizeEnd;
                    long size = long.Parse(fileInfo.Substring(sizeStart, sizeEnd - sizeStart));
                    string dateStr = fileInfo.Substring(sizeEnd + 1, 12);
                    //DateTime date = DateTime.Parse(dateStr);
                    string name = fileInfo.Substring(sizeEnd + 14);

                    //把.和..过滤了
                    if (name.Equals(".") || name.Equals(".."))
                    {
                        continue;
                    }

                    files.Add(new ServerFileInfo(name, size, owner, dateStr, directory, attribute));
                }
            }
            catch (Exception e)
            {                
                throw new Exception("流解析错误"+e.Message);
            }
            finally
            {
                if (reader != null) reader.Close();
            }
            return files;
        }

    }
}
