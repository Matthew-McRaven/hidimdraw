#include "rottablevalidator.h"
#include <QLineEdit>
#include <QDoubleSpinBox>
RotTableDelegator::RotTableDelegator(QObject *parent): QStyledItemDelegate(parent)
{

}

RotTableDelegator::~RotTableDelegator()
{

}

QWidget *RotTableDelegator::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    // The first column is not user editable, so do not create an editor.
    if(index.column() == 0) return 0;

    // Otherwise, defer to QStyledItemDelegate's implementation
    QDoubleSpinBox *spin = new QDoubleSpinBox(parent);
    spin->setSingleStep(.1);
    spin->setMinimum(MIN_VAL);
    spin->setMaximum(MAX_VAL);
    return spin;
}

void RotTableDelegator::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // The default value in the line edit should be the text currently in that cell.
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox *spin = static_cast<QDoubleSpinBox*>(editor);
    spin->setValue(value);
}

void RotTableDelegator::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    // Pass geometry information to the editor.
    editor->setGeometry(option.rect);
}

void RotTableDelegator::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Get text from editor and convert it to a integer
    QDoubleSpinBox *spin = static_cast<QDoubleSpinBox*>(editor);
    double value = spin->value();
    model->setData(index, value);
}
