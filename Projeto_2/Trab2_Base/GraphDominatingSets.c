//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

// Student Name :
// Student Number :
// Student Name :
// Student Number :

/*** COMPLETE THE GraphIsDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinWeightDominatingSet FUNCTION ***/

#include "GraphDominatingSets.h"

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"
#include "instrumentation.h"

static unsigned int _CollectVertexIds(const Graph* g, unsigned int** idsOut) {
  IndicesSet* vertices = GraphGetSetVertices(g);
  unsigned int numVertices = IndicesSetGetNumElems(vertices);
  unsigned int* ids = NULL;

  if (numVertices > 0) {
    ids = malloc(numVertices * sizeof(unsigned int));
    if (ids == NULL) abort();

    int idx = 0;
    int vertex = IndicesSetGetFirstElem(vertices);
    while (vertex != -1) {
      ids[idx++] = (unsigned int)vertex;
      vertex = IndicesSetGetNextElem(vertices);
    }
  }

  IndicesSetDestroy(&vertices);
  *idsOut = ids;
  return numVertices;
}

static void _SearchMinDominating(const Graph* g, unsigned int* vertexIds,
                                 unsigned int numVertices, unsigned int position,
                                 IndicesSet* currentSet, unsigned int* bestSize,
                                 IndicesSet** bestSet) {
  if (position == numVertices) {
    if (IndicesSetIsEmpty(currentSet)) {
      return;
    }

    unsigned int currentSize = IndicesSetGetNumElems(currentSet);
    if (currentSize >= *bestSize) {
      return;
    }

    if (GraphIsDominatingSet(g, currentSet)) {
      if (*bestSet != NULL) {
        IndicesSetDestroy(bestSet);
      }
      *bestSet = IndicesSetCreateCopy(currentSet);
      *bestSize = currentSize;
    }
    return;
  }

  unsigned int vertex = vertexIds[position];

  IndicesSetAdd(currentSet, (uint16_t)vertex);
  if (IndicesSetGetNumElems(currentSet) < *bestSize) {
    _SearchMinDominating(g, vertexIds, numVertices, position + 1, currentSet,
                         bestSize, bestSet);
  }
  IndicesSetRemove(currentSet, (uint16_t)vertex);

  _SearchMinDominating(g, vertexIds, numVertices, position + 1, currentSet,
                       bestSize, bestSet);
}

static void _SearchMinWeightDominating(const Graph* g, unsigned int* vertexIds,
                                       unsigned int numVertices, unsigned int position,
                                       IndicesSet* currentSet, double* weights,
                                       double currentWeight, double* bestWeight,
                                       IndicesSet** bestSet) {
  if (position == numVertices) {
    if (IndicesSetIsEmpty(currentSet)) {
      return;
    }

    if (GraphIsDominatingSet(g, currentSet)) {
      if (*bestSet == NULL || currentWeight < *bestWeight) {
        if (*bestSet != NULL) {
          IndicesSetDestroy(bestSet);
        }
        *bestSet = IndicesSetCreateCopy(currentSet);
        *bestWeight = currentWeight;
      }
    }
    return;
  }

  unsigned int vertex = vertexIds[position];
  double vertexWeight = weights[vertex];

  IndicesSetAdd(currentSet, (uint16_t)vertex);
  double nextWeight = currentWeight + vertexWeight;
  if (nextWeight < *bestWeight) {
    _SearchMinWeightDominating(g, vertexIds, numVertices, position + 1,
                               currentSet, weights, nextWeight, bestWeight,
                               bestSet);
  }
  IndicesSetRemove(currentSet, (uint16_t)vertex);

  _SearchMinWeightDominating(g, vertexIds, numVertices, position + 1,
                             currentSet, weights, currentWeight, bestWeight,
                             bestSet);
}

//
// TO BE COMPLETED
//
// Check if the given set is a dominating set for the graph
// Return 1 if true, or 0 otherwise
//
// A dominating set is a set of graph vertices such that every other
// graph vertex not in the set is adjacent to a graph vertex in the set
//
int GraphIsDominatingSet(const Graph* g, IndicesSet* vertSet) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);
  assert(IndicesSetIsEmpty(vertSet) == 0);

  IndicesSet* graphVertices = GraphGetSetVertices(g);
  if (!IndicesSetIsSubset(vertSet, graphVertices)) {
    IndicesSetDestroy(&graphVertices);
    return 0;
  }

  int vertex = IndicesSetGetFirstElem(graphVertices);
  while (vertex != -1) {
    if (!IndicesSetContains(vertSet, (uint16_t)vertex)) {
      IndicesSet* adjacents = GraphGetSetAdjacentsTo(g, (unsigned int)vertex);
      int dominated = 0;
      int adj = IndicesSetGetFirstElem(adjacents);
      while (adj != -1) {
        if (IndicesSetContains(vertSet, (uint16_t)adj)) {
          dominated = 1;
          break;
        }
        adj = IndicesSetGetNextElem(adjacents);
      }
      IndicesSetDestroy(&adjacents);
      if (!dominated) {
        IndicesSetDestroy(&graphVertices);
        return 0;
      }
    }

    vertex = IndicesSetGetNextElem(graphVertices);
  }

  IndicesSetDestroy(&graphVertices);
  return 1;
}

//
// TO BE COMPLETED
//
// Compute a MIN VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the/a dominating set
//
IndicesSet* GraphComputeMinDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  if (GraphGetNumVertices(g) == 0) {
    return IndicesSetCreateEmpty(GraphGetVertexRange(g));
  }

  unsigned int* vertexIds = NULL;
  unsigned int numVertices = _CollectVertexIds(g, &vertexIds);

  IndicesSet* current = IndicesSetCreateEmpty(GraphGetVertexRange(g));
  unsigned int bestSize = GraphGetNumVertices(g) + 1;
  IndicesSet* bestSet = NULL;

  _SearchMinDominating(g, vertexIds, numVertices, 0, current, &bestSize,
                       &bestSet);

  if (bestSet == NULL) {
    bestSet = GraphGetSetVertices(g);
  }

  IndicesSetDestroy(&current);
  free(vertexIds);

  return bestSet;
}

//
// TO BE COMPLETED
//
// Compute a MIN WEIGHT VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the dominating set
//
IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  if (GraphGetNumVertices(g) == 0) {
    return IndicesSetCreateEmpty(GraphGetVertexRange(g));
  }

  unsigned int* vertexIds = NULL;
  unsigned int numVertices = _CollectVertexIds(g, &vertexIds);

  double* weights = GraphComputeVertexWeights(g);

  IndicesSet* current = IndicesSetCreateEmpty(GraphGetVertexRange(g));
  double bestWeight = DBL_MAX;
  IndicesSet* bestSet = NULL;

  _SearchMinWeightDominating(g, vertexIds, numVertices, 0, current, weights,
                             0.0, &bestWeight, &bestSet);

  if (bestSet == NULL) {
    bestSet = GraphGetSetVertices(g);
    bestWeight = 0.0;
  }

  IndicesSetDestroy(&current);
  free(vertexIds);
  free(weights);

  return bestSet;
}
