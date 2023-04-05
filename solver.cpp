#include "solver.hpp"
#include "graphe.hpp"
#include <vector>
#include <iostream>
#include "ilcplex/cplex.h"
#include "ilcplex/ilocplex.h"

void solve(Graphe& G) {
  try {

  int nbNode = 4;
  int nbArete = 2;
  int nbSlot = 4;
  int higherBound = 3;

  IloEnv env;
  IloModel model(env);

  IloIntArray xCoord(env, 4, 0, 0, 5, 5); // Coord X des emplacements
  IloIntArray yCoord(env, 4, 0, 4, 0, 4); // Coord Y des emplacements

  IloIntArray node1Array(env, 2, 0, 2); // Noeud 1 des aretes
  IloIntArray node2Array(env, 2, 1, 3); // Noeud 2 des aretes

  IloIntVarArray nodes(env, nbNode, 0, higherBound); // Association des noeuds aux emplacements

  for (int i=0;i<nbNode-1;i++) {
    for (int j=i+1;j<nbNode;j++) {
      model.add(nodes[i] != nodes[j]);
    }
  }

  /*
  px = xCoord[nodes[node1Array[i]]];
  py = yCoord[nodes[node1Array[i]]];
  qx = xCoord[nodes[node2Array[i]]];
  qy = yCoord[nodes[node2Array[i]]];

  rx = xCoord[nodes[node1Array[j]]];
  ry = yCoord[nodes[node1Array[j]]];
  sx = xCoord[nodes[node2Array[j]]];
  sy = yCoord[nodes[node2Array[j]]];

  ag1 = (((xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[i]]]) * (yCoord[nodes[node1Array[j]]] - yCoord[nodes[node1Array[i]]]) - (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[i]]]) * (xCoord[nodes[node1Array[j]]] - xCoord[nodes[node1Array[i]]]))>0);
  ag2 = (((xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[i]]]) * (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[i]]]) - (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[i]]]) * (xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[i]]]))>0);
  ag3 = (((xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[j]]]) * (yCoord[nodes[node1Array[i]]] - yCoord[nodes[node1Array[j]]]) - (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[j]]]) * (xCoord[nodes[node1Array[i]]] - xCoord[nodes[node1Array[j]]]))>0);
  ag4 = (((xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[j]]]) * (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[j]]]) - (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[j]]]) * (xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[j]]]))>0);
  */
  
  IloExpr objExpr(env);
  int index = 0;
  for (int i=0;i<nbArete-1;i++) {
    for (int j=i+1;j<nbArete;j++) {
      //objExpr += ((((xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[i]]]) * (yCoord[nodes[node1Array[j]]] - yCoord[nodes[node1Array[i]]]) - (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[i]]]) * (xCoord[nodes[node1Array[j]]] - xCoord[nodes[node1Array[i]]]))>0) * (((xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[i]]]) * (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[i]]]) - (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[i]]]) * (xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[i]]]))>0) == 1);
      //objExpr += ((((xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[j]]]) * (xCoordyCoord[nodes[node1Array[i]]] - yCoord[nodes[node1Array[j]]]) - (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[j]]]) * (xCoord[nodes[node1Array[i]]] - xCoord[nodes[node1Array[j]]]))>0) * (((xCoord[nodes[node2Array[j]]] - xCoord[nodes[node1Array[j]]]) * (yCoord[nodes[node2Array[i]]] - yCoord[nodes[node1Array[j]]]) - (yCoord[nodes[node2Array[j]]] - yCoord[nodes[node1Array[j]]]) * (xCoord[nodes[node2Array[i]]] - xCoord[nodes[node1Array[j]]]))>0) == 1);
    }
  }

  model.add(IloMinimize(env, objExpr));
  //model.add(IloMaximize(env, objExpr));

  // solve the model
  IloCplex cplex(model);
  cplex.solve();

  // print the solution
  std::cout << "Solution status: " << cplex.getStatus() << std::endl;
  std::cout << "Solution value: " << cplex.getObjValue() << std::endl;
  for (int i = 0; i < nbNode; i++) {
    std::cout << "node[" << i << "] = " << cplex.getValue(nodes[i]) << std::endl;
  }

  env.end();
  }
  catch(IloException& e) {
      cerr << "CPLEX found the following exception: " << e << endl;
      e.end();
  }
  catch(...) {
      cerr << "The following unknown exception was found: " << endl;
  }
}

void solve2(Graphe& G) {
  int nbNode = G._noeuds.size();
  int nbArete = G._aretes.size();
  int nbSlot = G._emplacements.size();
  int higherBound = G._emplacements.size()-1;
  std::vector<std::pair<int,int>> edgeVector;
  for (int i=0;i<nbArete;i++) {
    edgeVector.push_back(make_pair(G._aretes[i].getNoeud1()->_id,G._aretes[i].getNoeud2()->_id));
  }
  std::vector<std::pair<int,int>> slotsVector;
  for (int i=0;i<nbSlot;i++) {
    slotsVector.push_back(make_pair(G._emplacements[i]._x,G._emplacements[i]._y));
  }

  IloEnv env;
  IloModel model(env);
  IloIntVarArray x(env, nbNode, 0, higherBound); // create an array of 10 integer variables with lower and upper bounds of 0 and 9 respectively

  for (int i=0;i<nbNode-1;i++) {
    for (int j=i+1;j<nbNode;j++) {
      model.add(x[i] != x[j]);
    }
  }

  int nbTestCrossings = (nbArete*nbArete - nbArete)/2; 
  IloBoolVarArray c(env,nbTestCrossings);
  int index = 0;
  for (int i=0;i<nbArete-1;i++) {
    for (int j=i+1;j<nbArete;j++) {
      //model.add(c[index] = 5+2>3);
      index++;
    }
  }

  // create the objective function to minimize the sum of all variables
  IloExpr objExpr(env);
  for (int i = 0; i < nbNode; i++) {
    objExpr += x[i];
  }
  model.add(IloMinimize(env, objExpr));

  // solve the model
  IloCplex cplex(model);
  cplex.solve();

  // print the solution
  std::cout << "Solution status: " << cplex.getStatus() << std::endl;
  std::cout << "Solution value: " << cplex.getObjValue() << std::endl;
  for (int i = 0; i < G._noeuds.size(); i++) {
    std::cout << "x[" << i << "] = " << cplex.getValue(x[i]) << std::endl;
  }

  env.end();
}

void solve3(Graphe& G) {
  try {

  int nbNode = 4;
  int nbArete = 2;
  int nbSlot = 4;
  int higherBound = 3;

  IloEnv env;
  IloModel model(env);

  IloArray<IloIntVarArray> nodesArray(env,2);
  for (int i=0;i<2;i++) {
    nodesArray[i] = IloIntVarArray(env,nbNode,0,higherBound);
  }
  IloIntArray node1Array(env, 2, 0, 2); // Noeud 1 des aretes
  IloIntArray node2Array(env, 2, 1, 3); // Noeud 2 des aretes

  for (int i=0;i<nbNode-1;i++) {
    for (int j=i+1;j<nbNode;j++) {
      model.add((nodesArray[0][i] != nodesArray[0][j])||(nodesArray[1][i] != nodesArray[1][j]));
    }
  }
  /*
  px = xCoord[nodes[node1Array[i]]]; nodesArray[0][node1Array[i]];
  py = yCoord[nodes[node1Array[i]]]; nodesArray[1][node1Array[i]];
  qx = xCoord[nodes[node2Array[i]]]; nodesArray[0][node2Array[i]];
  qy = yCoord[nodes[node2Array[i]]]; nodesArray[1][node2Array[i]];

  rx = xCoord[nodes[node1Array[j]]]; nodesArray[0][node1Array[j]];
  ry = yCoord[nodes[node1Array[j]]]; nodesArray[1][node1Array[j]];
  sx = xCoord[nodes[node2Array[j]]]; nodesArray[0][node2Array[j]];
  sy = yCoord[nodes[node2Array[j]]]; nodesArray[1][node2Array[j]];

  ag1 = (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node1Array[j]] - nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[i]]) * (nodesArray[0][node1Array[j]] - nodesArray[0][node1Array[i]]))>0);
  ag2 = (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[i]]) * (nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[i]]))>0);
  ag3 = (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node1Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node1Array[i]] - nodesArray[0][node1Array[j]]))>0);
  ag4 = (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[j]]))>0);
  */
  
  IloExpr objExpr(env);
  int index = 0;
  for (int i=0;i<nbArete-1;i++) {
    for (int j=i+1;j<nbArete;j++) {
      //objExpr += ((((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node1Array[j]] - nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[i]]) * (nodesArray[0][node1Array[j]] - nodesArray[0][node1Array[i]]))>0) * (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[i]]) * (nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[i]]))>0) == 1);
      //objExpr += ((((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node1Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node1Array[i]] - nodesArray[0][node1Array[j]]))>0) * (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[j]]))>0) == 1);
    }
  }

  //model.add(IloMinimize(env, objExpr));
  model.add(IloMaximize(env, objExpr));

  // solve the model
  IloCplex cplex(model);
  cplex.solve();

  // print the solution
  std::cout << "Solution status: " << cplex.getStatus() << std::endl;
  std::cout << "Solution value: " << cplex.getObjValue() << std::endl;
  for (int i = 0; i < nbNode; i++) {
    std::cout << "node[" << i << "][x] = " << cplex.getValue(nodesArray[0][i]) << " node[" << i << "][y] = " << cplex.getValue(nodesArray[1][i]) << std::endl;
  }

  env.end();
  }
  catch(IloException& e) {
      cerr << "CPLEX found the following exception: " << e << endl;
      e.end();
  }
  catch(...) {
      cerr << "The following unknown exception was found: " << endl;
  }
}

void solve4(Graphe& G) {
  try {

  int nbNode = 4;
  int nbArete = 2;
  int nbSlot = 4;
  int higherBound = 3;

  IloEnv env;
  IloModel model(env);

  IloArray<IloIntVarArray> nodesArray(env,2);
  for (int i=0;i<2;i++) {
    nodesArray[i] = IloIntVarArray(env,nbNode,0,higherBound);
  }
  IloIntArray node1Array(env, 2, 0, 2); // Noeud 1 des aretes
  IloIntArray node2Array(env, 2, 1, 3); // Noeud 2 des aretes

  for (int i=0;i<nbNode-1;i++) {
    for (int j=i+1;j<nbNode;j++) {
      model.add((nodesArray[0][i] != nodesArray[0][j])||(nodesArray[1][i] != nodesArray[1][j]));
    }
  }
  /*
  px = xCoord[nodes[node1Array[i]]]; nodesArray[0][node1Array[i]];
  py = yCoord[nodes[node1Array[i]]]; nodesArray[1][node1Array[i]];
  qx = xCoord[nodes[node2Array[i]]]; nodesArray[0][node2Array[i]];
  qy = yCoord[nodes[node2Array[i]]]; nodesArray[1][node2Array[i]];

  rx = xCoord[nodes[node1Array[j]]]; nodesArray[0][node1Array[j]];
  ry = yCoord[nodes[node1Array[j]]]; nodesArray[1][node1Array[j]];
  sx = xCoord[nodes[node2Array[j]]]; nodesArray[0][node2Array[j]];
  sy = yCoord[nodes[node2Array[j]]]; nodesArray[1][node2Array[j]];

  ag1 = (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node1Array[j]] -  nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] -  nodesArray[1][node1Array[i]]) * (nodesArray[0][node1Array[j]] - nodesArray[0][node1Array[i]]))>0);
  ag2 = (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node2Array[j]] -  nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] -  nodesArray[1][node1Array[i]]) * (nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[i]]))>0);
  ag3 = (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * ( nodesArray[1][node1Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node1Array[i]] - nodesArray[0][node1Array[j]]))>0);
  ag4 = (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[j]]))>0);
  */
  
  IloExpr objExpr(env);
  int index = 0;
  for (int i=0;i<nbArete-1;i++) {
    for (int j=i+1;j<nbArete;j++) {
      objExpr += ((((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node1Array[j]] -  nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] -  nodesArray[1][node1Array[i]]) * (nodesArray[0][node1Array[j]] - nodesArray[0][node1Array[i]]))>0) * (((nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[i]]) * (nodesArray[1][node2Array[j]] -  nodesArray[1][node1Array[i]]) - (nodesArray[1][node2Array[i]] -  nodesArray[1][node1Array[i]]) * (nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[i]]))>0) == 1);
      objExpr += ((((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * ( nodesArray[1][node1Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node1Array[i]] - nodesArray[0][node1Array[j]]))>0) * (((nodesArray[0][node2Array[j]] - nodesArray[0][node1Array[j]]) * (nodesArray[1][node2Array[i]] - nodesArray[1][node1Array[j]]) - (nodesArray[1][node2Array[j]] - nodesArray[1][node1Array[j]]) * (nodesArray[0][node2Array[i]] - nodesArray[0][node1Array[j]]))>0) == 1);
    }
  }

  model.add(IloMinimize(env, objExpr));
  //model.add(IloMaximize(env, objExpr));

  // solve the model
  IloCplex cplex(model);
  cplex.solve();

  // print the solution
  std::cout << "Solution status: " << cplex.getStatus() << std::endl;
  std::cout << "Solution value: " << cplex.getObjValue() << std::endl;
  for (int i = 0; i < nbNode; i++) {
    std::cout << "node[" << i << "][x] = " << cplex.getValue(nodesArray[0][i]) << " node[" << i << "][y] = " << cplex.getValue(nodesArray[1][i]) << std::endl;
  }

  env.end();
  }
  catch(IloException& e) {
      cerr << "CPLEX found the following exception: " << e << endl;
      e.end();
  }
  catch(...) {
      cerr << "The following unknown exception was found: " << endl;
  }
}
