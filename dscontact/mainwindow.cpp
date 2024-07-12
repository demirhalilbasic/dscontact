#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addcontact.h"
#include "editcontact.h"
#include "deletecontact.h"

#include <QHeaderView>
#include <QDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QCloseEvent>
#include <QIcon>
#include <QLabel>
#include <QPixmap>

enum CustomRoles {
    BirthdayRole = Qt::UserRole + 1
};

class HighlightRowDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyledItemDelegate::paint(painter, option, index);
        if (index.data(CustomRoles::BirthdayRole).toBool()) {
            painter->fillRect(option.rect, "#FFB6C1");

            QString cakeEmoji = "🎂";
            QRect cakeRect = option.rect.adjusted(10, 0, 10, 0);
            painter->drawText(cakeRect, Qt::TextSingleLine | Qt::AlignVCenter, cakeEmoji);

            QFontMetrics metrics(option.font);
            int cakeWidth = metrics.horizontalAdvance(cakeEmoji);
            QRect textRect = option.rect.adjusted(cakeWidth + 15, 0, cakeWidth + 15, 0);
            QString text = index.data().toString();
            painter->drawText(textRect, Qt::TextSingleLine | Qt::AlignVCenter, text);
        }
    }
};

void MainWindow::highlightIfBirthday(int row)
{
    QTableWidget *tableWidget = ui->tableWidget;
    QDate currentDate = QDate::currentDate();
    QString formattedDate = currentDate.toString("MM-dd");

    if(formattedDate == tableWidget->item(row, 5)->text().mid(5,5)) {
        for (int i = 0; i < tableWidget->columnCount(); ++i) {
            tableWidget->item(row, i)->setData(CustomRoles::BirthdayRole, true);
        }
    }
}

void MainWindow::populateTableFromFile(const QString &filePath)
{
    QTableWidget *tableWidget = ui->tableWidget;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file";
        return;
    }
    QTextStream in(&file);
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    tableWidget->setItemDelegate(new HighlightRowDelegate);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        for (int i = 0; i < fields.size(); ++i) {
            tableWidget->setItem(row, i, new QTableWidgetItem(fields[i]));
        }

        highlightIfBirthday(row);
    }

    tableWidget->setColumnHidden(5, true);
    file.close();
}

void MainWindow::filterTable() {
    QString kategorija;
    const QObjectList& groupBoxChildren = ui->groupBox->children();
    for (QObject *groupBoxChild : groupBoxChildren) {
        QRadioButton *radioButton = qobject_cast<QRadioButton*>(groupBoxChild);
        if (radioButton && radioButton->isChecked()) {
            kategorija = radioButton->text();
            break;
        }
    }
    QString query = ui->lineEdit_search->text();

    QTableWidget *tableWidget = ui->tableWidget;
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        bool rowMatches = false;
        for (int col = 0; col < 2; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (item->text().startsWith(query, Qt::CaseInsensitive) && (kategorija == "Svi" || kategorija == tableWidget->item(row, 4)->text())) {
                rowMatches = true;
                break;
            }
        }
        tableWidget->setRowHidden(row, !rowMatches);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_contactActionCount(0)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/Icons/dscontact.ico"));
    setWindowTitle("DSContact v1.0 | Mjesto za sve Vaše kontakte");

    /*QLabel *label = new QLabel(this);
    QPixmap pixmap(":/images/Icons/business_man_usersearch_thesearch_theclient_2356.ico");
    label->setPixmap(pixmap);*/
    QPixmap pixmap(":/images/Icons/business_man_usersearch_thesearch_theclient_2356.ico");
    int width = 32;
    int height = 32;
    QPixmap scaledPixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label->setPixmap(scaledPixmap);

    QObject::connect(ui->radioButton_5, &QRadioButton::toggled, [&](){
        if(ui->radioButton_5->isChecked())
            ui->tableWidget->setStyleSheet("QHeaderView::section { background-color: black; }");
    });

    QObject::connect(ui->radioButton_2, &QRadioButton::toggled, [&](){
        if(ui->radioButton_2->isChecked())
            ui->tableWidget->setStyleSheet("QHeaderView::section { background-color: #002D62; }");
    });

    QObject::connect(ui->radioButton_3, &QRadioButton::toggled, [&](){
        if(ui->radioButton_3->isChecked())
            ui->tableWidget->setStyleSheet("QHeaderView::section { background-color: #558b2f; }");
    });

    QObject::connect(ui->radioButton_4, &QRadioButton::toggled, [&](){
        if(ui->radioButton_4->isChecked())
            ui->tableWidget->setStyleSheet("QHeaderView::section { background-color: #e65100; }");
    });

    QObject::connect(ui->radioButton, &QRadioButton::toggled, [&](){
        if(ui->radioButton->isChecked())
            ui->tableWidget->setStyleSheet("QHeaderView::section { background-color: #8e24aa; }");
    });

    QToolBar *toolBar = findChild<QToolBar*>("toolBar");
    if (toolBar) {
        QSize iconSize(32, 32);
        toolBar->setIconSize(iconSize);
    } else {
        qDebug() << "Toolbar nije pronađen!";
    }


    QTableWidget *tableWidget = ui->tableWidget;
    tableWidget->setColumnCount(6);
    QStringList headers;
    headers << "Ime" << "Prezime" << "Email" << "Telefon" << "Kategorija";
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setRowCount(0);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setDefaultSectionSize(50);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    populateTableFromFile(":/contacts.csv");
    // Inicijalizacija sort state-a
    m_sortState = SortState();

    connect(tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onHeaderClicked);

    const QObjectList& groupBoxChildren = ui->groupBox->children();
    for (QObject *groupBoxChild : groupBoxChildren) {
        QRadioButton *radioButton = qobject_cast<QRadioButton*>(groupBoxChild);
        if (radioButton) {
            connect(radioButton, &QRadioButton::toggled, this, &MainWindow::on_radioButton_toggled);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionDodaj_novi_kontakt_triggered()
{
    m_contactActionCount++;
    addcontact addContact;
    connect(&addContact, &addcontact::contactAdded, this, &MainWindow::addContactToTable);
    addContact.setModal(true);
    addContact.exec();
}

void MainWindow::addContactToTable(const QString &ime, const QString &prezime, const QString &email, const QString &telefon, const QString &kategorija, const QString &rodjendan)
{
    QTableWidget *tableWidget = ui->tableWidget;
    if (tableWidget) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(ime));
        tableWidget->setItem(row, 1, new QTableWidgetItem(prezime));
        tableWidget->setItem(row, 2, new QTableWidgetItem(email));
        tableWidget->setItem(row, 3, new QTableWidgetItem(telefon));
        tableWidget->setItem(row, 4, new QTableWidgetItem(kategorija));
        tableWidget->setItem(row, 5, new QTableWidgetItem(rodjendan));
        highlightIfBirthday(row);
    }
}

void MainWindow::on_actionUredi_kontakt_triggered()
{
    m_contactActionCount++;
    QTableWidget *tableWidget = ui->tableWidget;
    if (tableWidget) {
        int currentRow = tableWidget->currentRow();
        if (currentRow >= 0) {
            QString ime = tableWidget->item(currentRow, 0)->text();
            QString prezime = tableWidget->item(currentRow, 1)->text();
            QString email = tableWidget->item(currentRow, 2)->text();
            QString telefon = tableWidget->item(currentRow, 3)->text();
            QString kategorija = tableWidget->item(currentRow, 4)->text();
            QString rodjendan = tableWidget->item(currentRow, 5)->text();

            editcontact editContactDialog;
            editContactDialog.setContactData(ime, prezime, email, telefon, kategorija, rodjendan);
            if (editContactDialog.exec() == QDialog::Accepted) {
                editContactDialog.getContactData(ime, prezime, email, telefon, kategorija, rodjendan);
                tableWidget->setItem(currentRow, 0, new QTableWidgetItem(ime));
                tableWidget->setItem(currentRow, 1, new QTableWidgetItem(prezime));
                tableWidget->setItem(currentRow, 2, new QTableWidgetItem(email));
                tableWidget->setItem(currentRow, 3, new QTableWidgetItem(telefon));
                tableWidget->setItem(currentRow, 4, new QTableWidgetItem(kategorija));
                tableWidget->setItem(currentRow, 5, new QTableWidgetItem(rodjendan));
                highlightIfBirthday(currentRow);
                filterTable();
            }
        } else {
            QMessageBox::warning(this, tr("Upozorenje"), tr("Molimo odaberite kontakt za uređivanje."));
        }
    }
}

void MainWindow::on_actionObrisi_kontakt_triggered()
{
    m_contactActionCount++;
    QTableWidget *tableWidget = ui->tableWidget;
    if (tableWidget) {
        int currentRow = tableWidget->currentRow();
        if (currentRow >= 0) {
            QString ime = tableWidget->item(currentRow, 0)->text();
            QString prezime = tableWidget->item(currentRow, 1)->text();
            QString email = tableWidget->item(currentRow, 2)->text();
            QString telefon = tableWidget->item(currentRow, 3)->text();
            QString kategorija = tableWidget->item(currentRow, 4)->text();
            QString rodjendan = tableWidget->item(currentRow, 5)->text();

            deletecontact deleteContactDialog;
            deleteContactDialog.setContactData(ime, prezime, email, telefon, kategorija, rodjendan);
            connect(&deleteContactDialog, &deletecontact::contactDeleted, [this, tableWidget, currentRow]() {
                tableWidget->removeRow(currentRow);
            });
            deleteContactDialog.exec();
        } else {
            QMessageBox::warning(this, tr("Upozorenje"), tr("Molimo odaberite kontakt za brisanje."));
        }
    }
}

void MainWindow::onHeaderClicked(int logicalIndex)
{
    QTableWidget *tableWidget = ui->tableWidget;
    if (!tableWidget)
        return;

    // Provjerite je li kliknuta ista kolona kao i prethodno
    bool sameColumnClicked = (m_sortState.column == logicalIndex);

    // Poništite prethodno sortiranje i uklonite strelice ako je kliknuta nova kolona
    if (!sameColumnClicked) {
        // Poništite prethodno sortiranje
        tableWidget->sortByColumn(logicalIndex, Qt::AscendingOrder);

        // Uklonite strelice sa svih kolona
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = tableWidget->horizontalHeaderItem(col);
            if (item) {
                item->setIcon(QIcon()); // Uklonite ikonu
            }
        }
    }

    // Ako je kliknuta ista kolona, promijenite redoslijed sortiranja
    if (sameColumnClicked) {
        m_sortState.order = (m_sortState.order == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        m_sortState.column = logicalIndex;
        m_sortState.order = Qt::AscendingOrder;
    }

    tableWidget->sortItems(m_sortState.column, m_sortState.order);

    // Postavljanje strelice na zaglavlje kolone
    QTableWidgetItem *item = tableWidget->horizontalHeaderItem(m_sortState.column);
    if (item) {
        Qt::ArrowType arrow = (m_sortState.order == Qt::AscendingOrder) ? Qt::ArrowType::DownArrow : Qt::ArrowType::UpArrow;
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
        if (m_sortState.order == Qt::DescendingOrder) {
            icon = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
        }
        item->setIcon(icon);
    }
}

void MainWindow::on_actionOsvjezi_triggered()
{
    QTableWidget *tableWidget = ui->tableWidget;
    if (!tableWidget)
        return;

    // Poništi prethodna sortiranja
    for (int col = 0; col < tableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = tableWidget->horizontalHeaderItem(col);
        if (item) {
            item->setIcon(QIcon()); // Ukloni ikonu
        }
    }

    // Sortiraj abecedno po koloni "Ime"
    tableWidget->sortByColumn(0, Qt::AscendingOrder);
}

void MainWindow::on_actionPatch_notes_triggered()
{
    QMessageBox::information(this, "Aplikacija DSContact",
                             "<strong>Verzija aplikacije</strong><br><br>"
                             "Trenutno koristite Stable v1.0 aplikacije!<br>"
                             "Posljednje ažuriranje dana 25.05.2024. godine.<br><br>"
                             "<strong>O Aplikaciji</strong><br><br>"
                             "Aplikacija <strong>DSContact</strong> nastala je kao projektni zadatak studenata<br>"
                             "Sakib Avdibašić i Demir Halilbašić iz predmeta Objektno programiranje na "
                             "Internacionalnoj poslovno-informacionoj akademiji Tuzla.<br><br>"
                             "<strong>Razvoj aplikacije</strong><br><br>"
                             "<i><u>Alpha v0.1 by Demir | 15.05.2024. | DSContact</u></i><br>"
                             "- Kreiran <i>tableWidget</i>, stiliziran za prikaz na čitavom ekranu, dodana mogućnost sortiranja "
                             "klikom po željenoj koloni, kao i osvježavanje sortiranja<br>"
                             "- Implementirane CRUD funkcionalnosti u okviru <i>tableWidget</i> koje nude mogućnosti dodavanja "
                             "novog, uređivanje postojećeg ili brisanje kontakta<br>"
                             "- Dodane ikonice za CRUD funkcionalnosti, osvježavanje, kao i za sekciju <i>Informacije</i><br><br>"
                             "<i><u>Alpha v0.2 by Sakib | 21.05.2024. | DSContact</u></i><br>"
                             "- Dodana mogućnost filtriranja sadržaja <i>tableWidgeta</i>, na osnovu kategorije, imena ili prezimena<br>"
                             "- Dodana mogućnost učitavanja podataka iz .csv datoteke u <i>tableWidget</i> i sačuvanja "
                             "podataka iz <i>tableWidgeta</i> u .csv datoteku<br>"
                             "- Dodana mogućnost slanja emaila duplim klikom na email bilo kojeg od kontakta<br>"
                             "- Implentirana funkcionalnost isticanja kontakta kojima je rođendan i slanja posebnog emaila istima<br><br>"
                             "<i><u>Stable v1.0 by Demir | 25.05.2024. | DSContact</u></i><br>"
                             "- Dodane <i>Save</i>, <i>Discard</i> i <i>Cancel opcije prilikom kreiranja nove liste kontakata, učitavanja postojeće ili izlaska iz aplikacije</i><br>"
                             "- Stilska dorada: ubačeni <i>WindowTitle</i> za prozore, <i>WindowIcon</i> i ikonica za pretragu<br><br>"
                             "Copyright (C) 2024 DSContact");
}

void MainWindow::on_actionQt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_actionUvoz_triggered()
{
    if (m_contactActionCount == 0) {
        m_contactActionCount = 0;
        QString filePath = QFileDialog::getOpenFileName(nullptr, "Učitaj iz CSV datoteke", "", "CSV datoteke (*.csv)");
        if (!filePath.isEmpty()) {
            populateTableFromFile(filePath);
        }
    } else {
        if (ui->tableWidget->rowCount() > 0) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Upozorenje | DSContact v1.0", "Da li želite sačuvati izmjene prije učitavanja nove liste kontakata?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Save) {
                on_actionIzvoz_triggered();
                m_contactActionCount = 0;
                QString filePath = QFileDialog::getOpenFileName(nullptr, "Učitaj iz CSV datoteke", "", "CSV datoteke (*.csv)");
                if (!filePath.isEmpty()) {
                    populateTableFromFile(filePath);
                }
            } else if (reply == QMessageBox::Discard) {
                m_contactActionCount = 0;
                QString filePath = QFileDialog::getOpenFileName(nullptr, "Učitaj iz CSV datoteke", "", "CSV datoteke (*.csv)");
                if (!filePath.isEmpty()) {
                    populateTableFromFile(filePath);
                }
            }
        } else {
            m_contactActionCount = 0;
            QString filePath = QFileDialog::getOpenFileName(nullptr, "Učitaj iz CSV datoteke", "", "CSV datoteke (*.csv)");
            if (!filePath.isEmpty()) {
                populateTableFromFile(filePath);
            }
        }
    }
}


void MainWindow::on_actionIzvoz_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(nullptr, "Sačuvaj u CSV datoteku", "", "CSV datoteke (*.csv)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Failed to open file for writing";
            return;
        }

        QTextStream out(&file);
        QTableWidget *tableWidget = ui->tableWidget;

        int rowCount = tableWidget->rowCount();
        int columnCount = tableWidget->columnCount();

        for (int row = 0; row < rowCount; ++row) {
            if(!tableWidget->isRowHidden(row)) {
                for (int column = 0; column < columnCount; ++column) {
                    if (column > 0) out << ",";
                    QTableWidgetItem *item = tableWidget->item(row, column);
                    if (item) out << item->text();
                }
                out << "\n";
            }
        }

        file.close();
    }

}

void MainWindow::on_radioButton_toggled(bool checked)
{
    filterTable();
}

void MainWindow::on_lineEdit_search_textChanged(const QString &arg1)
{
    filterTable();
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if (column == 2) {
        QTableWidgetItem *item = ui->tableWidget->item(row, column);
        QString recipient = item->text();
        QDesktopServices::openUrl(QUrl("mailto:" + recipient));

        if (item->data(CustomRoles::BirthdayRole).toBool()) {
            QDesktopServices::openUrl(QUrl("mailto:" + recipient + "?subject=" + QUrl::toPercentEncoding("Sretan rođendan") + "&body=" +
                                           QUrl::toPercentEncoding("Sretan ti rođendan! Želim da ti predstojeća godina bude najuspješnija do sada.")));
        }

    }
}

void MainWindow::on_actionNovi_triggered()
{
    if (m_contactActionCount == 0) {
        m_contactActionCount = 0;
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
    } else {
        if (ui->tableWidget->rowCount() > 0) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Upozorenje | DSContact v1.0", "Da li želite sačuvati izmjene prije kreiranja nove liste kontakata?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Save) {
                on_actionIzvoz_triggered();
                m_contactActionCount = 0;
                ui->tableWidget->clearContents();
                ui->tableWidget->setRowCount(0);
            } else if (reply == QMessageBox::Discard) {
                m_contactActionCount = 0;
                ui->tableWidget->clearContents();
                ui->tableWidget->setRowCount(0);
            }
        } else {
            m_contactActionCount = 0;
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);
        }
    }
}

void MainWindow::on_actionIzadi_triggered()
{
    if (m_contactActionCount == 0) {
        m_contactActionCount = 0;
        QApplication::quit();
    } else {
        if (ui->tableWidget->rowCount() > 0) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Upozorenje | DSContact v1.0", "Da li želite sačuvati izmjene prije izlaska iz aplikacije?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Save) {
                on_actionIzvoz_triggered();
                m_contactActionCount = 0;
                QApplication::quit();
            } else if (reply == QMessageBox::Discard) {
                m_contactActionCount = 0;
                QApplication::quit();
            }
        } else {
            m_contactActionCount = 0;
            QApplication::quit();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_contactActionCount == 0) {
        m_contactActionCount = 0;
        event->accept();
    } else {
        if (ui->tableWidget->rowCount() > 0) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Upozorenje | DSContact v1.0", "Da li želite sačuvati izmjene prije izlaska iz aplikacije?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Save) {
                on_actionIzvoz_triggered();
                m_contactActionCount = 0;
                event->accept();
            } else if (reply == QMessageBox::Discard) {
                m_contactActionCount = 0;
                event->accept();
            } else {
                event->ignore();
            }
        } else {
            m_contactActionCount = 0;
            event->accept();
        }
    }
}
