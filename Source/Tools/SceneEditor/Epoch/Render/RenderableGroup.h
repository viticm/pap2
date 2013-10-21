// File: RenderableGroup.h
// Desc:

#ifndef RENDERABLEGROUP_H
#define RENDERABLEGROUP_H

#include"Renderable.h"

#include<vector>
using std::vector;

namespace Render
{

class DLLAPI RenderableGroup
{
private:
	vector<Renderable*> m_Renderables;

};

}  // namespace

#endif