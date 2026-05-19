#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// Ready for future battle tests: a high-frequency tick generator
class MsTickGenerator : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        static uint64_t counter = 0;
        out["tick"] = static_cast<double>(++counter);
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Generic, "trigger", QVariant(0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "tick", QVariant()}};
    }

    QString nodeType() const override { return "system/time/msTick"; }
    QString nodeName() const override { return "MS Tick"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "Time"; }
    QString displayColor() const override { return "#6C5CE7"; }
};

inline void registerRandomValueNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"System", "System", QColor("#6C5CE7")});
    registerNodeType<MsTickGenerator>(model, "System");
}

} // namespace NodeEditor
