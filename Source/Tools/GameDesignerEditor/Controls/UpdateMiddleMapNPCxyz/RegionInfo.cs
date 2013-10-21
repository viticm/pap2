using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;

namespace UpdateMiddleMapNPCxyz
{
    class RegionInfo
    {
        private string strMapName;
        private string strRootDir;
        private string strAreaFileContent;

        private int RegionCountX = 0;//读RISettings.ini获得
        private int RegionCountZ = 0;

        const int RegionWidth = 32 * 32;
        const int RegionHeight = 32 * 32;

        private Bitmap bmpRLSplit;//灰度图

        public string strErrorMsg = "";
        public bool bHasError = false;

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

        private string BMPFileName
        {
            get
            {
                return string.Format("{0}\\data\\source\\maps\\{1}\\{1}RegionInfo\\RLSplit.bmp", strRootDir, strMapName);
            }
        }
        private string RISettingFileName
        {
            get
            {
                return string.Format("{0}\\data\\source\\maps\\{1}\\{1}RegionInfo\\RISettings.ini", strRootDir, strMapName);
            }
        }
        private string AreaTabFileName
        {
            get
            {
                return string.Format("{0}\\data\\source\\maps\\{1}minimap\\area.tab", strRootDir, strMapName);
            }
        }
       
        public bool Init(string rootdir, string mapname)
        {
            
            strErrorMsg = string.Empty;

            if (!Directory.Exists(rootdir))
            {
                strErrorMsg = string.Format("dictionary {0} is not exists.\r\n", rootdir);
                return false;
            }

            strRootDir = rootdir;
            strMapName = mapname;

            if (!File.Exists(BMPFileName))
            {
                strErrorMsg = string.Format("file {0} is not exists.\r\n", BMPFileName);
                return false;
            }
            if (!File.Exists(RISettingFileName))
            {
                strErrorMsg = string.Format("file {0} is not exists.\r\n", RISettingFileName);
                return false;
            }
            if (!File.Exists(AreaTabFileName))
            {
                strErrorMsg = string.Format("file {0} is not exists.\r\n", AreaTabFileName);
                return false;
            }

            bmpRLSplit = new Bitmap(BMPFileName);

            strAreaFileContent = Helper.FileToString(AreaTabFileName).Replace("\r", "");

            StringBuilder sbRegionCountXZ = new StringBuilder(255);
            Helper.ReadIniFile("RLSplit", "RegionCountXZ", "0,0", sbRegionCountXZ, 255, RISettingFileName);
            string[] arrXZ = sbRegionCountXZ.ToString().Split(',');
            RegionCountX = Convert.ToInt32(arrXZ[0]);
            RegionCountZ = Convert.ToInt32(arrXZ[1]);

            return true;
        }
        private int GetGreyDegree(int bmpX, int bmpY)
        {
            int nDegree = 0;
            if (bmpX < bmpRLSplit.Width && bmpY < bmpRLSplit.Height)
            {
                Color color;
                color = bmpRLSplit.GetPixel(bmpX, bmpY);
                nDegree = color.B;
            }            
            return nDegree;
        }
        private int GetGreyDegree(float xScale, float yScale)
        {
            int nDegree = 0;
            if (xScale >= 0.0f && xScale <= 1.0f &&
                yScale >= 0.0f &&yScale <=1.0f
                )
            {
                int x = (int)(bmpRLSplit.Width * xScale);
                int y = (int)(bmpRLSplit.Height * (1.0f - yScale));
                nDegree = GetGreyDegree(x, y);
            }
            return nDegree;
        }
        /// <summary>
        /// area.tab的字段顺序要为：id	name	middlemap	backgroundmusic	type	x	y	z
        /// middlemap要在第三个位置
        /// 中间要是\t末尾要是\n
        /// </summary>
        /// <param name="nGreyDegree"></param>
        /// <returns></returns>
        private int GetMiddleMapId(int nGreyDegree)
        {
            int nMiddleMapIdRet = 0;

            string strStart = string.Format("\n{0}", nGreyDegree);
            int nStartIndex = strAreaFileContent.IndexOf(strStart);
            if (nStartIndex < 0)
                return 0;

            int nEndIndex = strAreaFileContent.IndexOf('\n', nStartIndex + 1);
            string strOneLine = strAreaFileContent.Substring(nStartIndex, nEndIndex - nStartIndex);
            string[] arrOneLine = strOneLine.Split('\t');

            nMiddleMapIdRet = Convert.ToInt32(arrOneLine[2]);

            return nMiddleMapIdRet;
        }

        public int GetMiddleMapId(int nLogicalX, int nLogicalY)
        {
            int nMiddleMapIdRet = 0;

            if (nLogicalX <= RegionCountX * RegionWidth && nLogicalX >= 0 &&
                nLogicalY <= RegionCountZ * RegionHeight && nLogicalY >= 0
                )
            {
                float xScale = Convert.ToSingle(nLogicalX) / Convert.ToSingle(RegionCountX * RegionWidth);
                float yScale = Convert.ToSingle(nLogicalY) / Convert.ToSingle(RegionCountZ * RegionHeight);//logical文件里的y对应RISetting.in里的Z

                int nGreyDegree = GetGreyDegree(xScale, yScale);

                nMiddleMapIdRet = GetMiddleMapId(nGreyDegree);
            }
            else
            {
                bHasError = true;
                strErrorMsg = string.Format("x:{0},y:{1} is out range in map {2}.\r\n", nLogicalX, nLogicalY, strMapName);
            }            

            return nMiddleMapIdRet;
        }

    }
}
