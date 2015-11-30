#include "MantidWorkflowAlgorithms/IndirectQuickRun.h"

namespace Mantid {
namespace Algorithms {

using Mantid::Kernel::Direction;
using Mantid::API::WorkspaceProperty;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(IndirectQuickRun)

//----------------------------------------------------------------------------------------------
/** Constructor
 */
IndirectQuickRun::IndirectQuickRun() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
IndirectQuickRun::~IndirectQuickRun() {}

//----------------------------------------------------------------------------------------------

/// Algorithms name for identification. @see Algorithm::name
const std::string IndirectQuickRun::name() const { return "IndirectQuickRun"; }

/// Algorithm's version for identification. @see Algorithm::version
int IndirectQuickRun::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string IndirectQuickRun::category() const {
  return "Workflow\\MIDAS";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string IndirectQuickRun::summary() const {
  return "Reduces input data if required then runs Elwin and optionally I(Q, t)";
}

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void IndirectQuickRun::init() {
  declareProperty(
      new WorkspaceProperty<>("InputWorkspace", "", Direction::Input),
      "An input workspace.");
  declareProperty(
      new WorkspaceProperty<>("OutputWorkspace", "", Direction::Output),
      "An output workspace.");
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void IndirectQuickRun::exec() {
  // TODO Auto-generated execute stub
}

} // namespace WorkflowAlgorithms
} // namespace Mantid
