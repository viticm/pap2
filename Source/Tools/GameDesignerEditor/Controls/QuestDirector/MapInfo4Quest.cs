using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.IO;
using System.Windows.Forms;


namespace QuestDirector
{
    class MapInfo4Quest
    {
        private string m_strLogicalFileName;//场景逻辑文件的全路径
        private Hashtable m_htNpcs;        
        private Hashtable m_htDoodads;
        private int m_nNpcNum;
        private int m_nDoodadNum;
        private bool m_bAvailable;
        private Hashtable m_htNpcDirectPoint;
        private Hashtable m_htDoodadDirectPoint;


        readonly int MaxDoodadNum = 199;
        readonly int MaxNpcNum = 199;
        readonly int MaxListLength = 199;

        //对与非成群和多个即 多于1个少于GroupMinNum个的怪物的坐标要特殊处理
        const int GroupMinNum = 4;//最少多个才算成群

        public bool IsAvailabel
        {
            get
            {
                return m_bAvailable;
            }
        }

        
        private bool ReadLogicalFile()
        {
            if (!File.Exists(m_strLogicalFileName))
            {
                m_bAvailable = false;
                //MessageBox.Show(m_strLogicalFileName + "不存在!", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            // 读文件
            string content = Helper.FileToString(m_strLogicalFileName, Encoding.Default);
            byte[] bytes = Encoding.Default.GetBytes(content);
            bytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, bytes);
            char[] chars = Encoding.Unicode.GetChars(bytes);
            content = new String(chars);
            Helper.OpenIniFile(m_strLogicalFileName, content);
            if (Helper.m_inis == null)
            {
                string strErr = string.Format("{0} 不是正确的ini格式，\r\n可能包含有 \"<<<<<<< .mine\" 等 SVN 合并残留字符串。\r\n请检查之。", m_strLogicalFileName);
                MessageBox.Show(strErr);
                return false;
            }
            m_nDoodadNum = Convert.ToInt32(Helper.GetIniString("MAIN", "NumDoodad"));
            m_nNpcNum = Convert.ToInt32(Helper.GetIniString("MAIN", "NumNPC"));

            m_htDoodads = new Hashtable(m_nDoodadNum);
            m_htNpcs = new Hashtable(m_htNpcs);

            ReadTemplates(m_htDoodads, m_nDoodadNum, "Doodad");
            ReadTemplates(m_htNpcs, m_nNpcNum, "NPC");

            Helper.CloseIniFile();

            m_bAvailable = true;
            return true;
        }

        private bool ReadTemplates(Hashtable htDestination, int nNum, string strTemplateType)
        {
            string strSection = string.Empty;
            Point3d point;
            int nTemplateID = 0;
            ArrayList arrlistPoints;
            for (int i = 0; i < nNum; i++)
            {
                strSection = string.Format("{0}{1}", strTemplateType, i);
                point.nX = Convert.ToInt32(Helper.GetIniString(strSection, "nX"));
                point.nY = Convert.ToInt32(Helper.GetIniString(strSection, "nY"));
                point.nZ = 0;
                nTemplateID = Convert.ToInt32(Helper.GetIniString(strSection, "nTempleteID"));
                if (htDestination.ContainsKey(nTemplateID))
                {
                    arrlistPoints = htDestination[nTemplateID] as ArrayList;
                    arrlistPoints.Add(point);
                }
                else
                {
                    arrlistPoints = new ArrayList(MaxListLength);
                    arrlistPoints.Add(point);
                    htDestination[nTemplateID] = arrlistPoints;
                }

            }
            return true;
        }

        public MapInfo4Quest(string strLogicalFileName)
        {
            m_bAvailable = false;
            m_strLogicalFileName = strLogicalFileName;
            m_htDoodads = new Hashtable(MaxDoodadNum);
            m_htNpcs = new Hashtable(MaxNpcNum);
            m_htNpcDirectPoint = new Hashtable(MaxNpcNum);
            m_htDoodadDirectPoint = new Hashtable(MaxDoodadNum);
            
        }
        ~MapInfo4Quest()
        {
            m_htDoodads.Clear();
            m_htNpcs.Clear();
            m_htNpcDirectPoint.Clear();
            m_htDoodadDirectPoint.Clear();
        }
        public string LogicalFileName
        {
            set
            {
                if (File.Exists(value))
                {
                    m_strLogicalFileName = value;
                }
                else
                {
                    MessageBox.Show("文件不存在", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    m_strLogicalFileName = string.Empty;
                }
            }
        }

        public bool Init()
        {
            bool bRetCode = false;
            bRetCode = ReadLogicalFile();
            return bRetCode;
        }

        /// <summary>
        /// 根据模块类型和模块id获取所有模块
        /// </summary>
        /// <param name="nTemplateId"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        private ArrayList GetPointList(int nTemplateId, TemplateType type)
        {
            ArrayList arrlistRet;
            switch (type)
            {
                case TemplateType.DOODAD_TEMPLATE:
                    arrlistRet = m_htDoodads[nTemplateId] as ArrayList;
                    break;
                case TemplateType.NPC_TEMPLATE:
                    arrlistRet = m_htNpcs[nTemplateId] as ArrayList;
                    break;
                default:
                    arrlistRet = null;
                    break;
            }
            return arrlistRet;
        }
        public Point3d GetFinalPoint(int nTemplateId, TemplateType type, bool needRefence, Point3d pointRefence)
        {
            Point3d pointRet;
            switch (type)
            {
                case TemplateType.NPC_TEMPLATE:
                    pointRet = CheckFinalPoint(m_htNpcDirectPoint, m_htNpcs, nTemplateId, needRefence, pointRefence);
                    break;
                case TemplateType.DOODAD_TEMPLATE:
                    pointRet = CheckFinalPoint(m_htDoodadDirectPoint, m_htDoodads, nTemplateId, needRefence, pointRefence);
                    break;
                default:
                    pointRet.nX = -1;
                    pointRet.nY = -1;
                    pointRet.nZ = -1;
                    break;
            }

            return pointRet;
        }
        /// <summary>
        /// 从表htQurey查询nTemplateId对应的最终坐标，
        /// 如果htQurey里没有，则从表里取出对应的坐标列表生成最终坐标，并把最终坐标保存到htQurey表里
        /// </summary>
        /// <param name="htQurey"></param>
        /// <param name="htInfo"></param>
        /// <param name="nTemplateId"></param>
        /// <returns></returns>
        private Point3d CheckFinalPoint(Hashtable htQurey, Hashtable htInfo, int nTemplateId, bool needRefence, Point3d pointRefence)
        {
            Point3d pointRet;
            if (htInfo.ContainsKey(nTemplateId))
            {
                ArrayList arrPointlist = htInfo[nTemplateId] as ArrayList;
                int pointNum = arrPointlist.Count;
                if ((pointNum == 1 && pointNum >= GroupMinNum) &&
                    htQurey.ContainsKey(nTemplateId)
                    )
                {
                    pointRet = (Point3d)htQurey[nTemplateId];
                }
                else
                {
                    pointRet = CalculateFinalPoint(arrPointlist, needRefence, pointRefence);
                    htQurey[nTemplateId] = pointRet;
                }
            }            
            else  //在逻辑文件里不存在该类型（npc，doodad）的该nTempleteId
            {
                pointRet.nX = -1;
                pointRet.nY = -1;
                pointRet.nZ = -1;
            }
            return pointRet;
        }
        private Point3d CalculateFinalPoint(ArrayList arrPoint, bool needRefence, Point3d pointRefence)
        {
            Point3d pointRet;
            if (arrPoint == null || arrPoint.Count == 0)
            {
                pointRet.nX = -1;
                pointRet.nY = -1;
                pointRet.nZ = -1;
            }
            else if (arrPoint.Count == 1)
            {
                pointRet = (Point3d)arrPoint[0];
            }
            else if (arrPoint.Count <= 3 && needRefence)
            {

                pointRet = GetNearPoint(arrPoint, pointRefence);
            }
            else
            {
                pointRet = GetFocusPoint(arrPoint);
            }
            return pointRet;
        }
        private Point3d GetNearPoint(ArrayList arrPoint, Point3d refencePoint)
        {
            float fDistance = 0.0f;
            float fMinDistance = 0.0f;
            Point3d point = (Point3d)(arrPoint[0]);

            fDistance = (refencePoint.nX - point.nX) * (refencePoint.nX - point.nX) + (refencePoint.nY - point.nY) * (refencePoint.nY - point.nY);
            fMinDistance = fDistance;

            int MinDistancePointIndex = 0;

            for (int i = 1; i < arrPoint.Count; ++i)
            {                
                point = (Point3d)(arrPoint[i]);
                fDistance = (refencePoint.nX - point.nX) * (refencePoint.nX - point.nX) + (refencePoint.nY - point.nY) * (refencePoint.nY - point.nY);
                if (fMinDistance > fDistance)
                {
                    fMinDistance = fDistance;
                    MinDistancePointIndex = i;
                }
            }
            point = (Point3d)(arrPoint[MinDistancePointIndex]);
            
            return point;
        }
        public bool IsTemplateIdExists(int nTemplateId, TemplateType type)
        {
            switch (type)
            {
                case TemplateType.DOODAD_TEMPLATE:
                    return m_htDoodads.ContainsKey(nTemplateId);
                   
                case TemplateType.NPC_TEMPLATE:
                    return m_htNpcs.ContainsKey(nTemplateId);
                   
                default:
                    return false;
            }
        }
        private Point3d GetCentralPoint(ArrayList arrPoint)
        {
            Point3d pointRet = (Point3d)arrPoint[0];
            
            int nMaxx = ((Point3d)arrPoint[0]).nX;
            int nMaxy = ((Point3d)arrPoint[0]).nY;
            int nMinx = nMaxx;
            int nMiny = nMaxy;

            foreach (Point3d point in arrPoint)
            {
                if (point.nX > nMaxx)
                {
                    nMaxx = point.nX;
                }
                else if (point.nX < nMinx)
                {
                    nMinx = point.nX;
                }
                ////
                if (point.nY > nMaxy)
                {
                    nMaxy = point.nY;
                }
                else if (point.nY < nMiny)
                {
                    nMiny = point.nY;
                }
            }

            int nMidx = nMinx + (nMaxx - nMinx) / 2;
            int nMidy = nMiny + (nMaxy - nMiny) / 2;

            float fDistance = (nMidx - nMinx) * (nMidx - nMinx) + (nMidy - nMiny) * (nMidy - nMiny);
            float fMinDistance = fDistance;
            

            foreach (Point3d point2 in arrPoint)
            {
                fDistance = (nMidx - point2.nX) * (nMidx - point2.nX) + (nMidy - point2.nY) * (nMidy - point2.nY);
                if (fDistance < fMinDistance)
                {
                    pointRet = point2;
                }
            }
            return pointRet;

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
                while ( moveRight <= nMaxx)
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

           // ArrayList areaArrPointList = GetAreaPointArrList(goalRight - MoveWidth, goalTop, goalRight, goalTop - MoveHeight, arrPoint);

            //Point3d pointRet = GetCentralPoint(areaArrPointList);
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

        private ArrayList GetAreaPointArrList(int left, int top, int right, int bottom, ArrayList arrPoint)
        {
            ArrayList arrListRet = new ArrayList();

            foreach (Point3d point in arrPoint)
            {
                if (point.nX >= left && point.nX <= right && point.nY >= bottom && point.nY <= top)
                {
                    arrListRet.Add(point);
                }
            }
            return arrListRet;
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

    }
}
