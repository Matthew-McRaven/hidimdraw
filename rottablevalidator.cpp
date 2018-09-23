#include "rottablevalidator.h"
#include <QLineEdit>
RotTableDelegator::RotTableDelegator(QObject *parent): QStyledItemDelegate(parent)
{

}

RotTableDelegator::~RotTableDelegator()
{

}

QWidget *RotTableDelegator::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // The first column is not user editable, so do not create an editor.
    if(index.column() == 0) return 0;
    // Otherwise, defer to QStyledItemDelegate's implementation, which returns a LineEdit
    QLineEdit *line = qobject_cast<QLineEdit*>(QStyledItemDelegate::createEditor(parent, option, index));
    // Apply a validator, so that a user cannot input anything other than a one byte hexadecimal constant
    line->setValidator(new QRegExpValidator(QRegExp("[0-9]+\.[0-9]+"), line));
    return line;
}

void RotTableDelegator::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // The default value in the line edit should be the text currently in that cell.
    QString value = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *line = static_cast<QLineEdit*>(editor);
        line->setText(value);
}

void RotTableDelegator::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    // Pass geometry information to the editor.
    editor->setGeometry(option.rect);
}

void RotTableDelegator::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Get text from editor and convert it to a integer
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    QString value = line->text();
    model->setData(index,value);
}
