#pragma once

#include <QObject>
#include <QVariant>
#include <QList>
#include <QString>
#include <QHash>
#include <QImage>
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

    // Render preview image (separate from compute for async preview generation)
    // Returns null QImage if not supported (uses compute() "display" output instead)
    virtual QImage render(const QVariantMap &inputs, QSize maxSize = {});
    // Helper: render + base64 encode
    QString renderToBase64(const QVariantMap &inputs, QSize maxSize = {});

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
