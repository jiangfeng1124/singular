// Author: Karl Stratos (karlstratos@gmail.com)
//
// An application of CCA for inducing lexical representations. In a particular
// setting, CCA is shown to recover the emission parameters of a hard-clustering
// hidden Markov model (HMM): A spectral algorithm for learning class-based
// n-gram models of natrual language (Stratos et al., 2014).

#ifndef WORDREP_H
#define WORDREP_H

#include <fstream>

#include "sparsecca.h"

typedef size_t Word;
typedef size_t Context;

class CanonWord {
public:
    // Initializes CanonWord with no output directory.
    CanonWord() { }

    // Initializes CanonWord with an output directory.
    CanonWord(const string &output_directory) {
	SetOutputDirectory(output_directory);
    }

    ~CanonWord() { }

    // Sets the output directory.
    void SetOutputDirectory(const string &output_directory);

    // Removes the content in the output directory.
    void ResetOutputDirectory();

    // Computes word counts from the corpus with appropriate preprocessing.
    void ExtractStatistics(const string &corpus_file);

    // Induces lexical representations from word counts in the output directory.
    void InduceLexicalRepresentations();

    // Sets the rare word cutoff value (-1 lets the model decide).
    void set_rare_cutoff(int rare_cutoff) { rare_cutoff_ = rare_cutoff; }

    // Sets the context window size.
    void set_window_size(int window_size) { window_size_ = window_size; }

    // Sets the flag for indicating that there is a sentence per line in the
    // text corpus.
    void set_sentence_per_line(bool sentence_per_line) {
	sentence_per_line_ = sentence_per_line;
    }

    // Sets the dimension of the CCA subspace.
    void set_cca_dim(size_t cca_dim) { cca_dim_ = cca_dim; }

    // Sets the smoothing term for calculating the correlation matrix (-1 lets
    // the model decide).
    void set_smoothing_term(int smoothing_term) {
	smoothing_term_ = smoothing_term;
    }

    // Sets the number of clusters.
    void set_num_clusters(int num_clusters) { num_clusters_ = num_clusters; }

    // Returns the computed word vectors
    unordered_map<string, Eigen::VectorXd> *wordvectors() {
	return &wordvectors_;
    }

    // Returns the singular values of the correlation matrix .
    Eigen::VectorXd *singular_values() { return &singular_values_; }

    // Returns the special string for representing rare words.
    string kRareString() { return kRareString_; }

    // Returns the special string for buffering.
    string kBufferString() { return kBufferString_; }

    // Returns the path to the word-context count file.
    string CountWordContextPath() {
	return output_directory_ + "/count_word_context_" + Signature(1);
    }

    // Returns the path to the word count file.
    string CountWordPath() {
	return output_directory_ + "/count_word_" + Signature(0);
    }

    // Returns the path to the context count file.
    string CountContextPath() {
	return output_directory_ + "/count_context_" + Signature(1);
    }

    // Returns the integer ID corresponding to a word string.
    Word word_str2num(const string &word_string);

    // Returns the original string form of a word integer ID.
    string word_num2str(Word word);

    // Returns the integer ID corresponding to a context string.
    Context context_str2num(const string &context_string);

    // Returns the original string form of a context integer ID.
    string context_num2str(Context context);

private:
    // Extracts the count of each word type appearing in the given corpus file.
    void CountWords(const string &corpus_file);

    // Adds the word to the word dictionary if not already known.
    Word AddWordIfUnknown(const string &word_string);

    // Adds the context to the context dictionary if not already known.
    Context AddContextIfUnknown(const string &context_string);

    // Determines rare word types.
    void DetermineRareWords();

    // Computes the unnormalized covariance values (i.e., counts) from the given
    // corpus file.
    void ComputeCovariance(const string &corpus_file);

    // Induces vector representations of word types based on cached count files.
    void InduceWordVectors(
	const vector<pair<string, size_t> > &sorted_wordcount);

    // Returns the path to the corpus information file.
    string CorpusInfoPath() { return output_directory_ + "/corpus_info"; }

    // Returns the path to the log file.
    string LogPath() { return output_directory_ + "/log"; }

    // Returns the path to the sorted word types file.
    string SortedWordTypesPath() {
	return output_directory_ + "/sorted_word_types";
    }

    // Returns the path to the rare word file.
    string RarePath() {
	return output_directory_ + "/rare_words_" + Signature(0);
    }

    // Returns the path to the str2num mapping for words.
    string WordStr2NumPath() {
	return output_directory_ + "/word_str2num_" + Signature(0);
    }

    // Returns the path to the str2num mapping for context.
    string ContextStr2NumPath() {
	return output_directory_ + "/context_str2num_" + Signature(1);
    }

    // Returns the path to the word vectors.
    string WordVectorsPath() {
	return output_directory_ + "/wordvectors_" + Signature(2);
    }

    // Returns the path to the singular values.
    string SingularValuesPath() {
	return output_directory_ + "/singular_values_" + Signature(2);
    }

    // Returns the path to the PCA variance values.
    string PCAVariancePath() {
	return output_directory_ + "/pca_variance_" + Signature(2);
    }

    // Returns the path to the K-means assignment.
    string KMeansPath() {
	return output_directory_ + "/kmeans_" + Signature(3);
    }

    // Returns a string signature of tunable parameters.
    //    version=0: rare_cutoff_
    //    version=1: rare_cutoff_, window_size_, sentence_per_line_
    //    version=2: rare_cutoff_, window_size_, sentence_per_line_, cca_dim_,
    //               smoothing_term_
    //    version=3: rare_cutoff_, window_size_, sentence_per_line_, cca_dim_,
    //               smoothing_term_, num_clusters_
    string Signature(size_t version);

    // Loads the word-integer dictionary from a cached file.
    void LoadWordIntegerDictionary();

    // Loads the word counts from a cached file.
    void LoadWordCounts();

    // Performs CCA on cached count files and returns the projection for the
    // first view.
    Eigen::MatrixXd PerformCCAOnComputedCounts();

    // Performs PCA on word vectors to put them in the PCA basis. Assumes that
    // each column of the matrix is a word vector.
    void ChangeOfBasisToPCACoordinates(Eigen::MatrixXd *word_matrix);

    // Do K-means clustering over word vectors. It initializes the K centroids
    // as the K most frequent word types.
    void PerformKMeans(size_t K,
		       const vector<pair<string, size_t> > &sorted_wordcount);

    // Count of each word string type appearing in a corpus.
    unordered_map<string, size_t> wordcount_;

    // Maps a word string to an integer ID.
    unordered_map<string, Word> word_str2num_;

    // Maps a word integer ID to its original string form.
    unordered_map<Word, string> word_num2str_;

    // Maps a context string to an integer ID.
    unordered_map<string, Context> context_str2num_;

    // Maps a context integer ID to its original string form.
    unordered_map<Context, string> context_num2str_;

    // Path to the log file.
    ofstream log_;

    // Number of words (i.e., size of the corpus).
    size_t num_words_ = 0;

    // Special string for representing rare words.
    const string kRareString_ = "<?>";

    // Special string for representing the out-of-sentence buffer.
    const string kBufferString_ = "<!>";

    // Computed word vectors.
    unordered_map<string, Eigen::VectorXd> wordvectors_;

    // Singular values of the correlation matrix.
    Eigen::VectorXd singular_values_;

    // Path to the output directory.
    string output_directory_;

    // If a word type appears <= this number, treat it as a rare symbol.
    // If it is -1, we let the model decide rare words based on word counts.
    int rare_cutoff_ = -1;

    // Size of the context to compute covariance on. Note that it needs to be
    // odd if we want the left and right context to have the same length.
    size_t window_size_ = 3;

    // Have a sentence per line in the text corpus?
    bool sentence_per_line_ = false;

    // Dimension of the CCA subspace.
    size_t cca_dim_;

    // Smoothing term for calculating the correlation matrix. If it's negative,
    // we let the model decide based on the smallest word count.
    int smoothing_term_ = -1;

    // Number of clusters. If negative (default), it's set as the CCA dimension.
    int num_clusters_ = -1;
};

#endif  // WORDREP_H