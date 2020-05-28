#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Dijkstra.h"
#include "PQ.h"

#define INFINITY 100000

typedef PredNode *PredNodeP;

// Creates and returns the ShortestPaths struct that contains
// information about paths from 'src' to all other nodes in 'g'
ShortestPaths dijkstra(Graph g, Vertex src);
// Diagnostic
void showShortestPaths(ShortestPaths sps);
// Frees all malloced data from 'sps'
void freeShortestPaths(ShortestPaths sps);
// Recursively frees a PredNode list
static void freePredList(PredNodeP p);
// Creates a newPredNode
static PredNodeP newPredNode(Vertex v);



// Primary function for populating ShortestPaths. Uses a priority queue
// to reach every node from every other node. 
// Implements Dijkstra's shortest path algorithm
ShortestPaths dijkstra(Graph g, Vertex src) {
	int numNodes = GraphNumVertices(g);

	ShortestPaths new = (ShortestPaths) {
		.numNodes = numNodes,
		.src = src,
		.dist = malloc(numNodes * sizeof(int)),	
		.pred = malloc(numNodes * sizeof(PredNodeP))
	};
	
	int visited[new.numNodes];

	PQ pq = PQNew();
	// Defaulting values
	for (int i = 0; i < new.numNodes; i++) {
		new.dist[i] = INFINITY;
		new.pred[i] = NULL;
		visited[i] = -1;
	}

	// Add src to the queue
	ItemPQ item;
	item.key = src;
	item.value = new.dist[src];
	PQAdd(pq, item);

	new.dist[src] = 0;

	// Same as 'for each unvisited node do'
	while (!PQIsEmpty(pq)) {
		// Get node with lowest 'dist' in queue
		ItemPQ currV = PQDequeue(pq);
		// If currV is unvisited
		if (visited[currV.key] != -1) continue;

		// Get neighbours for currV
		AdjList neighbours = GraphOutIncident(g, currV.key);
		while (neighbours != NULL) {
			// For each neighbour:
			// Get length from source node including path from currV to neighbour
			int len = new.dist[currV.key] + neighbours->weight;
			// If this path through currV is faster than one before (or no path at all),
			// update its distance and record currV as its previous.
			// Unless its the same distance, then don't update and add a new pred
			if (len < new.dist[neighbours->v]) {
				// Update shortest distance
				new.dist[neighbours->v] = len;
				// Free all preds
				freePredList(new.pred[neighbours->v]);
				// New pred
				new.pred[neighbours->v] = newPredNode(currV.key);
			} else if (len == new.dist[neighbours->v]) {
				// Another path so add another predNode onto the end
				// Add onto front
				PredNodeP newP = newPredNode(currV.key);
				newP->next = new.pred[neighbours->v];
				new.pred[neighbours->v] = newP;
			}
			// Enqueue neighbours as they're visited
			item.key = neighbours->v;
			item.value = new.dist[neighbours->v];
			PQAdd(pq, item);

			neighbours = neighbours->next;
		}
		visited[currV.key] = 1;
	}
	// Unvisited nodes will have dist == INFINITY, so set them to 0 per 
	// the spec
	for (int i = 0; i < new.numNodes; i++) {
		if (new.dist[i] == INFINITY) new.dist[i] = 0;
	}

	PQFree(pq);

	return new;
}

// Malloc's, returns and defaults a new predNode
static PredNodeP newPredNode(Vertex v) {
	PredNodeP p = malloc(sizeof(PredNode));
	p->next = NULL;
	p->v = v;

	return p;
}

// Diagnostic
void showShortestPaths(ShortestPaths sps) {

	printf("/*\nNum nodes: %d\n\n", sps.numNodes);
	printf("Source vertex: %d\n\n", sps.src);
	
	for (int i = 0; i < sps.numNodes; i++) {
		printf("Distance to vertex %d is %d\n", i, sps.dist[i]);
		printf("        Preds are: ");
		for (PredNodeP currPred = sps.pred[i]; currPred != NULL; currPred = currPred->next) {
			printf("[%d] -> ", currPred->v);
		}
		printf("X\n");
	}
	printf("*/\n");
}

// First free's the PredNode lists, then the two arrays.
void freeShortestPaths(ShortestPaths sps) {
	for (int i = 0; i < sps.numNodes; i++) {
		// ME PROBLEM. Trying to free the same thing twice?
		freePredList(sps.pred[i]);
	}
	free(sps.dist);
	free(sps.pred);
}

// Recursive tool for freeing a list of PredNodes
static void freePredList(PredNodeP p) {
	if (p == NULL) return;
	freePredList(p->next);
	free(p);
}

