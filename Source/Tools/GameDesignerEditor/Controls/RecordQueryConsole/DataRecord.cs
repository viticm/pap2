using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;


namespace RecordQueryConsole
{
    class DataRecord
    {  
        public static string  TheMessage = string.Empty;

        public static void QueryDataRecord(string sqlConnStr, string StartTime, string EndTime, string exportFilePath)
        {
            try
            {
                SqlConnection Conn = new SqlConnection(sqlConnStr);

                if (!System.IO.File.Exists(exportFilePath))
                {
                    exportFilePath = Helper.CreatePath(exportFilePath);
                }

                string select = "hostname, edittime=convert(varchar(19), d.edittime,121)," +
                "(select m.modelname_cn from sys_modl_def m where [id] = d.modelid) as modelname ," +
                "(select t.name from sys_modl_tab_def t where t.modtabid=d.tabid) as tabname," +
                "catfields, nodename, fielddisplayname, oldvalue, newvalue, remarks ";

                string sql = string.Format("select {0} from sys_Datarecord d where edittime >= '{1}' and edittime <= '{2}'", select, StartTime, EndTime);
            

                DataTable dtRecord = Helper.GetDataTable(sql, Conn);
                if (dtRecord != null)
                {
                    dtRecord.TableName = "OneRecord";
                    dtRecord.DataSet.DataSetName = "DataRecords";
                    if (Helper.DataTableExport(dtRecord, exportFilePath))
                    {
                        TheMessage = "查询并导出成功。";
                    }
                    else
                    {
                        TheMessage = "导出失败，可能的原因是导出路径不存在或者后缀名不对";
                    }                    
                    
                }
            }
            catch(Exception ex)
            {
                TheMessage = ex.ToString();
            }
          
           
        }
    }
}
