#pragma once

#include "NodeEditor/BaseNode.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStack>

namespace NodeEditor {

// JSONScriptInterpreterNode: executes simple JSON-based scripts
// Script format: [{"cmd":"push","args":[5]},{"cmd":"push","args":[3]},{"cmd":"add","args":[]},{"cmd":"print","args":[]}]
// Commands: push, add, sub, mul, div, print, dup, swap, drop
class JSONScriptInterpreterNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;

    QVariantMap compute(const QVariantMap &inputs) override
    {
        QVariantMap out;
        QString scriptStr = inputs.value("script").toString();
        bool ok = false;
        double initial = inputs.value("initialValue").toDouble(&ok);

        QJsonDocument doc = QJsonDocument::fromJson(scriptStr.toUtf8());
        if (doc.isNull() || !doc.isArray()) {
            out["result"] = QVariant();
            out["error"] = "Invalid JSON script";
            out["stackStr"] = "ERROR";
            return out;
        }

        QStack<double> stack;
        if (ok) stack.push(initial);

        QJsonArray cmds = doc.array();
        for (const QJsonValue &cmdVal : cmds) {
            QJsonObject cmd = cmdVal.toObject();
            QString cmdName = cmd["cmd"].toString();
            QJsonArray args = cmd["args"].toArray();

            if (cmdName == "push") {
                for (const QJsonValue &arg : args)
                    stack.push(arg.toDouble());
            } else if (cmdName == "add") {
                if (stack.size() < 2) { out["error"] = "Stack underflow"; break; }
                double b = stack.pop(), a = stack.pop();
                stack.push(a + b);
            } else if (cmdName == "sub") {
                if (stack.size() < 2) { out["error"] = "Stack underflow"; break; }
                double b = stack.pop(), a = stack.pop();
                stack.push(a - b);
            } else if (cmdName == "mul") {
                if (stack.size() < 2) { out["error"] = "Stack underflow"; break; }
                double b = stack.pop(), a = stack.pop();
                stack.push(a * b);
            } else if (cmdName == "div") {
                if (stack.size() < 2) { out["error"] = "Stack underflow"; break; }
                double b = stack.pop(), a = stack.pop();
                if (b == 0) { out["error"] = "Division by zero"; break; }
                stack.push(a / b);
            } else if (cmdName == "dup") {
                if (stack.isEmpty()) { out["error"] = "Stack underflow"; break; }
                stack.push(stack.top());
            } else if (cmdName == "swap") {
                if (stack.size() < 2) { out["error"] = "Stack underflow"; break; }
                double a = stack.pop(), b = stack.pop();
                stack.push(a); stack.push(b);
            } else if (cmdName == "drop") {
                if (stack.isEmpty()) { out["error"] = "Stack underflow"; break; }
                stack.pop();
            } else if (cmdName == "print") {
                // Print is handled via the output string
            } else {
                out["error"] = QString("Unknown command: %1").arg(cmdName);
                break;
            }
        }

        // Build stack string
        QStringList parts;
        for (double v : stack)
            parts << QString::number(v, 'f', 2);
        out["stackStr"] = parts.isEmpty() ? "(empty)" : parts.join(" | ");

        if (!stack.isEmpty())
            out["result"] = stack.top();

        return out;
    }

    QList<PortInfo> inputSpec() const override
    {
        return {{PortType::String, "script", QVariant(R"([{"cmd":"push","args":[5]},{"cmd":"push","args":[3]},{"cmd":"add","args":[]},{"cmd":"print","args":[]}])")},
                {PortType::Float, "initialValue", QVariant(0.0)}};
    }

    QList<PortInfo> outputSpec() const override
    {
        return {{PortType::Float, "result", QVariant()},
                {PortType::String, "stackStr", QVariant()},
                {PortType::String, "error", QVariant()}};
    }

    QString nodeType() const override { return "script/jsonInterpreter"; }
    QString nodeName() const override { return "JSON Script Interpreter"; }
    QString nodeCategory() const override { return "Scripting"; }
    QString nodeSubCategory() const override { return "JSON"; }
    QString displayColor() const override { return "#A29BFE"; }
};

inline void registerScriptNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Scripting", "Scripting", QColor("#A29BFE")});
    registerNodeType<JSONScriptInterpreterNode>(model, "Scripting");
}

} // namespace NodeEditor
