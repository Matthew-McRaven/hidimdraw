#ifndef ROTTABLEVALIDATOR_H
#define ROTTABLEVALIDATOR_H
#include <QWidget>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>

/*
 * Class to handle input validation and custom editor creation in the rotation table of the main window.
 * Replaces QStyledItemDelegate's default ediotr (QLineEdit) with a double spin box. By using a double spin box,
 * and picking a reasonable minimum and maximum, no additional input validation must be performed.
 */
class RotTableDelegator: public QStyledItemDelegate {
    static constexpr double MIN_VAL = -4, MAX_VAL = 4;
public:
    RotTableDelegator(QObject* parent = nullptr);
    virtual ~RotTableDelegator() override;
    // See http://doc.qt.io/qt-5/qstyleditemdelegate.html#subclassing-qstyleditemdelegate for explanation on the methods being reimplemented.

    // If the index is editable, create an double spin box editor, otherwise return nullptr.
    // Returns a QDoubleSpinBox, since it has more intuitive controls for floating point manipulation and input validation.
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // Provides editor widget with starting data for editing.
    virtual void setEditorData(QWidget * editor, const QModelIndex &index) const override;
    // Ensure that editor is displayed correctly on the item view
    virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // Update rotation model with new data
    virtual void setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const override;
};
#endif // ROTTABLEVALIDATOR_H
