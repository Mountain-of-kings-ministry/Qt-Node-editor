#pragma once

#include "NodeEditor/BaseNode.h"
#include <QColor>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// Color Basics
// ══════════════════════════════════════════════════════════

class ColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/basics/color"; }
    QString nodeName() const override { return "Color"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Basics"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor(Qt::white))}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"color", inputs.value("color")}};
    }
};

class RGBColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/basics/rgb"; }
    QString nodeName() const override { return "RGB Color"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Basics"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "r", QVariant(0.0)}, {PortType::Float, "g", QVariant(0.0)}, {PortType::Float, "b", QVariant(0.0)}, {PortType::Float, "a", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c;
        c.setRedF(std::clamp(inputs.value("r").toDouble(), 0.0, 1.0));
        c.setGreenF(std::clamp(inputs.value("g").toDouble(), 0.0, 1.0));
        c.setBlueF(std::clamp(inputs.value("b").toDouble(), 0.0, 1.0));
        c.setAlphaF(std::clamp(inputs.value("a").toDouble(), 0.0, 1.0));
        return {{"color", c}};
    }
};

class HSVColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/basics/hsv"; }
    QString nodeName() const override { return "HSV Color"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Basics"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "h", QVariant(0.0)}, {PortType::Float, "s", QVariant(1.0)}, {PortType::Float, "v", QVariant(1.0)}, {PortType::Float, "a", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c;
        c.setHsvF(std::clamp(inputs.value("h").toDouble(), 0.0, 1.0),
                   std::clamp(inputs.value("s").toDouble(), 0.0, 1.0),
                   std::clamp(inputs.value("v").toDouble(), 0.0, 1.0),
                   std::clamp(inputs.value("a").toDouble(), 0.0, 1.0));
        return {{"color", c}};
    }
};

class HexColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/basics/hex"; }
    QString nodeName() const override { return "Hex Color"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Basics"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor(Qt::white))}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"color", inputs.value("color")}};
    }
};

class ColorFromStringNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/basics/fromString"; }
    QString nodeName() const override { return "Color From String"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Basics"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor(Qt::red))}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"color", inputs.value("color")}};
    }
};

// ══════════════════════════════════════════════════════════
// Manipulation
// ══════════════════════════════════════════════════════════

class BlendColorsNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/blend"; }
    QString nodeName() const override { return "Blend Colors"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "a", QVariant::fromValue(QColor())},
                {PortType::Color, "b", QVariant::fromValue(QColor())},
                {PortType::Float, "t", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor a = inputs.value("a").value<QColor>();
        QColor b = inputs.value("b").value<QColor>();
        double t = std::clamp(inputs.value("t").toDouble(), 0.0, 1.0);
        QColor c;
        c.setRedF(a.redF() + (b.redF() - a.redF()) * t);
        c.setGreenF(a.greenF() + (b.greenF() - a.greenF()) * t);
        c.setBlueF(a.blueF() + (b.blueF() - a.blueF()) * t);
        c.setAlphaF(a.alphaF() + (b.alphaF() - a.alphaF()) * t);
        return {{"result", c}};
    }
};

class MultiplyColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/multiply"; }
    QString nodeName() const override { return "Multiply Color"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}, {PortType::Float, "factor", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        double f = inputs.value("factor").toDouble();
        c.setRedF(std::clamp(c.redF() * f, 0.0, 1.0));
        c.setGreenF(std::clamp(c.greenF() * f, 0.0, 1.0));
        c.setBlueF(std::clamp(c.blueF() * f, 0.0, 1.0));
        return {{"result", c}};
    }
};

class InvertColorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/invert"; }
    QString nodeName() const override { return "Invert"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        c.setRedF(1.0 - c.redF());
        c.setGreenF(1.0 - c.greenF());
        c.setBlueF(1.0 - c.blueF());
        return {{"result", c}};
    }
};

class SaturationNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/saturation"; }
    QString nodeName() const override { return "Saturation"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}, {PortType::Float, "factor", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        double f = std::clamp(inputs.value("factor").toDouble(), 0.0, 2.0);
        float h, s, v, a;
        c.getHsvF(&h, &s, &v, &a);
        c.setHsvF(h, std::clamp(s * static_cast<float>(f), 0.0f, 1.0f), v, a);
        return {{"result", c}};
    }
};

class BrightnessNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/brightness"; }
    QString nodeName() const override { return "Brightness"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}, {PortType::Float, "factor", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        double f = std::clamp(inputs.value("factor").toDouble(), 0.0, 2.0);
        float h, s, v, a;
        c.getHsvF(&h, &s, &v, &a);
        c.setHsvF(h, s, std::clamp(v * static_cast<float>(f), 0.0f, 1.0f), a);
        return {{"result", c}};
    }
};

class ContrastNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/manipulation/contrast"; }
    QString nodeName() const override { return "Contrast"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Manipulation"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}, {PortType::Float, "amount", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        double amt = std::clamp(inputs.value("amount").toDouble(), 0.0, 2.0);
        auto contrast = [amt](double v) { return std::clamp((v - 0.5) * amt + 0.5, 0.0, 1.0); };
        c.setRedF(contrast(c.redF()));
        c.setGreenF(contrast(c.greenF()));
        c.setBlueF(contrast(c.blueF()));
        return {{"result", c}};
    }
};

// ══════════════════════════════════════════════════════════
// Conversion
// ══════════════════════════════════════════════════════════

class RGBToHSVNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/conversion/rgbToHsv"; }
    QString nodeName() const override { return "RGB to HSV"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Conversion"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "h", QVariant()}, {PortType::Float, "s", QVariant()}, {PortType::Float, "v", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c = inputs.value("color").value<QColor>();
        float h, s, v, a;
        c.getHsvF(&h, &s, &v, &a);
        return {{"h", static_cast<double>(h)}, {"s", static_cast<double>(s)}, {"v", static_cast<double>(v)}};
    }
};

class HSVToRGBNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/conversion/hsvToRgb"; }
    QString nodeName() const override { return "HSV to RGB"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Conversion"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "h", QVariant(0.0)}, {PortType::Float, "s", QVariant(1.0)}, {PortType::Float, "v", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor c;
        c.setHsvF(std::clamp(inputs.value("h").toDouble(), 0.0, 1.0),
                   std::clamp(inputs.value("s").toDouble(), 0.0, 1.0),
                   std::clamp(inputs.value("v").toDouble(), 0.0, 1.0));
        return {{"color", c}};
    }
};

class RGBToHEXNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/conversion/rgbToHex"; }
    QString nodeName() const override { return "RGB to HEX"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Conversion"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "color", QVariant::fromValue(QColor())}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "hex", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"hex", inputs.value("color").value<QColor>().name(QColor::HexArgb)}};
    }
};

class HEXToRGBNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/conversion/hexToRgb"; }
    QString nodeName() const override { return "HEX to RGB"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Conversion"; }
    QString displayColor() const override { return "#F0DB4F"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "hex", QVariant("#FFFFFF")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"color", QColor(inputs.value("hex").toString())}};
    }
};

// ══════════════════════════════════════════════════════════
// Palette
// ══════════════════════════════════════════════════════════

class GradientNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/palette/gradient"; }
    QString nodeName() const override { return "Gradient"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Palette"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "from", QVariant::fromValue(QColor(Qt::black))},
                {PortType::Color, "to", QVariant::fromValue(QColor(Qt::white))},
                {PortType::Float, "t", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor a = inputs.value("from").value<QColor>();
        QColor b = inputs.value("to").value<QColor>();
        double t = std::clamp(inputs.value("t").toDouble(), 0.0, 1.0);
        QColor c;
        c.setRedF(a.redF() + (b.redF() - a.redF()) * t);
        c.setGreenF(a.greenF() + (b.greenF() - a.greenF()) * t);
        c.setBlueF(a.blueF() + (b.blueF() - a.blueF()) * t);
        c.setAlphaF(a.alphaF() + (b.alphaF() - a.alphaF()) * t);
        return {{"color", c}};
    }
};

class PaletteGeneratorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "color/palette/generator"; }
    QString nodeName() const override { return "Palette Generator"; }
    QString nodeCategory() const override { return "Color"; }
    QString nodeSubCategory() const override { return "Palette"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Color, "base", QVariant::fromValue(QColor(Qt::blue))},
                {PortType::Int, "count", QVariant(5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "palette", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QColor base = inputs.value("base").value<QColor>();
        int count = std::max(1, inputs.value("count").toInt());
        float h, s, v, a;
        base.getHsvF(&h, &s, &v, &a);
        QVariantList palette;
        for (int i = 0; i < count; ++i) {
            QColor c;
            double t = double(i) / (count - 1);
            c.setHsvF(std::fmod(h + static_cast<float>(t * 0.3), 1.0f),
                       std::clamp(s * static_cast<float>(1.0 - t * 0.3), 0.3f, 1.0f),
                       std::clamp(v * static_cast<float>(0.7 + t * 0.3), 0.3f, 1.0f));
            palette.append(QVariant::fromValue(c));
        }
        return {{"palette", palette}};
    }
};

inline void registerColorNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Color", "Color", QColor("#A29BFE")});
    registerNodeType<ColorNode>(model, "Color");
    registerNodeType<RGBColorNode>(model, "Color");
    registerNodeType<HSVColorNode>(model, "Color");
    registerNodeType<HexColorNode>(model, "Color");
    registerNodeType<ColorFromStringNode>(model, "Color");
    registerNodeType<BlendColorsNode>(model, "Color");
    registerNodeType<MultiplyColorNode>(model, "Color");
    registerNodeType<InvertColorNode>(model, "Color");
    registerNodeType<SaturationNode>(model, "Color");
    registerNodeType<BrightnessNode>(model, "Color");
    registerNodeType<ContrastNode>(model, "Color");
    registerNodeType<RGBToHSVNode>(model, "Color");
    registerNodeType<HSVToRGBNode>(model, "Color");
    registerNodeType<RGBToHEXNode>(model, "Color");
    registerNodeType<HEXToRGBNode>(model, "Color");
    registerNodeType<GradientNode>(model, "Color");
    registerNodeType<PaletteGeneratorNode>(model, "Color");
}

} // namespace NodeEditor
