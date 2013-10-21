using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace luaEditor
{
    public class LuaEditorTableItem
    {
        private readonly string name; // 显示值
        private readonly string itemType; // 类型
        private readonly object itemValue; // 实际输出值
        private readonly string info; // 说明信息

        public LuaEditorTableItem(string name, string type, object value, string info)
        {
            this.name = name;
            this.itemType = type;
            this.itemValue = value;
            this.info = info;
        }

        public string Name
        {
            get
            {
                return name;
            }
        }

        public string ItemType
        {
            get
            {
                return itemType;
            }
        }

        public object ItemValue
        {
            get
            {
                return itemValue;
            }
        }

        public string Info
        {
            get
            {
                return info;
            }
        }
    };
}
