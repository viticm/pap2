using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using DataTableRemotingCommon;
using DataTableRemoting;

namespace DataTableRemoting.RemotingInterface
{
    public class MonitorRemoting : MarshalByRefObject, IMonitorRemoting
    {
        public delegate object CommandHandler(string[] args);

        private Dictionary<string, CommandHandler> mCommandHandlers;

        public MonitorRemoting()
        {
            mCommandHandlers = new Dictionary<string, CommandHandler>();
            mCommandHandlers["savealltable"] = SaveAndReleaseAllTable;
            mCommandHandlers["closeserver"] = CloseServer;
        }

        public string[] GetDBNames()
        {
            string[] result = TableHelper.GetDBNames();
            return result;
        }

        public string[] GetTablenames(string dbname)
        {

            string[] result = null;
            TableUnitManager tm = TableHelper.GetTableUnitManangerByDbname(dbname);
            if (tm == null)
            {
                result = new string[0];
            }
            else
            {
                result = tm.GetTableNames();
            }
            return result;
        }

        public int GetTableRowCount(string tblname, string dbname)
        {
            int nRet = 0;
            TableUnitManager tm = TableHelper.GetTableUnitManangerByDbname(dbname);
            if (tm != null)
            {
                bool bRet = tm.ExistTable(tblname);
                if (bRet)
                {
                    TableUnit unit = tm.GetTableUnit(tblname);
                    DataTable table = unit.GetTable();
                    nRet = table.Rows.Count;
                }
            }
            return nRet;
        }

        public string[] GetNetClientKeys()
        {
            string[] result = NetClientManager.GetClientKeys();
            return result;
        }

        public NetClient GetNetClient(string clientkey)
        {
            NetClient client = NetClientManager.GetNetClient(clientkey);
            return client;
        }


        public object DoCommand(string[] args)
        {
            object ret = null;
            if (mCommandHandlers.ContainsKey(args[0]))
            {
                CommandHandler command = mCommandHandlers[args[0].ToLower()];
                ret = command(args);
            }
            return ret;
        }


        private object SaveAndReleaseAllTable(string[] args)
        {
            int count = TableHelper.SaveAndReleaseAllTable();
            return count;
        }

        private object CloseServer(string[] args)
        {
            DataTableServer.RunServer = false;
            string message = string.Format("Close Server!");
            return message;
        }
        
        public override object InitializeLifetimeService()
        {
            return null;
        }
    }
}
