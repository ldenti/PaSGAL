/**
 * @file    main.cpp
 * @author  Chirag Jain <cjain7@gatech.edu>
 */
#include <iostream>


#ifdef VTUNE_SUPPORT
#include <ittnotify.h>
#endif

#include "graphLoad.hpp"
#include "align.hpp"
#include "utils.hpp"
#include "base_types.hpp"
#include "clipp.h"

int main(int argc, char **argv)
{
#ifdef VTUNE_SUPPORT
  __itt_pause();
#endif

  std::string rfile = "", qfile = "", mode = "";

  auto cli = (
      clipp::required("-m") & clipp::value("mode", mode).doc("reference graph format [vg or txt]"),
      clipp::required("-r") & clipp::value("ref", rfile).doc("reference graph file"),
      clipp::required("-q") & clipp::value("query", qfile).doc("query file (fasta/fastq)[.gz]")
      );

  if(!clipp::parse(argc, argv, cli)) 
  {
    clipp::print ( clipp::make_man_page(cli, argv[0]) );
    exit(1);
  }

  // print execution environment based on which MACROs are set
  // for convenience
  psgl::showExecutionEnv();

  std::cout << "INFO, psgl::main, reference file = " << rfile << " (in " << mode  << " format) " << std::endl;
  std::cout << "INFO, psgl::main, query file = " << qfile << std::endl;

  using VertexIdType = int32_t;
  using EdgeIdType = int32_t;
  using ScoreType = int32_t;

  psgl::graphLoader<VertexIdType, EdgeIdType> g;

  if (mode.compare("vg") == 0)
    g.loadFromVG(rfile);
  else if(mode.compare("txt") == 0)
    g.loadFromTxt(rfile);
  else
  {
    std::cerr << "Invalid format " << mode << std::endl;
    exit(1);
  }

  std::vector< psgl::BestScoreInfo<ScoreType, VertexIdType> > bestScoreVector;

  psgl::alignToDAG<ScoreType> (qfile, g.diCharGraph, bestScoreVector, psgl::MODE::LOCAL);  

#ifdef DEBUG
  g.diCharGraph.printDegreeHistogram();
  g.diCharGraph.printHopLengthHistogram();
  g.printGraph();
#endif
}
