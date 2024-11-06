// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Widgets.hpp"

#include <imgui_internal.h>

FLOW_UI_SUBNAMESPACE_START(widgets)

void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color, const ImVec4& innerColor)
{
    if (ImGui::IsRectVisible(size))
    {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList  = ImGui::GetWindowDrawList();
        DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
    }

    ImGui::Dummy(size);
}

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32)
{
    auto rect                 = ImRect(a, b);
    auto rect_x               = rect.Min.x;
    auto rect_y               = rect.Min.y;
    auto rect_w               = rect.Max.x - rect.Min.x;
    auto rect_h               = rect.Max.y - rect.Min.y;
    auto rect_center_x        = (rect.Min.x + rect.Max.x) * 0.5f;
    auto rect_center_y        = (rect.Min.y + rect.Max.y) * 0.5f;
    auto rect_center          = ImVec2(rect_center_x, rect_center_y);
    const auto outline_scale  = rect_w / 24.0f;
    const auto extra_segments = static_cast<int>(10 * outline_scale); // for full circle

    auto triangleStart = rect_center_x + 0.32f * rect_w;

    auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

    rect.Min.x += rect_offset;
    rect.Max.x += rect_offset;
    rect_x += rect_offset;
    rect_center_x += rect_offset * 0.5f;
    rect_center.x += rect_offset * 0.5f;

    if (type == IconType::Circle)
    {
        const auto c = rect_center;
        const auto r = 0.5f * rect_w / 2.0f;

        if (filled) drawList->AddCircleFilled(c, r / 2, color, 12 + extra_segments);
        drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
    }

    if (type == IconType::Square)
    {
        const auto r  = 0.5f * rect_w / 2.0f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        drawList->AddRect(p0, p1, color, 0, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);

        if (filled)
        {
            const auto ip0 = rect_center - ImVec2(r / 2, r / 2);
            const auto ip1 = rect_center + ImVec2(r / 2, r / 2);

            drawList->AddRectFilled(ip0, ip1, color, 0, ImDrawFlags_RoundCornersAll);
        }
    }

    if (type == IconType::Grid)
    {
        const auto r = 0.5f * rect_w / 2.0f;
        const auto w = ceilf(r / 3.0f);

        const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
        const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

        auto tl = baseTl;
        auto br = baseBr;
        for (int i = 0; i < 3; ++i)
        {
            tl.x = baseTl.x;
            br.x = baseBr.x;
            drawList->AddRectFilled(tl, br, color);
            tl.x += w * 2;
            br.x += w * 2;
            if (i != 1 || filled) drawList->AddRectFilled(tl, br, color);
            tl.x += w * 2;
            br.x += w * 2;
            drawList->AddRectFilled(tl, br, color);

            tl.y += w * 2;
            br.y += w * 2;
        }

        triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
    }

    if (type == IconType::RoundSquare)
    {
        const auto r  = 0.5f * rect_w / 2.0f;
        const auto cr = r * 0.5f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        drawList->AddRect(p0, p1, color, cr, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
        if (filled)
        {
            const auto icr = (r / 2.f) * 0.5f;
            const auto ip0 = rect_center - ImVec2(r / 2, r / 2);
            const auto ip1 = rect_center + ImVec2(r / 2, r / 2);

            drawList->AddRectFilled(ip0, ip1, color, icr, ImDrawFlags_RoundCornersAll);
        }
    }
    else if (type == IconType::Diamond)
    {
        const auto r = 0.607f * rect_w / 2.0f - 0.5f;
        const auto c = rect_center;

        drawList->PathLineTo(c + ImVec2(0, -r));
        drawList->PathLineTo(c + ImVec2(r, 0));
        drawList->PathLineTo(c + ImVec2(0, r));
        drawList->PathLineTo(c + ImVec2(-r, 0));

        drawList->PathStroke(color, true, 2.0f * outline_scale);

        if (filled)
        {
            const auto ir = 0.607f * rect_w / 4.0f;
            const auto ic = rect_center;

            drawList->PathLineTo(ic + ImVec2(0, -ir));
            drawList->PathLineTo(ic + ImVec2(ir, 0));
            drawList->PathLineTo(ic + ImVec2(0, ir));
            drawList->PathLineTo(ic + ImVec2(-ir, 0));
            drawList->PathFillConvex(color);
        }
    }

    const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

    drawList->AddTriangleFilled(ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
                                ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                                ImVec2(triangleStart, rect_center_y - 0.15f * rect_h), color);
}

FLOW_UI_SUBNAMESPACE_END
