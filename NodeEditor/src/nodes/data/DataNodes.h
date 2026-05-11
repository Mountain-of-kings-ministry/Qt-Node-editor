#pragma once

#include "NodeEditor/BaseNode.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// JSON
// ══════════════════════════════════════════════════════════

class ParseJSONNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/json/parse"; }
    QString nodeName() const override { return "Parse JSON"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "JSON"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "json", QVariant("{}")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::JSON, "object", QVariant()}, {PortType::String, "error", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(inputs.value("json").toString().toUtf8(), &err);
        if (err.error != QJsonParseError::NoError)
            return {{"object", QVariant()}, {"error", err.errorString()}};
        return {{"object", doc.toJson(QJsonDocument::Compact)}, {"error", QString()}};
    }
};

class JSONGetNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/json/get"; }
    QString nodeName() const override { return "JSON Get"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "JSON"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::JSON, "object", QVariant()}, {PortType::String, "key", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto doc = QJsonDocument::fromJson(inputs.value("object").toString().toUtf8());
        if (!doc.isObject()) return {{"value", QVariant()}};
        auto obj = doc.object();
        return {{"value", obj.value(inputs.value("key").toString()).toVariant()}};
    }
};

class JSONSetNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/json/set"; }
    QString nodeName() const override { return "JSON Set"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "JSON"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::JSON, "object", QVariant()},
                {PortType::String, "key", QVariant("")},
                {PortType::Generic, "value", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::JSON, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto doc = QJsonDocument::fromJson(inputs.value("object").toString().toUtf8());
        QJsonObject obj = doc.object();
        obj[inputs.value("key").toString()] = QJsonValue::fromVariant(inputs.value("value"));
        return {{"result", QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact))}};
    }
};

class JSONArrayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/json/array"; }
    QString nodeName() const override { return "JSON Array"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "JSON"; }
    QString displayColor() const override { return "#6C5CE7"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Array, "items", QVariantList()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::JSON, "json", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonArray arr = QJsonArray::fromVariantList(inputs.value("items").toList());
        return {{"json", QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact))}};
    }
};

// ══════════════════════════════════════════════════════════
// Serialization
// ══════════════════════════════════════════════════════════

class SerializeObjectNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/serialization/serialize"; }
    QString nodeName() const override { return "Serialize Object"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "Serialization"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "object", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "json", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QJsonObject obj;
        QVariantMap data = inputs.value("object").toMap();
        for (auto it = data.begin(); it != data.end(); ++it)
            obj[it.key()] = QJsonValue::fromVariant(it.value());
        return {{"json", QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact))}};
    }
};

class DeserializeObjectNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/serialization/deserialize"; }
    QString nodeName() const override { return "Deserialize Object"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "Serialization"; }
    QString displayColor() const override { return "#A29BFE"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "json", QVariant("{}")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Map, "object", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto doc = QJsonDocument::fromJson(inputs.value("json").toString().toUtf8());
        return {{"object", doc.object().toVariantMap()}};
    }
};

// ══════════════════════════════════════════════════════════
// File I/O
// ══════════════════════════════════════════════════════════

class ReadFileNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/file/read"; }
    QString nodeName() const override { return "Read File"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "File I/O"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "path", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "content", QVariant()}, {PortType::Bool, "success", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QFile f(inputs.value("path").toString());
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            return {{"content", QString()}, {"success", false}};
        return {{"content", QString::fromUtf8(f.readAll())}, {"success", true}};
    }
};

class WriteFileNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/file/write"; }
    QString nodeName() const override { return "Write File"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "File I/O"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "path", QVariant("")}, {PortType::String, "content", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "success", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QFile f(inputs.value("path").toString());
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            return {{"success", false}};
        f.write(inputs.value("content").toString().toUtf8());
        return {{"success", true}};
    }
};

class CSVParseNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "data/file/csvParse"; }
    QString nodeName() const override { return "CSV Parse"; }
    QString nodeCategory() const override { return "Data"; }
    QString nodeSubCategory() const override { return "File I/O"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "csv", QVariant("")}, {PortType::String, "separator", QVariant(",")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "rows", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QString sep = inputs.value("separator").toString();
        QVariantList rows;
        for (const auto &line : inputs.value("csv").toString().split('\n', Qt::SkipEmptyParts)) {
            QVariantList row;
            for (const auto &cell : line.split(sep))
                row.append(cell.trimmed());
            rows.append(row);
        }
        return {{"rows", rows}};
    }
};

inline void registerDataNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Data", "Data", QColor("#6C5CE7")});
    registerNodeType<ParseJSONNode>(model, "Data");
    registerNodeType<JSONGetNode>(model, "Data");
    registerNodeType<JSONSetNode>(model, "Data");
    registerNodeType<JSONArrayNode>(model, "Data");
    registerNodeType<SerializeObjectNode>(model, "Data");
    registerNodeType<DeserializeObjectNode>(model, "Data");
    registerNodeType<ReadFileNode>(model, "Data");
    registerNodeType<WriteFileNode>(model, "Data");
    registerNodeType<CSVParseNode>(model, "Data");
}

} // namespace NodeEditor
