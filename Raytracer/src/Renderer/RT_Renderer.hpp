#pragma once
#include "../Core.hpp"
#include "RT_Object.hpp"
#include "../GLAPI/GLapi.hpp"
#include "RT_Camera.hpp"

RT_START

// The realtime renderer
namespace renderer {

	struct Viewport
	{
		glm::vec2 pos;
		glm::vec2 size;
	};

	uint32_t totalFrames = 0;
	unsigned int drawCalls = 0;
	double lastFrameMS = 0.0;
	float waitBetweenFramesMS = 2;
	bool active = true;
	bool renderlights = true;
	float lightScale = 0.02;

	glm::vec4 clearColor = { 0, 0, 0, 1 };

	bool renderCubemap = false;
	float cubemapMip = 0.0f;
	bool prefilter = false;

	struct RendererProperties
	{
		Viewport viewport = {};
		float gamma = 2.0f;
		float exposure = 3.2f;
		float ambient = 0.001f;

		glm::vec3 filter = {1,1,1 };
		float saturation = 1.0f;

		bool iblSpecular = false;
		bool iblDiffuseIradiance = false;

		bool toneMapping = true;
		bool gammaCorrection = true;
	};

	RendererProperties properties;

	GL_ Framebuffer gBuffer;
	GL_ Shader gShader;

	GL_ Shader lShader;
	GL_ Framebuffer postBuffer;

	GL_ Shader pShader;

	GL_ Shader cubemapShader;

	GL_ Texture lightTexture;

	RT_ Camera c;
	GL_ Mesh cube, quad;

	GL_ EquirectangularMap cubemap, prefilteredCubemap, iradianceMap;

	struct MeshProperties
	{
		std::string name;
	};

	std::vector<GL_ Mesh> meshes;
	std::vector<MeshProperties> meshProperties;
	std::vector<RT_ Object> objects;
	std::vector<RT_ PointLight> lights;

	GL_ ComputeShader compute;

	GLuint brdfTexture;

	void renderBRDF() {
		GLCALL(glGenTextures(1, &brdfTexture));

		h3dgl::Shader brdfShader;
		brdfShader.create("src/shader/Brdf.shader");


		// pre-allocate enough memory for the LUT texture.
		GLCALL(glBindTexture(GL_TEXTURE_2D, brdfTexture));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0));
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		unsigned int captureFBO, captureRBO;
		GLCALL(glGenFramebuffers(1, &captureFBO));
		GLCALL(glGenRenderbuffers(1, &captureRBO));

		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
		GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512));
		GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTexture, 0));
		GLCALL(glViewport(0, 0, 512, 512));
		brdfShader.use();
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCALL(glDisable(GL_CULL_FACE));
		GLCALL(glDisable(GL_DEPTH_TEST));

		quad.bind();
		GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		quad.unbind();

		brdfShader.unuse();
		brdfShader.destroy();
		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCALL(glDeleteFramebuffers(1, &captureFBO));
		GLCALL(glDeleteRenderbuffers(1, &captureRBO));
		rt_info("Renderer", "Calculated Bidirectional Reflectance Distribution Function");
	}

	void init(float rwidth, float rheight)
	{

		properties.viewport = { {0,0}, {rwidth, rheight} };

		// Cube & Quad

		GL_ Vertex cubeVertex[] = H3D_CUBE_VERTICES(1, 1, 1);
		uint32_t cubeIndex[] = H3D_CUBE_INDICES;

		const h3dgl::Vertex quadVertices[] = {
            h3dgl::Vertex{-1,-1,0, 0,0, 0,0,1},
            h3dgl::Vertex{-1, 1,0, 0,1, 0,0,1},
            h3dgl::Vertex{ 1,-1,0, 1,0, 0,0,1},
            h3dgl::Vertex{ 1, 1,0, 1,1, 0,0,1}
        };

		const uint32_t quadIndices[] = { 2,1,0, 2,3,1 }; // Counter clockwise

		cube.load(cubeVertex, 8, cubeIndex, 36);
		quad.load(quadVertices, 4, quadIndices, 6);

		lightTexture.load("assets/images/light.png");

		

		// Object
		meshes.push_back(GL_ Mesh());
		meshes.push_back(GL_ Mesh());
		objects.push_back(RT_ Object());
		GL_ Vertex v[] = H3D_NORMAL_CUBE_VERTICES(1, 1, 1);
		meshes[0].loadFromFile("assets/model/hirsch.obj");
		meshes[1].loadFromFile("assets/model/sphere.obj");
		objects.at(0).create("Cube", &meshes[0], { {0.1,1,0.4}, 0.2f, 0.0f });
		objects.at(0).getPosition().z = -6;
		objects.at(0).updateTransform();
		objects.push_back(RT_ Object());
		objects.at(1).create("Cube2", &meshes[0], { {1,0.2,0.2}, 0.1f, 1.0f });
		objects.at(1).getPosition() = {2, -1, -5};
		objects.at(1).updateTransform();

		objects.push_back(RT_ Object());
		objects.at(2).create("Sphere", &meshes[1], { {1,0.3,0.1}, 0.4f, 0.0f });
		objects.at(2).getPosition() = { -2, 1, -3 };
		objects.at(2).updateTransform();

		lights.push_back(PointLight{ {2,2,2}, {1,1,1}, 1.0f });
		lights.push_back(PointLight{ {-2,2,-2}, {0.2,1,0.2}, 2.0f });

		// Camera
		c.create(90.0, rwidth, rheight, 0.1, 100.0);
		rt_info("Renderer", "Created perspective camera (FOV: ", 90.0f, " Degree)");


		gShader.create("src/shader/GeometryPass.shader", true);
		lShader.create("src/shader/LightingPass.shader", false);
		pShader.create("src/shader/PostProcessing.shader", false);
		cubemapShader.create("src/shader/Cubemap.shader", false);
		rt_info("Renderer", "Loaded shaders (4 Shaders)");

		GLenum postBufferAttachment = GL_COLOR_ATTACHMENT0;
		postBuffer.load(rwidth, rheight, 1, &postBufferAttachment, false);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		gBuffer.load(rwidth, rheight, 4, attachments, true);
		rt_info("Renderer", "Allocated framebuffers (2 Buffers, 5 Attachments + Depth)");

		compute.load("src/shader/Compute.shader");

		renderBRDF();

		rt_info("Renderer", "Initialized realtime renderer");
	}

	void render()
	{
		if (!active) return;
		drawCalls = 0;

		glViewport(properties.viewport.pos.x, properties.viewport.pos.y, properties.viewport.size.x, properties.viewport.size.y);



		gBuffer.bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		c.update();



		gShader.use();
		int i = 0;

		
		gShader.setMatrix4f("u_viewProj", c.viewProjection());
		gShader.setVector3f("camPos", c.position());

		GLCALL(glDisable(GL_CULL_FACE));
		GLCALL(glEnable(GL_DEPTH_TEST));
		if (renderlights)
		{
			for (int i = 0; i < lights.size(); i++)
			{
				gShader.setUniform1i("useDeffuseMap", true);
				lightTexture.bind(0);
				gShader.setUniform1i("useNormalMap", false);
				gShader.setUniform1i("useMetallicMap", false);
				gShader.setUniform1i("useRoughnessMap", false);
				gShader.setUniform1i("useAmbientOcclusionMap", false);
				quad.bind();
				glm::vec3 lclr = lights.at(i).color;
				glm::vec3 lpos = lights.at(i).position;
				gShader.setUniform4f("albedo", lclr.r, lclr.g, lclr.b, 1.0f);
				glm::mat4 m = glm::mat4(1.0f);
				m = glm::translate(m, { lpos.x, lpos.y, lpos.z });
				float d = glm::length(lpos - c.position());
				m = glm::scale(m, d * glm::vec3(lightScale));
				
				m = glm::rotate(m, ((float)RT_PI/2.0f) - c.rotation().y, {0,1,0});
				m = glm::rotate(m, -((float)RT_PI / 2.0f) + c.rotation().x, { 1,0,0 });
				gShader.setMatrix4f("u_model", m);

				gShader.setUniform1i("lightBulb", true);

				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
				GLCALL(glDrawElements(GL_TRIANGLES, quad.indexCount(), GL_UNSIGNED_INT, NULL));
				drawCalls++;
				lightTexture.unbind(0);
				quad.unbind();
			}
		}
		gShader.setUniform1i("lightBulb", false);
		for (i = 0; i < objects.size(); i++)
		{
			GLCALL(glDisable(GL_CULL_FACE));
			GLCALL(glDisable(GL_DEPTH_TEST));
			RT_ Object* obj = &objects.at(i);
			if (!obj->visible())
				continue;

			gShader.setMatrix4f("u_model", obj->getTransform());
			gShader.setVector4f("albedo", glm::vec4(obj->material._albedo, 1.0f));
			gShader.setUniform1f("u_roughness", obj->material._roughness);
			gShader.setUniform1f("u_metallic", obj->material._metallic);

			gShader.setUniform1i("useDeffuseMap", obj->usesAlbedoMap());
			gShader.setUniform1i("useNormalMap", obj->usesNormalMap());
			gShader.setUniform1i("useMetallicMap", obj->usesMetallicMap());
			gShader.setUniform1i("useRoughnessMap", obj->usesRoughnessMap());
			gShader.setUniform1i("useAmbientOcclusionMap", obj->usesAmbientOcclusionMap());
			gShader.setUniform1i("deffuseMap", 0);
			gShader.setUniform1i("normalMap", 1);
			gShader.setUniform1i("roughnessMap", 2);
			gShader.setUniform1i("metallicMap", 3);
			gShader.setUniform1i("ambientOcclusionMap", 4);
			gShader.setUniform1i("smoothShaded", obj->smooth());

			obj->bind();
			if (obj->Wireframe()) {
				GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			}
			if (obj->cullMode()) { GLCALL(glCullFace(GL_FRONT)); }
			else {
				GLCALL(glCullFace(GL_BACK));
			}
			if (obj->backfaceCulling()) {
				GLCALL(glEnable(GL_CULL_FACE));
			}
			if (obj->depthTesting()) {
				GLCALL(glEnable(GL_DEPTH_TEST));
			}
			GLCALL(glDrawElements(GL_TRIANGLES, obj->IndexCount(), GL_UNSIGNED_INT, NULL));
			drawCalls++;
			if (obj->Wireframe()) { GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); }
			obj->unbind();

		}


		gShader.unuse();
		gBuffer.unbind();


		postBuffer.bind();
		GLCALL(glDisable(GL_CULL_FACE));
		GLCALL(glDisable(GL_DEPTH_TEST));
		GLCALL(glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));
		// Cubemap

		if (renderCubemap && cubemap.isLoaded() && prefilteredCubemap.isLoaded())
		{
			cubemapShader.use();

			cubemapShader.setMatrix4f("u_proj", renderer::c.projection());
			cubemapShader.setMatrix4f("u_view", renderer::c.view());
			cubemapShader.setUniform1f("u_mip", cubemapMip);
			cubemapShader.setUniform1i("u_gammacorrection", renderer::properties.gammaCorrection);
			cubemapShader.setUniform1i("u_toneMapping", renderer::properties.toneMapping);
			cubemapShader.setUniform1f("u_exposure", renderer::properties.exposure);
			cubemapShader.setUniform1f("u_gamma", renderer::properties.gamma);
			cubemapShader.setUniform1i("u_cubemap", 0);
			cubemap.bind(0);
			if (prefilter) prefilteredCubemap.bind(0);
			cube.bind();
			GLCALL(glDrawElements(GL_TRIANGLES, cube.indexCount(), GL_UNSIGNED_INT, NULL));
			drawCalls++;
			cube.unbind();
			cubemap.unbind(0);
			cubemapShader.unuse();
		}
		

		lShader.use();
		
		
		for (i = 0; i < 4; i++)
		{
			GLCALL(glActiveTexture(GL_TEXTURE0 + i));
			GLCALL(glBindTexture(GL_TEXTURE_2D, gBuffer.getTextureAttachment(i)));
		}

		cubemap.bind(10);
		if (prefilter) prefilteredCubemap.bind(10);

		lShader.setUniform1i("g_Albedo", 0);
		lShader.setUniform1i("g_Position", 1);
		lShader.setUniform1i("g_Normal", 2);
		lShader.setUniform1i("g_Material", 3);
		lShader.setMatrix4f("u_view", c.view());
		lShader.setVector3f("camPos", c.position());

		lShader.setUniform1f("u_gamma", properties.gamma);
		lShader.setUniform1f("u_exposure", properties.exposure);
		lShader.setUniform1f("u_ambient", properties.ambient);

		// Lights (Hard coded yet)
		lShader.setUniform1i("amountOfLights", lights.size());

		for (int i = 0; i < lights.size(); i++)
		{
			std::string uniformName = "lights[" + std::to_string(i) + "].";
			lShader.setVector3f(uniformName + "position", lights[i].position);
			lShader.setVector3f(uniformName + "color", lights[i].color * lights[i].brightness);
		}



		// IBL
		lShader.setUniform1i("u_irradianceMapSet", properties.iblDiffuseIradiance);
		lShader.setUniform1i("u_specularIBL", properties.iblSpecular);
		lShader.setUniform1i("u_cubemap", 10);
		lShader.setUniform1i("irradianceMap", 11);
		lShader.setUniform1i("u_brdf", 12);

		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, brdfTexture);
		iradianceMap.bind(11);
		glActiveTexture(GL_TEXTURE0);
		lShader.setUniform1i("toneMapping", properties.toneMapping);
		lShader.setUniform1i("gammaCorrection", properties.gammaCorrection);

		quad.bind();
		GLCALL(glDrawElements(GL_TRIANGLES, quad.indexCount(), GL_UNSIGNED_INT, NULL));
		drawCalls++;
		quad.unbind();
		
		
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, 0);

		for (i = 0; i < 4; i++)
		{
			GLCALL(glActiveTexture(GL_TEXTURE0 + i));
			GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
		
		cubemap.unbind(10);
		iradianceMap.unbind(11);
		lShader.unuse();
		postBuffer.unbind();

		
		//GLCALL(glBindImageTexture(0, postBuffer.getTextureAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F));
		//compute.dispatch(ceil(r_width/4), ceil(r_height/16), 1); // 4x16 Invocations (64 Wavefront/AMD)
		//GLCALL(glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F));

		fb.bind();
		GLCALL(glClearColor(0,0,0,0));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));
		pShader.use();

		pShader.setUniform1i("frame", 0);
		pShader.setUniform1f("saturation", properties.saturation);
		pShader.setVector3f("color_filter", properties.filter);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postBuffer.getTextureAttachment(0));

		quad.bind();
		GLCALL(glDrawElements(GL_TRIANGLES, quad.indexCount(), GL_UNSIGNED_INT, NULL));
		drawCalls++;
		quad.unbind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		pShader.unuse();
		fb.unbind();
		totalFrames++;
	}

	void dispose()
	{

		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i].material.albedo != nullptr)
			{
				objects[i].material.albedo->destroy();
				delete objects[i].material.albedo;
				objects[i].material.albedo = nullptr;
			}	
			if (objects[i].material.normal != nullptr)
			{
				objects[i].material.normal->destroy();
				delete objects[i].material.normal;
				objects[i].material.normal = nullptr;
			}
			if (objects[i].material.roughness != nullptr)
			{
				objects[i].material.roughness->destroy();
				delete objects[i].material.roughness;
				objects[i].material.roughness = nullptr;
			}
			if (objects[i].material.metallic != nullptr)
			{
				objects[i].material.metallic->destroy();
				delete objects[i].material.metallic;
				objects[i].material.metallic = nullptr;
			}
			if (objects[i].material.ambientOcclusion != nullptr)
			{
				objects[i].material.ambientOcclusion->destroy();
				delete objects[i].material.ambientOcclusion;
				objects[i].material.ambientOcclusion = nullptr;
			}
		}
		
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].destroy();
		}
		
		cubemap.destroy();
		prefilteredCubemap.destroy();
		lightTexture.destroy();
		cube.destroy();
		quad.destroy();

		cubemapShader.destroy();
		gShader.destroy();
		lShader.destroy();
		pShader.destroy();
		compute.destroy();
		rt_info("Renderer", "Destroyed shaders");
		gBuffer.destroy();
		postBuffer.destroy();
		rt_info("Renderer", "Destroyed buffers");
	
	}

}

RT_END
