#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// ScrollingTextNode: takes a string input and outputs LED matrix row data
class ScrollingTextNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        QString text = inputs.value("text").toString();
        int offset = inputs.value("scrollOffset").toInt();
        int width = std::clamp(inputs.value("width").toInt(), 1, 64);

        // Pad text with spaces for scrolling
        QString padded = QString(width, ' ') + text + QString(width, ' ');
        int idx = offset % padded.size();
        QString visible = padded.mid(idx, width);
        if (visible.size() < width)
            visible = visible.leftJustified(width);

        out["display"] = visible;
        out["nextOffset"] = offset + 1;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::String, "text", QVariant("")},
                {PortType::Int, "width", QVariant(16)},
                {PortType::Int, "scrollOffset", QVariant(0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::String, "display", QVariant()},
                {PortType::Int, "nextOffset", QVariant()}};
    }

    QString nodeType() const override { return "timeDisplay/scrollingText"; }
    QString nodeName() const override { return "Scrolling Text"; }
    QString nodeCategory() const override { return "Display"; }
    QString nodeSubCategory() const override { return "LED Matrix"; }
    QString displayColor() const override { return "#00CEC9"; }
};

inline void registerTimeDisplayNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Display", "Display", QColor("#00CEC9")});
    registerNodeType<ScrollingTextNode>(model, "Display");
}

} // namespace NodeEditor
