#ifndef IMAGE_SELECTION_PANEL_H
#define IMAGE_SELECTION_PANEL_H

#include <QWidget>
#include <QListView>
#include <QShortcut>

#include "image_selection.h"


class ImageSelectionPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSelectionPanel(QWidget *parent = nullptr);

    ImageSelection *model() const { return m_model; }
    QListView *view() const { return m_view; }

private:
    void remove(const QModelIndex &index);
    void removeSelection();

    ImageSelection *m_model;
    QListView *m_view;
};

#endif // IMAGE_SELECTION_PANEL_H
