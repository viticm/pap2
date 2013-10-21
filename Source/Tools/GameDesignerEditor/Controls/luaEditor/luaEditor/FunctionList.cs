using System;
using System.Collections.Generic;
using System.Text;

namespace luaEditor
{
    public class FunctionList
    {
        private readonly string functionName;
        private readonly string fullText;
        private readonly int lineNumber;

        public FunctionList(string functionName, string fullText, int lineNumber)
        {
            this.fullText = fullText;
            this.functionName = functionName;
            this.lineNumber = lineNumber;
        }

        public string FunctionName
        {
            get
            {
                return functionName;
            }
        }

        public string FullText
        {
            get
            {
                return fullText;
            }
        }

        public int LineNumber
        {
            get
            {
                return lineNumber;
            }
        }

        public override string ToString()
        {
            return functionName;
        }
    }
}
