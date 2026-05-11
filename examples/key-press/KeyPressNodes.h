#pragma once

#include "NodeEditor/BaseNode.h"
#include <QDateTime>
#include <QHash>

namespace NodeEditor {

class KeyPressCaptureNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    void setKeyEvent(const QString &key, bool pressed)
    {
        m_lastKey = key;
        m_isPressed = pressed;
        m_timestamp = QDateTime::currentMSecsSinceEpoch();
        setDirty(true);
    }

    QVariantMap compute(const QVariantMap &inputs) override
    {
        Q_UNUSED(inputs)
        QVariantMap out;
        out["key"] = m_lastKey;
        out["isPressed"] = m_isPressed;
        out["timestamp"] = m_timestamp;
        return out;
    }

    QList<PortInfo> inputSpec() const override { return {}; }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::String, "key", QVariant("")},
                {PortType::Bool, "isPressed", QVariant(false)},
                {PortType::Float, "timestamp", QVariant(0.0)}};
    }

    QString nodeType() const override { return "keyPress/capture"; }
    QString nodeName() const override { return "Key Press Capture"; }
    QString nodeCategory() const override { return "Input"; }
    QString nodeSubCategory() const override { return "Keyboard"; }
    QString displayColor() const override { return "#E17055"; }

private:
    QString m_lastKey;
    bool m_isPressed = false;
    qint64 m_timestamp = 0;
};

// KeyToColorNode: maps a key to a display color
class KeyToColorNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        QString key = inputs.value("key").toString();
        bool pressed = inputs.value("isPressed").toBool();

        if (!pressed || key.isEmpty()) {
            out["color"] = QColor("#2D3436");
            out["display"] = "";
            out["brightness"] = 0.0;
            return out;
        }

        // Simple hash-based color from key
        int h = qHash(key) % 360;
        out["color"] = QColor::fromHsl(h, 200, 150);
        out["display"] = key.toUpper();
        out["brightness"] = 1.0;
        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::String, "key", QVariant("")},
                {PortType::Bool, "isPressed", QVariant(false)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Color, "color", QVariant()},
                {PortType::String, "display", QVariant()},
                {PortType::Float, "brightness", QVariant()}};
    }

    QString nodeType() const override { return "keyPress/keyToColor"; }
    QString nodeName() const override { return "Key To Color"; }
    QString nodeCategory() const override { return "Conversion"; }
    QString nodeSubCategory() const override { return "Keyboard"; }
    QString displayColor() const override { return "#6C5CE7"; }
};

inline void registerKeyPressNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Input", "Input", QColor("#E17055")});
    model->registerCategory({"Conversion", "Conversion", QColor("#6C5CE7")});
    registerNodeType<KeyPressCaptureNode>(model, "Input");
    registerNodeType<KeyToColorNode>(model, "Conversion");
}

} // namespace NodeEditor
