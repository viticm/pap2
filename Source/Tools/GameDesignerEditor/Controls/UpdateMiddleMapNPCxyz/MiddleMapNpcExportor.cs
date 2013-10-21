using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;

namespace UpdateMiddleMapNPCxyz
{
    //导出的字段为id	middlemap	kind	type	x	y	z
    class MiddleMapNpcExportor
    {
        private SqlConnection Conn;
        private string RootDir;

        private string HeaderFields = string.Format("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\n", "id", "middlemap", "kind", "type", "x", "y", "z");
        private string[] ExportFields = {"id", "middlemap", "kind", "type", "x", "y", "z" };//所要导出的列名
        private string TableName = "middlemap_npc";
        
        public MiddleMapNpcExportor(string rootdir, SqlConnection conn)
        {
            Conn = conn;
            RootDir = rootdir;
        }
        /// <summary>
        /// 获取一个场景的数据信息
        /// </summary>
        /// <param name="mapname">场景名称</param>
        /// <param name="dtAll">middlemap_npc的所有信息</param>
        /// <returns>返回用于导出的字符串</returns>
        private string GetOneMapExportStr(string mapname, DataTable dtAll)
        {
            string strRet = HeaderFields;

            string strFilter = string.Format("map = '{0}'", mapname);
            string sortOrder = string.Format("{0}", "_index");


            DataRow[] rows = dtAll.Select(strFilter, sortOrder);

            int columnCnt = dtAll.Columns.Count;
            string oneCell;
            foreach (DataRow row in rows)
            {
                foreach (string column in ExportFields)
                {
                    if (row.IsNull(column))
                    {
                        oneCell = "0";
                    }
                    else
                    {
                        oneCell = row[column].ToString();
                    }
                    strRet += string.Format("{0}\t", oneCell);
                }
                strRet = strRet.Remove(strRet.Length - 1);
                strRet += "\n";
            }   
            return strRet;
        }       
       
        public void Export()
        {
            string strSql = string.Format("select * from {0} where id != 0",  TableName);
            DataTable dtAll = Helper.GetDataTable(strSql, Conn);

            string strSqlMaps = string.Format("select DISTINCT {0} from {1}", "map", TableName);
            DataTable dtMaps = Helper.GetDataTable(strSqlMaps, Conn);

            string oneMapData;
            string mapName;
            string tabFileName;
            
            foreach (DataRow rowMap in dtMaps.Rows)
            {
                if (rowMap.IsNull(0))
                {
                    continue;
                }

                mapName = rowMap[0].ToString();
                oneMapData = GetOneMapExportStr(mapName, dtAll);
                tabFileName = string.Format("{0}\\data\\source\\maps\\{1}minimap\\npc.tab", RootDir, mapName);

                Helper.StringToFile(oneMapData, tabFileName, Encoding.GetEncoding("gb2312"));

            }


        }
        
    }
}
