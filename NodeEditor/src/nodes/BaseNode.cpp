#include "NodeEditor/BaseNode.h"
#include <QBuffer>

namespace NodeEditor {

static QString imageToBase64(const QImage &img)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    return QString::fromLatin1(bytes.toBase64());
}

QHash<QString, BaseNode::Factory> BaseNode::s_registry;

BaseNode::BaseNode(QObject *parent)
    : QObject(parent)
{
}

QString BaseNode::nodeName() const
{
    auto parts = nodeType().split('/');
    return parts.isEmpty() ? nodeType() : parts.last();
}

QString BaseNode::nodeCategory() const
{
    return nodeType().split('/').value(0);
}

QString BaseNode::nodeSubCategory() const
{
    return nodeType().split('/').value(1);
}

QString BaseNode::displayColor() const
{
    return "#4A9EFF";
}

bool BaseNode::isDirty() const
{
    return m_dirty;
}

void BaseNode::setDirty(bool dirty)
{
    m_dirty = dirty;
}

QImage BaseNode::render(const QVariantMap &, QSize)
{
    return {};
}

QString BaseNode::renderToBase64(const QVariantMap &inputs, QSize maxSize)
{
    QImage img = render(inputs, maxSize);
    if (img.isNull()) return {};
    return imageToBase64(img);
}

void BaseNode::registerType(const QString &type, Factory factory)
{
    s_registry[type] = factory;
}

BaseNode *BaseNode::create(const QString &type)
{
    auto it = s_registry.find(type);
    if (it != s_registry.end())
        return it.value()();
    return nullptr;
}

} // namespace NodeEditor
