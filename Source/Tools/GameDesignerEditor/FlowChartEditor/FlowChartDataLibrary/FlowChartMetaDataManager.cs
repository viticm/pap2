using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace FlowChartDataLibrary
{
    public class FlowChartMetaDataManager
    {
        private int flowChartID; // 流程图的ID
        private List<FlowChartMetaData> entranceNodeList; // 入口结点元数据链表
        private Hashtable eventTable = new Hashtable(); // 入口结点的哈希表 
        private string mapName; // 场景名称
        private int version; // 版本号

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="flowChartID">流程图的ID</param>
        /// <param name="entranceNodeList">入口结点元数据链表</param>
        public FlowChartMetaDataManager(int flowChartID, List<FlowChartMetaData> entranceNodeList)
        {
            this.flowChartID = flowChartID;
            this.entranceNodeList = entranceNodeList;
            Init();
        }        

        /// <summary>
        /// 场景名称
        /// </summary>
        public string MapName
        {
            set
            {
                mapName = value;
            }

            get
            {
                return mapName;
            }
        }

        /// <summary>
        /// 版本号
        /// </summary>
        public int Version
        {
            set
            {
                version = value;
            }

            get
            {
                return version;
            }
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            foreach (FlowChartMetaData eventNode in entranceNodeList)
            {
                List<FlowChartMetaData> list = new List<FlowChartMetaData>();
                foreach (FlowChartMetaData metaData in eventNode.NextMetaDataList)
                {
                    GetNodeNames(metaData, list);
                }
                eventTable[eventNode] = list;
            }
        }

        /// <summary>
        /// 获取本流程图中一共有多少个入口结点
        /// </summary>
        /// <returns>入口结点个数</returns>
        public int GetEventCount()
        {
            return entranceNodeList.Count;
        }

        /// <summary>
        /// 获取事件函数的名字
        /// </summary>
        /// <param name="i">事件函数的序号</param>
        /// <returns>事件函数的名字</returns>
        public string GetEventFunctionName(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            string functionName = eventNode.DataType + "_" + flowChartID + "_" + eventNode.ID;
            return functionName;
        }

        /// <summary>
        /// 将所有后续元数据加入链表
        /// </summary>
        /// <param name="metaData">当前元数据</param>
        /// <param name="list">保存元数据的链表</param>
        private void GetNodeNames(FlowChartMetaData metaData, List<FlowChartMetaData> list)
        {
            if(!list.Contains(metaData))
            {
                if(metaData.DataType != "Line" && metaData.DataType != "AILine")
                {
                    list.Add(metaData);
                }
                
                foreach(FlowChartMetaData d in metaData.NextMetaDataList)
                {
                    GetNodeNames(d, list);
                }
            }
        }

        /// <summary>
        /// 获取事件的所有的节点的名字
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <returns>所有结点的名字</returns>
        public string GetNodeNames(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;

            string nodeNames = "";
            foreach(FlowChartMetaData metaData in list)
            {
                nodeNames += metaData.DataType + "_" + flowChartID + "_" + metaData.ID + ", ";
            }
            nodeNames = nodeNames.TrimEnd(new char[] { ',', ' ' });

            return nodeNames;
        }

        /// <summary>
        /// 获取事件的节点总数
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <returns>所有结点的名字</returns>
        public int GetNodeCount(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;

            return list.Count;
        }

        /// <summary>
        /// 获取事件的结点的名字
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>结点的名字</returns>
        public string GetNodeName(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];

            string nodeName = metaData.DataType + "_" + flowChartID + "_" + metaData.ID;
            return nodeName;
        }

        /// <summary>
        /// 获取事件的的结点的类型
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>结点的类型</returns>
        public string GetNodeType(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];

            return metaData.DataType;
        }

        /// <summary>
        /// 获取事件的的节点的生成代码
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>结点的生成代码</returns>
        public string GetNodeCode(int i , int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];            

            return metaData.Code;
        }

        /// <summary>
        /// 获取事件的节点的后续连接线的数量
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>结点后续连接线的数量</returns>
        public int GetNextConnetionCount(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            int count;

            if(j == 0) // 直接取入口结点
            {
                count = eventNode.NextMetaDataList.Count;
            }
            else
            {
                List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
                FlowChartMetaData metaData = list[j - 1];
                count = metaData.NextMetaDataList.Count;
            }
            
            return count;
        }

        /// <summary>
        /// 获取当前结点下一个结点的集合
        /// </summary>
        /// <param name="metaData">当前结点</param>
        /// <param name="list">保存集合的链表</param>
        public void GetNextNode(FlowChartMetaData metaData, List<FlowChartMetaData> list)
        {
            foreach(FlowChartMetaData m in metaData.NextMetaDataList)
            {
                foreach(FlowChartMetaData n in m.NextMetaDataList)
                {
                    list.Add(n);
                }
            }
        }

        /// <summary>
        /// 获取事件的的节点的连接线连接的后续节点类型是否结束节点
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <param name="k">连接线的序号</param>
        /// <returns>是否是结束结点</returns>
        public bool IsNextNodeFinish(int i, int j, int k)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];
            FlowChartMetaData line = metaData.NextMetaDataList[k - 1];
            FlowChartMetaData newMetaData = line.NextMetaDataList[0];

            bool result = newMetaData.DataType == "EndNode";

            return result;
        }

        /// <summary>
        /// 获取事件的节点的后续节点名
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <param name="k">后续结点的序号</param>
        /// <returns>结点名</returns>
        public string GetNextNodeName(int i, int j, int k)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            string nodeName;

            if(j == 0) // 直接取当前入口结点
            {
                List<FlowChartMetaData> list = new List<FlowChartMetaData>();
                GetNextNode(eventNode, list);
                FlowChartMetaData nextMetaData = list[k - 1];

                nodeName = nextMetaData.DataType + "_" + flowChartID + "_" + nextMetaData.ID; 
            }
            else
            {
                List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
                FlowChartMetaData metaData = list[j - 1];
                list = new List<FlowChartMetaData>();
                GetNextNode(metaData, list);
                FlowChartMetaData nextMetaData = list[k - 1];

                nodeName = nextMetaData.DataType + "_" + flowChartID + "_" + nextMetaData.ID;
            }
            
            return nodeName;
        }

        /// <summary>
        /// 获取事件的节点的后续执行线程数量
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>执行线程数量</returns>
        public int GetNextThreadCount(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];

            return metaData.NextMetaDataList.Count;
        }

        /// <summary>
        /// 获取事件的节点的分支执行流程数量
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>分支执行流程数量</returns>
        public int GetNodeSwitchCount(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];

            return metaData.NextMetaDataList.Count;
        }

        /// <summary>
        /// 获取事件的节点的连接线上的条件内容
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <param name="k">条件的序号</param>
        /// <returns>条件内容</returns>
        public string GetConditionResult(int i,int j, int k)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];
            FlowChartMetaData nextMetaData = metaData.NextMetaDataList[k - 1];

            return nextMetaData.Code;
        }

        /// <summary>
        /// 获取事件的结点的显示文本
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <param name="j">结点的序号</param>
        /// <returns>显示文本</returns>
        public string GetNodeText(int i, int j)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            List<FlowChartMetaData> list = eventTable[eventNode] as List<FlowChartMetaData>;
            FlowChartMetaData metaData = list[j - 1];
            string text = metaData.DisplayText;
            if(text == null)
            {
                text = "";
            }

            return text;
        }

        /// <summary>
        /// 获取事件的显示文本
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <returns>显示文本</returns>
        public string GetEventText(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            string text = eventNode.DisplayText;
            if (text == null)
            {
                text = "";
            }

            return text;
        }

        /// <summary>
        /// 获取事件的内容
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <returns>事件的内容</returns>
        public object GetEventData(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            return eventNode.Data;
        }

        /// <summary>
        /// 获取事件元数据
        /// </summary>
        /// <param name="i">事件的序号</param>
        /// <returns>事件元数据</returns>
        public FlowChartMetaData GetEventNode(int i)
        {
            FlowChartMetaData eventNode = entranceNodeList[i - 1];
            return eventNode;
        }

        /// <summary>
        /// 获取所有的元数据链表
        /// </summary>
        /// <returns>所有的元数据链表</returns>
        public List<FlowChartMetaData> GetAllNodes()
        {
            List<FlowChartMetaData> list = new List<FlowChartMetaData>();

            foreach(FlowChartMetaData eventNode in entranceNodeList)
            {
                GetNodeNames(eventNode, list);
            }

            return list;
        }
    }
}
