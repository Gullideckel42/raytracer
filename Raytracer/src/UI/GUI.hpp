#pragma once

#include "../Core.hpp"
#include "../Util/FileDialog.hpp"
#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_impl_glfw.h"
#include "../imgui-docking/imgui_impl_opengl3.h"
#include "../imgui-docking/ImGuizmo.h"

RT_START
namespace ui {

    int selectedObject = 0;
    float fontsize = 15.5;
    bool grid = false;

    enum GizmoOperation
    {
        NONE = 0,
        TRANSLATE = 1,
        ROTATE = 2,
        SCALE = 3
    };

    GizmoOperation selectedOperation = GizmoOperation::NONE;

    GL_ Texture checkerBoard, openFolder, pin, crosshair, crosshairRed;

    bool sceneViewFocused = false;

    namespace item
    {
        bool vec3Component(std::string label, glm::vec3* data, glm::vec3 resetValue, const char* s[3], float speed = 0.01f, float min = 0.0f, float max = 0.0f)
        {

            bool r = false;

            ImGui::PushID(label.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button(s[0])) {
                data->x = resetValue.x;
                r = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            r |= ImGui::DragFloat("##X", &data->x, speed, min, max);
            // Y
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if (ImGui::Button(s[1]))
            {
                data->y = resetValue.y;
                r = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            r |= ImGui::DragFloat("##Y", &data->y, speed, min, max);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            if (ImGui::Button(s[2]))
            {
                data->z = resetValue.z;
                r = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            r |= ImGui::DragFloat("##Z", &data->z, speed, min, max);

            ImGui::PopID();
            return r;
        }

        inline void textureSelector(GL_ Texture** texture, bool* usesTexture)
        {
            const char* filter = "Texture (*.png, *.jpg, *.jpeg)\0*.png;*.jpg;*.jpeg\0";
            float colWidth = ImGui::GetContentRegionAvail().x / 2.4f;
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, colWidth);

            ImTextureID img = (ImTextureID)((*texture) ? (*texture)->getId() : checkerBoard.getId());

            ImGui::Image(img, { colWidth - 15.0f, colWidth - 15.0f });

            ImGui::NextColumn();

            ImGui::Checkbox("Use Texture", usesTexture);

            if (ImGui::ImageButton((ImTextureID)openFolder.getId(), { 25,25 }, ImVec2(0, 1), ImVec2(1, 0)))
            {
                GL_ Texture* tex = *texture;
                std::string path = util::filedialog::OpenFile(w.handle(), filter);
                if (path != "___failed___")
                {
                    if (tex != nullptr)
                    {
                        tex->destroy();
                        delete tex;
                        *texture = nullptr;
                    }
                    *texture = new GL_ Texture;
                    (*texture)->load(path);
                }
            }

            ImGui::Columns(1);
        }
    }

    void init()
    {
        checkerBoard.load("assets/images/Checkerboard.png");
        openFolder.load("assets/images/folder.png");
        pin.load("assets/images/pin.png");
        crosshair.load("assets/images/fokus.png");
        crosshairRed.load("assets/images/fokusRed.png");
    }

    void dispose()
    {
        crosshairRed.destroy();
        crosshair.destroy();
        pin.destroy();
        openFolder.destroy();
        checkerBoard.destroy();
    }

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
            if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
            {
                // TODO
            }
            if (ImGui::MenuItem("Save Frame", "Ctrl + D"))
            {
                std::string path = util::filedialog::SaveFile(w.handle(), "PNG (, *.png)\0*.png;\0");
                if (path != std::string("___failed___"))
                {
                    fb.saveToPng(path, 0);
                    rt_info("UI", "Saved frame to ", path);
                }
                else
                {
                    rt_warn("UI", "File dialog canceled");
                }
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
            if (ImGui::MenuItem("Toggle Grid", "G")) {
                grid = !grid;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::MenuItem("Render frame", "F12")) {
                rt_warn("UI", "Renderer not implemented yet");
            }
            if (ImGui::BeginMenu("Reload Shaders"))
            {
                if (ImGui::MenuItem("Geometry pass"))
                {
                    renderer::gShader.reload();
                }
                if (ImGui::MenuItem("Cubemap pass"))
                {
                    renderer::cubemapShader.reload();
                }
                if (ImGui::MenuItem("Lighting pass"))
                {
                    renderer::lShader.reload();
                }
                if (ImGui::MenuItem("Postprocessing pass"))
                {
                    renderer::pShader.reload();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
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
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)pin.getId(), { 15,15 }, ImVec2(0, 1), ImVec2(1, 0)))
            {
                renderer::c.lookAt() = obj->getPosition();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)(selectedObject == renderer::c.following() ? crosshairRed.getId(): crosshair.getId()), { 15,15 }, ImVec2(0, 1), ImVec2(1, 0)))
            {
                if (renderer::c.following() == selectedObject)
                {
                    renderer::c.following() = -1;
                }
                else
                    renderer::c.following() = selectedObject;
                renderer::c.lookAt() = obj->getPosition();
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
            float columnWidth = ImGui::CalcTextSize("_____________________").x;
            if (ImGui::CollapsingHeader("Rendering"))
            {

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, columnWidth);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
                ImGui::Text("Visible");
                ImGui::BeginDisabled(!obj->visible());
                ImGui::Text("Smooth Shading");
                ImGui::Text("Wireframe");
                ImGui::Text("Face culling"); 
                ImGui::Text("Depth testing");
                ImGui::EndDisabled();
                ImGui::PopStyleVar();
                ImGui::NextColumn();

                ImGui::Checkbox("##Visible_", &obj->visible());
                ImGui::BeginDisabled(!obj->visible());
                ImGui::Checkbox("##Smooth", &obj->smooth());
                ImGui::Checkbox("##Wireframe", &obj->Wireframe());
                ImGui::Checkbox("##Face culling", &obj->backfaceCulling());
                
                ImGui::SameLine();
                ImGui::BeginDisabled(!obj->backfaceCulling());
                static const char* cullModes[] = {"Clockwise", "Counterclockwise" };
                static int currentItem = 0;
                if (ImGui::Combo("##cullmode", &currentItem, cullModes, 2))
                {
                    obj->cullMode() = (bool)currentItem;
                }
                ImGui::EndDisabled();
                ImGui::Checkbox("##depthTesting", &obj->depthTesting());
                ImGui::EndDisabled();
            }
            ImGui::Columns(1);
            ImGui::BeginDisabled(!obj->visible());
            if (ImGui::CollapsingHeader("Material"))
            {
                
                static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_TabListPopupButton;

                if (ImGui::BeginTabBar("Material-Tab-Bar", tab_bar_flags))
                {
                    if (ImGui::BeginTabItem("Albedo"))
                    {
                        ImGui::PushID("properties-albdeo");
                        ImGui::BeginDisabled(obj->usesAlbedoMap());
                        ImGui::ColorEdit3("Color", &obj->material._albedo[0]);
                        ImGui::EndDisabled();
                        item::textureSelector(&obj->material.albedo, &obj->usesAlbedoMap());
                        
                        ImGui::PopID();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Normal"))
                    {
                        ImGui::PushID("properties-normal");

                        item::textureSelector(&obj->material.normal, &obj->usesNormalMap());

                        ImGui::PopID();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Roughness"))
                    {
                        ImGui::PushID("properties-roughness");
                        ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 6);
                        ImGui::BeginDisabled(obj->usesRoughnessMap());
                        ImGui::DragFloat("##Roughness", &obj->material._roughness, 0.01, 0.0, 1.0);
                        ImGui::PopItemWidth();
                        ImGui::SameLine();
                        ImGui::SliderFloat("##r", &obj->material._roughness, 0.0, 1.0);
                        ImGui::EndDisabled();
                        item::textureSelector(&obj->material.roughness, &obj->usesRoughnessMap());

                        ImGui::PopID();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Metallic"))
                    {
                        ImGui::PushID("properties-metallic");
                        ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 6);
                        ImGui::BeginDisabled(obj->usesMetallicMap());
                        ImGui::DragFloat("##Metallic", &obj->material._metallic, 0.01, 0.0, 1.0);
                       
                        ImGui::PopItemWidth();
                        ImGui::SameLine();
                        ImGui::SliderFloat("##m", &obj->material._metallic, 0.0, 1.0);
                        ImGui::EndDisabled();
                        item::textureSelector(&obj->material.metallic, &obj->usesMetallicMap());

                        ImGui::PopID();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
                
            }
            columnWidth = ImGui::CalcTextSize("______________").x;
            if (ImGui::CollapsingHeader("Transform"))
            {
                ImGui::PushID("transform");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, columnWidth);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
                ImGui::Text("Position");
                ImGui::Text("Rotation");
                ImGui::Text("Scale");
                ImGui::PopStyleVar();
                ImGui::NextColumn();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, ImGui::GetStyle().ItemSpacing.y });
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4);
                const char* xyz[] = { "X", "Y", "Z" };
                bool r = item::vec3Component("obj-position", &obj->getPosition(), { 0,0,0 }, xyz);
                r |= item::vec3Component("obj-rotation", &obj->getRotation(), { 0,0,0 }, xyz);
                r |= item::vec3Component("obj-scale", &obj->getScale(), { 1,1,1 }, xyz);
                ImGui::PopItemWidth();
                if (r) obj->updateTransform();
                ImGui::PopStyleVar();
                ImGui::Columns(1);
                ImGui::PopID();
            }
            ImGui::EndDisabled();
        }
        else
        {
            RT_ PointLight* light = &renderer::lights[selectedObject - objects.size()];
            float columnWidth = ImGui::CalcTextSize("______________").x;
            ImGui::PushID("Properties-Light");
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10,10 });
            ImGui::Text("Color");
            ImGui::Text("Position");
            ImGui::Text("Brightness");
            ImGui::PopStyleVar();
            ImGui::NextColumn();
            const char* xyz[] = { "X", "Y", "Z" };
            const char* rgb[] = { "R", "G", "B" };
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, ImGui::GetStyle().ItemSpacing.y });
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4.5);
            item::vec3Component("clr", &light->color, { 1,1,1 }, rgb, 0.01f, 0.0f, 1.0f);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::ColorEdit3("##Color", &light->color[0], ImGuiColorEditFlags_NoInputs);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4.2);
            item::vec3Component("Position", &light->position, { 0,0,0 }, xyz);
            ImGui::PopItemWidth();
            ImGui::DragFloat("##brightness", &light->brightness, 0.01, 0.0, 20.0f);
            ImGui::PopStyleVar();
            ImGui::Columns(1);
            ImGui::PopID();
        }

		ImGui::End();

	}


    void sceneHierarchyPanel()
    {
        using renderer::objects;
        using renderer::lights;

        ImGui::Begin("Scene Hierarchy");
        bool open = ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_SpanFullWidth);

        if (open) {
           
            for (int i = 0; i < objects.size(); i++)
            {
                std::string objectName = (std::to_string(i) + " \t " + objects[i].getName());
                if (ImGui::Selectable(objectName.c_str(), selectedObject == i))
                {
                    selectedObject = i;
                }
            }
        }
        
        bool b = ImGui::CollapsingHeader("Lights");

        if (b)
        {
            if (ImGui::Button("[+] New light"))
            {
                lights.push_back(RT_ PointLight{});
            }
            for (int i = 0; i < lights.size(); i++)
            {
                if (ImGui::Selectable((std::string("Light ") + std::to_string(i)).c_str(), selectedObject == (i + objects.size())))
                {
                    selectedObject = i + objects.size();
                    renderer::c.lookAt() = lights.at(selectedObject - objects.size()).position;
                    
                }
            }
        }


        ImGui::End();

    }

    void settingsPanel()
    {
        ImGui::ShowDemoWindow();
        
        using renderer::c;
        ImGui::Begin("Scene Renderer");
        float lineHeight = fontsize + ImGui::GetStyle().FramePadding.y * 2.0f;
        float columnWidth = ImGui::CalcTextSize("_____________________").x;
        if (ImGui::CollapsingHeader("Scene renderer"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
            ImGui::Text("Renderer");
            ImGui::Text("Graphics processor");
            ImGui::Text("Active");
            ImGui::Text("Clear");
            ImGui::Text("Wait between frames");
            ImGui::Text("Clear Color");
            ImGui::Text("Render light bulbs");
            ImGui::Text("Light bulb scale");
            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            static int currentItem;
            constexpr char* gapis[] = { "OpenGL", "Vulkan (not implemented)"};
            ImGui::Combo("##renderer", &currentItem, gapis, 1);
            ImGui::Text((const char*)glGetString(GL_RENDERER));
            ImGui::Checkbox("##active", &renderer::active);
            if (ImGui::Button("Clear"))
            {
                fb.bind();
                glClearColor(renderer::clearColor.r, renderer::clearColor.g, renderer::clearColor.b, renderer::clearColor.a);
                glClear(GL_COLOR_BUFFER_BIT);
                fb.unbind();
            }
            ImGui::DragFloat("##wait", &renderer::waitBetweenFramesMS, 0.01f, 0.0f, 100.0f);
            ImGui::ColorEdit4("##clearcolor", &renderer::clearColor[0]);
            ImGui::Checkbox("##lightBulbs", &renderer::renderlights);
            ImGui::DragFloat("##lightscale", &renderer::lightScale, 0.0005f, 0.0001f, 0.2f);
            ImGui::PopItemWidth();
        }
        ImGui::Columns(1);
        if (ImGui::CollapsingHeader("Cubemap"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
            ImGui::Text("Open");
            ImGui::Text("Render");
            ImGui::Text("Mip");
            ImGui::Text("Prefiltered");
            ImGui::Text("Specular IBL");
            ImGui::Text("Diffuse Iradiance");
            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            
            if (ImGui::ImageButton((ImTextureID)openFolder.getId(), { 15,15 }, ImVec2(0, 1), ImVec2(1, 0)))
            {
                std::string path = util::filedialog::OpenFile(rt::w.handle(), "Texture (*.hdr, *.png, *.jpg, *.jpeg,)\0*.hdr;*.png;*.jpg;*.jpeg\0");
                if (path != "___failed___")
                {
                    renderer::prefilteredCubemap.destroy();
                    renderer::cubemap.destroy();
                    renderer::iradianceMap.destroy();

                    renderer::cubemap.load(path);
                    renderer::prefilteredCubemap = renderer::cubemap.prefilter();
                    renderer::iradianceMap = renderer::cubemap.convolute();
#ifdef RT_PLATFORM_WINDOWS
                    Sleep(1000);
#else
                    std::this_thread::sleep(std::chrono::seconds(1));
#endif
                }

            }
            ImGui::BeginDisabled(!renderer::cubemap.isLoaded());
            ImGui::Checkbox("##rendercubemap", &renderer::renderCubemap);
            ImGui::DragFloat("##cubemapmip", &renderer::cubemapMip, 0.001, 0.0, 5.0);
            ImGui::Checkbox("##prefiltered", &renderer::prefilter);
            ImGui::Checkbox("##specibl", &renderer::properties.iblSpecular);
            ImGui::Checkbox("##diffibl", &renderer::properties.iblDiffuseIradiance);
            ImGui::EndDisabled();
            ImGui::PopItemWidth();
        }
        ImGui::Columns(1);
        if (ImGui::CollapsingHeader("Postprocessing"))
        {   
            ImGui::BeginDisabled(!renderer::active);
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
            ImGui::Text("Gamma");
            ImGui::Text("Exposure");
            ImGui::Text("Ambient");
            ImGui::Text("Saturation");
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.8,0.1,0.1,1 });
            ImGui::Text("Red");
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.1,0.8,0,1 });
            ImGui::Text("Green");
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.1,0.1,0.8,1 });
            ImGui::Text("Blue");
            ImGui::PopStyleColor();
            ImGui::Text("Tone mapping");
            ImGui::Text("Gamma correction");
            ImGui::PopStyleVar();

            ImGui::NextColumn();
            
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##Gamma", &renderer::properties.gamma, 0.01, 0.0, 10.0);
            ImGui::DragFloat("##Exposure", &renderer::properties.exposure, 0.01, 0.0, 10.0);
            ImGui::DragFloat("##Ambient", &renderer::properties.ambient, 0.01, 0.0, 10.0);
            ImGui::DragFloat("##saturation", &renderer::properties.saturation, 0.01, 0.0f, 4.0f);
            ImGui::DragFloat("##red", &renderer::properties.filter.r, 0.01, 0.0f, 4.0f);
            ImGui::DragFloat("##green", &renderer::properties.filter.g, 0.01, 0.0f, 4.0f);
            ImGui::DragFloat("##blue", &renderer::properties.filter.b, 0.01, 0.0f, 4.0f);
            ImGui::PopItemWidth();
            ImGui::Checkbox("##tonemapping", &renderer::properties.toneMapping);
            ImGui::Checkbox("##gammacorrection", &renderer::properties.gammaCorrection);
            ImGui::EndDisabled();

        }
        ImGui::Columns(1);

        
        ImGui::End();
    }
    
    void cameraPanel()
    {
        using renderer::c;
        ImGui::Begin("Camera");
        float columnWidth = ImGui::CalcTextSize("___________________").x;
        if (ImGui::CollapsingHeader("Projection"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
            ImGui::Text("FOV (Deg)");
            ImGui::Text("Near Plane");
            ImGui::Text("Far Plane");
            ImGui::Text("Aspect Ratio");

            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            bool b = false;
            b |= ImGui::DragFloat("##FOV", &c.fov(), 0.1, 20.0f, 300.0f);
            b |= ImGui::DragFloat("##Near", &c.nearPlane(), 0.1, 0.001f, 10.0f);
            b |= ImGui::DragFloat("##Far", &c.farPlane(), 0.1, 10.0f, 3000.0f);
            float itemwidth = ImGui::GetContentRegionAvail().x / 2.20f;
            ImGui::PopItemWidth();
            ImGui::PushItemWidth(itemwidth);
            b |= ImGui::DragFloat("##Width", &c.width()); ImGui::SameLine();
            ImGui::Text("/"); ImGui::SameLine();
            b |= ImGui::DragFloat("##Height", &c.height());
            ImGui::PopItemWidth();
            if (b) { c.updateProj(); }
        }
        ImGui::Columns(1);

        if (ImGui::CollapsingHeader("Viewport"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });

            ImGui::Text("Position");
            ImGui::Text("Size");

            ImGui::PopStyleVar();
            ImGui::NextColumn();


            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x/4);

            ImGui::PushID("viewport-position");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, ImGui::GetStyle().ItemSpacing.y });
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X")) {
                renderer::properties.viewport.pos.x = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##X", &renderer::properties.viewport.pos.x);
            // Y
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if(ImGui::Button("Y"))
            {
                renderer::properties.viewport.pos.y = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &renderer::properties.viewport.pos.y);
            ImGui::PopID();


            ImGui::PushID("viewport-size");

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X")) {
                renderer::properties.viewport.size.x = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##X", &renderer::properties.viewport.size.x);
            // Y
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if (ImGui::Button("Y"))
            {
                renderer::properties.viewport.size.y = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &renderer::properties.viewport.size.y);
            ImGui::PopID();

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();
        }
        ImGui::Columns(1);

        
        if (ImGui::CollapsingHeader("Movement"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
            ImGui::Text("Focus point");
            ImGui::Text("Rotation (RAD)");
            ImGui::Text("Radius");
            ImGui::Text("Up");
            ImGui::Text("Scroll sensititity");
            ImGui::Text("Movement speed");
            ImGui::PopStyleVar();
            ImGui::NextColumn();

            
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4);

            ImGui::PushID("movement-focus");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, ImGui::GetStyle().ItemSpacing.y});
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X")) {
                renderer::c.lookAt().x = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##X", &renderer::c.lookAt().x, 0.01f);
            // Y
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if (ImGui::Button("Y"))
            {
                renderer::c.lookAt().y = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &renderer::c.lookAt().y, 0.01f);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            if (ImGui::Button("Z"))
            {
                renderer::c.lookAt().z = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##Z", &renderer::c.lookAt().z, 0.01f);

            ImGui::PopID();


            ImGui::PushID("movement-rotation");
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X")) {
                renderer::c.rotation().x = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##X", &renderer::c.rotation().x, 0.01f, 0.01f, 3.14159f);
            // Y
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            if (ImGui::Button("Y"))
            {
                renderer::c.rotation().y = 0;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &renderer::c.rotation().y, 0.01f);

            ImGui::PopID();

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();


            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##Radius", &c.radius(), 0.01f, 0.01f, 100.0f);
            const char* xyz[] = { "X", "Y", "Z" };
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, ImGui::GetStyle().ItemSpacing.y });
            if (item::vec3Component("up", &renderer::c.up(), { 0,1,0 }, xyz))
            {
                renderer::c.update();
            }
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
            ImGui::DragFloat("##Scrollsensitivity", &c.scrollSensitivity(), 0.01f, 0.05f, 2.0f);
            ImGui::DragFloat("##movementspeed", &c.movementspeed, 0.01f, 0.001f, 2.0f);
            ImGui::PopItemWidth();


        }
        ImGui::Columns(1);

        ImGui::End();
    }

    void gBufferPanel()
    {
        using renderer::gBuffer;
        ImGui::Begin("G-Buffer");
        ImGui::Text("Geometry Buffer");
        ImGui::Separator();
        ImGui::Text("Albdeo");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(0), {ImGui::GetContentRegionAvail().x, r_height/r_width * ImGui::GetContentRegionAvail().x }, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("Position (World space)");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(1), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x }, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("Normal (World space)");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(2), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x }, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("Material");
        ImGui::Image((ImTextureID)gBuffer.getTextureAttachment(3), { ImGui::GetContentRegionAvail().x, r_height / r_width * ImGui::GetContentRegionAvail().x }, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
    }

	void sceneView()
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0,2});
		ImGui::Begin("Scene View");
        sceneViewFocused = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();

        float w = ImGui::GetContentRegionAvail().x;
        float h = r_height / r_width * w;

        ImGui::Image((ImTextureID) fb.getTextureAttachment(0), { w, h }, ImVec2(0, 1), ImVec2(1, 0));

        // ImGuizmo Transformation Gizmos

        if (grid || (selectedOperation != GizmoOperation::NONE))
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, w, h);
        }
        const glm::mat4 camPr = renderer::c.projection();
        const glm::mat4 camView = renderer::c.view();

        if (grid)
        {

            const glm::mat4 matr = glm::mat4(1.0);
            ImGuizmo::DrawGrid((float*)&camView, (float*)&camPr, (float*)&matr, 15.0f);
        }

        if (selectedObject >= 0 && selectedObject < renderer::objects.size())
        {
            RT_ Object* object = &renderer::objects.at(selectedObject);
            const glm::mat4 objTr = object->getTransform();
            bool modified = false;
            switch (selectedOperation)
            {
            case GizmoOperation::TRANSLATE:
                
                modified |= ImGuizmo::Manipulate((float*)&camView, (float*)&camPr,
                    ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, (float*)&objTr);
                object->getPosition() = objTr[3];
                break;
            case GizmoOperation::ROTATE:

                modified |= ImGuizmo::Manipulate((float*)&camView, (float*)&camPr,
                    ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, (float*)&objTr);
                glm::vec3 newRotation = glm::vec3(0.0f);
                newRotation[0] = atan2f(objTr[1][2], objTr[2][2]);
                newRotation[1] = atan2f(-objTr[0][2], sqrtf(objTr[1][2] * objTr[1][2] + objTr[2][2] * objTr[2][2]));
                newRotation[2] = atan2f(objTr[0][1], objTr[0][0]);
                object->getRotation() = newRotation;
                break;
            case GizmoOperation::SCALE:

                modified |= ImGuizmo::Manipulate((float*)&camView, (float*)&camPr,
                    ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, (float*)&objTr);
                float sx = glm::length(objTr[0]);
                float sy = glm::length(objTr[1]);
                float sz = glm::length(objTr[2]);
                object->getScale() = { sx, sy, sz };
                break;
            }

            if(modified)
                object->updateTransform();
        }
        else
        {

            rt::PointLight* light = &renderer::lights.at(selectedObject - renderer::objects.size());

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), light->position);

            if (selectedOperation == ui::GizmoOperation::TRANSLATE)
            {
                ImGuizmo::Manipulate((float*)&camView, (float*)&camPr,
                    ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, (float*)&transform);
                light->position = transform[3];
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
	}

    void logPanel()
    {
        ImGui::Begin("Log");
        
        ImGui::End();
    }

    void statisticsPanel()
    {
        using namespace renderer;

        ImGui::Begin("Statistics");
        float columnWidth = ImGui::CalcTextSize("_________________________").x;
        if (ImGui::CollapsingHeader("Graphics Processor"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });

            ImGui::Text("Graphics Processor");
            ImGui::Text("GPU Vendor");
            ImGui::Text("OpenGL API Version");
            ImGui::Text("GLSL Version");
            
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            ImGui::Text((const char*) glGetString(GL_RENDERER));
            ImGui::Text((const char*)glGetString(GL_VENDOR));
            ImGui::Text((const char*)glGetString(GL_VERSION));
            ImGui::Text((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();
            ImGui::Columns(1);
        }
        if (ImGui::CollapsingHeader("Realtime renderer"))
        {
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });

            ImGui::Text("Current frame");
            ImGui::Text("Drawcalls per frame");
            ImGui::Text("Framerate (fps)");
            ImGui::Text("Time per frame (ms)");

            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            static double frameTime = renderer::lastFrameMS;
            if (totalFrames % 20 == 0)
            {
                frameTime = renderer::lastFrameMS;
            }

            ImGui::Text(std::to_string(totalFrames).c_str());
            ImGui::Text(std::to_string(drawCalls).c_str());
            ImGui::Text((std::to_string(1000.0/ frameTime)).c_str());
            ImGui::Text((std::to_string(frameTime) + "ms").c_str());
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();
            ImGui::Columns(1);

        }

        ImGui::End();

    }

}

RT_END