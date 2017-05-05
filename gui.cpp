#include "gui.hpp"

GUI::GUI(QWidget *parent)
    : QMainWindow(parent)
{
    QFont f("Helvetica", 10);
    this->setFont(f);

    LifeField = 0;
    OptionGroup = 0;
    GroupLayout = 0;
    isSimulationOn = false;
    LifeCount = 0;
    IterationCount = 0;

    WindowLayout = new QHBoxLayout;

    QWidget *mycentralwidget = new QWidget;
    mycentralwidget->setLayout(WindowLayout);

    this->setCentralWidget(mycentralwidget);
    this->setWindowTitle("Gra w życie");

    connect(&Algorithm, SIGNAL(CellChanged(int,int,bool)), this, SLOT(FeedCell(int,int,bool)));
    connect(&Algorithm, SIGNAL(ChangeStatus(int,int)), this, SLOT(StatusUpdate(int,int)));
    connect(this, SIGNAL(CellsChanged(int,int,bool)), &Algorithm, SLOT(CellModified(int,int,bool)));

    /* Tutaj połączenie sygnałów i slotów
     */
    connect(this, SIGNAL(NewRows(int)), &Algorithm, SLOT(NewRows(int)));
    connect(this, SIGNAL(NewCols(int)), &Algorithm, SLOT(NewColumns(int)));

    InitLifeField();
    InitGroupBox();
    RowsChanged();
    ColumnsChanged();

    this->setMinimumSize(680, 480);

    connect(ColumnChanger, SIGNAL(valueChanged(int)), this, SLOT(ColumnsChanged()));
    connect(RowChanger, SIGNAL(valueChanged(int)), this, SLOT(RowsChanged()));
    connect(Generator, SIGNAL(clicked()), this, SLOT(GenerateField()));
    connect(Cleaner, SIGNAL(clicked()), this, SLOT(CleanNow()));
    connect(LifeField, SIGNAL(cellClicked(int,int)), this, SLOT(FeedCell(int, int)));
    connect(Starter, SIGNAL(clicked()), this, SLOT(SimulationTrigger()));

    connect(this, SIGNAL(StartSimulation()), &Algorithm, SLOT(SetTimer()));
    connect(this, SIGNAL(StopSimulation()), &Algorithm, SLOT(StopTimer()));
    connect(&Algorithm, SIGNAL(SimulationFinished()), this, SLOT(SimulationTrigger()));

    qsrand(QTime::currentTime().msecsTo(QTime(0, 0, 0, 0)));
}

GUI::~GUI()
{
}

void GUI::InitGroupBox()
{
    if( (GroupLayout != 0) || (OptionGroup != 0) || (WindowLayout == 0) ) return;

    OptionGroup = new QGroupBox;
    OptionGroup->setTitle("Opcje");

    GroupLayout = new QFormLayout(OptionGroup);
    ColumnChanger = new QSpinBox;
    RowChanger = new QSpinBox;
    Starter = new QPushButton("Start / Stop");
    Generator = new QPushButton("Generuj losowo");
    Cleaner = new QPushButton("Wyczyść");
    LifeFeed = new QLCDNumber;
    Iteration = new QLCDNumber;

    Starter->setEnabled(false);

    GroupLayout->addRow("Kolumn:", ColumnChanger);
    GroupLayout->addRow("Wierszy:", RowChanger);

    OptionGroup->setMinimumWidth(120);

    ColumnChanger->setMinimum(20);
    ColumnChanger->setMaximum(300);

    RowChanger->setMinimum(20);
    RowChanger->setMaximum(250);

    ColumnChanger->setValue(60);
    RowChanger->setValue(60);

    Spacers.append(new QSpacerItem(10,15));
    GroupLayout->addItem(Spacers.last());
    LifeFeed->setSegmentStyle(QLCDNumber::Flat);
    Iteration->setSegmentStyle(QLCDNumber::Flat);
    GroupLayout->addRow("Żyje:", LifeFeed);
    GroupLayout->addRow("Kroków:", Iteration);
    LifeFeed->setDigitCount(7);
    Iteration->setDigitCount(7);

    Spacers.append(new QSpacerItem(10, 50));
    GroupLayout->addItem(Spacers.last());
    GroupLayout->setWidget(GroupLayout->rowCount(), QFormLayout::SpanningRole, Generator);

    Spacers.append(new QSpacerItem(10, 20));
    GroupLayout->addItem(Spacers.last());
    GroupLayout->setWidget(GroupLayout->rowCount(), QFormLayout::SpanningRole, Starter);
    GroupLayout->setWidget(GroupLayout->rowCount(), QFormLayout::SpanningRole, Stoper);

    Spacers.append(new QSpacerItem(10, 50));
    GroupLayout->addItem(Spacers.last());
    GroupLayout->setWidget(GroupLayout->rowCount(), QFormLayout::SpanningRole, Cleaner);

    WindowLayout->addWidget(OptionGroup);
}

void GUI::InitLifeField()
{
    if( (LifeField != 0) || (WindowLayout == 0) ) return;

    LifeField = new QTableWidget;

    LifeField->setShowGrid(true);
    LifeField->setSelectionMode(QTableWidget::NoSelection);
    LifeField->horizontalHeader()->hide();
    LifeField->verticalHeader()->hide();

    WindowLayout->addWidget(LifeField);
}

void GUI::RowsChanged()
{
    if(LifeField->rowCount() == RowChanger->value()) return;

    int oldRows = LifeField->rowCount();
    int lifeCropped = 0;
    QTableWidgetItem *item;

    //Gdy wierszy jest mniej
    for(int i = RowChanger->value(); i < oldRows; ++i)
    {
        for(int j = 0; j < LifeField->columnCount(); ++j)
        {
            item = LifeField->item(i,j);
            if(item == 0) continue;
            if(item->data(Qt::UserRole) == true) ++lifeCropped;
        }
    }

    if(lifeCropped > 0) emit StatusUpdate(LifeCount - lifeCropped, IterationCount);

    LifeField->setRowCount(RowChanger->value());

    //Gdy wierszy jest więcej
    for(int i = oldRows; i < RowChanger->value(); ++i)
    {
        LifeField->setRowHeight(i, 5);
    }

    emit NewRows(RowChanger->value()); //Teraz wysyłam odpowiedni sygnał
}

void GUI::ColumnsChanged()
{
    if(LifeField->columnCount() == ColumnChanger->value()) return;

    int oldColumns = LifeField->columnCount();
    int lifeCropped = 0;
    QTableWidgetItem *item;

    //Gdy kolumn jest mniej
    for(int i = ColumnChanger->value(); i < oldColumns; ++i)
    {
        for(int j = 0; j < LifeField->rowCount(); ++j)
        {
            item = LifeField->item(j, i);
            if(item == 0) continue;
            if(item->data(Qt::UserRole) == true) ++lifeCropped;
        }
    }

    if(lifeCropped > 0) emit StatusUpdate(LifeCount - lifeCropped, IterationCount);

    LifeField->setColumnCount(ColumnChanger->value());

    //Gdy kolumn jest więcej
    for(int i = oldColumns; i < ColumnChanger->value(); ++i)
    {
        LifeField->setColumnWidth(i, 5);
    }

    emit NewCols(ColumnChanger->value()); //Teraz wysyłam odpowiedni sygnał
}

void GUI::CleanNow()
{
    LifeField->clear();

    Starter->setEnabled(false);
    Generator->setEnabled(true);
    ColumnChanger->setEnabled(true);
    RowChanger->setEnabled(true);

    LifeFeed->display(0);
    Iteration->display(0);
    LifeCount = 0;
    IterationCount = 0;

    Algorithm.Restart();
    Algorithm.ClearTable(); //Tutaj jak wspominałem, muszę dodatkowo wyczyścić zawartość tablicy
}

void GUI::GenerateField()
{
    bool good = Algorithm.Generate();

    if(good == true) Starter->setEnabled(true);
}

void GUI::SimulationTrigger()
{
    isSimulationOn = !isSimulationOn;

    isSimulationOn ? SimulationOn() : SimulationOff();
}

void GUI::SimulationOn()
{
    Cleaner->setEnabled(false);
    Generator->setEnabled(false);
    ColumnChanger->setEnabled(false);
    RowChanger->setEnabled(false);

    emit StartSimulation();
}

void GUI::SimulationOff()
{
    Cleaner->setEnabled(true);
    Generator->setEnabled(true);
    ColumnChanger->setEnabled(true);
    RowChanger->setEnabled(true);

    emit StopSimulation();
}

void GUI::StatusUpdate(int feeds, int iterations)
{
    LifeFeed->display(feeds);
    Iteration->display(iterations);
    LifeCount = feeds;
    IterationCount = iterations; //Tutaj był błąd - mała literówka

    Starter->setEnabled(feeds);
}

void GUI::FeedCell(int row, int column)
{
    QTableWidgetItem *item = LifeField->item(row, column);
    bool populated;

    if(item == 0)
    {
        item = new QTableWidgetItem;
        LifeField->setItem(row, column, item);
        populated = false;
    }
    else populated = item->data(Qt::UserRole).toBool();

    if(populated == true)
    {
        ItemEmpty(item);
        --LifeCount;
    }
    else
    {
        ItemPopulated(item);
        ++LifeCount;
    }

    emit StatusUpdate(LifeCount, IterationCount);
    emit CellsChanged(row, column, !populated);
}

void GUI::ItemPopulated(QTableWidgetItem * item)
{
    item->setData(Qt::UserRole, QVariant(true));
    item->setBackgroundColor(QColor("darkorange"));
}

void GUI::ItemEmpty(QTableWidgetItem * item)
{
    item->setData(Qt::UserRole, QVariant(false));
    item->setBackgroundColor(QColor("white"));
}

void GUI::FeedCell(int row, int column, bool life)
{
    QTableWidgetItem *item = LifeField->item(row, column);
    if(item == 0)
    {
        item = new QTableWidgetItem;
        LifeField->setItem(row, column, item);
    }
    life == false ? ItemEmpty(item) : ItemPopulated(item);
}
