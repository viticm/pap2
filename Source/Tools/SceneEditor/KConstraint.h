/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KConstraint.h
* Coded by		: HJQ
* Date			: 2004-07-02
* Comment	    : Ô¼Êø¸¸Àà
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/
#pragma once

namespace Physics
{

inline float SafeInvNum(float num) 
{
  if (num > 0.0f)
    return (1.0f / num);
  else
    return 0.0f;
}

class KConstraint
{
public:
//	KConstraint(){};
	virtual ~KConstraint(){};
	virtual int PreApply(float TinyTime) = 0;
	virtual BOOL Apply(float TinyTime) = 0;
};

};