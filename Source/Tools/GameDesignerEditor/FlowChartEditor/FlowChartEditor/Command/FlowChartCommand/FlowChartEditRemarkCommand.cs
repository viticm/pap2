using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;
using FlowChartEditor.BaseGraphElement;
using GraphElementLibrary;
using DataElementLibrary;

namespace FlowChartEditor.Command.FlowChartCommand
{
    class FlowChartEditRemarkCommand:FlowChartBaseCommand
    {
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public FlowChartEditRemarkCommand(object data, string description):
            base(data, description)
        {
            
        }

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">命令的参数</param>
        /// <returns>是否执行成功</returns>
        public override bool Execute(object o)
        {
            bool success = false;           
            SlotContainer slotContainer = o as SlotContainer;
            string remark = "";

            try
            {
                Assembly assembly = Assembly.LoadFrom("Plugins\\FlowChart\\TextEditor.dll");
                DataElement dataElement = assembly.CreateInstance("TextEditor.TextEditor") as DataElement;
                Hashtable table = new Hashtable();
                table["data"] = slotContainer.Remark;                
                if(dataElement.EditData(table))
                {
                    success = true;
                    remark = dataElement.Data as string;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("在编辑图元注释时产生异常: " + ex.ToString(), "注释编辑", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }

            // 创建注释结点
            if(success)
            {
                // 保存执行前的数据
                description = "编辑图元注释 " + slotContainer.Name;
                if (firstCommand) // 只有第一条命令保存执行前的数据
                {
                    SaveBeforeExecute(flowChartManager.GetArguments());                    
                }   
                
                slotContainer.Remark = remark;
                RemarkGraphElement remarkGraphElement;
                if (slotContainer.RemarkNode == null) // 需要创建注释结点
                {
                    remarkGraphElement = new RemarkGraphElement(slotContainer, slotContainer.Location + new Size(slotContainer.ElementSize.Width + 50, 0),
                                                                new Size(1, 1));
                    remarkGraphElement.Text = remark;
                    remarkGraphElement.Name = "注释结点";
                    remarkGraphElement.Enable = true;                    
                    remarkGraphElement.Refresh();
                    remarkGraphElement.AdjustElementSize();
                    remarkGraphElement.AdjustLine();
                    slotContainer.RemarkNode = remarkGraphElement;
                    slotContainer.ShowRemark = true;
                }
                else
                {                    
                    remarkGraphElement = slotContainer.RemarkNode;
                    remarkGraphElement.Text = remark;
                    remarkGraphElement.AdjustElementSize();
                }                                                                          

                // 保存执行后的数据
                flowChartManager.ContentChanged = true;
                SaveAfterExecute(flowChartManager.GetArguments());            
            }

            return success;
        }
    }
}
