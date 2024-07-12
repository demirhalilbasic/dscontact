#include "editcontact.h"
#include "ui_editcontact.h"

editcontact::editcontact(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editcontact)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/Icons/dscontact.ico"));
    setWindowTitle("Uređivanje kontakta | DSContact v1.0");
}

editcontact::~editcontact()
{
    delete ui;
}

void editcontact::setContactData(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan)
{
    ui->lineEdit_ime->setText(ime);
    ui->lineEdit_prezime->setText(prezime);
    ui->lineEdit_email->setText(email);
    ui->lineEdit_telefon->setText(telefon);
    int index = ui->comboBox_kategorija->findText(kategorija);
    if (index != -1) ui->comboBox_kategorija->setCurrentIndex(index);
    ui->dateEdit_rodjendan->setDate(QDate::fromString(rodjendan, "yyyy-MM-dd"));
}

void editcontact::getContactData(QString &ime, QString &prezime, QString &email, QString &telefon, QString &kategorija, QString &rodjendan) const
{
    ime = ui->lineEdit_ime->text();
    prezime = ui->lineEdit_prezime->text();
    email = ui->lineEdit_email->text();
    telefon = ui->lineEdit_telefon->text();
    kategorija = ui->comboBox_kategorija->currentText();
    rodjendan = ui->dateEdit_rodjendan->text();
}

void editcontact::on_pushButton_dodaj_clicked()
{
    accept();  // Close dialog and indicate success
}

void editcontact::on_pushButton_odustani_clicked()
{
    reject();  // Close dialog and indicate cancellation
}
