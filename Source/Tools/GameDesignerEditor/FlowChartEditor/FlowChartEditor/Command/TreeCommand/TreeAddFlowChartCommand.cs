using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using DBManager;

namespace FlowChartEditor.Command.TreeCommand
{
    class TreeAddFlowChartCommand : TreeBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前数据</param>
        /// <param name="description">命令描述</param>
        public TreeAddFlowChartCommand(object data, string description):
            base(data, description)
        {

        }        

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = true;

            // 执行逻辑操作
            LogicBaseManager logicManager = Helper.GetHelper().GetLogicManager(documentManager.CurrentChartMode);
            logicManager.DoLogicOperation(documentManager.CurrentFlowChartManager, LogicType.CreateChart, o);            

            return success;
        }
    }
}
