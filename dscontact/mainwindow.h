#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionDodaj_novi_kontakt_triggered();
    void addContactToTable(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan);
    void on_actionUredi_kontakt_triggered();
    void on_actionObrisi_kontakt_triggered();
    void onHeaderClicked(int logicalIndex); // Dodajemo novi slot za klik na zaglavlje kolone

    void on_actionOsvjezi_triggered();

    void on_actionPatch_notes_triggered();

    void on_actionQt_triggered();

    void on_actionUvoz_triggered();

    void on_actionIzvoz_triggered();

    void on_radioButton_toggled(bool checked);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void populateTableFromFile(const QString &filePath);

    void on_lineEdit_search_textChanged(const QString &arg1);

    void filterTable();

    void highlightIfBirthday(int row);

    void on_actionNovi_triggered();

    void on_actionIzadi_triggered();

private:
    Ui::MainWindow *ui;

    // Dodajemo strukturu za praćenje stanja sortiranja
    struct SortState {
        int column = -1;
        Qt::SortOrder order = Qt::AscendingOrder;
    } m_sortState;

    int m_contactActionCount;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
