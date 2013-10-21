using System;
using System.Collections.Generic;
using System.Text;

namespace ExportUITable
{
    public class TableInfo
    {
        private readonly string filePath;
        private readonly List<string> includeFieldList;

        public TableInfo(string filePath, List<string> includeFieldList)
        {
            this.filePath = filePath;
            this.includeFieldList = includeFieldList;
        }

        public string FilePath
        {
            get
            {
                return filePath;
            }
        }

        public List<string> IncludeFieldList
        {
            get
            {
                return includeFieldList;
            }
        }
    }
}
