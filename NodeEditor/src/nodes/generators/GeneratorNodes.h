#pragma once

#include "NodeEditor/BaseNode.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QImage>

namespace NodeEditor {

class SineWaveNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "generators/wave/sine"; }
    QString nodeName() const override { return "Sine Wave"; }
    QString nodeCategory() const override { return "Generators"; }
    QString nodeSubCategory() const override { return "Wave"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "frequency", QVariant(1.0)},
                {PortType::Float, "amplitude", QVariant(1.0)},
                {PortType::Float, "phase", QVariant(0.0)},
                {PortType::Float, "time", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double f = inputs.value("frequency").toDouble();
        double a = inputs.value("amplitude").toDouble();
        double p = inputs.value("phase").toDouble();
        double t = inputs.value("time").toDouble();
        return {{"value", a * std::sin(2 * M_PI * f * t + p)}};
    }
};

class NoiseGeneratorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "generators/wave/noise"; }
    QString nodeName() const override { return "Noise Generator"; }
    QString nodeCategory() const override { return "Generators"; }
    QString nodeSubCategory() const override { return "Wave"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "seed", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"value", double(QRandomGenerator::global()->generateDouble())}};
    }
};

class GradientGeneratorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "generators/color/gradient"; }
    QString nodeName() const override { return "Gradient Generator"; }
    QString nodeCategory() const override { return "Generators"; }
    QString nodeSubCategory() const override { return "Color"; }
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
        return {{"color", c}};
    }
};

class OscillatorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "generators/wave/oscillator"; }
    QString nodeName() const override { return "Oscillator"; }
    QString nodeCategory() const override { return "Generators"; }
    QString nodeSubCategory() const override { return "Wave"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "frequency", QVariant(1.0)},
                {PortType::Float, "amplitude", QVariant(1.0)},
                {PortType::Float, "time", QVariant(0.0)},
                {PortType::String, "type", QVariant("sine")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double f = inputs.value("frequency").toDouble();
        double a = inputs.value("amplitude").toDouble();
        double t = inputs.value("time").toDouble();
        QString type = inputs.value("type").toString().toLower();
        double phase = std::fmod(2 * M_PI * f * t, 2 * M_PI);
        double v = 0;
        if (type == "sine") v = std::sin(phase);
        else if (type == "square") v = std::sin(phase) >= 0 ? 1 : -1;
        else if (type == "triangle") v = 2 * std::abs(2 * (phase / (2 * M_PI) - std::floor(phase / (2 * M_PI) + 0.5))) - 1;
        else if (type == "sawtooth") v = 2 * (phase / (2 * M_PI) - std::floor(phase / (2 * M_PI) + 0.5));
        return {{"value", a * v}};
    }
};

class CameraFeedNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "generators/media/camera"; }
    QString nodeName() const override { return "Camera Feed"; }
    QString nodeCategory() const override { return "Generators"; }
    QString nodeSubCategory() const override { return "Media"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Int, "deviceId", QVariant(0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Image, "frame", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"frame", QVariant::fromValue(QImage())}};
    }
};

inline void registerGeneratorNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Generators", "Generators", QColor("#55EFC4")});
    registerNodeType<SineWaveNode>(model, "Generators");
    registerNodeType<NoiseGeneratorNode>(model, "Generators");
    registerNodeType<GradientGeneratorNode>(model, "Generators");
    registerNodeType<OscillatorNode>(model, "Generators");
    registerNodeType<CameraFeedNode>(model, "Generators");
}

} // namespace NodeEditor
