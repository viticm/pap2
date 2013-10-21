using System;
using System.Collections.Generic;
using System.Text;

namespace FlowChartDataLibrary
{
    /// <summary>
    /// 流程图元数据类
    /// </summary>
    public class FlowChartMetaData
    {
        private readonly int id; // ID
        private readonly object data; // 数据
        private readonly string dataType; // 数据类型
        private string code = ""; // 生成代码
        private string displayText = ""; // 显示的文本
        private List<FlowChartMetaData> nextMetaDataList = new List<FlowChartMetaData>(); // 连入的流程图元数据链表
        private List<FlowChartMetaData> previousMetaDataList = new List<FlowChartMetaData>(); // 连出的流程图元数据链表

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="id">id</param>
        /// <param name="data">数据</param>
        /// <param name="dataType">数据类型</param>
        public FlowChartMetaData(int id, object data, string dataType)
        {
            this.id = id;
            this.data = data;
            this.dataType = dataType;
        }

        /// <summary>
        /// 显示的文本属性
        /// </summary>
        public string DisplayText
        {
            set
            {
                displayText = value;
            }

            get
            {
                return displayText;
            }
        }

        /// <summary>
        /// 生成代码属性
        /// </summary>
        public string Code
        {
            set
            {
                code = value;
            }

            get
            {
                return code;
            }
        }

        /// <summary>
        /// ID属性
        /// </summary>
        public int ID
        {
            get
            {
                return id;
            }
        }

        /// <summary>
        /// 数据属性
        /// </summary>
        public object Data
        {
            get
            {
                return data;
            }
        }

        /// <summary>
        /// 数据类型属性
        /// </summary>
        public string DataType
        {
            get
            {
                return dataType;
            }
        }

        /// <summary>
        /// 连入的流程图元数据链表
        /// </summary>
        public List<FlowChartMetaData> NextMetaDataList
        {
            get
            {
                return nextMetaDataList;
            }
        }

        /// <summary>
        /// 连出的流程图元数据链表
        /// </summary>
        public List<FlowChartMetaData> PreviousMetaDataList
        {
            get
            {                
                return previousMetaDataList;
            }
        }

        /// <summary>
        /// 添加连入流程图的元数据
        /// </summary>
        /// <param name="metaData">元数据</param>
        public void AddPreviousMetaData(FlowChartMetaData metaData)
        {
            if(!previousMetaDataList.Contains(metaData))
            {
                previousMetaDataList.Add(metaData);
            }
        }

        /// <summary>
        /// 添加连出流程图的元数据
        /// </summary>
        /// <param name="metaData">元数据</param>
        public void AddNextMetaData(FlowChartMetaData metaData)
        {
            if(!nextMetaDataList.Contains(metaData))
            {
                nextMetaDataList.Add(metaData);
            }
        }

        /// <summary>
        /// 清理连入流程图的元数据链表
        /// </summary>
        public void ClearPreviousMetaDataList()
        {
            previousMetaDataList.Clear();
        }

        /// <summary>
        /// 清理连出流程图的元数据链表
        /// </summary>
        public void ClearNextMetaDataList()
        {
            nextMetaDataList.Clear();
        }
    }
}
