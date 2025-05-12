#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Система непересекающихся множеств, своя реализация
// (https://github.com/ekhalturin-ki15/Kata)
template<typename T>
struct DSU
{
public:
    DSU() = delete;

    DSU(int _n) : n { _n }
    {
        count = n;
        rank.assign(n, 0);
        s.assign(n, 1);
        pr.resize(n);
        int i = 0;
        for (auto& it : pr)
            it = i++;
    }

    int Find(int i)
    {
        if (i == pr[i])
            return i;
        return (pr[i] = Find(pr[i]));
    }

    bool isSame(int l, int r)
    {
        return (Find(l) == Find(r));
    }

    void Merge(int l_, int r_)
    {
        int l = Find(l_);
        int r = Find(r_);
        if (l == r)
            return;

        --count;
        if (rank[l] == rank[r])
            ++rank[l];

        if (rank[l] > rank[r])
        {
            s[l] += s[r];
            pr[r] = l;
            return;
        }

        s[r] += s[l];
        pr[l] = r;
    }

    int GetCount() const
    {
        return count;
    }

private:
    vector<T> rank, pr, s;
    int       n;
    int       count;
};