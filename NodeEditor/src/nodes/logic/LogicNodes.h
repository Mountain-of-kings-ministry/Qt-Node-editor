#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

class ANDNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/basic/and"; }
    QString nodeName() const override { return "AND"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Basic"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "a", QVariant(false)}, {PortType::Bool, "b", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toBool() && inputs.value("b").toBool()}};
    }
};

class ORNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/basic/or"; }
    QString nodeName() const override { return "OR"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Basic"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "a", QVariant(false)}, {PortType::Bool, "b", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toBool() || inputs.value("b").toBool()}};
    }
};

class XORNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/basic/xor"; }
    QString nodeName() const override { return "XOR"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Basic"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "a", QVariant(false)}, {PortType::Bool, "b", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        bool a = inputs.value("a").toBool(), b = inputs.value("b").toBool();
        return {{"result", a != b}};
    }
};

class NOTNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/basic/not"; }
    QString nodeName() const override { return "NOT"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Basic"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "value", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", !inputs.value("value").toBool()}};
    }
};

class BooleanCompareNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/compare/booleanCompare"; }
    QString nodeName() const override { return "Boolean Compare"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Compare"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "a", QVariant(false)}, {PortType::Bool, "b", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "equal", QVariant()}, {PortType::Bool, "notEqual", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        bool eq = inputs.value("a").toBool() == inputs.value("b").toBool();
        return {{"equal", eq}, {"notEqual", !eq}};
    }
};

class IsTrueNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/check/isTrue"; }
    QString nodeName() const override { return "Is True"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Check"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "value", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("value").toBool()}};
    }
};

class IsFalseNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "logic/check/isFalse"; }
    QString nodeName() const override { return "Is False"; }
    QString nodeCategory() const override { return "Logic"; }
    QString nodeSubCategory() const override { return "Check"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "value", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", !inputs.value("value").toBool()}};
    }
};

} // namespace NodeEditor
