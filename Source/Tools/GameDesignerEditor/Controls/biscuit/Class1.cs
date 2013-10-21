using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace biscuit
{
    public class Class1
    {
        private MainForm mainform;

        /// <summary>
        /// 初始化控件
        /// </summary>
        /// <param name="conn">传入的数据库连接</param>
        /// <param name="keyword">唯一的关键字，采用表名+逗号+主键1的值+逗号+...，如tablename,2,4</param>
        /// <returns>返回一个Control的引用，其实也就是返回了一个tabControl</returns>
        public Control Init(SqlConnection conn, string rootDir, string catname)
        {
            mainform = new MainForm(conn, rootDir, catname);
            return mainform.OutterCtrl;
        }

        /// <summary>
        /// 是否被用户改变
        /// </summary>
        /// <returns>返回一个是否修改的标志</returns>
        public Boolean hasChanged()
        {
            return mainform.hasChanged();
        }


        /// <summary>
        /// 保存所有页
        /// </summary>
        public void saveAll()
        {
            mainform.saveAll();
        }

        /// <summary>
        /// 刷新所有页面
        /// </summary>
        public void reloadAll()
        {
            mainform.reloadAll();
        }

        /// <summary>
        /// 按照新关键字重新载入
        /// </summary>
        /// <param name="keyword">新关键字</param>
        public void reloadAll(string keyword)
        {
            mainform.reloadAll(keyword);
        }
    }
}
