using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using FieldInfoLib;

namespace GuideTool
{
    class FieldControl
    {
        private Control controlInstance;
        private FieldInfo fieldInfoInstance;

        /// <summary>
        /// 封装的选项类
        /// </summary>
        /// <param name="controlInstance">控件实例</param>
        /// <param name="fieldInfoInstance">选项数据类实例</param>
        public FieldControl(Control controlInstance, FieldInfo fieldInfoInstance)
        {
            this.controlInstance = controlInstance;
            this.fieldInfoInstance = fieldInfoInstance;
        }

        /// <summary>
        /// 控件是否有效
        /// </summary>
        public bool Enable
        {
            set
            {
                controlInstance.Enabled = value;
            }

            get
            {
                return controlInstance.Enabled;
            }
        }

        /// <summary>
        /// 选项值
        /// </summary>
        public string Value
        {
            set
            {
                controlInstance.Text = value;
            }

            get
            {
                return controlInstance.Text;
            }
        }    
        
        /// <summary>
        /// 选项类型
        /// </summary>
        public string FieldType
        {
            get
            {
                return fieldInfoInstance.FieldType;
            }
        }

        /// <summary>
        /// 选项名称
        /// </summary>
        public string Name
        {
            get
            {
                return fieldInfoInstance.FieldName;                
            }
        }        
    }
}
