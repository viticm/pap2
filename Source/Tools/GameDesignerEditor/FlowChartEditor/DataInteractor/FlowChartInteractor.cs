using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using DataElementLibrary;

namespace DataInteractor
{
    [Serializable]
    public class FlowChartInteractor
    {
        private string customText; // 自定义文本
        private Hashtable customTable = new Hashtable(); // 自定义哈希表

        /// <summary>
        /// 构造函数
        /// </summary>
        public FlowChartInteractor()
        {

        }

        /// <summary>
        /// 自定义文本
        /// </summary>
        public string CustomText
        {
            set
            {
                customText = value;
            }

            get
            {
                return customText;
            }
        }

        /// <summary>
        /// 自定义哈希表
        /// </summary>
        public Hashtable CustomTable
        {
            get
            {
                if(customTable == null)
                {
                    customTable = new Hashtable();
                }

                return customTable;
            }
        }

        /// <summary>
        /// 设置数据参数
        /// </summary>
        /// <param name="args">数据参数哈希表</param>
        public void SetArguments(Hashtable args)
        {
            customText = args["customText"] as string;
            customTable = args["customTable"] as Hashtable;
        }

        /// <summary>
        /// 获取数据参数
        /// </summary>
        /// <returns></returns>
        public Hashtable GetArguments()
        {
            Hashtable args = new Hashtable();
            args["customText"] = customText;
            args["customTable"] = customTable;

            return args;
        }
    }
}
