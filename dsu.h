#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//Система непересекающихся множеств, своя реализация (https://github.com/ekhalturin-ki15/Kata)
template <typename T>
struct True_DSU
{
	vector<T> v, rank, pr, s;
	int n;
	int count;

	True_DSU() = delete;

	True_DSU(const vector<T>& v_) : v{ v_ }
	{
		n = v.size();
		count = n;
		rank.assign(n, 0);
		s.assign(n, 1);
		pr.resize(n); int i = 0; for (auto& it : pr) it = i++;
	}

	int Find(int i)
	{
		if (i == pr[i]) return i;
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
		if (l == r) return;

		--count;
		if (rank[l] == rank[r]) ++rank[l];

		if (rank[l] > rank[r])
		{
			s[l] += s[r];
			pr[r] = l;
			return;
		}

		s[r] += s[l];
		pr[l] = r;
	}
};