using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;

namespace AIEnumEditor
{
    public class Program
    {
        private EnumValuesEditor enumValuesEditor; // 枚举值列表编辑器
        private AIChartSelector aiChartSelector; // AI图选择器
        private EnumValueSelector enumValueSelector; // 枚举值选择器
        private AIArgumentsEditor aiArgumentEditor; // AI参数编辑器
        private ParameterEditor parameterEditor; // 动作参数编辑器
        private CodeWriter codeWriter; // 代码转换器

        public Program()
        {

        }

        /// <summary>
        /// 动作参数
        /// </summary>
        public string ParameterValue
        {
            get
            {
                return parameterEditor.NewValue;
            }
        }

        /// <summary>
        /// 枚举值列表
        /// </summary>
        public string EnumValues
        {
            get
            {
                return enumValuesEditor.NewValue;
            }
        }

        /// <summary>
        /// AI图的ID
        /// </summary>
        public string AIChartID
        {
            get
            {
                return aiChartSelector.AIChartID;
            }
        }

        /// <summary>
        /// AI图的路径
        /// </summary>
        public string AIChartPath
        {
            get
            {
                return aiChartSelector.AIChartPath;
            }
        }

        /// <summary>
        /// 枚举值的序号
        /// </summary>
        public int EnumValueIndex
        {
            get
            {
                return enumValueSelector.EnumValueIndex;
            }
        }

        /// <summary>
        /// 枚举值的字符串
        /// </summary>
        public string EnumValueString
        {
            get
            {
                return enumValueSelector.EnumValueString;
            }
        }

        /// <summary>
        /// AI参数
        /// </summary>
        public string AIArguments
        {
            get
            {
                return aiArgumentEditor.AIArguments;
            }
        }

        /// <summary>
        /// 编辑枚举值列表
        /// </summary>
        /// <param name="oldValue">旧值</param>
        /// <returns>是否编辑成功</returns>
        public bool EditEnumValues(string oldValue)
        {
            bool editSuccess = false;
            
            enumValuesEditor = new EnumValuesEditor(oldValue);
            DialogResult result = enumValuesEditor.ShowDialog();

            if(result == DialogResult.OK)
            {
                editSuccess = true;
            }

            return editSuccess;
        }

        /// <summary>
        /// 选择AI图
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="id">当前AI图ID</param>
        /// <returns>是否选择成功</returns>
        public bool SelectAIChart(SqlConnection conn, string id)
        {
            bool selected = false;

            aiChartSelector = new AIChartSelector(conn, id);
            DialogResult result = aiChartSelector.ShowDialog();

            if(result == DialogResult.OK)
            {
                selected = true;
            }

            return selected;
        }

        /// <summary>
        /// 编辑AI参数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="diagramPath">流程图路径</param>        
        /// <param name="oldValue">旧值</param>
        /// <param name="filtField">过滤的字段</param>
        /// <returns>是否编辑成功</returns>
        public bool EditAIParam(SqlConnection conn, string diagramPath, string oldValue, string filtField)
        {
            bool success = false;

            aiArgumentEditor = new AIArgumentsEditor(conn, diagramPath, oldValue, filtField);
            DialogResult result = aiArgumentEditor.ShowDialog();
            
            if(result == DialogResult.OK)
            {
                success = true;
            }

            return success;
        }        

        /// <summary>
        /// 编辑动作参数
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="oldValue">旧值</param>
        /// <returns>是否编辑成功</returns>
        public bool EditParameter(SqlConnection conn, string oldValue)
        {
            bool success = false;

            parameterEditor = new ParameterEditor(conn, oldValue);
            DialogResult result = parameterEditor.ShowDialog();

            if(result == DialogResult.OK)
            {
                success = true;
            }

            return success;
        }

        /// <summary>
        /// 获取参数字符串
        /// </summary>
        /// <param name="diagramPath">流程图路径</param>
        /// <param name="value">原参数值</param>
        /// <returns>参数字符串</returns>
        public string GetArgumentsValue(string diagramPath, string value)
        {
            if(codeWriter == null)
            {
                codeWriter = new CodeWriter();
            }

            string newValue = codeWriter.GetArgumentsValue(diagramPath, value);
            return newValue;
        }

        /// <summary>
        /// 导出AI文件
        /// </summary>
        /// <param name="rootDir">客户端根目录</param>
        /// <param name="conn">sql连接</param>
        /// <returns>是否导出成功</returns>
        public bool ExportAIFile(string rootDir, SqlConnection conn)
        {
            bool success = false;

            AIFileWriter aiFileWriter = new AIFileWriter(rootDir, conn);
            aiFileWriter.ExportLSFile();
            aiFileWriter.ExportCustomFunctionsFile();

            return success;
        }
    }
}
