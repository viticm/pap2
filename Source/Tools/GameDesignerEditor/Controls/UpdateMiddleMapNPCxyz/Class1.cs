using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;


//创建时间：2009-1-21


namespace UpdateMiddleMapNPCxyz
{
    //读取客户端的地图logical文件
    //更新表middlemap_npc里的字段middlemap,x,y,z 
    public class TableUpdater
    {
        public string Update(string rootdir, System.Data.SqlClient.SqlConnection conn)
        {
            UpdateMiddleMapNpc updater = UpdateMiddleMapNpc.GetUpdateMiddleMapNpcInstance();

            if ( updater.Init(rootdir, conn))
            {
                updater.UpdateAllTable();
            }
            return updater.CheckError();
        }
        public string UpdateOneNode(string rootdir, System.Data.SqlClient.SqlConnection conn, TreeNode node)
        {
            UpdateMiddleMapNpc updater = UpdateMiddleMapNpc.GetUpdateMiddleMapNpcInstance();

            if (updater.Init(rootdir, conn))
            {
                updater.UpdateTable4OneNode(node);
            }
            return updater.CheckError();

        }
    }
    
    //将数据表middlemap_npc导出到npc.tab文件(会有多个)
    public class DB2TabExportor
    {
        public bool ExportNpcTab(System.Data.SqlClient.SqlConnection conn, string rootdir)
        {
            MiddleMapNpcExportor npcExportor = new MiddleMapNpcExportor(rootdir, conn);
            npcExportor.Export();

            return true;
        }
    };
}
