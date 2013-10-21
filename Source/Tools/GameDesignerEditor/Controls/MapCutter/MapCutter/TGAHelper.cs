using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Imaging;

namespace MapCutter
{
    class TGAHelper
    {
        private BitMapInfo bitMapInfo = new BitMapInfo();
        private int fillFloors = 2;
        private uint[] defaultData;

        private enum Axis
        {
            AXIS_BEGIN,
            AXIS_INVALID,
            AXIS_VERTICAL,
            AXIS_HORIZONTAL,
            AXIS_VERTICAL_HORIZONTAL,
            AXIS_COUNT
        }

        class TgaFileHeader
        {
	        public byte IDLength = 0; // ID length
	        public byte ColorMapType = 0; // Color map type
	        public byte ImageType = 0; // Image type
	        public byte[] ColorMapSpec = new byte[5]; // Color map specification
	        public short X = 0; // Image specification
            public short Y = 0;
	        public short Width = 0;
            public short Height = 0;
	        public byte PixelDep = 0;
	        public byte Desc = 0;
        }

        class BitMapHeader
        {
            public int biSize = 0;
            public int biWidth = 0;
            public int biHeight = 0;
            public short biPlanes = 0;
            public short biBitCount = 0;
            public int biCompression = 0;
            public int biSizeImage = 0;
            public int biXPelsPerMeter = 0;
            public int biYPelsPerMeter = 0;
            public int biClrUsed = 0;
            public int biClrImportant = 0;
        }

        class RGBQuad
        {
            public byte rgbBlue = 0;
            public byte rgbGreen = 0;
            public byte rgbRed = 0;
            public byte rgbReserved = 0; 
        }

        class BitMapInfo
        {
            public BitMapHeader bitMapHeader = new BitMapHeader();
            public RGBQuad[] bitMapColors = new RGBQuad[] { new RGBQuad() };
        }

        public void CutImage(string inputFileName, string outputDir, int layer, int width, bool fillEdge)
        {
            if (InitScissors(width))
            {
                ScissorMinimap(inputFileName, outputDir, layer, width, fillEdge);
            }
        }

        private void InitBmpInfo(int nWidth)
        {
            bitMapInfo.bitMapHeader.biBitCount = 32;
            bitMapInfo.bitMapHeader.biClrImportant = 0;
            bitMapInfo.bitMapHeader.biClrUsed = 0;
            bitMapInfo.bitMapHeader.biCompression = 0;
            bitMapInfo.bitMapHeader.biHeight = 0 - nWidth;
            bitMapInfo.bitMapHeader.biPlanes = 1;
            bitMapInfo.bitMapHeader.biSize = 40;
            bitMapInfo.bitMapHeader.biSizeImage = nWidth * nWidth * 4;
            bitMapInfo.bitMapHeader.biWidth = nWidth;
            bitMapInfo.bitMapHeader.biXPelsPerMeter = 2834;
            bitMapInfo.bitMapHeader.biYPelsPerMeter = 2834;
            bitMapInfo.bitMapColors[0].rgbBlue = 255;
            bitMapInfo.bitMapColors[0].rgbGreen = 255;
            bitMapInfo.bitMapColors[0].rgbRed = 255;
            bitMapInfo.bitMapColors[0].rgbReserved = 0;
        }

        private bool InitByDefaultData(int nWidth)
        {
            if (defaultData == null)
            {
                defaultData = new uint[nWidth * nWidth];
            }

            for (int i = 0; i < defaultData.Length; i++)
            {
                defaultData[i] = 0xFF000000;
            }
            return true;
        }

        private bool InitScissors(int nWidth)
        {
            InitBmpInfo(nWidth);
            return InitByDefaultData(nWidth);
        }

        private void ScissorMinimap(int nWidth, string filePath)
        {
            FileInfo fi = new FileInfo(filePath);

            if (fi.Exists)
            {
                string fileName = fi.Name.Replace(".bmp", "");
                int nLayer = int.Parse(fileName);
			    ScissorMinimap(fileName, "", nLayer, nWidth, true);               
            }
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "切割图片", MessageBoxButtons.OK,
                                MessageBoxIcon.Error);
            }

        }

        private Bitmap FlipMap(Bitmap bitMap, Axis emAxis)
        {
            int nHeight = 0;
            int nWidth = 0;
            Rectangle rcOrg = new Rectangle(); 

            nWidth = bitMap.Width;
            nHeight = bitMap.Height;
           
            rcOrg.X = 0;
            rcOrg.Y = 0;
            rcOrg.Width = nWidth;
            rcOrg.Height = nHeight;

            Bitmap newBitmap = new Bitmap(nWidth, nHeight, PixelFormat.Format32bppRgb);
            BitmapData bitmapData1 = bitMap.LockBits(rcOrg, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
            BitmapData bitmapData2 = newBitmap.LockBits(rcOrg, ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);

            // 好麻烦，要通过Interop来移动数据
            byte[] buffer2 = new byte[nHeight * bitmapData1.Stride];
            IntPtr ptr2 = bitmapData2.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(ptr2, buffer2, 0, buffer2.Length);
            bitmapData2.Scan0 = ptr2;
            bitmapData2.Stride = bitmapData1.Stride;

            byte[] buffer1 = new byte[nHeight * bitmapData1.Stride];
            IntPtr ptr1 = bitmapData1.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(ptr1, buffer1, 0, buffer1.Length);

            byte[] pInData  = buffer1;
            byte[] pOutData = buffer2;

            switch (emAxis)
            {
                case Axis.AXIS_VERTICAL:
                    {
                        for (int i = 0; i < nWidth; i++)
                        {
                            for (int j = 0; j < nHeight; j++)
                            {
                                for (int k = 0; k < 3; k++)
                                {
                                    pOutData[i * 3 + j * bitmapData2.Stride + k] = pInData[(nWidth - i - 1) * 3 + j * bitmapData1.Stride + k]; 
                                }                                                               
                            }
                        }

                        break;
                    }
                case Axis.AXIS_HORIZONTAL:
                    {
                        for (int i = 0; i < nHeight; i++)
                        {
                            for (int j = 0; j < bitmapData2.Stride; j++)
                            {
                                pOutData[(nHeight - i - 1) * bitmapData2.Stride + j] = pInData[i * bitmapData1.Stride + j];
                            }
                        }

                        break;
                    }
                case Axis.AXIS_VERTICAL_HORIZONTAL:
                    {
                        byte[] byPix = new byte[nHeight * bitmapData1.Stride];
                
                        for (int i = 0; i < nWidth; i++)
                        {
                            for (int j = 0; j < nHeight; j++)
                            {
                                for (int k = 0; k < 3; k++)
                                {
                                    byPix[i * 3 + j * bitmapData1.Stride + k] = pInData[(nWidth - i - 1) * 3 + j * bitmapData1.Stride + k];
                                }
                            }
                        }

                        for (int i = 0; i < nHeight; i++)
                        {
                            for (int j = 0; j < bitmapData2.Stride; j++)
                            {
                                pOutData[(nHeight - i - 1) * bitmapData2.Stride + j] = byPix[i * bitmapData1.Stride + j];
                            }
                        }

                        break;
                    }
            }

            // 好麻烦，要通过Interop来移动数据
            System.Runtime.InteropServices.Marshal.Copy(buffer1, 0, ptr1, buffer1.Length);
            System.Runtime.InteropServices.Marshal.Copy(buffer2, 0, ptr2, buffer2.Length);

            bitMap.UnlockBits(bitmapData1);
            newBitmap.UnlockBits(bitmapData2);            
            
            return newBitmap;
        }

        private bool FillMap(Bitmap srcBitmap, Rectangle srcRect, Bitmap desBitmap, Rectangle desRect, Axis emAxis)
        {
            bool processSuccess = true;
            
            /*
            BitmapData srcBmpData = null;
            BitmapData bmpData = null;
            Bitmap cpyBitmap = null;
            Bitmap flipCpyBitmap = null;
            Rectangle rect = new Rectangle();

            if (emAxis != Axis.AXIS_INVALID && emAxis != Axis.AXIS_BEGIN)
            {
                cpyBitmap = srcBitmap.Clone(srcRect, PixelFormat.Format32bppRgb);                
                flipCpyBitmap = FlipMap(cpyBitmap, emAxis);

                if (flipCpyBitmap == null)
                {
                    return false;
                }

                rect.X = 0;
                rect.Y = 0;
                rect.Width  = flipCpyBitmap.Width;
                rect.Height = flipCpyBitmap.Height;
                srcBmpData = flipCpyBitmap.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
            }
            else
            {
                srcBmpData = srcBitmap.LockBits(srcRect, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
            }

            bmpData = desBitmap.LockBits(desRect, ImageLockMode.UserInputBuffer, PixelFormat.Format24bppRgb, srcBmpData);            
            desBitmap.UnlockBits(bmpData);

            if (emAxis != Axis.AXIS_INVALID && emAxis != Axis.AXIS_BEGIN)
            {
                flipCpyBitmap.UnlockBits(srcBmpData);
            }
            else
            {
                srcBitmap.UnlockBits(srcBmpData);
            }

            processSuccess = true;

            if (cpyBitmap != null)
            {
                cpyBitmap.Dispose();
            }
            
            if (flipCpyBitmap != null)
            {
                flipCpyBitmap.Dispose();
            }
            */

            if (emAxis != Axis.AXIS_INVALID && emAxis != Axis.AXIS_BEGIN)
            {
                Bitmap tempBMP = srcBitmap.Clone(srcRect, PixelFormat.Format32bppRgb);
                Bitmap copyImage = FlipMap(tempBMP, emAxis);

                if (copyImage == null)
                {
                    return false;
                }

                Rectangle rectangle = new Rectangle();
                rectangle.Width = copyImage.Width;
                rectangle.Height = copyImage.Height;
                Graphics g = Graphics.FromImage(desBitmap);
                g.DrawImage(copyImage, desRect, rectangle, GraphicsUnit.Pixel);
                tempBMP.Dispose();
            }
            else
            {
                Graphics g = Graphics.FromImage(desBitmap);
                g.DrawImage(srcBitmap, desRect, srcRect, GraphicsUnit.Pixel);
            }                       

            return processSuccess;
        }

        private Bitmap GetCompletionBitmap(Bitmap minimap, int nSize)
        {
            bool processSuccess = true;
            Bitmap newBitMap = null;

            int nWidth = 0;
            int nHeight = 0;
            int nDesWidth = 0;
            int nDesHeight = 0;
            int nFillSize = 0;
            int nTopDist = 0;
            int nLeftDist = 0;
            int nRightDist = 0;
            int nBottomDist = 0;

            Rectangle srcRect = new Rectangle();
            Rectangle desRect = new Rectangle();

            nWidth = minimap.Width;
            nHeight = minimap.Height;
            nFillSize = fillFloors * nSize;

            if (!(nWidth >= nFillSize && nHeight >= nFillSize))
            {
                return null;
            }

            nDesWidth = nFillSize + nWidth + nFillSize + (nSize - nWidth % nSize) % nSize;
            nDesHeight = nFillSize + nHeight + nFillSize + (nSize - nHeight % nSize) % nSize;
            nTopDist = nFillSize;
            nLeftDist = nFillSize;
            nRightDist = nDesWidth - nWidth - nFillSize;
            nBottomDist = nDesHeight - nHeight - nFillSize;

            newBitMap = new Bitmap(nDesWidth, nDesHeight, PixelFormat.Format32bppRgb);

            // Middle
            srcRect.X = 0;
            srcRect.Y = 0;
            srcRect.Width = nWidth;
            srcRect.Height = nHeight;
            desRect.Width = nWidth;
            desRect.Height = nHeight;
            desRect.X = nLeftDist;
            desRect.Y = nBottomDist;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_INVALID);
            if (!processSuccess)
            {
                return null;
            }

            // BOTTOMLEFT
            srcRect.X = 0;
            srcRect.Y = 0;
            srcRect.Width = nLeftDist;
            srcRect.Height = nBottomDist;
            desRect = srcRect;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            // BOTTOMRIGHT
            srcRect.X = nWidth - nRightDist;
            srcRect.Y = 0;
            srcRect.Width = nRightDist;
            srcRect.Height = nBottomDist;
            desRect.Width = nRightDist;
            desRect.Height = nBottomDist;

            desRect.X = nDesWidth - nRightDist;
            desRect.Y = 0;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            // TOPLEFT
            srcRect.X = 0;
            srcRect.Y = nHeight - nTopDist;
            srcRect.Width = nLeftDist;
            srcRect.Height = nTopDist;
            desRect.Width = nLeftDist;
            desRect.Height = nTopDist;

            desRect.X = 0;
            desRect.Y = nDesHeight - nFillSize;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            // TOPRIGHT
            srcRect.X = nWidth - nRightDist;
            srcRect.Y = nHeight - nTopDist;
            srcRect.Width = desRect.Width = nRightDist;
            srcRect.Height = desRect.Height = nTopDist;

            desRect.X = nDesWidth - nRightDist;
            desRect.Y = nDesHeight - nTopDist;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            // LEFT
            srcRect.X = 0;
            srcRect.Y = 0;
            srcRect.Width = nLeftDist;
            srcRect.Height = nHeight;
            desRect.Width = nLeftDist;
            desRect.Height = nHeight;

            desRect.X = 0;
            desRect.Y = nBottomDist;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL);
            if (!processSuccess)
            {
                return null;
            }

            // BOTTOM
            srcRect.X = 0;
            srcRect.Y = 0;
            srcRect.Width  = nWidth;
            srcRect.Height  = nBottomDist;
            desRect.Width = nWidth;
            desRect.Height = nBottomDist;

            desRect.X = nLeftDist;
            desRect.Y = 0;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            // RIGHT
            srcRect.X = nWidth - nRightDist;
            srcRect.Y = 0;
            srcRect.Width = nRightDist;
            srcRect.Height = nHeight;
            desRect.Width = nRightDist;
            desRect.Height = nHeight;

            desRect.X = nDesWidth - nRightDist;
            desRect.Y = nBottomDist;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_VERTICAL);
            if (!processSuccess)
            {
                return null;
            }

            // TOP
            srcRect.X = 0;
            srcRect.Y = nHeight - nTopDist;
            srcRect.Width = nWidth;
            srcRect.Height = nTopDist;
            desRect.Width = nWidth;
            desRect.Height = nTopDist;

            desRect.X = nLeftDist;
            desRect.Y = nDesHeight - nTopDist;

            processSuccess = FillMap(minimap, srcRect, newBitMap, desRect, Axis.AXIS_HORIZONTAL);
            if (!processSuccess)
            {
                return null;
            }

            processSuccess = true;

            return newBitMap;
        }

        private bool IsBmpDataNull(Bitmap bmp)
        {
	        if (bmp == null)
            {
		        return true;
            }

	        BitmapData bmpData = new BitmapData();

	        int w = bmp.Width;
	        int h = bmp.Height;
	        Rectangle rectangle = new Rectangle(0, 0, w, h);
	        bmpData = bmp.LockBits(rectangle, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            // 需要Interop来移动数据，好麻烦
            IntPtr ptr = bmpData.Scan0;
            byte[] buffer = new byte[h * w];
            System.Runtime.InteropServices.Marshal.Copy(ptr, buffer, 0, buffer.Length);

	        if (bmpData.Stride / 4 != w)
            {
                return true;
            }

	        for(int row = 0; row < h; ++row)
	        {
		        for(int col = 0; col < w; ++col)
		        {
			        if ((buffer[col + row * h] & 0x00FFFFFF) != 0)
			        {
				        bmp.UnlockBits(bmpData);
				        return false;
			        }
		        }
	        }

	        bmp.UnlockBits(bmpData);

	        return true;
        }

        public void SaveMinimapBlock(string outputDir, int nLayer, int uX, int uY, Bitmap bmp)
        {
	        string fileName = Path.Combine(outputDir, string.Format("{0}_{1}_{2}plus.tga", nLayer, uY, uX));        	

	        if (!IsBmpDataNull(bmp))
	        {
		        Rectangle rectangle = new Rectangle(0, 0, bmp.Width, bmp.Height);
		        BitmapData bitmapData = bmp.LockBits(rectangle, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

                // 如果目标目录不存在则先创建目录
                if (!Directory.Exists(outputDir))
                {
                    Directory.CreateDirectory(outputDir);
                }

                FileStream fs = new FileStream(fileName, FileMode.OpenOrCreate);
                byte[] buffer;
                List<byte> byteList = new List<byte>();
                		 
       		    TgaFileHeader tgaFileHeader = new TgaFileHeader();       
		        tgaFileHeader.Width = (short)bitmapData.Width;
		        tgaFileHeader.Height = (short)bitmapData.Height;
		        tgaFileHeader.PixelDep = 24;

                if (bitmapData.PixelFormat == PixelFormat.Format32bppArgb)
                {
                    tgaFileHeader.PixelDep = 32;
                }

		        tgaFileHeader.ImageType = 2;
		        tgaFileHeader.Desc = 8;

                // 写入header
                byteList.Add(tgaFileHeader.IDLength);
                byteList.Add(tgaFileHeader.ColorMapType);
                byteList.Add(tgaFileHeader.ImageType);
                byteList.AddRange(tgaFileHeader.ColorMapSpec);
                buffer = BitConverter.GetBytes(tgaFileHeader.X);
                byteList.AddRange(buffer);
                buffer = BitConverter.GetBytes(tgaFileHeader.Y);
                byteList.AddRange(buffer);
                buffer = BitConverter.GetBytes(tgaFileHeader.Width);
                byteList.AddRange(buffer);
                buffer = BitConverter.GetBytes(tgaFileHeader.Height);
                byteList.AddRange(buffer);
                byteList.Add(tgaFileHeader.PixelDep);
                byteList.Add(tgaFileHeader.Desc);
                
                // 需要用Interop来移动数据，真麻烦
                IntPtr ptr = bitmapData.Scan0;
                byte[] bmpBuffer = new byte[(int)(bitmapData.Width * bitmapData.Stride)];
                System.Runtime.InteropServices.Marshal.Copy(ptr, bmpBuffer, 0, bmpBuffer.Length);

                int blockSize = bitmapData.Width * tgaFileHeader.PixelDep / 8;

                for (int i = 0; i < bmp.Height; i++)
                {
                    for (int j = 0; j < blockSize; j++)
                    {
                        byteList.Add(bmpBuffer[(bmp.Height - i - 1) * bitmapData.Stride + j]);
                    }
                }

                buffer = byteList.ToArray();
                fs.Write(buffer, 0, buffer.Length);
                fs.Close();       
         		
                // 需要用Interop来移动数据，真麻烦
                System.Runtime.InteropServices.Marshal.Copy(bmpBuffer, 0, ptr, bmpBuffer.Length);

		        bmp.UnlockBits(bitmapData);
	        }
        }

        private void SaveMinimapSideBlock(string outputDir, int nLayer, int nWidth, int uX, int uY, Bitmap bmp)
        {
            byte[] buffer;
            List<byte> byteList = new List<byte>();
            
            for (int i = 0; i < defaultData.Length; i++)
            {
                buffer = BitConverter.GetBytes(defaultData[i]);
                byteList.AddRange(buffer);
            }

            buffer = byteList.ToArray();

            MemoryStream ms = new MemoryStream(buffer);
            Bitmap newBmp = Bitmap.FromStream(ms) as Bitmap;

            if (newBmp != null)
            {                
                InitByDefaultData(nWidth);

                /*
                Rectangle rcOrg = new Rectangle(0, 0, bmp.Width, bmp.Height);
                BitmapData bmpData = bmp.LockBits(rcOrg, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                Rectangle rc = new Rectangle(0, nWidth - bmp.Height, bmp.Width, bmp.Height);
                newBmp.LockBits(rc, ImageLockMode.UserInputBuffer, PixelFormat.Format32bppArgb, bmpData);
				
                newBmp.UnlockBits(bmpData);
                bmp.UnlockBits(bmpData);
                bmp.Dispose();
                */

                Graphics g = Graphics.FromImage(newBmp);
                Rectangle srcRectangle = new Rectangle(0, 0, bmp.Width, bmp.Height);
                Rectangle descRectangle = new Rectangle(0, nWidth - bmp.Height, bmp.Width, bmp.Height);
                g.DrawImage(bmp, descRectangle, srcRectangle, GraphicsUnit.Pixel);
                bmp.Dispose();

                SaveMinimapBlock(outputDir, nLayer, uX, uY, newBmp);
                newBmp.Dispose();
            }
        }

        private void ScissorMinimap(string fileName, string outputDir, int nLayer, int nWidth, bool bFillEdge)
        {
	        Bitmap minimap = Image.FromFile(fileName) as Bitmap;

            if (bFillEdge)
            {
                if ((fillFloors * nWidth) > minimap.Width || (fillFloors * nWidth) > minimap.Height)
                {
                    return;
                }

                Bitmap newBitMap = GetCompletionBitmap(minimap, nWidth);
                
                if (newBitMap == null)
                {
                    return;
                }

                int nBitHeight = newBitMap.Height;
                int nRow = newBitMap.Width / nWidth;
                int nColomn = nBitHeight / nWidth;
                int nRowID = - fillFloors;
                int nColomnID = - fillFloors;

                for (int i = 0; i < nRow; i++)
                {
                    int nID = nColomnID;

                    for (int j = 0; j < nColomn; j++)
                    {
                        Rectangle rectangle = new Rectangle(i * nWidth, nBitHeight - (j + 1) * nWidth, nWidth, nWidth);
                        Bitmap blockBitMap = newBitMap.Clone(rectangle, PixelFormat.Format32bppArgb);                        
                        SaveMinimapBlock(outputDir, nLayer, nRowID, nID, blockBitMap);
                        blockBitMap.Dispose();			
                        nID += 1;
                    }

                    nRowID += 1;
                }

                newBitMap.Dispose();
                return;
            }

	        int uW = minimap.Width;
	        int uH = minimap.Height;
            int uX = uW / nWidth;
            int uY = uH / nWidth;
            
	        for (int i = 0; i < uX; ++i)
	        {
		        for (int j = 0; j < uY; ++j)
		        {
			        Rectangle rectangle = new Rectangle(i * nWidth, uH - (j + 1) * nWidth, nWidth, nWidth);
			        Bitmap blockBitmap = minimap.Clone(rectangle, PixelFormat.Format32bppArgb);			        
				    SaveMinimapBlock(outputDir, nLayer, i, j, blockBitmap);
				    blockBitmap.Dispose();			
		        }
	        }

	        int uLeftW = uW % nWidth;

	        if (uLeftW > 0)
	        {
		        for (int j = 0; j < uY; ++j)
		        {
			        Rectangle rectangle = new Rectangle(uX * nWidth, uH - (j + 1) * nWidth, uLeftW, nWidth);
			        Bitmap blockBitmap = minimap.Clone(rectangle, PixelFormat.Format32bppArgb);
				    SaveMinimapSideBlock(outputDir, nLayer, nWidth, uX, j, blockBitmap);
				    blockBitmap.Dispose();
		        }
	        }
        
	        int uLeftH = uH % nWidth;
	        if (uLeftH > 0)
	        {
		        for (int i = 0; i < uX; ++i)
		        {
			        Rectangle rectangle = new Rectangle(i * nWidth, 0, nWidth, uLeftH);
			        Bitmap blockBitmap = minimap.Clone(rectangle, PixelFormat.Format32bppArgb);			        
			        SaveMinimapSideBlock(outputDir, nLayer, nWidth, i, uY, blockBitmap);
			        blockBitmap.Dispose();			        
		        }
	        }

	        if (uLeftW > 0 && uLeftH > 0)
	        {
		        Rectangle rectangle = new Rectangle(uX * nWidth, 0, uLeftW, uLeftH);
		        Bitmap blockBitmap = minimap.Clone(rectangle, PixelFormat.Format32bppArgb);		        
		        SaveMinimapSideBlock(outputDir, nLayer, nWidth, uX, uY, blockBitmap);
                blockBitmap.Dispose();
	        }
        }
    }
}
