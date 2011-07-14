#include "TRender_Texture.h"
#include "TEngine_Private.h"
#include "TEngine_RenderModule.h"
#include "TRender_Renderer.h"

namespace TsiU
{
	Texture::Texture()
	{
		m_poRenderer = poGetEngine()->GetRenderMod()->GetRenderer();
	}
}