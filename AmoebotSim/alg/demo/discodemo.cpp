/* Copyright (C) 2020 Joshua J. Daymude, Robert Gmyr, and Kristian Hinnenthal.
 * The full GNU GPLv3 can be found in the LICENSE file, and the full copyright
 * notice can be found at the top of main/main.cpp. */

#include "alg/demo/discodemo.h"

DiscoDemoParticle::DiscoDemoParticle(const Node &head, const int globalTailDir,
                                     const int orientation,
                                     AmoebotSystem &system,
                                     const int counterMax)
    : AmoebotParticle(head, globalTailDir, orientation, system),
      _counter(counterMax),
      visited(false),
      _counterMax(counterMax)
{
  _state = getRandColor();
}

bool DiscoDemoParticle::colorNbrCount(std::vector<int> labels) const
{
  for (const int label : labels)
  {
    if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == _state)
    {
      return true;
    }
  }
  return false;
}

void DiscoDemoParticle::activate()
{
  //   std::vector<std::vector<DiscoDemoParticle>> DiscoDemoSystem::getClusters();

  // First decrement the particle's counter. If it's zero, reset the counter and
  // get a new color.
  _counter--;

  if (_counter == 0)
  {
    _counter = _counterMax;
    //    _state = getRandColor();  MichaelM removed getRandColor
  }

  /*  if (DiscoDemoParticle::head.x % 2 == 1) {
      int expandDir = 3;
  } else {int expandDir = randDir(); } */

  // Next, handle movement. If the particle is contracted, choose a random
  // direction to try to expand towards, but only do so if the node in that
  // direction is unoccupied. Otherwise, if the particle is expanded, simply
  // contract its tail.
  if (isContracted())
  {
    //      if (DiscoDemoSystem::getClusters())

    /*     for (int label : uniqueLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
            nbrAtLabel(label)._state = State::Red;
  }
            break;
        }

      for (int label : headLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
            nbrAtLabel(label)._state = State::Red;
  }
            break;
        }
*/
    //   int expandDir = randDir();

    //bool nbrFlag = false;
    int expandDir = randDir();

    //      int globalizedDir = localToGlobalDir(expandDir);
    /*      if (DiscoDemoParticle::head.x == 1 || 2 || 3) {
          expandDir = 3;
      } else { expandDir = 3;
      }
      localToGlobalDir(expandDir); */
    //if (localToGlobalDir(expandDir) == 3 && canExpand(expandDir)) {
    if (canExpand(expandDir) && !colorNbrCount(uniqueLabels()))
    {
      expand(expandDir);
    }

    //}
    //    if (canExpand(localToGlobalDir(expandDir))) {
    //      expand(localToGlobalDir(expandDir));

    /*   for (int label : uniqueLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
            nbrAtLabel(label)._state = State::Red;
        }
        break;
      }

    for (int label : headLabels()) {
      if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
          nbrAtLabel(label)._state = State::Red;
      }
      break;
    }

    for (int label : tailLabels()) {
      if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
          nbrAtLabel(label)._state = State::Red;

          break;
      }
} */
    //}

    /*  } else {
      for (int label : headLabels()) {
        if (hasNbrAtLabel(label) && nbrAtLabel(label)._state == State::Blue) {
            nbrAtLabel(label)._state = State::Red;
        }
        break;
      }
    for (int label : tailLabels()) {
      if (hasNbrAtLabel(label) && nbrAtLabel(label)._state != State::Blue) {
          nbrAtLabel(label)._state = State::Red;

          break;
      } */
  }
  else
  {
    contractTail();
  }

  //  if (system.getCount("# Activations")._value % 1 == 0) {
  //      system.particleMap.find()
  //  }

  //if (system.getCount("# Activations")._value % 50 == 0) {
  //      int sideLen = static_cast<int>(std::round(2 * std::sqrt(5)));

  //      int x = randInt(-sideLen + 1, sideLen);
  //      int y = randInt(1, 2 * sideLen);
  //      Node node(x, y);

  //      // If the node satisfies (iii) and is unoccupied, place a particle there.
  //      if (0 < x + y && x + y < 2 * sideLen
  //          && system.particleMap.find(node) == system.particleMap.end()) {
  //          //if (DiscoDemoSystem::getClusters() > 1) {
  //        system.insert(new DiscoDemoParticle(node, -1, randDir(), system, _counterMax));
  //}

  //  }
  system.remove(this);
}
//}

int DiscoDemoParticle::headMarkColor() const
{
  switch (_state)
  {
  case State::Red:
    return 0xff0000;
  case State::Blue:
    return 0x0000ff;
    //    case State::Yellow: return 0xffff00;
    //    case State::Green:  return 0x00ff00;
    //    case State::Blue:   return 0x0000ff;
    //    case State::Indigo: return 0x4b0082;
    //    case State::Violet: return 0xbb00ff;      MichaelM removed extra states except Red and Blue
  }

  return -1;
}

int DiscoDemoParticle::tailMarkColor() const
{
  return headMarkColor();
}

QString DiscoDemoParticle::inspectionText() const
{
  QString text;
  text += "Global Info:\n";
  text += "  head: (" + QString::number(head.x) + ", " + QString::number(head.y) + ")\n";
  text += "  orientation: " + QString::number(orientation) + "\n";
  text += "  globalTailDir: " + QString::number(globalTailDir) + "\n\n";
  text += "Local Info:\n";
  text += "  state: ";
  text += [this]() {
    switch (_state)
    {
    case State::Red:
      return "red\n";
    case State::Blue:
      return "blue\n";
/*      case State::Yellow: return "yellow\n";
      case State::Green:  return "green\n";
      case State::Blue:   return "blue\n";
      case State::Indigo: return "indigo\n";
      case State::Violet: return "violet\n";        MichaelM removed extra colors
 */   }
return "no state\n";
  }();
  text += "  counter: " + QString::number(_counter);

  return text;
}

DiscoDemoParticle::State DiscoDemoParticle::getRandColor() const
{
  // Randomly select an integer and return the corresponding state via casting.
  return static_cast<State>(randInt(0, 2)); //MichaelM integers correspond to the existing colors
} //(only red and blue at the moment)

DiscoDemoParticle &DiscoDemoParticle::nbrAtLabel(int label) const
{
  return AmoebotParticle::nbrAtLabel<DiscoDemoParticle>(label);
}

DiscoDemoSystem::DiscoDemoSystem(unsigned int numParticles, int counterMax)
{
  // In order to enclose an area that's roughly 3.7x the # of particles using a
  // regular hexagon, the hexagon should have side length 1.4*sqrt(# particles).
  int sideLen = static_cast<int>(std::round(20 * std::sqrt(5))); //MichaelM changed 1.4 * std::sqrt
  Node boundNode(0, 0);                                          //to 4 * std::sqrt (larger area)
  for (int dir = 0; dir < 6; ++dir)
  { //1.4 is original size
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
  while (occupied.size() < numParticles)
  {
    // First, choose an x and y position at random from the (i) and (ii) bounds.
    int x = randInt(-sideLen + 1, sideLen);
    int y = randInt(1, 2 * sideLen);
    Node node(x, y);

    // If the node satisfies (iii) and is unoccupied, place a particle there.
    if (0 < x + y && x + y < 2 * sideLen && occupied.find(node) == occupied.end())
    {
      insert(new DiscoDemoParticle(node, -1, randDir(), *this, counterMax));
      occupied.insert(node);

      //MichaelM: experimental addition of inserting new particle as # activations increases,
      //but error codes result and particles clone themselves only at the very beginning

      //     if (getCount("# Activations")._value % 10 == 0) {
      //      insert(new DiscoDemoParticle(node, -1, randDir(), *this, counterMax));
      //      occupied.insert(node);
      //      }
    }
  }
  int x = 0;
  if (getClusters().size() > 1)
  {
    x = 3;
  }
  else
  {
    x = 0;
  }

  //  _measures.push_back(new ClusterMeasure("Clusters", 1, *this));
}

void DiscoDemoSystem::DFS(DiscoDemoParticle &p, std::vector<DiscoDemoParticle> cluster)
{
  p.visited = true;
  cluster.push_back(p);

  for (int j = 0; j < 6; j++)
  {
    if (p.hasNbrAtLabel(j) && !p.nbrAtLabel(j).visited)
    {
      DFS(p.nbrAtLabel(j), cluster);
    }
  }
}

std::vector<std::vector<DiscoDemoParticle>> DiscoDemoSystem::getClusters()
{
  std::vector<std::vector<DiscoDemoParticle>> allClusters;

  // Reset all visited flags.
  for (auto &p : particles)
  {
    auto disco_p = dynamic_cast<DiscoDemoParticle *>(p);
    disco_p->visited = false;
  }

  // Do DFS.
  for (auto &p : particles)
  {
    auto disco_p = dynamic_cast<DiscoDemoParticle *>(p);
    if (!disco_p->visited)
    {
      std::vector<DiscoDemoParticle> cluster = {};
      DFS(*disco_p, cluster);
      allClusters.push_back(cluster);
    }
  }
  return allClusters;
}

/* void print(std::vector<DiscoDemoParticle> const &allClusters) {
std::cout << "The vector elements are : ";
for (int i=0; i < allClusters.size(); i++)
    std::cout << allClusters.at(i) << ' ';
 } */

/* double ClusterMeasure::calculate() const {
    std::vector<std::vector<DiscoDemoParticle>> allClusters;

     // Reset all visited flags.
     for (auto& p : particles) {
        auto disco_p = dynamic_cast<DiscoDemoParticle*>(p);
        disco_p->visited = false;
     }

     // Do DFS.
     for (auto& p : particles) {
       auto disco_p = dynamic_cast<DiscoDemoParticle*>(p);
       if (!disco_p->visited) {
         std::vector<DiscoDemoParticle> cluster = {};
         DFS(*disco_p, cluster);
         allClusters.push_back(cluster);
       }
     }
     return allClusters;
} */
