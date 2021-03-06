#include "Data.h"
#include "Scorer.h"
#include "ScorerFactory.h"

#define BOOST_TEST_MODULE MertData
#include <boost/test/unit_test.hpp>

#include <boost/scoped_ptr.hpp>

//very basic test of sharding
BOOST_AUTO_TEST_CASE(shard_basic) {
  boost::scoped_ptr<Scorer> scorer(ScorerFactory::getScorer("BLEU", ""));
  Data data(scorer.get());
  FeatureArray fa1, fa2, fa3, fa4;
  ScoreArray sa1, sa2, sa3, sa4;
  fa1.setIndex("1");
  fa2.setIndex("2");
  fa3.setIndex("3");
  fa4.setIndex("4");
  sa1.setIndex("1");
  sa2.setIndex("2");
  sa3.setIndex("3");
  sa4.setIndex("4");
  data.getFeatureData()->add(fa1);
  data.getFeatureData()->add(fa2);
  data.getFeatureData()->add(fa3);
  data.getFeatureData()->add(fa4);
  data.getScoreData()->add(sa1);
  data.getScoreData()->add(sa2);
  data.getScoreData()->add(sa3);
  data.getScoreData()->add(sa4);

  vector<Data> shards;
  data.createShards(2,0,"",shards);

  BOOST_CHECK_EQUAL(shards.size(),2);
  BOOST_CHECK_EQUAL(shards[1].getFeatureData()->size(),2);
}
