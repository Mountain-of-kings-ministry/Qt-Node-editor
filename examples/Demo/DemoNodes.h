#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// InputNode: no inputs, one Output "value"
class InputNode : public BaseNode {
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

    QString nodeType() const override { return "Input"; }
    QString nodeName() const override { return "Input"; }
    QString nodeCategory() const override { return "Input"; }
    QString nodeSubCategory() const override { return "Input"; }
    QString displayColor() const override { return "#4CDF8B"; }
};

// AddNode: inputs a+b, output result
class AddNode : public BaseNode {
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

    QString nodeType() const override { return "Add"; }
    QString nodeName() const override { return "Add"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
};

// MultiplyNode: inputs a*b, output result
class MultiplyNode : public BaseNode {
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

    QString nodeType() const override { return "Multiply"; }
    QString nodeName() const override { return "Multiply"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#4A9EFF"; }
};

// OutputNode: one input "value", displays/sinks it
class OutputNode : public BaseNode {
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

    QString nodeType() const override { return "Output"; }
    QString nodeName() const override { return "Output"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
};

} // namespace NodeEditor
