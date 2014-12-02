#include "changepassdialog.h"
#include "ui_changepassdialog.h"

ChangePassDialog::ChangePassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePassDialog)
{
    ui->setupUi(this);
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(checkPasses()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle("pCloud Drive");
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding); // to have min size for diff OSs
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);  //not resized
}

ChangePassDialog::~ChangePassDialog()
{
    delete ui;
}
void ChangePassDialog::checkPasses()
{
    if (ui->line_newpass->text() != ui->line_newpass2->text())
    {
        //QMessageBox::information(this,trUtf8("New password"), trUtf8("New passwords do not match"));
        QMessageBox::warning(this,trUtf8("New password"), trUtf8("New passwords do not match"));
        return;
    }
    if(ui->line_newpass->text().length() < 6)
    {
        QMessageBox::warning(this,trUtf8("New password"), trUtf8("New passwords should consist of at least six symbols"));
        ui->line_newpass->setFocus();
        return;
    }
    else
        this->accept();
}
