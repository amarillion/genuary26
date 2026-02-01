#pragma once

#include <vector>
#include <set>
#include <functional>
#include <tuple>

/**
 * Step-wise maze generator using recursive backtracking
 */
template<typename N, typename E>
class RecursiveBacktracker {
	typedef std::function<std::vector<std::tuple<E, N>>(N)> AdjacencyFunc;
	typedef std::function<void(N, E, N)> LinkFunc;

	AdjacencyFunc getAdjacent;
	LinkFunc link;
	std::vector<N> open;
	std::set<N> visited;

public:
	RecursiveBacktracker(N start, AdjacencyFunc getAdjacent, LinkFunc link): getAdjacent(getAdjacent), link(link) {
		open.push_back(start);
	}

	void step() {
		while(true) {
			if (open.empty()) return;

			N current = open.back();
			
			std::vector<std::tuple<E, N>> unvisitedAdjacents;
			for (const auto &i : getAdjacent(current)) {
				if (visited.contains(get<1>(i))) continue;
				unvisitedAdjacents.push_back(i);
			}

			if (unvisitedAdjacents.empty()) {
				open.pop_back();
			}
			else {
				int idx = rand() % unvisitedAdjacents.size();
				auto &chosen = unvisitedAdjacents[idx]; 
				N &dest = std::get<1>(chosen);
				E &dir = std::get<0>(chosen); 
				open.push_back(dest);
				visited.insert(dest);
				link(current, dir, dest);
				return;
			}
		}
	}

	bool isDone() {
		return open.empty();
	}
};
