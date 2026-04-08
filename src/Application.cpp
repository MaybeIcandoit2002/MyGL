#include "MyWindow.h"
#include "RandomDevice.h"
#include "Macros.h"

#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <io.h>
#include <direct.h>
#include <locale>
#include <codecvt>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_internal.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include "vendor/json.hpp"

#include "tests/TestMenu.h"
#include "tests/AddNewObject.h"
#include "tests/EditProperties.h"
#include "tests/SearchRoot.h"
#include "tests/Setting.h"
#include "tests/GlobalPhysicParams.h"
#include "tests/ConfirmPopup.h"

inline static const ComponentTemplate* FindTemplateByName(const std::string& name)
{
    for (const auto& ct : Sources::Instance()->GetComponentTemplate())
    {
        if (ct.name == name) return &ct;
    }
    return nullptr;
}

inline static std::wstring Utf8ToWide(const std::string& utf8)
{
    if (utf8.empty()) return std::wstring();
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(utf8);
    }
    catch (...)
    {
        return std::wstring();
    }
}

inline static std::string WideToUtf8(const std::wstring& wide)
{
    if (wide.empty()) return std::string();
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wide);
    }
    catch (...)
    {
        return std::string();
    }
}

inline static bool ReadJsonFileUtf8Path(const std::string& path, nlohmann::json& outJson)
{
    const std::wstring wpath = Utf8ToWide(path);
    if (wpath.empty()) return false;

    FILE* fp = nullptr;
    if (_wfopen_s(&fp, wpath.c_str(), L"rb") != 0 || !fp)
        return false;

    std::string content;
    char buffer[4096];
    while (true)
    {
        const size_t n = fread(buffer, 1, sizeof(buffer), fp);
        if (n > 0) content.append(buffer, buffer + n);
        if (n < sizeof(buffer)) break;
    }
    fclose(fp);

    if (content.empty()) return false;
    outJson = nlohmann::json::parse(content, nullptr, false);
    return !outJson.is_discarded();
}

inline static bool WriteJsonFileUtf8Path(const std::string& path, const nlohmann::json& json)
{
    const std::wstring wpath = Utf8ToWide(path);
    if (wpath.empty()) return false;

    FILE* fp = nullptr;
    if (_wfopen_s(&fp, wpath.c_str(), L"wb") != 0 || !fp)
        return false;

    const std::string content = json.dump(2);
    const size_t wrote = fwrite(content.data(), 1, content.size(), fp);
    fclose(fp);
    return wrote == content.size();
}

inline static bool IsValidSceneName(const std::string& name)
{
    const std::wstring wname = Utf8ToWide(name);
    if (wname.empty() || wname.size() > 64) return false;

    for (wchar_t c : wname)
    {
        if (c < 32) return false;
        switch (c)
        {
        case L'<': case L'>': case L':': case L'"': case L'/': case L'\\': case L'|': case L'?': case L'*':
            return false;
        default:
            break;
        }
    }

    if (!wname.empty() && (wname.back() == L' ' || wname.back() == L'.'))
        return false;

    return true;
}

inline static std::string NormalizeSceneName(const std::string& sceneName)
{
    if (sceneName.size() > 5 && sceneName.substr(sceneName.size() - 5) == ".json")
        return sceneName.substr(0, sceneName.size() - 5);
    return sceneName;
}

inline static std::string BuildScenePath(const std::string& sceneName)
{
    const std::string normalized = NormalizeSceneName(sceneName);
    return "res/scenes/" + normalized + ".json";
}

inline static std::string ResolveScenePathForLoad(const std::string& sceneName)
{
    const std::string normalized = NormalizeSceneName(sceneName);
    const std::string jsonPath = "res/scenes/" + normalized + ".json";
    const std::wstring wjsonPath = Utf8ToWide(jsonPath);
    if (!wjsonPath.empty() && _waccess(wjsonPath.c_str(), 0) == 0)
        return jsonPath;

    const std::string rawPath = "res/scenes/" + normalized;
    const std::wstring wrawPath = Utf8ToWide(rawPath);
    if (!wrawPath.empty() && _waccess(wrawPath.c_str(), 0) == 0)
        return rawPath;

    return jsonPath;
}

inline static bool DeleteSceneByName(const std::string& sceneName)
{
    const std::string normalized = NormalizeSceneName(sceneName);
    bool removed = false;

    const std::string jsonPath = "res/scenes/" + normalized + ".json";
    const std::wstring wjsonPath = Utf8ToWide(jsonPath);
    if (!wjsonPath.empty() && _waccess(wjsonPath.c_str(), 0) == 0)
    {
        if (_wremove(wjsonPath.c_str()) == 0)
            removed = true;
    }

    const std::string rawPath = "res/scenes/" + normalized;
    const std::wstring wrawPath = Utf8ToWide(rawPath);
    if (!wrawPath.empty() && _waccess(wrawPath.c_str(), 0) == 0)
    {
        if (_wremove(wrawPath.c_str()) == 0)
            removed = true;
    }

    return removed;
}

inline static std::vector<std::string> GetSceneNames()
{
    std::vector<std::string> names;
    _wmkdir(L"res");
    _wmkdir(L"res/scenes");

    _wfinddata_t fileInfo{};
    intptr_t handle = _wfindfirst(L"res/scenes/*", &fileInfo);
    if (handle == -1)
        return names;

    do
    {
        if (fileInfo.attrib & _A_SUBDIR)
            continue;

        const std::wstring wfilename = fileInfo.name;
        std::string filename = WideToUtf8(wfilename);
        if (!IsValidSceneName(filename))
            continue;

        std::string displayName = filename;
        if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json")
            displayName = filename.substr(0, filename.size() - 5);

        if (!displayName.empty())
            names.push_back(displayName);
    } while (_wfindnext(handle, &fileInfo) == 0);

    _findclose(handle);
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}

inline static Component* CreateComponentByTemplate(MyWindow* window, const ComponentTemplate& ct, Component* parent)
{
    Mesh* mesh = Sources::Instance()->GetMesh(ct.meshName);
    if (!mesh || !window || !parent) return nullptr;

    Component* child = new Component(mesh);
    parent->AddChild(child);
    child->templateName = ct.name;
    child->name = ct.name;
    child->shapeType = ct.shapeType;
    child->SetTextureSlot(ct.textureSlot);
    child->SetPosition(0, 0);
    child->SetScale(ct.scale1, ct.scale2);
    child->SetBackgroundColor(ct.backgroundColor[0], ct.backgroundColor[1], ct.backgroundColor[2], ct.backgroundColor[3]);

    bool physicsInited = false;
    switch (ct.shapeType)
    {
    case ShapeType::Circle:
        child->InitPhysicProperty(ct.physicSize1, ct.physicMass, ct.physicRestitution, ct.physicFriction);
        physicsInited = true;
        break;
    case ShapeType::Box:
        if (ct.physicSize2)
        {
            child->InitPhysicProperty(ct.physicSize1, ct.physicSize2[0], ct.physicMass, ct.physicRestitution, ct.physicFriction);
            physicsInited = true;
        }
        break;
    case ShapeType::Polygon:
        if (ct.physicSize2)
        {
            child->InitPhysicProperty(static_cast<int>(ct.physicSize1), ct.physicSize2, ct.physicMass, ct.physicRestitution, ct.physicFriction);
            physicsInited = true;
        }
        break;
    default:
        break;
    }

    if (!physicsInited || !child->HasPhysicsShape())
    {
        parent->RemoveChild(child);
        delete child;
        return nullptr;
    }

    child->SetSensor(true);
    return child;
}

inline static void CollectSceneNodes(Component* node, std::uint64_t parentId, nlohmann::json& objects)
{
    if (!node) return;

    glm::vec2 pos, scale, relPos, relScale;
    glm::vec4 color;
    float rot = 0.0f;
    node->GetPosition(pos);
    node->GetScale(scale);
    node->GetRotation(rot);
    node->GetBackgroundColor(color);
    node->GetPhysicRelativePosition(relPos);
    node->GetPhysicRelativeScale(relScale);

    nlohmann::json obj;
    obj["id"] = node->GetStableId();
    obj["parentId"] = parentId;
    obj["templateName"] = node->templateName.empty() ? node->name : node->templateName;
    obj["name"] = node->name;
    obj["isDescriptionComponent"] = node->isDescriptionComponent;
    if (node->isDescriptionComponent)
    {
        obj["descriptionText"] = node->descriptionText;
        obj["descriptionOffset"] = { node->descriptionOffset.x, node->descriptionOffset.y };
        obj["descriptionShowParams"] = node->descriptionShowParams;
        obj["descriptionShowParamName"] = node->descriptionShowParamName;
        obj["descriptionShowParamPosition"] = node->descriptionShowParamPosition;
        obj["descriptionShowParamScale"] = node->descriptionShowParamScale;
        obj["descriptionShowParamRotation"] = node->descriptionShowParamRotation;
        obj["descriptionShowParamPhysMass"] = node->descriptionShowParamPhysMass;
        obj["descriptionShowParamPhysFriction"] = node->descriptionShowParamPhysFriction;
        obj["descriptionShowParamPhysRestitution"] = node->descriptionShowParamPhysRestitution;
        obj["descriptionShowParamPhysVelocity"] = node->descriptionShowParamPhysVelocity;
        obj["descriptionShowParamPhysAngularVelocity"] = node->descriptionShowParamPhysAngularVelocity;
        obj["descriptionFontSize"] = node->descriptionFontSize;
        obj["descriptionLineSpacing"] = node->descriptionLineSpacing;
    }
    obj["enabled"] = node->enabled;
    obj["position"] = { pos.x, pos.y };
    obj["scale"] = { scale.x, scale.y };
    obj["rotation"] = rot;
    obj["color"] = { color.x, color.y, color.z, color.w };
    obj["textureSlot"] = node->GetTextureSlot();
    obj["hasPhysicBody"] = node->hasPhysicBody;
    obj["shapeType"] = static_cast<int>(node->shapeType);
    obj["physicMass"] = node->physicMass;
    obj["physicSize1"] = node->physicSize1;
    obj["physicSize2"] = node->physicSize2 ? node->physicSize2[0] : 0.0f;
    obj["friction"] = node->GetFriction();
    obj["restitution"] = node->GetRestitution();
    obj["relativePos"] = { relPos.x, relPos.y };
    obj["relativeScale"] = { relScale.x, relScale.y };

    if (node->GetBody())
    {
        obj["bodyType"] = static_cast<int>(cpBodyGetType(node->GetBody()));
        const glm::vec2 velocity = node->GetVelocity();
        obj["velocity"] = { velocity.x, velocity.y };
        obj["angularVelocity"] = node->GetAngleVelocity();
    }

    objects.push_back(obj);

    for (Component* child : node->children)
        CollectSceneNodes(child, node->GetStableId(), objects);
}

inline static void SaveScene(MyWindow* window, const std::string& path)
{
    if (!window) return;
    nlohmann::json scene;
    scene["objects"] = nlohmann::json::array();
    scene["joints"] = nlohmann::json::array();

    Component* root = window->GetRootComponent();
    if (!root) return;

    glm::vec4 rootColor;
    root->GetBackgroundColor(rootColor);

    PhysicWorld* world = window->GetPhysicWorld();
    cpVect gravity = world ? world->GetGravity() : cpvzero;
    glm::vec4 clearColor = window->GetClearColor();
    glm::vec4 jointLineColor = window->GetJointLineColor();
    glm::vec2 viewCenter = window->GetViewCenter();

    scene["global"] = {
        {"clearColor", { clearColor.r, clearColor.g, clearColor.b, clearColor.a }},
        {"rootColor", { rootColor.r, rootColor.g, rootColor.b, rootColor.a }},
        {"viewScale", window->GetViewScale()},
        {"viewCenter", { viewCenter.x, viewCenter.y }},
        {"windowScale", window->GetWindowScale()},
        {"autoHideEditProperties", window->GetAutoHideEditProperties()},
        {"enableConfirmPopup", window->GetEnableConfirmPopup()},
        {"jointLineColor", { jointLineColor.r, jointLineColor.g, jointLineColor.b, jointLineColor.a }},
        {"jointLineThickness", window->GetJointLineThickness()},
        {"gravity", { static_cast<float>(gravity.x), static_cast<float>(gravity.y) }},
        {"staticFrictionCoeff", world ? static_cast<float>(world->GetStaticFrictionCoeff()) : 1.2f},
        {"kineticFrictionCoeff", world ? static_cast<float>(world->GetKineticFrictionCoeff()) : 0.8f},
        {"slipSpeedThreshold", world ? static_cast<float>(world->GetSlipSpeedThreshold()) : 0.5f},
        {"running", window->running}
    };

    std::unordered_map<cpBody*, std::uint64_t> bodyToId;
    for (Component* child : root->children)
    {
        CollectSceneNodes(child, 0, scene["objects"]);
    }

    std::vector<Component*> stack = root->children;
    while (!stack.empty())
    {
        Component* node = stack.back();
        stack.pop_back();
        if (!node) continue;
        if (node->GetBody()) bodyToId[node->GetBody()] = node->GetStableId();
        for (Component* c : node->children) stack.push_back(c);
    }

    if (world)
    {
        const auto jointSnapshots = world->GetJointSnapshots();
        for (const auto& j : jointSnapshots)
        {
            auto itA = bodyToId.find(j.bodyA);
            auto itB = bodyToId.find(j.bodyB);
            if (itA == bodyToId.end() || itB == bodyToId.end()) continue;

            nlohmann::json item;
            item["id"] = j.id;
            item["aId"] = itA->second;
            item["bId"] = itB->second;
            item["type"] = j.type;
            item["anchorA"] = { static_cast<float>(j.anchorA.x), static_cast<float>(j.anchorA.y) };
            item["anchorB"] = { static_cast<float>(j.anchorB.x), static_cast<float>(j.anchorB.y) };
            item["distance"] = static_cast<float>(j.distance);
            item["stiffness"] = static_cast<float>(j.stiffness);
            item["damping"] = static_cast<float>(j.damping);
            scene["joints"].push_back(item);
        }
    }

    WriteJsonFileUtf8Path(path, scene);
}

inline static void LoadScene(MyWindow* window, const std::string& path)
{
    if (!window) return;

    nlohmann::json scene;
    if (!ReadJsonFileUtf8Path(path, scene))
        return;

    Component* root = window->GetRootComponent();
    if (!root) return;

    while (!root->children.empty())
    {
        Component* child = root->children.back();
        root->RemoveChild(child);
        delete child;
    }

    std::unordered_map<std::uint64_t, Component*> idToComponent;

    if (scene.contains("objects") && scene["objects"].is_array())
    {
        std::size_t createdCount = 0;
        const std::size_t totalCount = scene["objects"].size();

        while (createdCount < totalCount)
        {
            bool progress = false;
            for (auto& obj : scene["objects"])
            {
                const std::uint64_t id = obj.value("id", static_cast<std::uint64_t>(0));
                if (id == 0 || idToComponent.find(id) != idToComponent.end())
                    continue;

                const std::uint64_t parentId = obj.value("parentId", static_cast<std::uint64_t>(0));
                Component* parent = (parentId == 0) ? root : (idToComponent.count(parentId) ? idToComponent[parentId] : nullptr);
                if (!parent)
                    continue;

                const bool isDescriptionComponent = obj.value("isDescriptionComponent", false);

                std::string templateName = obj.value("templateName", obj.value("name", std::string()));
                const ComponentTemplate* ct = FindTemplateByName(templateName);
                Component* node = nullptr;
                if (isDescriptionComponent)
                {
                    node = parent->CreateOrGetDescriptionChild();
                    if (node)
                    {
                        node->descriptionText = obj.value("descriptionText", std::string("Description"));
                        if (obj.contains("descriptionOffset") && obj["descriptionOffset"].is_array() && obj["descriptionOffset"].size() >= 2)
                            node->descriptionOffset = glm::vec2(obj["descriptionOffset"][0].get<float>(), obj["descriptionOffset"][1].get<float>());
                        else if (obj.contains("position") && obj["position"].is_array() && obj["position"].size() >= 2)
                            node->descriptionOffset = glm::vec2(obj["position"][0].get<float>(), obj["position"][1].get<float>());
                        else
                            node->descriptionOffset = glm::vec2(0.0f, 40.0f);
                        node->descriptionShowParams = obj.value("descriptionShowParams", false);
                        node->descriptionShowParamName = obj.value("descriptionShowParamName", true);
                        node->descriptionShowParamPosition = obj.value("descriptionShowParamPosition", true);
                        node->descriptionShowParamScale = obj.value("descriptionShowParamScale", false);
                        node->descriptionShowParamRotation = obj.value("descriptionShowParamRotation", false);
                        const bool legacyShowPhys = obj.value("descriptionShowPhysicsParams", false);
                        node->descriptionShowParamPhysMass = obj.value("descriptionShowParamPhysMass", legacyShowPhys);
                        node->descriptionShowParamPhysFriction = obj.value("descriptionShowParamPhysFriction", legacyShowPhys);
                        node->descriptionShowParamPhysRestitution = obj.value("descriptionShowParamPhysRestitution", legacyShowPhys);
                        node->descriptionShowParamPhysVelocity = obj.value("descriptionShowParamPhysVelocity", legacyShowPhys);
                        node->descriptionShowParamPhysAngularVelocity = obj.value("descriptionShowParamPhysAngularVelocity", legacyShowPhys);
                        node->descriptionFontSize = obj.value("descriptionFontSize", 16.0f);
                        node->descriptionLineSpacing = obj.value("descriptionLineSpacing", 2.0f);
                        node->templateName.clear();
                        node->SetTextureSlot(-1);
                        node->SetSensor(true);
                    }
                }
                else
                {
                    if (!ct) continue;
                    node = CreateComponentByTemplate(window, *ct, parent);
                }
                if (!node) continue;

                node->SetStableId(id);
                node->templateName = templateName;
                node->name = obj.value("name", templateName);
                node->enabled = obj.value("enabled", true);
                node->shapeType = isDescriptionComponent
                    ? ShapeType::Box
                    : static_cast<ShapeType>(obj.value("shapeType", static_cast<int>(ct->shapeType)));

                glm::vec2 pos(obj["position"][0].get<float>(), obj["position"][1].get<float>());
                glm::vec2 scale(obj["scale"][0].get<float>(), obj["scale"][1].get<float>());
                glm::vec4 color(obj["color"][0].get<float>(), obj["color"][1].get<float>(), obj["color"][2].get<float>(), obj["color"][3].get<float>());
                float rot = obj.value("rotation", 0.0f);

                node->SetPosition(pos);
                node->SetScale(scale);
                node->SetRotation(rot);
                node->SetBackgroundColor(color);
                if (isDescriptionComponent)
                {
                    node->SetTextureSlot(-1);
                    node->SetSensor(true);
                }
                else
                {
                    node->SetTextureSlot(obj.value("textureSlot", ct->textureSlot));

                    node->physicMass = obj.value("physicMass", ct->physicMass);
                    node->SetPhysicSize(obj.value("physicSize1", ct->physicSize1), obj.value("physicSize2", ct->physicSize2[0]));
                    node->SetFriction(obj.value("friction", ct->physicFriction));
                    node->SetRestitution(obj.value("restitution", ct->physicRestitution));
                    node->SetPhysicRelativePosition(obj["relativePos"][0].get<float>(), obj["relativePos"][1].get<float>());
                    node->SetPhysicRelativeScale(obj["relativeScale"][0].get<float>(), obj["relativeScale"][1].get<float>());
                    node->SetSensor(!obj.value("hasPhysicBody", false));
                }

                if (!isDescriptionComponent && node->GetBody())
                {
                    const int bodyType = obj.value("bodyType", static_cast<int>(cpBodyGetType(node->GetBody())));
                    if (bodyType == static_cast<int>(CP_BODY_TYPE_STATIC))
                        node->SwitchToStatic();
                    else
                        node->SwitchToDynamic();

                    if (obj.contains("velocity"))
                    {
                        glm::vec2 vel(obj["velocity"][0].get<float>(), obj["velocity"][1].get<float>());
                        node->SetVelocity(vel);
                    }
                    if (obj.contains("angularVelocity"))
                    {
                        node->SetAngleVelocity(obj["angularVelocity"].get<float>());
                    }
                }

                idToComponent[id] = node;
                ++createdCount;
                progress = true;
            }
            if (!progress) break;
        }
    }

    PhysicWorld* world = window->GetPhysicWorld();
    if (world && scene.contains("joints") && scene["joints"].is_array())
    {
        for (auto& j : scene["joints"])
        {
            const std::uint64_t aId = j.value("aId", static_cast<std::uint64_t>(0));
            const std::uint64_t bId = j.value("bId", static_cast<std::uint64_t>(0));
            if (!idToComponent.count(aId) || !idToComponent.count(bId))
                continue;

            Component* a = idToComponent[aId];
            Component* b = idToComponent[bId];
            if (!a || !b || !a->GetBody() || !b->GetBody())
                continue;

            const cpVect anchorA = cpv(j["anchorA"][0].get<float>(), j["anchorA"][1].get<float>());
            const cpVect anchorB = cpv(j["anchorB"][0].get<float>(), j["anchorB"][1].get<float>());
            const cpFloat distance = j.value("distance", 0.0f);
            const int type = j.value("type", 0);

            if (type == 1)
                world->AddRod(a->GetBody(), b->GetBody(), anchorA, anchorB, distance);
            else if (type == 2)
                world->AddSpring(
                    a->GetBody(), b->GetBody(),
                    anchorA, anchorB,
                    j.value("distance", distance),
                    j.value("stiffness", 60.0f),
                    j.value("damping", 8.0f));
            else
                world->AddCord(a->GetBody(), b->GetBody(), anchorA, anchorB, distance);
        }
    }

    if (scene.contains("global"))
    {
        const auto& g = scene["global"];
        if (g.contains("clearColor"))
            window->SetClearColor(g["clearColor"][0].get<float>(), g["clearColor"][1].get<float>(), g["clearColor"][2].get<float>(), g["clearColor"][3].get<float>());
        if (g.contains("rootColor"))
            root->SetBackgroundColor(g["rootColor"][0].get<float>(), g["rootColor"][1].get<float>(), g["rootColor"][2].get<float>(), g["rootColor"][3].get<float>());
        if (g.contains("windowScale"))
            window->SetWindowScale(g["windowScale"].get<float>());
        if (g.contains("viewScale"))
            window->SetViewScale(g["viewScale"].get<float>());
        if (g.contains("viewCenter"))
            window->SetViewCenter(glm::vec2(g["viewCenter"][0].get<float>(), g["viewCenter"][1].get<float>()));
        if (g.contains("autoHideEditProperties"))
            window->SetAutoHideEditProperties(g["autoHideEditProperties"].get<bool>());
        if (g.contains("enableConfirmPopup"))
            window->SetEnableConfirmPopup(g["enableConfirmPopup"].get<bool>());
        if (g.contains("jointLineColor"))
            window->SetJointLineColor(glm::vec4(g["jointLineColor"][0].get<float>(), g["jointLineColor"][1].get<float>(), g["jointLineColor"][2].get<float>(), g["jointLineColor"][3].get<float>()));
        if (g.contains("jointLineThickness"))
            window->SetJointLineThickness(g["jointLineThickness"].get<float>());
        if (world)
        {
            if (g.contains("gravity"))
                world->SetGravity(cpv(g["gravity"][0].get<float>(), g["gravity"][1].get<float>()));
            if (g.contains("staticFrictionCoeff"))
                world->SetStaticFrictionCoeff(g["staticFrictionCoeff"].get<float>());
            if (g.contains("kineticFrictionCoeff"))
                world->SetKineticFrictionCoeff(g["kineticFrictionCoeff"].get<float>());
            if (g.contains("slipSpeedThreshold"))
                world->SetSlipSpeedThreshold(g["slipSpeedThreshold"].get<float>());
        }
        if (g.contains("running"))
            window->running = g["running"].get<bool>();
    }

    window->ClearSelection();
    window->ClearBindingAnchor();
}

inline static bool TryLoadChineseFont(ImGuiIO& io, const char* path, float fontSize)
{
    if (_access(path, 0) != 0)
        return false;

    ImFont* font = io.Fonts->AddFontFromFileTTF(path, fontSize, nullptr, io.Fonts->GetGlyphRangesChinese());
    if (!font)
        return false;

    io.FontDefault = font;
    return true;
}

inline static void ConfigureEditorImGuiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(12.0f, 10.0f);
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.93f, 0.95f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.60f, 0.66f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.14f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.18f, 0.75f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.12f, 0.16f, 0.96f);
    colors[ImGuiCol_Border] = ImVec4(0.26f, 0.31f, 0.38f, 0.55f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.36f, 0.44f, 0.55f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.17f, 0.20f, 0.26f, 0.70f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.26f, 0.34f, 0.85f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.31f, 0.40f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.35f, 0.58f, 0.78f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.46f, 0.74f, 0.92f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.33f, 0.56f, 1.00f);

    colors[ImGuiCol_Header] = ImVec4(0.19f, 0.34f, 0.56f, 0.65f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.46f, 0.74f, 0.86f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.39f, 0.64f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.72f, 0.97f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.46f, 0.72f, 0.97f, 0.80f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.57f, 0.80f, 1.00f, 1.00f);
}

inline static void TestInitGui(MyWindow& window, test::TestMenu*& testMenu) {
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window.GetWindow(), false);

    ImGuiIO& io = ImGui::GetIO();
    bool chineseFontLoaded = false;
    chineseFontLoaded = chineseFontLoaded || TryLoadChineseFont(io, "C:/Windows/Fonts/msyh.ttc", 18.0f);
    chineseFontLoaded = chineseFontLoaded || TryLoadChineseFont(io, "C:/Windows/Fonts/msyh.ttf", 18.0f);
    chineseFontLoaded = chineseFontLoaded || TryLoadChineseFont(io, "C:/Windows/Fonts/simhei.ttf", 18.0f);
    chineseFontLoaded = chineseFontLoaded || TryLoadChineseFont(io, "res/fonts/NotoSansCJK-Regular.ttc", 18.0f);
    if (!chineseFontLoaded)
        io.Fonts->AddFontDefault();

    ConfigureEditorImGuiStyle();

    testMenu = new test::TestMenu(&window);
    testMenu->RegisterTest<test::AddNewObject>("Add New Object");
    testMenu->RegisterTest<test::SearchRoot>("RootComponent");
    testMenu->RegisterHiddenTest<test::EditProperties>("Edit Properties");
    testMenu->RegisterHiddenTest<test::GlobalPhysicParams>("Global Physic Params");
    testMenu->RegisterTest<test::Setting>("Setting");
}

struct PhysicsPanelState
{
    bool timerEnabled = true;
    double physicsRunTimer = 0.0;
    float countdownSetSeconds = 10.0f;
    float countdownRemainingSeconds = 0.0f;
    bool countdownActive = false;
    char sceneNameBuffer[64] = "runtime_scene";
    int selectedSceneIndex = -1;
    bool deleteScenePopupRequested = false;
    std::string pendingDeleteSceneName;
};

class TestGuiUtils
{
public:
    static void ClampWindowsToViewport()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 display = io.DisplaySize;
        for (int i = 0; i < ImGui::GetCurrentContext()->Windows.Size; ++i)
        {
            ImGuiWindow* w = ImGui::GetCurrentContext()->Windows[i];
            if (!w)
                continue;

            const float maxX = display.x - w->SizeFull.x - style.DisplayWindowPadding.x;
            const float maxY = display.y - w->SizeFull.y - style.DisplayWindowPadding.y;
            ImVec2 clamped(
                w->Pos.x < style.DisplayWindowPadding.x ? style.DisplayWindowPadding.x : (w->Pos.x > maxX ? maxX : w->Pos.x),
                w->Pos.y < style.DisplayWindowPadding.y ? style.DisplayWindowPadding.y : (w->Pos.y > maxY ? maxY : w->Pos.y)
            );
            if (clamped.x != w->Pos.x || clamped.y != w->Pos.y)
                ImGui::SetWindowPos(w->Name, clamped, ImGuiCond_Always);
        }
    }

    static void RenderViewScaleOverlay(MyWindow* window)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 12.0f, 12.0f), 0, ImVec2(1.0f, 0.0f));
        ImGui::Begin("##ViewScaleOverlay", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove);
        char scaleLabel[64] = {};
        snprintf(scaleLabel, sizeof(scaleLabel), "Scale: %.2fx", window->GetViewScale());
        if (ImGui::SmallButton(scaleLabel))
        {
            window->ResetView();
        }
        ImGui::End();
    }

    static void UpdatePhysicsState(MyWindow* window, PhysicsPanelState& state)
    {
        if (state.timerEnabled && window->running)
        {
            state.physicsRunTimer += ImGui::GetIO().DeltaTime;
        }

        if (state.countdownActive && window->running)
        {
            state.countdownRemainingSeconds -= ImGui::GetIO().DeltaTime;
            if (state.countdownRemainingSeconds <= 0.0f)
            {
                state.countdownRemainingSeconds = 0.0f;
                state.countdownActive = false;
                window->running = false;
            }
        }
    }

    static void RenderPhysicsPanel(MyWindow* window, PhysicsPanelState& state)
    {
        PhysicWorld* world = window ? window->GetPhysicWorld() : nullptr;

        ImGui::Text("Runtime");
        if (window->running)
        {
            if (ImGui::Button("Stop Physics"))
            {
                window->running = false;
            }
        }
        else
        {
            if (ImGui::Button("Run Physics"))
            {
                window->running = true;
            }
        }
        ImGui::SameLine();
        if (!window->running && ImGui::Button("Step"))
        {
            window->GetPhysicWorld()->Step(1.0 / 60.0);
        }

        ImGui::Separator();
        ImGui::Text("Global Common");
        if (world)
        {
            cpVect gravity = world->GetGravity();
            float gravityValue[2] = { static_cast<float>(gravity.x), static_cast<float>(gravity.y) };
            if (ImGui::DragFloat2("Gravity", gravityValue, 0.1f, -100.0f, 100.0f))
            {
                world->SetGravity(cpv(gravityValue[0], gravityValue[1]));
            }
        }

        Component* anchorA = window->GetBindingAnchor();
        const char* anchorName = (!anchorA) ? "<none>" : (anchorA->name.empty() ? "<unnamed>" : anchorA->name.c_str());
        ImGui::Text("Joint A: %s", anchorName);
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear"))
        {
            window->ClearBindingAnchor();
        }

        ImGui::Separator();
        ImGui::Text("Timer: %.2f s", state.physicsRunTimer);
        ImGui::Checkbox("Enable Timer", &state.timerEnabled);
        ImGui::SameLine();
        if (ImGui::Button("Reset Timer"))
        {
            state.physicsRunTimer = 0.0;
        }

        ImGui::Text("Countdown");
        if (window->GetSelectedComponent() == nullptr)
        {
            if (ImGui::DragFloat("Set Countdown (s)", &state.countdownSetSeconds, 0.1f, 0.1f, 36000.0f, "%.1f"))
            {
                if (state.countdownSetSeconds < 0.1f) state.countdownSetSeconds = 0.1f;
            }
            ImGui::Text("Remaining: %.2f s", state.countdownActive ? state.countdownRemainingSeconds : state.countdownSetSeconds);
            if (!state.countdownActive)
            {
                if (ImGui::Button("Start Countdown"))
                {
                    state.countdownRemainingSeconds = state.countdownSetSeconds;
                    state.countdownActive = true;
                }
            }
            else
            {
                if (ImGui::Button("Stop Countdown"))
                {
                    state.countdownActive = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Countdown"))
            {
                state.countdownActive = false;
                state.countdownRemainingSeconds = state.countdownSetSeconds;
            }
        }
        else
        {
            ImGui::TextDisabled("Countdown setup available only when no object is selected.");
            ImGui::Text("Remaining: %.2f s", state.countdownActive ? state.countdownRemainingSeconds : state.countdownSetSeconds);
        }

        std::vector<std::string> sceneNames = GetSceneNames();
        if (state.selectedSceneIndex >= static_cast<int>(sceneNames.size()))
            state.selectedSceneIndex = sceneNames.empty() ? -1 : 0;

        ImGui::Separator();
        ImGui::Text("Scene");
        ImGui::InputText("Scene Name", state.sceneNameBuffer, IM_ARRAYSIZE(state.sceneNameBuffer));
        const std::string sceneName = state.sceneNameBuffer;
        const bool validSceneName = IsValidSceneName(sceneName);
        if (!validSceneName)
        {
            ImGui::TextDisabled("Scene name: 1-64 chars, only [A-Z a-z 0-9 _ -]");
        }

        if (ImGui::Button("Save Scene"))
        {
            if (validSceneName)
            {
                SaveScene(window, BuildScenePath(sceneName));
                sceneNames = GetSceneNames();
                for (int i = 0; i < static_cast<int>(sceneNames.size()); ++i)
                {
                    if (sceneNames[i] == sceneName)
                    {
                        state.selectedSceneIndex = i;
                        break;
                    }
                }
            }
        }

        const char* currentScenePreview = (state.selectedSceneIndex >= 0 && state.selectedSceneIndex < static_cast<int>(sceneNames.size()))
            ? sceneNames[state.selectedSceneIndex].c_str()
            : "<none>";

        if (ImGui::BeginCombo("Load Scene", currentScenePreview))
        {
            for (int i = 0; i < static_cast<int>(sceneNames.size()); ++i)
            {
                const bool isSelected = (i == state.selectedSceneIndex);
                if (ImGui::Selectable(sceneNames[i].c_str(), isSelected))
                    state.selectedSceneIndex = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (!window->running && state.selectedSceneIndex >= 0 && state.selectedSceneIndex < static_cast<int>(sceneNames.size()))
        {
            if (ImGui::Button("Load Selected Scene"))
            {
                LoadScene(window, ResolveScenePathForLoad(sceneNames[state.selectedSceneIndex]));
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Selected Scene"))
            {
                const std::string selectedName = sceneNames[state.selectedSceneIndex];
                if (window->GetEnableConfirmPopup())
                {
                    state.pendingDeleteSceneName = selectedName;
                    state.deleteScenePopupRequested = true;
                }
                else
                {
                    DeleteSceneByName(selectedName);
                    sceneNames = GetSceneNames();
                    state.selectedSceneIndex = sceneNames.empty() ? -1 : 0;
                }
            }
        }

        if (window->GetEnableConfirmPopup())
        {
            const int sceneDeleteConfirm = test::RenderCenteredConfirmModal(
                "##DeleteSceneConfirm",
                "Confirm delete selected scene?",
                state.deleteScenePopupRequested);
            if (sceneDeleteConfirm == 1 && !state.pendingDeleteSceneName.empty())
            {
                DeleteSceneByName(state.pendingDeleteSceneName);
                state.pendingDeleteSceneName.clear();
                sceneNames = GetSceneNames();
                state.selectedSceneIndex = sceneNames.empty() ? -1 : 0;
            }
            else if (sceneDeleteConfirm == -1)
            {
                state.pendingDeleteSceneName.clear();
            }
        }
    }

    static void RenderPhysicsPanelAdapter(MyWindow* window, void* userData)
    {
        if (!userData)
            return;
        RenderPhysicsPanel(window, *static_cast<PhysicsPanelState*>(userData));
    }
};

inline static void TestGui(const void* testObject, test::TestMenu* testMenu) {
    ImGui_ImplGlfwGL3_NewFrame();
	MyWindow* window = testMenu->window;

    static PhysicsPanelState physicsPanelState;
    TestGuiUtils::ClampWindowsToViewport();
    TestGuiUtils::RenderViewScaleOverlay(window);
    TestGuiUtils::UpdatePhysicsState(window, physicsPanelState);

    if (!window->running)
    {
        window->DrawColliderOutline();
    }
    window->DrawJointBindings();
    window->DrawDescriptionLabels();

    if (window->HasSelectionChanged())
    {
        window->AcknowledgeSelectionChanged();
        if (window->GetSelectedComponent())
        {
            testMenu->OpenWindow("Edit Properties");
        }
        else if (window->GetAutoHideEditProperties())
        {
            testMenu->CloseWindow("Edit Properties");
        }
    }

    testMenu->SetRootWindowExtraRenderer(&TestGuiUtils::RenderPhysicsPanelAdapter, &physicsPanelState);
    testMenu->OnImGuiRender(window);

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}
int main(void)
{
	RandomDevice randomDevice;
	int Width = 1080, Height = 720;
    {
        MyWindow* window = nullptr;
        Renderer* renderer = nullptr;
        PhysicWorld* physicWorld = nullptr;

        window = new MyWindow(Width, Height, "Hello World");
        renderer = window->GetRenderer();
        physicWorld = window->GetPhysicWorld();

        Sources::Instance()->LoadImages("res/textures/ball.png", renderer); // 预加载纹理: 0
        Sources::Instance()->LoadImages("res/textures/car.png", renderer); // 预加载纹理: 1
        Sources::Instance()->LoadImages("res/textures/circle.png", renderer); // 预加载纹理: 2
        Sources::Instance()->LoadImages("res/textures/face_faint.png", renderer); // 预加载纹理: 3
        Sources::Instance()->LoadImages("res/textures/face_sad.png", renderer); // 预加载纹理: 4
        Sources::Instance()->LoadImages("res/textures/planet.png", renderer); // 预加载纹理: 5
        Sources::Instance()->LoadComponentFromJson("res/Components/default.json");
        window->SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        renderer->SendToGPU();
        test::TestMenu* testMenu = nullptr;
        TestInitGui(*window, testMenu);

        double deltaTime;
        while (window->Loop(deltaTime))
        {
            DEBUG_RUN(TestGui(nullptr, testMenu));
            window->LoopEnd();
        }
        delete testMenu;
        ImGui_ImplGlfwGL3_Shutdown();
        ImGui::DestroyContext();
		window->LoopEnd();
    }
    return 0;
}