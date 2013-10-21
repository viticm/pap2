using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace DBManager
{
    /// <summary>
    /// 使用表驱动法分析并处理字符串
    /// </summary>
    //DFA图
    //{{DIAGRAM_START
    //
    //            .----.           .----.           .----.          .----.         .----.         .----.
    //            |一  |           |一  |           |[   |          |[   |         |]   |         |]   |
    //            |    |           |    |           |    |          |    |         |    |         |    |
    //    .----.  '----'           '----'           '----'          '----'         '----'         '----'
    //    |    >----------->----.          .----.           .----.          .----.         .----.         .----.
    //    |1   <-----------<2   >---------->3   >----------->4   >---------->5   >--------->6   >--------->1'  |
    //    |    |  .----.   |    |          |    |           |    |          |    <---------<    |         |    |
    //    |    |  |非一|   '----'          'v--v'           'v---'          'v---'         '----'         ^----'
    //    |    |  |    |                    |  |             |               |      .----.                |
    //    |    |  '----'       .----.       |  |             |               |      |其他|                |
    //    '----^               |回车|       |  |             |               |      |    |                |
    //         |               |    |       |  |             |               |      '----'                |
    //         |               '----'       |  |             |               |                            |
    //         ------------------------------  |             |.----.         |                            |
    //                                   .----.|             ||其他|         | .----.                     |
    //                                   |其他||             ||    |         | |其他|                     |
    //                                   |    ||             |'----'         | |    |                     |
    //                                   '----'|             |               | '----'                     |
    //                                         |             |               |                            |
    //                                         |             |              .v---.                        |
    //              .----.             .----<---             |              |5   |                        |
    //              |7'  <-------------<7   <-----------------              |    |                        |
    //              |    |             |    |                               '----'                        |
    //              '----'   .----.    '---v'                                                             |
    //                       |非回|        ----------------------------------------------------------------
    //                       |车  |                                     .----.
    //                       '----'                                     |回车|
    //                                                                  |    |
    //                                                                  '----'
    //
    //            .----.           .----.                    .----.           .----.
    //            |"   |           |\   |                    |'   |           |\   |
    //            |    |           |    |                    |    |           |    |
    //    .----.  '----'   .----.  '----'  .----.     .----. '----' .----.    '----'  .----.
    //    |1   >----------->8   >---------->9   |     |1   >-------->10  >------------>11  |
    //    |    <-----------<    <----------<    |     |    <--------<    <------------<    |
    //    '----'  .----.   '----'  .----.  '----'     '----' .----. '----'    .----.  '----'
    //            |"   |           |任意|                    |'   |           |任意|
    //            |    |           |    |                    |    |           |    |
    //            '----'           '----'                    '----'           '----'
    //
    //            .----.           .----.              .----.         .----.
    //            |[   |           |[   |              |]   |         |]   |
    //            |    |           |    |              |    |         |    |
    //            '----'           '----'              '----'         '----'
    //    .----.           .----.               .----.        .----.          .----.
    //    |1   >----------->12  >--------------->13  >-------->14  >---------->1'  |
    //    |    <-----------<    |               |    <--------<    |          |    |
    //    '----'           'v--v'               'v---' .----. '----'          '----'
    //            .----.    |  |   .----.        |     |其他|
    //            |其他|    |  |   |任意|        |     |    |
    //            |    |    |  |   |    |        |     '----'
    //            '----'    |  |   '----'        |
    //                      |  |                 |  .----.
    //                      |  |                 |  |其他|
    //                      |  |                 |  |    |
    //            .----.    |  |   .----.        |  '----'
    //            |'   |    |  |   |''  |        |
    //            |    |    |  |   |    |       .v---.
    //            '----'    |  |   '----'       |13' |
    //                      |  |                |    |
    //              .----<---  -->----.         '----'
    //              |10  |       |8   |
    //              |    |       |    |
    //              '----'       '----'
    //}}DIAGRAM_END
    public class DFATable
    {
        // 字符表
        Hashtable ht_Char = new Hashtable();

        //=============状态表=======================
        //状态量  实例        说明
        // 1.     xxx         正常
        // 2.     -           不完全单行注释
        // 3.     --          不完全多行注释
        // 4.     --[         不完全多行注释
        // 5.     --[[        标准多行注释
        // 6.     --[[]       不完全多行注释结束
        // 7.     --xxxxx     标准单行注释
        // 8.     "           标准单行字符串
        // 9.     "xx\        标准单行字符串(转义)
        // 10.    '           标准单行字符串
        // 11.    'xx\        标准单行字符串(转义)
        // 12.    [           不完全多行字符串
        // 13.    [[          标准多行字符串
        // 14.    [[]         不完全多行字符串结束
        //==========================================

        //状态迁移表
        //列：旧状态     字符      新状态     回掉函数
        ArrayList al_Table = new ArrayList();

        public DFATable()
        {
            //初始化字符表
            ht_Char[0] = "";        // 0表示匹配所有字符
            ht_Char[1] = "'";
            ht_Char[2] = "\"";
            ht_Char[3] = "-";
            ht_Char[4] = "[";
            ht_Char[5] = "]";
            ht_Char[6] = "\\";
            ht_Char[7] = "\n";

            //初始化状态迁移表
            al_Table.Add(new ZTableItem(1, 1, 10, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(1, 2, 8, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(1, 3, 2, null));
            al_Table.Add(new ZTableItem(1, 4, 12, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(1, 0, 1, null));    // 0表示匹配所有字符，所以所有0必须在旧状态的最后一项

            al_Table.Add(new ZTableItem(2, 1, 10, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(2, 2, 8, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(2, 3, 3, null));
            al_Table.Add(new ZTableItem(2, 0, 1, null));

            al_Table.Add(new ZTableItem(3, 4, 4, null));
            al_Table.Add(new ZTableItem(3, 7, 1, null));
            al_Table.Add(new ZTableItem(3, 0, 7, null));

            al_Table.Add(new ZTableItem(4, 4, 5, null));
            al_Table.Add(new ZTableItem(4, 7, 1, null));
            al_Table.Add(new ZTableItem(4, 0, 7, null));

            al_Table.Add(new ZTableItem(5, 5, 6, null));
            al_Table.Add(new ZTableItem(5, 0, 5, null));

            al_Table.Add(new ZTableItem(6, 5, 1, null));
            al_Table.Add(new ZTableItem(6, 0, 5, null));

            al_Table.Add(new ZTableItem(7, 7, 1, null));
            al_Table.Add(new ZTableItem(7, 0, 7, null));

            al_Table.Add(new ZTableItem(8, 2, 1, new TEndRecord()));
            al_Table.Add(new ZTableItem(8, 6, 9, new TReocrd()));
            al_Table.Add(new ZTableItem(8, 0, 8, new TReocrd()));

            al_Table.Add(new ZTableItem(9, 0, 8, new TReocrd()));

            al_Table.Add(new ZTableItem(10, 1, 1, new TEndRecord()));
            al_Table.Add(new ZTableItem(10, 6, 11, new TReocrd()));
            al_Table.Add(new ZTableItem(10, 0, 10, new TReocrd()));

            al_Table.Add(new ZTableItem(11, 0, 10, new TReocrd()));

            al_Table.Add(new ZTableItem(12, 1, 10, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(12, 2, 8, new TClearAndRecord()));
            al_Table.Add(new ZTableItem(12, 4, 13, new TReocrd()));
            al_Table.Add(new ZTableItem(12, 0, 1, null));

            al_Table.Add(new ZTableItem(13, 5, 14, new TReocrd()));
            al_Table.Add(new ZTableItem(13, 0, 13, new TReocrd()));

            al_Table.Add(new ZTableItem(14, 5, 1, new TEndRecord()));
            al_Table.Add(new ZTableItem(14, 0, 13, new TReocrd()));
        }

        public Hashtable getStringList(string strCode)
        {

            CData.al_ret = new Hashtable();
            int zt = 1;
            for (int i = 0; i < strCode.Length; i++)
            {
                string strCh = strCode.Substring(i, 1);
                for (int j = 0; j < al_Table.Count; j++)
                {
                    ZTableItem zti = al_Table[j] as ZTableItem;
                    if (zti.zfrom == zt)
                    {
                        string strTableChar = ht_Char[zti.charTableIndex].ToString();
                        if (strTableChar == "" || strTableChar == strCh)
                        {
                            zt = zti.zto;
                            if (zti.funcdo != null)
                                zti.funcdo.Do(zt, strCh, zti.zto, i);
                            break;
                        }
                    }
                }
            }
            return CData.al_ret;
        }
    }

    /// <summary>
    /// 全局数据
    /// </summary>
    public static class CData
    {
        // 定义字符缓冲区
        public static string strString = "";

        // 定义全局索引
        public static string strIndex = "0";

        // 返回值
        public static Hashtable al_ret = new Hashtable();
    }

    /// <summary>
    /// 缓冲处理接口
    /// </summary>
    public interface IToDo
    {
        void Do(int zfrom, string charInput, int zto, int index);
    }
    
    /// <summary>
    /// 追加字符到缓冲
    /// </summary>
    public class TReocrd : IToDo
    {
        public void Do(int zfrom, string charInput, int zto, int index)
        {
            CData.strString += charInput;
        }
    };

    /// <summary>
    /// 重置缓冲字符
    /// </summary>
    public class TClearAndRecord : IToDo
    {
        public void Do(int zfrom, string charInput, int zto, int index)
        {
            CData.strIndex = index.ToString();
            CData.strString = charInput;
        }
    };

    /// <summary>
    /// 记录字符并记录整个缓冲区内容
    /// </summary>
    public class TEndRecord : IToDo
    {
        public void Do(int zfrom, string charInput, int zto, int index)
        {
            CData.strString += charInput;

            // 这里可以加入开头为中文的判断（如果需要的话）
            if (CheckStringAvail(CData.strString))
            {
                CData.al_ret.Add(CData.strIndex, CData.strString);
            }

            CData.strString = "";
        }

        public bool CheckStringAvail(string s)
        {
            bool avail = false;
            string content = s.Trim(new char[] { '"', '\'', ' ', ']', '[', '\r', '\n', '\t' });

            if (content.EndsWith(".lua") || content.EndsWith(".ls") || content.EndsWith(".lh")) // 如果是包含文件名的字符串就不用提取了
            {
                avail = false;
            }
            else
            {
                char[] charArray = content.ToCharArray();

                // ASCII码之外的字符都需要提取出来
                foreach (char c in charArray)
                {
                    if ((int)c > 127)
                    {
                        avail = true;
                        break;
                    }
                }
            }

            return avail;
        }
    }

    /// <summary>
    /// 状态迁移表表项
    /// </summary>
    public class ZTableItem
    {
        public ZTableItem(int zfrom, int charTableIndex, int zto, IToDo funcdo)
        {
            this.zfrom = zfrom;
            this.zto = zto;
            this.charTableIndex = charTableIndex;
            this.funcdo = funcdo;
        }

        public int zfrom = 0;
        public int zto = 0;
        public int charTableIndex = 0;
        public IToDo funcdo = null;
    }
}
