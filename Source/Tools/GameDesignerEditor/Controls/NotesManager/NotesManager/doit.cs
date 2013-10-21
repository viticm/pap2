using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace NotesManager
{
    public class doit
    {
        private frmMain mainFm;

#if false
        public int show(SqlConnection conn, string keyword)
        {
            frmMain fm = new frmMain(conn,keyword);
            fm.Activate();
            fm.ShowDialog();
            return 1;
        }
#endif


        /// <summary>
        /// 初始化控件
        /// </summary>
        /// <param name="conn">传入的数据库连接</param>
        /// <param name="keyword">唯一的关键字，采用表名+逗号+主键1的值+逗号+...，如tablename,2,4</param>
        /// <returns>返回一个Control的引用，其实也就是返回了一个tabControl</returns>
        public Control Init(SqlConnection conn)
        {
            mainFm = new frmMain(conn);
            return mainFm.tabControl1;
        }

        /// <summary>
        /// 是否被用户改变
        /// </summary>
        /// <returns>返回一个是否修改的标志</returns>
        public Boolean hasChanged()
        {
            return mainFm.hasChanged();
        }


        /// <summary>
        /// 保存所有页
        /// </summary>
        public void saveAll()
        {
            mainFm.saveAll();
        }

        /// <summary>
        /// 刷新所有页面
        /// </summary>
        public void reloadAll()
        {
            mainFm.reloadAll();
        }

        /// <summary>
        /// 按照新关键字重新载入
        /// </summary>
        /// <param name="keyword">新关键字</param>
        public void reloadAll(string keyword)
        {
            mainFm.reloadAll(keyword);
        }
    }

}
