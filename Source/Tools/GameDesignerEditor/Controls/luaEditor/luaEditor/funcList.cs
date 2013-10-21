using System;
using System.Collections.Generic;
using System.Text;

namespace luaEditor
{
    public class funcList
    {

        public funcList(string funcName, string fullText, int nlineNumber)
        {
            this.fullText = fullText;
            this.funcName = funcName;
            this.nlineNumber = nlineNumber;
        }

        public override string ToString()
        {
            return fullText;
        }

        public string funcName;
        public string fullText;
        public int nlineNumber;
    }
}
