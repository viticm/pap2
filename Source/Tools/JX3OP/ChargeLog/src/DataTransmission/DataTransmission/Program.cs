using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Timers;
using System.Threading;
using DataTransmission.Bll;

namespace DataTransmission
{
    class Program
    {
        private static DataTransmission.Bll.DataOperater dataOperater = new DataTransmission.Bll.DataOperater();

        static void Main(string[] args)
        {
            bool canCreateNew;
            //限制单例运行
            Mutex mutex = new Mutex(true, "DataTransmission", out canCreateNew);
            if (canCreateNew)
            {
                DateTime dtBeginDateNow = System.DateTime.Now;
                try
                {
                    //执行数据传输
                    dataOperater.Work();
                    DateTime dtEndDateNow = System.DateTime.Now;
                    TimeSpan ts = dtEndDateNow.Subtract(dtBeginDateNow);
                    Console.Write("开始时间:{0},结束时间{1}, 时差分钟{2},时差秒{3}", dtBeginDateNow.ToString(), dtEndDateNow.ToString(), ts.Minutes.ToString(), ts.Seconds.ToString());
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                finally
                {
                    mutex.ReleaseMutex();       //释放锁(重要必须)
                }
            }
            else
            {
                Console.WriteLine("系统已经有一个实例正在运行之中");
            }
        }



    }
}
