#pragma once
#include "Utils.h"
#include "../MyWindow.h"

enum class LayoutArrangeMode
{
    Auto,          // 自动排列
    SingleRowList, // 单排，从左到右
    MultiRowGrid   // 多排列表，类似网格
};

class Layouts : public Component
{
public:
    Layouts(Mesh* mesh)
        : Component(Sources::Instance()->GetMesh("box")),
          arrangeMode(LayoutArrangeMode::Auto),
          itemWidth(100.0f),
          itemHeight(30.0f),
          columns(1),
          paddingX(5.0f),
          paddingY(5.0f)
    {}

    void AddLayout(Layouts* layout) { AddChild(layout); childrenLayouts.push_back(layout); UpdateChildrenLayout(); }
    void RemoveLayout(Layouts* layout)
    {
        childrenLayouts.erase(std::find(childrenLayouts.begin(), childrenLayouts.end(), layout));
        RemoveChild(layout);
        UpdateChildrenLayout();
    }
    void SetArrangeMode(LayoutArrangeMode mode) { arrangeMode = mode; UpdateChildrenLayout(); }
    void SetItemSize(float w, float h) { itemWidth = w; itemHeight = h; UpdateChildrenLayout(); }
    void SetColumns(int cols) { columns = cols > 0 ? cols : 1; UpdateChildrenLayout(); }
    void SetPadding(float x, float y) { paddingX = x; paddingY = y; UpdateChildrenLayout(); }

protected:
    void UpdateChildrenLayout()
    {
        switch (arrangeMode)
        {
        case LayoutArrangeMode::Auto:
            break;

        case LayoutArrangeMode::SingleRowList:
        {
            float x = 0.0f;
            for (auto* c : childrenLayouts)
            {
                c->SetPosition(x, 0.0f);
                c->SetWorldScale(itemWidth, itemHeight);
                x += itemWidth + paddingX;
            }
            break;
        }

        case LayoutArrangeMode::MultiRowGrid:
        {
            int idx = 0;
            for (auto* c : childrenLayouts)
            {
                int col = idx % columns;
                int row = idx / columns;

                float x = col * (itemWidth + paddingX);
                float y = -row * (itemHeight + paddingY); // Y 轴方向按你的坐标系调整

                c->SetPosition(x, y);
                c->SetWorldScale(itemWidth, itemHeight);
                ++idx;
            }
            break;
        }
        }
    }

private:
    std::vector<Layouts*> childrenLayouts;

    LayoutArrangeMode arrangeMode;
    float itemWidth;
    float itemHeight;
    int   columns;
    float paddingX;
    float paddingY;
};