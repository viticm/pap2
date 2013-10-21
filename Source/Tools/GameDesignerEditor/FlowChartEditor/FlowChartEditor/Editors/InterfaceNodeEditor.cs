using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Windows.Forms;

using DataElementLibrary;
using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    [Serializable]
    public class InterfaceNodeEditor : DataElement
    {
        private string interfaceName = ""; // 接口名称
        private bool isInput = true; // 是否是入口

        /// <summary>
        /// 接口名称
        /// </summary>
        public string InterfaceName
        {
            get
            {
                return interfaceName;
            }
        }

        /// <summary>
        /// 是否是入口
        /// </summary>
        public bool IsInput
        {
            get
            {
                return isInput;
            }
        }

        /// <summary>
        /// 初始化表现信息
        /// </summary>
        private void InitDisplayInfo()
        {
            string mark = "↓";
            description = "出口";

            if (isInput)
            {
                mark = "↑";
                description = "入口";
            }

            // text = string.Format("{0} {1}", mark, interfaceName);
            text = interfaceName;
            tooltipText = string.Format("<bold>类型\r\n{0}", description);
        }

        /// <summary>
        /// 编辑数据
        /// </summary>
        /// <param name="o">数据元的数据</param>
        /// <return>是否编辑成功</return>
        public override bool EditData(Hashtable table)
        {
            bool editSuccess = false;

            Hashtable infoTable = new Hashtable();        

            InterfaceNodeForm interfaceNodeForm = new InterfaceNodeForm(interfaceName, isInput);

            if (interfaceNodeForm.ShowDialog() == DialogResult.OK)
            {
                interfaceName = interfaceNodeForm.InterfaceName;
                isInput = interfaceNodeForm.IsInput;

                infoTable["interfaceName"] = interfaceName;
                infoTable["isInput"] = isInput;

                // 初始化表现信息
                InitDisplayInfo();

                this.data = infoTable;
                
                DocumentManager documentManager = DocumentManager.GetDocumentManager();
                InterfaceGraphElement interfaceGraphElement = documentManager.CurrentFlowChartManager.CurrentDataManager.FindGraphElementByID(id) as InterfaceGraphElement; 
                
                if (isInput)
                {
                    interfaceGraphElement.InSlotCount = 0;
                    interfaceGraphElement.OutSlotCount = 1;
                }
                else
                {
                    interfaceGraphElement.InSlotCount = 1;
                    interfaceGraphElement.OutSlotCount = 0;
                }

                // 锁定出口插槽
                interfaceGraphElement.LockOutSlot();

                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 重读数据
        /// </summary>
        /// <param name="table">当前数据</param>
        /// <return>是否重读成功</return>
        public override bool ReloadData(Hashtable table)
        {
            Hashtable infoTable = data as Hashtable;
            
            if (infoTable != null)
            {
                interfaceName = infoTable["interfaceName"] as string;
                isInput = (bool)infoTable["isInput"];

                // 初始化表现信息
                InitDisplayInfo();
            }             

            return true;
        }

        /// <summary>
        /// 获取输出的参数
        /// </summary>
        /// <returns>输出的参数</returns>
        public override Hashtable GetOutputArgs()
        {
            Hashtable infoTable = data as Hashtable;

            return infoTable;
        }
    }
}
