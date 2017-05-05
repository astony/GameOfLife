#ifndef CONWAYALG_HPP
#define CONWAYALG_HPP

#include <QVector>
#include <QInputDialog>
#include <QObject>
#include <QTimer>

class ConwayAlg : public QObject
{
    Q_OBJECT

public:
    ConwayAlg();
    void Restart();
    bool Generate();

    /************************************************
     * Dodałem tę metodę, ponieważ, po wciśnięciu   *
     * "Wyczyść", zawartość pola Field się nie      *
     * czyściła i mogło pojawiąć się z nikąd życie. *
     ***********************************************/

    inline void ClearTable()
    {
        for(int i = 0; i < Rows; ++i)
        {
            for(int j = 0; j < Columns; ++j)
            {
                Field[i][j] = false;
            }
        }
    }

private:
    int Rows;
    int Columns;
    int Feeds;
    int Iterations;

    bool isRunning;
    QVector< QVector<bool> > Field;

    inline void setCellLive(int row, int column)
    {
        Field[row][column] = true;
        ++Feeds;
        emit CellChanged(row, column, true);
    }
    inline void setCellDead(int row, int column)
    {
        Field[row][column] = false;
        emit CellChanged(row, column, false);
    }

    inline void CopyTable(QVector< QVector<bool> > & NewT)
    {
        Feeds = 0;
        for(int i = 0; i < Rows; ++i)
        {
            for(int j = 0; j < Columns; ++j)
            {
                Field[i][j] = NewT.at(i).at(j);
                if(NewT.at(i).at(j) == true) ++Feeds;
            }
        }
    }

    inline int isLive(const int row, const int col)
    {
        int count = 0;

        for(int tmpr = row - 1; tmpr < row + 2; ++tmpr)
        {
            if( (tmpr < 0) || (tmpr >= Rows) ) continue;

            for(int tmpc = col - 1; tmpc < col + 2; ++tmpc)
            {
                if( (tmpc < 0) || (tmpc >= Columns) ) continue;
                if( (tmpr == row) && (tmpc == col) ) continue;
                if(Field.at(tmpr).at(tmpc) == true) ++count;
            }
        }

        //jeśli dana komórka jest pusta
        if(Field.at(row).at(col) == false)
        {
            //ożyje, gdy ma 3 sąsiadów
            if(count == 3) return 1;
            return 0;
        }
        else
        {
            //zyje przy liczbie sąsiadów 2 lub 3
            if( (count > 1) && (count < 4) ) return 0;
            return -1;
        }
    }

    QTimer Timer;

signals:
    void ChangeStatus(int feeds, int iteration);
    void CellChanged(int row, int column, bool life);
    void SimulationFinished();

private slots:
    /************************************************
     * Podmieniłem slot NewDimension(...) na dwa    *
     * inne sloty NewRows(...) oraz NewColumns(...) *
     * Można to było zrobić w jednym slocie, ale    *
     * miałem wcześniej trochę inną koncepcję i już *
     * tak zostało.                                 *
     ***********************************************/
    void NewRows(int newRow);
    void NewColumns(int newCol);

    void CellModified(int row, int column, bool life);
    void SetTimer();
    void StopTimer();
    void Step();

};

#endif // CONWAYALG_H
