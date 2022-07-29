#pragma once

#include "../Core.hpp"
#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_impl_glfw.h"
#include "../imgui-docking/imgui_impl_opengl3.h"


RT_START
namespace ui {

    int selectedObject = 0;

    void setDarkTheme() {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }


    void docking()
    {
    
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", 0, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 300.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        }

        ImGui::BeginMenuBar();
        bool b = false;
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save (JSON)", "Ctrl + S"))
            {
                // TODO
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Undo", "Ctrl + Z");
            ImGui::MenuItem("Redo", "Ctrl + Y");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::MenuItem("Hide UI", "F")) {

            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();


        ImGui::End();
        //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }


	void propertiesPanel()
	{
		
        using renderer::objects;

		ImGui::Begin("Properties", 0, ImGuiWindowFlags_MenuBar);
		ImGui::Text("Properties");

        if (selectedObject >= 0 && selectedObject < objects.size()) {
            RT_ Object* obj = &objects.at(selectedObject);

            char buffer[256];
            memset((void*)buffer, 0, sizeof(buffer));
#pragma warning(suppress : 4996).
            std::strncpy(buffer, obj->getName().c_str(), sizeof(buffer));
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                std::string nme = std::string(buffer);
                obj->setName(nme);
            }
            ImGui::SameLine();
            ImGui::Checkbox("##Visible", &obj->visible());

            if (ImGui::TreeNode("Rendering"))
            {

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Material"))
            {

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Transform"))
            {

                ImGui::TreePop();
            }

        }

		ImGui::End();
	
	}


    void sceneHierarchyPanel()
    {
        using renderer::objects;
        
        ImGui::Begin("Scene Hierarchy");
        for (int i = 0; i < objects.size(); i++)
        {
            std::string objectName = (std::to_string(i) + " \t " + objects[i].getName());
            if (ImGui::Selectable(objectName.c_str(), selectedObject == i))
            {
                renderer::c.lookAt() = objects[i].getPosition();
                selectedObject = i;
            }
        }
        ImGui::End();

    }

    void settingsPanel()
    {
        using renderer::c;
        ImGui::Begin("Settings");


        ImGui::End();
    }
    

    void cameraPanel()
    {
        using renderer::c;
        ImGui::Begin("Camera");
        ImGui::Text("Projection");

        bool b = false;
        b |= ImGui::DragFloat("FOV", &c.fov(), 0.1, 20.0f, 300.0f);
        b |= ImGui::DragFloat("Near", &c.near(), 0.1, 0.001f, 10.0f);
        b |= ImGui::DragFloat("Far", &c.far(), 0.1, 10.0f, 3000.0f);
        b |= ImGui::DragFloat("Width", &c.width());
        b |= ImGui::DragFloat("Height", &c.height());

        if (b) { c.updateProj(); }
        ImGui::Separator();
        ImGui::Text("Movement");
        if (ImGui::DragFloat3("Focus", &c.lookAt()[0], 0.01f))
        {
            renderer::c.update();

        }

        if (ImGui::DragFloat2("Rotate", &c.rotation()[0], 0.01f, 0.01f, 3.14159f))
        {
            c.update();
        }

        ImGui::DragFloat("Radius", &c.radius(), 0.01f, 0.01f, 100.0f);

        ImGui::End();
    }

    void gBufferPanel()
    {
        using renderer::gBuffer;
        ImGui::Begin("G-Buffer");
        ImGui::Text("Geometry Buffer");
        ImGui::Separator();
        ImGui::Text("Albdeo");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(0), {ImGui::GetContentRegionAvail().x, r_height/r_width * ImGui::GetContentRegionAvail().x });
        ImGui::Text("Position (World space)");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(1), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x });
        ImGui::Text("Normal (World space)");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(2), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x });
        ImGui::Text("Material");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(3), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x });

        ImGui::End();
    }

	void sceneView()
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5,5});
		ImGui::Begin("Scene View");

        float w = ImGui::GetContentRegionAvail().x;
        float h = r_height / r_width * w;

        ImGui::Image((ImTextureID)renderer::gBuffer.getTextureAttachment(2), { w, h });

        ImGui::End();
        ImGui::PopStyleVar(2);
	}

}

RT_END