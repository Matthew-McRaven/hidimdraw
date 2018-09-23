#ifndef ROTTABLEVALIDATOR_H
#define ROTTABLEVALIDATOR_H
#include <QWidget>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
/*
 * Item delegate that handles input validation of hex constants, and disables editing of address and hex dump columns.
 * Eventually, it can be extended to be signaled to enable or disable editing
 */
class RotTableDelegator: public QStyledItemDelegate {
public:
    RotTableDelegator(QObject* parent = 0);
    virtual ~RotTableDelegator();
    // See http://doc.qt.io/qt-5/qstyleditemdelegate.html#subclassing-qstyleditemdelegate for explanation on the methods being reimplemented.

    // If the index is editable, create an editor that validates byte hex constants, otherwise return nullptr
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // Provides editor widget with starting data for editing.
    virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override;
    // Ensure that editor is displayed correctly on the item view
    virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
    // Handle updating data in the model via calling the memorySection
    virtual void setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const override;
};
#endif // ROTTABLEVALIDATOR_H
