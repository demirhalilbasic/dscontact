#include "addcontact.h"
#include "ui_addcontact.h"

addcontact::addcontact(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addcontact)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/Icons/dscontact.ico"));
    setWindowTitle("Dodavanje novog kontakta | DSContact v1.0");
}

addcontact::~addcontact()
{
    delete ui;
}

void addcontact::on_pushButton_dodaj_clicked()
{
    QString ime = ui->lineEdit_ime->text();
    QString prezime = ui->lineEdit_prezime->text();
    QString email = ui->lineEdit_email->text();
    QString telefon = ui->lineEdit_telefon->text();
    QString kategorija = ui->comboBox_kategorija->currentText();
    QString rodjendan = ui->dateEdit_rodjendan->text();

    // Emit signal with contact information
    emit contactAdded(ime, prezime, email, telefon, kategorija, rodjendan);

    // Close the dialog
    accept();
}

void addcontact::on_pushButton_odustani_clicked()
{
    // Close the dialog without doing anything
    reject();
}

