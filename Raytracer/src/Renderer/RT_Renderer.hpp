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

	

	struct RendererProperties
	{
		Viewport viewport = {};
		float gamma = 2.0f;
		float exposure = 3.2f;
		float ambient = 0.01f;

		glm::vec3 filter = {1,1,1 };
		float saturation = 1.0f;

		bool toneMapping = true;
	};

	RendererProperties properties;

	GL_ Framebuffer gBuffer;
	GL_ Shader gShader;

	GL_ Shader lShader;
	GL_ Framebuffer postBuffer;

	GL_ Shader pShader;

	RT_ Camera c;
	GL_ Mesh cube, quad;

	std::vector <GL_ Mesh> meshes;
	std::vector<RT_ Object> objects;

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


		// Camera
		c.create(90.0, rwidth, rheight, 0.1, 100.0);
		rt_info("Created perspective camera (FOV: ", 90.0f, " Degree)");

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		gBuffer.load(rwidth, rheight, 4, attachments, true);
		rt_info("Loaded GBuffer");

		gShader.create("src/shader/GeometryPass.shader", true);
		rt_info("Loaded geometry pass shader");

		lShader.create("src/shader/LightingPass.shader", false);
		rt_info("Loaded lighting pass shader");

		GLenum postBufferAttachment = GL_COLOR_ATTACHMENT0;
		postBuffer.load(rwidth, rheight, 1, &postBufferAttachment, false);

		pShader.create("src/shader/PostProcessing.shader", false);

		compute.load("src/shader/Compute.shader");

		rt_info("Initialized realtime renderer");
	}

	void render()
	{
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
		lShader.setUniform1i("amountOfLights", 2);
		lShader.setVector3f("lights[0].position", { 1,1,1 });
		lShader.setVector3f("lights[0].color", { 79,19,19 });
		lShader.setVector3f("lights[1].position", { 0,0,-4 });
		lShader.setVector3f("lights[1].color", { 1,1,1 });



		// IBL
		lShader.setUniform1i("u_irradianceMapSet", false);
		lShader.setUniform1i("u_specularIBL", false);
		lShader.setUniform1i("u_cubemap", 10);
		lShader.setUniform1i("irradianceMap", 11);
		lShader.setUniform1i("u_brdf", 12);

		lShader.setUniform1i("toneMapping", properties.toneMapping);
		
		quad.bind();
		GLCALL(glDrawElements(GL_TRIANGLES, quad.indexCount(), GL_UNSIGNED_INT, NULL));
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
		quad.unbind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		pShader.unuse();
		fb.unbind();
	}

	void dispose()
	{

		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].destroy();
			rt_info("Destroyed mesh ", i);
		}
		
		cube.destroy();
		quad.destroy();

		gShader.destroy();
		rt_info("Destroyed geometry pass shader");
		gBuffer.destroy();
		rt_info("Destroyed GBuffer");

		lShader.destroy();
		rt_info("Destroyed lighting pass shader");
		postBuffer.destroy();
		rt_info("Destroyed post processing buffer");

		pShader.destroy();
		rt_info("Destroyed post processing shader");

		compute.destroy();

		rt_info("Disposed renderer");
	}

}

RT_END
