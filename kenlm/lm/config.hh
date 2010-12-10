#ifndef LM_CONFIG__
#define LM_CONFIG__

#include <iosfwd>

#include "util/mmap.hh"

/* Configuration for ngram model.  Separate header to reduce pollution. */

namespace lm { namespace ngram {

class EnumerateVocab;

struct Config {
  // EFFECTIVE FOR BOTH ARPA AND BINARY READS 

  // Where to log messages including the progress bar.  Set to NULL for
  // silence.
  std::ostream *messages;

  // This will be called with every string in the vocabulary.  See
  // enumerate_vocab.hh for more detail.  Config does not take ownership; you
  // are still responsible for deleting it (or stack allocating).  
  EnumerateVocab *enumerate_vocab;



  // ONLY EFFECTIVE WHEN READING ARPA

  // What to do when <unk> isn't in the provided model. 
  typedef enum {THROW_UP, COMPLAIN, SILENT} UnknownMissing;
  UnknownMissing unknown_missing;

  // The probability to substitute for <unk> if it's missing from the model.  
  // No effect if the model has <unk> or unknown_missing == THROW_UP.
  float unknown_missing_prob;

  // Size multiplier for probing hash table.  Must be > 1.  Space is linear in
  // this.  Time is probing_multiplier / (probing_multiplier - 1).  No effect
  // for sorted variant.  
  // If you find yourself setting this to a low number, consider using the
  // Sorted version instead which has lower memory consumption.  
  float probing_multiplier;

  // Amount of memory to use for building.  The actual memory usage will be
  // higher since this just sets sort buffer size.  Only applies to trie
  // models.
  std::size_t building_memory;

  // Template for temporary directory appropriate for passing to mkdtemp.  
  // The characters XXXXXX are appended before passing to mkdtemp.  Only
  // applies to trie.  If NULL, defaults to write_mmap.  If that's NULL,
  // defaults to input file name.  
  const char *temporary_directory_prefix;

  // Level of complaining to do when an ARPA instead of a binary format.
  typedef enum {ALL, EXPENSIVE, NONE} ARPALoadComplain;
  ARPALoadComplain arpa_complain;

  // While loading an ARPA file, also write out this binary format file.  Set
  // to NULL to disable.  
  const char *write_mmap;

  // Include the vocab in the binary file?  Only effective if write_mmap != NULL.  
  bool include_vocab;

  
  
  // ONLY EFFECTIVE WHEN READING BINARY
  
  // How to get the giant array into memory: lazy mmap, populate, read etc.
  // See util/mmap.hh for details of MapMethod.  
  util::LoadMethod load_method;



  // Set defaults. 
  Config();
};

} /* namespace ngram */ } /* namespace lm */

#endif // LM_CONFIG__