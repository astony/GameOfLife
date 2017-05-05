#include "conway_alg.hpp"

ConwayAlg::ConwayAlg()
{
    Rows = 0;
    Columns = 0;
    Feeds = 0;
    Iterations = 0;

    isRunning = false;

    connect(&Timer, SIGNAL(timeout()), this, SLOT(Step()));
}

void ConwayAlg::Restart()
{
    isRunning = false;
    Feeds = 0;
    Iterations = 0;
}

bool ConwayAlg::Generate()
{
    bool done = false;
    int percent = 15;
    QInputDialog ask;
    percent = QInputDialog::getInt(0, "Generowanie życia", "Podaj liczbe losową [10 - 70]:",
                         percent, 10, 70, 1, &done, Qt::WindowCloseButtonHint);
    if(done == false)
        return done;

    Restart();

    for(int i = 0; i < Field.size(); ++i)
    {
        for(int j = 0; j < Field.at(i).size(); ++j)
        {
            ((qrand() % 100) + 1 <= percent) == true ? this->setCellLive(i, j) : this->setCellDead(i, j);
        }
    }

    emit ChangeStatus(Feeds, 0);
    return done;
}

/************************************************
 * Opis tych slotów.                            *
 ***********************************************/

void ConwayAlg::NewColumns(int newCol)
{
    /* Nowe kolumny zaczynają się od pierwszego wiersza aż do ostatniego, dlatego tutaj rozpoczynam od i = 0, a warunkiem
     * kończącym pętle jest i < Rows, aktualnej liczby wierszy
     */

    for(int i = 0; i < Rows; ++i)
    {
        Field[i].resize(newCol); //Każdy nowy wiersz rozszerzam o nowe kolumny
    }

    Columns = newCol; //Aktualizuję liczbę kolumn
}

void ConwayAlg::NewRows(int newRow)
{
    /* Tutaj najpierw muszę zwiększyć liczbę wierszy, abym mógł mieć do nich dostęp. */

    Field.resize(newRow);

    /* Tutaj rozpoczynam pętlę od i = Rows, czyli od aktualnej liczby wierszy, a warunkiem kończącym pętlę jest i < newRow, czyli
     * staram się trochę zaoszczędzić czasu i rozszerzam tylko te wiersze, o które powiększyłem moje pole
     */

    for(int i = Rows; i < newRow; ++i)
    {
        Field[i].resize(Columns); //Tutaj rozszerzam każdy wiersz aktualną liczbą kolumn
    }

    Rows = newRow; //Zapisuję nową wartość kolumn do swojego obiektu
}

void ConwayAlg::CellModified(int row, int column, bool life)
{
    if(row > Rows) return;
    if(column > Columns) return;
    Field[row][column] = life;
}

void ConwayAlg::SetTimer()
{
    isRunning = true;
    Timer.setInterval(10); //Ustawiłem na 100, w wersji Release każdy kolejny krok był wywoływany dosyć szybko
    Timer.setSingleShot(true);
    Timer.start();
}

void ConwayAlg::StopTimer()
{
    Timer.stop();
    isRunning = false;
}

void ConwayAlg::Step()
{
    /*Zrezygnowałem z warunku, nie jest on potrzebny, jak już wspominałem w materiale video. Testowałem i po prostu tak zostało już.
     */

    QVector< QVector<bool> > tmp;
    tmp.resize(Rows);
    int live;

    for(int row = 0; row < Rows; ++row)
    {
        tmp[row].resize(Columns);

        for(int col = 0; col < Columns; ++col)
        {
            live = isLive(row, col);
            if(live == -1) tmp[row][col] = false;
            else if(live == 0) tmp[row][col] = Field.at(row).at(col);
            else tmp[row][col] = true;

            emit CellChanged(row, col, tmp.at(row).at(col));
        }
    }

    ++Iterations;
    CopyTable(tmp);
    emit ChangeStatus(Feeds, Iterations);

    /* Tutaj też zrezygnowałem z warunku, po prostu sygnał zostanie przetworzony, gdyż sygnał zostanie przetworzony w momencie wykonania
     * wszystkich metod. Dopóki procesor jest zajęty wykonaniem tej metody, nie zostanie przetworzony odpowiedni slot. Wina pracy w
     * jednym wątku, sygnały i sloty z innych wątków, powinny być przetwarzane.
     */

    if(Feeds > 0) Timer.start();
    else
    {
        emit SimulationFinished();
        Restart();
    }
}
