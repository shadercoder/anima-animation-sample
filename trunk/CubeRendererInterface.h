#pragma once
#include "DisplayList.h"
class RenderContext;

class CubeRendererInterface : public DisplayList::Node 
{
public:
	virtual void AddCubes( int count ) = 0;
	virtual void RemoveCubes( int count ) = 0;
	virtual int GetNumCubes() = 0;

	virtual void SetScale( float scale ) = 0;
	virtual float GetScale() = 0;

	virtual void SetRotation( float rotation ) = 0;
//	virtual float GetRotation() = 0;

	virtual void SetDynamicUpdate( bool doDynamicUpdate ) = 0;

	virtual void  Update( float dt ) = 0;

	virtual void ReleaseResources( RenderContext* context ) = 0;
	virtual void AcquireResources( RenderContext* context ) = 0;

};

