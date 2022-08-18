#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <string>
#include "GLapi_GLerror.h"

namespace h3dgl {
	class Texture
	{
	protected:
		GLuint texture = 0;
		int m_width = 0;
		int m_height = 0;
		int m_bitsPerPixel = 0;
		bool m_loaded = false;
		std::string m_path = "";
	public:
		void load(const std::string& path);

		bool isLoaded();

		inline std::string getPath() { return m_path; }

		virtual void destroy();

		virtual void bind(unsigned int slot) const;

		virtual void unbind(unsigned int slot) const;

		int getWidth() const;

		int getHeight() const;

		GLenum getId();

	};

	class Cubemap: public Texture {
	public:

		void load(const std::string& path);

		void load(
			const std::string& pX,
			const std::string& pY,
			const std::string& pZ,
			const std::string& nX,
			const std::string& nY,
			const std::string& nZ
		);
		void bind(unsigned int slot) const override;
		void unbind(unsigned int slot) const override;
	};

}
