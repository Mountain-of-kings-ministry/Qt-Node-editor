#pragma once

#include <QObject>
#include <QVariant>
#include <QList>
#include <QString>
#include <QHash>
#include <functional>
#include "NodeEditor/GraphModel.h"

namespace NodeEditor {

class BaseNode : public QObject {
    Q_OBJECT
public:
    explicit BaseNode(QObject *parent = nullptr);
    virtual ~BaseNode() = default;

    virtual QVariantMap compute(const QVariantMap &inputs) = 0;
    virtual QList<PortInfo> inputSpec() const = 0;
    virtual QList<PortInfo> outputSpec() const = 0;
    virtual QString nodeType() const = 0;
    virtual QString nodeName() const;
    virtual QString nodeCategory() const;
    virtual QString nodeSubCategory() const;
    virtual QString displayColor() const;

    bool isDirty() const;
    void setDirty(bool dirty = true);

    using Factory = std::function<BaseNode*()>;
    static void registerType(const QString &type, Factory factory);
    static BaseNode *create(const QString &type);

signals:
    void computeComplete();

private:
    bool m_dirty = true;
    static QHash<QString, Factory> s_registry;
};

} // namespace NodeEditor
