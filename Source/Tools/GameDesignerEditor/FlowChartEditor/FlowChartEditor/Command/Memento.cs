using System;
using System.Collections.Generic;
using System.Text;

namespace FlowChartEditor.Command
{
    public abstract class Memento
    {
        /// <summary>
        /// 保存对象数据
        /// </summary>
        /// <param name="o">当前对象</param>
        public abstract void SaveData(object o);

        /// <summary>
        /// 加载对象数据
        /// </summary>
        /// <returns>对象镜像</returns>
        public abstract object LoadData();
    }
}
