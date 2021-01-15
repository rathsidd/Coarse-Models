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

    double x = 1.0; //Diffusion Rate without neighbors. All values acceptable.
    double y = 0.6; //Binding Affinity when encountering new neighbors. ALl values above 0.5 are reasonable. ("updates" / "updates2" was 0.2)
    double z = 0.4; //Affinity to detach from cluster. (updates / updates2 was 0.8)
    double a = 0.0015; //Old A value was 0.0015
    double b = 1.2; //Probability to detach from end of line

       if (isContracted()) {
int expandDir = randDir();      //Store a potential direction to expand into (for use later)
q = randDouble(0, 1);

//      //      //      FOR WT-GRBP5        //      //      //

if (!hasRBNbrInLine() && !stuckInRedLine()) {       //REMOVED && _state != State::Black &&

    if (_state == State::Red) {
        if (q < 0.4032) {
            _state = State::Blue;
        } else { _state = State::Red; }
    } else if (_state == State::Blue) {
        if (q < 0.5968) {
            _state = State::Red;
        } else { _state = State::Blue; }
    }
     if (_state == State::Black && !hasRBNbrInLine() && !stuckInRedLine()) {
         if (q < 0.5968) {
             _state = State::Red;
         } if (q > 0.5968) {
             _state = State::Blue;
  }
     }
}

//  //  //      FOR WT-GRBP5-8A     //      //      //

/* if (!hasRBNbrInLine() && !stuckInRedLine()) { //REMOVED && _state != State::Black &&

     if (_state == State::Red) {
         if (q < 0.5) {
             _state = State::Red;
         } else { _state = State::Blue;
     }
     }
     else if (_state == State::Blue) {
         if (q < 0.333) {
             _state = State::Green;
         }
         if (q > 0.333 && q < 0.666) {
             _state = State::Blue;
         }
         if (q > 0.666) {
             _state = State::Red;
         }

     }
     else if (_state == State::Green) {
              if (q < 0.5) {
                  _state = State::Green;
              } else { _state = State::Blue;
          }
          }

     if (_state == State::Black && !hasRBNbrInLine() && !stuckInRedLine()) {
         if (q < 0.33) {
             _state = State::Red;
         } if (q > 0.33 && q < 0.66) {
             _state = State::Blue;
         } if (q > 0.66) {
             _state = State::Green;
         }
     }
   }
*/

if (stuckInRedLine() && _state == State::Red) {
    _state = State::Black;
}

if (hasRBNbrInLine() && !stuckInRedLine() && _state == State::Red && q < a) {     //If it is in a line with another particle, decide if it will turn black or not.
    _state = State::Black;
}

if (hasRBNbrInLine() && _state == State::Red) {                 //If it is at the end of a line, pick q value so that once it expands it will contract back to be at the front of the line
    q = randDouble(1, 2);
    z = 0.00;
}

if (!hasRBNbrInLine() && q < 1 && _state != State::Black) { //Left out "&& redNbrCount(uniqueLabels()) == 0"
    _direction = rand() % 3;
}

       if (canExpand(expandDir) && !hasExpNbr()) {
      // Count neighbors in original position and expand.
      //numRedNbrsBefore = redNbrCount(uniqueLabels());
      //numRedNbrsSameDirBefore = redNbrCountSameDir(uniqueLabels());
        numNbrsBefore = nbrCount(uniqueLabels());
        numNbrsSameDirBefore = nbrCountSameDir(uniqueLabels());
      expand(expandDir);


      if (_state == State::Black) {
          contractHead();
      }
      if (q > b && _state == State::Red) { //If its red, and its in a line or stuck in a line, contract head most of time (due to b value)
          contractHead();
      }


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
      //if (numNbrsBefore > 0 && )

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

if (system.getCount("# Activations")._value < 1000000000) {
  if (system.getCount("# Activations")._value % 8000 == 0) {
        int sideLen = static_cast<int>(std::round(50));

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
  else if (_state == State::Green){
      return 0x00ff00;
  }
  else if (_state == State::Black){
      return 000000;
      //return -1;
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
*/

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

//this determines whether a particle has a red or black neighbor (RB) that it is aligned with
bool CompressionParticle::hasRBNbrInLine() const {

    if (_direction == 0) {
    if (hasNbrAtLabel(0)) {
        if (nbrAtLabel(0)._direction == 0 && (nbrAtLabel(0)._state == State::Red || nbrAtLabel(0)._state == State::Black)) {
            return true;
        }
    }
     if (hasNbrAtLabel(3)) {
        if (nbrAtLabel(3)._direction == 0 && (nbrAtLabel(3)._state == State::Red || nbrAtLabel(3)._state == State::Black)) {
                    return true;
                }
    }
}
     if (_direction == 1) {
    if (hasNbrAtLabel(1)) {
        if (nbrAtLabel(1)._direction == 1 && (nbrAtLabel(1)._state == State::Red || nbrAtLabel(1)._state == State::Black)) {
            return true;
        }
    }
     if (hasNbrAtLabel(4)) {
        if (nbrAtLabel(4)._direction == 1 && (nbrAtLabel(4)._state == State::Red || nbrAtLabel(4)._state == State::Black)) {
                    return true;
                }
    }
}
    if (_direction == 2) {
    if (hasNbrAtLabel(2)) {
        if (nbrAtLabel(2)._direction == 2 && (nbrAtLabel(2)._state == State::Red || nbrAtLabel(2)._state == State::Black)) {
            return true;
        }
    }
     if (hasNbrAtLabel(5)) {
        if (nbrAtLabel(5)._direction == 2 && (nbrAtLabel(5)._state == State::Red || nbrAtLabel(5)._state == State::Black)) {
                    return true;
                }
    }
}
    return false;
}


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

bool CompressionParticle::stuckInRedLine() const {

    if (_direction == 0) {
    if (hasNbrAtLabel(0) && hasNbrAtLabel(3)) {
        if (nbrAtLabel(0)._direction == 0 && nbrAtLabel(3)._direction == 0
                && (nbrAtLabel(0)._state == State::Red || nbrAtLabel(0)._state == State::Black)
                && (nbrAtLabel(3)._state == State::Red || nbrAtLabel(3)._state == State::Black)) {
            return true;
        }
    }
}

    if (_direction == 1) {
    if (hasNbrAtLabel(1) && hasNbrAtLabel(4)) {
        if (nbrAtLabel(1)._direction == 1 && nbrAtLabel(4)._direction == 1
                && (nbrAtLabel(1)._state == State::Red || nbrAtLabel(1)._state == State::Black)
                && (nbrAtLabel(4)._state == State::Red || nbrAtLabel(4)._state == State::Black)) {
            return true;
        }
    }
}

    if (_direction == 2) {
    if (hasNbrAtLabel(2) && hasNbrAtLabel(5)) {
        if (nbrAtLabel(2)._direction == 2 && nbrAtLabel(5)._direction == 2
                && (nbrAtLabel(2)._state == State::Red || nbrAtLabel(2)._state == State::Black)
                && (nbrAtLabel(5)._state == State::Red || nbrAtLabel(5)._state == State::Black)) {
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

bool CompressionParticle::checkProp1(std::vector<int> S) const { //Check Property 1 as it applies to all particles (regardless of color)
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

bool CompressionParticle::checkProp2(std::vector<int> S) const { //Check Property 2 as it applies to all particles (regardless of color)
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

bool CompressionParticle::checkRedProp1(std::vector<int> S) const { //Check Property 1 as it applies to only red particles
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

bool CompressionParticle::checkBlueProp1(std::vector<int> S) const {    //Check Property 1 as it applies to only blue particles
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

bool CompressionParticle::checkRedProp2(std::vector<int> S) const {     //Check Property 2 as it applies only to red particles
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

bool CompressionParticle::checkBlueProp2(std::vector<int> S) const { //Check Property 2 as it applies to only blue particles
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

}

CompressionSystem::CompressionSystem(unsigned int numRedParticles, unsigned int numBlueParticles, unsigned int numGreenParticles, double lambda) {
  Q_ASSERT(lambda > 1);

  //  int numParticles = numBlueParticles + numRedParticles;
    //MichaelM added hexagon creation and random particle insertion similar to DiscoDemo but for CompressionParticles
    int sideLen = static_cast<int>(std::round(50)); //MichaelM changed 1.4 to 3.0 (control hexagon size)
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
         insert(new CompressionParticle(blueNode, -1, 0, *this, lambda, CompressionParticle::State::Blue));
         occupied.insert(blueNode);
         numBlueAdded++;
       }
   }
     }

     unsigned int numGreenAdded = 0;
     if (numGreenParticles > 0) {
     while (numGreenAdded < numGreenParticles) {
       // First, choose an x and y position at random from the (i) and (ii) bounds.
       int x = randInt(-sideLen + 1, sideLen);
       int y = randInt(1, 2 * sideLen);
       Node greenNode(x, y);

       // If the node satisfies (iii) and is unoccupied, place a particle there.
       if (0 < x + y && x + y < 2 * sideLen
           && occupied.find(greenNode) == occupied.end()) {
         insert(new CompressionParticle(greenNode, -1, 0, *this, lambda, CompressionParticle::State::Green));
         occupied.insert(greenNode);
         numGreenAdded++;
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
