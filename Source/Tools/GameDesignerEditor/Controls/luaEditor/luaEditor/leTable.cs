using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace luaEditor
{
    public class leTableItem
    {
        public leTableItem(string name, string type, object value, string info)
        {
            this.name = name;
            this.type = type;
            this.value = value;
            this.info = info;
        }
        public string name;         //显示值
        public string type;         //类型
        public object value;        //实际输出值
        public string info;         //说明信息
    };
}
