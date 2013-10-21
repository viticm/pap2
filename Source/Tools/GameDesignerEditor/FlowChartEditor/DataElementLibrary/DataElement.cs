using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace DataElementLibrary
{
    [Serializable]
    public abstract class DataElement
    {
        protected string name; // 数据元名称
        protected string description; // 数据元的描述
        protected string dataType; // 数据元的类型
        protected object data; // 数据元的数据
        protected string text = ""; // 数据元文本
        protected string tooltipText = ""; // 数据元的提示文本
        protected int id; // 数据元的id

        public delegate void PrintInfo(string text); // 输出调试信息
        [NonSerialized]
        protected PrintInfo printInfo; // 输出调试信息的回调实例

        /// <summary>
        /// 数据元的提示文本
        /// </summary>
        public virtual string TooltipText
        {
            set
            {
                tooltipText = value;
            }

            get
            {
                return tooltipText;
            }
        }

        /// <summary>
        /// 数据元文本属性
        /// </summary>
        public virtual string Text
        {
            set
            {
                text = value;
            }

            get
            {
                return text;
            }
        }

        /// <summary>
        /// 数据元的名称属性
        /// </summary>
        public virtual string Name
        {
            set
            {
                name = value;
            }

            get
            {
                return name;
            }
        }

        /// <summary>
        /// 数据元的描述属性
        /// </summary>
        public virtual string Description
        {
            set
            {
                description = value;
            }

            get
            {
                return description;
            }
        }

        /// <summary>
        /// 数据元的数据属性
        /// </summary>
        public virtual object Data
        {
            set
            {
                data = value;
            }

            get
            {
                return data;
            }
        }

        /// <summary>
        /// 数据元的id属性
        /// </summary>
        public virtual int ID
        {
            set
            {
                id = value;
            }

            get
            {
                return id;
            }
        }

        /// <summary>
        /// 输出调试信息的回调
        /// </summary>
        public virtual PrintInfo PrintInformation
        {
            set
            {
                printInfo = value;
            }

            get
            {
                return printInfo;
            }
        }

        /// <summary>
        /// 数据元的数据类型
        /// </summary>
        public virtual string DataType
        {
            set
            {
                dataType = value;
            }

            get
            {
                return dataType;
            }
        }

        /// <summary>
        /// 编辑数据
        /// </summary>
        /// <param name="o">数据元的数据</param>
        /// <return>是否编辑成功</return>
        public abstract bool EditData(Hashtable table);

        /// <summary>
        /// 重读数据
        /// </summary>
        /// <param name="table">当前数据</param>
        /// <return>是否重读成功</return>
        public abstract bool ReloadData(Hashtable table);

        /// <summary>
        /// 获取输出的参数
        /// </summary>
        /// <returns>输出的参数</returns>
        public abstract Hashtable GetOutputArgs();
    }
}
