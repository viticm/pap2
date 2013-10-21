// File: StringVector.h
// Desc:

#ifndef STRINGVECTOR_H
#define STRINGVECTOR_H

#include"String.h"
#include<vector>
using std::vector;

namespace Common
{

class DLLAPI StringVector: public vector<String>
{
};

}  // namespace

#endif