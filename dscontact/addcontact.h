#ifndef ADDCONTACT_H
#define ADDCONTACT_H

#include <QDialog>
#include <QDate>

namespace Ui {
class addcontact;
}

class addcontact : public QDialog
{
    Q_OBJECT

public:
    explicit addcontact(QWidget *parent = nullptr);
    ~addcontact();

signals:
    void contactAdded(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan);

private slots:
    void on_pushButton_dodaj_clicked();
    void on_pushButton_odustani_clicked();

private:
    Ui::addcontact *ui;
};

#endif // ADDCONTACT_H
