#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

class BeginNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/begin"; }
    QString nodeName() const override { return "Begin"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "started", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"started", true}};
    }
};

class TickNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/tick"; }
    QString nodeName() const override { return "Tick"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "deltaTime", QVariant()}, {PortType::Float, "elapsed", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"deltaTime", 0.016}, {"elapsed", 0.0}};
    }
};

class TimerEventNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/timerEvent"; }
    QString nodeName() const override { return "Timer Event"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "interval", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "fired", QVariant()}, {PortType::Float, "count", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"fired", true}, {"count", 1.0}};
    }
};

class InputEventNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/inputEvent"; }
    QString nodeName() const override { return "Input Event"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "eventType", QVariant("keyPress")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "triggered", QVariant()}, {PortType::String, "data", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"triggered", false}, {"data", QString()}};
    }
};

class SignalEventNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/signalEvent"; }
    QString nodeName() const override { return "Signal Event"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "signal", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "received", QVariant()}, {PortType::Generic, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"received", false}, {"value", QVariant()}};
    }
};

class PropertyChangedNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "events/core/propertyChanged"; }
    QString nodeName() const override { return "Property Changed"; }
    QString nodeCategory() const override { return "Events"; }
    QString nodeSubCategory() const override { return "Core"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "property", QVariant("")}, {PortType::Generic, "value", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "changed", QVariant()}, {PortType::Generic, "newValue", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"changed", true}, {"newValue", inputs.value("value")}};
    }
};

inline void registerEventNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Events", "Events", QColor("#FDCB6E")});
    registerNodeType<BeginNode>(model, "Events");
    registerNodeType<TickNode>(model, "Events");
    registerNodeType<TimerEventNode>(model, "Events");
    registerNodeType<InputEventNode>(model, "Events");
    registerNodeType<SignalEventNode>(model, "Events");
    registerNodeType<PropertyChangedNode>(model, "Events");
}

} // namespace NodeEditor
