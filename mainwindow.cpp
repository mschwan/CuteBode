/*
    This file is part of CuteBode.

    CuteBode is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CuteBode is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CuteBode.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // create user interface
    ui->setupUi(this);

    // add plot widgets
    plot = new Plot(this);
    plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    ui->layoutPlot->addWidget(plot);

    // setup table to show type and tau
    ui->table->setRowCount(0);
    ui->table->setColumnCount(2);
    ui->table->setHorizontalHeaderLabels(QStringList() << "Type" << "Tau");

    // connect the buttons to their corresponding actions
    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(addType()));
    connect(ui->buttonRemove, SIGNAL(clicked()), this, SLOT(deleteType()));
    connect(ui->buttonView, SIGNAL(clicked()), this, SLOT(viewPlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addType()
{
    // add a row for new widgets
    ui->table->setRowCount(ui->table->rowCount() + 1);

    // the type
    QComboBox *type = new QComboBox();
    type->addItems(QStringList() << "None" << "1" << "2" << "3" << "4");
    ui->table->setCellWidget(ui->table->rowCount() - 1, 0, type);

    // and tau
    QDoubleSpinBox *tau = new QDoubleSpinBox();
    ui->table->setCellWidget(ui->table->rowCount() - 1, 1, tau);
}

void MainWindow::deleteType()
{
    if(ui->table->currentRow() >= 0) { // row exists?
        // then delete it!
        ui->table->removeRow(ui->table->currentRow());
    }
}

void MainWindow::viewPlot()
{
    // clear the list before we fill it
    while(!trfList.isEmpty()) {
        trfList.removeLast();
    }

    for(int i = 0; i < ui->table->rowCount(); i++) {
        QComboBox *comboBox;
        QDoubleSpinBox *spinBox;
        Trf *trf = new Trf();

        comboBox = dynamic_cast<QComboBox *>(ui->table->cellWidget(i, 0));
        spinBox = dynamic_cast<QDoubleSpinBox *>(ui->table->cellWidget(i, 1));

        switch(comboBox->currentText().toInt()) {
        case 1:
            trf->setType(Trf::Type1);
            break;
        case 2:
            trf->setType(Trf::Type2);
            break;
        case 3:
            trf->setType(Trf::Type3);
            break;
        case 4:
            trf->setType(Trf::Type4);
            break;
        default:
            delete trf;
            continue;
        }

        trf->setTau(spinBox->value());
        trfList.append(trf);
    }

    // for debug purposes only
    qDebug() << "----------";
    foreach(Trf *t, trfList) {
        qDebug() << t << t->getType() << t->getTau();
    }

    plot->calculateMagnitude(trfList);

    qDebug() << "---";
    foreach(QPointF *p, plot->magnitude) {
        qDebug() << p << p->x() << p->y();
    }

    plot->plot();
}
