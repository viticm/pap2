using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;

namespace CreateAICustomFunction
{
    public class Program
    {
        private CustomFunctionForm cForm; // 自定义动作编辑窗体

        /// <summary>
        /// 自定义动作的名称
        /// </summary>
        public string FunctionName
        {
            get
            {
                return cForm.FunctionName;
            }
        }

        /// <summary>
        /// 自定义动作的ID
        /// </summary>
        public object[] FunctionKey
        {
            get
            {
                return new object[] { cForm.FunctionID };
            }            
        }

        /// <summary>
        /// 创建自定义动作
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <returns>是否编辑成功</returns>
        public bool CreateFunction(SqlConnection conn)
        {
            bool editSuccess = false;

            cForm = new CustomFunctionForm(conn);
            DialogResult result = cForm.ShowDialog();
            if (result == DialogResult.OK)
            {
                editSuccess = true;
            }

            return editSuccess;
        }

    }
}
