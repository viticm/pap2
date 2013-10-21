// CS3LogicForm.h  : Declaration of ActiveX Control wrapper class(es) created by Microsoft Visual C++

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CS3LogicForm

class CS3LogicForm : public CWnd
{
protected:
	DECLARE_DYNCREATE(CS3LogicForm)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x30019763, 0xD673, 0x4798, { 0xA8, 0x81, 0x57, 0x44, 0x73, 0x29, 0x44, 0xE7 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID, 
		CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
		UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
			pPersist, bStorage, bstrLicKey); 
	}

	// Attributes
public:
	enum
	{
		afbNone = 0,
		afbSingle = 1,
		afbSunken = 2,
		afbRaised = 3
	}TxActiveFormBorderStyle;
	enum
	{
		poNone = 0,
		poProportional = 1,
		poPrintToFit = 2
	}TxPrintScale;
	enum
	{
		mbLeft = 0,
		mbRight = 1,
		mbMiddle = 2
	}TxMouseButton;


	// Operations
public:

	// IS3LogicForm

	// Functions
	//

	BOOL get_Visible()
	{
		BOOL result;
		InvokeHelper(0xc9, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Visible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xc9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_AutoScroll()
	{
		BOOL result;
		InvokeHelper(0xca, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_AutoScroll(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xca, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_AutoSize()
	{
		BOOL result;
		InvokeHelper(0xcb, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_AutoSize(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xcb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_AxBorderStyle()
	{
		long result;
		InvokeHelper(0xcc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_AxBorderStyle(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xcc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Caption()
	{
		CString result;
		InvokeHelper(DISPID_CAPTION, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Caption(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(DISPID_CAPTION, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	unsigned long get_Color()
	{
		unsigned long result;
		InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYGET, VT_UI4, (void*)&result, NULL);
		return result;
	}
	void put_Color(unsigned long newValue)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(DISPID_BACKCOLOR, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Font()
	{
		LPDISPATCH result;
		InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void put_Font(LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_DISPATCH ;
		InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void putref_Font(LPDISPATCH * newValue)
	{
		static BYTE parms[] = VTS_PDISPATCH ;
		InvokeHelper(DISPID_FONT, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_KeyPreview()
	{
		BOOL result;
		InvokeHelper(0xcd, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_KeyPreview(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xcd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_PixelsPerInch()
	{
		long result;
		InvokeHelper(0xce, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_PixelsPerInch(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xce, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_PrintScale()
	{
		long result;
		InvokeHelper(0xcf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_PrintScale(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xcf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Scaled()
	{
		BOOL result;
		InvokeHelper(0xd0, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Scaled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xd0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Active()
	{
		BOOL result;
		InvokeHelper(0xd1, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	BOOL get_DropTarget()
	{
		BOOL result;
		InvokeHelper(0xd2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DropTarget(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xd2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_HelpFile()
	{
		CString result;
		InvokeHelper(0xd3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_HelpFile(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xd3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_ScreenSnap()
	{
		BOOL result;
		InvokeHelper(0xd4, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_ScreenSnap(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xd4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_SnapBuffer()
	{
		long result;
		InvokeHelper(0xd5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_SnapBuffer(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_DoubleBuffered()
	{
		BOOL result;
		InvokeHelper(0xd6, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DoubleBuffered(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xd6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_AlignDisabled()
	{
		BOOL result;
		InvokeHelper(0xd7, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	long get_VisibleDockClientCount()
	{
		long result;
		InvokeHelper(0xd8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL get_Enabled()
	{
		BOOL result;
		InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Enabled(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(DISPID_ENABLED, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void AddScriptInfo(short Index, LPCTSTR Name, LPCTSTR FileName)
	{
		static BYTE parms[] = VTS_I2 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0xd9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Index, Name, FileName);
	}
	void DelScriptInfo(short Index)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xda, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Index);
	}
	CString get_MapFileName()
	{
		CString result;
		InvokeHelper(0xdb, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_MapFileName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xdb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void Save()
	{
		InvokeHelper(0xdc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Load()
	{
		InvokeHelper(0xdd, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long get_Count()
	{
		long result;
		InvokeHelper(0xde, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void GetItem(long Index, BSTR * ItemName, long * LogicInfo, BSTR * ScriptFileName)
	{
		static BYTE parms[] = VTS_I4 VTS_PBSTR VTS_PI4 VTS_PBSTR ;
		InvokeHelper(0xdf, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Index, ItemName, LogicInfo, ScriptFileName);
	}

	// Properties
	//



};