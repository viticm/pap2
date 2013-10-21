using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartDisconnectCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartDisconnectCommand(object data, string description):
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
            Helper helper = Helper.GetHelper();            
            SlotGraphElement slot = o as SlotGraphElement;

            // 保存命令执行前的数据
            if (firstCommand) // 只有第一条命令保存执行前的数据
            {
                SaveBeforeExecute(flowChartManager.GetArguments());                
            }

            ConnectorGraphElement connector = slot.BindingConnector;
            ConnectorContainer line = connector.Line;
            SlotContainer slotContainer = slot.SlotContainer;
            description = "解除连接图元 " + slotContainer.Name;

            slot.UnBind();
            if(slot.CanDelete && (slot.IsInSlot || slot.SlotContainer.OutSlotCount > 1)) // 插槽可以删除
            {
                slotContainer.RemoveSlot(slot);                
            }                                   

            // 执行逻辑操作
            LogicBaseManager logicManager = helper.GetLogicManager(flowChartManager.CurrentChartMode);
            object[] logicData = new object[] { slot, connector };
            logicManager.DoLogicOperation(flowChartManager, LogicType.Disconnect, logicData);

            if(success) // 保存命令执行后的数据
            {
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());                
            }                        

            return success;
        }
    }
}
