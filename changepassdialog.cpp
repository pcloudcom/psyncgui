#include "changepassdialog.h"
#include "ui_changepassdialog.h"

ChangePassDialog::ChangePassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePassDialog)
{
    ui->setupUi(this);
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
}

ChangePassDialog::~ChangePassDialog()
{
    delete ui;
}
