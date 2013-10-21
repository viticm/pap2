using System;
using System.Collections.Generic;
using System.Text;

namespace FieldInfoLib
{
    [Serializable]
    public class FieldInfo
    {        
        private readonly string fieldID; // 数据库ID        
        private string fieldName; // 名称        
        private string fieldType; // 类型        
        private string typeData; // 类型数据        
        private string defaultValue; // 默认值        
        private string fieldValue; // 值        
        private bool allowNullValue; // 是否允许空值                

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="fieldID">数据库ID</param>
        /// <param name="fieldName">名称</param>
        /// <param name="fieldType">类型</param>
        /// <param name="typeData">类型数据</param>
        /// <param name="defaultValue">默认值</param>
        /// <param name="allowNullValue">是否允许空值</param>
        public FieldInfo(string fieldID, string fieldName, string fieldType, string typeData, string defaultValue, bool allowNullValue)
        {
            this.fieldID = fieldID;
            this.fieldName = fieldName;
            this.fieldType = fieldType;
            this.typeData = typeData;
            this.defaultValue = defaultValue;
            this.allowNullValue = allowNullValue;
        }

        /// <summary>
        /// 数据库ID
        /// </summary>
        public string FieldID
        {
            get
            {
                return fieldID;
            }
        }

        /// <summary>
        /// 名称
        /// </summary>
        public string FieldName
        {
            get
            {
                return fieldName;
            }
        }

        /// <summary>
        /// 类型
        /// </summary>
        public string FieldType
        {
            get
            {
                return fieldType;
            }
        }

        /// <summary>
        /// 类型数据
        /// </summary>
        public string TypeData
        {
            get
            {
                return typeData;
            }
        }

        /// <summary>
        /// 值
        /// </summary>
        public string FieldValue
        {
            set
            {
                fieldValue = value;
            }

            get
            {
                return fieldValue;
            }
        }

        /// <summary>
        /// 是否允许空值
        /// </summary>
        public bool AllowNullValue
        {
            get
            {
                return allowNullValue;
            }
        }
    }
}
