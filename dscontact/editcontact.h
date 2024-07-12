#ifndef EDITCONTACT_H
#define EDITCONTACT_H

#include <QDialog>

namespace Ui {
class editcontact;
}

class editcontact : public QDialog
{
    Q_OBJECT

public:
    explicit editcontact(QWidget *parent = nullptr);
    ~editcontact();

    void setContactData(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan);
    void getContactData(QString &ime, QString &prezime, QString &email, QString &telefon, QString &kategorija, QString &rodjendan) const;

private slots:
    void on_pushButton_dodaj_clicked();
    void on_pushButton_odustani_clicked();

private:
    Ui::editcontact *ui;
};

#endif // EDITCONTACT_H
