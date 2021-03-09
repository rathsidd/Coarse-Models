/* Copyright (C) 2020 Joshua J. Daymude, Robert Gmyr, and Kristian Hinnenthal.
 * The full GNU GPLv3 can be found in the LICENSE file, and the full copyright
 * notice can be found at the top of main/main.cpp. */

#include "alg/compression.h"

#include <algorithm> // For distance() and find().
#include <set>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <iterator>
#include <unordered_set>

#include <QtGlobal>

CompressionParticle::CompressionParticle(const Node head,
                                         const int globalTailDir,
                                         const int orientation,
                                         AmoebotSystem &system,
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
      _state(state)
{
  _direction = rand() % 3;
}

void CompressionParticle::activate()
{
  double x = this->system.diffusionRate;    //Diffusion Rate without neighbors. All values acceptable.
  double y = this->system.bindingAffinity;    //Binding Affinity when encountering new neighbors. ALl values above 0.5 are reasonable. ("updates" / "updates2" was 0.2)
  double z = this->system.seperationAffinity; ;    //Affinity to detach from cluster. (updates / updates2 was 0.8)
  double a = this->system.convertToStable; //Old A value was 0.0015
  double b = this->system.detachFromLine;   //Probability to detach from end of line

/*
  std::cout << "lambda " << lambda << std::endl;
  std::cout << "diffusionRate " << x << std::endl;
  std::cout << "bindingAffinity " << y << std::endl;
  std::cout << "seperationAffinity " << z << std::endl;
  std::cout << "convertToStable " << a << std::endl;
  std::cout << "detachFromLine " << b << std::endl;
  std::cout << std::endl;
  */

  if (isContracted())
  {
    int expandDir = randDir(); //Store a potential direction to expand into (for use later)
    q = randDouble(0, 1);

    //      //      //      FOR WT-GRBP5        //      //      //

    if (!hasRBNbrInLine() && !stuckInRedLine())
    { //REMOVED && _state != State::Black &&

      if (_state == State::Red)
      {
        if (q < 0.4032)
        {
          _state = State::Blue;
        }
        else
        {
          _state = State::Red;
        }
      }
      else if (_state == State::Blue)
      {
        if (q < 0.5968)
        {
          _state = State::Red;
        }
        else
        {
          _state = State::Blue;
        }
      }
      if (_state == State::Black && !hasRBNbrInLine() && !stuckInRedLine())
      {
        if (q < 0.5968)
        {
          _state = State::Red;
        }
        if (q > 0.5968)
        {
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

    if (stuckInRedLine() && _state == State::Red)
    {
      _state = State::Black;
    }

    if (hasRBNbrInLine() && !stuckInRedLine() && _state == State::Red && q < a)
    { //If it is in a line with another particle, decide if it will turn black or not.
      _state = State::Black;
    }

    if (hasRBNbrInLine() && _state == State::Red)
    { //If it is at the end of a line, pick q value so that once it expands it will contract back to be at the front of the line
      q = randDouble(1, 2);
      z = 0.00;
    }

    if (!hasRBNbrInLine() && q < 1 && _state != State::Black)
    { //Left out "&& redNbrCount(uniqueLabels()) == 0"
      _direction = rand() % 3;
    }

    if (canExpand(expandDir) && !hasExpNbr())
    {
      // Count neighbors in original position and expand.
      //numRedNbrsBefore = redNbrCount(uniqueLabels());
      //numRedNbrsSameDirBefore = redNbrCountSameDir(uniqueLabels());
      numNbrsBefore = nbrCount(uniqueLabels());
      numNbrsSameDirBefore = nbrCountSameDir(uniqueLabels());
      expand(expandDir);

      if (_state == State::Black)
      {
        contractHead();
      }
      if (q > b && _state == State::Red)
      { //If its red, and its in a line or stuck in a line, contract head most of time (due to b value)
        contractHead();
      }

      flag = !hasExpNbr();
    }
  }
  else
  { // isExpanded().
    //int numRedNbrsAfter = redNbrCount(headLabels());
    //int numRedNbrsSameDirAfter = redNbrCountSameDir(headLabels());
    int numNbrsAfter = nbrCount(headLabels());
    int numNbrsSameDirAfter = nbrCountSameDir(headLabels());

    if (!flag || numNbrsSameDirBefore == 5)
    {
      contractHead();
    }

    else if (numNbrsAfter == 0 && numNbrsBefore == 0 && q < x)
    { //Diffusion rate
      contractTail();
    }
    else if (numNbrsAfter == 0 && numNbrsBefore == 0 && q > x)
    {
      contractHead();
    }

    else if (numNbrsAfter != 0 && numNbrsBefore == 0 && q < y)
    {
      contractTail();
    }
    else if (numNbrsAfter != 0 && numNbrsBefore == 0 && q > y)
    {
      contractHead();
    }
    else if (numNbrsSameDirAfter == 1)
    {
      contractTail();
    }

    else
    {
      // Count neighbors in new position and compute the set S.
      //      int numRedNbrsAfter = redNbrCount(headLabels());
      std::vector<int> S;
      for (const int label : {headLabels()[4], tailLabels()[4]})
      {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
        { //Left out "&& nbrAtLabel(label)._direction == _direction"
          S.push_back(label);
        }
      }

      if (q < z)
      {
        contractTail();
      }
      //if (numNbrsBefore > 0 && )

      // If the conditions are satisfied, contract to the new position;
      // otherwise, contract back to the original one.

      else if ((q < pow(lambda, numNbrsAfter - numNbrsBefore)) && (checkProp1(S) || checkProp2(S)))
      {
        contractTail();
      }
      else
      {
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

  if (system.getCount("# Activations")._value < 1000000000)
  {
    if (system.getCount("# Activations")._value % 8000 == 0)
    {
      //int sideLen = system.sideLen;//static_cast<int>(std::round(5));

      int x = randInt(-system.sideLen + 1, system.sideLen);
      int y = randInt(1, 2 * system.sideLen);
      Node node(x, y);

      // If the node satisfies (iii) and is unoccupied, place a particle there.
      if (0 < x + y && x + y < 2 * system.sideLen && system.particleMap.find(node) == system.particleMap.end())
      {
        //if (DiscoDemoSystem::getClusters() > 1) {
        system.insert(new CompressionParticle(node, -1, 0, system, lambda, CompressionParticle::State::Red));
        //system.nodesOccupied++;
        //nodesOccupied++;
      }
    }
  }
  // system.getCount("Surface Coverage").record(round(system.size()/((3*sqrt(3) * pow(50, 2))/2)));
  //(system).getClusters();
}
// end of activate

int CompressionParticle::headMarkColor() const
{
  if (_state == State::Red)
  {
    return 0xff0000;
  }
  else if (_state == State::Blue)
  {
    return 0x0000ff;
  }
  else if (_state == State::Green)
  {
    return 0x00ff00;
  }
  else if (_state == State::Black)
  {
    return 000000;
    //return -1;
  }
  return -1;
}

double CompressionParticle::headMarkDir() const
{
  return _direction;
}

int CompressionParticle::tailMarkColor() const
{
  return headMarkColor();
}

int CompressionParticle::tailMarkDir() const
{
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

CompressionParticle &CompressionParticle::nbrAtLabel(int label) const
{
  return AmoebotParticle::nbrAtLabel<CompressionParticle>(label);
}

bool CompressionParticle::hasExpNbr() const
{
  for (const int label : uniqueLabels())
  {
    if (hasNbrAtLabel(label) && nbrAtLabel(label).isExpanded())
    {
      return true;
    }
  }

  return false;
}

//this determines whether a particle has a red or black neighbor (RB) that it is aligned with
bool CompressionParticle::hasRBNbrInLine() const
{

  if (_direction == 0)
  {
    if (hasNbrAtLabel(0))
    {
      if (nbrAtLabel(0)._direction == 0 && (nbrAtLabel(0)._state == State::Red || nbrAtLabel(0)._state == State::Black))
      {
        return true;
      }
    }
    if (hasNbrAtLabel(3))
    {
      if (nbrAtLabel(3)._direction == 0 && (nbrAtLabel(3)._state == State::Red || nbrAtLabel(3)._state == State::Black))
      {
        return true;
      }
    }
  }
  if (_direction == 1)
  {
    if (hasNbrAtLabel(1))
    {
      if (nbrAtLabel(1)._direction == 1 && (nbrAtLabel(1)._state == State::Red || nbrAtLabel(1)._state == State::Black))
      {
        return true;
      }
    }
    if (hasNbrAtLabel(4))
    {
      if (nbrAtLabel(4)._direction == 1 && (nbrAtLabel(4)._state == State::Red || nbrAtLabel(4)._state == State::Black))
      {
        return true;
      }
    }
  }
  if (_direction == 2)
  {
    if (hasNbrAtLabel(2))
    {
      if (nbrAtLabel(2)._direction == 2 && (nbrAtLabel(2)._state == State::Red || nbrAtLabel(2)._state == State::Black))
      {
        return true;
      }
    }
    if (hasNbrAtLabel(5))
    {
      if (nbrAtLabel(5)._direction == 2 && (nbrAtLabel(5)._state == State::Red || nbrAtLabel(5)._state == State::Black))
      {
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

bool CompressionParticle::stuckInLine() const
{

  if (_direction == 0)
  {
    if (hasNbrAtLabel(0) && hasNbrAtLabel(3))
    {
      if (nbrAtLabel(0)._direction == 0 && nbrAtLabel(3)._direction == 0)
      {
        return true;
      }
    }
  }

  if (_direction == 1)
  {
    if (hasNbrAtLabel(1) && hasNbrAtLabel(4))
    {
      if (nbrAtLabel(1)._direction == 1 && nbrAtLabel(4)._direction == 1)
      {
        return true;
      }
    }
  }

  if (_direction == 2)
  {
    if (hasNbrAtLabel(2) && hasNbrAtLabel(5))
    {
      if (nbrAtLabel(2)._direction == 2 && nbrAtLabel(5)._direction == 2)
      {
        return true;
      }
    }
  }

  return false;
}

bool CompressionParticle::stuckInRedLine() const
{

  if (_direction == 0)
  {
    if (hasNbrAtLabel(0) && hasNbrAtLabel(3))
    {
      if (nbrAtLabel(0)._direction == 0 && nbrAtLabel(3)._direction == 0 && (nbrAtLabel(0)._state == State::Red || nbrAtLabel(0)._state == State::Black) && (nbrAtLabel(3)._state == State::Red || nbrAtLabel(3)._state == State::Black))
      {
        return true;
      }
    }
  }

  if (_direction == 1)
  {
    if (hasNbrAtLabel(1) && hasNbrAtLabel(4))
    {
      if (nbrAtLabel(1)._direction == 1 && nbrAtLabel(4)._direction == 1 && (nbrAtLabel(1)._state == State::Red || nbrAtLabel(1)._state == State::Black) && (nbrAtLabel(4)._state == State::Red || nbrAtLabel(4)._state == State::Black))
      {
        return true;
      }
    }
  }

  if (_direction == 2)
  {
    if (hasNbrAtLabel(2) && hasNbrAtLabel(5))
    {
      if (nbrAtLabel(2)._direction == 2 && nbrAtLabel(5)._direction == 2 && (nbrAtLabel(2)._state == State::Red || nbrAtLabel(2)._state == State::Black) && (nbrAtLabel(5)._state == State::Red || nbrAtLabel(5)._state == State::Black))
      {
        return true;
      }
    }
  }

  return false;
}

bool CompressionParticle::hasExpHeadAtLabel(const int label) const
{
  return hasNbrAtLabel(label) && nbrAtLabel(label).isExpanded() && nbrAtLabel(label).pointsAtMyHead(*this, label);
}

int CompressionParticle::nbrCount(std::vector<int> labels) const
{
  int numNbrs = 0;
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
    {
      ++numNbrs;
    }
  }

  return numNbrs;
}

int CompressionParticle::redNbrCount(std::vector<int> labels) const
{
  int numRedNbrs = 0;
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red)
    {
      ++numRedNbrs;
    }
  }

  return numRedNbrs;
}

int CompressionParticle::redNbrCountSameDir(std::vector<int> labels) const
{
  int numRedNbrsSameDir = 0;
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._direction == _direction)
    {
      ++numRedNbrsSameDir;
    }
  }

  return numRedNbrsSameDir;
}

int CompressionParticle::nbrCountSameDir(std::vector<int> labels) const
{
  int numNbrsSameDir = 0;
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._direction == _direction)
    {
      ++numNbrsSameDir;
    }
  }

  return numNbrsSameDir;
}

int CompressionParticle::blueNbrCount(std::vector<int> labels) const
{
  int numBlueNbrs = 0;
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue)
    {
      ++numBlueNbrs;
    }
  }

  return numBlueNbrs;
}

bool CompressionParticle::checkProp1(std::vector<int> S) const
{ //Check Property 1 as it applies to all particles (regardless of color)
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  //if (_state == State::Red) { //MichaelM only Red particles follow compression algorithm,
  //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
  //affecting rate of diffusivity
  if (S.size() == 0)
  {
    return false; // S has to be nonempty for Property 1.
  }
  else
  {
    const std::vector<int> labels = uniqueLabels();
    std::set<int> adjNbrs;

    // Starting from the particles in S, sweep out and mark connected neighbors.
    for (int s : S)
    {
      adjNbrs.insert(s);
      int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

      // First sweep counter-clockwise, stopping when an unoccupied position or
      // expanded head is encountered.
      for (uint offset = 1; offset < labels.size(); ++offset)
      {
        int label = labels[(i + offset) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
        { //Left out "&& nbrAtLabel(label)._direction == _direction"
          adjNbrs.insert(label);
        }
        else
        {
          break;
        }
      }

      // Then sweep clockwise.
      for (uint offset = 1; offset < labels.size(); ++offset)
      {
        int label = labels[(i - offset + labels.size()) % labels.size()];
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
        { //Left out "&& nbrAtLabel(label)._direction == _direction"
          adjNbrs.insert(label);
        }
        else
        {
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

bool CompressionParticle::checkProp2(std::vector<int> S) const
{ //Check Property 2 as it applies to all particles (regardless of color)
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  //if (_state == State::Red) {   //MichaelM only Red particles follow compression algorithm,
  //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
  //affecting rate of diffusivity
  if (S.size() != 0)
  {               //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster                           // S has to be empty for Property 2.
    return false; //MichaelM changed "return false" to "return true"
  }
  else
  {
    //    const int numRedHeadNbrsSameDir = redNbrCountSameDir(headLabels());
    //    const int numRedTailNbrsSameDir = redNbrCountSameDir(tailLabels());
    const int numHeadNbrs = nbrCount(headLabels());
    const int numTailNbrs = nbrCount(tailLabels());

    // Check if the head's neighbors are connected.
    int numAdjHeadNbrs = 0;
    bool seenNbr = false;
    for (const int label : headLabels())
    {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
      { //Left out "&& nbrAtLabel(label)._direction == _direction"
        seenNbr = true;
        ++numAdjHeadNbrs;
      }
      else if (seenNbr)
      {
        break;
      }
    }

    // Check if the tail's neighbors are connected.
    int numAdjTailNbrs = 0;
    seenNbr = false;
    for (const int label : tailLabels())
    {
      if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label))
      {
        seenNbr = true;
        ++numAdjTailNbrs;
      }
      else if (seenNbr)
      {
        break;
      }
    }

    // Property 2 is satisfied if both the head and tail have at least one
    // neighbor and all head (tail) neighbors are connected.
    return (numHeadNbrs > 0) && (numTailNbrs > 0) &&
           (numHeadNbrs == numAdjHeadNbrs) && (numTailNbrs == numAdjTailNbrs);
  }
}

bool CompressionParticle::checkRedProp1(std::vector<int> S) const
{ //Check Property 1 as it applies to only red particles
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  if (_state == State::Red)
  { //MichaelM only Red particles follow compression algorithm,
    //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
    //affecting rate of diffusivity
    if (S.size() == 0)
    {
      return false; // S has to be nonempty for Property 1.
    }
    else
    {
      const std::vector<int> labels = uniqueLabels();
      std::set<int> redAdjNbrs;

      // Starting from the particles in S, sweep out and mark connected neighbors.
      for (int s : S)
      {
        redAdjNbrs.insert(s);
        int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

        // First sweep counter-clockwise, stopping when an unoccupied position or
        // expanded head is encountered.
        for (uint offset = 1; offset < labels.size(); ++offset)
        {
          int label = labels[(i + offset) % labels.size()];
          if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red)
          { //Left out "&& nbrAtLabel(label)._direction == _direction"
            redAdjNbrs.insert(label);
          }
          else
          {
            break;
          }
        }

        // Then sweep clockwise.
        for (uint offset = 1; offset < labels.size(); ++offset)
        {
          int label = labels[(i - offset + labels.size()) % labels.size()];
          if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red)
          { //Left out "&& nbrAtLabel(label)._direction == _direction"
            redAdjNbrs.insert(label);
          }
          else
          {
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

bool CompressionParticle::checkBlueProp1(std::vector<int> S) const
{ //Check Property 1 as it applies to only blue particles
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  if (_state == State::Blue && q < 0.1)
  { //MichaelM only Red particles follow compression algorithm,
    //since q is randomly gen. between 0, 1 then prop1 rate of satisfaction is variable
    //affecting rate of diffusivity
    if (S.size() == 0)
    {
      return false; // S has to be nonempty for Property 1.
    }
    else
    {
      const std::vector<int> labels = uniqueLabels();
      std::set<int> blueAdjNbrs;

      // Starting from the particles in S, sweep out and mark connected neighbors.
      for (int s : S)
      {
        blueAdjNbrs.insert(s);
        int i = distance(labels.begin(), find(labels.begin(), labels.end(), s));

        // First sweep counter-clockwise, stopping when an unoccupied position or
        // expanded head is encountered.
        for (uint offset = 1; offset < labels.size(); ++offset)
        {
          int label = labels[(i + offset) % labels.size()];
          if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue)
          {
            blueAdjNbrs.insert(label);
          }
          else
          {
            break;
          }
        }

        // Then sweep clockwise.
        for (uint offset = 1; offset < labels.size(); ++offset)
        {
          int label = labels[(i - offset + labels.size()) % labels.size()];
          if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue)
          {
            blueAdjNbrs.insert(label);
          }
          else
          {
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

bool CompressionParticle::checkRedProp2(std::vector<int> S) const
{ //Check Property 2 as it applies only to red particles
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  if (_state == State::Red)
  { //MichaelM only Red particles follow compression algorithm,
    //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
    //affecting rate of diffusivity
    if (S.size() != 0)
    {               //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster                           // S has to be empty for Property 2.
      return false; //MichaelM changed "return false" to "return true"
    }
    else
    {
      //    const int numRedHeadNbrsSameDir = redNbrCountSameDir(headLabels());
      //    const int numRedTailNbrsSameDir = redNbrCountSameDir(tailLabels());
      const int numRedHeadNbrs = redNbrCount(headLabels());
      const int numRedTailNbrs = redNbrCount(tailLabels());

      // Check if the head's neighbors are connected.
      int numRedAdjHeadNbrs = 0;
      bool seenNbr = false;
      for (const int label : headLabels())
      {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red)
        { //Left out "&& nbrAtLabel(label)._direction == _direction"
          seenNbr = true;
          ++numRedAdjHeadNbrs;
        }
        else if (seenNbr)
        {
          break;
        }
      }

      // Check if the tail's neighbors are connected.
      int numRedAdjTailNbrs = 0;
      seenNbr = false;
      for (const int label : tailLabels())
      {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Red)
        {
          seenNbr = true;
          ++numRedAdjTailNbrs;
        }
        else if (seenNbr)
        {
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

bool CompressionParticle::checkBlueProp2(std::vector<int> S) const
{ //Check Property 2 as it applies to only blue particles
  Q_ASSERT(isExpanded());
  Q_ASSERT(S.size() <= 2);
  Q_ASSERT(flag); // Not required, but equivalent/cleaner for implementation.

  if (_state == State::Blue && q < 0.1)
  { //MichaelM only Red particles follow compression algorithm,
    //since q is randomly gen. between 0, 1 then prop2 rate of satisfaction is variable
    //affecting rate of diffusivity
    if (S.size() == 0)
    {              //also changed (S.size() != 0) to (S.size() == 0) (somewhat disables property 2 and allows particles to breakaway from cluster
                   // S has to be empty for Property 2.
      return true; //MichaelM changed "return false" to "return true"
    }
    else
    {
      const int numBlueHeadNbrs = blueNbrCount(headLabels());
      const int numBlueTailNbrs = blueNbrCount(tailLabels());

      // Check if the head's neighbors are connected.
      int numBlueAdjHeadNbrs = 0;
      bool seenNbr = false;
      for (const int label : headLabels())
      {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue)
        {
          seenNbr = true;
          ++numBlueAdjHeadNbrs;
        }
        else if (seenNbr)
        {
          break;
        }
      }

      // Check if the tail's neighbors are connected.
      int numBlueAdjTailNbrs = 0;
      seenNbr = false;
      for (const int label : tailLabels())
      {
        if (hasNbrAtLabel(label) && !hasExpHeadAtLabel(label) && nbrAtLabel(label)._state == State::Blue)
        {
          seenNbr = true;
          ++numBlueAdjTailNbrs;
        }
        else if (seenNbr)
        {
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

CompressionSystem::CompressionSystem(unsigned int numRedParticles, unsigned int numBlueParticles,
 unsigned int numGreenParticles, double lambda, double diffusionRate, 
 double bindingAffinity, double seperationAffinity, double convertToStable,
 double detachFromLine)
{
  this->diffusionRate = diffusionRate;
  this->bindingAffinity = bindingAffinity;
  this->seperationAffinity = seperationAffinity;
  this->convertToStable = convertToStable;
  this->detachFromLine = detachFromLine;

  /*
  std::cout << "lambda " << lambda << std::endl;
  std::cout << "diffusionRate " << diffusionRate<< std::endl;
  std::cout << "bindingAffinity " << bindingAffinity << std::endl;
  std::cout << "seperationAffinity " << seperationAffinity  << std::endl;
  std::cout << "convertToStable " << convertToStable << std::endl;
  std::cout << "detachFromLine " << detachFromLine<< std::endl;
  std::cout << std::endl;
  */

  Q_ASSERT(lambda > 1);
  //std::cout << "lambda " <<lambda <<std::endl;
  //  int numParticles = numBlueParticles + numRedParticles;
  //MichaelM added hexagon creation and random particle insertion similar to DiscoDemo but for CompressionParticles

  // sideLen is now a var of the system
  //int sideLen = static_cast<int>(std::round(50));
  this->sideLen = static_cast<int>(std::round(50)); //MichaelM changed 1.4 to 3.0 (control hexagon size)
  Node boundNode(0, 0);                             //perhaps make this a variable? to easily modify?
  for (int dir = 0; dir < 6; ++dir)
  {
    for (int i = 0; i < sideLen; ++i)
    {
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
  while (numRedAdded < numRedParticles)
  {
    // First, choose an x and y position at random from the (i) and (ii) bounds.
    //std::cout << "lambda " <<lambda <<std::endl;
    int x = randInt(-sideLen + 1, sideLen);
    int y = randInt(1, 2 * sideLen);
    Node node(x, y);

    // If the node satisfies (iii) and is unoccupied, place a particle there.
    if (0 < x + y && x + y < 2 * sideLen && occupied.find(node) == occupied.end())
    {
      insert(new CompressionParticle(node, -1, 0, *this, lambda, CompressionParticle::State::Red));
      // this->nodesOccupied++;
      occupied.insert(node);
      numRedAdded++;
    }
  }

  unsigned int numBlueAdded = 0;
  if (numBlueParticles > 0)
  {
    while (numBlueAdded < numBlueParticles)
    {
      // First, choose an x and y position at random from the (i) and (ii) bounds.
      int x = randInt(-sideLen + 1, sideLen);
      int y = randInt(1, 2 * sideLen);
      Node blueNode(x, y);

      // If the node satisfies (iii) and is unoccupied, place a particle there.
      if (0 < x + y && x + y < 2 * sideLen && occupied.find(blueNode) == occupied.end())
      {
        insert(new CompressionParticle(blueNode, -1, 0, *this, lambda, CompressionParticle::State::Blue));
        // this->nodesOccupied++;
        occupied.insert(blueNode);
        numBlueAdded++;
      }
    }
  }

  unsigned int numGreenAdded = 0;
  if (numGreenParticles > 0)
  {
    while (numGreenAdded < numGreenParticles)
    {
      // First, choose an x and y position at random from the (i) and (ii) bounds.
      int x = randInt(-sideLen + 1, sideLen);
      int y = randInt(1, 2 * sideLen);
      Node greenNode(x, y);

      // If the node satisfies (iii) and is unoccupied, place a particle there.
      if (0 < x + y && x + y < 2 * sideLen && occupied.find(greenNode) == occupied.end())
      {
        insert(new CompressionParticle(greenNode, -1, 0, *this, lambda, CompressionParticle::State::Green));
        // this->nodesOccupied++;
        occupied.insert(greenNode);
        numGreenAdded++;
      }
    }
  }
  _measures.push_back(new PerimeterMeasure("Perimeter", 1, *this));
  _measures.push_back(new SurfaceArea("% SC Nodes/Nodes", 1, *this));
  _measures.push_back(new SurfaceAreaNumeratorParticles("% SC Particles/Nodes", 1, *this));
  _measures.push_back(new PercentOrdering("% Ordering", 1, *this));
  _measures.push_back(new AvgHeight("Avg Height", 1, *this));
  _measures.push_back(new AvgWidth("Avg Width", 1, *this));
  _measures.push_back(new MaxHeight("Max Height", 1, *this));
  _measures.push_back(new MaxWidth("Max Width", 1, *this));
  //_counts.push_back(new Count("Surface Coverage"));

  //totalNodes = (3*sqrt(3) * pow(50, 2))/2;  // Hexagon area = (3*√3 *(sideLen)^2)/ 2
}

void CompressionSystem::DFSCluster(CompressionParticle &p, std::vector<CompressionParticle> &cluster,
                                   std::vector<std::vector<CompressionParticle>> &heights, std::vector<std::vector<CompressionParticle>> &widths)
{
  // have height cluster and width cluster? no new method for width.!!!!!!!!!!!!!!!
  p.countedCluster = true;
  cluster.push_back(p);

  for (int j = 0; j < 6; j++)
  {
    if (p.hasNbrAtLabel(j) && !p.nbrAtLabel(j).countedCluster && (p._direction == p.nbrAtLabel(j)._direction) && (p.nbrAtLabel(j)._state == CompressionParticle::State::Black))
    {
      DFSCluster(p.nbrAtLabel(j), cluster, heights, widths);
    }

    if (!p.countedHeight && p._state == CompressionParticle::State::Black)
    {
      std::vector<CompressionParticle> oneHeight = {};
      DFSHeight(p, oneHeight);
      heights.push_back(oneHeight);
    }

    if (!p.countedWidth && p._state == CompressionParticle::State::Black)
    {
      std::vector<CompressionParticle> oneWidth = {};
      DFSWidth(p, oneWidth, -2);
      widths.push_back(oneWidth);
    }
  }
}

void CompressionSystem::DFSWidth(CompressionParticle &p, std::vector<CompressionParticle> &cluster, int dir)
{
  // have height cluster and width cluster? no new method for width.!!!!!!!!!!!!!!!
  p.countedWidth = true;
  cluster.push_back(p);
  if (dir != -2)
  {
    //std::cout << "Dir: " << dir << std::endl;
    //std::cout << "Direction: " << p._direction << std::endl;
  }
  if (dir == -2)
  {
    bool found = false;
    for (int j = 0; j < 6; j++)
    {
      if (!found && (j != p._direction) && (j != (p._direction + 3)))
      {
        if (p.hasNbrAtLabel(j) && !p.nbrAtLabel(j).countedWidth && (p._direction == p.nbrAtLabel(j)._direction) && (p.nbrAtLabel(j)._state == CompressionParticle::State::Black))
        {
          int dirToPass;
          if (j >= 3)
          {
            dirToPass = j - 3;
          }
          else
          {
            dirToPass = j;
          }
          found = true;
          DFSWidth(p.nbrAtLabel(j), cluster, dirToPass);
          break;
        }
      }
    }
  }
  else
  {
    //std::cout << "dir one " << dir << std::endl;
    // this is a given
    if (dir < 3 && dir >= 0)
    {
      //std::cout << "dir two " << dir << std::endl;
      for (int j = dir; j < 6; j = j + 3)
      {
        if (p.hasNbrAtLabel(j) && !p.nbrAtLabel(j).countedWidth && (p._direction == p.nbrAtLabel(j)._direction) && (p.nbrAtLabel(j)._state == CompressionParticle::State::Black))
        {
          //std::cout << "dir three" << dir << std::endl;
          DFSWidth(p.nbrAtLabel(j), cluster, dir);
          //std::cout << "dir four" << dir << std::endl;
        }
      }
    }
  }
}

void CompressionSystem::DFSHeight(CompressionParticle &p, std::vector<CompressionParticle> &cluster)
{
  // have height cluster and width cluster? no new method for width.!!!!!!!!!!!!!!!
  p.countedHeight = true;
  cluster.push_back(p);

  // there are the only direction we need to check to get height
  for (int j = p._direction; j < 6; j = j + 3)
  {
    if (p.hasNbrAtLabel(j) && !p.nbrAtLabel(j).countedHeight && (p._direction == p.nbrAtLabel(j)._direction) && (p.nbrAtLabel(j)._state == CompressionParticle::State::Black))
    {
      DFSHeight(p.nbrAtLabel(j), cluster);
    }
  }
}

std::vector<std::vector<CompressionParticle>> CompressionSystem::getClusters()
//void CompressionSystem::getClusters()
{
  std::vector<std::vector<CompressionParticle>> allHeights;
  std::vector<std::vector<CompressionParticle>> allWidths;
  std::vector<std::vector<CompressionParticle>> allClusters;

  // Reset all visited flags.
  for (auto &p : particles)
  {
    auto disco_p = dynamic_cast<CompressionParticle *>(p);
    disco_p->countedHeight = false;
    disco_p->countedWidth = false;
    disco_p->countedCluster = false;
  }

  // Do DFS.
  for (auto &p : particles)
  {
    auto disco_p = dynamic_cast<CompressionParticle *>(p);
    if (!disco_p->countedHeight && disco_p->_state == CompressionParticle::State::Black)
    {
      std::vector<CompressionParticle> heights = {};
      DFSHeight(*disco_p, heights);
      allHeights.push_back(heights);
    }
    if (!disco_p->countedWidth && disco_p->_state == CompressionParticle::State::Black)
    {
      std::vector<CompressionParticle> width = {};
      DFSWidth(*disco_p, width, -2);
      allWidths.push_back(width);
    }

    // To Find hieght and width for each cluster independently
    /*if (!disco_p->countedCluster && disco_p->_state == CompressionParticle::State::Black)
    {
      std::vector<CompressionParticle> clusters = {};
      std::vector<std::vector<CompressionParticle>> heights = {};
      std::vector<std::vector<CompressionParticle>> widths = {};
      DFSCluster(*disco_p, clusters, heights, widths);
      allClusters.push_back(clusters);
      if (clusters.size() > 1)
      {
        std::cout << "cluster size " << clusters.size() << std::endl;
        std::cout << "heights in this cluster: " << heights.size() << std::endl;
        int sumHeights = 0;
        for (auto vec1 : heights)
        {
          //if (vec1.size() > 1)
          //{
            //std::cout << vec1.size() << " ";
            sumHeights+= vec1.size();
          //}
        }
        double avgHeight = ((double)sumHeights)/((double)heights.size());
        std::cout << "avg height for this cluster: " << avgHeight << std::endl;
        //std::cout << std::endl;
        std::cout << "widths in this cluster: " << widths.size() << std::endl;
        int sumWidths = 0;
        for (auto vec1 : widths)
        {
          //if (vec1.size() > 1)
          //{
            //std::cout << vec1.size() << " ";
            sumWidths+= vec1.size();
          //}
        }
        double avgWidth = ((double)sumWidths)/((double)widths.size());
        std::cout << "avg width for this cluster: " << avgWidth << std::endl;
        std::cout << std::endl;
      }
    }*/
  }
  int sumAllHeights = 0;
  int sumAllWidths = 0;
  this->maxHeight = 0;
  this->maxWidth = 0;
  //std::cout << "heights: " ;
  for (auto h : allHeights)
  {
    // std::cout << h.size() << " ";
    sumAllHeights += h.size();
    if(h.size() > this->maxHeight) {
     
      this->maxHeight = h.size();
    }
  }
  //std::cout << std::endl << "widths: ";
  for (auto w : allWidths)
  {
    //std::cout << w.size() << " ";
    sumAllWidths += w.size();
    
     if(w.size() > this->maxWidth) {
      this->maxWidth = w.size();
    }
  }
   //std::cout << std::endl;
  this->averageHeight = ((double)sumAllHeights) / ((double)allHeights.size());
  this->averageWidth = ((double)sumAllWidths) / ((double)allWidths.size());
  //std::cout << averageHeight << "  " << averageWidth << std::endl;
  /*  std::map<std::vector<int>, std::vector<int>> heightsWidthsByCluster;
  // clustervec
  for (auto cv : allClusters)
  {
    std::vector<int> widthsForCluster;
    std::vector<int> heightsForCluster;
    // for particle in cluster
    for (auto &p : cv)
    {
      auto disco_p = dynamic_cast<CompressionParticle *>(p);
      //vector = wv
      for (auto wv : allWidths)
      {
        if (std::find(wv.begin(), wv.end(), p) != wv.end())
        {
          // wv contains p
          widthsForCluster.insert(wv.size());
          allWidths.erase(wv);
        }
      }
      for (auto hv : allHeights)
      {
        if (std::find(hv.begin(), hv.end(), p) != hv.end())
        int size = static_cast<int>(hv.size());
        heightsForCluster.insert(size);
        allHeights.erase(hv);
      }
    }

    //avg height of cluster
    int heightSumForCluster = 0;
    for(auto hei: heightsForCluster) {
      heightSumForCluster += hei;
    }
    int avgHeight = heightSumForCluster/heightsForCluster.size();

    //avg width of cluster
    int widthSumForCluster = 0;
    for(auto wid: widthsForCluster) {
      widthSumForCluster += wid;
    }
    int avgWidth = widthSumForCluster/widthsForCluster.size();
    
    heightsWidthsByCluster.insert(avgHeight, avgWidth);
    
  }*/
  /*std::cout << "all heights:" << std::endl;
  for (auto vec1 : allHeights)
  {
    if (vec1.size() > 1)
    {
      std::cout << vec1.size() << " ";
    }
  }
  std::cout << std::endl;
  std::cout << "all widths:" << std::endl;
  for (auto vec1 : allWidths)
  {
    if (vec1.size() > 1)
    {
      std::cout << vec1.size() << " ";
    }
  }
  std::cout << std::endl;
  std::cout << "all clusters:" << std::endl;
  for (auto vec1 : allClusters)
  {
    if (vec1.size() > 1)
    {
      std::cout << vec1.size() << " ";
    }
  }
  std::cout << std::endl;
  */
  return allWidths;
}

// map? we want to find average for each group?
// heights first then widths avgs

//sample_map.insert(pair<int, string>(1, "one"));
//sample_map.insert(pair<int, string>(2, "two"));

/*
void DFS(Graph const &graph, int v, vector<bool> &discovered)
{
    // mark current node as discovered
    discovered[v] = true;
 
    // print current node
    cout << v << " ";
 
    // do for every edge (v -> u)
    for (int u : graph.adjList[v])
    {
        // u is not discovered
        if (!discovered[u])
            DFS(graph, u, discovered);
    }
}
*/

// right now this just gets the
/*void CompressionSystem::allGroups()
{
  std::vector<int> avgHeights;
  for (const auto &p : this->particles)
  {
    auto particle = dynamic_cast<CompressionParticle *>(p);
    //for(AmoebotParticle particleTemp: this->particles) {
    //CompressionParticle* particle dynamic_cast<CompressionParticle *> (particleTemp);
    if (!particle->counted && particle->_state == CompressionParticle::State::Black)
    {
      int height = this->findGroup(particle);
      avgHeights.insert(avgHeights.end(), height);
    }
  }
}
*/
// right now this just gets average height of the group particle is in
/*int CompressionSystem::findGroup(CompressionParticle* particle)
{
  // have groupNum field so we can associate hieght with width?
  // have dir be a var locally so no repeat
  // Set of particles yet to traverse for that group: no duplicates
  std::set<CompressionParticle*> inGroup;
  // set of heights for that group
  std::vector<int> heights;
  // set of widths for that group
  std::vector<int> widths;

  // only want to go through this group if particle is not already counted
  // because if this particle is counted then whole group should already be counted
  if (!particle->counted)

  {
    particle->counted = true;

    // count is for height righ now
    int count = 0;


    // if in a certain direction, only want to count particles in that direction
    // DO THIS FOR ALL DIRECTIONS AFTER CONFIRMING IT IS OK
    // for dir 1, nbr 1 and 4
    // for dir 2, nbr 2 and 5
    if (particle->_direction == 0)
    {
      // line not needed probably
      inGroup.insert(particle);
      // while there is a neighbor at 0, keep going to that side and count
      // it and add it to the set
      while (particle->hasNbrAtLabel(0))
      {
        // check if same direction, black color, and not counted
      
        if ((particle->nbrAtLabel(0)._direction == 0)
         && particle->nbrAtLabel(0)._state == CompressionParticle::State::Black 
         && particle->nbrAtLabel(0).counted == false)
        {
          count++;

          // Can't set particle to be equal to another particle???
          particle = dynamic_cast<CompressionParticle *>(particle->nbrAtLabel(0));
          particle->counted = true;
          inGroup.insert(particle);
        }
      }
    
    
      // while there is a neighbor at 3, keep going to that side and count
      // it and add it to the set
      while (particle.hasNbrAtLabel(3))
      {
        // check if same direction, black color, and not counted
        if (particle.nbrAtLabel(3)._direction == 0 && particle.nbrAtLabel(0)._state == CompressionParticle::State::Black && (particle.nbrAtLabel(3).counted == false){
          count++;
          particle = particle.nbrAtLabel(3);
          particle.counted = true;
          inGroup.add(particle);
        }
      }
      // traveled across the height so add this to hieghts list for averaging
      height.add(count);
      // go through all of the particles we went though
      for (CompressionParticle cp : inGroup)
      {
        // and check thier nieghbors
        for (CompressionParticle neighbors : cp.headLabels())
        {
          // check if same direction, black color, and not counted
          if (particle._direction == 0 && particle.nbrAtLabel(0)._state == CompressionParticle::State::Black && !particle.counted)
          {
            count = 0;
            particle = neighbor;
            // then travel across height again and add to heights list
            while (particle.hasNbrAtLabel(0))
            {
              if ((particle.nbrAtLabel(0)._direction == 0) && particle.nbrAtLabel(0)._state == CompressionParticle::State::Black && (particle.nbrAtLabel(0).counted == false))
              {
                count++;
                particle = particle.nbrAtLabel(0);
                particle.counted = true;
                // keep adding particles that it goes to to inGroup, so
                // we check all of the neighbors of the particles we are traversing
                inGroup.add(particle);
              }
            }
            while (particle.hasNbrAtLabel(3))
            {
              if (particle.nbrAtLabel(3)._direction == 0 && particle.nbrAtLabel(0)._state == CompressionParticle::State::Black && (particle.nbrAtLabel(3).counted == false)
              {
                count++;
                particle = particle.nbrAtLabel(3);
                particle.counted = true;
                inGroup.add(particle);
              }
            }
            height.add(count);
          }
        }
      }
    }
  }
  int sum = 0;
  for (int i = 0; i < heights.size(); i++)
  {
    sum += heights.get(i);
  }
  return sum / heights.size();

  return 0;
}
*/
/*if (_direction == 1)
      {
        while (hasNbrAtLabel(1))
        {
        if ((nbrAtLabel(1)._direction == 1) && (nbrAtLabel(1).counted == false){
            count++;
            particle = nbrAtLabel(1);
            particle.counted = true;
            
        }
        }
        while (hasNbrAtLabel(4))
        {
        if (nbrAtLabel(4)._direction == 1 && (nbrAtLabel(0).counted == false){
            count++;
            particle = nbrAtLabel(4);
            particle.counted = true;
        }
        }
      }

      if (_direction == 2)
      {
        while (hasNbrAtLabel(2))
        {
        if ((nbrAtLabel(2)._direction == 2) && (nbrAtLabel(2).counted == false){
            count++;
            particle = nbrAtLabel(2);
            particle.counted = true;
            
        }
        }
        while (hasNbrAtLabel(5))
        {
        if (nbrAtLabel(5)._direction == 2 && (nbrAtLabel(5).counted == false){
            count++;
            particle = nbrAtLabel(5);
            particle.counted = true;
        }
        }
      }*/

bool CompressionSystem::hasTerminated() const
{
#ifdef QT_DEBUG
  if (!isConnected(particles))
  {
    return true;
  }
#endif

  return false;
}

PerimeterMeasure::PerimeterMeasure(const QString name, const unsigned int freq,
                                   CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double PerimeterMeasure::calculate() const
{
  int numEdges = 0;
  for (const auto &p : _system.particles)
  {
    auto comp_p = dynamic_cast<CompressionParticle *>(p);
    auto tailLabels = comp_p->isContracted() ? comp_p->uniqueLabels()
                                             : comp_p->tailLabels();
    for (const int label : tailLabels)
    {
      if (comp_p->hasNbrAtLabel(label) && !comp_p->hasExpHeadAtLabel(label))
      {
        ++numEdges;
      }
    }
  }

  return (3 * _system.size()) - (numEdges / 2) - 3;
}

SurfaceArea::SurfaceArea(const QString name, const unsigned int freq,
                         CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double SurfaceArea::calculate() const
{
  int nodesOccupied = 0;

  // Loop through all particles of the system.
  for (const auto &p : _system.particles)
  {
    // Convert the pointer to a MetricsDemoParticle so its color can be checked.
    auto metr_p = dynamic_cast<CompressionParticle *>(p);
    if (metr_p->isExpanded())
    {
      nodesOccupied += 2;
    }
    else
    {
      nodesOccupied++;
    }
  }

  return ((double)(nodesOccupied)*100.0) / (3.0005 * pow(_system.sideLen, 2) - 3.0246 * _system.sideLen + 1.0154);
}

SurfaceAreaNumeratorParticles::SurfaceAreaNumeratorParticles(const QString name, const unsigned int freq,
                                                             CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double SurfaceAreaNumeratorParticles::calculate() const
{
  int particles = 0;

  // Loop through all particles of the system.
  for (const auto &p : _system.particles)
  {
    // Convert the pointer to a MetricsDemoParticle so its color can be checked.
    auto metr_p = dynamic_cast<CompressionParticle *>(p);
    //if (metr_p->isExpanded()) {
    //  nodesOccupied +=2;
    //}
    //else
    //{
    particles++;
    //}
  }

  return ((double)(particles)*100.0) / (3.0005 * pow(_system.sideLen, 2) - 3.0246 * _system.sideLen + 1.0154);
}

PercentOrdering::PercentOrdering(const QString name, const unsigned int freq,
                                 CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double PercentOrdering::calculate() const
{
  int numBlack = 0;

  // Loop through all particles of the system.
  for (const auto &p : _system.particles)
  {
    // Convert the pointer to a MetricsDemoParticle so its color can be checked.
    auto metr_p = dynamic_cast<CompressionParticle *>(p);
    if (metr_p->_state == CompressionParticle::State::Black)
    {
      numBlack++;
    }
  }

  return ((double)(numBlack)*100.0) / static_cast<double>(_system.size());
}

AvgHeight::AvgHeight(const QString name,
                     const unsigned int freq,
                     CompressionSystem &system)
    : Measure(name, freq),
      _system(system)
{
  counter = 0;
}

double AvgHeight::calculate() const
{
  //counter++;
  //if (counter % 5 == 0)
  //{
    std::vector<std::vector<CompressionParticle>> clusters = _system.getClusters();
  //}
  return _system.averageHeight;
}
AvgWidth::AvgWidth(const QString name,
                   const unsigned int freq,
                   CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double AvgWidth::calculate() const
{
  //counter++;
  //if (counter % 5 == 0)
  //{
  std::vector<std::vector<CompressionParticle>> clusters = _system.getClusters();
  //}
  return _system.averageWidth;
}
MaxWidth::MaxWidth(const QString name,
                   const unsigned int freq,
                   CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double MaxWidth::calculate() const
{
  std::vector<std::vector<CompressionParticle>> clusters = _system.getClusters();
  //std::cout << "NEW ROUND " << std::endl;
  //std::cout << "maxW: " << _system.maxWidth << std::endl;
  return _system.maxWidth;
}

MaxHeight::MaxHeight(const QString name,
                   const unsigned int freq,
                   CompressionSystem &system)
    : Measure(name, freq),
      _system(system) {}

double MaxHeight::calculate() const
{
  std::vector<std::vector<CompressionParticle>> clusters = _system.getClusters();
  //std::cout << "maxH: " << _system.maxHeight << std::endl;
  return _system.maxHeight;
}