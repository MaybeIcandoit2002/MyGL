#pragma once
#include <vector>
#include <functional>
#include "Component.h"

struct Target
{
    Component* object = nullptr;
    std::function<void(Component*)> onMouseDown;
    std::function<void(Component*)> onMouseMove;
    std::function<void(Component*)> onMouseUp;
};

/// <summary>
/// 触摸事件分发器，负责管理所有注册的触摸事件目标，并在鼠标事件发生时调用相应的回调函数。
/// 触摸事件目标必须是 Component 的实例，并且需要满足以下条件才能被触发：
/// 1. 组件必须启用（enabled == true）。
/// 2. 组件必须具有物理碰撞体（hasPhysicBody == true）。
/// 3. 鼠标点击位置必须在组件的物理形状内（checkPointInShape(x, y) == true）。
/// 触摸事件按照注册顺序进行分发，先注册的目标优先响应事件。
/// </summary>
class TouchDispatcher
{
public:

    static TouchDispatcher& Instance()
    {
        static TouchDispatcher inst;
        return inst;
    }

    void RegisterTarget(
        Component* object,
        std::function<void(Component*)> onDown,
        std::function<void(Component*)> onMove,
        std::function<void(Component*)> onUp)
    {
        Target t;
        t.object      = object;
        t.onMouseDown = std::move(onDown);
        t.onMouseMove = std::move(onMove);
        t.onMouseUp   = std::move(onUp);
        targets.push_back(std::move(t));
    }

    void UnregisterTarget(Component* object)
    {
        targets.erase(
            std::remove_if(targets.begin(), targets.end(),
                           [object](const Target& t) { return t.object == object; }),
            targets.end());
    }

    void OnMouseDown(float x, float y)
    {
        mouseDownCallbacks.clear();
        mouseMoveCallbacks.clear();
        mouseUpCallbacks.clear();

        downPositionX = x;
        downPositionY = y;

        for (auto& t : targets)
        {
            if (!t.object ||
                !t.object->enabled ||
                !t.object->hasPhysicBody ||
				!t.object->checkPointInShape(x, y))
                continue;

            if (t.onMouseDown)
                mouseDownCallbacks.emplace_back([&t]() { t.onMouseDown(t.object); });
            if (t.onMouseMove)
                mouseMoveCallbacks.emplace_back([&t]() { t.onMouseMove(t.object); });
            if (t.onMouseUp)
                mouseUpCallbacks.emplace_back([&t]() { t.onMouseUp(t.object); });
        }

        for (auto& cb : mouseDownCallbacks)
        {
            cb();
        }
    }

    void OnMouseMove(float x, float y)
    {
        touchX = x;
        touchY = y;
        for (auto& cb : mouseMoveCallbacks)
        {
            cb();
        }
    }

    void OnMouseUp(float x, float y)
    {
		touchX = x;
		touchY = y;
        for (auto& cb : mouseUpCallbacks)
        {
            cb();
        }

        mouseDownCallbacks.clear();
        mouseMoveCallbacks.clear();
        mouseUpCallbacks.clear();
    }

private:
    TouchDispatcher() = default;

    std::vector<Target> targets;

    std::vector<std::function<void()>> mouseDownCallbacks;
    std::vector<std::function<void()>> mouseMoveCallbacks;
    std::vector<std::function<void()>> mouseUpCallbacks;

    float downPositionX = 0.0f;
    float downPositionY = 0.0f;
	float touchX = 0.0f;
	float touchY = 0.0f;
};
