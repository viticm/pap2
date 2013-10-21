using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace NpcModelNevigate
{
    public class NpcModelNev
    {
        private string filePath; // 文件路径
        private string modelName; // npc模型名称

        public NpcModelNev()
        {

        }

        /// <summary>
        /// 文件路径属性
        /// </summary>
        public string FilePath
        {
            get
            {
                return filePath;
            }
        }

        /// <summary>
        /// npc模型名称
        /// </summary>
        public string ModelName
        {
            get
            {
                return modelName;
            }
        }

        public int Show(SqlConnection Conn, object oldValue, string tblname, string catfieldname, string mapname)
        {
            Form1 frm = new Form1(Conn, tblname, catfieldname, mapname);
            DialogResult result = frm.ShowDialog();
            if (result == DialogResult.OK)
            {
                filePath = frm.FilePath;
                modelName = frm.NpcName;
                return frm.ret;
            }
            else // cancel
            {
                return Convert.ToInt32(oldValue);
            }
        }

    }
}
