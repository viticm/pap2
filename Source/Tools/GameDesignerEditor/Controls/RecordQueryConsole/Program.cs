using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace RecordQueryConsole
{
    class Program
    {       
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if (args.Length == 0)//图像窗口
            {

                SelectDataBaseForm dbForm = new SelectDataBaseForm();
                if (dbForm.ShowDialog() != DialogResult.OK)
                {
                    return;
                }
                string ChoosedDBName = dbForm.ChoosedDBName;
                Application.Run(new DataRecordsQureyForm(ChoosedDBName));
            }
            else if (args.Length != 4)//控制台
            {
                string usage = "Usage:\r\nRecordQueryConsole.exe SqlConnString StartTime EndTime SaveFilePath\r\n" +
                "\tSqlConnString:\t数据库连接字符串，包括要访问的：数据库名称、用户、密码、ip\r\n" +
                "\tStartTime:\t起始时间，格式为 2009-04-06 16:26:00\r\n" +
                "\tEndTime:\t终止时间，格式同上\r\n" +
                "\tSaveFilePath:\t导出文件的路径，需要带扩展名，可选的为：xlsx、xml\r\n\r\n" +
                "\t可选的数据库如下：\r\n" +
                "\t\ts3design_rtm:\t\tHead库\r\n" +
                "\t\ts3design-1-0-8-1380:\t1380备份库\r\n" +
                "\t\ts3design-1-0-8-1380-new:1380库\r\n" +
                "\t\tss3design_skill_branch:\t技能分支库\r\n" +
                "Example:\r\nRecordQueryConsole.exe \"Server = 192.168.27.31; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;\" \"2009-04-05 16:26:00\" \"2009-04-06 16:26:00\" D:\\DataRecordQueryResult.xml\r\n";

                Console.Write(usage);
            }
            else
            {
                string sqlConnStr;
                string StartTime;
                string EndTime;
                string FilePath;

                sqlConnStr = args[0];
                StartTime = args[1];
                EndTime = args[2];
                FilePath = args[3];

                if (!Helper.IsLegalDataTimeString(StartTime))
                {
                    Console.WriteLine("StartTime时间格式不对");
                }
                else if (!Helper.IsLegalDataTimeString(EndTime))
                {
                    Console.WriteLine("EndTime时间格式不对");
                }
                else
                {
                    Console.Write("正在查询和导出，请稍等....\r\n");
                    DataRecord.QueryDataRecord(sqlConnStr, StartTime, EndTime, FilePath);
                    Console.Write(DataRecord.TheMessage);
                }

            }
            
        }

       







    }
}
