#pragma once

#include "NodeEditor/BaseNode.h"
#include <QTime>
#include <QProcessEnvironment>
#include <QSysInfo>
#include <QClipboard>
#include <QGuiApplication>

namespace NodeEditor {

class CurrentTimeNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "system/info/currentTime"; }
    QString nodeName() const override { return "Current Time"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "Info"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "time", QVariant()}, {PortType::Float, "timestamp", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        auto now = QDateTime::currentDateTime();
        return {{"time", now.toString(Qt::ISODate)}, {"timestamp", double(now.toMSecsSinceEpoch()) / 1000.0}};
    }
};

class EnvironmentVariableNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "system/info/envVar"; }
    QString nodeName() const override { return "Environment Variable"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "Info"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "name", QVariant("PATH")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "value", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"value", QProcessEnvironment::systemEnvironment().value(inputs.value("name").toString())}};
    }
};

class OSInfoNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "system/info/osInfo"; }
    QString nodeName() const override { return "OS Info"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "Info"; }
    QString displayColor() const override { return "#636E72"; }
    QList<PortInfo> inputSpec() const override {
        return {};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "os", QVariant()}, {PortType::String, "kernel", QVariant()}, {PortType::String, "architecture", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"os", QSysInfo::prettyProductName()},
                {"kernel", QSysInfo::kernelType()},
                {"architecture", QSysInfo::currentCpuArchitecture()}};
    }
};

class ClipboardNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "system/io/clipboard"; }
    QString nodeName() const override { return "Clipboard"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "I/O"; }
    QString displayColor() const override { return "#00B894"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "text", QVariant("")}, {PortType::Bool, "set", QVariant(false)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "text", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &inputs) override {
        if (inputs.value("set").toBool()) {
            QGuiApplication::clipboard()->setText(inputs.value("text").toString());
        }
        return {{"text", QGuiApplication::clipboard()->text()}};
    }
};

class ProcessExecuteNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "system/process/execute"; }
    QString nodeName() const override { return "Process Execute"; }
    QString nodeCategory() const override { return "System"; }
    QString nodeSubCategory() const override { return "Process"; }
    QString displayColor() const override { return "#E17055"; }
    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "command", QVariant("")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "output", QVariant()}, {PortType::Bool, "success", QVariant()}};
    }
    QVariantMap compute(const QVariantMap &) override {
        return {{"output", "Process execution (stub)"}, {"success", false}};
    }
};

} // namespace NodeEditor
