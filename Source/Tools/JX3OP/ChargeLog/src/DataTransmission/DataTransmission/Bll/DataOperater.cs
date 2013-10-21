using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Data.OracleClient;

using DataTransmission.Common;
using DataTransmission.DataAccess;
using System.Configuration;

namespace DataTransmission.Bll
{
    /// <summary>
    /// 数据操作类
    /// </summary>
    class DataOperater
    {
        private static DataAccess.SrcData srcData = new DataAccess.SrcData();
        private static DataAccess.DesData desData = new DataAccess.DesData();
        private static int Rows = 100;

        #region 构造函数
        /// <summary>
        /// 构造函数
        /// </summary>
        public DataOperater()
        {   //初始每次更新数据条数
            if (!string.IsNullOrEmpty(ConfigurationSettings.AppSettings["Rows"]))
            {
                Rows = int.Parse(ConfigurationSettings.AppSettings["Rows"].ToString());
            }
        }
        #endregion

        #region 开始运行
        /// <summary>
        /// 开始运行
        /// </summary>
        public void Work()
        {
            DateTime dtBeginDate = new DateTime();
            DateTime dtEndDate = System.DateTime.Now;

            //判断数据更新日期
            if (!string.IsNullOrEmpty(ConfigurationSettings.AppSettings["UpdateDay"]))
            {
                int intUpdateDay = int.Parse(ConfigurationSettings.AppSettings["UpdateDay"].ToString());
                dtBeginDate = dtEndDate.AddDays(intUpdateDay);
                //删除最近日期的数据
                DeleteDesDataByDate(dtBeginDate, dtEndDate);
            }
            else
            {
                dtBeginDate = GetDesMaxDate().AddSeconds(1);
            }
            //数据传输
            DataTransmission(dtBeginDate, dtEndDate);
        }
        #endregion

        #region 数据传输
        /// <summary>
        /// 数据传输
        /// <param name="dtBeginDate">开始日期</param>
        /// <param name="dtEndDate">结束日期</param>
        /// </summary>
        public void DataTransmission(DateTime dtBeginDate, DateTime dtEndDate)
        {

            //获取源数据
            DataSet ds = new DataSet();
            Console.WriteLine("获取源数据中……");
            ds = GetSrcDataByDate(dtBeginDate, dtEndDate);

            if (ds != null && ds.Tables.Count > 0)
            {
                DataTable dt = ds.Tables[0];
                //插入Sql字符串
                string strInsertSql = "insert into fill_log_all (iid, game_id, fill_datetime, card_type, card_amount, fill_type, "  + "\"UID\"" +  ", gameuid, cardcode, ip, fill_from_to, server_ip, vouch_code, gateway, income, detail ) values (:v_iid, :v_game_id, :v_fill_datetime, :v_card_type, :v_card_amount, :v_fill_type, :v_uid, :v_gameuid, :v_cardcode, :v_ip, :v_fill_from_to, :v_server_ip, :v_vouch_code, :v_gateway, :v_income, :v_detail )";
                using (OracleConnection conn = new OracleConnection(OracleHelper.Conn))
                {
                    conn.Open();
                    OracleTransaction trans = conn.BeginTransaction();
                    try
                    {
                        for (int i = 0; i < dt.Rows.Count; i++)
                        {
                            OracleCommand cmd = new OracleCommand();
                            cmd.Connection = conn;
                            cmd.CommandType = CommandType.Text;
                            cmd.CommandText = strInsertSql;
                            cmd.Transaction = trans;

                            int iid = int.Parse(dt.Rows[i]["iid"].ToString());
                            int game_id = int.Parse(dt.Rows[i]["game_id"].ToString());
                            DateTime fill_datetime = DateTime.Parse(dt.Rows[i]["fill_datetime"].ToString());
                            int card_type = int.Parse(dt.Rows[i]["card_type"].ToString());
                            decimal card_amount = decimal.Parse(dt.Rows[i]["card_amount"].ToString());
                            int fill_type = Int32.Parse(dt.Rows[i]["fill_type"].ToString());
                            string uid = dt.Rows[i]["uid"].ToString();
                            string gameuid = dt.Rows[i]["gameuid"].ToString();
                            string cardcode = dt.Rows[i]["cardcode"].ToString();
                            string ip = dt.Rows[i]["ip"].ToString();
                            int fill_from_to = int.Parse(dt.Rows[i]["fill_from_to"].ToString());
                            string server_ip = dt.Rows[i]["server_ip"].ToString();
                            string vouch_code = dt.Rows[i]["vouch_code"].ToString();
                            string gateway = dt.Rows[i]["gateway"].ToString();
                            int income = int.Parse(dt.Rows[i]["income"].ToString());
                            string detail = dt.Rows[i]["detail"].ToString();


                            cmd.Parameters.Add(":v_iid", OracleType.Number);
                            cmd.Parameters.Add(":v_game_id", OracleType.Number);
                            cmd.Parameters.Add(":v_fill_datetime", OracleType.DateTime);
                            cmd.Parameters.Add(":v_card_type", OracleType.VarChar, 32);
                            cmd.Parameters.Add(":v_card_amount", OracleType.Number);
                            cmd.Parameters.Add(":v_fill_type", OracleType.Number);
                            cmd.Parameters.Add(":v_uid", OracleType.VarChar, 32);
                            cmd.Parameters.Add(":v_gameuid", OracleType.VarChar, 32);
                            cmd.Parameters.Add(":v_cardcode", OracleType.VarChar, 12);
                            cmd.Parameters.Add(":v_ip", OracleType.VarChar, 32);
                            cmd.Parameters.Add(":v_fill_from_to", OracleType.Number);
                            cmd.Parameters.Add(":v_server_ip", OracleType.VarChar, 32);
                            cmd.Parameters.Add(":v_vouch_code", OracleType.VarChar, 20);
                            cmd.Parameters.Add(":v_gateway", OracleType.VarChar, 6);
                            cmd.Parameters.Add(":v_income", OracleType.Number);
                            cmd.Parameters.Add(":v_detail", OracleType.VarChar, 2048);

                            cmd.Parameters[0].Value = iid;
                            cmd.Parameters[1].Value = game_id;
                            cmd.Parameters[2].Value = fill_datetime;
                            cmd.Parameters[3].Value = card_type;
                            cmd.Parameters[4].Value = card_amount;
                            cmd.Parameters[5].Value = fill_type;
                            cmd.Parameters[6].Value = uid;
                            cmd.Parameters[7].Value = gameuid;
                            cmd.Parameters[8].Value = cardcode;
                            cmd.Parameters[9].Value = ip;
                            cmd.Parameters[10].Value = fill_from_to;
                            cmd.Parameters[11].Value = server_ip;
                            cmd.Parameters[12].Value = vouch_code;
                            cmd.Parameters[13].Value = gateway;
                            cmd.Parameters[14].Value = income;
                            cmd.Parameters[15].Value = detail;



                            cmd.ExecuteNonQuery();
                        }
                        trans.Commit();
                    }
                    catch (Exception ex)
                    {
                        trans.Rollback();
                        throw ex;
                    }
                    finally
                    {
                        if (conn.State == ConnectionState.Open)
                        {
                            conn.Close();
                        }
                    }
                }
                dt.Dispose();
            }
            ds.Dispose();
        }
        #endregion


        #region 获取目标数据库最近充值日期
        /// <summary>
        /// 获取目标数据库最近充值日期
        /// </summary>
        /// <returns></returns>
        public DateTime GetDesMaxDate()
        {
            return desData.GetMaxDate();
        }
        #endregion

        #region 获取一段日期内的充值数据
        /// <summary>
        /// 获取一段日期内的充值数据
        /// </summary>
        /// <param name="dtBeginDate">开始日期</param>
        /// <param name="dtEndDate">结束日期</param>
        /// <returns></returns>
        public DataSet GetSrcDataByDate(DateTime dtBeginDate, DateTime dtEndDate)
        {
            return srcData.GetDataByDate(dtBeginDate, dtEndDate);
        }
        #endregion

        #region 删除一段日期内的数据
        /// <summary>
        /// 删除一段日期内的数据
        /// </summary>
        /// <param name="dtBeginDate"></param>
        /// <param name="dtEndDate"></param>
        /// <returns></returns>
        public int DeleteDesDataByDate(DateTime dtBeginDate, DateTime dtEndDate)
        {
            return desData.DeleteDataByDate(dtBeginDate, dtEndDate);
        }
        #endregion

    }
}
