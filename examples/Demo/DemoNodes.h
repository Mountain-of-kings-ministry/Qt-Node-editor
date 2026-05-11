#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// DemoInputNode: no inputs, one Output "value"
class DemoInputNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        out["value"] = inputs.value("input", 0.0);
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "input", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "value", QVariant()}};
    }

    QString nodeType() const override { return "demo/input"; }
    QString nodeName() const override { return "Demo Input"; }
    QString nodeCategory() const override { return "Demo"; }
    QString nodeSubCategory() const override { return "Values"; }
    QString displayColor() const override { return "#4CDF8B"; }
};

// DemoAddNode: inputs a+b, output result
class DemoAddNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double a = inputs.value("a", 0.0).toDouble();
        double b = inputs.value("b", 0.0).toDouble();
        out["result"] = a + b;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "a", QVariant(0.0)},
                {PortType::Float, "b", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "result", QVariant()}};
    }

    QString nodeType() const override { return "demo/add"; }
    QString nodeName() const override { return "Demo Add"; }
    QString nodeCategory() const override { return "Demo"; }
    QString nodeSubCategory() const override { return "Math"; }
    QString displayColor() const override { return "#FF9F43"; }
};

// DemoMultiplyNode: inputs a*b, output result
class DemoMultiplyNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double a = inputs.value("a", 1.0).toDouble();
        double b = inputs.value("b", 1.0).toDouble();
        out["result"] = a * b;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "a", QVariant(1.0)},
                {PortType::Float, "b", QVariant(1.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "result", QVariant()}};
    }

    QString nodeType() const override { return "demo/multiply"; }
    QString nodeName() const override { return "Demo Multiply"; }
    QString nodeCategory() const override { return "Demo"; }
    QString nodeSubCategory() const override { return "Math"; }
    QString displayColor() const override { return "#4A9EFF"; }
};

// DemoOutputNode: one input "value", displays/sinks it
class DemoOutputNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double val = inputs.value("value", 0.0).toDouble();
        qDebug() << "Output computed:" << val;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {};
    }

    QString nodeType() const override { return "demo/output"; }
    QString nodeName() const override { return "Demo Output"; }
    QString nodeCategory() const override { return "Demo"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
};

} // namespace NodeEditor