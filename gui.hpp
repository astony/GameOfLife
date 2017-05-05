#ifndef GUI_HPP
#define GUI_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QWidget>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QHeaderView>
#include <QSpacerItem>
#include <QVector>
#include <QLCDNumber>
#include <QTime>

#include "conway_alg.hpp"


class GUI : public QMainWindow
{
    Q_OBJECT

public:
    GUI(QWidget *parent = 0);
    ~GUI();

private:
    QHBoxLayout *WindowLayout;
    QTableWidget *LifeField;
    QGroupBox *OptionGroup;
    bool isSimulationOn;
    ConwayAlg Algorithm;

    //**********GROUPBOX***********//
    QFormLayout *GroupLayout;
    QSpinBox *ColumnChanger;
    QSpinBox *RowChanger;
    QPushButton *Starter;
    QPushButton *Stoper;
    QPushButton *Generator;
    QPushButton *Cleaner;
    QVector <QSpacerItem *> Spacers;
    QLCDNumber *LifeFeed;
    QLCDNumber *Iteration;
    int LifeCount;
    int IterationCount;

    void InitGroupBox();
    void SimulationOn();
    void SimulationOff();

    //**********TableWidget***********//
    void InitLifeField();
    void ItemPopulated(QTableWidgetItem * item);
    void ItemEmpty(QTableWidgetItem * item);

private slots:
    void StatusUpdate(int feeds, int iterations);
    void GenerateField();
    void CleanNow();
    void FeedCell(int row, int column);
    void RowsChanged();
    void ColumnsChanged();
    void SimulationTrigger();
    void FeedCell(int row, int column, bool life);

signals:

    /************************************************
     * Dwa sygnały, których sloty omówiłem w pliku  *
     * nagłówkowym algorytmu.                       *
     ***********************************************/

    void NewRows(int row);
    void NewCols(int col);

    void CellsChanged(int row, int column, bool life);
    void StartSimulation();
    void StopSimulation();

};

#endif // GUI_H
