# Creating Custom Nodes

## Overview

Custom nodes extend `BaseNode` and provide input/output specs along with a compute function. Nodes are registered with the `GraphModel` via a type string and `NodeTypeInfo`.

## Step-by-Step

### 1. Define a Node Class

```cpp
#include <NodeEditor/BaseNode.h>

class MyCustomNode : public NodeEditor::BaseNode
{
public:
    QString nodeName() const override { return "My Custom Node"; }
    QString nodeType() const override { return "custom/myNode"; }
    QString nodeCategory() const override { return "custom"; }

    QList<PortInfo> inputSpec() const override {
        return {
            { PortType::Float, "input", 0.0f },
            { PortType::Float, "factor", 1.0f }
        };
    }

    QList<PortInfo> outputSpec() const override {
        return {
            { PortType::Float, "result" }
        };
    }

    void compute(const QVariantMap &inputs, QVariantMap &outputs) const override {
        float a = inputs.value("input").toFloat();
        float b = inputs.value("factor").toFloat();
        outputs["result"] = a * b + a;
    }
};
```

### 2. Register the Node

```cpp
#include <NodeEditor/GraphModel.h>
#include "MyCustomNode.h"

void registerNodes(NodeEditor::GraphModel *model)
{
    // Register a category first
    model->registerCategory({ "custom", "Custom", QColor("#FF6B6B") });

    // Register the node type
    MyCustomNode node;
    model->registerNodeType(node.nodeType(), {
        node.inputSpecMap(),
        node.outputSpecMap(),
        "#FF6B6B",
        "custom",
        "",
        node.nodeName()
    });
}
```

### 3. PortInfo Structure

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

## Best Practices

- Keep compute functions pure (no side effects) for predictable data flow.
- Use meaningful port names that describe the expected input.
- Provide sensible default values for all input ports.
- Register categories before registering nodes that belong to them.
