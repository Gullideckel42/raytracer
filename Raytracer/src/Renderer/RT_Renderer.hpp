#pragma once
#include "../Core.hpp"
#include "RT_Object.hpp"
#include "../GLAPI/GLapi.hpp"
#include "RT_Camera.hpp"
RT_START

// The realtime renderer
namespace renderer {

	GL_ Framebuffer gBuffer;
	GL_ Shader gShader;

	GL_ Shader lShader;
	GL_ Framebuffer postBuffer;

	GL_ Shader pShader;

	RT_ Camera c;
	GL_ Mesh cube, quad;

	std::vector <GL_ Mesh> meshes;
	std::vector<RT_ Object> objects;

	void init(float rwidth, float rheight)
	{


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
		objects.push_back(RT_ Object());
		GL_ Vertex v[] = H3D_NORMAL_CUBE_VERTICES(1, 1, 1);
		meshes[0].loadFromFile("assets/model/hirsch.obj");
		objects.at(0).create("Cube", &meshes[0], { {0,1,0}, 0.4f, 0.6f });
		objects.at(0).getPosition().z = -6;
		objects.at(0).updateTransform();
		objects.push_back(RT_ Object());
		objects.at(1).create("Cube2", &meshes[0], { {1,0,0}, 0.4f, 0.6f });
		objects.at(1).getPosition() = {2, -1, -5};
		objects.at(1).updateTransform();


		// Camera
		c.create(90.0, rwidth, rheight, 0.1, 100.0);
		rt_info("Created perspective camera (FOV: ", 90.0f, "°)");

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		gBuffer.load(rwidth, rheight, 4, attachments, true);
		rt_info("Loaded GBuffer");

		gShader.create("src/shader/GeometryPass.shader", true);
		rt_info("Loaded geometry pass shader");

		rt_info("Initialized realtime renderer");
	}

	void render()
	{
		glViewport(0, 0, r_width, r_height);
		gBuffer.bind();

		glClearColor(0, 0, 0, 1);
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
			gShader.setUniform1f("u_rougness", obj->material._roughness);
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
		

		fb.bind();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5, 0);
		glVertex2f(0.5, -0.5);
		glVertex2f(0.5, 0.5);
		glEnd();
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
		rt_info("Disposed renderer");
	}

}

RT_END
