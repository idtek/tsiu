#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "TCore_Types.h"

namespace TsiU
{
	class Renderer;

	class Texture
	{
	public:
		Texture();
		virtual ~Texture() {};

		/// This is the core loading function. It must be overwritten by your application.
        virtual void Load(const Char* fileName) = 0;

        virtual s16 GetWidth() const = 0;
        virtual s16 GetHeight() const = 0;

        /// Returns true if the texture has a valid internal buffer.
        /// The default behavior is to return false is both the width and height is 0, although your implementation
        /// might (or might not) require more precise decisions.
        //virtual bool IsValid() const;

	protected:
		Renderer *m_poRenderer;
	};
}

#endif