using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Collections;

namespace TableSelector
{
    public class Program
    {
        TreeForm tForm;

        /// <summary>
        /// 构造函数
        /// </summary>
        public Program()
        {

        }

        /// <summary>
        /// 选择的值
        /// </summary>
        public String SelectedValue
        {
            get
            {
                return tForm.SelectedValue;
            }
        }

        /// <summary>
        /// 选择的值组
        /// </summary>
        public Hashtable SelectedValues
        {
            get
            {
                return tForm.SelectedValues;
            }
        }

        /// <summary>
        /// 显示编辑窗口
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="title">窗体标题</param>
        /// <param name="tableName">表名</param>
        /// <param name="classifyField">分类字段</param>
        /// <param name="returnField">返回值字段</param>
        /// <returns>是否确认编辑</returns>
        public bool Show(SqlConnection conn, string title, string tableName, string classifyField, string returnField, string oldValue)
        {
            bool success = false; // 是否编辑成功

            tForm = new TreeForm(conn, title, tableName, classifyField, returnField, oldValue);
            DialogResult result = tForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                success = true;
            }

            return success;
        }


        private TreeForm m_form = null; 
        //初始化
        public bool Init(SqlConnection conn, string title, string sql, string classifyField, string returnField)
        {
            m_form = new TreeForm(conn, title, sql, classifyField, returnField, "");
            return false;
        }

        public bool ShowAgain(string oldValue)
        {
            return false;
        }
    }
}
