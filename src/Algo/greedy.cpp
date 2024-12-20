
#include "Algo/greedy.hpp"





std::vector<int> greedy(Graph* G){
	std::vector<int> dom = {};
	std::vector<bool> isDom = {};
	int n = G->getNumVertices();
	for(int i = 0; i < n; i++){
		isDom.push_back(false);
	}
	for(int i = 0; i < n; i++){
		if(!isDom[i]){
			dom.push_back(i);
			isDom[i] = true;
			for(int u : G->getNeighbours(i)){
				isDom[u] = true;
			}
		}
	}
	return dom;
}


std::vector<int> smartGreedy(Graph* G){
	int n = G->getNumVertices();
	std::vector<bool>dom(n, false);
	std::vector<bool>isDom(n, false);
	for(int i = 0; i < n; i++){
		if(G->isIsolated(i)){
			dom[i] = true;
		}
	}

	bool flag = true;
	while(flag){
		flag = false;
		int maxDeg = -1;
		int maxDegVertex = -1;
		// We look for the vertex of max degree
		for(int i = 0; i < n; i++){
			if(!isDom[i] && G->getDegree(i) > maxDeg){
				maxDeg = G->getDegree(i);
				maxDegVertex = i;
				flag = true;
			}
		}
		if(flag){
			dom[maxDegVertex] = true;
			isDom[maxDegVertex] = true;
			for(int u : G->getNeighbours(maxDegVertex)){
				isDom[u] = true;
				G->disconnectVertex(u);
			}
			G->disconnectVertex(maxDegVertex);
		}
	}
	std::vector<int> res = {};
	for(int i = 0; i < n; i++){
		if(dom[i]){
			res.push_back(i);
		}
	}
	return res;
};



//////////////////////////////
//   Smart Greedy with Heap
//////////////////////////////

std::vector<int> smarterGreedyHeap(Graph *G){
	int n = G->getNumVertices();
	std::vector<bool> isDom(n, false);
	std::vector<int> dom = {};
	std::vector<int> degreeInG(n, 0);
	BinaryHeap heap(n);
	for(int i = 0; i < n; i++){
		degreeInG[i] = G->getDegree(i);
		heap.addElement(i, degreeInG[i]);
	}
	// While the heap is not empty
	std::pair<int, int> max;
	while(heap.getNumberOfElements() > 0){
		//heap.printHeap();
		max = heap.getMax();
		// if the max is already dominated
		if(isDom[max.first]){
			heap.removeMax();
		}
		else{
			// if the current degree has changed, we update it
			if(max.second != G->getDegree(max.first)){
				heap.setMax(max.first, G->getDegree(max.first));
				heap.pushDown(0);
			}
			// otherwise we add the vertex to the dom set
			else{
				dom.push_back(max.first);
				for(int u : G->getNeighbours(max.first)){
					isDom[u] = true;
					G->disconnectVertex(u);
				}
				isDom[max.first] = true;
				G->disconnectVertex(max.first);
				heap.removeMax();
			}
		}
	}

	return dom;
}; 

std::vector<int> smarterGreedyHeapV2(Graph *G){
	int n = G->getNumVertices();
	int nonDominated = n;
	std::vector<bool> isInDom(n, false);
	std::vector<int> dom = {};
	std::vector<int> degreeInG(n, 0);
	BinaryHeap heap(n);
	for(int i = 0; i < n; i++){
		degreeInG[i] = G->getDegree(i);
		heap.addElement(i, degreeInG[i]);
	}
	// While the there exists non-dominated vertices
	std::pair<int, int> max;
	while(nonDominated > 0){
		//heap.printHeap();
		max = heap.getMax();
		// if the current degree has changed, we update it
		if(max.second != degreeInG[max.first]){
			heap.setMax(max.first, degreeInG[max.first]);
			heap.pushDown(0);
		}
		else{
			// otherwise we add the vertex to the dom set
			dom.push_back(max.first);
			heap.removeMax();
			nonDominated--;
			// and then we update the degrees
			for(int u : G->getNeighbours(max.first)){
				degreeInG[u]--;
				nonDominated--;
				for(int nei : G->getNeighbours(u)){
					degreeInG[nei]--;
				}
			} 
		}
	}

	return dom;
};





/////////////////////////////////////////
// Smart Greedy with Buckets of Buckets
/////////////////////////////////////////

std::vector<int> smarterBucketsOfBuckets(Graph *G, std::vector<int>forcedVertices){
	/*
	
	Here, we store the  current degree of non-dominated vertices in a double linked list of double linked list
	a - b - d
	 \ /
	  c

	In this graph, deg(a) = deg(c) = 2, deg(b) = 3, deg(d) = 1

	The data structure is then 	3 : b
								2 : a <-> c
								1 : d

	remove an element knowing its degree : O(1)
	Global costs of updates : O(m) (m : number of edges)
	*/
	// First, we put the the forced vertices in the dom set
	std::vector<int> dom = {};
	BucketsOfBuckets buckets(G);
	std::vector<bool> isDom = std::vector<bool>(buckets.getNumberOfElements(), false);
	for(int i = 0; i < buckets.getNumberOfElements(); i++){
		buckets.pushVertexUp(i, G->getDegree(i));
	}
	// from this points, the deta structure is fully built !
	// We can put the forced vertices in the dominant
	for(int u : forcedVertices){
		// If u has not already been dominated :
		std::vector<int> deleted = {};
		if(!isDom[u]){
			isDom[u] = true;
			dom.push_back(u);
			buckets.deleteVertex(u);
			deleted.push_back(u);
		}
		for(int v : G->getNeighbours(u)){
			if(!isDom[v]){
				isDom[v] = true;
				buckets.deleteVertex(v);
				deleted.push_back(v);
			}
		}
		for(int v : deleted){
			for(int nei : G->getNeighbours(v)){
				if(!isDom[nei]){
					buckets.decreaseVertex(nei);
				}
			}
		}
	}

	while(buckets.getHead() >= 0){
		std::vector<int> deleted = {};
		int vertex = buckets.getHeadVertex();						
		buckets.deleteVertex(vertex);
		deleted.push_back(vertex);
		dom.push_back(vertex);
		isDom[vertex] = true;
		for(int v : G->getNeighbours(vertex)){
			if(!isDom[v]){
				buckets.deleteVertex(v);
				deleted.push_back(v);
				isDom[v] = true;
			}
		}
		for(int v : deleted){
			for(int nei : G->getNeighbours(v)){
				if(!isDom[nei]){
					buckets.decreaseVertex(nei);
				}
			}
		}
	}
	return dom;
}; 

/*
std::vector<int> smarterBucketsOfBucketsV2(Graph *G, std::vector<int>forcedVertices){
	// First, we put the the forced vertices in the dom set
	int nonDominated = G->getNumVertices();
	std::vector<int> dom = {};
	BucketsOfBuckets buckets(G);
	std::vector<bool> isInDom = std::vector<bool>(buckets.getNumberOfElements(), false);
	for(int i = 0; i < buckets.getNumberOfElements(); i++){
		buckets.pushVertexUp(i, G->getDegree(i));
	}
	// from this points, the deta structure is fully built !
	// We can put the forced vertices in the dominant
	for(int u : forcedVertices){
		// If u has not already been dominated :
		dom.push_back(u);
		isInDom[u] = true;
		buckets.deleteVertex(u);
		nonDominated--;
		for(int v : G->getNeighbours(u)){
			if(!isInDom[v]){
				nonDominated--;
				buckets.decreaseVertex(v);
				for(int nei : G->getNeighbours(v)){
					if(!isInDom[nei]){
						buckets.decreaseVertex(nei);
					}
				}
			}
			
		}
	}
	while(nonDominated > 0){
		int vertex = buckets.getHeadVertex();						
		buckets.deleteVertex(vertex);
		dom.push_back(vertex);
		isInDom[vertex] = true;
		for(int v : G->getNeighbours(vertex)){
			if(!isInDom[v]){
				nonDominated--;
				buckets.decreaseVertex(v);
				for(int nei : G->getNeighbours(v)){
					if(!isInDom[nei]){
						buckets.decreaseVertex(nei);
					}
				}
			}
			
		}
	}
	return dom;
}; 
*/

