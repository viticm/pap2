#include "StdAfx.h"
#include "KG3DHeightMap.h"
#include "png.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DHeightMap::KG3DHeightMap(void)
{
	m_nWidth = 0;
	m_nHeight = 0;

	m_fHeightLowest = 0;
	m_fHeightTotal =1;

	m_lpData = NULL;
	m_lpNormal = NULL;
}

KG3DHeightMap::~KG3DHeightMap(void)
{
	UnInit();
}

HRESULT KG3DHeightMap::Init(int nWidth,int nHeight)
{
	UnInit();

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_lpData = new float [ m_nWidth * m_nHeight];
	KGLOG_PROCESS_ERROR(m_lpData);
	ZeroMemory(m_lpData,sizeof(float) * m_nWidth * m_nHeight);
	m_lpNormal = new D3DXVECTOR3 [ m_nWidth * m_nHeight];
	KGLOG_PROCESS_ERROR(m_lpNormal);
	ZeroMemory(m_lpNormal,sizeof(D3DXVECTOR3) * m_nWidth * m_nHeight);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DHeightMap::UnInit()
{
	SAFE_DELETE_ARRAY(m_lpData);
	SAFE_DELETE_ARRAY(m_lpNormal);
	return S_OK;
}

BYTE *KG3DHeightMap::LoadBmpFile(int&nSize,const char FileName[],BITMAPINFO *info)
{
	FILE             *fp;          /* Open file pointer */
	BYTE			 *bits;        /* Bitmap pixel bits */
	int              bitsize;      /* Size of bitmap */
	int              infosize;     /* Size of header information */
	BITMAPFILEHEADER header;       /* File header */

	if ((fp = fopen(FileName, "rb")) == NULL)
		return NULL;

	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return (NULL);
	}

	if (header.bfType != 'MB')	/* Check for BM reversed... */
	{
		/* Not a bitmap file - return NULL... */
		fclose(fp);
		return (NULL);
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	/*if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
	fclose(fp);
	return (NULL);
	}*/

	if (fread(info, 1, infosize, fp) < (size_t)infosize)
	{
		//	free(info);
		fclose(fp);
		return (NULL);
	}

	/* Now that we have all the header info read in, allocate memory for *
	* the bitmap and read *it* in...                                    */
	if ((bitsize = (info)->bmiHeader.biSizeImage) == 0)
	{
		bitsize = ((info)->bmiHeader.biWidth * (info)->bmiHeader.biBitCount + 7) / 8 * abs((info)->bmiHeader.biHeight);
	}

	nSize = bitsize;

	if ((bits = new BYTE[bitsize]) == NULL)
	{
		/* Couldn't allocate memory - return NULL! */
		//	free(info);
		fclose(fp);
		return (NULL);
	}

	if (fread(bits, 1, bitsize, fp) <(size_t) bitsize)
	{
		/* Couldn't read bitmap - free memory and return NULL! */
		//	free(info);
		SAFE_DELETE_ARRAY(bits);
		fclose(fp);
		return (NULL);
	}

	/* OK, everything went fine - return the allocated bitmap... */
	fclose(fp);
	return (bits);

	//return S_OK;
}

BYTE *KG3DHeightMap::LoadPngFile(const char file_name[])
{
#pragma warning(disable:4611)//_setjmp和C++不兼容
	int nResult = false;
	int nRetCode = false;

	png_FILE_p fpin = NULL;
	png_structp read_ptr = NULL;
	png_infop read_info_ptr = NULL;

	png_bytep image_buf = NULL;
	png_bytep write_pos = NULL;

	unsigned uRowBytes = 0;

	fpin = fopen(file_name, "rb");
	KG_PROCESS_ERROR(fpin);

	read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
		png_error_ptr_NULL, png_error_ptr_NULL);
	KG_PROCESS_ERROR(read_ptr);

	read_info_ptr = png_create_info_struct(read_ptr);
	KG_PROCESS_ERROR(read_info_ptr);

	nRetCode = setjmp(png_jmpbuf(read_ptr));
	KG_PROCESS_ERROR(!nRetCode);   

	png_init_io(read_ptr, fpin);

	png_read_info(read_ptr, read_info_ptr);


	png_get_IHDR(
		read_ptr, 
		read_info_ptr, 
		(png_uint_32 *)&m_PngFileInfo.width, 
		(png_uint_32 *)&m_PngFileInfo.height, 
		&m_PngFileInfo.bit_depth,
		&m_PngFileInfo.color_type, 
		&m_PngFileInfo.interlace_type, 
		&m_PngFileInfo.compression_type,
		&m_PngFileInfo.filter_type
		);
	KGLOG_PROCESS_ERROR(m_PngFileInfo.bit_depth == 16);

	uRowBytes = png_get_rowbytes(read_ptr, read_info_ptr);

	image_buf = (png_bytep)png_malloc(read_ptr, uRowBytes * m_PngFileInfo.height);
	KG_PROCESS_ERROR(image_buf);

	write_pos = image_buf;

	for (int y = 0; y < (int)m_PngFileInfo.height; y++)
	{
		png_read_rows(read_ptr, (png_bytepp)&write_pos, png_bytepp_NULL, 1);
		write_pos += uRowBytes;
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (image_buf)
		{
			delete []image_buf;
			image_buf = NULL;
		}
	}
	if (read_ptr)
	{
		png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
		read_ptr = NULL;
		read_info_ptr = NULL;
	}
	if (fpin)
	{
		fclose(fpin);
		fpin = NULL;
	}
	return image_buf;
#pragma warning(default:4611)
}


HRESULT KG3DHeightMap::CreateFromPngFile(LPSTR pFileName)
{
	HRESULT hr = E_FAIL;

	unsigned char *pbyImageData = NULL;
	unsigned TempHeight = 0;
	float HeightStep = 0.0f;

	//////////////////////////////////////////////////////////////////////////

	pbyImageData = LoadPngFile(pFileName);
	KGLOG_PROCESS_ERROR(pbyImageData);


	m_nWidth = m_PngFileInfo.width;
	m_nHeight = m_PngFileInfo.height; 

	hr = Init(m_nWidth,m_nHeight);
	KGLOG_COM_PROCESS_ERROR(hr);


	for (int z = 0;z < m_nHeight;++z)
	{
		for (int x = 0;x < m_nWidth;++x)
		{
			int index = (z * m_nWidth + x) * 6;
			int indexData = z * m_nWidth + x;

			TempHeight = pbyImageData[index];           //取2字节的红色通道
			TempHeight = TempHeight << 8;
			TempHeight += pbyImageData[index + 1];

			HeightStep = (float)TempHeight / 65536;

			m_lpData[indexData] = HeightStep;
		}
	}

	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pbyImageData);
	return hr;
}

HRESULT KG3DHeightMap::SetHeightScale(float fLowest,float fTotal)
{
	m_fHeightLowest = fLowest;
	m_fHeightTotal = fTotal;
	return S_OK;
}

HRESULT KG3DHeightMap::CreateFromBmpFile(LPCTSTR FileDir)
{
	HRESULT hr = E_FAIL;
	int i,j;

	unsigned char TempHeight;
	unsigned char *pbyImageData = NULL;
	float HeightStep;
	int base = 0;
	int nSize = 0;

	//////////////////////////////////////////////////////////////////////////
	pbyImageData = LoadBmpFile(nSize,(LPSTR)FileDir,&m_Bmpinfo);
	KGLOG_PROCESS_ERROR(pbyImageData);

	m_nWidth  = m_Bmpinfo.bmiHeader.biWidth;
	m_nHeight = m_Bmpinfo.bmiHeader.biHeight;

	hr = Init(m_nWidth,m_nHeight);
	KGLOG_COM_PROCESS_ERROR(hr);

	int  nPixelDepth =(unsigned char)( m_Bmpinfo.bmiHeader.biBitCount / 8);
	//////////////////////////////////////////////////////////////////////////
	int yushu = m_nWidth * nPixelDepth % 4;
	int pitch ;
	DWORD index;
	if (yushu != 0)
		pitch = m_nWidth * nPixelDepth + 4 - yushu;
	else
		pitch = m_nWidth * nPixelDepth;

	for (i = 0;i < m_nHeight;++i)
	{
		for (j = 0;j < m_nWidth;++j)
		{
			index = j * nPixelDepth + i * pitch;//base;
			int indexData = i/*(m_nHeight - i - 1)*/ * m_nWidth + j;

			if((int)index>=nSize)
			{
				TempHeight = 0;
			}
			else
			{
				TempHeight = pbyImageData[index];
			}
			HeightStep = (float)TempHeight / 255;
			m_lpData[indexData] = HeightStep;

		}
		base += pitch;
	}

	SmoothMapHeight();
	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pbyImageData);
	return hr;
}

float KG3DHeightMap::GetHeight(float fX,float fY)
{

	float X = 0 ,Y = 0;
	float u = max(0.0F,min(1.0F,fX));
	float v = max(0.0F,min(1.0F,fY));

	float u_T = u * m_nWidth;
	float v_T = v * m_nHeight;

	X = 0 ;Y = 0;
	float S_X = modf(u_T,&X);
	float S_Z = modf(v_T,&Y);

	float* pA, *pB, *pC, *pD;

	int xx = min(m_nWidth-1,(int)X);
	int zz = min(m_nHeight-1,(int)Y);

	pA = GetVertexPointer(xx,zz);

	if (xx < m_nWidth - 1)
		pB = GetVertexPointer(xx + 1, zz);
	else
		pB = GetVertexPointer(xx,zz);


	if (zz < m_nHeight - 1)
		pD = GetVertexPointer(xx, zz + 1);
	else
		pD = GetVertexPointer(xx,zz);


	if ((xx < m_nWidth - 1) && (zz < m_nHeight - 1))
		pC = GetVertexPointer(xx + 1, zz + 1);
	else
		pC = GetVertexPointer(xx,zz);


	float Height = 0;
	if (S_X + S_Z <= 1.0f)
	{
		float b = *pB - *pA;
		float d = *pD - *pA;
		Height = S_X * b + S_Z * d + *pA;
	}
	else
	{
		float c = *pC - *pB;
		float d = *pD - *pB;
		Height = (S_X - 1) * (c - d) + S_Z * c + *pB;
	}
	return Height * m_fHeightTotal + m_fHeightLowest;
}

D3DXVECTOR3 KG3DHeightMap::GetNormal(float fX,float fY)
{

	float X = 0 ,Y = 0;
	float u = modf(fX,&X);
	float v = modf(fY,&Y);

	float u_T = u * m_nWidth;
	float v_T = v * m_nHeight;

	X = 0 ;Y = 0;
	float S_X = modf(u_T,&X);
	float S_Z = modf(v_T,&Y);

	D3DXVECTOR3* pA, *pB, *pC, *pD;

	int xx = min(m_nWidth-1,(int)X);
	int zz = min(m_nHeight-1,(int)Y);

	pA = GetVertexNormalPointer(xx,zz);
	pB = GetVertexNormalPointer(xx,zz);
	pC = GetVertexNormalPointer(xx,zz);
	pD = GetVertexNormalPointer(xx,zz);

	if (xx < m_nWidth - 1)
		pB = GetVertexNormalPointer(xx + 1, zz);

	if (zz < m_nHeight - 1)
		pD = GetVertexNormalPointer(xx, zz + 1);

	if ((xx < m_nWidth - 1) && (zz < m_nHeight - 1))
		pC = GetVertexNormalPointer(xx + 1, zz + 1);

	D3DXVECTOR3 Height(0,0,0);
	if (S_X + S_Z <= 1.0f)
	{
		D3DXVECTOR3 b = *pB - *pA;
		D3DXVECTOR3 d = *pD - *pA;
		Height = S_X * b + S_Z * d + *pA;
	}
	else
	{
		D3DXVECTOR3 c = *pC - *pB;
		D3DXVECTOR3 d = *pD - *pB;
		Height = (S_X - 1) * (c - d) + S_Z * c + *pB;
	}
	//Height.y *= m_fHeightTotal;

	D3DXVECTOR3 Normal;
	D3DXVec3Normalize(&Normal,&Height);

	return Normal;
}

float* KG3DHeightMap::GetVertexPointer(int nX,int nZ)
{
	ASSERT(m_lpData);
	ASSERT(nX<m_nWidth);
	ASSERT(nZ<m_nHeight);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	return &m_lpData[nZ * m_nWidth + nX];
}

HRESULT KG3DHeightMap::SmoothMapHeight()
{
	int x,z;
	float H[4];

	for (int K=0;K<2;K++)
	{
		for (z = 0;z < m_nHeight;++z)
		{
			for (x = 0;x < m_nWidth;++x)
			{
				if (x<m_nWidth-1)
					H[0] =  *GetVertexPointer(x+1,z);
				else
					H[0] = *GetVertexPointer(x,z);

				if (x>0)
					H[1] =  *GetVertexPointer(x-1,z);
				else
					H[1] = *GetVertexPointer(x,z);

				if (z< m_nHeight-1)
					H[2] =  *GetVertexPointer(x,z+1);
				else
					H[2] = *GetVertexPointer(x,z);

				if (z>0)
					H[3] =  *GetVertexPointer(x,z-1);
				else
					H[3] = *GetVertexPointer(x,z);

				*GetVertexPointer(x,z) = 
					(H[0] + H[1] + H[2] + H[3]) * 0.25F;
			}
		}
	}
	return S_OK;
}
HRESULT KG3DHeightMap::SmoothMapHeight(int xStart,int xEnd,int zStart,int zEnd,float K)
{
	ASSERT(xStart >=0);
	ASSERT(xEnd < m_nWidth);
	ASSERT(zStart >=0);
	ASSERT(zEnd < m_nHeight);

	int i,j;
	float H[4];
	for (i = zStart;i <= zEnd;++i)
	{
		for (j = xStart;j <= xEnd;++j)
		{
			if (j<m_nWidth-1)
				H[0] = *GetVertexPointer(j+1,i);
			else
				H[0] = *GetVertexPointer(j,i);

			if(j>1)
				H[1] =  *GetVertexPointer(j-1,i);
			else
				H[1] = *GetVertexPointer(j,i);

			if (i<m_nHeight-1)
				H[2] = *GetVertexPointer(j,i+1);
			else
				H[2] = *GetVertexPointer(j,i);

			if (i>1)
				H[3] = *GetVertexPointer(j,i-1);
			else
				H[3] = *GetVertexPointer(j,i);

			*GetVertexPointer(j,i) = *GetVertexPointer(j,i) * (1 - K) + 
				(H[0] + H[1] + H[2] + H[3]) * 0.25F * K;
		}
	}
	return S_OK;
}

int KG3DHeightMap::GetPlaneBufferIndex(int nX,int nZ)
{
	nX = min(m_nWidth-1,nX);
	nZ = min(m_nHeight-1,nZ);
	return (nZ * m_nWidth + nX) * 2;
}

HRESULT KG3DHeightMap::ComputeNormal()
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 TempNormal[6];
	D3DXVECTOR3 SunNormal;
	float cell = 100;

	D3DXPLANE* pPlaneBuffer = new D3DXPLANE[m_nWidth*m_nHeight*2];
	KGLOG_PROCESS_ERROR(pPlaneBuffer);


	for (int nZ = 0;nZ < m_nHeight - 1;nZ++ )
	{
		for (int nX = 0;nX < m_nWidth - 1;nX++ )
		{
			int nIndex_A =  nZ    * m_nWidth + nX;
			int nIndex_B =  nZ    * m_nWidth + nX + 1;
			int nIndex_C = (nZ+1) * m_nWidth + nX + 1;
			int nIndex_D = (nZ+1) * m_nWidth + nX;

			int nPlane_1 = GetPlaneBufferIndex(nX,nZ);
			int nPlane_2 = nPlane_1 + 1;

			D3DXVECTOR3 A = D3DXVECTOR3(nX*cell,m_lpData[nIndex_A],nZ*cell);
			D3DXVECTOR3 B = D3DXVECTOR3((nX+1)*cell,m_lpData[nIndex_B],nZ*cell);
			D3DXVECTOR3 C = D3DXVECTOR3((nX+1)*cell,m_lpData[nIndex_C],(nZ+1)*cell);
			D3DXVECTOR3 D = D3DXVECTOR3(nX*cell,m_lpData[nIndex_D],(nZ+1)*cell);

			A.y = A.y * m_fHeightTotal + m_fHeightLowest ;
			B.y = B.y * m_fHeightTotal + m_fHeightLowest ;
			C.y = C.y * m_fHeightTotal + m_fHeightLowest ;
			D.y = D.y * m_fHeightTotal + m_fHeightLowest ;

			D3DXPlaneFromPoints(&pPlaneBuffer[nPlane_1],
				&A,
				&D,
				&C);

			D3DXPlaneFromPoints(&pPlaneBuffer[nPlane_2],
				&A,
				&C,
				&B);

			m_lpNormal[nIndex_A] = D3DXVECTOR3(0,0,0);
			m_lpNormal[nIndex_B] = D3DXVECTOR3(0,0,0);
			m_lpNormal[nIndex_C] = D3DXVECTOR3(0,0,0);
			m_lpNormal[nIndex_D] = D3DXVECTOR3(0,0,0);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int nZ = 0;nZ < m_nHeight -1;nZ++ )
	{
		for (int nX = 0;nX < m_nWidth -1;nX++ )
		{
			int index = GetPlaneBufferIndex(nX,nZ);
			TempNormal[0].x = pPlaneBuffer[index].a;
			TempNormal[0].y = pPlaneBuffer[index].b;
			TempNormal[0].z = pPlaneBuffer[index].c;
			TempNormal[1].x = pPlaneBuffer[index+1].a;
			TempNormal[1].y = pPlaneBuffer[index+1].b;
			TempNormal[1].z = pPlaneBuffer[index+1].c;

			int nIndex_A =  nZ    * m_nWidth + nX;
			int nIndex_B =  nZ    * m_nWidth + nX + 1;
			int nIndex_C = (nZ+1) * m_nWidth + nX + 1;
			int nIndex_D = (nZ+1) * m_nWidth + nX;
			m_lpNormal[nIndex_A] += TempNormal[0];
			m_lpNormal[nIndex_B] += TempNormal[1];
			m_lpNormal[nIndex_C] += TempNormal[1];
			m_lpNormal[nIndex_D] += TempNormal[0];
		}
	}

	for (int nZ = 0;nZ < m_nHeight;nZ++ )
	{
		for (int nX = 0;nX < m_nWidth;nX++ )
		{
			int nIndex_A =  nZ * m_nWidth + nX;

			D3DXVec3Normalize(&m_lpNormal[nIndex_A],&m_lpNormal[nIndex_A]);
		}
	}

	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pPlaneBuffer);
	return hr;
}

HRESULT KG3DHeightMap::ComputeNormal(int xStart,int xEnd,int zStart,int zEnd)
{
	HRESULT hr = E_FAIL;

	ASSERT(xStart >=0);
	ASSERT(xEnd < m_nWidth);
	ASSERT(zStart >=0);
	ASSERT(zEnd < m_nHeight);
	int xStartEx = max(xStart - 1,0);
	int zStartEx = max(zStart - 1,0);
	int xEndEx   = min(xEnd + 1 , m_nWidth - 1);
	int zEndEx   = min(zEnd + 1 , m_nHeight- 1);
	
	D3DXVECTOR3 TempNormal[6];
	D3DXVECTOR3 SunNormal;
	float cell = 100;
	
	D3DXPLANE* pPlaneBuffer = NULL;
	int nWidth  = xEndEx - xStartEx;
	int nHeight = zEndEx - zStartEx;
	int Num = (nWidth)*(nHeight)*2 ;
	KG_PROCESS_ERROR(Num > 0);

	pPlaneBuffer = new D3DXPLANE[Num];
	KGLOG_PROCESS_ERROR(pPlaneBuffer);
	
	for (int nZ = zStartEx;nZ < zEndEx;nZ++ )
	{
		for (int nX = xStartEx;nX < xEndEx;nX++ )
		{
			int nIndex_A =  nZ    * m_nWidth + nX;
			int nIndex_B =  nZ    * m_nWidth + nX + 1;
			int nIndex_C = (nZ+1) * m_nWidth + nX + 1;
			int nIndex_D = (nZ+1) * m_nWidth + nX;
			int nPlane_1 = ((nZ - zStartEx) * nWidth + (nX - xStartEx))*2;
			int nPlane_2 = nPlane_1 + 1;
			ASSERT(nPlane_1 >= 0);
			ASSERT(nPlane_1 < nWidth*nHeight*2 -1);
			D3DXVECTOR3 A = D3DXVECTOR3(nX*cell,m_lpData[nIndex_A],nZ*cell);
			D3DXVECTOR3 B = D3DXVECTOR3((nX+1)*cell,m_lpData[nIndex_B],nZ*cell);
			D3DXVECTOR3 C = D3DXVECTOR3((nX+1)*cell,m_lpData[nIndex_C],(nZ+1)*cell);
			D3DXVECTOR3 D = D3DXVECTOR3(nX*cell,m_lpData[nIndex_D],(nZ+1)*cell);
			
			A.y = A.y * m_fHeightTotal + m_fHeightLowest ;
			B.y = B.y * m_fHeightTotal + m_fHeightLowest ;
			C.y = C.y * m_fHeightTotal + m_fHeightLowest ;
			D.y = D.y * m_fHeightTotal + m_fHeightLowest ;

			D3DXPlaneFromPoints(&pPlaneBuffer[nPlane_1],
				&A,
				&D,
				&C);

			D3DXPlaneFromPoints(&pPlaneBuffer[nPlane_2],
				&A,
				&C,
				&B);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int nZ = zStartEx;nZ < zEndEx;nZ++ )
	{
		for (int nX = xStartEx;nX < xEndEx;nX++ )
		{
			int index = ((nZ - zStartEx) * nWidth + (nX - xStartEx))*2;
			TempNormal[0].x = pPlaneBuffer[index].a;
			TempNormal[0].y = pPlaneBuffer[index].b;
			TempNormal[0].z = pPlaneBuffer[index].c;
			TempNormal[1].x = pPlaneBuffer[index+1].a;
			TempNormal[1].y = pPlaneBuffer[index+1].b;
			TempNormal[1].z = pPlaneBuffer[index+1].c;

			int nIndex_A =  nZ    * m_nWidth + nX;
			int nIndex_B =  nZ    * m_nWidth + nX + 1;
			int nIndex_C = (nZ+1) * m_nWidth + nX + 1;
			int nIndex_D = (nZ+1) * m_nWidth + nX;
			if(nX != xStartEx && nZ != zStartEx)
				m_lpNormal[nIndex_A] += TempNormal[0];
			if(nX != (xEndEx-1) && nZ !=zStartEx)
				m_lpNormal[nIndex_B] += TempNormal[1];
			if(nX != (xEndEx-1) && nZ != (zEndEx-1))
				m_lpNormal[nIndex_C] += TempNormal[1];
			if( nX != xStartEx && nZ != (zEndEx-1))
				m_lpNormal[nIndex_D] += TempNormal[0];
		}
	}
	for (int nZ = zStart;nZ <= zEnd;nZ++ )
	{
		for (int nX = xStart;nX <= xEnd;nX++ )
		{
			int nIndex_A =  nZ * m_nWidth + nX;
			D3DXVec3Normalize(&m_lpNormal[nIndex_A],&m_lpNormal[nIndex_A]);
		}
	}
	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pPlaneBuffer);
	return hr;
}

HRESULT KG3DHeightMap::SmoothNormal()
{
	int x,z;
	D3DXVECTOR3 H[4];
	
	for (int K=0;K<2;K++)
	{
		for (z = 0;z < m_nHeight;++z)
		{
			for (x = 0;x < m_nWidth;++x)
			{
				if (x< m_nWidth - 1)
					H[0] =  *GetVertexNormalPointer(x+1,z);
				else
					H[0] = *GetVertexNormalPointer(x,z);

				if (x>0)
					H[1] =  *GetVertexNormalPointer(x-1,z);
				else
					H[1] = *GetVertexNormalPointer(x,z);

				if (z< m_nHeight - 1)
					H[2] =  *GetVertexNormalPointer(x,z+1);
				else
					H[2] = *GetVertexNormalPointer(x,z);

				if (z>0)
					H[3] =  *GetVertexNormalPointer(x,z-1);
				else
					H[3] = *GetVertexNormalPointer(x,z);

				*GetVertexNormalPointer(x,z) = 
					(H[0] + H[1] + H[2] + H[3]) * 0.25F;
			}
		}
	}
	return S_OK;
}
HRESULT KG3DHeightMap::SmoothNormal(int xStart,int xEnd,int zStart,int zEnd)
{
	ASSERT(xStart >=0);
	ASSERT(xEnd < m_nWidth);
	ASSERT(zStart >=0);
	ASSERT(zEnd < m_nHeight);

	int x,z;
	D3DXVECTOR3 H[4];
	for (int K=0;K<2;K++)
	{
		for (z = zStart;z <= zEnd;++z)
		{
			for (x = xStart;x <= xEnd;++x)
			{
				if (x< m_nWidth - 1)
					H[0] =  *GetVertexNormalPointer(x+1,z);
				else
					H[0] = *GetVertexNormalPointer(x,z);

				if (x>0)
					H[1] =  *GetVertexNormalPointer(x-1,z);
				else
					H[1] = *GetVertexNormalPointer(x,z);

				if (z< m_nHeight - 1)
					H[2] =  *GetVertexNormalPointer(x,z+1);
				else
					H[2] = *GetVertexNormalPointer(x,z);

				if (z>0)
					H[3] =  *GetVertexNormalPointer(x,z-1);
				else
					H[3] = *GetVertexNormalPointer(x,z);

				*GetVertexNormalPointer(x,z) = (H[0] + H[1] + H[2] + H[3]) * 0.25F;
			}
		}
	}
	return S_OK;
}

D3DXVECTOR3* KG3DHeightMap::GetVertexNormalPointer(int nX,int nZ)
{
	ASSERT(m_lpNormal);
	ASSERT(nX<m_nWidth);
	ASSERT(nZ<m_nHeight);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	return &m_lpNormal[nZ * m_nWidth + nX];
}

int KG3DHeightMap::GetMapHeight()
{
	return m_nHeight;
}
int KG3DHeightMap::GetMapWidth()
{
	return m_nWidth;
}

HRESULT KG3DHeightMap::SetHeight(int xStart,int xEnd,int zStart,int zEnd,float fHeight)
{
	int i,j;
	for (i = zStart;i <= zEnd;++i)
	{
		for (j = xStart;j <= xEnd;++j)
		{
			*GetVertexPointer(j,i) = fHeight;
		}
	}
	return S_OK;
}

HRESULT KG3DHeightMap::SaveToHeightMapFile(const char cszFileName[])
{
	HRESULT hr = E_FAIL;
	size_t n = 0; 
	FILE* pFile = fopen(cszFileName,"wb");
	KGLOG_PROCESS_ERROR(pFile);

	KG_PROCESS_ERROR(m_lpData);
	KG_PROCESS_ERROR(m_lpNormal);

	DWORD Version = 0;
	fwrite(&Version,sizeof(DWORD),1,pFile);
	fwrite(&m_nWidth,sizeof(int),1,pFile);
	fwrite(&m_nHeight,sizeof(int),1,pFile);
	fwrite(&m_fHeightLowest,sizeof(float),1,pFile);
	fwrite(&m_fHeightTotal,sizeof(float),1,pFile);
	n = fwrite(m_lpData,sizeof(float),m_nWidth*m_nHeight,pFile);
	KGLOG_PROCESS_ERROR( n ==(size_t)(m_nWidth*m_nHeight));
	n = fwrite(m_lpNormal,sizeof(D3DXVECTOR3),m_nWidth*m_nHeight,pFile);
	KGLOG_PROCESS_ERROR( n ==(size_t)(m_nWidth*m_nHeight));
	hr = S_OK;
Exit0:
	fclose(pFile);
	return hr;
}

HRESULT KG3DHeightMap::LoadFromHeightMapFile(const char cszFileName[])
{
	HRESULT hrCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
	IFile* pFile = NULL;
	size_t t = 0;
	pFile = g_OpenFile( cszFileName );
	KGLOG_PROCESS_ERROR(pFile);

	DWORD Version = 0;
	int Width = 0;
	int Height = 0;

	pFile->Read( &Version,sizeof(DWORD) );
	pFile->Read( &Width,sizeof(int) );
	pFile->Read( &Height,sizeof(int) );
	pFile->Read( &m_fHeightLowest,sizeof(float));
	pFile->Read( &m_fHeightTotal,sizeof(float));
	hrCode = Init(Width,Height);
	KG_COM_PROCESS_ERROR(hrCode);
	t = pFile->Read(m_lpData,sizeof(float)*Width*Height);
	KGLOG_PROCESS_ERROR(t==(size_t)(sizeof(float)*Width*Height));
	t = pFile->Read(m_lpNormal,sizeof(D3DXVECTOR3)*Width*Height);
	KGLOG_PROCESS_ERROR(t==(size_t)(sizeof(D3DXVECTOR3)*Width*Height));
	hrResult = S_OK;
Exit0:
	KG_COM_RELEASE( pFile );
	return hrResult;
}

HRESULT KG3DHeightMap::CreateFromR32File(int nWidth,int nHeight,LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	IFile* pFile = NULL;
	size_t t = 0;
	
	KGLOG_PROCESS_ERROR(pFileName);

	pFile = g_OpenFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

	hr = Init(nWidth,nHeight);
	KG_COM_PROCESS_ERROR(hr);

	t = pFile->Read(m_lpData,sizeof(float)*nWidth*nHeight);
	KGLOG_PROCESS_ERROR(t==(size_t)(sizeof(float)*nWidth*nHeight));

	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);

	return hr;
}

HRESULT KG3DHeightMap::SaveToR32File(int nWidth,int nHeight,LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	IFile* pFile = NULL;
	size_t t = 0;

	KGLOG_PROCESS_ERROR(pFileName);

	pFile = g_OpenFile( pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	t = pFile->Write(m_lpData,sizeof(float)*nWidth*nHeight);
	KGLOG_PROCESS_ERROR(t==(size_t)(sizeof(float)*nWidth*nHeight));

	pFile->Close();

	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);

	return hr;
}

HRESULT KG3DHeightMap::TranslateAbsoluteData()
{
	for (int nZ = 0;nZ < m_nHeight;nZ++ )
	{
		for (int nX = 0;nX < m_nWidth;nX++ )
		{
			int nIndex =  nZ * m_nWidth + nX;

			m_lpData[nIndex] = m_lpData[nIndex]* m_fHeightTotal + m_fHeightLowest;
		}
	}
	SetHeightScale(0, 1);
	return S_OK;
}