// KGImportSfxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGImportSfxDlg.h"

namespace {
    inline BOOL SetForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        dwBits |= 0x00000001 << dwIndex;

        return TRUE;
    }

    inline BOOL HasForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        if (dwBits & (0x00000001 << dwIndex))
            return TRUE;

        return FALSE;
    }

    inline BOOL DelForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        dwBits &= ~(0x00000001 << dwIndex);

        return TRUE;
    }
}

// KGImportSfxDlg dialog

IMPLEMENT_DYNAMIC(KGImportSfxDlg, CDialog)

KGImportSfxDlg::KGImportSfxDlg(CWnd* pParent, BYTE* pBuffer, IEKG3DSFX* pCurSfx)
	: CDialog(KGImportSfxDlg::IDD, pParent)
{
    m_pBuffer = pBuffer;
    m_pCurSfx = pCurSfx;
}

KGImportSfxDlg::~KGImportSfxDlg()
{
}

void KGImportSfxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGImportSfxDlg, CDialog)
    ON_COMMAND_RANGE(100, 200, OnCheckBoxUpdate)
    ON_BN_CLICKED(IDC_SCANL_IMPSFX, &KGImportSfxDlg::OnBnClickedScanlImpsfx)
    ON_BN_CLICKED(IDC_CUT_IMPSFX, &KGImportSfxDlg::OnBnClickedCutImpsfx)
    ON_BN_CLICKED(IDC_SCANL_CURSFX, &KGImportSfxDlg::OnBnClickedScanlCursfx)
END_MESSAGE_MAP()

BOOL KGImportSfxDlg::OnInitDialog()
{
    BOOL r = CDialog::OnInitDialog();

    GetDlgItem(ID_STATIC_PARTICLE_KIND)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_PARTICLE_KIND].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_PARTICLE_KIND].rect);
    m_aCheckBoxInfo[SFX_FEID_PARTICLE_KIND].num = 0;

    GetDlgItem(ID_STATIC_LAUNCHER)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_LAUNCHER].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_LAUNCHER].rect);
    m_aCheckBoxInfo[SFX_FEID_LAUNCHER].num = 0;
    
    GetDlgItem(ID_STATIC_FORCE_FIELD)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_FORCE_FIELD].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_FORCE_FIELD].rect);
    m_aCheckBoxInfo[SFX_FEID_FORCE_FIELD].num = 0;
    
    GetDlgItem(ID_STATIC_TEXTURE)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_TEXTURE].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_TEXTURE].rect);
    m_aCheckBoxInfo[SFX_FEID_TEXTURE].num = 0;

    GetDlgItem(ID_STATIC_BILLBOARD)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_BILLBOARD].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_BILLBOARD].rect);
    m_aCheckBoxInfo[SFX_FEID_BILLBOARD].num = 0;

    GetDlgItem(ID_STATIC_TRACK)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_TRACK].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_TRACK].rect);
    m_aCheckBoxInfo[SFX_FEID_TRACK].num = 0;

    GetDlgItem(ID_STATIC_MODEL2)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_MODEL].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_MODEL].rect);
    m_aCheckBoxInfo[SFX_FEID_MODEL].num = 0;

    GetDlgItem(ID_STATIC_METEOR)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_METEOR].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_METEOR].rect);
    m_aCheckBoxInfo[SFX_FEID_METEOR].num = 0;

    GetDlgItem(ID_STATIC_MAGIC_PARPET)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_MAGIC_PARPET].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_MAGIC_PARPET].rect);
    m_aCheckBoxInfo[SFX_FEID_MAGIC_PARPET].num = 0;

    GetDlgItem(ID_STATIC_CHANNEL)->GetWindowRect(&m_aCheckBoxInfo[SFX_FEID_CHANNEL].rect);
    ScreenToClient(&m_aCheckBoxInfo[SFX_FEID_CHANNEL].rect);
    m_aCheckBoxInfo[SFX_FEID_CHANNEL].num = 0;


    m_SfxImportInfo.clear();
    m_nCheckBoxNum = 0;

    DWORD dwCurLength = m_pCurSfx->GetFrameInterval() * m_pCurSfx->GetTotalFrameNum();
    DWORD dwImpLength = 0;

    SFX_FILE_HEADER* pHeader = reinterpret_cast<SFX_FILE_HEADER*>(m_pBuffer);
    ASSERT(pHeader->dwID == SFX_ID);
    ASSERT(LOWORD(pHeader->dwVersion) >= 2);

    DWORD dwSelfIndex[SFX_FEID_SIZE];
    for (DWORD i = 0; i < SFX_FEID_SIZE; ++i)
        dwSelfIndex[i] = 0;

    DWORD dwCurPos = sizeof(SFX_FILE_HEADER);
    for (DWORD i = 0; i < pHeader->dwElementNum; ++i, dwCurPos += sizeof(SFX_ELEMENT))
    {
        SFX_ELEMENT *pElem = reinterpret_cast<SFX_ELEMENT*>(&m_pBuffer[dwCurPos]);
        ASSERT(
            (pElem->dwElementID >= SFX_FEID_GENERAL_INFORMATION) &&
            (pElem->dwElementID < SFX_FEID_SIZE)
            );

        if (pElem->dwElementID == SFX_FEID_GENERAL_INFORMATION)
        {
            SFX_GENERAL_INFORMATION_BLOCK *pBlock = 
                (SFX_GENERAL_INFORMATION_BLOCK*)&m_pBuffer[pElem->dwElementOffset];
            dwImpLength = pBlock->dwFrameInterval * pBlock->dwTotalFrameNum;
            m_dwImpFrameNum = pBlock->dwTotalFrameNum;
            m_dwImpInterval = pBlock->dwFrameInterval;
            continue;
        }
        if (pElem->dwElementID == SFX_FEID_SOUND)
            continue;

        SfxEditImportElement element = {
            {pElem->dwElementID, pElem->dwElementOffset, dwSelfIndex[pElem->dwElementID]++}, true};
        m_SfxImportInfo.push_back(element);
    }

    for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
    {
            //&ReadGeneralInformationBlock,   // SFX_FEID_GENERAL_INFORMATION,
            //&ReadParticleKindBlock,         // SFX_FEID_PARTICLE_KIND,
            //&ReadLauncherBlock,             // SFX_FEID_LAUNCHER,
            //&ReadForceFieldBlock,           // SFX_FEID_FORCE_FIELD,
            //&ReadTextureBlock,              // SFX_FEID_TEXTURE,
            //&ReadBillboardBlock,            // SFX_FEID_BILLBOARD,
            //&ReadTrackBlock,                // SFX_FEID_TRACK,
            //&ReadModelBlock,                // SFX_FEID_MODEL,
            //&ReadMeteorBlock,               // SFX_FEID_METEOR,
            //&ReadMagicParpetBlock,          // SFX_FEID_MAGIC_PARPET,
            //&ReadChannelBlock,              // SFX_FEID_CHANNEL
            //&ReadSoundBlock,                // SFX_FEID_SOUND

        SfxEditImportElement& e = m_SfxImportInfo[i];


        CButton& bn = m_aCheckBoxs[m_nCheckBoxNum++];
        int index = m_aCheckBoxInfo[e.sie.dwType].num++;
        RECT BoundRect = m_aCheckBoxInfo[e.sie.dwType].rect;
        RECT rect = {
            BoundRect.left + 30,
            BoundRect.top + 30 * index + 15 + 2,
            BoundRect.right - 30,
            BoundRect.top + 30 * index + 15 + 30
        };
        CString str;
        str.Format("-%02d", index);
        str = GetCheckBoxName(e.sie.dwType) + str;
        bn.Create(str.GetBuffer(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, rect, this, (UINT)(i + 100));
        bn.SetFont(GetFont());
        str.ReleaseBuffer();
    }

    ((CButton*)GetDlgItem(IDC_SCANL_IMPSFX))->SetCheck(TRUE);
    if (m_pCurSfx->GetTotalFrameNum() == m_dwImpFrameNum && m_pCurSfx->GetFrameInterval() == m_dwImpInterval)
    {
        GetDlgItem(IDC_SCANL_IMPSFX)->EnableWindow(false);
        GetDlgItem(IDC_CUT_IMPSFX)->EnableWindow(false);
        GetDlgItem(IDC_SCANL_CURSFX)->EnableWindow(false);
    }
    CString strInfo;
    strInfo.Format("信息差异对比 : 当前sfx 总帧数( %d ) 帧间隔( %d ) 导入sfx 总帧数( %d ) 帧间隔( %d )",
        m_pCurSfx->GetTotalFrameNum(), m_pCurSfx->GetFrameInterval(), m_dwImpFrameNum, m_dwImpInterval
        );
    GetDlgItem(IDC_STATIC)->SetWindowText(strInfo);
    m_dwPrecept = SFX_IMPORT_SCANL_IMPSFX;

    UpdateUI();

    return r;
}

void KGImportSfxDlg::UpdateUI()
{
    for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
        ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(m_SfxImportInfo[i].enable);
}

LPCTSTR KGImportSfxDlg::GetCheckBoxName(DWORD dwType)
{
    LPCTSTR strNames[] = {
        TEXT("整体"),
        TEXT("粒子类型"),
        TEXT("发射器"),
        TEXT("力场"),
        TEXT("贴图"),
        TEXT("公告板"),
        TEXT("轨道"),
        TEXT("模型"),
        TEXT("拖尾"),
        TEXT("AOE"),
        TEXT("通道"),
        TEXT("声音")
    };
    return strNames[dwType];
}

void KGImportSfxDlg::OnCheckBoxUpdate(UINT CmdId)
{
    DWORD index = CmdId - 100;
    ASSERT(index >= 0 && index < m_SfxImportInfo.size());
    
    DWORD type = m_SfxImportInfo[index].sie.dwType;
	m_SfxImportInfo[index].enable = ((CButton*)GetDlgItem(CmdId))->GetCheck() ? true : false;

    switch (type)
    {
    case SFX_FEID_PARTICLE_KIND :
        {
             if (!m_SfxImportInfo[index].enable) 
             {
                 for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
                     if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_LAUNCHER)
                     {
                         SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                             (SFX_PARTICLE_LAUNCHER_BLOCK*)&m_pBuffer[m_SfxImportInfo[i].sie.dwOffset];
                         if (pBlock->dwParticleKindIndex == m_SfxImportInfo[index].sie.dwSelfIndex) 
                         {
                            ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(false);
                            OnCheckBoxUpdate((UINT)(i + 100));
                         }
                     }
             }
             else
             {
                 SFX_PARTICLE_KIND_BLOCK *pBlock = 
                     (SFX_PARTICLE_KIND_BLOCK*)&m_pBuffer[m_SfxImportInfo[index].sie.dwOffset];
                 for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
                     if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_TEXTURE)
                     {
                         if (pBlock->dwTextureIndex == m_SfxImportInfo[i].sie.dwSelfIndex) 
                         {
                             ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(true);
                             OnCheckBoxUpdate((UINT)(i + 100));
                         }
                     }
             }
        }
        break;
    case SFX_FEID_LAUNCHER :
        {
            if (m_SfxImportInfo[index].enable) 
            {
                SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                    (SFX_PARTICLE_LAUNCHER_BLOCK*)&m_pBuffer[m_SfxImportInfo[index].sie.dwOffset];
                for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
                {
                    if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_PARTICLE_KIND &&
                        pBlock->dwParticleKindIndex == m_SfxImportInfo[i].sie.dwSelfIndex)
                    {
                        ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(true);
                        OnCheckBoxUpdate((UINT)(i + 100));
                    }
                    if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_FORCE_FIELD)
                    {
                        if (HasForceBit(pBlock->dwEffForceBit, m_SfxImportInfo[i].sie.dwSelfIndex)) 
                        {
                            ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(true);
                            OnCheckBoxUpdate((UINT)(i + 100));
                        }
                    }
                }
            }
        }
        break;
    case SFX_FEID_TEXTURE :
        {
            if (!m_SfxImportInfo[index].enable) 
            {
                for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
                    if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_PARTICLE_KIND)
                    {
                        SFX_PARTICLE_KIND_BLOCK *pBlock = 
                            (SFX_PARTICLE_KIND_BLOCK*)&m_pBuffer[m_SfxImportInfo[i].sie.dwOffset];
                        if (pBlock->dwTextureIndex == m_SfxImportInfo[index].sie.dwSelfIndex) 
                        {
                            ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(false);
                            OnCheckBoxUpdate((UINT)(i + 100));
                        }
                    }
            }
        }
        break;
    case SFX_FEID_FORCE_FIELD :
        {
            if (!m_SfxImportInfo[index].enable) 
            {
                for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
                    if (m_SfxImportInfo[i].sie.dwType == SFX_FEID_LAUNCHER)
                    {
                        SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                            (SFX_PARTICLE_LAUNCHER_BLOCK*)&m_pBuffer[m_SfxImportInfo[i].sie.dwOffset];
                        if (HasForceBit(pBlock->dwEffForceBit, m_SfxImportInfo[index].sie.dwSelfIndex))
                        {
                            ((CButton*)GetDlgItem((int)(i + 100)))->SetCheck(false);
                            OnCheckBoxUpdate((UINT)(i + 100));
                        }
                    }
            }
        }
        break;
    default :
        break;
    }

    UpdateUI();

}

SfxImportInfo KGImportSfxDlg::GetImportInfo(DWORD& dwImpFrameNum, DWORD& dwImpInterval, DWORD& dwPrecept)
{
    dwImpFrameNum = m_dwImpFrameNum;
    dwImpInterval = m_dwImpInterval;
    dwPrecept = m_dwPrecept;


    for (int i = (int)m_SfxImportInfo.size() - 1; i >= 0; --i)
    {
        SfxEditImportElement e = m_SfxImportInfo[i];
        DWORD type = e.sie.dwType;
        if (type == SFX_FEID_PARTICLE_KIND && !e.enable)
        {
            int index = e.sie.dwSelfIndex;
             for (size_t t = 0; t < m_SfxImportInfo.size(); ++t)
             {
                 SfxEditImportElement e2 = m_SfxImportInfo[t];
                 DWORD type2 = e2.sie.dwType;
                 if (type2 == SFX_FEID_LAUNCHER)
                 {
                     SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                         (SFX_PARTICLE_LAUNCHER_BLOCK*)&m_pBuffer[m_SfxImportInfo[t].sie.dwOffset];
                     //ASSERT(pBlock->dwParticleKindIndex != index);
                     if (pBlock->dwParticleKindIndex > (DWORD)index)
                         pBlock->dwParticleKindIndex--;
                 }
             }
        }

        if (type == SFX_FEID_TEXTURE && !e.enable)
        {
            int index = e.sie.dwSelfIndex;
            for (size_t t = 0; t < m_SfxImportInfo.size(); ++t)
            {
                SfxEditImportElement e2 = m_SfxImportInfo[t];
                DWORD type2 = e2.sie.dwType;
                if (type2 == SFX_FEID_PARTICLE_KIND)
                {
                    SFX_PARTICLE_KIND_BLOCK *pBlock = 
                        (SFX_PARTICLE_KIND_BLOCK*)&m_pBuffer[m_SfxImportInfo[t].sie.dwOffset];
                    //ASSERT(pBlock->dwTextureIndex != index);
                    if (pBlock->dwTextureIndex > (DWORD)index)
                        pBlock->dwTextureIndex--;
                }
            }
        }

        if (type == SFX_FEID_FORCE_FIELD && !e.enable)
        {
            int index = e.sie.dwSelfIndex;
            for (size_t t = 0; t < m_SfxImportInfo.size(); ++t)
            {
                SfxEditImportElement e2 = m_SfxImportInfo[t];
                DWORD type2 = e2.sie.dwType;
                if (type2 == SFX_FEID_LAUNCHER)
                {
                    SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                        (SFX_PARTICLE_LAUNCHER_BLOCK*)&m_pBuffer[m_SfxImportInfo[t].sie.dwOffset];

                    for (int i = index + 1; i < 32; ++i)
                    {
                        if (HasForceBit(pBlock->dwEffForceBit, i))
                        {
                            DelForceBit(pBlock->dwEffForceBit, i);
                            SetForceBit(pBlock->dwEffForceBit, i - 1);
                        }
                    }
                }
            }
        }
    } 

    SfxImportInfo info;
    for (size_t i = 0; i < m_SfxImportInfo.size(); ++i)
        if (m_SfxImportInfo[i].enable)
            info.push_back(m_SfxImportInfo[i].sie);
    return info;
}

void KGImportSfxDlg::OnBnClickedScanlImpsfx()
{
    if (((CButton*)GetDlgItem(IDC_SCANL_IMPSFX))->GetCheck())
        m_dwPrecept = SFX_IMPORT_SCANL_IMPSFX;
}

void KGImportSfxDlg::OnBnClickedCutImpsfx()
{
    if (((CButton*)GetDlgItem(IDC_CUT_IMPSFX))->GetCheck())
        m_dwPrecept = SFX_IMPORT_CUT_IMPSFX;
}

void KGImportSfxDlg::OnBnClickedScanlCursfx()
{
    if (((CButton*)GetDlgItem(IDC_SCANL_CURSFX))->GetCheck())
        m_dwPrecept = SFX_IMPORT_SCANL_CURSFX;
}
