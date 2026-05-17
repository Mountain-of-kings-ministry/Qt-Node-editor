#pragma once

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QThreadPool>
#include <QAtomicInt>
#include <QMutex>
#include <QSharedPointer>
#include "NodeEditor/GraphModel.h"

namespace NodeEditor {

class BaseNode;

struct EvalSnapshot {
    QHash<uint64_t, GraphNode> nodes;
    QHash<uint64_t, GraphEdge> edges;
    QHash<uint64_t, QVariantMap> data;
    QVector<QList<uint64_t>> levels;
    QSet<uint64_t> dirty;
    uint64_t changeRoot = 0;

    QHash<uint64_t, QVariantMap> results;
    QList<uint64_t> computedOrder;
    QMutex resultMutex;
};

class AsyncEvaluator : public QObject {
    Q_OBJECT
    Q_PROPERTY(int maxThreads READ maxThreads WRITE setMaxThreads NOTIFY maxThreadsChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
public:
    explicit AsyncEvaluator(QObject *parent = nullptr);
    explicit AsyncEvaluator(GraphModel *model, QObject *parent = nullptr);
    ~AsyncEvaluator() override;

    void setGraphModel(GraphModel *model);
    GraphModel *graphModel() const;

    int maxThreads() const;
    void setMaxThreads(int n);
    bool isBusy() const;

    void evaluateDirty(const QSet<uint64_t> &dirtyNodes);
    void evaluateFrom(uint64_t nodeId, const QSet<uint64_t> &downstream);
    Q_INVOKABLE void qmlEvaluateDirty(const QStringList &dirtyIds);
    Q_INVOKABLE void qmlEvaluateFrom(const QString &nodeId);

signals:
    void started();
    void finished(QList<uint64_t> computed);
    void levelStarted(int level, int totalLevels);
    void levelFinished(int level);
    void nodeError(uint64_t nodeId, QString message);
    void busyChanged();
    void maxThreadsChanged();

    void qmlFinished(QStringList computedIds);

private:
    void buildSnapshot(const QSet<uint64_t> &dirtyNodes,
                       uint64_t changeRoot,
                       QSharedPointer<EvalSnapshot> &snapshot);
    void mergeResults(QSharedPointer<EvalSnapshot> snapshot,
                      const QList<uint64_t> &computed);
    void kickLevel(int level, QSharedPointer<EvalSnapshot> snapshot);
    void doEvalNode(uint64_t nodeId, QSharedPointer<EvalSnapshot> snapshot);

    GraphModel *m_model = nullptr;
    QThreadPool m_pool;
    int m_maxThreads = 4;
    bool m_busy = false;
    QHash<QString, BaseNode*> m_instances;
    QMutex m_instanceMutex;
};

} // namespace NodeEditor
