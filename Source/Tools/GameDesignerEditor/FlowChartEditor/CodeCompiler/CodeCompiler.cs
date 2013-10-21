using System;
using System.Collections.Generic;
using System.Text;

using FlowChartDataLibrary;

namespace CodeCompiler
{
    public abstract class Compiler
    {
        public delegate void PrintDebugInfo(string text); // 输出调试信息
        protected PrintDebugInfo printDebugInfo; // 输出调试信息的回调实例

        public delegate void SaveFile(string fileName, string content); // 将文本保存到文件中
        protected SaveFile saveFile; // 将文本保存到文件中的回调实例

        protected string rootDirectory; // 根目录
        protected string scriptPath; // 脚本路径

        /// <summary>
        /// 根目录
        /// </summary>
        public string RootDirectory
        {
            set
            {
                rootDirectory = value;
            }

            get
            {
                return rootDirectory;
            }
        }

        /// <summary>
        /// 脚本路径
        /// </summary>
        public string ScriptPath
        {
            set
            {
                scriptPath = value;
            }

            get
            {
                return scriptPath;
            }
        }

        /// <summary>
        /// 输出调试信息的回调
        /// </summary>
        public virtual PrintDebugInfo PrintDebugInformation
        {
            set
            {
                printDebugInfo = value;
            }

            get
            {
                return printDebugInfo;
            }
        }

        /// <summary>
        /// 将文本保存到文件中
        /// </summary>
        public virtual SaveFile SaveTheFile
        {
            set
            {
                saveFile = value;
            }

            get
            {
                return saveFile;
            }
        }

        /// <summary>
        /// 生成脚本
        /// </summary>
        /// <param name="chartID">绘图ID</param>
        /// <param name="data">绘图数据</param>
        /// <param name="manager">绘图数据管理器</param>
        /// <returns>是否生成成功</returns>
        public abstract bool GenerateCode(int chartID, List<FlowChartMetaData> data, FlowChartMetaDataManager manager);
    }
}
