using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

using GraphElementLibrary;

using FlowChartEditor.BaseGraphElement;

namespace FlowChartEditor
{
    class RegionManager
    {
        private List<RegionData> regionDataList = new List<RegionData>(); // 区域管理器链表
        private RegionData backupRegionData; // 边缘区域数据
        private bool[,] canvasGridBoard; // 绘图板网格点面板 
        private Size canvasSize; // 绘图板大小
        private int gridWidth; // 网格宽度
        private int gridBoardWidth; // 网格板宽度
        private int gridBoardHeight; // 网格板高度

        // 区域数据
        class RegionData
        {
            private Rectangle regionRectangle = Rectangle.Empty;
            private List<ConnectorContainer> connectorContainers = new List<ConnectorContainer>();
            private List<SlotContainer> slotContainers = new List<SlotContainer>();

            /// <summary>
            /// 构造函数
            /// </summary>
            /// <param name="regionRectangle">区域所在的矩形</param>
            public RegionData(Rectangle regionRectangle)
            {
                this.regionRectangle = regionRectangle;
            }

            /// <summary>
            /// 区域中所有的连接线
            /// </summary>
            public List<ConnectorContainer> ConnectorContainers
            {
                get
                {
                    return connectorContainers;
                }
            }

            /// <summary>
            /// 区域中所有的插槽容器
            /// </summary>
            public List<SlotContainer> SlotContainers
            {
                get
                {
                    return slotContainers;
                }
            }

            /// <summary>
            /// 删除区域中的插槽容器
            /// </summary>
            /// <param name="slotContainer">当前插槽容器</param>
            public void Remove(SlotContainer slotContainer)
            {
                slotContainers.Remove(slotContainer);
            }

            /// <summary>
            /// 删除区域中的连接线
            /// </summary>
            /// <param name="line">当前连接线</param>
            public void Remove(ConnectorContainer line)
            {
                connectorContainers.Remove(line);
            }

            /// <summary>
            /// 检查区域中是否包含当前插槽容器
            /// </summary>
            /// <param name="slotContainer">当前插槽容器</param>
            /// <returns>区域是否包含当前插槽容器</returns>
            public bool Contains(SlotContainer slotContainer)
            {
                bool result = slotContainers.Contains(slotContainer);

                return result;
            }

            /// <summary>
            /// 检查区域中是否包含当前连接线
            /// </summary>
            /// <param name="line">当前连接线</param>
            /// <returns>区域中是否包含当前连接线</returns>
            public bool Contains(ConnectorContainer line)
            {
                bool result = connectorContainers.Contains(line);

                return result;
            }

            /// <summary>
            /// 检查点是否在区域中
            /// </summary>
            /// <param name="p">当前点</param>
            /// <returns>点是否在区域中</returns>
            public bool IsInRegion(Point p)
            {
                bool result = regionRectangle.Contains(p);

                return result;
            }

            /// <summary>
            /// 检查矩形是否在和区域有交集
            /// </summary>
            /// <param name="rectangle">当前矩形</param>
            /// <returns>矩形是否和区域有交集</returns>
            public bool IsInRegion(Rectangle rectangle)
            {
                bool result = regionRectangle.Contains(rectangle) || regionRectangle.IntersectsWith(rectangle);

                return result;
            }

            /// <summary>
            /// 添加插槽容器到区域
            /// </summary>
            /// <param name="slotContainer">插槽容器</param>
            public void Add(SlotContainer slotContainer)
            {
                if (!slotContainers.Contains(slotContainer))
                {
                    slotContainers.Add(slotContainer);
                }
            }

            /// <summary>
            /// 添加连接线到区域
            /// </summary>
            /// <param name="line">添加连接线到区域</param>
            public void Add(ConnectorContainer line)
            {
                if (!connectorContainers.Contains(line))
                {
                    connectorContainers.Add(line);
                }
            }
        }

        /// <summary>
        /// 绘图板网格
        /// </summary>
        public bool[,] CanvasGridBoard
        {
            get
            {
                return canvasGridBoard;
            }
        }

        /// <summary>
        /// 初始化网格标记板
        /// </summary>
        public void InitCanvasGridBoard(List<SlotContainer> slotContainerList, Size canvasSize, int gridWidth)
        {
            this.canvasSize = canvasSize;
            this.gridWidth = gridWidth;

            gridBoardWidth = (int)Math.Ceiling((float)canvasSize.Width / gridWidth);
            gridBoardHeight = (int)Math.Ceiling((float)canvasSize.Height / gridWidth);
            canvasGridBoard = new bool[gridBoardWidth, gridBoardHeight];

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                Rectangle rectangle = slotContainer.InvalidRectangle;
                List<Point> gridPointList = GetGridPointList(slotContainer, gridWidth);

                foreach (Point point in gridPointList)
                {
                    int gridX = (int)(point.X / gridWidth);
                    int gridY = (int)(point.Y / gridWidth);

                    if (gridX >= 0 && gridX < gridBoardWidth && point.Y >= 0 && point.Y < gridBoardHeight)
                    {
                        canvasGridBoard[gridX, gridY] = true;
                    }
                }
            }
        }

        /// <summary>
        /// 更新插槽容器对应的网格
        /// </summary>
        /// <param name="slotContainer">插槽容器</param>
        private void UpdateSlotContainerGrid(SlotContainer slotContainer)
        {
            Rectangle rectangle = slotContainer.InvalidRectangle;
            List<Point> gridPointList = GetGridPointList(slotContainer, gridWidth);

            foreach (Point point in gridPointList)
            {
                int gridX = (int)(point.X / gridWidth);
                int gridY = (int)(point.Y / gridWidth);

                if (gridX >= 0 && gridX < gridBoardWidth && point.Y >= 0 && point.Y < gridBoardHeight)
                {
                    canvasGridBoard[gridX, gridY] = true;
                }
            }
        }

        /// <summary>
        /// 清除插槽容器对应的网格
        /// </summary>
        /// <param name="slotContainer">插槽容器</param>
        private void ClearSlotContainerGrid(SlotContainer slotContainer)
        {
            Rectangle rectangle = slotContainer.InvalidRectangle;
            List<Point> gridPointList = GetGridPointList(slotContainer, gridWidth);

            foreach (Point point in gridPointList)
            {
                int gridX = (int)(point.X / gridWidth);
                int gridY = (int)(point.Y / gridWidth);

                if (gridX >= 0 && gridX < gridBoardWidth && point.Y >= 0 && point.Y < gridBoardHeight)
                {
                    canvasGridBoard[gridX, gridY] = false;
                }
            }
        }

        /// <summary>
        /// 获取插槽容器覆盖网格点的集合
        /// </summary>
        /// <param name="slotContainer">插槽容器</param>
        /// <param name="gridWidth">网格宽度</param>
        /// <returns>插槽容器覆盖网格点的集合</returns>
        private List<Point> GetGridPointList(SlotContainer slotContainer, int gridWidth)
        {
            List<Point> list = new List<Point>();
            Rectangle rectangle = slotContainer.TextRectangle;
            int leftX = (int)Math.Ceiling((float)rectangle.Left / gridWidth) * gridWidth;
            int rightX = (int)Math.Floor((float)rectangle.Right / gridWidth) * gridWidth;
            int topY = (int)Math.Ceiling((float)rectangle.Top / gridWidth) * gridWidth;
            int bottomY = (int)Math.Floor((float)rectangle.Bottom / gridWidth) * gridWidth;
            Point point = Point.Empty;

            for (int i = leftX; i <= rightX; i += gridWidth)
            {
                for (int j = topY; j <= bottomY; j += gridWidth)
                {
                    point.X = i;
                    point.Y = j;
                    list.Add(point);
                }
            }

            return list;
        }

        /// <summary>
        /// 初始化区域管理器
        /// </summary>
        /// <param name="slotContainerList">插槽容器链表</param>
        /// <param name="connectorContainerList">连接线链表</param>
        /// <param name="canvasSize">绘图板的大小</param>
        public void Init(List<SlotContainer> slotContainerList, List<ConnectorContainer> connectorContainerList, Size canvasSize, int gridWidth)
        {
            regionDataList.Clear();

            for (int i = 0; i < canvasSize.Width; i += 600)
            {
                for (int j = 0; j < canvasSize.Height; j += 600)
                {
                    Rectangle regionRectangle = Rectangle.Empty;
                    regionRectangle.Location = new Point(i, j);
                    int regionWidth = 600;
                    int regionHeight = 600;

                    if (i + 600 > canvasSize.Width)
                    {
                        regionWidth = canvasSize.Width - i;
                    }

                    if (j + 600 > canvasSize.Height)
                    {
                        regionHeight = canvasSize.Height - j;
                    }

                    regionRectangle.Size = new Size(regionWidth, regionHeight);

                    RegionData regionData = new RegionData(regionRectangle);
                    regionDataList.Add(regionData);
                }
            }

            foreach (SlotContainer slotContainer in slotContainerList)
            {
                foreach (RegionData regionData in regionDataList)
                {
                    if (regionData.IsInRegion(slotContainer.InvalidRectangle))
                    {
                        regionData.Add(slotContainer);
                    }
                }
            }

            foreach (ConnectorContainer line in connectorContainerList)
            {
                foreach (RegionData regionData in regionDataList)
                {
                    if (regionData.IsInRegion(line.InvalidRectangle))
                    {
                        regionData.Add(line);
                    }
                }
            }

            InitCanvasGridBoard(slotContainerList, canvasSize, gridWidth);

            backupRegionData = new RegionData(Rectangle.Empty);
        }

        /// <summary>
        /// 将图元加入到区域中
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        public void AddToRegion(GraphElement graphElement)
        {
            if (graphElement is ConnectorContainer)
            {
                AddToRegion(graphElement as ConnectorContainer);
            }
            else if (graphElement is SlotContainer)
            {
                AddToRegion(graphElement as SlotContainer);
            }
        }

        /// <summary>
        /// 将插槽容器加入区域中
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        private void AddToRegion(SlotContainer slotContainer)
        {
            bool added = false;

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(slotContainer.InvalidRectangle))
                {
                    regionData.Add(slotContainer);
                    added = true;
                }
            }

            if (!added)
            {
                backupRegionData.Add(slotContainer);
            }

            // 更新网格数据
            UpdateSlotContainerGrid(slotContainer);
        }

        /// <summary>
        /// 将连接线加入区域中
        /// </summary>
        /// <param name="line">当前连接线</param>
        private void AddToRegion(ConnectorContainer line)
        {
            bool added = false;

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(line.InvalidRectangle))
                {
                    regionData.Add(line);
                    added = true;
                }
            }

            if (!added)
            {
                backupRegionData.Add(line);
            }
        }

        /// <summary>
        /// 将图元从区域中删除
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        public void DeleteFromRegion(GraphElement graphElement)
        {
            if (graphElement is ConnectorContainer)
            {
                DeleteFromRegion(graphElement as ConnectorContainer);
            }
            else if (graphElement is SlotContainer)
            {
                DeleteFromRegion(graphElement as SlotContainer);
            }
        }

        /// <summary>
        /// 将插槽容器从区域中删除
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        private void DeleteFromRegion(SlotContainer slotContainer)
        {
            foreach (RegionData regionData in regionDataList)
            {
                regionData.Remove(slotContainer);
            }

            backupRegionData.Remove(slotContainer);

            // 更新网格数据
            ClearSlotContainerGrid(slotContainer);
        }

        /// <summary>
        /// 将连接线从区域中删除
        /// </summary>
        /// <param name="line">当前连接线</param>
        private void DeleteFromRegion(ConnectorContainer line)
        {
            foreach (RegionData regionData in regionDataList)
            {
                regionData.Remove(line);
            }

            backupRegionData.Remove(line);
        }

        /// <summary>
        /// 更新图元所在的区域
        /// </summary>
        /// <param name="graphElement">当前图元</param>
        public void ChangeRegion(GraphElement graphElement)
        {
            if (graphElement is ConnectorContainer)
            {
                ChangeRegion(graphElement as ConnectorContainer);
            }
            else if (graphElement is SlotContainer)
            {
                ChangeRegion(graphElement as SlotContainer);
            }
        }

        /// <summary>
        /// 更新插槽容器所在的区域
        /// </summary>
        /// <param name="slotContainer">当前插槽容器</param>
        private void ChangeRegion(SlotContainer slotContainer)
        {
            bool added = false; // 是否已经添加到区域中

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.Contains(slotContainer) && !regionData.IsInRegion(slotContainer.InvalidRectangle))
                {
                    regionData.Remove(slotContainer);
                }

                if (!regionData.Contains(slotContainer) && regionData.IsInRegion(slotContainer.InvalidRectangle))
                {
                    regionData.Add(slotContainer);
                    added = true;
                }
            }

            if (!added)
            {
                backupRegionData.Add(slotContainer);
            }

            // 更新相关连接线所在的区域            
            foreach (ConnectorContainer line in slotContainer.GetConnectedLine())
            {
                ChangeRegion(line);
            }
        }

        /// <summary>
        /// 更新连接线所在的区域
        /// </summary>
        /// <param name="line">当前连接线</param>
        private void ChangeRegion(ConnectorContainer line)
        {
            bool added = false; // 是否已经添加到区域中

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.Contains(line) && !regionData.IsInRegion(line.InvalidRectangle))
                {
                    regionData.Remove(line);
                }

                if (!regionData.Contains(line) && regionData.IsInRegion(line.InvalidRectangle))
                {
                    regionData.Add(line);
                    added = true;
                }
            }

            if (!added)
            {
                backupRegionData.Add(line);
            }
        }

        /// <summary>
        /// 获取插槽容器链表
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>插槽容器链表</returns>
        public List<SlotContainer> GetSlotContainerList(Point p)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(p))
                {
                    foreach (SlotContainer slotContainer in regionData.SlotContainers)
                    {
                        if (slotContainer.IsInRegion(p) && !list.Contains(slotContainer))
                        {
                            list.Add(slotContainer);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.SlotContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取插槽容器链表
        /// </summary>
        /// <param name="rectangle">当前矩形</param>
        /// <returns>插槽容器链表</returns>
        public List<SlotContainer> GetSlotContainerList(Rectangle rectangle)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(rectangle))
                {
                    foreach (SlotContainer slotContainer in regionData.SlotContainers)
                    {
                        Rectangle invalidRectangle = slotContainer.InvalidRectangle;

                        if ((rectangle.IntersectsWith(invalidRectangle) || rectangle.Contains(invalidRectangle) || invalidRectangle.Contains(rectangle))
                            && !list.Contains(slotContainer))
                        {
                            list.Add(slotContainer);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.SlotContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取插槽容器链表
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>插槽容器链表</returns>
        public List<SlotContainer> GetSlotContainers(Point p)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(p))
                {
                    foreach (SlotContainer slotContainer in regionData.SlotContainers)
                    {
                        if (!list.Contains(slotContainer))
                        {
                            list.Add(slotContainer);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.SlotContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取插槽容器链表
        /// </summary>
        /// <param name="rectangle">当前矩形</param>
        /// <returns>插槽容器链表</returns>
        public List<SlotContainer> GetSlotContainers(Rectangle rectangle)
        {
            List<SlotContainer> list = new List<SlotContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(rectangle))
                {
                    foreach (SlotContainer slotContainer in regionData.SlotContainers)
                    {
                        if (!list.Contains(slotContainer))
                        {
                            list.Add(slotContainer);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.SlotContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取连接线链表
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>连接线链表</returns>
        public List<ConnectorContainer> GetConnectorContainerList(Point p)
        {
            List<ConnectorContainer> list = new List<ConnectorContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(p))
                {
                    foreach (ConnectorContainer line in regionData.ConnectorContainers)
                    {
                        if (line.IsInRegion(p) && !list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.ConnectorContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取连接线链表
        /// </summary>
        /// <param name="rectangle">当前矩形</param>
        /// <returns>连接线链表</returns>
        public List<ConnectorContainer> GetConnectorContainerList(Rectangle rectangle)
        {
            List<ConnectorContainer> list = new List<ConnectorContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(rectangle))
                {
                    foreach (ConnectorContainer line in regionData.ConnectorContainers)
                    {
                        Rectangle invalidRectangle = line.InvalidRectangle;

                        if ((rectangle.IntersectsWith(invalidRectangle) || rectangle.Contains(invalidRectangle) || invalidRectangle.Contains(rectangle))
                            && !list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.ConnectorContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取连接线链表
        /// </summary>
        /// <param name="p">当前点</param>
        /// <returns>连接线链表</returns>
        public List<ConnectorContainer> GetAllConnectorContainers(Point p)
        {
            List<ConnectorContainer> list = new List<ConnectorContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(p))
                {
                    foreach (ConnectorContainer line in regionData.ConnectorContainers)
                    {
                        if (!list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.ConnectorContainers);
            }

            return list;
        }

        /// <summary>
        /// 获取连接线链表
        /// </summary>
        /// <param name="rectangle">当前矩形</param>
        /// <returns>连接线链表</returns>
        public List<ConnectorContainer> GetConnectorContainers(Rectangle rectangle)
        {
            List<ConnectorContainer> list = new List<ConnectorContainer>();

            foreach (RegionData regionData in regionDataList)
            {
                if (regionData.IsInRegion(rectangle))
                {
                    foreach (ConnectorContainer line in regionData.ConnectorContainers)
                    {
                        if (!list.Contains(line))
                        {
                            list.Add(line);
                        }
                    }
                }
            }

            if (list.Count == 0)
            {
                list.AddRange(backupRegionData.ConnectorContainers);
            }

            return list;
        }
    }
}
