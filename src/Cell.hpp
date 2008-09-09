#ifndef CELL_HPP
#define CELL_HPP

#include "Index.hpp"
#include <cassert>

class Cell
{
    public:
        Cell();
        Cell(Index_t);
        Cell(const Cell &);
        Cell &operator=(const Cell &);

        bool HasValue() const;
        void SetValue(Index_t);
        Index_t GetValue() const;

        bool IsCandidate(Index_t) const;
        Index_t NumCandidates() const;
        bool ExcludeCandidate(Index_t);

    private:
        unsigned short _candidates; // bit mask of candidates
        Index_t _value;
};

/**
 * Constructs a cell with all candidates set.
 */
inline Cell::Cell()
{
    _candidates = 0x1ff;
    _value = 0;
}

/**
 * Constructs the cell with the given value.
 */
inline Cell::Cell(Index_t val)
{
    SetValue(val);
}

inline Cell::Cell(const Cell &x)
{
    *this = x;
}

inline Cell &Cell::operator=(const Cell &x)
{
    _value = x._value;
    _candidates = x._candidates;
    return *this;
}

inline bool Cell::HasValue() const
{
    return _value != 0;
}

inline void Cell::SetValue(Index_t val)
{
    assert(val >= 1 && val <= 9);

    _value = val;
    _candidates = 0;
}

inline Index_t Cell::GetValue() const
{
    assert(_value != 0);
    return _value;
}

inline Index_t Cell::NumCandidates() const
{
    Index_t tmp = 0;
    for (Index_t i = 0; i < 9; ++i) {
        if (_candidates & (1 << i))
            ++tmp;
    }
    return tmp;
}

inline bool Cell::IsCandidate(Index_t val) const
{
    assert(val >= 1 && val <= 9);
    return _candidates & (1 << (val - 1));
}

/**
 * Removes a candidate from the cell if needed. Returns true if a candidate was
 * excluded.
 */
inline bool Cell::ExcludeCandidate(Index_t val)
{
    assert(val >= 1 && val <= 9);

    if (!IsCandidate(val))
        return false;

    _candidates &= ~(1 << (val - 1));
    return true;
}


#endif