using System;
using System.Collections.Generic;
using System.Text;

namespace DataInteractor
{
    [Serializable]
    public class Interactor
    {
        private static Interactor interactor; // 数据交互管理器实例
        private object customData; // 自定义数据
        private FlowChartInteractor flowChartInteractor; // 流程图数据交互管理器实例
        private string rootDir; // 应用程序根目录

        /// <summary>
        /// 单例模式构造函数
        /// </summary>
        private Interactor()
        {

        }

        /// <summary>
        /// 应用程序根目录
        /// </summary>
        public string RootDir
        {
            set
            {
                rootDir = value;
            }

            get
            {
                return rootDir;
            }
        }

        /// <summary>
        /// 自定义数据
        /// </summary>
        public object CustomData
        {
            set
            {
                customData = value;
            }

            get
            {
                return customData;
            }
        }

        /// <summary>
        /// 流程图数据交互管理器实例
        /// </summary>
        public FlowChartInteractor CurrentFlowChartInteractor
        {
            set
            {
                flowChartInteractor = value;
            }

            get
            {
                return flowChartInteractor;
            }
        }

        /// <summary>
        /// 获取数据交互管理器实例
        /// </summary>
        /// <returns>数据交互管理器实例</returns>
        public static Interactor GetInteractor()
        {
            if(interactor == null)
            {
                interactor = new Interactor(); 
            }

            return interactor;
        }


    }
}
