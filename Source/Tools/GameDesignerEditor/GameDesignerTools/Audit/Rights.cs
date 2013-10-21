using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace GameDesingerTools
{
    static class RightsSettings
    {
        static public bool AllowSave = true;
        static public bool AllowModifyValue = true;
        static public bool AllowModifyTree = true;
        static public bool AllowModifyScript = true;
    };

    class RightManager
    {
        public string _hostname;
        public string _ip;
        public string _username;
        public string _psw;
        public string _userindex;

        public void Init()
        {
            try
            {
                StringBuilder sb = new StringBuilder(255);
                FileFolderHelper.ReadIniFile("Rights", "AllowSave", "1", sb, 255, Application.StartupPath + "/GameDesingerTools.ini");
                RightsSettings.AllowSave = Convert.ToInt32(sb.ToString()) == 1 ? true : false;
                RightsSettings.AllowModifyValue = RightsSettings.AllowSave;
                RightsSettings.AllowModifyTree = RightsSettings.AllowSave;
                RightsSettings.AllowModifyScript = RightsSettings.AllowSave;

                CheckACL();
            }
            catch (Exception ex)
            {
                string strExtMsg = "附加信息:\n";
                string strHostName = Helper.GetHostName().ToLower();
                strExtMsg += "机器名:" + strHostName + "\n";
                strExtMsg += "IP地址:";
                System.Net.IPAddress[] ips = Helper.GetIPAddresses();
                foreach (System.Net.IPAddress ip in ips)
                {
                    string strip = ip.ToString();
                    strExtMsg += strip + "\n";
                }

                throw new Exception("权限系统初始化失败！\n" + strExtMsg + "\r\n错误信息为：\r\n" + ex.Message);
            }
        }
        private void CheckClientVer()
        {
            //检查DUMMY表，仅用于强行使用户不能使用旧版本的客户端程序
            /*System.Data.DataTable tbldummy = CustomClassCache.GetDataTableWithKey("select * from sys_dummy_1");*/
            System.Data.DataTable tbldummy = CustomClassCache.GetDataTableWithKeyProxy("sys_dummy_1");
            tbldummy.Dispose();
        }

        private void CheckACL()
        {
            //if (MainForm.conn.Database.ToString().ToLower() == "s3design" || MainForm.conn.Database.ToString().ToLower() == "s3design_debug")
            //{
            //    return; //不检查调试库中的内容
            //}
            
            CheckClientVer();

            //检查是否用户访问列表中的用户，否则拒绝进入
            string strHost = Helper.GetHostName().ToLower();
            string _sql = string.Format("SELECT * FROM sys_useracl WHERE userid = '{0}'", strHost);
            /*System.Data.DataTable tbl = CustomClassCache.GetDataTableWithKey(_sql);*/
            string filter = string.Format("userid = '{0}'", strHost);
            System.Data.DataTable tbl = CustomClassCache.GetDataTableWithKeyProxy("sys_useracl", filter, null);

            //System.Data.DataRow row = null;
            bool bFind = false;
            System.Net.IPAddress[] ips = Helper.GetIPAddresses();
            foreach (System.Data.DataRow r in tbl.Rows)
            {
                foreach (System.Net.IPAddress ip in ips)
                {
                    if (r["ip"].ToString().Trim() == ip.ToString())
                    {
                        bFind = true;

                        // 更新信息
                        _hostname = Helper.GetHostName();
                        _ip = Helper.GetIPAddress();
                        _userindex = r["_index"].ToString().Trim();
                        //todo: 以后要从本地ini取
                        _username = string.Empty;
                        _psw = string.Empty;

                        break;
                    }
                }
            }
            if (!bFind)
                throw new Exception("无权访问！");
        }
        public void Uninit()
        {

        }
    };
}
