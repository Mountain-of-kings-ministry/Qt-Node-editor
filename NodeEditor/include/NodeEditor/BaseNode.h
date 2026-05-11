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

// Helper template to register a node class with both factory and metadata
template<typename T>
inline void registerNodeType(GraphModel *model, const QString &categoryId)
{
    T tmp;
    BaseNode::registerType(tmp.nodeType(), []() { return new T(); });
    NodeTypeInfo info;
    for (const auto &p : tmp.inputSpec()) info.inputs[p.name] = p;
    for (const auto &p : tmp.outputSpec()) info.outputs[p.name] = p;
    info.displayColor = tmp.displayColor();
    info.categoryId = categoryId;
    info.subCategory = tmp.nodeSubCategory();
    info.nodeName = tmp.nodeName();
    model->registerNodeType(tmp.nodeType(), info);
}

} // namespace NodeEditor
