#ifndef ACCEPTSHAREDIALOG_H
#define ACCEPTSHAREDIALOG_H

#include "psynclib.h"
#include "common.h"
#include <QDialog>

namespace Ui {
class AcceptShareDialog;
}

class AcceptShareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AcceptShareDialog(const char* name, QWidget *parent = 0);
    ~AcceptShareDialog();
    quint64 getFldrid();
    const char* getShareName();
private:
    Ui::AcceptShareDialog *ui;
    quint64 fldrid;
    const char* sharename;
    void initTree();
public slots:
    void addRequest();
};

#endif // ACCEPTSHAREDIALOG_H
