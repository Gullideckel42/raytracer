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
	bool active = true;

	struct RendererProperties
	{
		Viewport viewport = {};
		float gamma = 2.0f;
		float exposure = 3.2f;
		float ambient = 0.001f;

		glm::vec3 filter = {1,1,1 };
		float saturation = 1.0f;

		bool toneMapping = true;
		bool gammaCorrection = true;
	};

	RendererProperties properties;

	GL_ Framebuffer gBuffer;
	GL_ Shader gShader;

	GL_ Shader lShader;
	GL_ Framebuffer postBuffer;

	GL_ Shader pShader;

	RT_ Camera c;
	GL_ Mesh cube, quad;

	std::vector<GL_ Mesh> meshes;
	std::vector<RT_ Object> objects;
	std::vector<RT_ PointLight> lights;

	GL_ ComputeShader compute;

	void init(float rwidth, float rheight)
	{

		properties.viewport = { {0,0}, {rwidth, rheight} };

		// Cube & Quad

		GL_ Vertex cubeVertex[] = H3D_CUBE_VERTICES(1, 1, 1);
		uint32_t cubeIndex[] = H3D_NORMAL_CUBE_INDICES;

		const h3dgl::Vertex quadVertices[] = {
			h3dgl::Vertex{-1,-1,0, 0,0, 0,0,1},
			h3dgl::Vertex{-1, 1,0, 0,1, 0,0,1},
			h3dgl::Vertex{ 1,-1,0, 1,0, 0,0,1},
			h3dgl::Vertex{ 1, 1,0, 1,1, 0,0,1}
		};
		const uint32_t quadIndices[] = { 2,1,0, 2,3,1 }; // Counter clockwise

		cube.load(cubeVertex, 8, cubeIndex, 36);
		quad.load(quadVertices, 4, quadIndices, 6);

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
		rt_info("Renderer", "Loaded shaders (3 Shaders)");

		GLenum postBufferAttachment = GL_COLOR_ATTACHMENT0;
		postBuffer.load(rwidth, rheight, 1, &postBufferAttachment, false);
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		gBuffer.load(rwidth, rheight, 4, attachments, true);
		rt_info("Renderer", "Allocated framebuffers (2 Buffers, 5 Attachments + Depth)");

		compute.load("src/shader/Compute.shader");
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
		gShader.use();
		int i = 0;

		c.update();
		gShader.setMatrix4f("u_viewProj", c.viewProjection());
		gShader.setVector3f("camPos", c.position());

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
		lShader.use();
		GLCALL(glClearColor(0, 0, 0, 1));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT));
		
		for (i = 0; i < 4; i++)
		{
			GLCALL(glActiveTexture(GL_TEXTURE0 + i));
			GLCALL(glBindTexture(GL_TEXTURE_2D, gBuffer.getTextureAttachment(i)));
		}


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
		lShader.setUniform1i("u_irradianceMapSet", false);
		lShader.setUniform1i("u_specularIBL", false);
		lShader.setUniform1i("u_cubemap", 10);
		lShader.setUniform1i("irradianceMap", 11);
		lShader.setUniform1i("u_brdf", 12);

		lShader.setUniform1i("toneMapping", properties.toneMapping);
		lShader.setUniform1i("gammaCorrection", properties.gammaCorrection);

		quad.bind();
		GLCALL(glDrawElements(GL_TRIANGLES, quad.indexCount(), GL_UNSIGNED_INT, NULL));
		drawCalls++;
		quad.unbind();
		
		
		for (i = 0; i < 4; i++)
		{
			GLCALL(glActiveTexture(GL_TEXTURE0 + i));
			GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
		
		lShader.unuse();
		postBuffer.unbind();

		
		//GLCALL(glBindImageTexture(0, postBuffer.getTextureAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F));
		//compute.dispatch(ceil(r_width/4), ceil(r_height/16), 1); // 4x16 Invocations (64 Wavefront/AMD)
		//GLCALL(glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F));

		fb.bind();
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
		
		cube.destroy();
		quad.destroy();

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
