namespace Water
{
struct WATER_SURFACE
{
	float fHeight;     //高度
	float fVelocity;   //波动幅度
	bool bIsborder;    //是否是边界
	D3DXVECTOR3 vecNormal;   
//	D3DCOLOR    dwDiffuse;
	D3DXVECTOR2 vecTex;
	D3DXVECTOR3 vecPos;
	//	D3DXVECTOR3 vecCausticPos;
	//	D3DCOLOR    dwCausticDiffuse;

};
/*
struct WATER_REFRACT
{
	// Vrefract = (V + refract * N) * norm
	float fRefract;
	float fRefractNorm; 
	DWORD dwDiffuse;
}; */


#if defined(_X86) && !defined(_WIN64)
inline int f2i(float flt) 
{
	volatile int n; 

	__asm 
	{
		fld flt
			fistp n
	}

	return n;
}
#else
inline int f2i(float flt) 
{
	return (int) flt;
}
#endif




class CWater
{
public:

	FLOAT m_fAvgHeight;
	UINT m_uIndices;
	UINT m_uVertices;
	UINT m_uWaterSize; 

	WATER_SURFACE *m_pSurface;


	CWater()
	{
		m_uIndices  = 0;
		m_uVertices = 0;
		m_uWaterSize = 0;
		m_pSurface     = NULL;
	}
	~CWater()
	{
		if(m_pSurface)
			delete [] m_pSurface;
	}


	HRESULT Initialize( LPD3DXMESH m_pWaterUp, DWORD m_dwNumPoly_X, DWORD m_dwNumPoly_Z )
	{

		// Calculate number of vertices and indices
		m_uWaterSize = m_dwNumPoly_X+1;
		m_uVertices = (m_dwNumPoly_X+1)*(m_dwNumPoly_Z+1);
		m_uIndices  = m_uVertices * 2;

	/*
		// Create refraction table
		static WATER_REFRACT Refract[512];

		if(!m_pRefract)
		{
			m_pRefract = &Refract[256];

			for(UINT u = 0; u < 256; u++)
			{        
				float fCos0 = (float) u / (float) 256.0f;
				float f0 = acosf(fCos0);
				float fSin0 = sinf(f0);

				float fSin1 = fSin0 / 1.333f; // water
				float f1 = asinf(fSin1);
				float fCos1 = cosf(f1);

				m_pRefract[u].fRefract = fSin0 / fSin1 * fCos1 - fCos0;
				m_pRefract[u].fRefractNorm = - fSin1 / fSin0;
				m_pRefract[u].dwDiffuse = ((((0xff - u)*(0xff - u)*(0xff - u)) << 8) & 0xff000000);

				Refract[u] = Refract[256];
			}
		}
	*/
		
		VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
		if (FAILED(m_pWaterUp->LockVertexBuffer(0,(void**)&pVers)))
			return E_FAIL;

		// Create maps
		if(!m_pSurface)
		{
			if( ( m_pSurface = new WATER_SURFACE[m_uVertices+1] ) == NULL )
				return E_OUTOFMEMORY;
			else{
				memset(m_pSurface, 0x00, (m_uVertices+1) * sizeof(WATER_SURFACE));
				for(UINT i=0; i<m_uVertices; i++)
				{
					m_pSurface[i].vecPos = pVers[i].p;
					m_pSurface[i].vecTex.x = pVers[i].tu1;
					m_pSurface[i].vecTex.y = pVers[i].tv1;
					m_pSurface[i].bIsborder = 0;
				}
				for( UINT i = 0; i < m_uWaterSize; i++ )
				{
					m_pSurface[i].bIsborder = 1;
					m_pSurface[i*m_uWaterSize].bIsborder = 1;
					m_pSurface[(i+1)*m_uWaterSize-1].bIsborder = 1;
					m_pSurface[m_uVertices - 1 - i].bIsborder = 1;
				}
			}
		}

		if (FAILED(m_pWaterUp->UnlockVertexBuffer()))
			return E_FAIL;
		return S_OK;
	}






	HRESULT Drop()
	{
#define WATER_INDEX(x, y) \
	((x) | ((y) << 6))

#define WATER_INDEX_WRAP(x, y) \
	(((x) & (m_uWaterSize-1)) | (((y) & (m_uWaterSize-1)) << 6))
		UINT uX = rand();
		UINT uY = rand();
		//产生水纹
		/////////////////修改代码可产生不同类型的水纹//////////////////////

		for(int i=0; i<64*20; i++)  
		{
			uX = rand();
			uY = rand();
			m_pSurface[WATER_INDEX_WRAP(uX, uY)].fVelocity -= 0.02f;

		}  
		////////////////////////////////////////////////////////////////////
		return S_OK;
	}


	HRESULT Update()
	{

		UINT uXN, uX, uXP,uYN, uY0, uYP;

		// 改变网格顶点高度值
		m_fAvgHeight = 0.0f;
		WATER_SURFACE *pSurface = m_pSurface;

		uYN  = m_uVertices - m_uWaterSize;
		uY0  = 0;
		uYP  = m_uWaterSize;

		do
		{
			uXN  = m_uWaterSize - 1;
			uX   = 0;
			uXP  = 1;

			do
			{
				// Bowl

				float fDesire;

				if( pSurface->bIsborder == 1)
				{
					fDesire = 0.0f;
					pSurface->fHeight = 0.0f;
					pSurface->fVelocity = 0.0f;
				}
				else
				{

					fDesire = 
						(m_pSurface[uXN + uYN].fHeight + 
						m_pSurface[uXP + uYN].fHeight + 
						m_pSurface[uXN + uYP].fHeight + 
						m_pSurface[uXP + uYP].fHeight) * (1.0f / 12.0f)
						+
						(m_pSurface[uX  + uYN].fHeight +
						m_pSurface[uXN + uY0].fHeight +
						m_pSurface[uXP + uY0].fHeight +
						m_pSurface[uX  + uYP].fHeight) * (2.0f / 12.0f);
				}

				pSurface->fVelocity += 0.25f * (fDesire - pSurface->fHeight);
				m_fAvgHeight += pSurface->fHeight + pSurface->fVelocity;

				pSurface++;

				uXN = uX;
				uX  = uXP;
				uXP = (uXP + 1) & (m_uWaterSize-1);
			}
			while(uX);

			uYN = uY0;
			uY0 = uYP;
			uYP = (uYP + m_uWaterSize) & ((m_uWaterSize-1) << 6);
		}
		while(uY0);

		m_fAvgHeight /= (float) m_uVertices;

		D3DXVECTOR3 vec;
		D3DXVECTOR3 vecP, vecN;
		// 重新计算normal值
		pSurface = m_pSurface;

		uYN  = m_uVertices - m_uWaterSize;
		uY0  = 0;
		uYP  = m_uWaterSize;

		do
		{

			uXN  = m_uWaterSize - 1;
			uX   = 0;
			uXP  = 1;

			do
			{
				//高度值
				pSurface->vecPos.y = pSurface->fHeight = pSurface->fHeight + pSurface->fVelocity - m_fAvgHeight;

				float f;

				f = m_pSurface[uXN + uYN].fHeight - m_pSurface[uXP + uYP].fHeight; vecN.x = vecN.z = f;           
				f = m_pSurface[uX  + uYN].fHeight - m_pSurface[uX  + uYP].fHeight; vecN.z += f;
				f = m_pSurface[uXP + uYN].fHeight - m_pSurface[uXN + uYP].fHeight; vecN.x -= f; vecN.z += f;
				f = m_pSurface[uXN + uY0].fHeight - m_pSurface[uXP + uY0].fHeight; vecN.x += f;

				vecN.y = 1.0f;
				D3DXVec3Normalize(&vecN, &vecN);
				pSurface->vecNormal = vecN;
				pSurface++;
				
/*
				//纹理及refract效果
				vecP.x = pSurface->vecPos.x;
				vecP.y = pSurface->vecPos.y;
				vecP.z = pSurface->vecPos.z;
				D3DXVECTOR3 vecPos = D3DXVECTOR3(0,40.0f,40.0f);
				D3DXVec3Subtract(&vec, &vecPos, &vecP);
				D3DXVec3Normalize(&vec, &vec);

				WATER_REFRACT *pRefract;
				pRefract = m_pRefract + f2i(D3DXVec3Dot(&vec, &vecN) * 255.0f);
				float cc = f2i(D3DXVec3Dot(&vec, &vecN) * 255.0f);
				pSurface->vecNormal = vecN;
				pSurface->dwDiffuse = pRefract->dwDiffuse;

				vecPos = D3DXVECTOR3(0,15.0,15.0);
				vecP = 	vecP/200.0f;
				vecP.y -= 20.0f;
				// Update texture coords and diffuse based upon refraction
				D3DXVec3Subtract(&vec, &vecPos, &vecP);
				D3DXVec3Normalize(&vec, &vec);

				// Bowl
				D3DXVECTOR3 vecD;
				vecD = (vecN * pRefract->fRefract + vec) * pRefract->fRefractNorm;

				float fC = D3DXVec3Dot(&vecP, &vecP) - 200*30;



				if(fC < 0.0f)
				{
					float fB = D3DXVec3Dot(&vecD, &vecP) * 2.0f;
					float fD = fB * fB - 4.0f * fC;
					float fScale = (-fB + sqrtf(fD)) * 0.5f;
					pSurface->vecTex.x = (vecD.x * fScale + vecP.x) * (1/(64.0f));
					pSurface->vecTex.y = (vecD.z * fScale + vecP.z) * (1/(64.0f));
				}    

					//		pSurface->vecTex.x += 0.001f;
					//		pSurface->vecTex.y += 0.001f; 
					//		pSurface->vecTex.x+=(vecP.y-200)/(40.0f);

					//		pSurface->vecTex.x += 0.001f;
					//		pSurface->vecTex.y += 0.001f;
*/
				
				pSurface->vecTex.y +=(pSurface->vecPos.y-200)/(40.0f);
				uXN = uX;
				uX  = uXP;
				uXP = (uXP + 1) & (m_uWaterSize-1);
			}
			while(uX);



			uYN = uY0;
			uY0 = uYP;
			uYP = (uYP + m_uWaterSize) & ((m_uWaterSize-1) << 6);
		}
		while(uY0);

		/*
		//CalcCaustics效果
		BOOL bCalcCaustics = 1;
		pSurface = m_pSurface;
		if(bCalcCaustics)
		{
		#define TABLE_SIZE 8
		static DWORD Table[TABLE_SIZE];
		if(!Table[0])
		{
		for(UINT u = 0; u < TABLE_SIZE; u++)
		Table[u] = (0x40 / (u + 1)) * 0x00010101;
		}

		for(uY = 0; uY < m_uWaterSize; uY++)
		{
		for(uX = 0; uX < m_uWaterSize; uX++)
		{
		WATER_REFRACT *pRefract;
		pRefract = m_pRefract + f2i(pSurface->vecNormal.y * 255.0f);

		// Bowl
		D3DXVECTOR3 vecD, vecP;
		//   vecD = (pVertex->m_vecNormal * pRefract->fRefract + vecLight) * pRefract->fRefractNorm;
		vecP = pSurface->vecPos;
		//    vecP.y -= WATER_SPHERE_HEIGHT;

		//     float fC = D3DXVec3Dot(&vecP, &vecP) - WATER_SPHERE_RADIUS2;



		pSurface->vecCausticPos.x = vecP.x;
		pSurface->vecCausticPos.y = 202.0f;
		pSurface->vecCausticPos.z = vecP.z;


		if(uX && uY)
		{
		float fArea;
		fArea = (pSurface[-m_uWaterSize - 1].vecCausticPos.x - pSurface->vecCausticPos.x) *
		(pSurface[-m_uWaterSize    ].vecCausticPos.z - pSurface->vecCausticPos.z) -
		(pSurface[-m_uWaterSize - 1].vecCausticPos.z - pSurface->vecCausticPos.z) *
		(pSurface[-m_uWaterSize    ].vecCausticPos.x - pSurface->vecCausticPos.x);

		if(fArea < 0.0f)
		fArea = -fArea;

		UINT u = f2i(fArea * fArea * 4.0f);
		pSurface->dwCausticDiffuse = u < TABLE_SIZE ? Table[u] : 0;
		}

		pSurface++;

		}

		pSurface[-m_uWaterSize].dwCausticDiffuse = pSurface[-1].dwCausticDiffuse;
		}

		for(uX = 0; uX < m_uWaterSize; uX++)
		{
		m_pSurface[uX].dwCausticDiffuse = m_pSurface[uX + (m_uVertices - m_uWaterSize)].dwCausticDiffuse;
		}


		}
		*/

		return S_OK;
	}


};


class CRipple:public CWater
{
public:
	HRESULT Drop()
	{  // 产生涟漪
		#define WATER_INDEX(x, y) \
			((x) | ((y) << 6))

		#define WATER_INDEX_WRAP(x, y) \
			(((x) & (m_uWaterSize-1)) | (((y) & (m_uWaterSize-1)) << 6))

		UINT uX = rand();
		UINT uY = rand();

		m_pSurface[WATER_INDEX_WRAP(uX, uY)].fVelocity -= 0.25f*2;

		m_pSurface[WATER_INDEX_WRAP(uX - 1, uY)].fVelocity -= 0.125f*2;
		m_pSurface[WATER_INDEX_WRAP(uX + 1, uY)].fVelocity -= 0.125f*2;
		m_pSurface[WATER_INDEX_WRAP(uX, uY - 1)].fVelocity -= 0.125f*2;
		m_pSurface[WATER_INDEX_WRAP(uX, uY + 1)].fVelocity -= 0.125f*2;

		m_pSurface[WATER_INDEX_WRAP(uX - 1, uY - 1)].fVelocity -= 0.0625f*2;
		m_pSurface[WATER_INDEX_WRAP(uX + 1, uY + 1)].fVelocity -= 0.0625f*2;
		m_pSurface[WATER_INDEX_WRAP(uX + 1, uY - 1)].fVelocity -= 0.0625f*2;
		m_pSurface[WATER_INDEX_WRAP(uX - 1, uY + 1)].fVelocity -= 0.0625f*2;   

		return S_OK;
	}
	HRESULT Update()
	{

		UINT uXN, uX, uXP, uYN, uY0, uYP;

		// Compute desired height
		m_fAvgHeight = 0.0f;
		WATER_SURFACE *pSurface;
		pSurface = m_pSurface;

		uYN  = m_uVertices - m_uWaterSize;
		uY0  = 0;
		uYP  = m_uWaterSize;

		do
		{
			uXN  = m_uWaterSize - 1;
			uX   = 0;
			uXP  = 1;

			do
			{
				// Bowl
				float fDesire;

				if( pSurface->bIsborder == 1)
				{
					fDesire = 0.0f;
					pSurface->fHeight = 0.0f;
					pSurface->fVelocity = 0.0f;
				}
				else
				{

					fDesire = 
						(m_pSurface[uXN + uYN].fHeight + 
						m_pSurface[uXP + uYN].fHeight + 
						m_pSurface[uXN + uYP].fHeight + 
						m_pSurface[uXP + uYP].fHeight) * (1.0f / 12.0f)
						+
						(m_pSurface[uX  + uYN].fHeight +
						m_pSurface[uXN + uY0].fHeight +
						m_pSurface[uXP + uY0].fHeight +
						m_pSurface[uX  + uYP].fHeight) * (2.0f / 12.0f);
				}

				// Update velocity
				if(pSurface->fVelocity > 0.01f || pSurface->fVelocity  < -0.01f)
					pSurface->fVelocity *= 0.99f;     //涟漪纹衰减

				pSurface->fVelocity += 0.25f * (fDesire - pSurface->fHeight);
				m_fAvgHeight += pSurface->fHeight + pSurface->fVelocity;

				pSurface++;

				uXN = uX;
				uX  = uXP;
				uXP = (uXP + 1) & (m_uWaterSize-1);
			}
			while(uX);

			uYN = uY0;
			uY0 = uYP;
			uYP = (uYP + m_uWaterSize) & ((m_uWaterSize-1) << 6);
		}
		while(uY0);

		m_fAvgHeight /= (float) m_uVertices;

		D3DXVECTOR3 vec;
		D3DXVECTOR3 vecP, vecN;
		// Calculate surface normals
		pSurface = m_pSurface;

		uYN  = m_uVertices - m_uWaterSize;
		uY0  = 0;
		uYP  = m_uWaterSize;

		do
		{
			uXN  = m_uWaterSize - 1;
			uX   = 0;
			uXP  = 1;

			do
			{
				//高度值
				pSurface->vecPos.y = pSurface->fHeight = pSurface->fHeight + pSurface->fVelocity - m_fAvgHeight;

				float f;

				f = m_pSurface[uXN + uYN].fHeight - m_pSurface[uXP + uYP].fHeight; vecN.x = vecN.z = f;           
				f = m_pSurface[uX  + uYN].fHeight - m_pSurface[uX  + uYP].fHeight; vecN.z += f;
				f = m_pSurface[uXP + uYN].fHeight - m_pSurface[uXN + uYP].fHeight; vecN.x -= f; vecN.z += f;
				f = m_pSurface[uXN + uY0].fHeight - m_pSurface[uXP + uY0].fHeight; vecN.x += f;

				vecN.y = 1.0f;
				D3DXVec3Normalize(&vecN, &vecN);
				pSurface->vecNormal = vecN;
				pSurface++;

				pSurface->vecTex.x += 0.05f;
				pSurface->vecTex.y += 0.05f;


				uXN = uX;
				uX  = uXP;
				uXP = (uXP + 1) & (m_uWaterSize-1);
			}
			while(uX);
			uYN = uY0;
			uY0 = uYP;
			uYP = (uYP + m_uWaterSize) & ((m_uWaterSize-1) << 6);
		}
		while(uY0);



		return S_OK;
	}	

};
};
