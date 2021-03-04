/* Copyright (C) 2020 Joshua J. Daymude, Robert Gmyr, and Kristian Hinnenthal.
 * The full GNU GPLv3 can be found in the LICENSE file, and the full copyright
 * notice can be found at the top of main/main.cpp. */

#include "ui/algorithm.h"

#include "alg/demo/ballroomdemo.h"
#include "alg/demo/discodemo.h"
#include "alg/demo/metricsdemo.h"
#include "alg/demo/tokendemo.h"
#include "alg/compression.h"
#include "alg/infobjcoating.h"
#include "alg/leaderelection.h"
#include "alg/shapeformation.h"

Algorithm::Algorithm(QString name, QString signature)
    : _name(name),
      _signature(signature) {}

QString Algorithm::getName() const {
  return _name;
}

void Algorithm::setName(QString name) {
  _name = name;
}

QString Algorithm::getSignature() const {
  return _signature;
}

void Algorithm::setSignature(QString signature) {
  _signature = signature;
}

QStringList Algorithm::getParameterNames() const {
  QStringList names;
  for (auto parameter : _parameters) {
    names.append(parameter.first);
  }

  return names;
}

QStringList Algorithm::getParameterDefaults() const {
  QStringList defaults;
  for (auto parameter : _parameters) {
    defaults.append(parameter.second);
  }

  return defaults;
}

void Algorithm::addParameter(QString parameter, QString defaultValue) {
  _parameters.push_back(std::make_pair(parameter, defaultValue));
}

DiscoDemoAlg::DiscoDemoAlg() : Algorithm("Demo: Disco", "discodemo") {
    addParameter("# Particles", "30");
    addParameter("Counter Max", "5");
};

void DiscoDemoAlg::instantiate(const int numParticles, const int counterMax) {
    if (numParticles <= 0) {
      emit log("# particles must be > 0", true);
    } else if (counterMax <= 0) {
      emit log("counterMax must be > 0", true);
    } else {
      emit setSystem(std::make_shared<DiscoDemoSystem>(numParticles));
    }
  }

MetricsDemoAlg::MetricsDemoAlg() : Algorithm("Demo: Metrics", "metricsdemo") {
  addParameter("# Particles", "30");
  addParameter("Counter Max", "5");
};

void MetricsDemoAlg::instantiate(const int numParticles, const int counterMax) {
  if (numParticles <= 0) {
    emit log("# particles must be > 0", true);
  } else if (counterMax <= 0) {
    emit log("counterMax must be > 0", true);
  } else {
    emit setSystem(std::make_shared<MetricsDemoSystem>(numParticles));
  }
}

BallroomDemoAlg::BallroomDemoAlg() : Algorithm("Demo: Ballroom", "ballroomdemo") {
  addParameter("# Particles", "30");
}

void BallroomDemoAlg::instantiate(const int numParticles) {
  emit setSystem(std::make_shared<BallroomDemoSystem>(numParticles));
}

TokenDemoAlg::TokenDemoAlg() : Algorithm("Demo: Token Passing", "tokendemo") {
  addParameter("# Particles", "48");
  addParameter("Token Lifetime", "100");
}

void TokenDemoAlg::instantiate(const int numParticles, const int lifetime) {
  if (numParticles <= 6) {
    emit log("# particles must be > 6", true);
  } else if (lifetime <= 0) {
    emit log("token lifetime must be > 0", true);
  } else {
    emit setSystem(std::make_shared<TokenDemoSystem>(numParticles, lifetime));
  }
}

CompressionAlg::CompressionAlg() : Algorithm("Compression", "compression") {
  addParameter("# Red Particles", "15");
  addParameter("# Blue Particles", "15");
  addParameter("# Green Particles", "15");
  addParameter("Lambda", "4.0");
  addParameter("Diffusion Rate", "1.0");
  addParameter("Binding Affinity", "0.6");
  addParameter("Separation Affinity", "0.4");
  addParameter("Convert to Stable", "0.0015");
  addParameter("Detach from Line", "1.2");
}

void CompressionAlg::instantiate(const int numRedParticles, const int numBlueParticles, 
const int numGreenParticles, const double lambda, const double diffusionRate, 
const double bindingAffinity, const double seperationAffinity, const double convertToStable,
const double detachFromLine) {
    if (numRedParticles <= 0) {
      emit log("# red particles must be > 0", true);
    }
    if (numBlueParticles < 0) {
      emit log("# blue particles must be > 0 or = 0", true);
    }
    if (numGreenParticles < 0) {
      emit log("# green particles must be > 0 or = 0", true);
    }
    else if (lambda <= 0) {
      emit log("lambda must be > 0", true);
    }
    else if (diffusionRate <= 0) {
      emit log("diffusionRate must be > 0", true);
    }
    else if (bindingAffinity <= 0) {
      emit log("bindingAffinity must be > 0", true);
    }
    else if (seperationAffinity <= 0) {
      emit log("seperationAffinity must be > 0", true);
    }
    else if (convertToStable <= 0) {
      emit log("convertToStable must be > 0", true);
    }
    else if (detachFromLine <= 0) {
      emit log("Detach From Line must be > 0", true);
    }
    else {
      //emit setSystem(std::make_shared<CompressionSystem>(numRedParticles, numBlueParticles, numGreenParticles));
      emit setSystem(std::make_shared<CompressionSystem>(numRedParticles, numBlueParticles, 
      numGreenParticles, lambda, diffusionRate, bindingAffinity, seperationAffinity, convertToStable, detachFromLine));
    }
  }


InfObjCoatingAlg::InfObjCoatingAlg() :
  Algorithm("Infinite Object Coating", "infobjcoating") {
  addParameter("# Particles", "100");
  addParameter("Hole Prob.", "0.2");
}

void InfObjCoatingAlg::instantiate(const int numParticles,
                                   const double holeProb) {
  if (numParticles <= 0) {
    emit log("# particles must be > 0", true);
  } else if (holeProb < 0 || holeProb > 1) {
    emit log("holeProb in [0,1] required", true);
  } else {
    emit setSystem(std::make_shared<InfObjCoatingSystem>(numParticles,
                                                         holeProb));
  }
}

LeaderElectionAlg::LeaderElectionAlg() :
  Algorithm("Leader Election", "leaderelection") {
  addParameter("# Particles", "100");
  addParameter("Hole Prob.", "0.2");
}

void LeaderElectionAlg::instantiate(const int numParticles,
                                    const double holeProb) {
  if (numParticles <= 0) {
    emit log("# particles must be > 0", true);
  } else if (holeProb < 0 || holeProb > 1) {
    emit log("holeProb in [0,1] required", true);
  } else {
    emit setSystem(std::make_shared<LeaderElectionSystem>(numParticles,
                                                          holeProb));
  }
}

ShapeFormationAlg::ShapeFormationAlg() :
  Algorithm("Basic Shape Formation", "shapeformation") {
  addParameter("# Particles", "200");
  addParameter("Hole Prob.", "0.2");
  addParameter("Shape", "h");
}

void ShapeFormationAlg::instantiate(const int numParticles,
                                    const double holeProb, const QString mode) {
  std::set<QString> set = ShapeFormationSystem::getAcceptedModes();
  if (numParticles <= 0) {
    emit log("# particles must be > 0", true);
  } else if (holeProb < 0 || holeProb > 1) {
    emit log("holeProb in [0,1] required", true);
  } else if (set.find(mode) == set.end()) {
    QString accepted = "";
    for(std::set<QString>::iterator it = set.begin(); it != set.end(); ++it) {
      if (accepted != "") accepted = accepted + ", " + *it;
      else accepted = *it;
    }
    emit log("only accepted modes are: " + accepted, true);
  } else {
    emit setSystem(std::make_shared<ShapeFormationSystem>(numParticles,
                                                          holeProb, mode));
  }
}

AlgorithmList::AlgorithmList() {
  // Demo algorithms.
  _algorithms.push_back(new DiscoDemoAlg());  
  _algorithms.push_back(new MetricsDemoAlg());
  _algorithms.push_back(new BallroomDemoAlg());  
  _algorithms.push_back(new TokenDemoAlg());

  // General algorithms.
  _algorithms.push_back(new CompressionAlg());  
  _algorithms.push_back(new InfObjCoatingAlg());    
  _algorithms.push_back(new LeaderElectionAlg());
  _algorithms.push_back(new ShapeFormationAlg());
}

AlgorithmList::~AlgorithmList() {
  _algorithms.erase(_algorithms.begin(), _algorithms.end());
}

std::vector<Algorithm*> AlgorithmList::getAlgs() {
  return _algorithms;
}

Algorithm* AlgorithmList::getAlg(QString algName) const {
  Algorithm* algo = nullptr;

  for (auto alg : _algorithms) {
    if (alg->getName().compare(algName) == 0) {
      algo = alg;
      break;
    }
  }

  return algo;
}

QStringList AlgorithmList::getAlgNames() const {
  QStringList names;
  for (auto alg : _algorithms) {
    names.append(alg->getName());
  }
  names.sort();

  return names;
}

QString AlgorithmList::getAlgSignature(QString algName) const {
  QString signature;
  for (auto alg : _algorithms) {
    if (alg->getName().compare(algName) == 0) {
      signature = alg->getSignature();
      break;
    }
  }

  return signature;
}

QStringList AlgorithmList::getParameterNames(QString algName) const {
  QStringList names;
  for (auto alg : _algorithms) {
    if (alg->getName().compare(algName) == 0) {
      names = alg->getParameterNames();
      break;
    }
  }

  return names;
}

QStringList AlgorithmList::getParameterDefaults(QString algName) const {
  QStringList defaults;
  for (auto alg : _algorithms) {
    if (alg->getName().compare(algName) == 0) {
      defaults = alg->getParameterDefaults();
      break;
    }
  }

  return defaults;
}
