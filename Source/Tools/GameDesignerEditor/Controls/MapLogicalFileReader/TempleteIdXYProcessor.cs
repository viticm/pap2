using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Data;

namespace MapLogicalFileReader
{

   public class TempleteIdXYProcessor
    {
       private string MapLogicalFilePath;
       private string TableName = "NPC";
        
       const string nTempleteIDColumnName = "nTempleteID";
       const string nXColumnName = "nX";
       const string nYColumnName = "nY";

       private float TotalWidth;
       private float TotalHeight;
       private float Startx;
       private float Starty;
       private string ConfigFilePath;

       private bool HasError;
       private string errMsg = string.Empty;

        private MapLogicalTableReader npcTable;

       public TempleteIdXYProcessor(string logicalFilePath, string configFilePath, string tableName)
        {
            MapLogicalFilePath = logicalFilePath;
            ConfigFilePath = configFilePath;
            TableName = tableName;
        }
        public bool Init()
        {
            errMsg = string.Empty;
            HasError = false;

            string[] columnNames = { nXColumnName, nYColumnName, nTempleteIDColumnName };
            npcTable = new MapLogicalTableReader(MapLogicalFilePath, TableName, columnNames);
            if (!npcTable.Init())
            {
                errMsg += string.Format("{0} has error!", MapLogicalFilePath);
                HasError = true;
                return false;
            }
            if (!LoadConfig())
            {
                errMsg += string.Format("{0} has errors!", ConfigFilePath);
                HasError = true;
                return false;
            }
            return true;
        }
       private bool LoadConfig()
        {
            if (!Helper.IsFileExists(ConfigFilePath))
            {
                return false;
            }
            float cfgWidth;
            float cfgHeight;
            float cfgScale;


            string content = Helper.FileToString(ConfigFilePath, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(ConfigFilePath, content);
            if (Helper.m_inis == null)
            {
                string strErr = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", ConfigFilePath);

                return false;
            }

            bool bRetCode = true;
            string strWidth = Helper.GetIniString("middlemap0", "width");
            string strHeight = Helper.GetIniString("middlemap0", "height");
            string strScale = Helper.GetIniString("middlemap0", "scale");
            string strStartx = Helper.GetIniString("middlemap0", "startx");
            string strStarty = Helper.GetIniString("middlemap0", "starty");
            
            if (strScale == "0")
            {
                return false;
            }

            bRetCode = float.TryParse(strWidth, out cfgWidth);
            if (!bRetCode)
            {
                return false;
            }
            bRetCode = float.TryParse(strHeight, out cfgHeight);
            if (!bRetCode)
            {
                return false;
            }
            bRetCode = float.TryParse(strScale, out cfgScale);
            if (!bRetCode)
            {
                return false;
            }
           bRetCode = float.TryParse(strStartx, out Startx);
            if (!bRetCode)
            {
                return false;
            } 
           bRetCode = float.TryParse(strStarty, out Starty);
            if (!bRetCode)
            {
                return false;
            }

            TotalHeight = cfgHeight / cfgScale;
            TotalWidth = cfgWidth / cfgScale;
            
            Helper.CloseIniFile(); 
            return bRetCode;
        }
        /// <summary>
        /// 获取密集点的逻辑坐标
        /// </summary>
        /// <param name="nTemplateId"></param>
        /// <param name="nX"></param>
        /// <param name="nY"></param>
        /// <returns></returns>
        public bool GetFocusLogicalXY(int nTemplateId, ref int nX, ref int nY)
        {
            ArrayList arrPoint = GetArrList(nTemplateId);
            if (arrPoint.Count == 0)
            {
                return false;
            }

            Point3d point = new Point3d();
            if (arrPoint.Count == 1)
            {
                point = (Point3d)arrPoint[0];
            }
            else
            {
                point = GetFocusPoint(arrPoint);
            }

            nX = point.nX;
            nY = point.nY;
            return true;
        }
        /// <summary>
        /// 求得比例坐标,左上角为0.0，0.0
        /// 逻辑坐标左下角为startx，starty
        /// </summary>
        /// <param name="nTemplateId"></param>
        /// <param name="npc_x"></param>
        /// <param name="npc_y"></param>
        /// <returns></returns>
        public bool GetScaleXY(int nTemplateId, ref float npc_x, ref float npc_y)
        {
            int nX = 0;
            int nY = 0;
            if (!GetFocusLogicalXY(nTemplateId, ref nX, ref nY))
            {
                return false;
            }
            if (TotalHeight <=0 || TotalWidth <= 0)
            {
                return false;
            }
            npc_x = (nX - Startx) / TotalWidth;
            npc_y = (TotalHeight - (nY - Starty)) / TotalHeight;

            return true;
        }

       public ArrayList GetScaleArrList(int nTemplateId)
       {
           ArrayList arrRet = new ArrayList();
           ArrayList arrPoint3d = GetArrList(nTemplateId);

           foreach (object o in arrPoint3d)
           {
               Point3d point = (Point3d)o;
               fPoint2d fpoint = new fPoint2d();
               fpoint.fX = (point.nX - Startx) / TotalWidth;
               fpoint.fY = (TotalHeight - (point.nY - Starty)) / TotalHeight;
               arrRet.Add(fpoint);
           }

           return arrRet;
       }
        private ArrayList GetArrList(int nTemplateId)
        {
            ArrayList arrRet = new ArrayList();

            DataRow[] rows = npcTable.Select(nTempleteIDColumnName, nTemplateId.ToString());

            foreach (DataRow row in rows)
            {
                Point3d point = new Point3d();
                
                point.nX = GetCellIntValue(row, nXColumnName);
                point.nY = GetCellIntValue(row, nYColumnName);
                point.nZ = 0;

                arrRet.Add(point);
            }
            return arrRet;
        }
        private int GetCellIntValue(DataRow row, string columnName)
        {
            if (row.IsNull(columnName))
            {
                return 0;
            }
            return Convert.ToInt32(row[columnName]);
        }
        /// <summary>
        /// 获取密集的点
        /// </summary>
        /// <param name="arrPoint"></param>
        /// <returns></returns>
        private Point3d GetFocusPoint(ArrayList arrPoint)
        {
            const int StepAdd = 300;
            const int MoveWidth = 6000;
            const int MoveHeight = 6000;

            int nMaxx = 0;
            int nMaxy = 0;
            int nMinx = 0;
            int nMiny = 0;
            GetMaxMinxy(arrPoint, ref nMaxx, ref nMaxy, ref nMinx, ref nMiny);

            int moveLeft = nMinx;
            int moveBottom = nMiny;
            int moveRight = nMinx + MoveWidth;
            int moveTop = nMiny + MoveHeight;

            int goalRight = 0;
            int goalTop = 0;

            int MaxCnt = 0;
            int tempCnt = 0;

            if (moveTop > nMaxy)
            {
                moveTop = nMaxy;
            }

            while (moveTop <= nMaxy)
            {
                moveRight = nMinx + MoveWidth;
                if (moveRight > nMaxx)
                {
                    moveRight = nMaxx;
                }
                while (moveRight <= nMaxx)
                {
                    tempCnt = CountAreaPoint(moveRight - MoveWidth, moveTop, moveRight, moveTop - MoveHeight, arrPoint);
                    if (tempCnt > MaxCnt)
                    {
                        MaxCnt = tempCnt;
                        goalRight = moveRight;
                        goalTop = moveTop;
                    }

                    moveRight += StepAdd;
                    if (moveRight > nMaxx && moveRight - StepAdd < nMaxx)
                    {
                        moveRight = nMaxx;
                    }

                }
                moveTop += StepAdd;
                if (moveTop > nMaxy && moveTop - StepAdd < nMaxy)
                {
                    moveTop = nMaxy;
                }

            }

            Point3d pointRet = (Point3d)arrPoint[0];
            if (goalRight - MoveWidth < nMinx)
            {
                pointRet.nX = (nMinx + nMaxx) / 2;
            }
            else
            {
                pointRet.nX = goalRight - MoveWidth / 2;
            }
            if (goalTop - MoveHeight < nMiny)
            {
                pointRet.nY = (nMiny + nMaxy) / 2;
            }
            else
            {
                pointRet.nY = goalTop - MoveHeight / 2;
            }

            return pointRet;

        }
        private int CountAreaPoint(int left, int top, int right, int bottom, ArrayList arrPoint)
        {
            int nRetCnt = 0;
            foreach (Point3d point in arrPoint)
            {
                if (point.nX >= left && point.nX <= right && point.nY >= bottom && point.nY <= top)
                {
                    ++nRetCnt;
                }
            }
            return nRetCnt;

        }


        private void GetMaxMinxy(ArrayList arrPoint, ref int maxx, ref int maxy, ref int minx, ref int miny)
        {
            maxx = ((Point3d)arrPoint[0]).nX;
            maxy = ((Point3d)arrPoint[0]).nY;
            minx = maxx;
            miny = maxy;

            foreach (Point3d point in arrPoint)
            {
                if (point.nX > maxx)
                {
                    maxx = point.nX;
                }
                else if (point.nX < minx)
                {
                    minx = point.nX;
                }
                ////
                if (point.nY > maxy)
                {
                    maxy = point.nY;
                }
                else if (point.nY < miny)
                {
                    miny = point.nY;
                }
            }
        }

       public bool Available
       {
           get { return !HasError; }
       }
       public string ErrorMessage
       {
           get { return errMsg; }
       }

    }
    struct Point3d
    {
        public int nX;
        public int nY;
        public int nZ;
    };

    public struct fPoint2d
    {
        public float fX;
        public float fY;
    };

}
