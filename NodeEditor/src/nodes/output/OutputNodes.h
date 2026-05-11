#pragma once

#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// Visual Output
// ══════════════════════════════════════════════════════════

class ImageOutputNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/visual/image"; }
    QString nodeName() const override { return "Image Output"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Visual"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Image, "image", QVariant()},
                {PortType::Int, "width", QVariant(256)},
                {PortType::Int, "height", QVariant(256)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "info", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto img = inputs.value("image").value<QImage>();
        return {{"info", img.isNull() ? QString("No image") : QString("%1x%2").arg(img.width()).arg(img.height())}};
    }
};

class VideoOutputNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/visual/video"; }
    QString nodeName() const override { return "Video Output"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Visual"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Image, "frame", QVariant()},
                {PortType::Float, "fps", QVariant(30.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "status", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"status", "Video output (preview)"}};
    }
};

class ViewportOutputNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/visual/viewport"; }
    QString nodeName() const override { return "Viewport Output"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Visual"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Image, "frame", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "status", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"status", "Viewport"}};
    }
};

class PixelBufferOutputNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/visual/pixelBuffer"; }
    QString nodeName() const override { return "Pixel Buffer Output"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Visual"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Image, "buffer", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "info", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"info", "Pixel buffer"}};
    }
};

// ══════════════════════════════════════════════════════════
// Data Output
// ══════════════════════════════════════════════════════════

class FileWriterNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/data/fileWriter"; }
    QString nodeName() const override { return "File Writer"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Data"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "path", QVariant("output.txt")},
                {PortType::String, "content", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "success", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"success", true}};
    }
};

class JSONExportNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/data/jsonExport"; }
    QString nodeName() const override { return "JSON Export"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Data"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Map, "data", QVariantMap()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "json", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"json", "{}"}};
    }
};

// ══════════════════════════════════════════════════════════
// Debug Output
// ══════════════════════════════════════════════════════════

class PrintNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/debug/print"; }
    QString nodeName() const override { return "Print"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "text", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        qDebug().noquote() << inputs.value("text").toString();
        return {};
    }
};

class LoggerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/debug/logger"; }
    QString nodeName() const override { return "Logger"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "message", QVariant("")},
                {PortType::String, "level", QVariant("info")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        qDebug().noquote() << QString("[%1] %2").arg(inputs.value("level").toString().toUpper()).arg(inputs.value("message").toString());
        return {};
    }
};

class GraphInspectorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/debug/graphInspector"; }
    QString nodeName() const override { return "Graph Inspector"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "data", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "inspect", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"inspect", QString("Type: %1\nValue: %2").arg(inputs.value("data").typeName()).arg(inputs.value("data").toString())}};
    }
};

class WatchValueNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/debug/watchValue"; }
    QString nodeName() const override { return "Watch Value"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}, {PortType::String, "label", QVariant("Watch")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"display", QString("[%1] %2").arg(inputs.value("label").toString()).arg(inputs.value("value").toString())}};
    }
};

class PerformanceMonitorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/debug/performance"; }
    QString nodeName() const override { return "Performance Monitor"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Debug"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "fps", QVariant()}, {PortType::Float, "frameTime", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"fps", 60.0}, {"frameTime", 16.6}};
    }
};

} // namespace NodeEditor
