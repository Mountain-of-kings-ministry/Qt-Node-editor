#pragma once

#include "NodeEditor/BaseNode.h"
#include <QCryptographicHash>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════
// Flow
// ══════════════════════════════════════════════════════════

class SequenceNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/sequence"; }
    QString nodeName() const override { return "Sequence"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "input", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "a", QVariant()}, {PortType::Float, "b", QVariant()}, {PortType::Float, "c", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        double v = inputs.value("input").toDouble();
        return {{"a", v}, {"b", v}, {"c", v}};
    }
};

class BranchNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/branch"; }
    QString nodeName() const override { return "Branch"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Bool, "condition", QVariant(false)},
                {PortType::Generic, "trueValue", QVariant()},
                {PortType::Generic, "falseValue", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("condition").toBool() ? inputs.value("trueValue") : inputs.value("falseValue")}};
    }
};

class SwitchNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/switch"; }
    QString nodeName() const override { return "Switch"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Int, "index", QVariant(0)},
                {PortType::Generic, "case0", QVariant()},
                {PortType::Generic, "case1", QVariant()},
                {PortType::Generic, "default", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        int idx = inputs.value("index").toInt();
        if (idx == 0) return {{"result", inputs.value("case0")}};
        if (idx == 1) return {{"result", inputs.value("case1")}};
        return {{"result", inputs.value("default")}};
    }
};

class GateNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/gate"; }
    QString nodeName() const override { return "Gate"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "input", QVariant()}, {PortType::Bool, "open", QVariant(true)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        if (inputs.value("open").toBool())
            return {{"output", inputs.value("input")}};
        return {{"output", QVariant()}};
    }
};

class DelayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/delay"; }
    QString nodeName() const override { return "Delay"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "input", QVariant()}, {PortType::Float, "seconds", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"output", inputs.value("input")}};
    }
};

class TimerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/flow/timer"; }
    QString nodeName() const override { return "Timer"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Flow"; }
    QString displayColor() const override { return "#55EFC4"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "interval", QVariant(1.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "tick", QVariant()}, {PortType::Float, "elapsed", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"tick", true}, {"elapsed", 0.0}};
    }
};

// ══════════════════════════════════════════════════════════
// Value Utilities
// ══════════════════════════════════════════════════════════

class RerouteNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/reroute"; }
    QString nodeName() const override { return "Reroute"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "input", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"output", inputs.value("input")}};
    }
};

class CacheNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/cache"; }
    QString nodeName() const override { return "Cache"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "input", QVariant()}, {PortType::Bool, "update", QVariant(true)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"output", inputs.value("input")}};
    }
};

class ConstantNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/constant"; }
    QString nodeName() const override { return "Constant"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"output", inputs.value("value")}};
    }
};

class DebugPrintNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/debugPrint"; }
    QString nodeName() const override { return "Debug Print"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}, {PortType::String, "label", QVariant("Debug")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        qDebug().noquote() << QString("[%1] %2").arg(inputs.value("label").toString()).arg(inputs.value("value").toString());
        return {{"output", inputs.value("value")}};
    }
};

class WatchNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/watch"; }
    QString nodeName() const override { return "Watch"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"output", inputs.value("value")}};
    }
};

class TypeConvertNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/value/typeConvert"; }
    QString nodeName() const override { return "Type Convert"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "input", QVariant()}, {PortType::String, "targetType", QVariant("float")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Generic, "output", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QString type = inputs.value("targetType").toString().toLower();
        QVariant v = inputs.value("input");
        if (type == "float" || type == "double") return {{"output", v.toDouble()}};
        if (type == "int") return {{"output", v.toInt()}};
        if (type == "bool") return {{"output", v.toBool()}};
        if (type == "string") return {{"output", v.toString()}};
        return {{"output", v}};
    }
};

// ══════════════════════════════════════════════════════════
// Collections
// ══════════════════════════════════════════════════════════

class MakeArrayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/collections/makeArray"; }
    QString nodeName() const override { return "Make Array"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Collections"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "item0", QVariant()}, {PortType::Generic, "item1", QVariant()}, {PortType::Generic, "item2", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "array", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantList arr;
        if (inputs.value("item0").isValid()) arr.append(inputs.value("item0"));
        if (inputs.value("item1").isValid()) arr.append(inputs.value("item1"));
        if (inputs.value("item2").isValid()) arr.append(inputs.value("item2"));
        return {{"array", arr}};
    }
};

class AppendArrayNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/collections/appendArray"; }
    QString nodeName() const override { return "Append Array"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Collections"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Array, "array", QVariantList()}, {PortType::Generic, "item", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantList arr = inputs.value("array").toList();
        arr.append(inputs.value("item"));
        return {{"result", arr}};
    }
};

class RemoveArrayItemNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/collections/removeArrayItem"; }
    QString nodeName() const override { return "Remove Array Item"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Collections"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Array, "array", QVariantList()}, {PortType::Int, "index", QVariant(0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantList arr = inputs.value("array").toList();
        int idx = inputs.value("index").toInt();
        if (idx >= 0 && idx < arr.size()) arr.removeAt(idx);
        return {{"result", arr}};
    }
};

class CountNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/collections/count"; }
    QString nodeName() const override { return "Count"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Collections"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Array, "array", QVariantList()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Int, "length", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"length", inputs.value("array").toList().size()}};
    }
};

class ContainsNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/collections/contains"; }
    QString nodeName() const override { return "Contains"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Collections"; }
    QString displayColor() const override { return "#00CEC9"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::Array, "array", QVariantList()}, {PortType::Generic, "item", QVariant()}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "contains", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantList arr = inputs.value("array").toList();
        return {{"contains", arr.contains(inputs.value("item"))}};
    }
};

// ══════════════════════════════════════════════════════════
// String Utilities
// ══════════════════════════════════════════════════════════

class ConcatNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/concat"; }
    QString nodeName() const override { return "Concat"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "a", QVariant("")}, {PortType::String, "b", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("a").toString() + inputs.value("b").toString()}};
    }
};

class SplitNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/split"; }
    QString nodeName() const override { return "Split"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "string", QVariant("")}, {PortType::String, "separator", QVariant(",")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Array, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantList list;
        for (const auto &s : inputs.value("string").toString().split(inputs.value("separator").toString()))
            list.append(s);
        return {{"result", list}};
    }
};

class ReplaceNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/replace"; }
    QString nodeName() const override { return "Replace"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "string", QVariant("")},
                {PortType::String, "from", QVariant("")},
                {PortType::String, "to", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("string").toString().replace(inputs.value("from").toString(), inputs.value("to").toString())}};
    }
};

class RegexNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/regex"; }
    QString nodeName() const override { return "Regex"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "string", QVariant("")}, {PortType::String, "pattern", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::Bool, "match", QVariant()}, {PortType::String, "captured", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        QRegularExpression re(inputs.value("pattern").toString());
        auto m = re.match(inputs.value("string").toString());
        return {{"match", m.hasMatch()}, {"captured", m.hasMatch() ? m.captured(0) : QString()}};
    }
};

class ToUpperNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/toUpper"; }
    QString nodeName() const override { return "To Upper"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "string", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("string").toString().toUpper()}};
    }
};

class ToLowerNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/strings/toLower"; }
    QString nodeName() const override { return "To Lower"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "Strings"; }
    QString displayColor() const override { return "#81ECEC"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "string", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "result", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"result", inputs.value("string").toString().toLower()}};
    }
};

// ══════════════════════════════════════════════════════════
// UUID / Hash
// ══════════════════════════════════════════════════════════

class UUIDGenerateNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/hash/uuidGenerate"; }
    QString nodeName() const override { return "UUID Generate"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "UUID / Hash"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "uuid", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"uuid", QUuid::createUuid().toString(QUuid::WithoutBraces)}};
    }
};

class MD5Node : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/hash/md5"; }
    QString nodeName() const override { return "MD5"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "UUID / Hash"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "hash", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto hash = QCryptographicHash::hash(inputs.value("data").toString().toUtf8(), QCryptographicHash::Md5);
        return {{"hash", QString::fromLatin1(hash.toHex())}};
    }
};

class SHA256Node : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "utility/hash/sha256"; }
    QString nodeName() const override { return "SHA256"; }
    QString nodeCategory() const override { return "Utility"; }
    QString nodeSubCategory() const override { return "UUID / Hash"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "hash", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        auto hash = QCryptographicHash::hash(inputs.value("data").toString().toUtf8(), QCryptographicHash::Sha256);
        return {{"hash", QString::fromLatin1(hash.toHex())}};
    }
};

inline void registerUtilityNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Utility", "Utility", QColor("#00B894")});
    registerNodeType<SequenceNode>(model, "Utility");
    registerNodeType<BranchNode>(model, "Utility");
    registerNodeType<SwitchNode>(model, "Utility");
    registerNodeType<GateNode>(model, "Utility");
    registerNodeType<DelayNode>(model, "Utility");
    registerNodeType<TimerNode>(model, "Utility");
    registerNodeType<RerouteNode>(model, "Utility");
    registerNodeType<CacheNode>(model, "Utility");
    registerNodeType<ConstantNode>(model, "Utility");
    registerNodeType<DebugPrintNode>(model, "Utility");
    registerNodeType<WatchNode>(model, "Utility");
    registerNodeType<TypeConvertNode>(model, "Utility");
    registerNodeType<MakeArrayNode>(model, "Utility");
    registerNodeType<AppendArrayNode>(model, "Utility");
    registerNodeType<RemoveArrayItemNode>(model, "Utility");
    registerNodeType<CountNode>(model, "Utility");
    registerNodeType<ContainsNode>(model, "Utility");
    registerNodeType<ConcatNode>(model, "Utility");
    registerNodeType<SplitNode>(model, "Utility");
    registerNodeType<ReplaceNode>(model, "Utility");
    registerNodeType<RegexNode>(model, "Utility");
    registerNodeType<ToUpperNode>(model, "Utility");
    registerNodeType<ToLowerNode>(model, "Utility");
    registerNodeType<UUIDGenerateNode>(model, "Utility");
    registerNodeType<MD5Node>(model, "Utility");
    registerNodeType<SHA256Node>(model, "Utility");
}

} // namespace NodeEditor
