using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using System.Data.SqlClient;

namespace ChooseItem
{
    class Compute
    {
        /// <summary>
        /// 显示计算窗口
        /// </summary>
        /// <param name="dropValue">掉落值</param>
        /// <param name="luckyRate">幸运权值</param>
        public void Show(string dropValue, string luckyRate)
        {
            int dropV = 0;
            int luckyR = 0;

            try
            {
                dropV = int.Parse(dropValue);
                luckyR = int.Parse(luckyRate);
            }
            catch(Exception ex)
            {
                MessageBox.Show("在计算最终掉落率时产生异常: " + ex.ToString());
                return;
            }

            ComputeDropRate cForm = new ComputeDropRate();
            cForm.DropValue = dropV;
            cForm.LuckyRate = luckyR;
            DialogResult result = cForm.ShowDialog();
        }
    }
}
