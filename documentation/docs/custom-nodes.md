# Creating Custom Nodes

## Overview

Custom nodes extend `BaseNode` and provide input/output specs along with a `compute()` function. Nodes are registered with the `GraphModel` using the `registerNodeType<T>` template helper.

## Step-by-Step

### 1. Define a Node Class

```cpp
#include <NodeEditor/BaseNode.h>

class MyCustomNode : public NodeEditor::BaseNode
{
public:
    QString nodeName() const override { return "My Custom Node"; }
    QString nodeType() const override { return "custom/myNode"; }
    QString nodeCategory() const override { return "Custom"; }
    QString nodeSubCategory() const override { return "My Subcategory"; }
    QString displayColor() const override { return "#FF6B6B"; }

    QList<PortInfo> inputSpec() const override {
        return {{PortType::Float, "input", 0.0},
                {PortType::Float, "factor", 1.0}};
    }

    QList<PortInfo> outputSpec() const override {
        return {{PortType::Float, "result"}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        double a = inputs.value("input").toDouble();
        double b = inputs.value("factor").toDouble();
        return {{"result", a * b + a}};
    }
};
```

### 2. Register the Node

Use the `registerNodeType<T>` template helper:

```cpp
#include <NodeEditor/GraphModel.h>
#include "MyCustomNode.h"

void registerCustomNodes(NodeEditor::GraphModel *model)
{
    model->registerCategory({"Custom", "Custom", QColor("#FF6B6B")});
    registerNodeType<MyCustomNode>(model, "Custom");
}
```

The template helper automatically reads all specs (`inputSpec`, `outputSpec`, `nodeName`, `displayColor`, etc.) from the node class, so you don't need to manually construct `NodeTypeInfo`.

### 3. Inline Group Registration

For larger collections, group registrations in a single inline function within the header:

```cpp
inline void registerMathNodeTypes(GraphModel *model)
{
    if (!model) return;
    model->registerCategory({"Math", "Math", QColor("#4A9EFF")});
    registerNodeType<AddNode>(model, "Math");
    registerNodeType<SubtractNode>(model, "Math");
    registerNodeType<MultiplyNode>(model, "Math");
    // ...
}
```

Then call it from `registerDefaultNodeTypes()` in `DefaultNodes.cpp`.

## Creating Display Nodes with Inline Preview

Display nodes render their output visually inside the node body. To make a display node:

### Text-based display (shows formatted text inline)

Simply output a String on the `"display"` port:

```cpp
class MyNumberDisplay : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/myNumber"; }
    QString nodeSubCategory() const override { return "Value"; }
    QString displayColor() const override { return "#00CEC9"; }

    QList<PortInfo> inputSpec() const override {
        return {{PortType::Generic, "value", QVariant(0.0)}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        return {{"display", QString::number(inputs.value("value").toDouble(), 'f', 2)}};
    }
};
```

### Image-based display (shows a chart/graphic inline)

Render a `QImage`, convert to base64 PNG, and output as a String on `"display"`:

```cpp
#include <QImage>
#include <QPainter>
#include <QBuffer>

class MyChartNode : public BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "output/display/myChart"; }
    QString nodeSubCategory() const override { return "Chart"; }

    QList<PortInfo> inputSpec() const override {
        return {{PortType::String, "data", QVariant("[1,2,3,4,5]")}};
    }
    QList<PortInfo> outputSpec() const override {
        return {{PortType::String, "display", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QImage img(160, 80, QImage::Format_ARGB32);
        img.fill(QColor("#1E1E1E"));
        QPainter p(&img);
        // ... draw your chart ...
        p.end();

        QByteArray bytes;
        QBuffer buf(&bytes);
        buf.open(QIODevice::WriteOnly);
        img.save(&buf, "PNG");
        return {{"display", QString::fromLatin1(bytes.toBase64())}};
    }
};
```

The inline preview system in `Node.qml` automatically detects nodes with type starting with `output/display/` and renders their `"display"` output — as an `Image` element for base64 PNG data, or as a `Text` element for plain strings.

### Important

- Use `output/display/` as the type prefix for display nodes
- Output port `"display"` (String) is used for inline preview
- Display nodes are hidden in the Properties panel's output section to avoid showing raw base64/string data

## PortInfo Structure

```cpp
struct PortInfo {
    PortType type = PortType::Generic;  // Data type
    QString name;                        // Port display name
    QVariant defaultValue;               // Default value
};
```

## Node Lifecycle

1. **Construction**: Node is created and its specs are read by the graph model.
2. **Data Inputs**: Values arrive from connected edges or direct data fields.
3. **Computation**: `compute()` is called with current input values.
4. **Outputs**: Results are stored and propagated to downstream nodes.
5. **Inline Preview**: For display nodes, the `"display"` output is rendered inside the node body.

## Best Practices

- Keep compute functions pure (no side effects) for predictable data flow.
- Use meaningful port names that describe the expected input.
- Provide sensible default values for all input ports.
- Register categories before registering nodes that belong to them.
- For display nodes, use `output/display/<name>` as the type prefix to enable inline preview.
