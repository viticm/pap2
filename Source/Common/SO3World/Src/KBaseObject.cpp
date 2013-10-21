#include "stdafx.h"
#include "Global.h"
#include "KBaseObject.h"
#include "KAttribute.h"
#include "KItem.h"

KBaseObject::KBaseObject()
{
    m_dwID = ERROR_ID;
}

KBaseObject::~KBaseObject()
{
    m_dwID = ERROR_ID;
}

