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

    // setup table to show type and tau
    ui->table->setRowCount(0);
    ui->table->setColumnCount(2);
    ui->table->setHorizontalHeaderLabels(QStringList() << "Type" << "Tau");

    // connect the buttons to their corresponding actions
    connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(addType()));
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
