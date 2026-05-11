#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// QObject Nodes
// ══════════════════════════════════════════════════════════

class ConnectSignalNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qobject/connectSignal"; }
    QString nodeName() const override { return "Connect Signal"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QObject"; }
    QString displayColor() const override { return "#4A9EFF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "sender", QVariant("")},
                {PortType::String, "signal", QVariant("")},
                {PortType::String, "receiver", QVariant("")},
                {PortType::String, "slot", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "connected", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"connected", false}};
    }
};

class EmitSignalNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qobject/emitSignal"; }
    QString nodeName() const override { return "Emit Signal"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QObject"; }
    QString displayColor() const override { return "#4A9EFF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "signal", QVariant("")}, {PortType::Generic, "value", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "emitted", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"emitted", true}};
    }
};

class SetPropertyNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qobject/setProperty"; }
    QString nodeName() const override { return "Set Property"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QObject"; }
    QString displayColor() const override { return "#4A9EFF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "object", QVariant("")},
                {PortType::String, "property", QVariant("")},
                {PortType::Generic, "value", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "success", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"success", false}};
    }
};

class GetPropertyNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qobject/getProperty"; }
    QString nodeName() const override { return "Get Property"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QObject"; }
    QString displayColor() const override { return "#4A9EFF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "object", QVariant("")}, {PortType::String, "property", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"value", QVariant()}};
    }
};

class InvokeMethodNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qobject/invokeMethod"; }
    QString nodeName() const override { return "Invoke Method"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QObject"; }
    QString displayColor() const override { return "#4A9EFF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "object", QVariant("")},
                {PortType::String, "method", QVariant("")},
                {PortType::Generic, "arg", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"result", QVariant()}};
    }
};

// ══════════════════════════════════════════════════════════
// Widgets
// ══════════════════════════════════════════════════════════

class ButtonClickedNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/widgets/buttonClicked"; }
    QString nodeName() const override { return "Button Clicked"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "Widgets"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "text", QVariant("Click Me")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "clicked", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"clicked", false}};
    }
};

class SliderValueNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/widgets/sliderValue"; }
    QString nodeName() const override { return "Slider Value"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "Widgets"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "min", QVariant(0.0)}, {PortType::Float, "max", QVariant(100.0)}, {PortType::Float, "value", QVariant(50.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"value", inputs.value("value")}};
    }
};

class TextChangedNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/widgets/textChanged"; }
    QString nodeName() const override { return "Text Changed"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "Widgets"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "text", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "text", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"text", inputs.value("text")}};
    }
};

// ══════════════════════════════════════════════════════════
// QML
// ══════════════════════════════════════════════════════════

class QMLPropertyNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qml/qmlProperty"; }
    QString nodeName() const override { return "QML Property"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QML"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "objectId", QVariant("")},
                {PortType::String, "property", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"value", QVariant()}};
    }
};

class QMLSignalNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "qt/qml/qmlSignal"; }
    QString nodeName() const override { return "QML Signal"; }
    QString nodeCategory() const override { return "Qt"; }
    QString nodeSubCategory() const override { return "QML"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "objectId", QVariant("")}, {PortType::String, "signal", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"value", QVariant()}};
    }
};

} // namespace NodeEditor
