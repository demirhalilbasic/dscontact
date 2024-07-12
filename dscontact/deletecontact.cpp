#include "deletecontact.h"
#include "ui_deletecontact.h"

deletecontact::deletecontact(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deletecontact)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/Icons/dscontact.ico"));
    setWindowTitle("Brisanje kontakta | DSContact v1.0");
}

deletecontact::~deletecontact()
{
    delete ui;
}

void deletecontact::setContactData(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan)
{
    ui->label_ime->setText(ime);
    ui->label_prezime->setText(prezime);
    ui->label_email->setText(email);
    ui->label_telefon->setText(telefon);
    ui->label_kategorija->setText(kategorija);
    ui->label_rodjendan->setText(rodjendan);
}

void deletecontact::on_pushButton_obrisi_clicked()
{
    emit contactDeleted();
    accept();
}

void deletecontact::on_pushButton_odustani_clicked()
{
    reject();
}
