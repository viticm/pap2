using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace CoordinateEditor
{
    public class Program
    {
        CoordinateForm cForm;

        //public static void Main()
        //{
        //    CoordinateForm cForm = new CoordinateForm("1,2,3");
        //    cForm.ShowDialog();
        //}

        /// <summary>
        /// 构造函数
        /// </summary>
        public Program()
        {

        }

        /// <summary>
        /// 选择的坐标
        /// </summary>
        public string Cordinate
        {
            get
            {
                return cForm.Cordinate;
            }
        }

        /// <summary>
        /// 编辑插件
        /// </summary>
        /// <param name="cordinate">当前坐标</param>
        /// <returns>是否编辑成功</returns>
        public int Show(string cordinate)
        {
            int success = 0;
            cForm = new CoordinateForm(cordinate);
            DialogResult result = cForm.ShowDialog();
            if(result == DialogResult.OK)
            {
                success = 1;
            }

            return success;
        }
    }
}
