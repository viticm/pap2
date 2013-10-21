using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Configuration;
using DataTransmission.Common;

namespace DataTransmission.DataAccess
{
    /// <summary>
    /// 源数据操作类
    /// </summary>
    class SrcData
    {

        #region 构造函数
        /// <summary>
        /// 构造函数
        /// </summary>
        public SrcData() { }
        #endregion

        #region 获取一段时间内的数据
        /// <summary>
        /// 获取一段时间内的数据
        /// </summary>
        /// <param name="dtBeginDate">开始日期</param>
        /// <param name="dtEndDate">结束日期</param>
        /// <returns></returns>
        public DataSet GetDataByDate(DateTime dtBeginDate, DateTime dtEndDate)
        {
            int intGameId = int.Parse(ConfigurationSettings.AppSettings["GameId"].ToString());
            string cmdText = "select iid, game_id, fill_datetime, card_type, card_amount, fill_type, uid, gameuid, cardcode, ip, fill_from_to, server_ip, vouch_code, gateway, income, detail  from fill_log_all where game_id = @intGameId and fill_datetime between @dtBeginDate and @dtEndDate ";
            //string cmdText = "select  * from dbo.view_fill_log_jx3 where create_date > @dtBeginDate and create_date< @dtEndDate ";
            DataSet ds = new DataSet();
            SqlParameter[] parameters = { 
                                            new SqlParameter("@dtBeginDate", SqlDbType.DateTime),
                                            new SqlParameter("@dtEndDate", SqlDbType.DateTime),
                                            new SqlParameter("@intGameId", SqlDbType.Int)
                                        };
            parameters[0].Value = dtBeginDate;
            parameters[1].Value = dtEndDate;
            parameters[2].Value = intGameId;

            ds = SqlHelper.ExecuteDataSet(SqlHelper.Conn, CommandType.Text, cmdText, parameters);
            return ds;
        }
        #endregion

    }
}
