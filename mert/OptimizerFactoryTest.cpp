#include "OptimizerFactory.h"
#include "Optimizer.h"

#define BOOST_TEST_MODULE MertOptimizerFactory
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>

namespace {

inline void CheckBuildOptimizer(unsigned dim,
                                const vector<unsigned>& to_optimize,
                                const vector<parameter_t>& start,
                                const string& type,
                                unsigned int num_random) {
  boost::scoped_ptr<Optimizer> optimizer(
      OptimizerFactory::BuildOptimizer(dim, to_optimize, start, type, num_random));
  BOOST_CHECK(optimizer.get() != NULL);
}

} // namespace

BOOST_AUTO_TEST_CASE(optimizer_type) {
  BOOST_CHECK_EQUAL(OptimizerFactory::GetOptimizerType("powell"),
                    OptimizerFactory::POWELL);
  BOOST_CHECK_EQUAL(OptimizerFactory::GetOptimizerType("random"),
                    OptimizerFactory::RANDOM);
  BOOST_CHECK_EQUAL(OptimizerFactory::GetOptimizerType("random-direction"),
                    OptimizerFactory::RANDOM_DIRECTION);
}

BOOST_AUTO_TEST_CASE(optimizer_build) {
  const unsigned dim = 3;
  std::vector<unsigned> to_optimize;
  to_optimize.push_back(1);
  to_optimize.push_back(2);
  to_optimize.push_back(3);
  std::vector<parameter_t> start;
  start.push_back(0.3);
  start.push_back(0.1);
  start.push_back(0.2);
  const unsigned int num_random = 1;

  CheckBuildOptimizer(dim, to_optimize, start, "powell", num_random);
  CheckBuildOptimizer(dim, to_optimize, start, "random", num_random);
  CheckBuildOptimizer(dim, to_optimize, start, "random-direction", num_random);
}
