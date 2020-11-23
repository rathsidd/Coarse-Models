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
    numNbrsBefore(0),
    numRedNbrsBefore(0),
    numBlueNbrsBefore(0),
    numNbrsSameDirBefore(0),
    numRedNbrsSameDirBefore(0),
    flag(false),
    _state(state) {
    _direction = rand() % 3;
    }



void CompressionParticle::activate() {

//   if (_state == State::Red) {

/*    if (hasNbrInLine()) {
        _state = State::Blue;
    }

    if (!hasNbrInLine()) {
        _state = State::Red;
    } */

/*    if (atEndOfLine()) {
        _state = State::Blue;
    }

    if (!atEndOfLine()) {
        _state = State::Red;
    } */


    double x = 1.0; //Diffusion Rate without neighbors. All values acceptable.
    double y = 0.2; //Binding Affinity when encountering new neighbors. ALl values above 0.5 are reasonable.
    double z = 0.8; //Affinity to detach from cluster. Values less than .1 are acceptable.
//    double q2 = 0;

       if (isContracted()) {
int expandDir = randDir();      //Store a potential direction to expand into (for use later)
q = randDouble(0, 1);

if (hasNbrInLine() && q < 0.0015) {     //If it is in a line with another particle, decide if it will turn blue or not.
    _state = State::Blue;
}


/*           if (_direction == 0) {
               if (q < 0.51) {
               expandDir = 0;
} else expandDir = 3;
              } else if (_direction == 1) {
               if (q < 0.51) {
               expandDir = 1;
} else expandDir = 4;
               } else if (_direction == 2) {
               if (q < 0.51) {
               expandDir = 2;
} else expandDir = 5;
            } else if (_direction == 3) {
               if (q < 0.51) {
               expandDir = 3;
} else expandDir = 0;
            } else if (_direction == 4) {
               if (q < 0.51) {
               expandDir = 4;
} else expandDir = 1;
            } else if (_direction == 5) {
               if (q < 0.51) {
               expandDir = 5;
} else expandDir = 2;
            } */

if (!hasNbrInLine() && !stuckInLine()) {    //If by itself, turn back to red to move freely
    _state = State::Red;
}

if (hasNbrInLine() && stuckInLine() && q > 0.0015) {    //If it is sandwiched in a line but q is above the "convert to blue value" then do not break free
    z = 0.00;                                           //Choose q so that if it does expand, it will automatically contract head (for use later)
    q = 2;
   } else if (hasNbrInLine() && q < 0.0015) {           //Decide again if it will convert to blue (but this time if its stuck in line, too, not just at the end)
        z = 0.00;
        q = 2;
        _state = State::Blue;
    }

if (hasNbrInLine() && !stuckInLine()) {                 //If it is at the end of a line, pick q value so that once it expands it will contract back to be at the front of the line
    q = randDouble(1, 2);                               //Should this be random chance? IMPORTANT
    z = 0.00;
}

if (!hasNbrInLine() && q < 1) { //Left out "&& redNbrCount(uniqueLabels()) == 0"
    _direction = rand() % 3;
}

/* if (!hasNbrInLine()) {
if (q < 0.05 || q > 0.95) {
    expandDir = randDir();
}
} */

/* if (nbrCountSameDir(uniqueLabels()) == 1) {
    for (int dir = 0; dir < 6; ++dir) {

        if (hasNbrAtLabel(dir) && nbrAtLabel(dir)._direction == _direction) {
                if (dir < 5 && dir > 0) {
                    if (q < 0.51) {
            expandDir = dir + 1;
            } else {
                        expandDir = dir - 1;
                    }
}
        if  (dir == 5) {
           if (q < 0.51) {
             expandDir = 0;
             } else {
                    expandDir = 4;
                      }

}
    if (dir == 0) {
     if (q < 0.51) {
        expandDir = 1;
          } else {
                expandDir = 5;
              }

}
}
}
} */


       if (canExpand(expandDir) && !hasExpNbr()) {
      // Count neighbors in original position and expand.
      //numRedNbrsBefore = redNbrCount(uniqueLabels());
      //numRedNbrsSameDirBefore = redNbrCountSameDir(uniqueLabels());
        numNbrsBefore = nbrCount(uniqueLabels());
        numNbrsSameDirBefore = nbrCountSameDir(uniqueLabels());
      expand(expandDir);

//      if (q == 2) {
  //           contractHead();
//}
      if (_state == State::Blue) {
          contractHead();
      } else if (q > 1.1) { //If its red, and its in a line or stuck in a line, contract head most of time
          contractHead();
      }

/*      if (q > 1 && q < 1.01) {
          contractTail();
      }

      if (q > 1 && 1 > 1.01) {
          contractHead();
      } */


      flag = !hasExpNbr();
    }
       }   else {  // isExpanded().
     //int numRedNbrsAfter = redNbrCount(headLabels());
     //int numRedNbrsSameDirAfter = redNbrCountSameDir(headLabels());
     int numNbrsAfter = nbrCount(headLabels());
     int numNbrsSameDirAfter = nbrCountSameDir(headLabels());

     if (!flag || numNbrsSameDirBefore == 5) {
contractHead();
}

        else if (numNbrsAfter == 0 && numNbrsBefore == 0 && q < x) { //Diffusion rate
             contractTail();
         }
     else if (numNbrsAfter == 0 && numNbrsBefore == 0 && q > x) {
         contractHead();
     }

     else if (numNbrsAfter != 0 && numNbrsBefore == 0 && q < y) {
         contractTail();
     }
     else if (numNbrsAfter != 0 && numNbrsBefore == 0 && q > y) {
         contractHead();
     }
     else if (numNbrsSameDirAfter == 1) {
         contractTail();
     }



    else {
      // Count neighbors in new position and compute the set S.
//      int numRedNbrsAfter = redNbrCount(headLabels());
      std::vector<int> S;
      for (const int label : {headLabels()[4], tailLabels()[4]}) {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) { //Left out "&& nbrAtLabel(label)._direction == _direction"
          S.push_back(label);
        }
      }

      if (q < z) {
          contractTail();
     }

      // If the conditions are satisfied, contract to the new position;
      // otherwise, contract back to the original one.

         else if ((q < pow(lambda, numNbrsAfter - numNbrsBefore))
          && (checkProp1(S) || checkProp2(S))) {
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

if (system.getCount("# Activations")._value < 18000000) {
  if (system.getCount("# Activations")._value % 5000 == 0) {
        int sideLen = static_cast<int>(std::round(30));

        int x = randInt(-sideLen + 1, sideLen);
        int y = randInt(1, 2 * sideLen);
        Node node(x, y);

        // If the node satisfies (iii) and is unoccupied, place a particle there.
        if (0 < x + y && x + y < 2 * sideLen
            && system.particleMap.find(node) == system.particleMap.end()) {
            //if (DiscoDemoSystem::getClusters() > 1) {
          system.insert(new CompressionParticle(node, -1, 0, system, lambda, CompressionParticle::State::Red));
  }

    }
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

double CompressionParticle::headMarkDir() const {
    return _direction;
}

   int CompressionParticle::tailMarkColor() const {
     return headMarkColor();
   }

   int CompressionParticle::tailMarkDir() const {
       return _direction;
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
bool CompressionParticle::hasNbrInLine() const {

    if (_direction == 0) {
    if (hasNbrAtLabel(0)) {
        if (nbrAtLabel(0)._direction == 0) {
            return true;
        }
    }

     if (hasNbrAtLabel(3)) {
        if (nbrAtLabel(3)._direction == 0) {
                    return true;
                }
    }
}

     if (_direction == 1) {
    if (hasNbrAtLabel(1)) {
        if (nbrAtLabel(1)._direction == 1) {
            return true;
        }
    }

     if (hasNbrAtLabel(4)) {
        if (nbrAtLabel(4)._direction == 1) {
                    return true;
                }
    }
}

    if (_direction == 2) {
    if (hasNbrAtLabel(2)) {
        if (nbrAtLabel(2)._direction == 2) {
            return true;
        }
    }

     if (hasNbrAtLabel(5)) {
        if (nbrAtLabel(5)._direction == 2) {
                    return true;
                }
    }
}
    return false;
}

/* int CompressionParticle::almostInLine() const {
    //bool hasOneAdjNbrSameDir = false;

    if (redNbrCountSameDir(uniqueLabels()) == 1) {

    }

} */






/*    if (_direction == 0) {
    if (hasNbrAtLabel(0) || hasNbrAtLabel(3)) {
        if (nbrAtLabel(0)._direction == 0 || nbrAtLabel(3)._direction == 0) {
            return true;
        } else {return false;}
    }
    else {return false;}
} else {
    return false;
}
} */






 //   if (hasNbrAtLabel(_direction) && nbrAtLabel(_direction)._direction == _direction) {
//        return true;
 //   }


//    for (int dir = 0; dir < 6; dir++) {
//      if (hasNbrAtLabel(dir) && dir == _direction && nbrAtLabel(dir)._direction == _direction) {

//}
//}






/*    for (int label : uniqueLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._direction == _direction && pointsAtMe(nbrAtLabel(label), nbrAtLabel(label)._direction)) {
            return true;
        }
    }
    return false;
} */



    //    int numRedNbrsWithSameDir = 0;
 /*   if (_direction == 0) {
        if (hasNbrAtLabel(0) || hasNbrAtLabel(3)) {
            if (nbrAtLabel(0)._direction == 0 || nbrAtLabel(0)._direction == 3 || nbrAtLabel(3)._direction == 0 || nbrAtLabel(3)._direction == 3) {
                return true;
            }
        }
    } else if (_direction == 1) {
        if (hasNbrAtLabel(1) || hasNbrAtLabel(4)) {
            if (nbrAtLabel(1)._direction == 1 || nbrAtLabel(1)._direction == 4 || nbrAtLabel(4)._direction == 1 || nbrAtLabel(4)._direction == 4) {
                return true;
            }
        }
    } else if (_direction == 2) {
        if (hasNbrAtLabel(2) || hasNbrAtLabel(5)) {
            if (nbrAtLabel(2)._direction == 2 || nbrAtLabel(2)._direction == 5 || nbrAtLabel(5)._direction == 2 || nbrAtLabel(5)._direction == 5) {
                return true;
            }
        }
    }
     return false; */

//}

/* int CompressionParticle::hasAdjNbrWithSameDir() const {
    for (const int label: uniqueLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._direction == _direction) {
          int expandDir = ++label;
        }
    }
} */

/* bool CompressionParticle::atEndOfLine() const {
    if (hasNbrInLine() && redNbrCountSameDir(uniqueLabels()) == 1) {
        return true;
    }
    else { return false; }
} */

bool CompressionParticle::stuckInLine() const {

    if (_direction == 0) {
    if (hasNbrAtLabel(0) && hasNbrAtLabel(3)) {
        if (nbrAtLabel(0)._direction == 0 && nbrAtLabel(3)._direction == 0) {
            return true;
        }
    }
}

    if (_direction == 1) {
    if (hasNbrAtLabel(1) && hasNbrAtLabel(4)) {
        if (nbrAtLabel(1)._direction == 1 && nbrAtLabel(4)._direction == 1) {
            return true;
        }
    }
}

    if (_direction == 2) {
    if (hasNbrAtLabel(2) && hasNbrAtLabel(5)) {
        if (nbrAtLabel(2)._direction == 2 && nbrAtLabel(5)._direction == 2) {
            return true;
        }
    }
}

    return false;
}

bool CompressionParticle::hasExpHeadAtLabel(const int label) const {
  return hasNbrAtLabel(label) && nbrAtLabel(label).isExpanded()
         && nbrAtLabel(label).pointsAtMyHead(*this, label);
}

int CompressionParticle::nbrCount(std::vector<int> labels) const {
  int numNbrs = 0;
  for (const int label : labels) {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) {
      ++numNbrs;
    }
  }

  return numNbrs;
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

 int CompressionParticle::redNbrCountSameDir(std::vector<int> labels) const {
  int numRedNbrsSameDir = 0;
  for (const int label : labels) {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._direction == _direction) {
      ++numRedNbrsSameDir;
    }
  }

  return numRedNbrsSameDir;
}

 int CompressionParticle::nbrCountSameDir(std::vector<int> labels) const {
  int numNbrsSameDir = 0;
  for (const int label : labels) {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._direction == _direction) {
      ++numNbrsSameDir;
    }
  }

  return numNbrsSameDir;
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

bool CompressionParticle::checkProp1(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

//if (_state == State::Red) { //MichaelM only Red particles follow compression algorithm,
                                          //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
                                          //affecting rate of diffusivity
  if (S.size() == 0) {
    return false;  // S has to be nonempty for Property 1.
  } else {
    const std::vector<int> labels = uniqueLabels();
    std::set<int> adjNbrs;

    // Starting from the particles in S, sweep out and mark connected neighbors.
    for (int s : S) {
      adjNbrs.insert(s);
      int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

      // First sweep counter-clockwise, stopping when an unoccupied position or
      // expanded head is encountered.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i + offset) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) { //Left out "&& nbrAtLabel(label)._direction == _direction"
          adjNbrs.insert(label);
        } else {
          break;
        }
      }

      // Then sweep clockwise.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i - offset + labels.size()) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) { //Left out "&& nbrAtLabel(label)._direction == _direction"
          adjNbrs.insert(label);
        } else {
          break;
        }
      }
    }

    // If all neighbors are connected to a particle in S by a path through the
    // neighborhood, then the number of labels in adjNbrs should equal the total
    // number of neighbors.
    return adjNbrs.size() == (uint)nbrCount(labels); //MichaelM originally was just "nbrCount"
  }
}

bool CompressionParticle::checkProp2(std::vector<int> S) const {
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag);  // Not required, but equivalent/cleaner for implementation.

 //if (_state == State::Red) {   //MichaelM only Red particles follow compression algorithm,
                                             //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
                                             //affecting rate of diffusivity
  if (S.size() != 0) {     //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster                           // S has to be empty for Property 2.
    return false;  //MichaelM changed "return false" to "return true"
  } else {
//    const int numRedHeadNbrsSameDir = redNbrCountSameDir(headLabels());
//    const int numRedTailNbrsSameDir = redNbrCountSameDir(tailLabels());
    const int numHeadNbrs = nbrCount(headLabels());
    const int numTailNbrs = nbrCount(tailLabels());

    // Check if the head's neighbors are connected.
    int numAdjHeadNbrs = 0;
    bool seenNbr = false;
    for (const int label : headLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) { //Left out "&& nbrAtLabel(label)._direction == _direction"
        seenNbr = true;
        ++numAdjHeadNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Check if the tail's neighbors are connected.
    int numAdjTailNbrs = 0;
    seenNbr = false;
    for (const int label : tailLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label)) {
        seenNbr = true;
        ++numAdjTailNbrs;
      } else if (seenNbr) {
        break;
      }
    }

    // Property 2 is satisfied if both the head and tail have at least one
    // neighbor and all head (tail) neighbors are connected.
    return (numHeadNbrs > 0) && (numTailNbrs > 0) &&
           (numHeadNbrs == numAdjHeadNbrs) && (numTailNbrs == numAdjTailNbrs);
  }
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
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) { //Left out "&& nbrAtLabel(label)._direction == _direction"
          redAdjNbrs.insert(label);
        } else {
          break;
        }
      }

      // Then sweep clockwise.
      for (uint offset = 1; offset < labels.size(); ++offset) {
        int label = labels[(i - offset + labels.size()) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) { //Left out "&& nbrAtLabel(label)._direction == _direction"
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
//    const int numRedHeadNbrsSameDir = redNbrCountSameDir(headLabels());
//    const int numRedTailNbrsSameDir = redNbrCountSameDir(tailLabels());
    const int numRedHeadNbrs = redNbrCount(headLabels());
    const int numRedTailNbrs = redNbrCount(tailLabels());

    // Check if the head's neighbors are connected.
    int numRedAdjHeadNbrs = 0;
    bool seenNbr = false;
    for (const int label : headLabels()) {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red) { //Left out "&& nbrAtLabel(label)._direction == _direction"
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
    int sideLen = static_cast<int>(std::round(30)); //MichaelM changed 1.4 to 3.0 (control hexagon size)
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
         insert(new CompressionParticle(node, -1, 0, *this, lambda, CompressionParticle::State::Red));
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





































//OLD COMPRESSION CODE FOR FORCED LINEAR FORMATION
/*   if (_state == State::Red) {
       if (isContracted()) {

           q = randDouble(0, 1);
           int expandDir = randDir();
          if (q< 0.5) {
               expandDir = 3;
           } else {
               expandDir = 3;
           }

if (system.getCount("# Activations")._value < (pow(99, 99999999999999999))) {
       if (canExpand(expandDir) && !hasExpNbr()) {
      // Count neighbors in original position and expand.
      numRedNbrsBefore = redNbrCount(uniqueLabels());
      expand(expandDir);
      flag = !hasExpNbr();
    }
}
      else {
   if (q < 0.00) {
        if (canExpand(expandDir) && !hasExpNbr()) {
              // Count neighbors in original position and expand.
              numRedNbrsBefore = redNbrCount(uniqueLabels());
              expand(expandDir);
              flag = !hasExpNbr();
            }
   }
        else {
        if (CompressionParticle::head.y % 5 == 0) {
          if (canExpand(expandDir) && !hasExpNbr() && (localToGlobalDir(expandDir) == 0 || localToGlobalDir(expandDir) == 3)) {
              numRedNbrsBefore = redNbrCount(uniqueLabels());
              expand(expandDir);
              flag = !hasExpNbr();
          }
    }
        else if (CompressionParticle::head.x % 5 == 0) {
            if (canExpand(expandDir) && !hasExpNbr() && (localToGlobalDir(expandDir) == 2 || localToGlobalDir(expandDir) == 5)) {
                numRedNbrsBefore = redNbrCount(uniqueLabels());
                expand(expandDir);
                flag = !hasExpNbr();
            }
        }



        else if (CompressionParticle::head.x % 5 == 0) {
            if (canExpand(expandDir) && !hasExpNbr() && (localToGlobalDir(expandDir) == 1 || localToGlobalDir(expandDir) == 4)) {
                numRedNbrsBefore = redNbrCount(uniqueLabels());
                expand(expandDir);
                flag = !hasExpNbr();
            }
        }
        else if (CompressionParticle::head.x == (CompressionParticle::head.y * (-1))) {
            if (canExpand(expandDir) && !hasExpNbr() && (localToGlobalDir(expandDir) == 2 || localToGlobalDir(expandDir) == 5)) {
                numRedNbrsBefore = redNbrCount(uniqueLabels());
                expand(expandDir);
                flag = !hasExpNbr();
            }
        }
        else {
            if (canExpand(expandDir) && !hasExpNbr()) {
                  // Count neighbors in original position and expand.
                  numRedNbrsBefore = redNbrCount(uniqueLabels());
                  expand(expandDir);
                  flag = !hasExpNbr();
                }
        }
       }
}

       }   else {  // isExpanded().
     int numRedNbrsAfter = redNbrCount(headLabels());
     double x = 1.0; //Diffusion Rate without neighbors. All values acceptable.
     double y = 0.7; //Binding Affinity when encountering new neighbors. ALl values above 0.5 are reasonable.
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
   else if (_state == State::Blue) {
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
    }
}

   if (system.getCount("# Activations")._value % 50 == 0) {
        int sideLen = static_cast<int>(std::round(2 * std::sqrt(5)));

        int x = randInt(-sideLen + 1, sideLen);
        int y = randInt(1, 2 * sideLen);
        Node node(x, y);

        // If the node satisfies (iii) and is unoccupied, place a particle there.
        if (0 < x + y && x + y < 2 * sideLen
            && system.particleMap.find(node) == system.particleMap.end()) {
            //if (DiscoDemoSystem::getClusters() > 1) {
          system.insert(new CompressionParticle(node, -1, randDir(), system, lambda, CompressionParticle::State::Red));
  }

    }
} */
