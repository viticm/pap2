#include "stdafx.h"
#include <assert.h>
#include "./FontAdjust.h"

KFontAdjust g_FontAjust;

KFontAdjust::KFontAdjust()
{
	for (int i = 0; i < 256; ++i)
	{
		m_uch[i] = i;
	}

	m_uFontSize = 16;
	m_uDpi = 96;
	m_szFontFile[0] = '\0';
	m_fValue1 = 1.0f;
	m_fValue2 = 1.0f;
	m_nSharp = false;

	m_pFont = NULL;
}

KFontAdjust::~KFontAdjust()
{
	Exit();
}

void KFontAdjust::Init(LPCTSTR pcszFontFile, unsigned int uFontSize, unsigned int uDpi)
{	
	Exit();
	assert(!m_pFont);

	if (pcszFontFile && uFontSize && uDpi)
	{
		m_pFont = new Font;
		if (m_pFont)
		{
			if (m_pFont->Initialize(pcszFontFile, uFontSize, 0, uDpi))
			{
				m_uFontSize = uFontSize;
				m_uDpi = uDpi;
				strcpy(m_szFontFile, pcszFontFile);
				Load();
			}
			else
			{
				Exit();
			}
		}
	}
}

void KFontAdjust::Exit()
{
	if (m_pFont)
	{
		m_pFont->CleanUp();
		delete m_pFont;
		m_pFont = NULL;
	}
}

void KFontAdjust::ReLoad()
{
	Exit();
	assert(!m_pFont);

	if (m_szFontFile[0] && m_uFontSize && m_uDpi)
	{
		m_pFont = new Font;
		if (m_pFont)
		{
			if (!m_pFont->Initialize(m_szFontFile, m_uFontSize, 0, m_uDpi))
			{
				Exit();
			}
		}
	}
}

Font* KFontAdjust::GetFont()
{
	return m_pFont;
}
void KFontAdjust::AddControl1()
{
	m_fValue1 += 0.01f;
	Format();
}

void KFontAdjust::DecControl1()
{
	m_fValue1 -= 0.01f;
	if (m_fValue1 < 0.0f)
		m_fValue1 = 0.0f;
	Format();
}
void KFontAdjust::SetControl1(float fValue)
{
	if (fValue < 0.0f)
		fValue = 0.0f;
	m_fValue1 = fValue;
	Format();
}


void KFontAdjust::AddControl2()
{
	m_fValue2 += 0.01f;
	Format();
}

void KFontAdjust::DecControl2()
{
	m_fValue2 -= 0.01f;
	if (m_fValue2 < 0.0f)
		m_fValue2 = 0.0f;
	Format();
}

void KFontAdjust::SetControl2(float fValue)
{
	if (fValue < 0.0f)
		fValue = 0.0f;
	m_fValue2 = fValue;
	Format();
}


void KFontAdjust::AddDpi()
{
	if (m_uDpi < 200)
		m_uDpi += 1;
	Format();
}

void KFontAdjust::DecDpi()
{
	if (m_uDpi > 10)
		m_uDpi -= 1;
	Format();
}

void KFontAdjust::SetDpi(unsigned int uDpi)
{
	if (uDpi >= 10 && m_uDpi <= 200)
	{
		m_uDpi = uDpi;
		Format();
	}
}

unsigned int KFontAdjust::GetDpi()
{
	return m_uDpi;
}


void KFontAdjust::AddFontSize()
{
	if (m_uFontSize <100)
		m_uFontSize += 1;
	Load();
	Format();
}

void KFontAdjust::DecFontSize()
{
	if (m_uFontSize > 2)
		m_uFontSize -= 1;
	Load();
	Format();
}

void KFontAdjust::SetFontSize(unsigned int uSize)
{
	if (m_uFontSize >= 2 && m_uFontSize <= 100)
	{
		m_uFontSize = uSize;
		Load();
		Format();
	}
}

unsigned int KFontAdjust::GetFontSize()
{
	return m_uFontSize;
}

void KFontAdjust::Sharp()
{
	m_nSharp = true;
	Format();
}

void KFontAdjust::NoSharp()
{
	m_nSharp = false;
	Format();
}

void KFontAdjust::Save()
{
	char szPath[MAX_PATH];
	szPath[0] = '\0';

	strcpy(szPath, m_szFontFile);
	char *pszDot = strrchr(szPath, '.');
	if (pszDot)
	{
		sprintf(pszDot, "%d.prf", m_uFontSize);

		if (m_nSharp)
		{
			FILE *fp = fopen(szPath, "wb");
			if (fp)
			{
				fwrite(m_uch, sizeof(m_uch), 1, fp);
				fwrite(&m_fValue1, sizeof(m_fValue1), 1, fp);
				fwrite(&m_fValue2, sizeof(m_fValue2), 1, fp);
				fclose(fp);
				fp = NULL;
			}
		}
		else
		{
			::DeleteFile(szPath);
		}
	}
}

void KFontAdjust::SaveText()
{
	char szPath[MAX_PATH];
	szPath[0] = '\0';

	strcpy(szPath, m_szFontFile);
	char *pszDot = strrchr(szPath, '.');
	if (pszDot)
	{
		sprintf(pszDot, "%d.txt", m_uFontSize);

		if (m_nSharp)
		{
			FILE *fp = fopen(szPath, "w");
			if (fp)
			{
				for (int i = 0; i < 16; ++i)
				{
					for (int j = 0; j < 16; ++j)
					{
						fprintf(fp, "\t0x%.2x,", m_uch[i * 16 + j]);
					}
					fprintf(fp, "\n");
				}
				fclose(fp);
				fp = NULL;
			}
		}
		else
		{
			::DeleteFile(szPath);
		}
	}
}

void KFontAdjust::Load()
{
	char szPath[MAX_PATH];
	szPath[0] = '\0';

	strcpy(szPath, m_szFontFile);
	char *pszDot = strrchr(szPath, '.');
	if (pszDot)
	{
		sprintf(pszDot, "%d.prf", m_uFontSize);

		FILE *fp = fopen(szPath, "rb");
		if (fp)
		{
			size_t uCount = fread(m_uch, sizeof(m_uch), 1, fp);
			size_t uCount1 = fread(&m_fValue1, sizeof(m_fValue1), 1, fp);
			size_t uCount2 = fread(&m_fValue2, sizeof(m_fValue2), 1, fp);
			fclose(fp);
			fp = NULL;
			if (uCount != 1 || uCount1 != 1 || uCount2 != 1)
				NoSharp();
			else
				Sharp();
		}
		else
			NoSharp();
	}
	else
		NoSharp();
}

const char * KFontAdjust::GetFontFile()
{
	return m_szFontFile;
}

void KFontAdjust::Format()
{
	if (m_nSharp)
	{

		std::vector<D3DXVECTOR3> vec;

		D3DXVECTOR3 v1 = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 v2 = D3DXVECTOR3(1.f, 1.f, 0.f);
		D3DXVECTOR3 t1 = D3DXVECTOR3(m_fValue1, 0.f, 0.f);
		D3DXVECTOR3 t2 = D3DXVECTOR3(m_fValue2, 0.f, 0.f);

		for (int i = 0; i < 256; ++i)
		{
			D3DXVECTOR3 v;
			D3DXVec3Hermite(&v, &v1, &t1, &v2, &t2, i / 255.f);
			vec.push_back(v);
		}

		std::vector<D3DXVECTOR3>::iterator it = vec.begin();
		for (int i = 0; i < 256; ++i)
		{
			float x = (float)i;
			while(it != vec.end())
			{
				std::vector<D3DXVECTOR3>::iterator itS = it + 1;
				float x1 = it->x * 255.0f;
				float y1 = it->y * 255.0f;
				float x2 = 255.0f;
				float y2 = 255.0f;
				if (itS != vec.end())
				{
					x2 = itS->x * 255.0f;
					y2 = itS->y * 255.0f;
				}

				if (x >= (it->x * 255.0f) && x <= x2)
				{
					float fValue = ((y2 - y1) * x + x2 * y1 - x1 * y2) / (x2 - x1);
					int nValue = (int)(fValue) + ((fValue - (float)((int)(fValue)) >= 0.5) ? 1 : 0);
					if (nValue < 0)
						nValue = 0;
					if (nValue > 255)
						nValue = 255;
					m_uch[i] = nValue;
					break;
				}
				++it;
			}
		}
	}
	else
	{
		for (int i = 0; i < 256; ++i)
		{
			m_uch[i] = i;
		}
	}

	ReLoad();
}
