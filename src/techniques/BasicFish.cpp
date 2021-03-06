#include "Sudoku.hpp"
#include "Logging.hpp"
#include "LockedSet.hpp"

#include <sstream>

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/combination.hpp>

namespace {
typedef boost::tuple<Index_t, Index_t, Index_t> RowColVal;

bool BasicFishWithOrder(Sudoku &, Index_t);
Index_t MaxSizeOfBasicFish(const Sudoku &, Index_t value);
std::vector<Index_t> IndicesOfPossibleRowBaseBasicFish(const Sudoku &, Index_t, Index_t);
std::vector<Index_t> IndicesOfPossibleColBaseBasicFish(const Sudoku &, Index_t, Index_t);
bool RowBaseBasicFish(Sudoku &, Index_t val, Index_t order);
bool RowBaseBasicFishWithIndices(Sudoku &, Index_t val, std::vector<Index_t> &indices, Index_t order);
bool ColBaseBasicFish(Sudoku &, Index_t val, Index_t order);
bool ColBaseBasicFishWithIndices(Sudoku &, Index_t val, std::vector<Index_t> &indices, Index_t order);
void LogBasicFish(bool rowBase, const std::vector<Index_t> &rows,
        const std::vector<Index_t> &cols,
        const std::vector<RowColVal> &changed,
        Index_t value, Index_t order);
const char *OrderToString(Index_t order);
}

bool XWing(Sudoku &sudoku)
{
    Log(Trace, "searching for x-wings\n");
    return BasicFishWithOrder(sudoku, 2);
}

bool Swordfish(Sudoku &sudoku)
{
    Log(Trace, "searching for swordfish\n");
    return BasicFishWithOrder(sudoku, 3);
}

bool Jellyfish(Sudoku &sudoku)
{
    Log(Trace, "searching for jellyfish\n");
    return BasicFishWithOrder(sudoku, 4);
}

namespace {

bool BasicFishWithOrder(Sudoku &sudoku, Index_t order)
{
    for (Index_t val = 1; val <= 9; ++val) {
        if (RowBaseBasicFish(sudoku, val, order))
            return true;
        if (ColBaseBasicFish(sudoku, val, order))
            return true;
    }
    return false;
}

Index_t MaxSizeOfBasicFish(const Sudoku &sudoku, Index_t value)
{
    Index_t cnt = 0;
    for (Index_t i = 0; i < 9; ++i) {
        for (Index_t j = 0; j < 9; ++j) {
            Cell cell = sudoku.GetCell(i, j);
            if (cell.HasValue() && cell.GetValue() == value)
                ++cnt;
        }
    }
    return (9 - cnt)/2;
}


std::vector<Index_t> IndicesOfPossibleRowBaseBasicFish(const Sudoku &sudoku, Index_t value, Index_t order)
{
    std::vector<Index_t> ret;
    for (Index_t i = 0; i < 9; ++i) {
        Index_t num = NumTimesValueOpenInHouse(sudoku.GetRow(i), value);
        if (num <= order && num != 0)
            ret.push_back(i);
    }
    return ret;
}

std::vector<Index_t> IndicesOfPossibleColBaseBasicFish(const Sudoku &sudoku, Index_t value, Index_t order)
{
    std::vector<Index_t> ret;
    for (Index_t i = 0; i < 9; ++i) {
        Index_t num = NumTimesValueOpenInHouse(sudoku.GetCol(i), value);
        if (num <= order && num != 0)
            ret.push_back(i);
    }
    return ret;
}

bool RowBaseBasicFish(Sudoku &sudoku, Index_t value, Index_t order)
{
    if (MaxSizeOfBasicFish(sudoku, value) < order)
        return false;

    std::vector<Index_t> indices =
        IndicesOfPossibleRowBaseBasicFish(sudoku, value, order);
    if (indices.size() < order)
        return false;

    do {
        if (RowBaseBasicFishWithIndices(sudoku, value, indices, order))
            return true;
    } while (boost::next_combination(indices.begin(),
                indices.begin() + order, indices.end()));


    return false;
}

bool RowBaseBasicFishWithIndices(Sudoku &sudoku, Index_t val, std::vector<Index_t> &rowIndices, Index_t order)
{
    bool ret = false;
    boost::array<Index_t, 4> colIndices = {{ 0 }};
    Index_t numCols = 0;

    for (Index_t i = 0; i < order; ++i) {
        for (Index_t j = 0; j < 9; ++j) {
            if (sudoku.GetCell(rowIndices[i], j).IsCandidate(val) &&
                    std::find(colIndices.data(), colIndices.data() + numCols, j) == colIndices.data() + numCols) {
                if (numCols >= order)
                    return false;

                colIndices[numCols++] = j;
            }
        }
    }

    if (numCols == order) {
        std::vector<RowColVal> changed;
        for (Index_t i = 0; i < 9; ++i) {
            if (std::find(rowIndices.data(), rowIndices.data() + order, i) != rowIndices.data() + order)
                continue;

            for (Index_t j = 0; j < order; ++j) {
                Cell cell = sudoku.GetCell(i, colIndices[j]);
                if (cell.ExcludeCandidate(val))
                {
                    changed.push_back(RowColVal(i, colIndices[j], val));
                    sudoku.SetCell(cell, i, colIndices[j]);
                    ret = true;
                }
            }
        }

        if (ret) {
            std::vector<Index_t> tmp(colIndices.begin(), colIndices.end());
            LogBasicFish(true, rowIndices, tmp, changed, val, order);
        }
    }
    return ret;
}

bool ColBaseBasicFish(Sudoku &sudoku, Index_t value, Index_t order)
{
    if (MaxSizeOfBasicFish(sudoku, value) < order)
        return false;

    std::vector<Index_t> indices =
        IndicesOfPossibleColBaseBasicFish(sudoku, value, order);
    if (indices.size() < order)
        return false;

    do {
        if (ColBaseBasicFishWithIndices(sudoku, value, indices, order))
            return true;
    } while (boost::next_combination(indices.begin(),
                indices.begin() + order, indices.end()));

    return false;
}

bool ColBaseBasicFishWithIndices(Sudoku &sudoku, Index_t val, std::vector<Index_t> &colIndices, Index_t order)
{
    bool ret = false;
    boost::array<Index_t, 4> rowIndices = {{ 0 }};
    Index_t numRows = 0;

    for (Index_t i = 0; i < order; ++i) {
        for (Index_t j = 0; j < 9; ++j) {
            if (sudoku.GetCell(j, colIndices[i]).IsCandidate(val) &&
                    std::find(rowIndices.data(), rowIndices.data() + numRows, j) == rowIndices.data() + numRows) {
                if (numRows >= order)
                    return false;

                rowIndices[numRows++] = j;
            }
        }
    }

    if (numRows == order) {
        std::vector<RowColVal> changed;
        for (Index_t i = 0; i < 9; ++i) {
            if (std::find(colIndices.data(), colIndices.data() + order, i) != colIndices.data() + order)
                continue;

            for (Index_t j = 0; j < order; ++j) {
                Cell cell = sudoku.GetCell(rowIndices[j], i);
                if (cell.ExcludeCandidate(val))
                {
                    changed.push_back(RowColVal(rowIndices[j], i, val));
                    sudoku.SetCell(cell, rowIndices[j], i);
                    ret = true;
                }
            }
        }

        if (ret) {
            std::vector<Index_t> tmp(rowIndices.begin(), rowIndices.end());
            LogBasicFish(false, tmp, colIndices, changed, val, order);
        }
    }
    return ret;
}

void LogBasicFish(bool rowBase, const std::vector<Index_t> &rows,
        const std::vector<Index_t> &cols,
        const std::vector<RowColVal> &changed,
        Index_t value, Index_t order)
{
    std::ostringstream fishStr, changedStr;

    fishStr << (rowBase?'r':'c');
    for (Index_t i = 0; i < order; ++i)
        fishStr << (rowBase?rows[i]+1:cols[i]+1);
    fishStr << '/' << (rowBase?'c':'r');
    for (Index_t i = 0; i < order; ++i)
        fishStr << (rowBase?cols[i]+1:rows[i]+1);
    fishStr << '=' << value;

    for (std::vector<RowColVal>::const_iterator i = changed.begin();
            i != changed.end(); ++i) {
        if (i != changed.begin())
            changedStr << ", ";
        changedStr << 'r' << i->get<0>()+1 << 'c' << i->get<1>()+1 << '#' << i->get<2>();
    }

    Log(Info, "%s %s ==> %s\n", OrderToString(order),
            fishStr.str().c_str(), changedStr.str().c_str());
}

const char *OrderToString(Index_t order)
{
    switch (order) {
        case 1: return "1-fish";
        case 2: return "x-wing";
        case 3: return "swordfish";
        case 4: return "jellyfish";
        case 5: return "squirmbag";
        case 6: return "whale";
        case 7: return "leviathan";
        default: return "unknown";
    }
}

}
