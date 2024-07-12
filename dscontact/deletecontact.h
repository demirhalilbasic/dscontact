#ifndef DELETECONTACT_H
#define DELETECONTACT_H

#include <QDialog>

namespace Ui {
class deletecontact;
}

class deletecontact : public QDialog
{
    Q_OBJECT

public:
    explicit deletecontact(QWidget *parent = nullptr);
    ~deletecontact();

    void setContactData(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan);

private slots:
    void on_pushButton_obrisi_clicked();
    void on_pushButton_odustani_clicked();

signals:
    void contactDeleted();

private:
    Ui::deletecontact *ui;
};

#endif // DELETECONTACT_H
