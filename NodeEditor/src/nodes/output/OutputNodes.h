#pragma once

#include "NodeEditor/BaseNode.h"
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtMath>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRegularExpression>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// Helpers
// ══════════════════════════════════════════════════════════

static QString imageToBase64(const QImage &img)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    return QString::fromLatin1(bytes.toBase64());
}

static QColor valueToColor(double t)
{
    double r, g, b;
    if (t < 0.25) { double p = t / 0.25; r = 0; g = p; b = 1; }
    else if (t < 0.5) { double p = (t - 0.25) / 0.25; r = 0; g = 1; b = 1 - p; }
    else if (t < 0.75) { double p = (t - 0.5) / 0.25; r = p; g = 1; b = 0; }
    else { double p = (t - 0.75) / 0.25; r = 1; g = 1 - p; b = 0; }
    return QColor::fromRgbF(r, g, b);
}

// ══════════════════════════════════════════════════════════
// Existing Output Nodes
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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto img = inputs.value("image").value<QImage>();
        QString result = img.isNull() ? "No image" : QString("%1x%2").arg(img.width()).arg(img.height());
        return {{"display", result}};
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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"display", "Video output (preview)"}};
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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"display", "Viewport"}};
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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"display", "Pixel buffer"}};
    }
};

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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"display", "{}"}};
    }
};

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
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"display", QString("Type: %1\nValue: %2").arg(inputs.value("data").typeName()).arg(inputs.value("data").toString())}};
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

// ══════════════════════════════════════════════════════════
// Value Displays
// ══════════════════════════════════════════════════════════

class NumberDisplayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/number"; }
    QString nodeName() const override { return "Number Display"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariant v = inputs.value("value");
        QString s;
        if (v.typeId() == QMetaType::Double || v.typeId() == QMetaType::Float)
            s = QString::number(v.toDouble(), 'f', 2);
        else
            s = v.toString();
        return {{"display", s}};
    }
};

class TextDisplayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/text"; }
    QString nodeName() const override { return "Text Display"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "text", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"display", inputs.value("text").toString()}};
    }
};

class BooleanIndicatorNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/boolean"; }
    QString nodeName() const override { return "Boolean Indicator"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "value", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()},
                {PortType::Color, "color", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        bool val = inputs.value("value").toBool();
        QImage img(60, 30, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        QColor c = val ? QColor("#00B894") : QColor("#636E72");
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawEllipse(15, 0, 30, 30);
        p.end();
        return {{"display", imageToBase64(img)}, {"color", c}};
    }
};

class VectorDisplayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/vector"; }
    QString nodeName() const override { return "Vector Display"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "x", QVariant(0.0)},
                {PortType::Float, "y", QVariant(0.0)},
                {PortType::Float, "z", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double x = inputs.value("x").toDouble();
        double y = inputs.value("y").toDouble();
        double z = inputs.value("z").toDouble();
        return {{"display", QString("X: %1\nY: %2\nZ: %3").arg(x, 0, 'f', 2).arg(y, 0, 'f', 2).arg(z, 0, 'f', 2)}};
    }
};

// ══════════════════════════════════════════════════════════
// Chart Displays
// ══════════════════════════════════════════════════════════

class LinePlotNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/linePlot"; }
    QString nodeName() const override { return "Line Plot"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "series", QVariant("[1,2,3,4,5]")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray arr = QJsonDocument::fromJson(inputs.value("series").toString().toUtf8()).array();
        QVector<double> vals;
        double mn = 0, mx = 1;
        for (const auto &v : arr) { double d = v.toDouble(); vals.append(d); mn = qMin(mn, d); mx = qMax(mx, d); }
        if (mx - mn < 0.001) { mn -= 0.5; mx += 0.5; }
        if (vals.isEmpty()) return {{"display", "No data"}};

        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        double pad = 5, w = 150, h = 70;
        p.setPen(QColor("#444444"));
        p.drawLine(QPointF(pad, pad + h), QPointF(pad + w, pad + h));
        p.drawLine(QPointF(pad, pad), QPointF(pad, pad + h));
        p.setPen(QColor("#6C5CE7"));
        int n = vals.size();
        for (int i = 1; i < n; ++i) {
            double x1 = pad + (i - 1) * w / (n - 1);
            double y1 = pad + h - (vals[i - 1] - mn) / (mx - mn) * h;
            double x2 = pad + i * w / (n - 1);
            double y2 = pad + h - (vals[i] - mn) / (mx - mn) * h;
            p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class ScatterPlotNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/scatterPlot"; }
    QString nodeName() const override { return "Scatter Plot"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "xValues", QVariant("[1,2,3,4,5]")},
                {PortType::String, "yValues", QVariant("[1,4,9,16,25]")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray xa = QJsonDocument::fromJson(inputs.value("xValues").toString().toUtf8()).array();
        QJsonArray ya = QJsonDocument::fromJson(inputs.value("yValues").toString().toUtf8()).array();
        QVector<QPointF> pts;
        double mnx = 0, mxx = 1, mny = 0, mxy = 1;
        for (int i = 0; i < qMin(xa.size(), ya.size()); ++i) {
            double x = xa[i].toDouble(), y = ya[i].toDouble();
            pts.append(QPointF(x, y));
            if (i == 0) { mnx = mxx = x; mny = mxy = y; }
            else { mnx = qMin(mnx, x); mxx = qMax(mxx, x); mny = qMin(mny, y); mxy = qMax(mxy, y); }
        }
        if (mxx - mnx < 0.001) { mnx -= 0.5; mxx += 0.5; }
        if (mxy - mny < 0.001) { mny -= 0.5; mxy += 0.5; }
        if (pts.isEmpty()) return {{"display", "No data"}};

        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        double pad = 5, w = 150, h = 70;
        p.setPen(QColor("#444444"));
        p.drawLine(QPointF(pad, pad + h), QPointF(pad + w, pad + h));
        p.drawLine(QPointF(pad, pad), QPointF(pad, pad + h));
        p.setBrush(QColor("#A29BFE"));
        p.setPen(Qt::NoPen);
        for (const auto &pt : pts) {
            double x = pad + (pt.x() - mnx) / (mxx - mnx) * w;
            double y = pad + h - (pt.y() - mny) / (mxy - mny) * h;
            p.drawEllipse(QPointF(x, y), 2.5, 2.5);
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class BarChartNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/barChart"; }
    QString nodeName() const override { return "Bar Chart"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "values", QVariant("[4,7,2,9,5]")},
                {PortType::String, "labels", QVariant("[]")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray va = QJsonDocument::fromJson(inputs.value("values").toString().toUtf8()).array();
        if (va.isEmpty()) return {{"display", "No data"}};
        double mx = 1;
        for (const auto &v : va) mx = qMax(mx, v.toDouble());
        if (mx < 0.001) mx = 1;

        int n = va.size();
        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        double pad = 5, w = 150, h = 70, bw = w / n * 0.7;
        QColor colors[] = {{"#E17055"}, {"#FDCB6E"}, {"#00CEC9"}, {"#6C5CE7"}, {"#00B894"}, {"#74B9FF"}, {"#FF6B6B"}, {"#A29BFE"}};
        for (int i = 0; i < n; ++i) {
            double bh = (va[i].toDouble() / mx) * h;
            double x = pad + i * (w / n) + (w / n - bw) / 2;
            p.fillRect(QRectF(x, pad + h - bh, bw, bh), colors[i % 8]);
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class PieChartNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/pieChart"; }
    QString nodeName() const override { return "Pie Chart"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "segments", QVariant(R"({"A":10,"B":20,"C":15})")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonObject obj = QJsonDocument::fromJson(inputs.value("segments").toString().toUtf8()).object();
        if (obj.isEmpty()) return {{"display", "No data"}};
        double total = 0;
        for (auto it = obj.begin(); it != obj.end(); ++it) total += it.value().toDouble();
        if (total < 0.001) return {{"display", "No data"}};

        QImage img(100, 80, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r(5, 5, 70, 70);
        QColor colors[] = {{"#E17055"}, {"#FDCB6E"}, {"#00CEC9"}, {"#6C5CE7"}, {"#00B894"}, {"#74B9FF"}, {"#FF6B6B"}, {"#A29BFE"}};
        double startAngle = 90.0 * 16;
        int ci = 0;
        for (auto it = obj.begin(); it != obj.end(); ++it, ++ci) {
            double span = (it.value().toDouble() / total) * 360.0 * 16;
            p.setBrush(colors[ci % 8]);
            p.setPen(QPen(QColor("#1E1E1E"), 1));
            p.drawPie(r, (int)startAngle, (int)span);
            startAngle += span;
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class HistogramNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/histogram"; }
    QString nodeName() const override { return "Histogram"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant("[1,2,2,3,3,3,4,5,5,6]")},
                {PortType::Int, "bins", QVariant(5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray arr = QJsonDocument::fromJson(inputs.value("data").toString().toUtf8()).array();
        int nb = qBound(1, inputs.value("bins").toInt(), 20);
        if (arr.isEmpty()) return {{"display", "No data"}};
        double mn = arr[0].toDouble(), mx = arr[0].toDouble();
        for (const auto &v : arr) { double d = v.toDouble(); mn = qMin(mn, d); mx = qMax(mx, d); }
        if (mx - mn < 0.001) { mn -= 0.5; mx += 0.5; }
        QVector<int> bins(nb, 0);
        int maxBin = 0;
        for (const auto &v : arr) {
            int idx = qMin((int)((v.toDouble() - mn) / (mx - mn) * nb), nb - 1);
            maxBin = qMax(maxBin, ++bins[idx]);
        }
        if (maxBin < 1) maxBin = 1;

        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        double pad = 5, w = 150, h = 70, bw = w / nb * 0.8;
        for (int i = 0; i < nb; ++i) {
            double bh = ((double)bins[i] / maxBin) * h;
            double x = pad + i * (w / nb) + (w / nb - bw) / 2;
            p.fillRect(QRectF(x, pad + h - bh, bw, bh), QColor("#FF6B6B"));
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class HeatmapNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/heatmap"; }
    QString nodeName() const override { return "Heatmap"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Chart"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant(R"([[1,2,3],[4,5,6],[7,8,9]])")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray rows = QJsonDocument::fromJson(inputs.value("data").toString().toUtf8()).array();
        if (rows.isEmpty()) return {{"display", "No data"}};
        int rcnt = rows.size(), ccnt = rows[0].toArray().size();
        if (rcnt < 1 || ccnt < 1) return {{"display", "No data"}};

        QVector<double> vals;
        double mn = 1e9, mx = -1e9;
        for (const auto &row : rows) {
            QJsonArray ra = row.toArray();
            for (const auto &v : ra) { double d = v.toDouble(); vals.append(d); mn = qMin(mn, d); mx = qMax(mx, d); }
        }
        if (mx - mn < 0.001) { mn -= 0.5; mx += 0.5; }

        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        double cw = 150.0 / ccnt, ch = 70.0 / rcnt;
        double pad = 5;
        int vi = 0;
        for (int r = 0; r < rcnt; ++r) {
            for (int c = 0; c < ccnt; ++c, ++vi) {
                double t = (vals[vi] - mn) / (mx - mn);
                p.fillRect(QRectF(pad + c * cw, pad + r * ch, cw + 0.5, ch + 0.5), valueToColor(t));
            }
        }
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

// ══════════════════════════════════════════════════════════
// Matrix / Scientific
// ══════════════════════════════════════════════════════════

class MatrixDisplayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/matrix"; }
    QString nodeName() const override { return "Matrix Display"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Matrix"; }
    QString displayColor() const override { return "#74B9FF"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "rows", QVariant("[[1,0,0],[0,1,0],[0,0,1]]")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray rows = QJsonDocument::fromJson(inputs.value("rows").toString().toUtf8()).array();
        QStringList lines;
        for (const auto &r : rows) {
            QJsonArray ra = r.toArray();
            QStringList vals;
            for (const auto &v : ra) vals << QString::number(v.toDouble(), 'f', 2);
            lines << "[ " + vals.join(", ") + " ]";
        }
        return {{"display", lines.join("\n")}};
    }
};

class MemoryViewerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/memory"; }
    QString nodeName() const override { return "Memory Viewer"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Matrix"; }
    QString displayColor() const override { return "#0984E3"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant("48656C6C6F")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QString hex = inputs.value("data").toString().remove(QRegularExpression("[^0-9a-fA-F]"));
        QStringList lines;
        for (int i = 0; i < hex.size(); i += 32) {
            QString addr = QString("%1").arg(i / 2, 8, 16, QChar('0'));
            QString chunk = hex.mid(i, 32);
            QStringList parts;
            for (int j = 0; j < chunk.size(); j += 2)
                parts << chunk.mid(j, 2);
            QString ascii;
            for (int j = 0; j < chunk.size(); j += 2) {
                bool ok;
                int c = chunk.mid(j, 2).toInt(&ok, 16);
                ascii += (ok && c >= 32 && c <= 126) ? QChar(c) : '.';
            }
            lines << addr + "  " + parts.join(" ") + "  |" + ascii + "|";
        }
        return {{"display", lines.join("\n")}};
    }
};

// ══════════════════════════════════════════════════════════
// Utility Displays
// ══════════════════════════════════════════════════════════

class ProgressBarNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/progress"; }
    QString nodeName() const override { return "Progress Bar"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Utility"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = qBound(0.0, inputs.value("value").toDouble(), 1.0);
        QImage img(160, 20, QImage::Format_ARGB32);
        img.fill(QColor("#2D2D2D"));
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#00B894"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(2, 2, (v * 156), 16), 3, 3);
        p.setPen(QColor("#FFFFFF"));
        p.setFont(QFont("monospace", 8));
        p.drawText(QRectF(0, 0, 160, 20), Qt::AlignCenter, QString("%1%").arg((int)(v * 100)));
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class GaugeNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/gauge"; }
    QString nodeName() const override { return "Gauge"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Utility"; }
    QString displayColor() const override { return "#FDCB6E"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "value", QVariant(0.5)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = qBound(0.0, inputs.value("value").toDouble(), 1.0);
        QImage img(100, 80, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r(10, 15, 80, 65);
        p.setPen(QPen(QColor("#444444"), 6));
        p.drawArc(r, 225 * 16, 90 * 16);
        p.setPen(QPen(QColor("#FDCB6E"), 6));
        p.drawArc(r, 225 * 16, (int)(v * 90 * 16));
        double angle = 225 + v * 90;
        double rad = qDegreesToRadians(angle);
        double cx = r.center().x(), cy = r.center().y();
        double len = 35;
        p.setPen(QPen(QColor("#FFFFFF"), 2));
        p.drawLine(QPointF(cx, cy), QPointF(cx + len * qCos(rad), cy - len * qSin(rad)));
        p.setPen(QColor("#AAAAAA"));
        p.setFont(QFont("monospace", 8));
        p.drawText(r, Qt::AlignBottom | Qt::AlignHCenter, QString("%1%").arg((int)(v * 100)));
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class CompassNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/compass"; }
    QString nodeName() const override { return "Compass"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Utility"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "degrees", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double deg = fmod(inputs.value("degrees").toDouble(), 360.0);
        if (deg < 0) deg += 360;
        QImage img(80, 80, QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing);
        p.translate(40, 40);
        p.scale(0.9, 0.9);
        p.setPen(QPen(QColor("#636E72"), 2));
        p.setBrush(QColor("#2D2D2D"));
        p.drawEllipse(QPointF(0, 0), 38, 38);
        p.setPen(QColor("#FFFFFF"));
        p.setFont(QFont("monospace", 7, QFont::Bold));
        p.drawText(QRectF(-38, -38, 76, 76), Qt::AlignTop | Qt::AlignHCenter, "N");
        p.drawText(QRectF(-38, -38, 76, 76), Qt::AlignBottom | Qt::AlignHCenter, "S");
        p.drawText(QRectF(-38, -38, 76, 76), Qt::AlignLeft | Qt::AlignVCenter, "W");
        p.drawText(QRectF(-38, -38, 76, 76), Qt::AlignRight | Qt::AlignVCenter, "E");
        p.rotate(deg);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#E17055"));
        QPolygonF arrow;
        arrow << QPointF(0, -30) << QPointF(-6, 0) << QPointF(6, 0);
        p.drawPolygon(arrow);
        p.end();
        return {{"display", imageToBase64(img)}};
    }
};

class FPSMeterNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/fps"; }
    QString nodeName() const override { return "FPS Meter"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "Utility"; }
    QString displayColor() const override { return "#FF6B6B"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        qint64 dt = now - m_lastTime;
        m_lastTime = now;
        if (dt > 0) {
            double fps = 1000.0 / dt;
            m_fps = m_fps * 0.9 + fps * 0.1;
        }
        return {{"display", QString("FPS: %1\nFrame: %2ms").arg(m_fps, 0, 'f', 1).arg(1000.0 / qMax(m_fps, 0.1), 0, 'f', 1)}};
    }

private:
    qint64 m_lastTime = QDateTime::currentMSecsSinceEpoch();
    double m_fps = 60.0;
};

// ══════════════════════════════════════════════════════════
// LED Matrix Display
// ══════════════════════════════════════════════════════════

class LEDMatrixDisplayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/ledMatrix"; }
    QString nodeName() const override { return "LED Matrix Display"; }
    QString nodeCategory() const override { return "Output"; }
    QString nodeSubCategory() const override { return "LED Matrix"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant()},
                {PortType::Int, "width", QVariant(16)},
                {PortType::Int, "height", QVariant(8)},
                {PortType::Color, "fgColor", QVariant::fromValue(QColor("#00CEC9"))},
                {PortType::Color, "bgColor", QVariant::fromValue(QColor("#0A2A2A"))},
                {PortType::Float, "ledSize", QVariant(6.0)},
                {PortType::Float, "spacing", QVariant(2.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override;
};

inline QVariantMap LEDMatrixDisplayNode::compute(const QVariantMap &inputs)
{
    int cols = qBound(1, inputs.value("width").toInt(), 64);
    int rows = qBound(1, inputs.value("height").toInt(), 32);
    double ledSize = qBound(2.0, inputs.value("ledSize").toDouble(), 20.0);
    double gap = qBound(0.0, inputs.value("spacing").toDouble(), 10.0);

    QColor fg = inputs.value("fgColor").value<QColor>();
    QColor bg = inputs.value("bgColor").value<QColor>();
    QString text = inputs.value("value").toString().trimmed();

    int img_w = static_cast<int>(cols * (ledSize + gap) + gap);
    int img_h = static_cast<int>(rows * (ledSize + gap) + gap);

    QImage pix(img_w, img_h, QImage::Format_ARGB32);
    pix.fill(bg);

    if (text.isEmpty()) {
        return {{"display", imageToBase64(pix)}};
    }

    // === Text Layer ===
    QImage textLayer(cols, rows, QImage::Format_ARGB32);
    textLayer.fill(Qt::transparent);

    QPainter tp(&textLayer);
    tp.setRenderHint(QPainter::Antialiasing, false);
    tp.setRenderHint(QPainter::TextAntialiasing, false);
    tp.setRenderHint(QPainter::SmoothPixmapTransform, false);

    QFont font;
    font.setFamily("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setBold(true);
    font.setPixelSize(qMax(6, rows - 1));

    tp.setFont(font);
    tp.setPen(fg);

    QRect textRect(0, 0, cols, rows);
    QFontMetrics fm(font);
    QString displayText = text;

    if (fm.horizontalAdvance(text) > cols) {
        displayText = text.left(cols / 2);
    }

    tp.drawText(textRect, Qt::AlignCenter, displayText);
    tp.end();

    // === Render LEDs with glow ===
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing, true);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double x = gap + c * (ledSize + gap);
            double y = gap + r * (ledSize + gap);

            QRgb pixel = textLayer.pixel(c, r);
            bool isLit = (qAlpha(pixel) > 80);

            QColor color = isLit ? fg : bg;

            // Main LED
            p.setBrush(color);
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(x + ledSize/2.0, y + ledSize/2.0),
                         ledSize/2.0 - 0.5, ledSize/2.0 - 0.5);

            // Glow / Highlight effect (only on lit LEDs)
            if (isLit) {
                p.setBrush(fg.lighter(160));        // brighter inner glow
                p.drawEllipse(QPointF(x + ledSize/2.0, y + ledSize/2.0),
                             ledSize/2.0 - 2.8, ledSize/2.0 - 2.8);
            }
        }
    }

    return {{"display", imageToBase64(pix)}};
}

// ══════════════════════════════════════════════════════════
// Registration
// ══════════════════════════════════════════════════════════

inline void registerOutputNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Output", "Output", QColor("#FF6B6B")});

    registerNodeType<ImageOutputNode>(model, "Output");
    registerNodeType<VideoOutputNode>(model, "Output");
    registerNodeType<ViewportOutputNode>(model, "Output");
    registerNodeType<PixelBufferOutputNode>(model, "Output");
    registerNodeType<FileWriterNode>(model, "Output");
    registerNodeType<JSONExportNode>(model, "Output");
    registerNodeType<PrintNode>(model, "Output");
    registerNodeType<LoggerNode>(model, "Output");
    registerNodeType<GraphInspectorNode>(model, "Output");
    registerNodeType<WatchValueNode>(model, "Output");
    registerNodeType<PerformanceMonitorNode>(model, "Output");

    registerNodeType<NumberDisplayNode>(model, "Output");
    registerNodeType<TextDisplayNode>(model, "Output");
    registerNodeType<BooleanIndicatorNode>(model, "Output");
    registerNodeType<VectorDisplayNode>(model, "Output");
    registerNodeType<LinePlotNode>(model, "Output");
    registerNodeType<ScatterPlotNode>(model, "Output");
    registerNodeType<BarChartNode>(model, "Output");
    registerNodeType<PieChartNode>(model, "Output");
    registerNodeType<HistogramNode>(model, "Output");
    registerNodeType<HeatmapNode>(model, "Output");
    registerNodeType<MatrixDisplayNode>(model, "Output");
    registerNodeType<MemoryViewerNode>(model, "Output");
    registerNodeType<ProgressBarNode>(model, "Output");
    registerNodeType<GaugeNode>(model, "Output");
    registerNodeType<CompassNode>(model, "Output");
    registerNodeType<FPSMeterNode>(model, "Output");
    registerNodeType<LEDMatrixDisplayNode>(model, "Output");
}

} // namespace NodeEditor
