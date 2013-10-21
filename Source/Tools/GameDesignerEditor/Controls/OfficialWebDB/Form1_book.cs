using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace OfficialWebDB
{
    public partial class Form1
    {
        private bool Fill_Book()
        {
            // 清空老表数据
            Helper.ClearDbTable("web_book", Conn);

            // 填入
            DataRow defaultrow = TblBook.Rows[0];
            foreach (DataRow row in TblBook.Rows)
            {
                string bookname = row["Name"].ToString().Trim();

                int requirelevel = 0;

                string booktype = "默认分类";

                string price = "";

                // 插入一条quest的数据
                string sql = string.Format("insert into web_book values('{0}', {1}, '{2}', '{3}')",
                    bookname, requirelevel, booktype, price);
                Helper.ExecSqlCmd(sql, Conn);
            }

            return true;
        }

    }
}
