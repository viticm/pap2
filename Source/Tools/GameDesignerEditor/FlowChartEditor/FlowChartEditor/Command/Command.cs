using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace FlowChartEditor.Command
{
    abstract public class Command
    {
        protected string description; // 命令的描述
        protected object data; // 当前对象
        protected Memento dataBeforeExecute; // 命令执行前的数据
        protected Memento dataAfterExecute; // 命令执行后的数据
        protected bool firstCommand = false; // 是否是第一个命令         

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="data">当前对象</param>
        /// <param name="description">命令的描述</param>
        public Command(object data, string description)
        {
            this.data = data;
            this.description = description;
        }

        /// <summary>
        /// 命令的描述属性
        /// </summary>
        public virtual string Description
        {
            get
            {
                return description;
            }
        }

        /// <summary>
        /// 是否是第一个命令属性
        /// </summary>
        public virtual bool FirstCommand
        {
            set
            {
                firstCommand = value;
            }

            get
            {
                return firstCommand;
            }
        }

        /// <summary>
        /// 在命令执行前保存当前对象的数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected abstract void SaveBeforeExecute(object o);

        /// <summary>
        /// 在命令执行后保存当前对象的数据
        /// </summary>
        /// <param name="o">当前对象</param>
        protected abstract void SaveAfterExecute(object o);

        /// <summary>
        /// 执行命令
        /// </summary>
        /// <param name="o">当前对象</param>
        /// <return>是否执行成功</return>
        public abstract bool Execute(object o);

        /// <summary>
        /// 执行命令
        /// </summary>
        public abstract void Execute();

        /// <summary>
        /// 取消执行命令
        /// </summary>
        public abstract void Unexecute();
    }
}
