/* Copyright (C) 2020 Joshua J. Daymude, Robert Gmyr, and Kristian Hinnenthal.
 * The full GNU GPLv3 can be found in the LICENSE file, and the full copyright
 * notice can be found at the top of main/main.cpp. */

#include "alg/compression.h"

#include <algorithm>  // For distance() and find().
#include <set>
#include <vector>

#include <QtGlobal>

CompressionParticle::CompressionParticle(const Node head,
                                         const int globalTailDir,
                                         const int orientation,
                                         AmoebotSystem& system,
                                         const double lambda,
                                         State state)
  : AmoebotParticle(head, globalTailDir, orientation, system),
    lambda(lambda),
    q(0),
    numRedNbrsBefore(0),
    numBlueNbrsBefore(0),
    flag(false),
    _state(state) {}

void CompressionParticle::activate() {

    if (_state == State::Red) {

       if (isContracted()) {
           q = randDouble(0, 1);
           int expandDir = randDir();
    /*       if (q< 0.5) {
               expandDir = 3;
           } else {
               expandDir = 3;
           } */

       if (canExpand(expandDir) && !hasExpNbr()) {
      // Count neighbors in original position and expand.
      numRedNbrsBefore = redNbrCount(uniqueLabels());
      expand(expandDir);
      flag = !hasExpNbr();
    }

  } else {  // isExpanded().
     int numRedNbrsAfter = redNbrCount(headLabels());
     double x = 1; //Diffusion Rate without neighbors. All values acceptable.
     double y = 0.9; //Binding Affinity when encountering new neighbors. ALl values above 0.5 are reasonable.
     double z = 0.02; //Affinity to detach from cluster. Values less than .1 are acceptable.

     if (!flag || numRedNbrsBefore == 5) {
contractHead();
}
        else if (numRedNbrsAfter == 0 && numRedNbrsBefore == 0 && q < x) { //Diffusion rate
             contractTail();
         }
     else if (numRedNbrsAfter == 0 && numRedNbrsBefore == 0 && q > x) {
         contractHead();
     }

     else if (numRedNbrsAfter != 0 && numRedNbrsBefore == 0 && q < y) {
         contractTail();
     }
     else if (numRedNbrsAfter != 0 && numRedNbrsBefore == 0 && q > y) {
         contractHead();
     }
    else {
      // Count neighbors in new position and compute the set S.
//      int numRedNbrsAfter = redNbrCount(headLabels());
      std::vector<int> S;
      for (const int label : {headLabels()[4], tailLabels()[4]}) {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) {
          S.push_back(label);
        }
      }

      if (q < z) {
          contractTail();
      }

      // If the conditions are satisfied, contract to the new position;
      // otherwise, contract back to the original one.
      else if ((q < pow(lambda, numRedNbrsAfter - numRedNbrsBefore))
          && (checkRedProp1(S) || checkRedProp2(S))) {
        contractTail();
      }  else {
        contractHead();
      }
    }
  }
 /*   else if (_state == State::Blue) {
        if (isContracted()) {
            int expandDir = randDir();  // Select a random neighboring location.
            q = randDouble(0, 1);        // Select a random q in (0,1).

            if (canExpand(expandDir) && !hasExpNbr()) {
              // Count neighbors in original position and expand.

                numBlueNbrsBefore = blueNbrCount(uniqueLabels());
                expand(expandDir);
                flag = !hasExpNbr();
            }
        } else {  // isExpanded().
            if (!flag || numBlueNbrsBefore == 5) {
              contractHead();
            } else {
              // Count neighbors in new position and compute the set S.
              int numBlueNbrsAfter = blueNbrCount(headLabels());
              std::vector<int> S;
              for (const int label : {headLabels()[4], tailLabels()[4]}) {
                if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) {
                  S.push_back(label);
                } //MichaelM: DONT UNDERSTAND WHAT ABOVE 3 LINES DO? MAY CAUSE PROBLEMS!
              }

              // If the conditions are satisfied, contract to the new position;
              // otherwise, contract back to the original one.
              if ((q < pow(lambda, numBlueNbrsAfter - numBlueNbrsBefore))
                  && (checkBlueProp1(S) || checkBlueProp2(S))) {
                contractTail();
              } else {
                contractHead();
              }
            }
          }
    } */
}
}


int CompressionParticle::headMarkColor() const {
  if (_state == State::Red) {
      return 0xff0000;
  }
  else if (_state == State::Blue){
      return 0x0000ff;
  }
  return -1;
}

   int CompressionParticle::tailMarkColor() const {
     return headMarkColor();
   }

/* QString CompressionParticle::inspectionText() const {
  QString text;
  text += "Global Info:\n";
  text += "  head: (" + QString::number(head.x) + ", "
                      + QString::number(head.y) + ")\n";
  text += "  orientation: " + QString::number(orientation) + "\n";
  text += "  globalTailDir: " + QString::number(globalTailDir) + "\n\n";
  text += "Properties:\n";
  text += "  lambda = " + QString::number(lambda) + ",\n";
  text += "  q in (0,1) = " + QString::number(q) + ",\n";
  text += "  flag = " + QString::number(flag) + ".\n";

 if(isContracted()) {
    text += "Contracted properties:\n";
    text += "  #neighbors before = " + QString::number(numNbrsBefore) + ",\n";
  } else {  // isExpanded().
    text += "Expanded properties:\n";
    text += "  #neighbors before = " + QString::number(numNbrsBefore) + ",\n";
    text += "  #neighbors after = " + QString::number(nbrCount(headLabels()))
            + ".\n";
  }

  return text;
}
*/ //MichaelM removed Qstring because numNbrsBefore no longer matched and is unnecessary
   //This removal may cause problems when trying to do different colored particles

CompressionParticle& CompressionParticle::nbrAtLabel(int label) const {
  return AmoebotParticle::nbrAtLabel<CompressionParticle>(label);
}

bool CompressionParticle::hasExpNbr() const {
  for (const int label: uniqueLabels()) {
    if (hasNbrAtLabel(label) && nbrAtLabel(label).isExpanded()) {
      return true;
    }
  }

  return false;
}

bool CompressionParticle::hasExpHeadAtLabel(const int label) const {
  return hasNbrAtLabel(label) && nbrAtLabel(label).isExpanded()
         && nbrAtLabel(label).pointsAtMyHead(*this, label);
}

int CompressionParticle::redNbrCount(std::vector<int> labels) const {
  int numRedNbrs = 0;
  for (const int label : labels) {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) {
      ++numRedNbrs;
    }
  }

  return numRedNbrs;
}

int CompressionParticle::blueNbrCount(std::vector<int> labels) const {
  int numBlueNbrs = 0;
  for (const int label : labels) {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
      ++numBlueNbrs;
    }
  }

  return numBlueNbrs;
}



bool CompressionParticle::checkRedProp1(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

if (_state == State::Red) { //MichaelM only Red particles follow compression algorithm,
                                          //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
                                          //affecting rate of diffusivity
  if (S.size() == 0) {
    return false;  // S has to be nonempty for Property 1.
  } else {
    const std::vector<int> labels = uniqueLabels();
    std::set<int> redAdjNbrs;

    // Starting from the particles in S, sweep out and mark connected neighbors.
    for (int s : S) {
      redAdjNbrs.insert(s);
      int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

      // First sweep counter-clockwise, stopping when an unoccupied position or
      // expanded head is encountered.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i + offset) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) {
          redAdjNbrs.insert(label);
        } else {
          break;
        }
      }

      // Then sweep clockwise.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i - offset + labels.size()) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) {
          redAdjNbrs.insert(label);
        } else {
          break;
        }
      }
    }

    // If all neighbors are connected to a particle in S by a path through the
    // neighborhood, then the number of labels in adjNbrs should equal the total
    // number of neighbors.
    return redAdjNbrs.size() == (uint)redNbrCount(labels); //MichaelM originally was just "nbrCount"
  }
}
}

bool CompressionParticle::checkBlueProp1(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

if (_state == State::Blue && q < 0.1) { //MichaelM only Red particles follow compression algorithm,
                                          //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
                                          //affecting rate of diffusivity
  if (S.size() == 0) {
    return false;  // S has to be nonempty for Property 1.
  } else {
    const std::vector<int> labels = uniqueLabels();
    std::set<int> blueAdjNbrs;

    // Starting from the particles in S, sweep out and mark connected neighbors.
    for (int s : S) {
      blueAdjNbrs.insert(s);
      int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

      // First sweep counter-clockwise, stopping when an unoccupied position or
      // expanded head is encountered.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i + offset) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
          blueAdjNbrs.insert(label);
        } else {
          break;
        }
      }

      // Then sweep clockwise.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i - offset + labels.size()) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
          blueAdjNbrs.insert(label);
        } else {
          break;
        }
      }
    }

    // If all neighbors are connected to a particle in S by a path through the
    // neighborhood, then the number of labels in adjNbrs should equal the total
    // number of neighbors.
    return blueAdjNbrs.size() == (uint)blueNbrCount(labels); //MichaelM originally was just "nbrCount"
  }
}

}

//MichaelM: I get the error code "control may reach end of non-void function"
//I think I need something to break the if statement at the beginning of "checkprop"

bool CompressionParticle::checkRedProp2(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

 if (_state == State::Red) {   //MichaelM only Red particles follow compression algorithm,
                                             //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
                                             //affecting rate of diffusivity
  if (S.size() != 0) {     //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster                           // S has to be empty for Property 2.
    return false;  //MichaelM changed "return false" to "return true"
  } else {
    const int numRedHeadNbrs = redNbrCount(headLabels());
    const int numRedTailNbrs = redNbrCount(tailLabels());

    // Check if the head's neighbors are connected.
    int numRedAdjHeadNbrs = 0;
    bool seenNbr = false;
    for (const int label : headLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) {
        seenNbr = true;
        ++numRedAdjHeadNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Check if the tail's neighbors are connected.
    int numRedAdjTailNbrs = 0;
    seenNbr = false;
    for (const int label : tailLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) {
        seenNbr = true;
        ++numRedAdjTailNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Property 2 is satisfied if both the head and tail have at least one
    // neighbor and all head (tail) neighbors are connected.
    return (numRedHeadNbrs > 0) && (numRedTailNbrs > 0) &&
           (numRedHeadNbrs == numRedAdjHeadNbrs) && (numRedTailNbrs == numRedAdjTailNbrs);
  }
}
}

bool CompressionParticle::checkBlueProp2(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

 if (_state == State::Blue && q < 0.1) {   //MichaelM only Red particles follow compression algorithm,
                                             //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
                                             //affecting rate of diffusivity
  if (S.size() == 0) {     //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster
                           // S has to be empty for Property 2.
    return true;  //MichaelM changed "return false" to "return true"
  } else {
    const int numBlueHeadNbrs = blueNbrCount(headLabels());
    const int numBlueTailNbrs = blueNbrCount(tailLabels());

    // Check if the head's neighbors are connected.
    int numBlueAdjHeadNbrs = 0;
    bool seenNbr = false;
    for (const int label : headLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
        seenNbr = true;
        ++numBlueAdjHeadNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Check if the tail's neighbors are connected.
    int numBlueAdjTailNbrs = 0;
    seenNbr = false;
    for (const int label : tailLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
        seenNbr = true;
        ++numBlueAdjTailNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Property 2 is satisfied if both the head and tail have at least one
    // neighbor and all head (tail) neighbors are connected.
    return (numBlueHeadNbrs > 0) && (numBlueTailNbrs > 0) &&
           (numBlueHeadNbrs == numBlueAdjHeadNbrs) && (numBlueTailNbrs == numBlueAdjTailNbrs);
  }
}

} //MichaelM: I get the error code "control may reach end of non-void function"
  //I think I need something to break the if statement at the beginning of "checkprop"

//MichaelM added this similar to discodemo so that compression particles get a color
/*CompressionParticle::State CompressionParticle::getRandColor() const {
    //Randomly select an integer and return the corresponding state via casting
    return static_cast<State>(randInt(0,2));
}*/


CompressionSystem::CompressionSystem(unsigned int numRedParticles, unsigned int numBlueParticles, double lambda) {
  Q_ASSERT(lambda > 1);

  //  int numParticles = numBlueParticles + numRedParticles;
    //MichaelM added hexagon creation and random particle insertion similar to DiscoDemo but for CompressionParticles
    int sideLen = static_cast<int>(std::round(20)); //MichaelM changed 1.4 to 3.0 (control hexagon size)
     Node boundNode(0, 0);                                                     //perhaps make this a variable? to easily modify?
     for (int dir = 0; dir < 6; ++dir) {
       for (int i = 0; i < sideLen; ++i) {
         insert(new Object(boundNode));
         boundNode = boundNode.nodeInDir(dir);
       }
     }

     // Let s be the bounding hexagon side length. When the hexagon is created as
      // above, the nodes (x,y) strictly within the hexagon have (i) -s < x < s,
      // (ii) 0 < y < 2s, and (iii) 0 < x+y < 2s. Choose interior nodes at random to
      // place particles, ensuring at most one particle is placed at each node.

     std::set<Node> occupied;
   //  unsigned int numParticlesAdded = 0;
     unsigned int numRedAdded = 0;
     while (numRedAdded < numRedParticles) {
       // First, choose an x and y position at random from the (i) and (ii) bounds.
       int x = randInt(-sideLen + 1, sideLen);
       int y = randInt(1, 2 * sideLen);
       Node node(x, y);

       // If the node satisfies (iii) and is unoccupied, place a particle there.
       if (0 < x + y && x + y < 2 * sideLen
           && occupied.find(node) == occupied.end()) {
         insert(new CompressionParticle(node, -1, randDir(), *this, lambda, CompressionParticle::State::Red));
         occupied.insert(node);
         numRedAdded++;
       }
   }

     unsigned int numBlueAdded = 0;
     if (numBlueParticles > 0) {
     while (numBlueAdded < numBlueParticles) {
       // First, choose an x and y position at random from the (i) and (ii) bounds.
       int x = randInt(-sideLen + 1, sideLen);
       int y = randInt(1, 2 * sideLen);
       Node blueNode(x, y);

       // If the node satisfies (iii) and is unoccupied, place a particle there.
       if (0 < x + y && x + y < 2 * sideLen
           && occupied.find(blueNode) == occupied.end()) {
         insert(new CompressionParticle(blueNode, -1, randDir(), *this, lambda, CompressionParticle::State::Blue));
         occupied.insert(blueNode);
         numBlueAdded++;
       }
   }
     }
      _measures.push_back(new PerimeterMeasure("Perimeter", 1, *this));

      }


bool CompressionSystem::hasTerminated() const {
  #ifdef QT_DEBUG
    if (!isConnected(particles)) {
        return true;
    }
  #endif

  return false;
}


PerimeterMeasure::PerimeterMeasure(const QString name, const unsigned int freq,
                                   CompressionSystem& system)
    : Measure(name, freq),
      _system(system) {}

double PerimeterMeasure::calculate() const {
  int numEdges = 0;
  for (const auto& p : _system.particles) {
    auto comp_p = dynamic_cast<CompressionParticle*>(p);
    auto tailLabels = comp_p->isContracted() ? comp_p->uniqueLabels()
                                             : comp_p->tailLabels();
    for (const int label : tailLabels) {
      if (comp_p->hasNbrAtLabel(label) && !comp_p->hasExpHeadAtLabel(label)) {
        ++numEdges;
      }
    }
  }

  return (3 * _system.size()) - (numEdges / 2) - 3;
}

