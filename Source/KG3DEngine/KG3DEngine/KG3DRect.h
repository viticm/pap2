////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRect.h
//  Version     : 1.0
//  Creator     : 
//  Create Date : 2008-8-14 10:51:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRECT_H_
#define _INCLUDE_KG3DRECT_H_

////////////////////////////////////////////////////////////////////////////////

typedef struct  FRECT
{
	float Bottom;
	float Height;
	float Left;
	float Width;
	FRECT()
	{
		Bottom = 0;
		Height = 0;
		Left = 0;
		Width =0;
	}
	FRECT(float L,float B,float W,float H)
	{
		Bottom  = B;
		Height  = H;
		Left    = L;
		Width   = W;
	}

	FRECT(D3DXVECTOR2 Center,float Size)
	{
		Left = Center.x - Size/2;
		Bottom  = Center.y - Size/2;
		Width  = Size;
		Height = Size;
	}
	void ToPosition(D3DXVECTOR2* A,D3DXVECTOR2* C) const
	{
		A->x = Left;
		A->y = Bottom;
		C->x = Left + Width;
		C->y = Bottom  + Height;
	}

	BOOL IsIntersect(const FRECT& NewRect)
	{
		D3DXVECTOR2 m_A,m_C;
		D3DXVECTOR2 N_A,N_C;

		ToPosition(&m_A,&m_C);
		NewRect.ToPosition(&N_A,&N_C);

		if ((N_A.x <= m_C.x)&&(N_C.x >= m_A.x)&&(N_A.y <= m_C.y)&&(N_C.y >= m_A.y))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL IsPositionInRect(D3DXVECTOR2 Pos)
	{
		if ((Pos.x >= Left)&&(Pos.x <= Left + Width)&&
			(Pos.y >= Bottom)&&(Pos.y <= Bottom  + Height))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	FRECT operator - (FRECT Rect)
	{
		FRECT K;
		K.Bottom  = Bottom - Rect.Bottom;
		K.Left = Left - Rect.Left;
		K.Height = Height;// - Rect.Height;
		K.Width  = Width;// - Rect.Width;
		return K;
	}
	FRECT operator + (FRECT Rect)
	{
		FRECT K;
		K.Bottom  = Bottom + Rect.Bottom;
		K.Left = Left + Rect.Left;
		K.Height = Height;// + Rect.Height;
		K.Width  = Width;// + Rect.Width;
		return K;
	}
	FRECT operator | (FRECT Rect)//合并一个比较大的区域
	{
		D3DXVECTOR2 A,C;
		D3DXVECTOR2 NA,NC;
		this->ToPosition(&A,&C);
		Rect.ToPosition(&NA,&NC);

		if(NA.x<A.x) A.x = NA.x;
		if(NA.y<A.y) A.y = NA.y;
		if(NC.x>C.x) C.x = NC.x;
		if(NC.y>C.y) C.y = NC.y;


		FRECT K;
		K.Bottom  = A.y;
		K.Left = A.x;
		K.Height = C.y - A.y;
		K.Width  = C.x - A.x;
		return K;
	}
	void BoolAdd(FRECT Rect)//合并一个比较大的区域
	{
		D3DXVECTOR2 A,C;
		D3DXVECTOR2 NA,NC;
		this->ToPosition(&A,&C);
		Rect.ToPosition(&NA,&NC);

		if(NA.x<A.x) A.x = NA.x;
		if(NA.y<A.y) A.y = NA.y;
		if(NC.x>C.x) C.x = NC.x;
		if(NC.y>C.y) C.y = NC.y;

		Bottom  = A.y;
		Left = A.x;
		Height = C.y - A.y;
		Width  = C.x - A.x;

	}

	D3DXVECTOR2 GetCenter()
	{
		float X = Left + Width*0.5f;
		float Z = Bottom  + Height*0.5f;

		return D3DXVECTOR2(X,Z);
	}

}*LPFRECT;
#endif //_INCLUDE_KG3DRECT_H_
