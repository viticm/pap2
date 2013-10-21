using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;

namespace UpdateMiddleMapNPCxyz
{
    struct PointXY
    {
        public int nX;
        public int nY;
    };

    //为指定地图生成npc索引
    //key是nTempleteID,value是xy坐标点例如 ht[123] = PointXY
    //对于有多个nTempleteID相同的，只取了最先读到的第一个
    class NpcIndex
    {
        private string strMapName;
        private string strRootDir;

        private string strErrorMsg;
        private bool bHasError = false;

        const int MaxNpcCapcity = 199;
        private Hashtable htIndex = new Hashtable(MaxNpcCapcity);//key:nTemepleteId,value:[NPC**]
        
        public string RootDir
        {
            set
            {
                if (Directory.Exists(value))
                    strRootDir = value;
                else
                {
                    strRootDir = "";
                    strErrorMsg = string.Format("Dictinory [{0}] is not exists!\r\n", value);
                    bHasError = true;
                }
            }
        }

        public string MapName
        {
            set
            {
                string strMapLogicalFileFullName = string.Format("{0}\\data\\source\\maps\\{1}\\{1}.Map.Logical", strRootDir, value);
                if (File.Exists(strMapLogicalFileFullName))
                    strMapName = value;
                else
                {
                    strErrorMsg = string.Format("[{0}]'s map.logical is not exists!\r\n", value);
                    bHasError = true;
                    strMapName = "";
                }
            }
            get
            {
                return strMapName;
            }
        }

        public string ErrorMsg
        {
            get
            {
                return strErrorMsg;
            }
        }
        //构造函数
        public NpcIndex(string rootdir)
        {           
            RootDir = rootdir;            
        }

        //读取strMapName的Logical.map文件，填充htIndex
        private bool ReadLogicalFile()
        {
            string strMapLogicalFileFullName = string.Format("{0}\\data\\source\\maps\\{1}\\{1}.Map.Logical", strRootDir, strMapName);
            if (!File.Exists(strMapLogicalFileFullName))
            {
                strErrorMsg = string.Format("file {0} is not exists!\r\n", strMapLogicalFileFullName);
                return false;
            }

            // 读文件
            string content = Helper.FileToString(strMapLogicalFileFullName, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(strMapLogicalFileFullName, content);
            if (Helper.m_inis == null)
            {
                strErrorMsg = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", strMapLogicalFileFullName);                
                return false;
            }
            
            int NpcCnt = Convert.ToInt32(Helper.GetIniString("MAIN", "NumNPC"));
            string strSection = string.Empty;
            int nTempleteID = 0;

            for (int i = 0; i < NpcCnt; ++i)
            {
                strSection = string.Format("NPC{0}", i);
                nTempleteID = Convert.ToInt32(Helper.GetIniString(strSection, "nTempleteID"));
                if (!htIndex.ContainsKey(nTempleteID))
                {
                    PointXY point = new PointXY();
                    point.nX = Convert.ToInt32(Helper.GetIniString(strSection, "nX"));
                    point.nY = Convert.ToInt32(Helper.GetIniString(strSection, "nY"));
                    htIndex[nTempleteID] = point;
                }                
            }
            Helper.CloseIniFile();

            return true;
        }

        public bool Init()
        {
            bool bRet = false;
            if (!bHasError)
            {
                bRet = ReadLogicalFile();
            }            
            return bRet;
        }

        //根据nTempleteID获得坐标
        //id不存在返回0，0
        public PointXY GetPoint(int templateid)
        {
            PointXY pointRet;
            if (!htIndex.ContainsKey(templateid))
            {
                strErrorMsg = string.Format("mapname = {0},nTempleteID = {1} is not exists.\r\n", strMapName, templateid);                
                pointRet = new PointXY();
                pointRet.nX = 0;
                pointRet.nY = 0;
            }
            else
            {
                pointRet = (PointXY)htIndex[templateid];
            }
            return pointRet;
        }
    }
}
