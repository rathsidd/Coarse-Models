/* Copyright (C) 2020 Joshua J. Daymude, Robert Gmyr, and Kristian Hinnenthal.
 * The full GNU GPLv3 can be found in the LICENSE file, and the full copyright
 * notice can be found at the top of main/main.cpp. */

// Defines the particle system and composing particles for the Compression
// Algorithm as defined in 'A Markov Chain Algorithm for Compression in
// Self-Organizing Particle Systems' [arxiv.org/abs/1603.07991]. In particular,
// this simulates the local, distributed, asynchronous algorithm A using the
// #neighbors metric instead of the #triangles metric.

#ifndef AMOEBOTSIM_ALG_COMPRESSION_H_
#define AMOEBOTSIM_ALG_COMPRESSION_H_

#include <algorithm> // For distance() and find().
#include <set>
#include <vector>
#include <map>
#include <string>
#include <iostream> 
#include <list> 
#include <iterator>
#include <unordered_set>

#include <QString>

#include "core/amoebotparticle.h"
#include "core/amoebotsystem.h"

class CompressionParticle : public AmoebotParticle {
  friend class CompressionSystem;
  friend class PerimeterMeasure;
  friend class SurfaceArea;
  friend class PercentOrdering;
  friend class SurfaceAreaNumeratorParticles;
  friend class AvgHeight;
  friend class AvgWidth;

  enum class State {
      Red,
      Blue,
      Green,
      Black,
  };

  enum class Direction {
      a,
      b,
      c,
      d,
      e,
      f,
  };


 public:
  // Constructs a new particle with a node position for its head, a global
  // compass direction from its head to its tail (-1 if contracted), an offset
  // for its local compass, a system which it belongs to, and a bias parameter.
  CompressionParticle(const Node head, const int globalTailDir,
                      const int orientation, AmoebotSystem& system,
                      const double lambda,
                      State state);


  // Executes one particle activation.
  virtual void activate() override;

  // Returns the string to be displayed when this particle is inspected; used
  // to snapshot the current values of this particle's memory at runtime.
 // virtual QString inspectionText() const;
    int headMarkColor() const override; //MichaelM added head and tail colors, also removed "const override;" to "const;"
    int tailMarkColor() const override; //Not sure if this causes problems.
      double headMarkDir() const override;
      int tailMarkDir() const override;
protected:
  // Particle memory.
  const double lambda;
  double q;
  int numNbrsBefore;
  int numRedNbrsBefore;
  int numBlueNbrsBefore;
  int numNbrsSameDirBefore;
  int numRedNbrsSameDirBefore;
  bool flag;
  State _state;   //MichaelM added states (like in DiscoDemo)
  int _direction; //MichaelM added directions
//  Direction getRandDir() const;

  //Member variables:
     //MichaelM added states

private:
  // Gets a reference to the neighboring particle incident to the specified port
  // label. Crashes if no such particle exists at this label; consider using
  // hasNbrAtLabel() first if unsure.
  CompressionParticle& nbrAtLabel(int label) const;

  // hasExpNbr() checks whether this particle has an expanded neighbor, while
  // hasExpHeadAtLabel() checks whether the head of an expanded neighbor is at
  // the position at the specified label.
  bool hasExpNbr() const;
  bool hasExpHeadAtLabel(const int label) const;
  bool hasRBNbrInLine() const; //has red or black neighbor that it is aligned with
  int almostInLine() const;
  bool atEndOfLine() const;
  bool stuckInLine() const;
  bool stuckInRedLine() const;

  // Counts the number of neighbors in the labeled positions. Note: this
  // implicitly assumes all neighbors are unique, as none are expanded.
  int nbrCount(std::vector<int> labels) const;
  int nbrCountSameDir(std::vector<int> labels) const;
  int redNbrCount(std::vector<int> labels) const;
  int redNbrCountSameDir(std::vector<int> labels) const;
  int blueNbrCount(std::vector<int> labels) const;

  // Functions for checking Properties 1 and 2 of the compression algorithm.
  bool checkProp1(std::vector<int> S) const;
  bool checkProp2(std::vector<int> S) const;
  bool checkRedProp1(std::vector<int> S) const;
  bool checkRedProp2(std::vector<int> S) const;
  bool checkBlueProp1(std::vector<int> S) const;
  bool checkBlueProp2(std::vector<int> S) const;
};

class CompressionSystem : public AmoebotSystem {
  friend class PerimeterMeasure;
  friend class SurfaceArea;
  friend class PercentOrdering;
  friend class SurfaceAreaNumeratorParticles;
  friend class AvgHeight;
  friend class AvgWidth;

 public:
  // Constructs a system of CompressionParticles connected to a randomly
  // generated surface (with no tunnels). Takes an optionally specified size
  // (#particles) and a bias parameter. A bias above 2 + sqrt(2) will provably
  // yield compression; a bias below 2.17 will provably yield expansion.
  CompressionSystem(unsigned int numRedParticles = 15, unsigned int numBlueParticles = 15, unsigned int numGreenParticles = 15, double lambda = 4.0);
  int findGroup(CompressionParticle* particle);
  void allGroups();
  //std::vector<CompressionParticle> DFS(CompressionParticle &p);
  void DFSHeight(CompressionParticle &p, std::vector<CompressionParticle> &cluster);
  void DFSWidth(CompressionParticle &p, std::vector<CompressionParticle> &cluster, int dir);
  void DFSCluster(CompressionParticle &p, std::vector<CompressionParticle> &cluster, 
    std::vector<std::vector<CompressionParticle>> &heights, std::vector<std::vector<CompressionParticle>> &widths);
  std::vector<std::vector<CompressionParticle>> getClusters();
  //void getClusters();
  // Because this algorithm never terminates, this simply returns false.
  virtual bool hasTerminated() const;
  int sideLen;
  double averageHeight;
  double averageWidth;
  protected:
    //int nodesOccupied; // Priti
    // int totalNodes; // Priti
};

class PerimeterMeasure : public Measure {
 public:
  // Constructs a PerimeterMeasure by using the parent constructor and adding a
  // reference to the CompressionSystem being measured.
  PerimeterMeasure(const QString name, const unsigned int freq,
                   CompressionSystem& system);

  // Calculates the perimeter of the system, i.e., the number of edges on the
  // walk around the unique external boundary of the system. Uses the fact
  // that perimeter = (3 * #particles) - (#nearest neighbor pairs) - 3.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

class SurfaceArea : public Measure {
 public:
  // Constructs a SurfaceArea
  SurfaceArea(const QString name, const unsigned int freq,
                    CompressionSystem& system);

  // Calculated the percentage of surface area covered.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

class SurfaceAreaNumeratorParticles : public Measure {
 public:
  // Constructs a SurfaceArea
  SurfaceAreaNumeratorParticles(const QString name, const unsigned int freq,
                    CompressionSystem& system);

  // Calculated the percentage of surface area covered.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

class PercentOrdering : public Measure {
 public:
  // Constructs a SurfaceArea
  PercentOrdering(const QString name, const unsigned int freq,
                    CompressionSystem& system);

  // Calculated the percentage of surface area covered.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

class AvgHeight : public Measure {
 public:
  // Constructs a SurfaceArea
  AvgHeight(const QString name, const unsigned int freq,
                    CompressionSystem& system);

  // Calculated the percentage of surface area covered.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

class AvgWidth : public Measure {
 public:
  // Constructs a SurfaceArea
  AvgWidth(const QString name, const unsigned int freq,
                    CompressionSystem& system);

  // Calculated the percentage of surface area covered.
  double calculate() const final;

 protected:
  CompressionSystem& _system;
};

#endif  // AMOEBOTSIM_ALG_COMPRESSION_H_
