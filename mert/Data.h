/*
 *  Data.h
 *  mert - Minimum Error Rate Training
 *
 *  Created by Nicola Bertoldi on 13/05/08.
 *
 */

#ifndef MERT_DATA_H_
#define MERT_DATA_H_

using namespace std;

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Util.h"
#include "FeatureData.h"
#include "ScoreData.h"

class Scorer;

typedef boost::shared_ptr<ScoreData> ScoreDataHandle;
typedef boost::shared_ptr<FeatureData> FeatureDataHandle;

// NOTE: there is no copy constructor implemented, so only the
// compiler synthesised shallow copy is available.
class Data
{
private:
  Scorer* m_scorer;
  std::string m_score_type;
  size_t m_num_scores;
  bool m_sparse_flag;
  ScoreDataHandle m_score_data;
  FeatureDataHandle m_feature_data;

  // Helper functions for loadnbest();
  void InitFeatureMap(const std::string& str);
  void AddFeatures(const std::string& str,
                   const std::string& sentence_index);

public:
  explicit Data(Scorer* scorer);
  Data();

  void clear() {
    m_score_data->clear();
    m_feature_data->clear();
  }

  ScoreDataHandle getScoreData() { return m_score_data; }

  FeatureDataHandle getFeatureData() { return m_feature_data; }

  Scorer* getScorer() { return m_scorer; }

  size_t NumberOfFeatures() const {
    return m_feature_data->NumberOfFeatures();
  }

  void NumberOfFeatures(size_t v) { m_feature_data->NumberOfFeatures(v); }

  std::string Features() const { return m_feature_data->Features(); }
  void Features(const std::string &f) { m_feature_data->Features(f); }

  bool hasSparseFeatures() const { return m_sparse_flag; }
  void mergeSparseFeatures();

  void loadNBest(const std::string &file);

  void load(const std::string &featfile, const std::string &scorefile);

  void save(const std::string &featfile, const std::string &scorefile, bool bin=false);

  //ADDED BY TS
  void removeDuplicates();
  //END_ADDED

  inline bool existsFeatureNames() const {
    return m_feature_data->existsFeatureNames();
  }

  inline std::string getFeatureName(size_t idx) const {
    return m_feature_data->getFeatureName(idx);
  }

  inline size_t getFeatureIndex(const std::string& name) const {
    return m_feature_data->getFeatureIndex(name);
  }

  /**
   * Create shard_count shards. If shard_size == 0, then the shards are non-overlapping
   * and exhaust the data. If 0 < shard_size <= 1, then shards are chosen by sampling
   * the data (with replacement) and shard_size is interpreted as the proportion
   * of the total size.
   */
  void createShards(size_t shard_count, float shard_size, const std::string& scorerconfig,
                    std::vector<Data>& shards);
};

#endif  // MERT_DATA_H_
