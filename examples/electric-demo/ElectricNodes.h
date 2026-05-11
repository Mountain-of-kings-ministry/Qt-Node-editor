#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// BatteryNode: provides a fixed voltage
class BatteryNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        Q_UNUSED(inputs)
        QVariantMap out;
        double v = inputs.value("voltage", 9.0).toDouble();
        out["voltage"] = v;
        out["current"] = 0.0;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "voltage", QVariant(9.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "voltage", QVariant()},
                {PortType::Float, "current", QVariant()}};
    }

    QString nodeType() const override { return "electric/battery"; }
    QString nodeName() const override { return "Battery"; }
    QString nodeCategory() const override { return "Circuit"; }
    QString nodeSubCategory() const override { return "Sources"; }
    QString displayColor() const override { return "#00B894"; }
};

// ResistorNode: has resistance, computes current from voltage drop
class ResistorNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double vin = inputs.value("inVoltage").toDouble();
        double r = inputs.value("resistance", 1000.0).toDouble();
        double i = (r > 0) ? vin / r : 0.0;
        out["outVoltage"] = vin;
        out["current"] = i;
        out["power"] = vin * i;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "inVoltage", QVariant(0.0)},
                {PortType::Float, "resistance", QVariant(1000.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "outVoltage", QVariant()},
                {PortType::Float, "current", QVariant()},
                {PortType::Float, "power", QVariant()}};
    }

    QString nodeType() const override { return "electric/resistor"; }
    QString nodeName() const override { return "Resistor"; }
    QString nodeCategory() const override { return "Circuit"; }
    QString nodeSubCategory() const override { return "Passive"; }
    QString displayColor() const override { return "#FDCB6E"; }
};

// LEDNode: lights up based on forward voltage
class LEDNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double vf = inputs.value("forwardVoltage", 2.0).toDouble();
        double vin = inputs.value("inVoltage").toDouble();
        double brightness = (vin >= vf) ? std::min(1.0, (vin - vf) / 3.0) : 0.0;
        out["brightness"] = brightness;
        out["lit"] = brightness > 0.0;
        out["color"] = brightness > 0.0 ? QColor("#FFEAA7") : QColor("#2D3436");
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "inVoltage", QVariant(0.0)},
                {PortType::Float, "forwardVoltage", QVariant(2.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "brightness", QVariant()},
                {PortType::Bool, "lit", QVariant()},
                {PortType::Color, "color", QVariant()}};
    }

    QString nodeType() const override { return "electric/led"; }
    QString nodeName() const override { return "LED"; }
    QString nodeCategory() const override { return "Circuit"; }
    QString nodeSubCategory() const override { return "Output"; }
    QString displayColor() const override { return "#FFEAA7"; }
};

// CapacitorNode: simple integrator (charge/discharge over time)
class CapacitorNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        double vin = inputs.value("inVoltage").toDouble();
        double c = inputs.value("capacitance", 0.001).toDouble();
        double prevV = inputs.value("prevVoltage", 0.0).toDouble();
        double dt = inputs.value("dt", 0.1).toDouble();
        double vcap = prevV + (vin - prevV) * dt / (c * 1000 + dt);
        out["voltage"] = vcap;
        out["charge"] = c * vcap;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "inVoltage", QVariant(0.0)},
                {PortType::Float, "capacitance", QVariant(0.001)},
                {PortType::Float, "prevVoltage", QVariant(0.0)},
                {PortType::Float, "dt", QVariant(0.1)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "voltage", QVariant()},
                {PortType::Float, "charge", QVariant()}};
    }

    QString nodeType() const override { return "electric/capacitor"; }
    QString nodeName() const override { return "Capacitor"; }
    QString nodeCategory() const override { return "Circuit"; }
    QString nodeSubCategory() const override { return "Passive"; }
    QString displayColor() const override { return "#74B9FF"; }
};

// WireNode: simple pass-through connector
class WireNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        out["voltage"] = inputs.value("voltage").toDouble();
        out["current"] = inputs.value("current").toDouble();
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::Float, "voltage", QVariant(0.0)},
                {PortType::Float, "current", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "voltage", QVariant()},
                {PortType::Float, "current", QVariant()}};
    }

    QString nodeType() const override { return "electric/wire"; }
    QString nodeName() const override { return "Wire"; }
    QString nodeCategory() const override { return "Circuit"; }
    QString nodeSubCategory() const override { return "Connectors"; }
    QString displayColor() const override { return "#636E72"; }
};

inline void registerElectricNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Circuit", "Circuit", QColor("#00B894")});
    registerNodeType<BatteryNode>(model, "Circuit");
    registerNodeType<ResistorNode>(model, "Circuit");
    registerNodeType<LEDNode>(model, "Circuit");
    registerNodeType<CapacitorNode>(model, "Circuit");
    registerNodeType<WireNode>(model, "Circuit");
}

} // namespace NodeEditor
