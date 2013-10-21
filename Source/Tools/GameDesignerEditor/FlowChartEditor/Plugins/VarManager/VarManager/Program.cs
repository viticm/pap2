using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace VarManager
{
    public class Program
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        public Program()
        {

        }

        /// <summary>
        /// 调试用的主方法
        /// </summary>
        //[STAThread]
        //public static void Main()
        //{
        //    SqlConnection conn = new SqlConnection("Server = jx3web; DataBase = s3design_debug; Uid = s3design; Password = davidbowie");
        //    MainForm mForm = new MainForm(conn);
        //    DialogResult result = mForm.ShowDialog();
        //}

        /// <summary>
        /// 显示编辑窗口
        /// </summary>
        /// <param name="conn">sql连接</param>
        public void Show(SqlConnection conn)
        {
            MainForm mForm = new MainForm(conn);
            DialogResult result = mForm.ShowDialog();
        }

        /// <summary>
        /// 显示选择窗口
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="conditionFields">条件列</param>
        /// <param name="conditonValues">条件值</param>
        public string SelectVar(SqlConnection conn, string conditionFields, string conditionValues)
        {
            string varString = null;
            SelectForm sForm = new SelectForm(conn, conditionFields, conditionValues);
            DialogResult result = sForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                varString = sForm.VarString;
            }

            return varString;
        }
    }
}
