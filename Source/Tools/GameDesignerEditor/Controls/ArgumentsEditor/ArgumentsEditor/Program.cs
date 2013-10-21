using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace ArgumentsEditor
{
    public class Program
    {
        private EditForm eForm; // 参数编辑窗体
        private OperationEditForm oForm; // 表达式编辑窗体 

        /// <summary>
        /// 参数字符串
        /// </summary>
        public string Value
        {
            get
            {
                return eForm.ArgumentsValue;
            }
        }

        /// <summary>
        /// 表达式
        /// </summary>
        public string Expression
        {
            get
            {
                return oForm.Value;
            }
        }

        /// <summary>
        /// 调试用入口主函数
        /// </summary>
        //[STAThread]
        //public static void Main()
        //{
        //    SqlConnection conn = new SqlConnection("Server = 192.168.8.54; DataBase = FS2Settings; Uid = sa; Password = 1023;");
        //    OperationEditForm oForm = new OperationEditForm(conn, "");
        //    oForm.ShowDialog();
        //}

        /// <summary>
        /// 显示编辑窗体
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="oldValue">旧值</param>
        /// <returns>是否编辑成功</returns>
        public bool Show(SqlConnection conn, string oldValue)
        {            
            eForm = new EditForm(conn, oldValue);
            bool editSuccess = (eForm.ShowDialog() == DialogResult.OK);

            return editSuccess;
        }

        /// <summary>
        /// 编辑表达式
        /// </summary>
        /// <returns>是否编辑成功</returns>
        public bool EditExpression(SqlConnection conn, string oldValue)
        {
            oForm = new OperationEditForm(conn, oldValue);
            bool editSuccess = (oForm.ShowDialog() == DialogResult.OK);

            return editSuccess;
        }
    }
}
