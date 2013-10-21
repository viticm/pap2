using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.OracleClient;

using DataTransmission.Common;

namespace DataTransmission.DataAccess
{
    /// <summary>
    /// 目标数据库操作类
    /// </summary>
    class DesData
    {

        #region 构造函数
        /// <summary>
        /// 构造函数
        /// </summary>
        public DesData() { }
        #endregion

        #region 获取最近充值日期
        /// <summary>
        /// 获取最近充值日期
        /// </summary>
        /// <returns></returns>
        public DateTime GetMaxDate()
        {
            string cmdText = "select max(fill_datetime) as MaxDate  from fill_log_all";
            DateTime dtMaxDate = DateTime.MinValue; //仅适用于Oracle
            //object obj = OracleHelper.ExecuteScalar(OracleHelper.Conn, CommandType.Text, cmdText);
            string strMaxDate = OracleHelper.ExecuteScalar(OracleHelper.Conn, CommandType.Text, cmdText).ToString();
            if (strMaxDate != "")
                dtMaxDate = DateTime.Parse(strMaxDate);
            else
                dtMaxDate = DateTime.Parse("1970-01-01");
            
            return dtMaxDate;
        }
        #endregion

        #region 删除一段日期内的数据
        /// <summary>
        /// 删除一段日期内的数据
        /// </summary>
        /// <param name="dtBeginDate"></param>
        /// <param name="dtEndDate"></param>
        /// <returns></returns>
        public int DeleteDataByDate(DateTime dtBeginDate, DateTime dtEndDate)
        {
            string cmdText = "delete from fill_log_all where fill_datetime between :v_BeginDate and :v_EndDate";
            OracleParameter[] parameters = { 
                                               new OracleParameter(":v_BeginDate", OracleType.DateTime, 19),
                                               new OracleParameter(":v_EndDate", OracleType.DateTime, 19)
                                           };
            parameters[0].Value = dtBeginDate;
            parameters[1].Value = dtEndDate;

            int rowsAffected = 0;
            rowsAffected = OracleHelper.ExecuteNonQuery(OracleHelper.Conn, CommandType.Text, cmdText, parameters);

            return rowsAffected;
        }
        #endregion

    }
}
