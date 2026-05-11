#pragma once

#include "NodeEditor/BaseNode.h"
#include <QtMath>
#include <QRandomGenerator>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// Basic Operations
// ══════════════════════════════════════════════════════════

class AddNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/add"; }
    QString nodeName() const override { return "Add"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toDouble() + inputs.value("b").toDouble()}};
    }
};

class SubtractNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/subtract"; }
    QString nodeName() const override { return "Subtract"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toDouble() - inputs.value("b").toDouble()}};
    }
};

class MultiplyNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/multiply"; }
    QString nodeName() const override { return "Multiply"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(1.0)}, {PortType::Float, "b", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toDouble() * inputs.value("b").toDouble()}};
    }
};

class DivideNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/divide"; }
    QString nodeName() const override { return "Divide"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double b = inputs.value("b").toDouble();
        return {{"result", qFuzzyIsNull(b) ? 0.0 : inputs.value("a").toDouble() / b}};
    }
};

class ModuloNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/modulo"; }
    QString nodeName() const override { return "Modulo"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double b = inputs.value("b").toDouble();
        return {{"result", qFuzzyIsNull(b) ? 0.0 : std::fmod(inputs.value("a").toDouble(), b)}};
    }
};

class PowerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/power"; }
    QString nodeName() const override { return "Power"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "base", QVariant(1.0)}, {PortType::Float, "exponent", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::pow(inputs.value("base").toDouble(), inputs.value("exponent").toDouble())}};
    }
};

class SquareRootNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/squareRoot"; }
    QString nodeName() const override { return "Square Root"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::sqrt(std::max(0.0, inputs.value("value").toDouble()))}};
    }
};

class AbsoluteNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/absolute"; }
    QString nodeName() const override { return "Absolute"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::abs(inputs.value("value").toDouble())}};
    }
};

class NegateNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/basic/negate"; }
    QString nodeName() const override { return "Negate"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Basic Operations"; }
    QString displayColor() const override { return "#FF9F43"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", -inputs.value("value").toDouble()}};
    }
};

// ══════════════════════════════════════════════════════════
// Comparison
// ══════════════════════════════════════════════════════════

class EqualNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/equal"; }
    QString nodeName() const override { return "Equal"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", qFuzzyCompare(inputs.value("a").toDouble(), inputs.value("b").toDouble())}};
    }
};

class NotEqualNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/notEqual"; }
    QString nodeName() const override { return "Not Equal"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", !qFuzzyCompare(inputs.value("a").toDouble(), inputs.value("b").toDouble())}};
    }
};

class GreaterThanNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/greaterThan"; }
    QString nodeName() const override { return "Greater Than"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toDouble() > inputs.value("b").toDouble()}};
    }
};

class LessThanNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/lessThan"; }
    QString nodeName() const override { return "Less Than"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toDouble() < inputs.value("b").toDouble()}};
    }
};

class ClampNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/clamp"; }
    QString nodeName() const override { return "Clamp"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)},
                {PortType::Float, "min", QVariant(0.0)},
                {PortType::Float, "max", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = inputs.value("value").toDouble();
        double mn = inputs.value("min").toDouble();
        double mx = inputs.value("max").toDouble();
        return {{"result", std::max(mn, std::min(mx, v))}};
    }
};

class MinNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/min"; }
    QString nodeName() const override { return "Min"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::min(inputs.value("a").toDouble(), inputs.value("b").toDouble())}};
    }
};

class MaxNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/compare/max"; }
    QString nodeName() const override { return "Max"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Comparison"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::max(inputs.value("a").toDouble(), inputs.value("b").toDouble())}};
    }
};

// ══════════════════════════════════════════════════════════
// Advanced
// ══════════════════════════════════════════════════════════

class LerpNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/lerp"; }
    QString nodeName() const override { return "Lerp"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "a", QVariant(0.0)},
                {PortType::Float, "b", QVariant(1.0)},
                {PortType::Float, "t", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double a = inputs.value("a").toDouble();
        double b = inputs.value("b").toDouble();
        double t = inputs.value("t").toDouble();
        return {{"result", a + (b - a) * t}};
    }
};

class SmoothstepNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/smoothstep"; }
    QString nodeName() const override { return "Smoothstep"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "edge0", QVariant(0.0)},
                {PortType::Float, "edge1", QVariant(1.0)},
                {PortType::Float, "x", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double e0 = inputs.value("edge0").toDouble();
        double e1 = inputs.value("edge1").toDouble();
        double x = std::clamp((inputs.value("x").toDouble() - e0) / (e1 - e0), 0.0, 1.0);
        return {{"result", x * x * (3 - 2 * x)}};
    }
};

class RoundNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/round"; }
    QString nodeName() const override { return "Round"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::round(inputs.value("value").toDouble())}};
    }
};

class FloorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/floor"; }
    QString nodeName() const override { return "Floor"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::floor(inputs.value("value").toDouble())}};
    }
};

class CeilNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/ceil"; }
    QString nodeName() const override { return "Ceil"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::ceil(inputs.value("value").toDouble())}};
    }
};

class FractNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/fract"; }
    QString nodeName() const override { return "Fract"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = inputs.value("value").toDouble();
        return {{"result", v - std::floor(v)}};
    }
};

class SignNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/sign"; }
    QString nodeName() const override { return "Sign"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = inputs.value("value").toDouble();
        return {{"result", v > 0 ? 1.0 : (v < 0 ? -1.0 : 0.0)}};
    }
};

class NormalizeNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/advanced/normalize"; }
    QString nodeName() const override { return "Normalize"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Advanced"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Vec3, "vector", QVariant::fromValue(QVector3D(1, 0, 0))}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Vec3, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto v = inputs.value("vector").value<QVector3D>();
        return {{"result", QVariant::fromValue(v.isNull() ? QVector3D() : v.normalized())}};
    }
};

// ══════════════════════════════════════════════════════════
// Trigonometry
// ══════════════════════════════════════════════════════════

class SinNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/sin"; }
    QString nodeName() const override { return "Sin"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "radians", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::sin(inputs.value("radians").toDouble())}};
    }
};

class CosNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/cos"; }
    QString nodeName() const override { return "Cos"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "radians", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::cos(inputs.value("radians").toDouble())}};
    }
};

class TanNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/tan"; }
    QString nodeName() const override { return "Tan"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "radians", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::tan(inputs.value("radians").toDouble())}};
    }
};

class ASinNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/asin"; }
    QString nodeName() const override { return "ASin"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::asin(std::clamp(inputs.value("value").toDouble(), -1.0, 1.0))}};
    }
};

class ACosNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/acos"; }
    QString nodeName() const override { return "ACos"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::acos(std::clamp(inputs.value("value").toDouble(), -1.0, 1.0))}};
    }
};

class ATanNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/trig/atan"; }
    QString nodeName() const override { return "ATan"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Trigonometry"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "y", QVariant(0.0)}, {PortType::Float, "x", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", std::atan2(inputs.value("y").toDouble(), inputs.value("x").toDouble())}};
    }
};

// ══════════════════════════════════════════════════════════
// Vector
// ══════════════════════════════════════════════════════════

class Vec2Node : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/vec2"; }
    QString nodeName() const override { return "Vec2"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "x", QVariant(0.0)}, {PortType::Float, "y", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Vec2, "vec2", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"vec2", QVariant::fromValue(QVector2D(inputs.value("x").toFloat(), inputs.value("y").toFloat()))}};
    }
};

class Vec3Node : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/vec3"; }
    QString nodeName() const override { return "Vec3"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "x", QVariant(0.0)},
                {PortType::Float, "y", QVariant(0.0)},
                {PortType::Float, "z", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Vec3, "vec3", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"vec3", QVariant::fromValue(QVector3D(inputs.value("x").toFloat(),
                                                       inputs.value("y").toFloat(),
                                                       inputs.value("z").toFloat()))}};
    }
};

class Vec4Node : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/vec4"; }
    QString nodeName() const override { return "Vec4"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "x", QVariant(0.0)},
                {PortType::Float, "y", QVariant(0.0)},
                {PortType::Float, "z", QVariant(0.0)},
                {PortType::Float, "w", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Vec4, "vec4", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"vec4", QVariant::fromValue(QVector4D(inputs.value("x").toFloat(),
                                                       inputs.value("y").toFloat(),
                                                       inputs.value("z").toFloat(),
                                                       inputs.value("w").toFloat()))}};
    }
};

class DotProductNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/dotProduct"; }
    QString nodeName() const override { return "Dot Product"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Vec3, "a", QVariant::fromValue(QVector3D())},
                {PortType::Vec3, "b", QVariant::fromValue(QVector3D())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto a = inputs.value("a").value<QVector3D>();
        auto b = inputs.value("b").value<QVector3D>();
        return {{"result", QVector3D::dotProduct(a, b)}};
    }
};

class CrossProductNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/crossProduct"; }
    QString nodeName() const override { return "Cross Product"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Vec3, "a", QVariant::fromValue(QVector3D())},
                {PortType::Vec3, "b", QVariant::fromValue(QVector3D())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Vec3, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto a = inputs.value("a").value<QVector3D>();
        auto b = inputs.value("b").value<QVector3D>();
        return {{"result", QVariant::fromValue(QVector3D::crossProduct(a, b))}};
    }
};

class LengthNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/length"; }
    QString nodeName() const override { return "Length"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Vec3, "vector", QVariant::fromValue(QVector3D())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", static_cast<double>(inputs.value("vector").value<QVector3D>().length())}};
    }
};

class DistanceNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/vector/distance"; }
    QString nodeName() const override { return "Distance"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Vector"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Vec3, "a", QVariant::fromValue(QVector3D())},
                {PortType::Vec3, "b", QVariant::fromValue(QVector3D())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto a = inputs.value("a").value<QVector3D>();
        auto b = inputs.value("b").value<QVector3D>();
        return {{"result", static_cast<double>(a.distanceToPoint(b))}};
    }
};

// ══════════════════════════════════════════════════════════
// Random
// ══════════════════════════════════════════════════════════

class RandomFloatNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/random/float"; }
    QString nodeName() const override { return "Random Float"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Random"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "min", QVariant(0.0)}, {PortType::Float, "max", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double mn = inputs.value("min").toDouble();
        double mx = inputs.value("max").toDouble();
        double r = QRandomGenerator::global()->generateDouble();
        return {{"result", mn + (mx - mn) * r}};
    }
};

class RandomIntegerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/random/integer"; }
    QString nodeName() const override { return "Random Integer"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Random"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Int, "min", QVariant(0)}, {PortType::Int, "max", QVariant(100)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Int, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        int mn = inputs.value("min").toInt();
        int mx = inputs.value("max").toInt();
        return {{"result", QRandomGenerator::global()->bounded(mn, mx + 1)}};
    }
};

class NoiseNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/random/noise"; }
    QString nodeName() const override { return "Noise"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Random"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "x", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double x = inputs.value("x").toDouble();
        int ix = static_cast<int>(std::floor(x));
        double fx = x - std::floor(x);
        auto hash = [](int i) { i = (i << 13) ^ i; return (i * (i * i * 15731 + 789221) + 1376312589) & 0x7fffffff; };
        double n0 = hash(ix) / double(0x7fffffff);
        double n1 = hash(ix + 1) / double(0x7fffffff);
        double t = fx * fx * (3 - 2 * fx);
        return {{"result", n0 + (n1 - n0) * t}};
    }
};

class SeedRandomNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "math/random/seedRandom"; }
    QString nodeName() const override { return "Seed Random"; }
    QString nodeCategory() const override { return "Math"; }
    QString nodeSubCategory() const override { return "Random"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Int, "seed", QVariant(0)}, {PortType::Float, "min", QVariant(0.0)}, {PortType::Float, "max", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        int seed = inputs.value("seed").toInt();
        double mn = inputs.value("min").toDouble();
        double mx = inputs.value("max").toDouble();
        seed = (seed * 16807) % 2147483647;
        double r = double(seed) / 2147483647.0;
        return {{"result", mn + (mx - mn) * r}};
    }
};

} // namespace NodeEditor
