#ifndef __TUI_TEXTURE_H__
#define __TUI_TEXTURE_H__

#include "TRender_Renderer.h"
namespace TsiU
{
	class UITexture
	{
	public:
		static void SetRenderer( Renderer * render) { mpRenderer = render;}

		UITexture() {};
		virtual ~UITexture() {};

		/// This is the core loading function. It must be overwritten by your application.
        virtual void Load(const char* fileName) = 0;

        virtual short GetWidth() const = 0;
        virtual short GetHeight() const = 0;

        /// Returns true if the texture has a valid internal buffer.
        /// The default behavior is to return false is both the width and height is 0, although your implementation
        /// might (or might not) require more precise decisions.
        //virtual bool IsValid() const;


	protected:
		static Renderer * mpRenderer;

	};
}

#endif