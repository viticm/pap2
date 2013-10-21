using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Text.RegularExpressions;

namespace MapLogicalCompare
{
    class IniReader
    {
        private Hashtable sectionTable = new Hashtable();        

        private string[] sectionNames;
        private string fileName; // 文件名
        private string fileContents = null; // 文本内容

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="fileName">文件名</param>
        public IniReader(string fileName)
        {
            this.fileName = fileName;
            Init();            
        }

        public string FileContent
        {
            get
            {
                return fileContents;
            }
        }

        private void Init()
        {
            if (File.Exists(fileName))
            {
                StreamReader sr = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                fileContents = sr.ReadToEnd();
                sr.Close();

                // 用正则表达式匹配实在是太慢了，自己写一个试下
                string currentSection = null;
                StringBuilder sectionContent = null;
                string[] lines = fileContents.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < lines.Length; i++)
                {
                    string line = lines[i];
                    line = line.Substring(0, line.Length - 1);

                    if (line.StartsWith("[") && line.EndsWith("]"))
                    {
                        currentSection = line.Substring(1, line.Length - 2);
                        sectionTable[currentSection] = new Hashtable();
                        sectionContent = new StringBuilder();
                    }
                    else if (line == "")
                    {
                        Hashtable keyTable = sectionTable[currentSection] as Hashtable;
                        keyTable["SectionContent"] = sectionContent.ToString(); 
                        currentSection = null;                        
                    }
                    else
                    {
                        int index = line.IndexOf("=");
                        string key = line.Substring(0, index);
                        string value = line.Substring(index + 1, line.Length - index - 1);
                        sectionContent.AppendLine(line);

                        Hashtable keyTable = sectionTable[currentSection] as Hashtable;
                        keyTable[key] = value; 

                        if (i == lines.Length - 1)
                        {
                            keyTable["SectionContent"] = sectionContent.ToString();
                        }
                    }
                }
            }   
        }

        /// <summary>   
        /// Get section names in an array, Read only   
        /// </summary>   
        public string[] SectionNames
        {
            get
            {
                if (sectionNames == null)
                {
                    /*
                    // Using regular expression to get all section names.   
                    string regexPattern = @"\[(?<SectionName>\w*(\w|\s)*)\]";

                    Regex r = new Regex(regexPattern); // Match "[anywords]"   
                    MatchCollection matches = r.Matches(fileContents);

                    // Writing all section names to a string array.   
                    string[] results = new string[matches.Count];

                    for (int i = 0; i < matches.Count; i++)
                    {
                        results[i] = matches[i].Result("${SectionName}");
                    }
                    
                    sectionNames = results; 
                    */

                    sectionNames = new string[sectionTable.Keys.Count];

                    int index = 0;
                    foreach (string key in sectionTable.Keys)
                    {
                        sectionNames[index] = key;
                        index++;
                    }
                }

                return sectionNames;
            }
        }

        public string this[string sectionName]
        {
            get
            {
                // return GetSectionString(sectionName);
                string sectionValue = "";

                if (sectionTable[sectionName] != null)
                {
                    Hashtable keyTable = sectionTable[sectionName] as Hashtable;
                    if (keyTable["SectionContent"] != null)
                    {
                        sectionValue = keyTable["SectionContent"] as string;
                    }
                }

                return sectionValue;
            }
        }

        public string this[string sectionName, string keyName]
        {
            get
            {
                string keyValue = "";

                if (sectionTable[sectionName] != null)
                {
                    Hashtable keyTable = sectionTable[sectionName] as Hashtable;
                    
                    if (keyTable[keyName] != null)
                    {
                        keyValue = keyTable[keyName] as string;
                    }
                }

                return keyValue;

                // return GetKeyString(sectionName, keyName);
            }
        }        

        /// <summary>   
        /// Get the whole string in one section   
        /// </summary>   
        /// <param name="sectionName">分类名称</param>   
        /// <returns>分类内容</returns>   
        private string GetSectionString(string sectionName)
        {
            //string regexPattern = @"(\[" + sectionName + @"\]"
              //+ @"(?<SectionString>.*)\[)";
            string regexPattern = @"(\[" + sectionName + @"\]"
              + @"(?<SectionString>.*?)\r\n\r\n)";

            Regex r = new Regex(regexPattern, RegexOptions.Singleline);
            //Regex r = new Regex(regexPattern);
            
            if (r.IsMatch(fileContents))
            {
                return r.Match(fileContents).Result("${SectionString}");
            }
            return string.Empty;
        }

        /// <summary>   
        /// Get the string value of certain key in certain section   
        /// </summary>   
        /// <param name="sectionName">分类名称</param>   
        /// <param name="keyName">属性名称</param>   
        /// <returns>属性值</returns>   
        private string GetKeyString(string sectionName, string keyName)
        {
            string sectionString = this.GetSectionString(sectionName) + "\r\n";
            string regexPattern = @"(" + keyName + @"=(?<value>.*)\r\n)";

            Regex r = new Regex(regexPattern);
            if (r.IsMatch(sectionString))
            {
                return r.Match(sectionString).Result("${value}");
            }

            return string.Empty;
        } 
  
        /// <summary>
        /// 获取特定分类下的属性名集合
        /// </summary>
        /// <param name="sectionName">分类名称</param>
        /// <returns>属性名集合</returns>
        private string[] GetKeys(string sectionName)
        {
            string[] keys;
            string sectionString = this.GetSectionString(sectionName) + "\r\n";
            string regexPattern = @"(" + "(?<value>.*)" + @"=(\w+)\r\n)";
            Regex r = new Regex(regexPattern);
            MatchCollection matches = r.Matches(sectionString);

            keys = new string[matches.Count];
            for (int i = 0; i < matches.Count; i++)
            {
                keys[i] = matches[i].Result("${value}");
            }
            return keys;
        }
    }
}
