using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;

namespace QuestDirector2
{
    public class RegionInfo
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
            if (sbRegionCountXZ.ToString() == "0,0")
            {
                Helper.ReadIniFile("DWSplit", "RegionCountXZ", "0,0", sbRegionCountXZ, 255, RISettingFileName);
            }
            string[] arrXZ = sbRegionCountXZ.ToString().Split(',');
            RegionCountX = Convert.ToInt32(arrXZ[0]);
            RegionCountZ = Convert.ToInt32(arrXZ[1]);
            if (RegionCountX == 0 || RegionCountZ == 0)
            {
                return false;
            }

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


        public bool GetGreyDegree(int nLogicalX, int nLogicalY, ref int nGreyDegree)
        {
            bool bRet = false;
            if (nLogicalX <= RegionCountX * RegionWidth && nLogicalX >= 0 &&
                nLogicalY <= RegionCountZ * RegionHeight && nLogicalY >= 0
                )
            {
                float xScale = Convert.ToSingle(nLogicalX) / Convert.ToSingle(RegionCountX * RegionWidth);
                float yScale = Convert.ToSingle(nLogicalY) / Convert.ToSingle(RegionCountZ * RegionHeight);//logical文件里的y对应RISetting.in里的Z

                nGreyDegree = GetGreyDegree(xScale, yScale);
                bRet = true;
                
            }
            else
            {
                bHasError = true;
                strErrorMsg = string.Format("x:{0},y:{1} is out range in map {2}.\r\n", nLogicalX, nLogicalY, strMapName);
            }

            return bRet;
        }
        /// <summary>
        /// 获取指定灰度在middlemap图上打点的比例列表
        /// 返回的比例是相对于左上角的
        /// fStartX，fStartY 是相对于左下角的
        /// 比例用，隔开。多个比例之间用；隔开
        /// </summary>
        /// <param name="nGreyDegree"></param>
        /// <param name="fStartX">middlemap的cfgStartX</param>
        /// <param name="fStartY">middlemap的cfgStartY</param>
        /// <param name="fTotalWidth">middlemap的总高</param>
        /// <param name="fTotalHeight">middlemap的总宽</param>
        /// <param name="nStep">相对于灰度图隔多远打一个点</param>
        /// <returns></returns>
        public bool GetGreyPoints(int nGreyDegree, float fStartX, float fStartY, float fTotalWidth, float fTotalHeight, int nStep, float[,] dataRet, out int dataLen)
        {
            float fLogicalWidth = (float)(RegionCountX * RegionWidth);
            float fLogicalHeight = (float)(RegionCountZ * RegionHeight);
            int BmpWidth = bmpRLSplit.Width;
            int BmpHeight = bmpRLSplit.Height;

            StringBuilder sb = new StringBuilder();
            float fLogicalX;
            float fLogicalY;
            float fScalex;
            float fScaley;//相对于左上角

            dataLen = 0;
            int MaxDataLen = dataRet.GetLength(0);

            for (int x = 0; x < BmpWidth; x += nStep)
            {
                for (int y = 0; y < BmpHeight; y += nStep)
                {
                    if (GetGreyDegree(x, y) == nGreyDegree)
                    {
                        fLogicalX = (float)x / (float)BmpWidth * fLogicalWidth;
                        fLogicalY = (float)y / (float)BmpHeight * fLogicalHeight;
                        fScalex = (fLogicalX - fStartX) / fTotalWidth;
                        fScaley = (fTotalHeight - fLogicalHeight + fLogicalY + fStartY) / fTotalHeight;
                        if (dataLen == MaxDataLen)
                        {
                            return false;
                        }
                        dataRet[dataLen, 0] = fScalex;
                        dataRet[dataLen, 1] = fScaley;
                        dataLen++;                       
                        sb.Append(string.Format("{0},{1};", fScalex, fScaley));
                    }
                }
            }            
            return true;
        }
         
    }
}
