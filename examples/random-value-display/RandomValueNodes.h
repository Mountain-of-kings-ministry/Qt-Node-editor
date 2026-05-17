#pragma once

#include "NodeEditor/BaseNode.h"
#include <random>
#include <QDateTime>

namespace NodeEditor {

class MsTickGenerator : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        uint64_t now = QDateTime::currentMSecsSinceEpoch();
        out["tick"] = static_cast<double>(now);
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

class RandomRangeNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double minVal = inputs.value("min", 0.0).toDouble();
        double maxVal = inputs.value("max", 1.0).toDouble();
        if (maxVal < minVal) std::swap(minVal, maxVal);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(minVal, maxVal);
        out["value"] = dist(gen);
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "min", QVariant(0.0)},
                {PortType::Float, "max", QVariant(1.0)},
                {PortType::Float, "trigger", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "value", QVariant()}};
    }

    QString nodeType() const override { return "utility/random/range"; }
    QString nodeName() const override { return "Random Range"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Random"; }
    QString displayColor() const override { return "#00B894"; }
};

inline void registerRandomValueNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"System", "System", QColor("#6C5CE7")});
    model->registerCategory({"Utility", "Utility", QColor("#00B894")});
    registerNodeType<MsTickGenerator>(model, "System");
    registerNodeType<RandomRangeNode>(model, "Utility");
}

} // namespace NodeEditor
