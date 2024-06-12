/* 
 * Copyright 2024 Eduardo José Gómez Hernández (University of Murcia)
 * Copyright 2024 Emilio Domínguez (University of Murcia)
 * Copyright 2024 CAPS-Group (University of Murcia)
 * Copyright 2020 HPS/SAFARI Research Groups
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __CPU_PRED_TAGE_EMILIO_HH__
#define __CPU_PRED_TAGE_EMILIO_HH__

#include <vector>

#include "base/types.hh"
#include "cpu/pred/bpred_unit.hh"
#include "cpu/pred/tage_base.hh"
#include "params/TAGE_EMILIO.hh"

namespace gem5
{

namespace branch_prediction
{

#ifndef SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_
#define SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_

namespace tagescl {

struct CONFIG_64KB {
  // static constexpr bool PIPELINE_SUPPORT = true;
  static constexpr bool USE_LOOP_PREDICTOR = true;
  static constexpr bool USE_SC = true;
  static constexpr int CONFIDENCE_COUNTER_WIDTH = 7;

  struct TAGE {
    static constexpr int MIN_HISTORY_SIZE = 6;
    static constexpr int MAX_HISTORY_SIZE = 3000;
    static constexpr int NUM_HISTORIES = 18;
    static constexpr int PATH_HISTORY_WIDTH = 27;
    static constexpr int FIRST_LONG_HISTORY_TABLE = 13;
    static constexpr int FIRST_2WAY_TABLE = 9;
    static constexpr int LAST_2WAY_TABLE = 22;
    static constexpr int SHORT_HISTORY_TAG_BITS = 8;
    static constexpr int LONG_HISTORY_TAG_BITS = 12;
    static constexpr int PRED_COUNTER_WIDTH = 3;
    static constexpr int USEFUL_BITS = 1;
    static constexpr int LOG_ENTRIES_PER_BANK = 10;
    static constexpr int SHORT_HISTORY_NUM_BANKS = 10;
    static constexpr int LONG_HISTORY_NUM_BANKS = 20;
    static constexpr int EXTRA_ENTRIES_TO_ALLOCATE = 1;
    static constexpr int TICKS_UNTIL_USEFUL_SHIFT = 1024;
    static constexpr int ALT_SELECTOR_LOG_TABLE_SIZE = 4;
    static constexpr int ALT_SELECTOR_ENTRY_WIDTH = 5;
    static constexpr int BIMODAL_HYSTERESIS_SHIFT = 2;
    static constexpr int BIMODAL_LOG_TABLES_SIZE = 13;
  };

  struct LOOP {
    static constexpr int LOG_NUM_ENTRIES = 5;
    static constexpr int ITERATION_COUNTER_WIDTH = 10;
    static constexpr int TAG_BITS = 10;
    static constexpr int CONFIDENCE_THRESHOLD = 15;
  };

  struct SC {
    static constexpr int UPDATE_THRESHOLD_WIDTH = 12;
    static constexpr int PERPC_UPDATE_THRESHOLD_WIDTH = 8;
    static constexpr int INITIAL_UPDATE_THRESHOLD = 35 << 3;

    static constexpr bool USE_VARIABLE_THRESHOLD = true;
    static constexpr int LOG_SIZE_PERPC_THRESHOLD_TABLE =
        USE_VARIABLE_THRESHOLD ? 6 : 0;
    static constexpr int LOG_SIZE_VARIABLE_THRESHOLD_TABLE =
        LOG_SIZE_PERPC_THRESHOLD_TABLE / 2;
    static constexpr int VARIABLE_THRESHOLD_WIDTH = 6;
    static constexpr int INITIAL_VARIABLE_THRESHOLD = 7;
    static constexpr int INITIAL_VARIABLE_THRESHOLD_FOR_BIAS = 4;
    static constexpr int LOG_BIAS_ENTRIES = 8;

    static constexpr int LOG_SIZE_GLOBAL_HISTORY_GEHL = 10;
    struct GLOBAL_HISTORY_GEHL_HISTORIES {
      static constexpr int arr[] = {40, 24, 10};
    };
    static constexpr int LOG_SIZE_PATH_GEHL = 9;
    struct PATH_GEHL_HISTORIES {
      static constexpr int arr[] = {25, 16, 9};
    };

    static constexpr bool USE_LOCAL_HISTORY = true;
    static constexpr int FIRST_LOCAL_HISTORY_LOG_TABLE_SIZE = 8;
    static constexpr int FIRST_LOCAL_HISTORY_SHIFT = 2;
    static constexpr int LOG_SIZE_FIRST_LOCAL_GEHL = 10;
    struct FIRST_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {11, 6, 3};
    };

    static constexpr bool USE_SECOND_LOCAL_HISTORY = true;
    static constexpr int SECOND_LOCAL_HISTORY_LOG_TABLE_SIZE = 4;
    static constexpr int SECOND_LOCAL_HISTORY_SHIFT = 5;
    static constexpr int LOG_SIZE_SECOND_LOCAL_GEHL = 9;
    struct SECOND_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {16, 11, 6};
    };

    static constexpr bool USE_THIRD_LOCAL_HISTORY = true;
    static constexpr int THIRD_LOCAL_HISTORY_LOG_TABLE_SIZE = 4;
    static constexpr int THIRD_LOCAL_HISTORY_SHIFT = 10;
    static constexpr int LOG_SIZE_THIRD_LOCAL_GEHL = 10;
    struct THIRD_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {9, 4};
    };

    static constexpr bool USE_IMLI = true;
    static constexpr int IMLI_COUNTER_WIDTH = 8;
    static constexpr int IMLI_TABLE_SIZE = 1 << IMLI_COUNTER_WIDTH;
    static constexpr int log_size_first_imli_gehl = 8;
    struct FIRST_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {IMLI_COUNTER_WIDTH};
    };
    static constexpr int LOG_SIZE_SECOND_IMLI_GEHL = 9;
    struct SECOND_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {10, 4};
    };

    static constexpr int PRECISION = 6;
    static constexpr int SC_PATH_HISTORY_WIDTH = 27;
  };
};

constexpr int CONFIG_64KB::SC::GLOBAL_HISTORY_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::PATH_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::FIRST_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::SECOND_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::THIRD_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::FIRST_IMLI_GEHL_HISTORIES::arr[];
constexpr int CONFIG_64KB::SC::SECOND_IMLI_GEHL_HISTORIES::arr[];

/****************************************************************************************/
struct CONFIG_80KB {
  // static constexpr bool PIPELINE_SUPPORT = true;
  static constexpr bool USE_LOOP_PREDICTOR = true;
  static constexpr bool USE_SC = true;
  static constexpr int CONFIDENCE_COUNTER_WIDTH = 7;

  struct TAGE {
    static constexpr int MIN_HISTORY_SIZE = 6;
    static constexpr int MAX_HISTORY_SIZE = 3000;
    static constexpr int NUM_HISTORIES = 18;
    static constexpr int PATH_HISTORY_WIDTH = 27;
    static constexpr int FIRST_LONG_HISTORY_TABLE = 13;
    static constexpr int FIRST_2WAY_TABLE = 9;
    static constexpr int LAST_2WAY_TABLE = 22;
    static constexpr int SHORT_HISTORY_TAG_BITS = 8;
    static constexpr int LONG_HISTORY_TAG_BITS = 12;
    static constexpr int PRED_COUNTER_WIDTH = 3;
    static constexpr int USEFUL_BITS = 1;
    static constexpr int LOG_ENTRIES_PER_BANK = 10;
    static constexpr int SHORT_HISTORY_NUM_BANKS = 18;
    static constexpr int LONG_HISTORY_NUM_BANKS = 21;
    static constexpr int EXTRA_ENTRIES_TO_ALLOCATE = 1;
    static constexpr int TICKS_UNTIL_USEFUL_SHIFT = 1024;
    static constexpr int ALT_SELECTOR_LOG_TABLE_SIZE = 4;
    static constexpr int ALT_SELECTOR_ENTRY_WIDTH = 5;
    static constexpr int BIMODAL_HYSTERESIS_SHIFT = 2;
    static constexpr int BIMODAL_LOG_TABLES_SIZE = 13;
  };

  struct LOOP {
    static constexpr int LOG_NUM_ENTRIES = 5;
    static constexpr int ITERATION_COUNTER_WIDTH = 10;
    static constexpr int TAG_BITS = 10;
    static constexpr int CONFIDENCE_THRESHOLD = 15;
  };

  struct SC {
    static constexpr int UPDATE_THRESHOLD_WIDTH = 12;
    static constexpr int PERPC_UPDATE_THRESHOLD_WIDTH = 8;
    static constexpr int INITIAL_UPDATE_THRESHOLD = 35 << 3;

    static constexpr bool USE_VARIABLE_THRESHOLD = true;
    static constexpr int LOG_SIZE_PERPC_THRESHOLD_TABLE =
        USE_VARIABLE_THRESHOLD ? 6 : 0;
    static constexpr int LOG_SIZE_VARIABLE_THRESHOLD_TABLE =
        LOG_SIZE_PERPC_THRESHOLD_TABLE / 2;
    static constexpr int VARIABLE_THRESHOLD_WIDTH = 6;
    static constexpr int INITIAL_VARIABLE_THRESHOLD = 7;
    static constexpr int INITIAL_VARIABLE_THRESHOLD_FOR_BIAS = 4;
    static constexpr int LOG_BIAS_ENTRIES = 8;

    static constexpr int LOG_SIZE_GLOBAL_HISTORY_GEHL = 10;
    struct GLOBAL_HISTORY_GEHL_HISTORIES {
      static constexpr int arr[] = {40, 24, 10};
    };
    static constexpr int LOG_SIZE_PATH_GEHL = 9;
    struct PATH_GEHL_HISTORIES {
      static constexpr int arr[] = {25, 16, 9};
    };

    static constexpr bool USE_LOCAL_HISTORY = true;
    static constexpr int FIRST_LOCAL_HISTORY_LOG_TABLE_SIZE = 8;
    static constexpr int FIRST_LOCAL_HISTORY_SHIFT = 2;
    static constexpr int LOG_SIZE_FIRST_LOCAL_GEHL = 10;
    struct FIRST_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {11, 6, 3};
    };

    static constexpr bool USE_SECOND_LOCAL_HISTORY = true;
    static constexpr int SECOND_LOCAL_HISTORY_LOG_TABLE_SIZE = 4;
    static constexpr int SECOND_LOCAL_HISTORY_SHIFT = 5;
    static constexpr int LOG_SIZE_SECOND_LOCAL_GEHL = 9;
    struct SECOND_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {16, 11, 6};
    };

    static constexpr bool USE_THIRD_LOCAL_HISTORY = true;
    static constexpr int THIRD_LOCAL_HISTORY_LOG_TABLE_SIZE = 4;
    static constexpr int THIRD_LOCAL_HISTORY_SHIFT = 10;
    static constexpr int LOG_SIZE_THIRD_LOCAL_GEHL = 10;
    struct THIRD_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {9, 4};
    };

    static constexpr bool USE_IMLI = true;
    static constexpr int IMLI_COUNTER_WIDTH = 8;
    static constexpr int IMLI_TABLE_SIZE = 1 << IMLI_COUNTER_WIDTH;
    static constexpr int log_size_first_imli_gehl = 8;
    struct FIRST_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {IMLI_COUNTER_WIDTH};
    };
    static constexpr int LOG_SIZE_SECOND_IMLI_GEHL = 9;
    struct SECOND_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {10, 4};
    };

    static constexpr int PRECISION = 8;
    static constexpr int SC_PATH_HISTORY_WIDTH = 27;
  };
};

constexpr int CONFIG_80KB::SC::GLOBAL_HISTORY_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::PATH_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::FIRST_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::SECOND_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::THIRD_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::FIRST_IMLI_GEHL_HISTORIES::arr[];
constexpr int CONFIG_80KB::SC::SECOND_IMLI_GEHL_HISTORIES::arr[];

template <class TAGE_CONFIG>
struct Tage_Prediction_Info {
  // Overal prediction and confidence.
  bool prediction;
  bool high_confidence;
  bool medium_confidence;
  bool low_confidence;

  // Other useful intermediate predictions.
  bool longest_match_prediction;
  bool alt_prediction;
  bool alt_confidence;
  int hit_bank;
  int alt_bank;

  // Extra information needed for updates.
  int indices[2 * TAGE_CONFIG::NUM_HISTORIES + 1];
  int tags[2 * TAGE_CONFIG::NUM_HISTORIES + 1];
  int num_global_history_bits;
  int64_t global_history_head_checkpoint_;
  int64_t path_history_checkpoint;
};

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_

#ifndef SPEC_TAGE_SC_L_UTILS_HPP_
#define SPEC_TAGE_SC_L_UTILS_HPP_

#include <cassert>
#include <cstdint>
#include <vector>

namespace tagescl {

inline int get_min_num_bits_to_represent(int x) {
  assert(x > 0);
  int num_bits = 1;
  while (true) {
    if (1 << num_bits >= x) {
      return num_bits;
    }

    num_bits += 1;
  }
  assert(false);
}

/* Copying the implementation of std::conditional because PinCRT does not
 * include it*/
template <bool B, class T, class F>
struct conditional_type {
  typedef T type;
};

template <class T, class F>
struct conditional_type<false, T, F> {
  typedef F type;
};

template <int width>
struct Smallest_Int_Type {
  // This is logically equivalent to the following code assuming you could use
  // if statements for conditional type declarations:
  // if (width < 8) using type = int8_t;
  // else if (width < 16) using type = int16_t;
  // else if (width < 32) using type = int32_t;
  // else using type = int64_t;
  using type = typename conditional_type<
      (width < 8), int8_t,
      typename conditional_type<
          (width < 16), int16_t,
          typename conditional_type<(width < 32), int32_t,
                                    int64_t>::type>::type>::type;
};

/* Saturating counters: Could be signed or unsigned. Can be
 * directly updated using increment() or decrement(). Alternatively, one can
 * call upodate with a boolean indicting the direction.  */
template <int width, bool is_signed>
class Saturating_Counter {
 public:
  using Int_Type = typename Smallest_Int_Type<width>::type;

  Saturating_Counter() { set(0); }
  Saturating_Counter(Int_Type init_value) { set(init_value); }

  // Returns the current value of the counter.
  Int_Type get() const { return counter_; }

  // If condition is true, increments the counter, otherwise, decrements the
  // counter.
  void update(bool condition) {
    if (condition) {
      increment();
    } else {
      decrement();
    }
  }

  // Increments the counter (saturating).
  void increment(void) {
    if (counter_ < counter_max_) {
      counter_ += 1;
    }
  }

  // Decrements the counter (saturating).
  void decrement(void) {
    if (counter_ > counter_min_) {
      counter_ -= 1;
    }
  }

  // Sets the counter to value (value should be within the minimum and
  // maximum values).
  void set(Int_Type value) {
    assert(counter_min_ <= value && value <= counter_max_);
    counter_ = value;
  }

 private:
  static constexpr Int_Type counter_max_ =
      (is_signed ? ((1 << (width - 1)) - 1) : ((1 << width) - 1));
  static constexpr Int_Type counter_min_ =
      (is_signed ? -(1 << (width - 1)) : 0);

  Int_Type counter_;
};

// This is an ugly way to do random number generation, but I want to keep it
// compatible with Seznec for now.
class Random_Number_Generator {
 public:
  int operator()() {
    assert(phist_ptr_);
    assert(ptghist_ptr_);
    seed_++;
    seed_ ^= (*phist_ptr_);
    seed_ = (seed_ >> 21) + (seed_ << 11);
    seed_ ^= (int)(*ptghist_ptr_);
    seed_ = (seed_ >> 10) + (seed_ << 22);
    return (seed_);
  }

  int seed_ = 0;
  int64_t* phist_ptr_;
  int64_t* ptghist_ptr_;
};

struct Branch_Type {
  bool is_conditional;
  bool is_indirect;
};

template <typename T>
class Circular_Buffer {
 public:
  Circular_Buffer(unsigned max_size) {
    assert(max_size > 0);
    int min_num_address_bits = get_min_num_bits_to_represent(max_size);
    buffer_size_ = 1 << min_num_address_bits;
    buffer_access_mask_ = (1 << min_num_address_bits) - 1;
    buffer_.resize(buffer_size_);
  }

  T& operator[](int64_t id) {
    assert(id >= front_);
    assert(id <= back_);
    return buffer_[id & buffer_access_mask_];
  }

  int64_t back_id() const { return back_; }

  void deallocate_after(int64_t dealloc_id) {
    //DPRINTFN("Deallocate After: %lu -> %lu %lu\n", dealloc_id, front_, back_);
    assert(dealloc_id >= front_);
    assert(dealloc_id <= back_);
    size_ -= (back_ - dealloc_id);
    back_ = dealloc_id;
  }

  void deallocate_and_after(int64_t dealloc_id) {
    //DPRINTFN("Deallocate And After: %lu -> %lu %lu\n", dealloc_id, front_, back_);
    assert(dealloc_id >= front_);
    assert(dealloc_id <= back_);
    size_ -= (back_ - dealloc_id + 1);
    back_ = dealloc_id - 1;
  }

  int64_t allocate_back() {
    //DPRINTFN("Allocate: %lu\n", back_);
    assert(size_ < buffer_size_);
    back_ += 1;
    size_ += 1;
    return back_;
  }

  void deallocate_front(int64_t pop_id) {
    //DPRINTFN("Deallocate: %lu, %lu\n", pop_id, front_);
    assert(pop_id == front_);
    assert(size_ > 0);
    front_ += 1;
    size_ -= 1;
  }

 private:
  std::vector<T> buffer_;
  int64_t buffer_size_;
  int64_t buffer_access_mask_;

  int64_t back_ = -1;
  int64_t front_ = 0;
  int64_t size_ = 0;
};

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_UTILS_HPP_

#ifndef SPEC_TAGE_SC_L_LOOP_PREDICTOR_HPP_
#define SPEC_TAGE_SC_L_LOOP_PREDICTOR_HPP_

#include <vector>


namespace tagescl {

struct Loop_Predictor_Indices {
  int bank[4];
};

template <class LOOP_CONFIG>
struct Loop_Prediction_Info {
  int8_t hit_bank;
  bool valid;
  bool prediction;

  // Information needed for table updates.
  Loop_Predictor_Indices indices;
  int16_t tag;
  Saturating_Counter<LOOP_CONFIG::ITERATION_COUNTER_WIDTH, false>
      current_iter_checkpoint;
};

template <class LOOP_CONFIG>
class Loop_Predictor {
 public:
  Loop_Predictor(Random_Number_Generator& random_number_gen)
      : table_(1 << LOOP_CONFIG::LOG_NUM_ENTRIES),
        random_number_gen_(random_number_gen) {}

  void get_prediction(
      uint64_t br_pc,
      Loop_Prediction_Info<LOOP_CONFIG>* prediction_info) const {
    prediction_info->valid = false;
    prediction_info->prediction = false;
    prediction_info->hit_bank = -1;

    prediction_info->indices = get_indices(br_pc);
    int tag = get_tag(br_pc);
    prediction_info->tag = tag;

    for (int i = 0; i < 4; i++) {
      int index = prediction_info->indices.bank[i];

      if (table_[index].tag == tag) {
        prediction_info->hit_bank = i;
        prediction_info->valid =
            ((table_[index].confidence == LOOP_CONFIG::CONFIDENCE_THRESHOLD) ||
             (table_[index].confidence * table_[index].total_iterations > 128));

        prediction_info->current_iter_checkpoint =
            table_[index].speculative_current_iter.get();
        if (table_[index].speculative_current_iter.get() + 1 ==
            table_[index].total_iterations) {
          prediction_info->prediction = !table_[index].dir;
          break;
        } else {
          prediction_info->prediction = table_[index].dir;
          break;
        }
      }
    }
  }

  void update_speculative_state(
      const Loop_Prediction_Info<LOOP_CONFIG>& prediction_info) {
    if (prediction_info.hit_bank >= 0) {
      int index = prediction_info.indices.bank[prediction_info.hit_bank];
      if (table_[index].total_iterations != 0) {
        table_[index].speculative_current_iter.increment();
        if (table_[index].speculative_current_iter.get() >=
            table_[index].total_iterations) {
          table_[index].speculative_current_iter.set(0);
        }
      }
    }
  }

  void commit_state(uint64_t br_pc, bool resolve_dir,
                    const Loop_Prediction_Info<LOOP_CONFIG>& prediction_info,
                    bool finally_mispredicted, bool tage_prediction) {}

  void commit_state_at_retire(
      uint64_t br_pc, bool resolve_dir,
      const Loop_Prediction_Info<LOOP_CONFIG>& prediction_info,
      bool finally_mispredicted, bool tage_prediction) {
    if (prediction_info.hit_bank >= 0) {
      int index = prediction_info.indices.bank[prediction_info.hit_bank];
      if (table_[index].tag != prediction_info.tag) {
        // The entry must have been replaced by anoher entry.
        // assert(false);
        return;
      }

      if (prediction_info.valid) {
        if (resolve_dir != prediction_info.prediction) {
          // free the entry
          table_[index].total_iterations = 0;
          table_[index].confidence = 0;
          table_[index].age = 0;
          table_[index].current_iter.set(0);
          table_[index].speculative_current_iter.set(0);
          return;
        } else if ((prediction_info.prediction != tage_prediction) ||
                   ((random_number_gen_() & 7) == 0)) {
          if (table_[index].age < LOOP_CONFIG::CONFIDENCE_THRESHOLD) {
            table_[index].age += 1;
          }
        }
      }

      table_[index].current_iter.increment();
      if (table_[index].current_iter.get() > table_[index].total_iterations) {
        // treat like the 1st encounter of the loop
        table_[index].total_iterations = 0;
        table_[index].confidence = 0;
      }

      if (resolve_dir != table_[index].dir) {
        if (table_[index].current_iter.get() ==
            table_[index].total_iterations) {
          if (table_[index].confidence < LOOP_CONFIG::CONFIDENCE_THRESHOLD) {
            table_[index].confidence += 1;
          }

          if (table_[index].total_iterations < 3) {
            // just do not predict when the loop count is 1 or 2
            // free the entry
            table_[index].dir = resolve_dir;
            table_[index].total_iterations = 0;
            table_[index].age = 0;
            table_[index].current_iter.set(0);
            table_[index].speculative_current_iter.set(0);
          }
        } else {
          if (table_[index].total_iterations == 0) {
            // first complete nest;
            table_[index].confidence = 0;
            table_[index].total_iterations = table_[index].current_iter.get();
            table_[index].speculative_current_iter.set(0);
          } else {
            // not the same number of iterations as last time: free
            // the entry
            table_[index].total_iterations = 0;
            table_[index].confidence = 0;
          }
        }
        table_[index].current_iter.set(0);
      }

      if (finally_mispredicted) {
        table_[index].speculative_current_iter = table_[index].current_iter;
      }
    } else if (finally_mispredicted) {
      int random_bank = random_number_gen_() & 3;

      if ((random_number_gen_() & 3) == 0) {
        int tag = get_tag(br_pc);
        int index = prediction_info.indices.bank[random_bank];
        if (table_[index].age == 0) {
          // most of mispredictions are on last iterations
          table_[index].dir = !resolve_dir;
          table_[index].tag = tag;
          table_[index].total_iterations = 0;
          table_[index].age = 7;
          table_[index].confidence = 0;
          table_[index].current_iter.set(0);
          table_[index].speculative_current_iter.set(0);
        } else {
          table_[index].age -= 1;
        }
      }
    }
  }

  void global_recover_speculative_state(
      const Loop_Prediction_Info<LOOP_CONFIG>& prediction_info) {}

  void local_recover_speculative_state(
      const Loop_Prediction_Info<LOOP_CONFIG>& prediction_info) {
    if (prediction_info.hit_bank >= 0) {
      int index = prediction_info.indices.bank[prediction_info.hit_bank];
      if (table_[index].tag != prediction_info.tag) {
        // The entry must have been replaced by anoher entry.
        // assert(false);
        return;
      }
      table_[index].speculative_current_iter =
          prediction_info.current_iter_checkpoint;
    }
  }

  static void build_empty_prediction(
      Loop_Prediction_Info<LOOP_CONFIG>* prediction_info) {
    prediction_info->hit_bank = -1;
  }

 private:
  struct LoopPredictorEntry {
    int16_t total_iterations = 0;  // 10 bits
    int16_t tag = 0;               // 10 bits
    int8_t confidence = 0;         // 4 bits
    int8_t age = 0;                // 4 bits
    bool dir = 0;                  // 1 bit
    Saturating_Counter<LOOP_CONFIG::ITERATION_COUNTER_WIDTH, false>
        speculative_current_iter;  // 10 bits
    Saturating_Counter<LOOP_CONFIG::ITERATION_COUNTER_WIDTH, false>
        current_iter;  // 10 bits

    LoopPredictorEntry() : current_iter(0) {}
  };

  Loop_Predictor_Indices get_indices(uint64_t br_pc) const;
  int get_tag(uint64_t br_pc) const;

  std::vector<LoopPredictorEntry> table_;

  Random_Number_Generator& random_number_gen_;
};

template <class LOOP_CONFIG>
Loop_Predictor_Indices Loop_Predictor<LOOP_CONFIG>::get_indices(
    uint64_t br_pc) const {
  Loop_Predictor_Indices indices;
  int component1 =
      ((br_pc ^ (br_pc >> 2)) & ((1 << (LOOP_CONFIG::LOG_NUM_ENTRIES - 2)) - 1))
      << 2;
  int component2 = (br_pc >> (LOOP_CONFIG::LOG_NUM_ENTRIES - 2)) &
                   ((1 << (LOOP_CONFIG::LOG_NUM_ENTRIES - 2)) - 1);

  for (int i = 0; i < 4; ++i) {
    indices.bank[i] = (component1 ^ ((component2 >> i) << 2)) + i;
  }
  return indices;
}

template <class LOOP_CONFIG>
int Loop_Predictor<LOOP_CONFIG>::get_tag(uint64_t br_pc) const {
  int tag = (br_pc >> (LOOP_CONFIG::LOG_NUM_ENTRIES - 2)) &
            ((1 << 2 * LOOP_CONFIG::TAG_BITS) - 1);
  tag = tag ^ (tag >> LOOP_CONFIG::TAG_BITS);
  tag = tag & ((1 << LOOP_CONFIG::TAG_BITS) - 1);
  return tag;
}

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_LOOP_PREDICTOR_HPP_

#ifndef SPEC_TAGE_SC_L_STATISTICAL_CORRECTOR_HPP_
#define SPEC_TAGE_SC_L_STATISTICAL_CORRECTOR_HPP_


namespace tagescl {

/* A table of counters used for SC Gehl thresholds.
 * The accessors automatically creates a valid index by creating a hash of PC.*/
template <int width, int log_table_size>
class Threshold_Table {
 public:
  using Counter_Type = Saturating_Counter<width, true>;

  Threshold_Table(int init_value) {
    for (int i = 0; i < table_size; ++i) {
      table_[i].set(init_value);
    }
  }

  const Counter_Type& get_entry(uint64_t br_pc) const {
    return table_[(br_pc ^ (br_pc >> 2)) & (table_size - 1)];
  }

  Counter_Type& get_entry(uint64_t br_pc) {
    return table_[(br_pc ^ (br_pc >> 2)) & (table_size - 1)];
  }

  int temp_get_index(uint64_t br_pc) const {
    return (br_pc ^ (br_pc >> 2)) & (table_size - 1);
  }

 private:
  static constexpr int table_size = 1 << log_table_size;
  Counter_Type table_[table_size];
};

template <int log_table_size, int pc_shift>
class Local_History_Table {
 public:
  Local_History_Table() : table_() {}

  int64_t get_history(uint64_t br_pc) const { return table_[get_index(br_pc)]; }
  int64_t& get_history(uint64_t br_pc) { return table_[get_index(br_pc)]; }

 private:
  static constexpr int table_size = 1 << log_table_size;

  int get_index(uint64_t br_pc) const {
    return (br_pc ^ (br_pc >> pc_shift)) & (table_size - 1);
  }

  int pc_shift_;
  int64_t table_[table_size];
};

// A GEHL Table. Used by Statistical Corrector.
template <int counter_width, class Histories, int log_table_size>
class Gehl {
 public:
  using Counter_Type = Saturating_Counter<counter_width, true>;

  Gehl() : tables_() {
    for (int i = 0; i < num_histories; ++i) {
      for (int j = 0; j < ((1 << log_table_size) - 1); ++j) {
        tables_[i][j].set((j & 1) ? 0 : -1);
      }
    }
  }

  int get_prediction_sum(uint64_t br_pc, int64_t history) const {
    int sum = 0;
    for (int i = 0; i < num_histories; i++) {
      int index = get_index(br_pc, history, i);
      sum += (2 * tables_[i][index].get() + 1);
    }
    return sum;
  }

  void update(uint64_t br_pc, int64_t history, bool resolve_dir) {
    for (int i = 0; i < num_histories; i++) {
      int index = get_index(br_pc, history, i);
      tables_[i][index].update(resolve_dir);
    }
  }

 private:
  static constexpr int num_histories =
      sizeof(Histories::arr) / sizeof(Histories::arr[0]);

  int get_index(uint64_t br_pc, int64_t history, int history_id) const {
    int64_t masked_history =
        history & ((int64_t(1) << Histories::arr[history_id]) - 1);
    int64_t index = br_pc ^ masked_history;
    index ^= masked_history >> (8 - history_id);
    index ^= masked_history >> (16 - 2 * history_id);
    index ^= masked_history >> (24 - 3 * history_id);
    index ^= masked_history >> (32 - 3 * history_id);
    index ^= masked_history >> (40 - 4 * history_id);
    index &=
        ((1 << (log_table_size - (history_id >= (num_histories - 2)))) - 1);
    return static_cast<int>(index);
  }

  Counter_Type tables_[num_histories][1 << log_table_size];
};

struct SC_Histories_Snapshot {
  int64_t global_history;
  int64_t path;
  int64_t first_local_history;
  int64_t second_local_history;
  int64_t third_local_history;
  int64_t imli_counter;
  int64_t imli_local_history;
};

struct SC_Prediction_Info {
  int gehls_sum;
  int thresholds_sum;
  bool prediction;

  SC_Histories_Snapshot history_snapshot;
};

template <class CONFIG>
class Statistical_Corrector {
 public:
  Statistical_Corrector();

  void get_prediction(
      uint64_t br_pc,
      const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
      bool tage_or_loop_prediction, SC_Prediction_Info* prediction_info);

  void commit_state(
      uint64_t br_pc, bool resolve_dir,
      const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
      const SC_Prediction_Info& sc_prediction_info,
      bool tage_or_loop_prediction);

  void update_speculative_state(uint64_t br_pc, bool resolve_dir,
                                uint64_t br_taret, Branch_Type br_type,
                                SC_Prediction_Info* prediction_info);

  void commit_state_at_retire() {}

  void global_recover_speculative_state(
      const SC_Prediction_Info& prediction_info) {
    global_history_ = prediction_info.history_snapshot.global_history;
    path_ = prediction_info.history_snapshot.path;
  }

  void local_recover_speculative_state(
      uint64_t br_pc, const SC_Prediction_Info& prediction_info) {
    if (CONFIG::SC::USE_LOCAL_HISTORY) {
      first_local_history_table_.get_history(br_pc) =
          prediction_info.history_snapshot.first_local_history;
      if (CONFIG::SC::USE_SECOND_LOCAL_HISTORY) {
        second_local_history_table_.get_history(br_pc) =
            prediction_info.history_snapshot.second_local_history;
      }
      if (CONFIG::SC::USE_THIRD_LOCAL_HISTORY) {
        third_local_history_table_.get_history(br_pc) =
            prediction_info.history_snapshot.third_local_history;
      }
    }
    if (CONFIG::SC::USE_IMLI) {
      imli_counter_.set(prediction_info.history_snapshot.imli_counter);
      imli_table_[imli_counter_.get()] =
          prediction_info.history_snapshot.imli_local_history;
    }
  }

 private:
  using Counter_Type = Saturating_Counter<CONFIG::SC::PRECISION, true>;
  using Per_PC_Threshold_Table_Type =
      Threshold_Table<CONFIG::SC::PERPC_UPDATE_THRESHOLD_WIDTH,
                      CONFIG::SC::LOG_SIZE_PERPC_THRESHOLD_TABLE>;
  using Variable_Threshold_Table_Type =
      Threshold_Table<CONFIG::SC::VARIABLE_THRESHOLD_WIDTH,
                      CONFIG::SC::LOG_SIZE_VARIABLE_THRESHOLD_TABLE>;

  void initialize_bias_tables(void);

  int get_threshold_table_index(uint64_t br_pc);

  template <int PRECISION, class Gehl_Histories, int gehl_log_table_size,
            int threshold_width, int log_threshold_table_size>
  int get_gehl_prediction_sum(
      const Gehl<PRECISION, Gehl_Histories, gehl_log_table_size>& gehl,
      const Threshold_Table<threshold_width, log_threshold_table_size>&
          threshold_table,
      uint64_t br_pc, int64_t history) const {
    int prediction = gehl.get_prediction_sum(br_pc, history);
    if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
      if (threshold_table.get_entry(br_pc).get() >= 0) {
        prediction *= 2;
      }
    }
    return prediction;
  }

  template <int PRECISION, class Gehl_Histories, int gehl_log_table_size,
            int threshold_width, int log_threshold_table_size>
  void update_gehl_and_threshold(
      Gehl<PRECISION, Gehl_Histories, gehl_log_table_size>* gehl,
      Threshold_Table<threshold_width, log_threshold_table_size>*
          threshold_table,
      uint64_t br_pc, int64_t history, bool resolve_dir,
      int total_prediction_sum) {
    int gehl_sum = gehl->get_prediction_sum(br_pc, history);
    gehl->update(br_pc, history, resolve_dir);

    if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
      int total_sum_without_doubled_gehl =
          total_prediction_sum -
          (threshold_table->get_entry(br_pc).get() >= 0) * gehl_sum;
      bool prediction_without_multiplier =
          (total_sum_without_doubled_gehl >= 0);
      bool prediction_with_multiplier =
          (total_sum_without_doubled_gehl + gehl_sum >= 0);
      if (prediction_without_multiplier != prediction_with_multiplier) {
        threshold_table->get_entry(br_pc).update((gehl_sum >= 0) ==
                                                 resolve_dir);
      }
    }
  }

  int get_bias_table_index(
      uint64_t br_pc,
      const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
      bool tage_or_loop_prediction);

  int get_bias_sk_table_index(
      uint64_t br_pc,
      const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
      bool tage_or_loop_prediction);

  int get_bias_bank_table_index(
      uint64_t br_pc,
      const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
      bool tage_or_loop_prediction);

  int64_t global_history_ = 0;
  int64_t path_ = 0;
  Local_History_Table<CONFIG::SC::FIRST_LOCAL_HISTORY_LOG_TABLE_SIZE,
                      CONFIG::SC::FIRST_LOCAL_HISTORY_SHIFT>
      first_local_history_table_;
  Local_History_Table<CONFIG::SC::SECOND_LOCAL_HISTORY_LOG_TABLE_SIZE,
                      CONFIG::SC::SECOND_LOCAL_HISTORY_SHIFT>
      second_local_history_table_;
  Local_History_Table<CONFIG::SC::THIRD_LOCAL_HISTORY_LOG_TABLE_SIZE,
                      CONFIG::SC::THIRD_LOCAL_HISTORY_SHIFT>
      third_local_history_table_;
  Saturating_Counter<CONFIG::SC::IMLI_COUNTER_WIDTH, false> imli_counter_;
  int64_t imli_table_[CONFIG::SC::IMLI_TABLE_SIZE];

  Saturating_Counter<CONFIG::CONFIDENCE_COUNTER_WIDTH, true>
      first_high_confidence_ctr_;
  Saturating_Counter<CONFIG::CONFIDENCE_COUNTER_WIDTH, true>
      second_high_confidence_ctr_;

  Saturating_Counter<CONFIG::SC::UPDATE_THRESHOLD_WIDTH, true>
      update_threshold_;
  Per_PC_Threshold_Table_Type p_update_thresholds_;

  Gehl<CONFIG::SC::PRECISION,
       typename CONFIG::SC::GLOBAL_HISTORY_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_GLOBAL_HISTORY_GEHL>
      global_history_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::PATH_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_PATH_GEHL>
      path_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::FIRST_LOCAL_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_FIRST_LOCAL_GEHL>
      first_local_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::SECOND_LOCAL_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_SECOND_LOCAL_GEHL>
      second_local_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::THIRD_LOCAL_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_THIRD_LOCAL_GEHL>
      third_local_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::FIRST_IMLI_GEHL_HISTORIES,
       CONFIG::SC::log_size_first_imli_gehl>
      first_imli_gehl_;
  Gehl<CONFIG::SC::PRECISION, typename CONFIG::SC::SECOND_IMLI_GEHL_HISTORIES,
       CONFIG::SC::LOG_SIZE_SECOND_IMLI_GEHL>
      second_imli_gehl_;

  Variable_Threshold_Table_Type global_history_threshold_table_;
  Variable_Threshold_Table_Type path_threshold_table_;
  Variable_Threshold_Table_Type first_local_threshold_table_;
  Variable_Threshold_Table_Type second_local_threshold_table_;
  Variable_Threshold_Table_Type third_local_threshold_table_;
  Variable_Threshold_Table_Type first_imli_threshold_table_;
  Variable_Threshold_Table_Type second_imli_threshold_table_;
  Variable_Threshold_Table_Type bias_threshold_table_;

  std::vector<Counter_Type> bias_table_;
  std::vector<Counter_Type> bias_sk_table_;
  std::vector<Counter_Type> bias_bank_table_;
};

template <class CONFIG>
Statistical_Corrector<CONFIG>::Statistical_Corrector()
    : first_local_history_table_(),
      second_local_history_table_(),
      third_local_history_table_(),
      imli_counter_(0),
      imli_table_(),
      first_high_confidence_ctr_(0),
      second_high_confidence_ctr_(0),
      update_threshold_(CONFIG::SC::INITIAL_UPDATE_THRESHOLD),
      p_update_thresholds_(0),
      global_history_gehl_(),
      path_gehl_(),
      first_local_gehl_(),
      second_local_gehl_(),
      third_local_gehl_(),
      first_imli_gehl_(),
      second_imli_gehl_(),
      global_history_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      path_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      first_local_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      second_local_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      third_local_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      first_imli_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD),
      second_imli_threshold_table_(0),
      bias_threshold_table_(CONFIG::SC::INITIAL_VARIABLE_THRESHOLD_FOR_BIAS),
      bias_table_(1 << CONFIG::SC::LOG_BIAS_ENTRIES, Counter_Type(0)),
      bias_sk_table_(1 << CONFIG::SC::LOG_BIAS_ENTRIES, Counter_Type(0)),
      bias_bank_table_(1 << CONFIG::SC::LOG_BIAS_ENTRIES, Counter_Type(0)) {
  initialize_bias_tables();
}

template <class CONFIG>
void Statistical_Corrector<CONFIG>::get_prediction(
    uint64_t br_pc,
    const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
    bool tage_or_loop_prediction, SC_Prediction_Info* prediction_info) {
  int components_sum = 0;
  int thresholds_sum = (update_threshold_.get() >> 3) +
                       p_update_thresholds_.get_entry(br_pc).get();

  // Add bias.
  int bias_table_index = get_bias_table_index(br_pc, tage_prediction_info,
                                              tage_or_loop_prediction);
  components_sum += 2 * bias_table_[bias_table_index].get() + 1;

  // Add bias_sk.
  int bias_sk_table_index = get_bias_sk_table_index(br_pc, tage_prediction_info,
                                                    tage_or_loop_prediction);
  components_sum += 2 * bias_sk_table_[bias_sk_table_index].get() + 1;

  // Add bias_sk.
  int bias_bank_table_index = get_bias_bank_table_index(
      br_pc, tage_prediction_info, tage_or_loop_prediction);
  components_sum += 2 * bias_bank_table_[bias_bank_table_index].get() + 1;

  if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
    if (bias_threshold_table_.get_entry(br_pc).get() >= 0) {
      components_sum *= 2;
      thresholds_sum += 12;
    }
  }

  // Add global history and path GEHL components.
  components_sum += get_gehl_prediction_sum(
      global_history_gehl_, global_history_threshold_table_,
      (br_pc << 1) + (tage_or_loop_prediction ? 1 : 0), global_history_);
  components_sum +=
      get_gehl_prediction_sum(path_gehl_, path_threshold_table_, br_pc, path_);

  if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
    thresholds_sum +=
        12 * (global_history_threshold_table_.get_entry(br_pc).get() >= 0);
    thresholds_sum += 12 * (path_threshold_table_.get_entry(br_pc).get() >= 0);
  }

  // Add local history GEHL components.
  if (CONFIG::SC::USE_LOCAL_HISTORY) {
    components_sum += get_gehl_prediction_sum(
        first_local_gehl_, first_local_threshold_table_, br_pc,
        first_local_history_table_.get_history(br_pc));

    if (CONFIG::SC::USE_SECOND_LOCAL_HISTORY) {
      components_sum += get_gehl_prediction_sum(
          second_local_gehl_, second_local_threshold_table_, br_pc,
          second_local_history_table_.get_history(br_pc));
    }
    if (CONFIG::SC::USE_THIRD_LOCAL_HISTORY) {
      components_sum += get_gehl_prediction_sum(
          third_local_gehl_, third_local_threshold_table_, br_pc,
          third_local_history_table_.get_history(br_pc));
    }
  }
  if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
    thresholds_sum +=
        12 * (first_local_threshold_table_.get_entry(br_pc).get() >= 0);
    if (CONFIG::SC::USE_SECOND_LOCAL_HISTORY) {
      thresholds_sum +=
          12 * (second_local_threshold_table_.get_entry(br_pc).get() >= 0);
    }
    if (CONFIG::SC::USE_THIRD_LOCAL_HISTORY) {
      thresholds_sum +=
          12 * (third_local_threshold_table_.get_entry(br_pc).get() >= 0);
    }
  }
  if (CONFIG::SC::USE_IMLI) {
    components_sum +=
        get_gehl_prediction_sum(second_imli_gehl_, second_imli_threshold_table_,
                                br_pc, imli_table_[imli_counter_.get()]);
    components_sum +=
        get_gehl_prediction_sum(first_imli_gehl_, first_imli_threshold_table_,
                                br_pc, imli_counter_.get());
    if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
      thresholds_sum +=
          12 * (first_imli_threshold_table_.get_entry(br_pc).get() >= 0);
      // REVISIT: addition of the second IMLI component because it does
      // not
      // exist is Seznec's version, but it's probably a bug and should be
      // added
      // in.
      // thresholds_sum += (second_imli_threshold_table_.get_entry(br_pc)
      // >=
      // 0);
    }
  }

  prediction_info->gehls_sum = components_sum;
  prediction_info->thresholds_sum = thresholds_sum;
  bool sc_prediction = components_sum >= 0;

#ifdef DEBUG_PRINTS
  std::cout << "Tage pred: " << (int)tage_prediction_info.prediction
            << std::endl;
  std::cout << "Tage or loop pred: " << (int)tage_or_loop_prediction
            << std::endl;
  std::cout << "High confidence: " << (int)tage_prediction_info.high_confidence
            << std::endl;
  std::cout << "Medium confidence: "
            << (int)tage_prediction_info.medium_confidence << std::endl;
  std::cout << "Low confidence: " << (int)tage_prediction_info.low_confidence
            << std::endl;
  std::cout << "longest match prediction: "
            << (int)tage_prediction_info.longest_match_prediction << std::endl;
  std::cout << "alt prediction: " << (int)tage_prediction_info.alt_prediction
            << std::endl;
  std::cout << "alt confidence: " << (int)tage_prediction_info.alt_confidence
            << std::endl;
  std::cout << "hit bank: " << tage_prediction_info.hit_bank << std::endl;
  std::cout << "alt bank: " << tage_prediction_info.alt_bank << std::endl;
  std::cout << "gehl sum: " << components_sum << std::endl;
  std::cout << "thrs sum: " << thresholds_sum << std::endl;
#endif

  if (sc_prediction != tage_or_loop_prediction) {
    prediction_info->prediction = sc_prediction;

    if (tage_prediction_info.high_confidence) {
      if (std::abs(components_sum) < (thresholds_sum / 4)) {
        prediction_info->prediction = tage_or_loop_prediction;
      } else if (std::abs(components_sum) < (thresholds_sum / 2)) {
        prediction_info->prediction = (second_high_confidence_ctr_.get() < 0)
                                          ? sc_prediction
                                          : tage_or_loop_prediction;
      }
    }

    if (tage_prediction_info.medium_confidence) {
      if (std::abs(components_sum) < (thresholds_sum / 4)) {
        prediction_info->prediction = (first_high_confidence_ctr_.get() < 0)
                                          ? sc_prediction
                                          : tage_or_loop_prediction;
      } else {
        prediction_info->prediction = sc_prediction;
      }
    }
  } else {
    // SC and Tage_L predictions are equal, it does not matter which one we
    // choose.
    prediction_info->prediction = tage_or_loop_prediction;
  }
}

template <class CONFIG>
void Statistical_Corrector<CONFIG>::commit_state(
    uint64_t br_pc, bool resolve_dir,
    const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
    const SC_Prediction_Info& sc_prediction_info,
    bool tage_or_loop_prediction) {
  imli_table_[imli_counter_.get()];
  bool sc_prediction = (sc_prediction_info.gehls_sum >= 0);
  if (tage_or_loop_prediction != sc_prediction) {
    // REVIST: the first if statement seems to be redundant
    if (std::abs(sc_prediction_info.gehls_sum) <
        sc_prediction_info.thresholds_sum) {
      if (tage_prediction_info.high_confidence &&
          (std::abs(sc_prediction_info.gehls_sum) <
           sc_prediction_info.thresholds_sum / 2) &&
          (std::abs(sc_prediction_info.gehls_sum) >=
           sc_prediction_info.thresholds_sum / 4)) {
        second_high_confidence_ctr_.update(tage_or_loop_prediction ==
                                           resolve_dir);
      }
    }
    if (tage_prediction_info.medium_confidence &&
        (std::abs(sc_prediction_info.gehls_sum) <
         sc_prediction_info.thresholds_sum / 4)) {
      first_high_confidence_ctr_.update(tage_or_loop_prediction == resolve_dir);
    }
  }

  if (sc_prediction != resolve_dir || std::abs(sc_prediction_info.gehls_sum) <
                                          sc_prediction_info.thresholds_sum) {
    // update threshold counters.
    if (sc_prediction != resolve_dir) {
      update_threshold_.increment();
      p_update_thresholds_.get_entry(br_pc).increment();
    } else {
      update_threshold_.decrement();
      p_update_thresholds_.get_entry(br_pc).decrement();
    }

    // update biases.
    int bias_table_index = get_bias_table_index(br_pc, tage_prediction_info,
                                                tage_or_loop_prediction);
    int bias_sk_table_index = get_bias_sk_table_index(
        br_pc, tage_prediction_info, tage_or_loop_prediction);
    int bias_bank_table_index = get_bias_bank_table_index(
        br_pc, tage_prediction_info, tage_or_loop_prediction);

    if (CONFIG::SC::USE_VARIABLE_THRESHOLD) {
      int biases_sum = 2 * bias_table_[bias_table_index].get() + 1;
      biases_sum += 2 * bias_sk_table_[bias_sk_table_index].get() + 1;
      biases_sum += 2 * bias_bank_table_[bias_bank_table_index].get() + 1;

      int gehls_sum_without_doubled_biases =
          sc_prediction_info.gehls_sum -
          (bias_threshold_table_.get_entry(br_pc).get() >= 0) * biases_sum;

      bool prediction_without_multiplier =
          (gehls_sum_without_doubled_biases >= 0);
      bool prediction_with_multiplier =
          (gehls_sum_without_doubled_biases + biases_sum >= 0);
      if (prediction_without_multiplier != prediction_with_multiplier) {
        bias_threshold_table_.get_entry(br_pc).update((biases_sum >= 0) ==
                                                      resolve_dir);
      }
    }
    bias_table_[bias_table_index].update(resolve_dir);
    bias_sk_table_[bias_sk_table_index].update(resolve_dir);
    bias_bank_table_[bias_bank_table_index].update(resolve_dir);

    update_gehl_and_threshold(
        &global_history_gehl_, &global_history_threshold_table_,
        (br_pc << 1) + (tage_or_loop_prediction ? 1 : 0),
        sc_prediction_info.history_snapshot.global_history, resolve_dir,
        sc_prediction_info.gehls_sum);
    update_gehl_and_threshold(&path_gehl_, &path_threshold_table_, br_pc,
                              sc_prediction_info.history_snapshot.path,
                              resolve_dir, sc_prediction_info.gehls_sum);

    if (CONFIG::SC::USE_LOCAL_HISTORY) {
      update_gehl_and_threshold(
          &first_local_gehl_, &first_local_threshold_table_, br_pc,
          sc_prediction_info.history_snapshot.first_local_history, resolve_dir,
          sc_prediction_info.gehls_sum);
      if (CONFIG::SC::USE_SECOND_LOCAL_HISTORY) {
        update_gehl_and_threshold(
            &second_local_gehl_, &second_local_threshold_table_, br_pc,
            sc_prediction_info.history_snapshot.second_local_history,
            resolve_dir, sc_prediction_info.gehls_sum);
      }
      if (CONFIG::SC::USE_THIRD_LOCAL_HISTORY) {
        update_gehl_and_threshold(
            &third_local_gehl_, &third_local_threshold_table_, br_pc,
            sc_prediction_info.history_snapshot.third_local_history,
            resolve_dir, sc_prediction_info.gehls_sum);
      }
    }

    if (CONFIG::SC::USE_IMLI) {
      update_gehl_and_threshold(
          &second_imli_gehl_, &second_imli_threshold_table_, br_pc,
          sc_prediction_info.history_snapshot.imli_local_history, resolve_dir,
          sc_prediction_info.gehls_sum);
      update_gehl_and_threshold(
          &first_imli_gehl_, &first_imli_threshold_table_, br_pc,
          sc_prediction_info.history_snapshot.imli_counter, resolve_dir,
          sc_prediction_info.gehls_sum);
    }
  }
}

template <class CONFIG>
void Statistical_Corrector<CONFIG>::update_speculative_state(
    uint64_t br_pc, bool resolve_dir, uint64_t br_target, Branch_Type br_type,
    SC_Prediction_Info* prediction_info) {
  prediction_info->history_snapshot.global_history = global_history_;
  prediction_info->history_snapshot.path = path_;
  if (CONFIG::SC::USE_LOCAL_HISTORY) {
    prediction_info->history_snapshot.first_local_history =
        first_local_history_table_.get_history(br_pc);
    if (CONFIG::SC::USE_SECOND_LOCAL_HISTORY) {
      prediction_info->history_snapshot.second_local_history =
          second_local_history_table_.get_history(br_pc);
    }
    if (CONFIG::SC::USE_THIRD_LOCAL_HISTORY) {
      prediction_info->history_snapshot.third_local_history =
          third_local_history_table_.get_history(br_pc);
    }
  }
  if (CONFIG::SC::USE_IMLI) {
    prediction_info->history_snapshot.imli_counter = imli_counter_.get();
    prediction_info->history_snapshot.imli_local_history =
        imli_table_[imli_counter_.get()];
  }

  if ((br_type.is_conditional) && CONFIG::SC::USE_IMLI) {
    int table_index = imli_counter_.get();
    imli_table_[table_index] = (imli_table_[table_index] << 1) + resolve_dir;
    if (br_target < br_pc) {
      // This branch corresponds to a loop
      if (!resolve_dir) {
        // exit of the "loop"
        imli_counter_.set(0);
      } else {
        imli_counter_.increment();
      }
    }
  }

  if (br_type.is_conditional) {
    global_history_ =
        (global_history_ << 1) + (resolve_dir & (br_target < br_pc));
    int64_t& first_local_history =
        first_local_history_table_.get_history(br_pc);
    first_local_history = (first_local_history << 1) + resolve_dir;

    int64_t& second_local_history =
        second_local_history_table_.get_history(br_pc);
    second_local_history =
        ((second_local_history << 1) + resolve_dir) ^ (br_pc & 15);

    int64_t& third_local_history =
        third_local_history_table_.get_history(br_pc);
    third_local_history = (third_local_history << 1) + resolve_dir;
  }

  // REVIST: redoing the path update already done in Tage. Tage and Sc
  // should probably share the same histories and TAGE_SC_L should be
  // responsible for updates.
  int num_bit_inserts = 2;
  if (br_type.is_conditional) {
    num_bit_inserts = 2;
  } else if ((br_type.is_indirect)) {  // unconditional indirect branches
    num_bit_inserts = 3;
  }
  int path_hash = br_pc ^ (br_pc >> 2) ^ (br_pc >> 4);
  if ((br_type.is_conditional && br_type.is_indirect) & resolve_dir) {
    path_hash = path_hash ^ (br_target >> 2) ^ (br_target >> 4);
  }

  for (int i = 0; i < num_bit_inserts; ++i) {
    path_ = (path_ << 1) ^ (path_hash & 127);
    path_hash >>= 1;
  }
  path_ = path_ & ((1 << CONFIG::SC::SC_PATH_HISTORY_WIDTH) - 1);
}

template <class CONFIG>
void Statistical_Corrector<CONFIG>::initialize_bias_tables(void) {
  int min_value = -(1 << (CONFIG::SC::PRECISION - 1));
  int max_value = (1 << (CONFIG::SC::PRECISION - 1)) - 1;
  for (int i = 0; i < (1 << CONFIG::SC::LOG_BIAS_ENTRIES); ++i) {
    switch (i & 3) {
      case 0:
        bias_table_[i].set(min_value);
        bias_bank_table_[i].set(min_value);
        bias_sk_table_[i].set(min_value / 4);
        break;
      case 1:
        bias_table_[i].set(max_value);
        bias_bank_table_[i].set(max_value);
        bias_sk_table_[i].set(max_value / 4);
        break;
      case 2:
        bias_table_[i].set(-1);
        bias_bank_table_[i].set(-1);
        bias_sk_table_[i].set(min_value);
        break;
      case 3:
        bias_table_[i].set(0);
        bias_bank_table_[i].set(0);
        bias_sk_table_[i].set(max_value);
        break;
    }
  }
}

template <class CONFIG>
int Statistical_Corrector<CONFIG>::get_threshold_table_index(uint64_t br_pc) {
  return (br_pc ^ (br_pc >> 2)) &
         ((1 << CONFIG::SC::LOG_SIZE_VARIABLE_THRESHOLD_TABLE) - 1);
}

template <class CONFIG>
int Statistical_Corrector<CONFIG>::get_bias_table_index(
    uint64_t br_pc,
    const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
    bool tage_or_loop_prediction) {
  int index = ((br_pc ^ (br_pc >> 2)) << 1);
  index ^= tage_prediction_info.low_confidence &
           (tage_prediction_info.longest_match_prediction !=
            tage_prediction_info.alt_prediction);
  index = (index << 1) + tage_or_loop_prediction;
  index = index & ((1 << CONFIG::SC::LOG_BIAS_ENTRIES) - 1);
  return index;
}

template <class CONFIG>
int Statistical_Corrector<CONFIG>::get_bias_sk_table_index(
    uint64_t br_pc,
    const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
    bool tage_or_loop_prediction) {
  int index = ((br_pc ^ (br_pc >> (CONFIG::SC::LOG_BIAS_ENTRIES - 2))) << 1);
  index ^= tage_prediction_info.high_confidence;
  index = (index << 1) + tage_or_loop_prediction;
  index = index & ((1 << CONFIG::SC::LOG_BIAS_ENTRIES) - 1);
  return index;
}

template <class CONFIG>
int Statistical_Corrector<CONFIG>::get_bias_bank_table_index(
    uint64_t br_pc,
    const Tage_Prediction_Info<typename CONFIG::TAGE>& tage_prediction_info,
    bool tage_or_loop_prediction) {
  int index = (br_pc ^ (br_pc >> 2)) << 7;
  index += ((tage_prediction_info.hit_bank + 1) / 4) << 4;
  index += (tage_prediction_info.alt_bank != 0) << 3;
  index += tage_prediction_info.low_confidence << 2;
  index += tage_prediction_info.high_confidence << 1;
  index += tage_or_loop_prediction;
  index = index & ((1 << CONFIG::SC::LOG_BIAS_ENTRIES) - 1);
  return index;
}

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_STATISTICAL_CORRECTOR_HPP_

#ifndef SPEC_TAGE_SC_L_TAGE_HPP_
#define SPEC_TAGE_SC_L_TAGE_HPP_

#include <cmath>
#include <vector>


namespace tagescl {

/* The main history register suitable for very large history. The history is
 * implemented as a circular buffer for efficiency. The API only allows
 * insertions of bits into the most recent position of the history and provides
 * an accessor for random access of individual bits. It also provides an API for
 * rewinding the history to support recovery from mispeculation */
template <int history_size>
class Long_History_Register {
 public:
  // Buffer_size needs to be a power of 2. (buffer_size - history_size) should
  // be large enough to cover speculative branches that are not yet retired.
  Long_History_Register(int max_in_flight_bits) : history_bits_() {
    int log_buffer_size =
        get_min_num_bits_to_represent(history_size + max_in_flight_bits);
    buffer_size_ = 1 << log_buffer_size;
    buffer_access_mask_ = (1 << log_buffer_size) - 1;
    max_num_speculative_bits_ = buffer_size_ - history_size;
    history_bits_.resize(buffer_size_);
  }

  // Pushes one bit into the history at the head. Increments
  // num_speculative_bits_
  // (saturated at max_num_speculative_bits_).
  void push_bit(bool bit) {
    // TODO: it will be cleaner to mask head_ with (size_ - 1) now. But I
    // want to keep it compatible with Seznec.
    head_ -= 1;
    history_bits_[head_ & buffer_access_mask_] = bit;

    num_speculative_bits_ += 1;
    assert(num_speculative_bits_ <= max_num_speculative_bits_);
  }

  // Rewinds num_rewind_bits branches out of the history.
  void rewind(int num_rewind_bits) {
    assert(num_rewind_bits > 0 && num_rewind_bits <= num_speculative_bits_);
    num_speculative_bits_ -= num_rewind_bits;
    head_ += num_rewind_bits;
  }

  // Retire speculative bits.
  void retire(int num_retire_bits) {
    assert(num_retire_bits > 0 && num_retire_bits <= num_speculative_bits_);
    num_speculative_bits_ -= num_retire_bits;
  }

  // Random access interface, i=0 is the most recent branch (head).
  bool operator[](size_t i) const {
    return history_bits_[(head_ + i) & buffer_access_mask_];
  }

  const int64_t& head_idx() const { return head_; }

  int64_t& head_idx() { return head_; }

 private:
  int num_speculative_bits_ = 0;  // keeps track of how many bits can be
                                  // discarded during a rewind without losing
                                  // bits in the most significant position.
  std::vector<bool> history_bits_;
  int64_t head_ = 0;
  int64_t buffer_size_;
  int64_t buffer_access_mask_;
  int64_t max_num_speculative_bits_;
};

/* Computes the a folded history of a large history, as bits are shifted into
 * the history. The caller should update the folded history everytime  */
template <int history_size>
class Folded_History {
 public:
  Folded_History(int original_length, int compressed_length)
      : current_value_(0),
        original_length_(original_length),
        compressed_length_(compressed_length),
        outpoint_(original_length % compressed_length) {}

  int64_t get_value() const { return current_value_; }

  void update(const Long_History_Register<history_size>& history_register) {
    // Shift in the most recent GHR bit.
    current_value_ = (current_value_ << 1) ^ history_register[0];

    // Shift out the least recent GHR bit.
    current_value_ ^= history_register[original_length_] << outpoint_;

    // Fold shifted-out bit in.
    current_value_ ^= current_value_ >> compressed_length_;

    // Mask out the unused bits.
    current_value_ &= (1 << compressed_length_) - 1;
  }

  void update_reverse(
      const Long_History_Register<history_size>& history_register) {
    // Fold out the most recent GHR bit.
    current_value_ ^= history_register[0];

    // Fold out the least recent GHR bit.
    current_value_ ^= history_register[original_length_] << outpoint_;

    // Rotate the low bit around to high bit.
    current_value_ = ((current_value_ & 1) << (compressed_length_ - 1)) |
                     (current_value_ >> 1);

    // Mask out the unused bits.
    current_value_ &= (1 << compressed_length_) - 1;
  }

 private:
  int64_t current_value_;
  int original_length_;
  int compressed_length_;
  int outpoint_;
};

template <class TAGE_CONFIG>
struct Tage_History_Sizes {
  static constexpr int N = TAGE_CONFIG::NUM_HISTORIES;
  constexpr Tage_History_Sizes() : arr() {
    double max_history = static_cast<double>(TAGE_CONFIG::MAX_HISTORY_SIZE);
    double min_history = static_cast<double>(TAGE_CONFIG::MIN_HISTORY_SIZE);
    double min_max_ratio = max_history / min_history;

    for (int i = 0; i < N; ++i) {
      double geometric_power =
          static_cast<double>(i) / static_cast<double>(N - 1);
      double geometric_multiplier = pow(min_max_ratio, geometric_power);
      arr[i] = static_cast<int>(min_history * geometric_multiplier + 0.5);
    }
  }
  int arr[N];
};

template <class TAGE_CONFIG>
struct Tage_Tables_Enabled {
  static constexpr int N = TAGE_CONFIG::NUM_HISTORIES;
  constexpr Tage_Tables_Enabled() : arr() {
    // Use 2-way tables for the middle tables and use direct-mapped for
    // others.
    for (int i = 1; i <= 2 * N; ++i) {
      bool is_even_table = (i - 1) & 1;
      bool is_middle_table = (i >= TAGE_CONFIG::FIRST_2WAY_TABLE) &&
                             (i <= TAGE_CONFIG::LAST_2WAY_TABLE);
      arr[i] = is_even_table || is_middle_table;
    }

    // Eliminate some of the history sizes completely. From Seznec's
    // comments,
    // it has "very very marginal" effect on accruacy.
    arr[4] = false;
    arr[2 * N - 2] = false;
    arr[8] = false;
    arr[2 * N - 6] = false;
  }
  bool arr[2 * N + 1];
};

template <class TAGE_CONFIG>
struct Tage_Tag_Bits {
  static constexpr int N = TAGE_CONFIG::NUM_HISTORIES;
  constexpr Tage_Tag_Bits() : arr() {
    for (int i = 0; i < N; ++i) {
      if ((2 * i + 1) < TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE) {
        arr[i] = TAGE_CONFIG::SHORT_HISTORY_TAG_BITS;
      } else {
        arr[i] = TAGE_CONFIG::LONG_HISTORY_TAG_BITS;
      }
    }
  }
  int arr[N];
};

struct Bimodal_Output {
  bool prediction;
  bool confidence;
};

struct Matched_Table_Banks {
  int hit_bank;
  int alt_bank;
};

template <class TAGE_CONFIG>
class Tage_Histories {
 public:
  Tage_Histories(int max_in_flight_branches)
      : history_register_(3 * max_in_flight_branches) {
    path_history_ = 0;
    intialize_folded_history();
  }

  void push_into_history(uint64_t br_pc, uint64_t br_target,
                         Branch_Type br_type, bool branch_dir,
                         Tage_Prediction_Info<TAGE_CONFIG>* prediction_info) {
    int num_bit_inserts = 2;
    if (br_type.is_indirect && !br_type.is_conditional) {
      num_bit_inserts = 3;
    }

    int pc_dir_hash = ((br_pc ^ (br_pc >> 2))) ^ branch_dir;
    int path_hash = br_pc ^ (br_pc >> 2) ^ (br_pc >> 4);
    if ((br_type.is_indirect && br_type.is_conditional) & branch_dir) {
      pc_dir_hash = (pc_dir_hash ^ (br_target >> 2));
      path_hash = path_hash ^ (br_target >> 2) ^ (br_target >> 4);
    }

    prediction_info->num_global_history_bits = num_bit_inserts;
    prediction_info->path_history_checkpoint = path_history_;
    prediction_info->global_history_head_checkpoint_ =
        history_register_.head_idx();

    for (int i = 0; i < num_bit_inserts; ++i) {
      history_register_.push_bit(pc_dir_hash & 1);
      pc_dir_hash >>= 1;

      path_history_ = (path_history_ << 1) ^ (path_hash & 127);
      path_hash >>= 1;

      for (int j = 0; j < TAGE_CONFIG::NUM_HISTORIES; ++j) {
        folded_histories_for_indices_[j].update(history_register_);
        folded_histories_for_tags_0_[j].update(history_register_);
        folded_histories_for_tags_1_[j].update(history_register_);
      }
    }

    path_history_ =
        path_history_ & ((1 << TAGE_CONFIG::PATH_HISTORY_WIDTH) - 1);
  }

  void intialize_folded_history(void);

  // Hash function for the path history used in creating table indices.
  int64_t compute_path_hash(int64_t path_history, int max_width, int bank,
                            int index_size) const;

  // Derived constants
  static constexpr int twice_num_histories_ = 2 * TAGE_CONFIG::NUM_HISTORIES;
  static constexpr Tage_History_Sizes<TAGE_CONFIG> history_sizes_ = {};
  static constexpr Tage_Tag_Bits<TAGE_CONFIG> tag_bits_ = {};

  // Predictor State
  Long_History_Register<TAGE_CONFIG::MAX_HISTORY_SIZE> history_register_;
  std::vector<Folded_History<TAGE_CONFIG::MAX_HISTORY_SIZE>>
      folded_histories_for_indices_;
  std::vector<Folded_History<TAGE_CONFIG::MAX_HISTORY_SIZE>>
      folded_histories_for_tags_0_;
  std::vector<Folded_History<TAGE_CONFIG::MAX_HISTORY_SIZE>>
      folded_histories_for_tags_1_;

  int64_t path_history_;
};

template <class TAGE_CONFIG>
class Tage {
 public:
  Tage(Random_Number_Generator& random_number_gen, int max_in_flight_branches)
      : tagged_table_ptrs_(),
        tage_histories_(max_in_flight_branches),
        low_history_tagged_table_(),
        high_history_tagged_table_(),
        alt_selector_table_(),
        random_number_gen_(random_number_gen) {
    initialize_table_sizes();
    intialize_predictor_state();
  }

  void get_prediction(
      uint64_t br_pc,
      Tage_Prediction_Info<TAGE_CONFIG>* prediction_info) const {
    // std::cerr << "history: " << tage_histories_.path_history_ << " "
    //           << tage_histories_.history_register_.head_idx() << "\n";

    fill_table_indices_tags(br_pc, prediction_info);
    auto& indices = prediction_info->indices;
    auto& tags = prediction_info->tags;

    // First use the bimodal table to make an initial prediction.
    Bimodal_Output bimodal_output = get_bimodal_prediction_confidence(br_pc);
    prediction_info->alt_prediction = bimodal_output.prediction;
    prediction_info->alt_confidence = bimodal_output.confidence;
    prediction_info->high_confidence = prediction_info->alt_confidence;
    prediction_info->medium_confidence = false;
    prediction_info->low_confidence = !prediction_info->high_confidence;
    prediction_info->prediction = prediction_info->alt_prediction;
    prediction_info->longest_match_prediction = prediction_info->alt_prediction;

    // Find matching tagged tables and update prediction and alternate
    // prediction
    // if necessary.

    Matched_Table_Banks matched_banks =
        get_two_longest_matching_tables(indices, tags);
    prediction_info->hit_bank = matched_banks.hit_bank;
    prediction_info->alt_bank = matched_banks.alt_bank;
    if (prediction_info->hit_bank != 0) {
      int8_t longest_match_counter =
          tagged_table_ptrs_[prediction_info->hit_bank]
                            [indices[prediction_info->hit_bank]]
                                .pred_counter.get();
      prediction_info->longest_match_prediction = longest_match_counter >= 0;
      if (prediction_info->alt_bank != 0) {
        int8_t alt_match_counter =
            tagged_table_ptrs_[prediction_info->alt_bank]
                              [indices[prediction_info->alt_bank]]
                                  .pred_counter.get();
        prediction_info->alt_prediction = alt_match_counter >= 0;
        prediction_info->alt_confidence =
            std::abs(2 * alt_match_counter + 1) > 1;
      }

      int alt_selector_table_index =
          (((prediction_info->hit_bank - 1) / 8) << 1) +
          (prediction_info->alt_confidence ? 1 : 0);
      alt_selector_table_index =
          alt_selector_table_index %
          ((1 << TAGE_CONFIG::ALT_SELECTOR_LOG_TABLE_SIZE) - 1);
      bool use_alt = alt_selector_table_[alt_selector_table_index].get() >= 0;
      if ((!use_alt) || std::abs(2 * longest_match_counter + 1) > 1) {
        prediction_info->prediction = prediction_info->longest_match_prediction;
      } else {
        prediction_info->prediction = prediction_info->alt_prediction;
      }

      // REVISIT: this seems buggy, only works for COUNTER_BITS = 3
      prediction_info->high_confidence =
          std::abs(2 * longest_match_counter + 1) >=
          ((1 << TAGE_CONFIG::PRED_COUNTER_WIDTH) - 1);
      prediction_info->medium_confidence =
          std::abs(2 * longest_match_counter + 1) == 5;
      prediction_info->low_confidence =
          std::abs(2 * longest_match_counter + 1) == 1;
    }
  }

  void update_speculative_state(
      uint64_t br_pc, uint64_t br_target, Branch_Type br_type,
      bool final_prediction,
      Tage_Prediction_Info<TAGE_CONFIG>* prediction_info) {
    tage_histories_.push_into_history(br_pc, br_target, br_type,
                                      final_prediction, prediction_info);
  }

  void commit_state(uint64_t br_pc, bool resolve_dir,
                    const Tage_Prediction_Info<TAGE_CONFIG>& prediction_info,
                    bool final_prediction) {
    const int* indices = prediction_info.indices;
    const int* tags = prediction_info.tags;

    bool allocate_new_entry =
        (prediction_info.prediction != resolve_dir) &&
        (prediction_info.hit_bank <
         Tage_Histories<TAGE_CONFIG>::twice_num_histories_);

    if (prediction_info.hit_bank > 0) {
      // Manage the selection between longest matching and alternate
      // matching
      // for "pseudo"-newly allocated longest matching entry.
      // This is extremely important for TAGE only, not that important
      // when the
      // overall predictor is implemented.
      // An entry is considered as newly allocated if its prediction
      // counter is
      // weak.
      Tagged_Entry& matched_entry =
          tagged_table_ptrs_[prediction_info.hit_bank]
                            [indices[prediction_info.hit_bank]];
      if (std::abs(2 * matched_entry.pred_counter.get() + 1) <= 1) {
        if (prediction_info.longest_match_prediction == resolve_dir) {
          // If it was delivering the correct prediction, no need to
          // allocate a
          // new entry even if the overall prediction was false.
          allocate_new_entry = false;
        }

        if (prediction_info.longest_match_prediction !=
            prediction_info.alt_prediction) {
          int alt_selector_table_index =
              (((prediction_info.hit_bank - 1) / 8) << 1);
          alt_selector_table_index += prediction_info.alt_confidence ? 1 : 0;
          alt_selector_table_index =
              alt_selector_table_index %
              ((1 << TAGE_CONFIG::ALT_SELECTOR_LOG_TABLE_SIZE) - 1);

          alt_selector_table_[alt_selector_table_index].update(
              prediction_info.alt_prediction == resolve_dir);
        }
      }
    }

    if (final_prediction == resolve_dir) {
      if ((random_number_gen_() & 31) != 0) {
        allocate_new_entry = false;
      }
    }

    if (allocate_new_entry) {
      int num_extra_entries_to_allocate =
          TAGE_CONFIG::EXTRA_ENTRIES_TO_ALLOCATE;
      int tick_penalty = 0;
      int num_allocated = 0;

      int temp_value = 1;
      if ((random_number_gen_() & 127) < 32) {
        temp_value = 2;
      }
      int allocation_bank =
          ((((prediction_info.hit_bank - 1 + 2 * temp_value) & 0xffe)) ^
           (random_number_gen_() & 1));
      // std::cerr << "allocate 0x" << std::hex << br_pc << std::dec << " "
      //           << allocation_bank << "\n";

      for (;
           allocation_bank < Tage_Histories<TAGE_CONFIG>::twice_num_histories_;
           allocation_bank += 2) {
        int i = allocation_bank + 1;  // REVISIT: is i needed?
        bool done = false;
        if (tables_enabled_.arr[i]) {
          Tagged_Entry& bank_entry = tagged_table_ptrs_[i][indices[i]];
          if (bank_entry.useful.get() == 0) {
            if (std::abs(2 * bank_entry.pred_counter.get() + 1) <= 3) {
              // std::cerr << "  inside [" << i << "][" << indices[i] << "]\n";
              // std::cerr << "  tag=" << tags[i] << "\n";
              // std::cerr << "  dir=" << ("01"[resolve_dir]) << "\n";
              bank_entry.tag = tags[i];
              bank_entry.pred_counter.set(resolve_dir ? 0 : -1);
              num_allocated += 1;
              if (num_extra_entries_to_allocate <= 0) {
                break;
              }
              allocation_bank += 2;
              done = true;
              num_extra_entries_to_allocate -= 1;
            } else {
              if (bank_entry.pred_counter.get() > 0) {
                bank_entry.pred_counter.decrement();
              } else {
                bank_entry.pred_counter.increment();
              }
            }
          } else {
            tick_penalty += 1;
          }
        }

        // REVISIT: this the repeat of the code above on a different
        // bank. The
        // code should be abstracted in a function.
        if (!done) {
          i = (allocation_bank ^ 1) + 1;
          if (tables_enabled_.arr[i]) {
            Tagged_Entry& bank_entry = tagged_table_ptrs_[i][indices[i]];

            if (bank_entry.useful.get() == 0) {
              if (std::abs(2 * bank_entry.pred_counter.get() + 1) <= 3) {
                bank_entry.tag = tags[i];
                bank_entry.pred_counter.set(resolve_dir ? 0 : -1);
                num_allocated += 1;
                if (num_extra_entries_to_allocate <= 0) {
                  break;
                }
                allocation_bank += 2;
                num_extra_entries_to_allocate -= 1;
              } else {
                if (bank_entry.pred_counter.get() > 0) {
                  bank_entry.pred_counter.decrement();
                } else {
                  bank_entry.pred_counter.increment();
                }
              }
            } else {
              tick_penalty += 1;
            }
          }
        }
      }

      tick_ += (tick_penalty - 2 * num_allocated);
      tick_ = std::max(tick_, 0);
      if (tick_ >= TAGE_CONFIG::TICKS_UNTIL_USEFUL_SHIFT) {
        shift_tage_useful_bits(low_history_tagged_table_,
                               TAGE_CONFIG::SHORT_HISTORY_NUM_BANKS *
                                   (1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK));
        shift_tage_useful_bits(high_history_tagged_table_,
                               TAGE_CONFIG::LONG_HISTORY_NUM_BANKS *
                                   (1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK));
        tick_ = 0;
      }
    }

    // Update prediction
    if (prediction_info.hit_bank > 0) {
      Tagged_Entry& matched_entry =
          tagged_table_ptrs_[prediction_info.hit_bank]
                            [indices[prediction_info.hit_bank]];
      if (std::abs(2 * matched_entry.pred_counter.get() + 1) == 1) {
        if (prediction_info.longest_match_prediction !=
            resolve_dir) {  // acts as a protection
          if (prediction_info.alt_bank > 0) {
            Tagged_Entry& alt_matched_entry =
                tagged_table_ptrs_[prediction_info.alt_bank]
                                  [indices[prediction_info.alt_bank]];
            alt_matched_entry.pred_counter.update(resolve_dir);
          } else {
            update_bimodal(br_pc, resolve_dir);
          }
        }
      }

      matched_entry.pred_counter.update(resolve_dir);
      // sign changes: no way it can have been useful
      if (std::abs(2 * matched_entry.pred_counter.get() + 1) == 1) {
        matched_entry.useful.set(0);
      }
      if (prediction_info.alt_prediction == resolve_dir &&
          prediction_info.alt_bank > 0) {
        Tagged_Entry& alt_matched_entry =
            tagged_table_ptrs_[prediction_info.alt_bank]
                              [indices[prediction_info.alt_bank]];
        if (std::abs(2 * alt_matched_entry.pred_counter.get() + 1) == 7 &&
            matched_entry.useful.get() == 1 &&
            prediction_info.longest_match_prediction == resolve_dir) {
          matched_entry.useful.set(0);
        }
      }
    } else {
      update_bimodal(br_pc, resolve_dir);
    }

    if (prediction_info.longest_match_prediction !=
            prediction_info.alt_prediction &&
        prediction_info.longest_match_prediction == resolve_dir) {
      Tagged_Entry& matched_entry =
          tagged_table_ptrs_[prediction_info.hit_bank]
                            [indices[prediction_info.hit_bank]];
      matched_entry.useful.increment();
    }
  }

  void commit_state_at_retire(
      const Tage_Prediction_Info<TAGE_CONFIG>& prediction_info) {
    tage_histories_.history_register_.retire(
        prediction_info.num_global_history_bits);
  }

  void global_recover_speculative_state(
      const Tage_Prediction_Info<TAGE_CONFIG>& prediction_info) {
    int64_t num_flushed_bits =
        (prediction_info.global_history_head_checkpoint_ -
         tage_histories_.history_register_.head_idx());
    for (int i = 0; i < num_flushed_bits; ++i) {
      for (int j = 0; j < TAGE_CONFIG::NUM_HISTORIES; ++j) {
        tage_histories_.folded_histories_for_indices_[j].update_reverse(
            tage_histories_.history_register_);
        tage_histories_.folded_histories_for_tags_0_[j].update_reverse(
            tage_histories_.history_register_);
        tage_histories_.folded_histories_for_tags_1_[j].update_reverse(
            tage_histories_.history_register_);
      }
      tage_histories_.history_register_.rewind(1);
    }
    tage_histories_.path_history_ = prediction_info.path_history_checkpoint;
  }

  void local_recover_speculative_state(
      const Tage_Prediction_Info<TAGE_CONFIG>& prediction_info) {}

  static void build_empty_prediction(
      Tage_Prediction_Info<TAGE_CONFIG>* prediction_info) {
    *prediction_info = {};
  }

 private:
  struct Bimodal_Entry {
    int8_t hysteresis = 1;
    int8_t prediction = 0;
  };

  struct Tagged_Entry {
    Saturating_Counter<TAGE_CONFIG::PRED_COUNTER_WIDTH, true> pred_counter;
    Saturating_Counter<TAGE_CONFIG::USEFUL_BITS, false> useful;
    int tag = 0;

    Tagged_Entry() : pred_counter(0), useful(0) {}
  };

  void initialize_tag_bits(void);
  void initialize_table_sizes(void);
  void intialize_predictor_state(void);

  // Produce indices and tags for all Tagged table look-ups.
  void fill_table_indices_tags(
      uint64_t br_pc, Tage_Prediction_Info<TAGE_CONFIG>* tage_output) const;

  // Get the prediction and confidence of the bimodal table.
  Bimodal_Output get_bimodal_prediction_confidence(uint64_t br_pc) const;

  void update_bimodal(uint64_t br_pc, bool resolve_dir);

  // Get the banks IDs of matching tables with longest histories.
  // A bank of 0 means a match was not found.
  Matched_Table_Banks get_two_longest_matching_tables(int indices[],
                                                      int tags[]) const;

  void shift_tage_useful_bits(Tagged_Entry* table, int size);

  // Derived constants
  static constexpr Tage_Tables_Enabled<TAGE_CONFIG> tables_enabled_ = {};

  Tagged_Entry*
      tagged_table_ptrs_[Tage_Histories<TAGE_CONFIG>::twice_num_histories_ + 1];

  // Predictor State
  Tage_Histories<TAGE_CONFIG> tage_histories_;
  Bimodal_Entry bimodal_table_[1 << TAGE_CONFIG::BIMODAL_LOG_TABLES_SIZE];
  Tagged_Entry
      low_history_tagged_table_[TAGE_CONFIG::SHORT_HISTORY_NUM_BANKS *
                                (1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK)];
  Tagged_Entry
      high_history_tagged_table_[TAGE_CONFIG::LONG_HISTORY_NUM_BANKS *
                                 (1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK)];

  Saturating_Counter<TAGE_CONFIG::ALT_SELECTOR_ENTRY_WIDTH, true>
      alt_selector_table_[1 << TAGE_CONFIG::ALT_SELECTOR_LOG_TABLE_SIZE];
  int tick_;  // for resetting the useful bits

  Random_Number_Generator& random_number_gen_;
};

template <class TAGE_CONFIG>
constexpr Tage_History_Sizes<TAGE_CONFIG>
    Tage_Histories<TAGE_CONFIG>::history_sizes_;

template <class TAGE_CONFIG>
constexpr Tage_Tables_Enabled<TAGE_CONFIG> Tage<TAGE_CONFIG>::tables_enabled_;

template <class TAGE_CONFIG>
constexpr Tage_Tag_Bits<TAGE_CONFIG> Tage_Histories<TAGE_CONFIG>::tag_bits_;

template <class TAGE_CONFIG>
void Tage<TAGE_CONFIG>::initialize_table_sizes(void) {
  for (int i = 1; i < TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE; ++i) {
    tagged_table_ptrs_[i] = low_history_tagged_table_;
  }
  for (int i = TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE;
       i <= Tage_Histories<TAGE_CONFIG>::twice_num_histories_; ++i) {
    tagged_table_ptrs_[i] = high_history_tagged_table_;
  }
}

template <class TAGE_CONFIG>
void Tage_Histories<TAGE_CONFIG>::intialize_folded_history(void) {
  // insert dummy entries for the zeroth entry of the vector
  for (int i = 0; i < TAGE_CONFIG::NUM_HISTORIES; i++) {
    // For some reason I cannot pass LOG_ENTRIES_PER_BANK to emplace_bank()
    // directly (gcc complains that variable is undefined), do not know why.
    // REVISIT: since I got rid of LOG_ENTRIES_PER_BANK as a constant, this
    // should be fine now.
    const int LOG_ENTRIES_PER_BANK2 = TAGE_CONFIG::LOG_ENTRIES_PER_BANK;
    folded_histories_for_indices_.emplace_back(history_sizes_.arr[i],
                                               LOG_ENTRIES_PER_BANK2);
    folded_histories_for_tags_0_.emplace_back(history_sizes_.arr[i],
                                              tag_bits_.arr[i]);
    folded_histories_for_tags_1_.emplace_back(history_sizes_.arr[i],
                                              tag_bits_.arr[i] - 1);
  }
}

template <class TAGE_CONFIG>
void Tage<TAGE_CONFIG>::intialize_predictor_state(void) {
  tick_ = 0;
  random_number_gen_.phist_ptr_ = &tage_histories_.path_history_;
  random_number_gen_.ptghist_ptr_ =
      &tage_histories_.history_register_.head_idx();
}

template <class TAGE_CONFIG>
int64_t Tage_Histories<TAGE_CONFIG>::compute_path_hash(int64_t path_history,
                                                       int max_width, int bank,
                                                       int index_size) const {
  int64_t temp1, temp2;

  // truncate path history to index size.
  path_history = (path_history & ((1 << max_width) - 1));
  temp1 = (path_history & ((1 << index_size) - 1));

  // Take high part of path history and left rotate it by "bank" ammount
  // this is just to generate a unique hash for each bank
  temp2 = (path_history >> index_size);
  if (bank < index_size) {
    temp2 = ((temp2 << bank) & ((1 << index_size) - 1)) +
            (temp2 >> (index_size - bank));
  }

  // fold rotated high part of path into low part of path
  path_history = temp1 ^ temp2;

  // left rotate that chunk by "bank"
  if (bank < index_size) {
    path_history = ((path_history << bank) & ((1 << index_size) - 1)) +
                   (path_history >> (index_size - bank));
  }
  return path_history;
}

template <class TAGE_CONFIG>
void Tage<TAGE_CONFIG>::fill_table_indices_tags(
    uint64_t br_pc, Tage_Prediction_Info<TAGE_CONFIG>* output) const {
  // Generate tags and indices, ignore bank bits for now.
  for (int i = 1; i <= Tage_Histories<TAGE_CONFIG>::twice_num_histories_;
       i += 2) {
    if (tables_enabled_.arr[i] || tables_enabled_.arr[i + 1]) {
      int max_path_width =
          (tage_histories_.history_sizes_.arr[(i - 1) / 2] >
           TAGE_CONFIG::PATH_HISTORY_WIDTH)
              ? TAGE_CONFIG::PATH_HISTORY_WIDTH
              : tage_histories_.history_sizes_.arr[(i - 1) / 2];
      int64_t path_hash = tage_histories_.compute_path_hash(
          tage_histories_.path_history_, max_path_width, i,
          TAGE_CONFIG::LOG_ENTRIES_PER_BANK);
      int64_t index = br_pc;
      index ^= br_pc >> (std::abs(TAGE_CONFIG::LOG_ENTRIES_PER_BANK - i) + 1);
      index ^= tage_histories_.folded_histories_for_indices_[(i - 1) / 2]
                   .get_value();
      index ^= path_hash;
      output->indices[i] =
          index & ((1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK) - 1);

      int64_t tag = br_pc;
      tag ^=
          tage_histories_.folded_histories_for_tags_0_[(i - 1) / 2].get_value();
      tag ^=
          tage_histories_.folded_histories_for_tags_1_[(i - 1) / 2].get_value()
          << 1;
      output->tags[i] =
          tag & ((1 << tage_histories_.tag_bits_.arr[(i - 1) / 2]) - 1);

      output->tags[i + 1] = output->tags[i];
      output->indices[i + 1] =
          output->indices[i] ^
          (output->tags[i] & ((1 << TAGE_CONFIG::LOG_ENTRIES_PER_BANK) - 1));
    }
  }

  // Now add bank bits to the indices of high history tables.
  int temp = (br_pc ^
              (tage_histories_.path_history_ &
               ((int64_t(1)
                 << tage_histories_.history_sizes_
                        .arr[(TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE - 1) / 2]) -
                1))) %
             TAGE_CONFIG::LONG_HISTORY_NUM_BANKS;
  for (int i = TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE;
       i <= Tage_Histories<TAGE_CONFIG>::twice_num_histories_; ++i) {
    if (tables_enabled_.arr[i]) {
      output->indices[i] += (temp << TAGE_CONFIG::LOG_ENTRIES_PER_BANK);
      temp++;
      temp = temp % TAGE_CONFIG::LONG_HISTORY_NUM_BANKS;
    }
  }

  // Now add bank bits to the indices of low history tables.
  temp = (br_pc ^ (tage_histories_.path_history_ &
                   ((1 << tage_histories_.history_sizes_.arr[0]) - 1))) %
         TAGE_CONFIG::SHORT_HISTORY_NUM_BANKS;
  for (int i = 1; i <= TAGE_CONFIG::FIRST_LONG_HISTORY_TABLE - 1; ++i) {
    if (tables_enabled_.arr[i]) {
      output->indices[i] += (temp << TAGE_CONFIG::LOG_ENTRIES_PER_BANK);
      temp++;
      temp = temp % TAGE_CONFIG::SHORT_HISTORY_NUM_BANKS;
    }
  }
  // for (int i = 1; i <= Tage_Histories<TAGE_CONFIG>::twice_num_histories_; ++i) {
  //   std::cerr << "  " << i << ": " << output->indices[i] << " "
  //             << output->tags[i] << std::endl;
  // }
}

template <class TAGE_CONFIG>
Bimodal_Output Tage<TAGE_CONFIG>::get_bimodal_prediction_confidence(
    uint64_t br_pc) const {
  Bimodal_Output output;
  int index = (br_pc ^ (br_pc >> 2)) &
              ((1 << TAGE_CONFIG::BIMODAL_LOG_TABLES_SIZE) - 1);
  int8_t bimodal_output =
      (bimodal_table_[index].prediction << 1) +
      (bimodal_table_[index >> TAGE_CONFIG::BIMODAL_HYSTERESIS_SHIFT]
           .hysteresis);

  output.prediction = bimodal_table_[index].prediction > 0;
  output.confidence = (bimodal_output == 0) || (bimodal_output == 3);
  return output;
}

template <class TAGE_CONFIG>
void Tage<TAGE_CONFIG>::update_bimodal(uint64_t br_pc, bool resolve_dir) {
  int index = (br_pc ^ (br_pc >> 2)) &
              ((1 << TAGE_CONFIG::BIMODAL_LOG_TABLES_SIZE) - 1);
  int8_t bimodal_output =
      (bimodal_table_[index].prediction << 1) +
      (bimodal_table_[index >> TAGE_CONFIG::BIMODAL_HYSTERESIS_SHIFT]
           .hysteresis);
  if (resolve_dir && bimodal_output < 3) {
    bimodal_output += 1;
  } else if (!resolve_dir && bimodal_output > 0) {
    bimodal_output -= 1;
  }
  bimodal_table_[index].prediction = bimodal_output >> 1;
  bimodal_table_[index >> TAGE_CONFIG::BIMODAL_HYSTERESIS_SHIFT].hysteresis =
      (bimodal_output & 1);
}

template <class TAGE_CONFIG>
Matched_Table_Banks Tage<TAGE_CONFIG>::get_two_longest_matching_tables(
    int indices[], int tags[]) const {
  int first_match = 0;
  int second_match = 0;
  for (int i = 2 * TAGE_CONFIG::NUM_HISTORIES; i > 0; --i) {
    if (tables_enabled_.arr[i]) {
      if (tagged_table_ptrs_[i][indices[i]].tag == tags[i]) {
        if (first_match == 0) {
          first_match = i;
        } else {
          second_match = i;
          break;
        }
      }
    }
  }
  return Matched_Table_Banks{first_match, second_match};
}

template <class TAGE_CONFIG>
void Tage<TAGE_CONFIG>::shift_tage_useful_bits(Tagged_Entry* table, int size) {
  for (int i = 0; i < size; ++i) {
    table[i].useful.set(table[i].useful.get() >> 1);
  }
}

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_TAGE_HPP_

#ifndef SPEC_TAGE_SC_L_TAGESCL_HPP_
#define SPEC_TAGE_SC_L_TAGESCL_HPP_

#include <iostream>

namespace tagescl {

template <class CONFIG>
struct Tage_SC_L_Prediction_Info {
  Tage_Prediction_Info<typename CONFIG::TAGE> tage;
  Loop_Prediction_Info<typename CONFIG::LOOP> loop;
  SC_Prediction_Info sc;
  uint64_t br_pc;
  int rng_seed;
  bool tage_or_loop_prediction;
  bool final_prediction;
};

class Tage_SC_L_Base {
 public:
  virtual int64_t get_new_branch_id() = 0;
  virtual bool get_prediction(int64_t branch_id, uint64_t br_pc) = 0;
  virtual void update_speculative_state(int64_t branch_id, uint64_t br_pc,
                                        Branch_Type br_type, bool branch_dir,
                                        uint64_t br_target) = 0;
  virtual void commit_state(int64_t branch_id, uint64_t br_pc,
                            Branch_Type br_type, bool resolve_dir) = 0;
  virtual void commit_state_at_retire(int64_t branch_id, uint64_t br_pc,
                                      Branch_Type br_type, bool resolve_dir,
                                      uint64_t br_target) = 0;
  virtual void flush_branch_and_repair_state(int64_t branch_id, uint64_t br_pc,
                                             Branch_Type br_type,
                                             bool resolve_dir,
                                             uint64_t br_target) = 0;
};

/* Interface functions:
 *
 * warmup() a wrapper for updating predictor state during the warmup phase of a
 * simulation.
 *
 * predict_and_update() a wrapper for consecutive simultaneous prediction and
 * update that implement the idealistic algorithms without considering pipeline
 * requirements. (same as Championship Branch Prediction Interface)
 */
template <class CONFIG>
class Tage_SC_L : public Tage_SC_L_Base {
 public:
  Tage_SC_L(int max_in_flight_branches)
      : tage_(random_number_gen_, max_in_flight_branches),
        statistical_corrector_(),
        loop_predictor_(random_number_gen_),
        loop_predictor_beneficial_(-1),
        prediction_info_buffer_(max_in_flight_branches) {}

  // Gets a new branch_id for a new in-flight branch. The id remains valid
  // until
  // the branch is retired or flushed. The class internally maintains metadata
  // for each in-flight branch. The rest of the public functions in this class
  // need the id of a branch to work on.
  int64_t get_new_branch_id() override {
    int64_t branch_id = prediction_info_buffer_.allocate_back();
    auto& prediction_info = prediction_info_buffer_[branch_id];
    Tage<typename CONFIG::TAGE>::build_empty_prediction(&prediction_info.tage);
    Loop_Predictor<typename CONFIG::LOOP>::build_empty_prediction(
        &prediction_info.loop);
    // std::cerr << "get_new_branch_id() -> " << branch_id << "\n";
    return branch_id;
  }

  // It uses the speculative state of the predictor to generate a prediction.
  // Should be called before update_speculative_state.
  bool get_prediction(int64_t branch_id, uint64_t br_pc) override;

  // It updates the speculative state (e.g. to insert history bits in Tage's
  // global history register). For conditional branches, it should be called
  // after get_prediction() in the front-end of a pipeline. For unconditional
  // branches, it should be the only function called in the front-end.
  void update_speculative_state(int64_t branch_id, uint64_t br_pc,
                                Branch_Type br_type, bool branch_dir,
                                uint64_t br_target) override;

  // Invokes the default update algorithm for updating the predictor state.
  // Can
  // be called either at the end of execute or retire. Note that even though
  // updating at the end of execute is speculative, committing the state
  // cannot
  // be undone.
  void commit_state(int64_t branch_id, uint64_t br_pc, Branch_Type br_type,
                    bool resolve_dir) override;

  // Updates predictor states that are critical for algorithm correctness.
  // Thus, should always be called in the retire state and after
  // commit_state()
  // is called. branch_id is invalidated and should not be used anymore.
  void commit_state_at_retire(int64_t branch_id, uint64_t br_pc,
                              Branch_Type br_type, bool resolve_dir,
                              uint64_t br_target) override;

  // Flushes the branch and all branches that came after it and repairs the
  // speculative state of the predictor. It invalidated all branch_id of
  // flushed
  // branches.
  void flush_branch_and_repair_state(int64_t branch_id, uint64_t br_pc,
                                     Branch_Type br_type, bool resolve_dir,
                                     uint64_t br_target) override;
  void flush_branch(int64_t branch_id);

 private:
  Random_Number_Generator random_number_gen_;
  Tage<typename CONFIG::TAGE> tage_;
  Statistical_Corrector<CONFIG> statistical_corrector_;
  Loop_Predictor<typename CONFIG::LOOP> loop_predictor_;

  // Counter for choosing between Tage and Loop Predictor.
  Saturating_Counter<CONFIG::CONFIDENCE_COUNTER_WIDTH, true>
      loop_predictor_beneficial_;

  // Used for remembering necessary information gathered during prediction
  // that
  // are needed for update.
  Circular_Buffer<Tage_SC_L_Prediction_Info<CONFIG>> prediction_info_buffer_;
};

template <class CONFIG>
bool Tage_SC_L<CONFIG>::get_prediction(int64_t branch_id, uint64_t br_pc) {
  auto& prediction_info = prediction_info_buffer_[branch_id];
  // std::cerr << "get_prediction(" << branch_id << ", 0x" << std::hex << br_pc
  //           << std::dec << ")\n";

  // First, use Tage to make a prediction.
  tage_.get_prediction(br_pc, &prediction_info.tage);
  prediction_info.tage_or_loop_prediction = prediction_info.tage.prediction;

  if (CONFIG::USE_LOOP_PREDICTOR) {
    // Then, look up the loop predictor and override Tage's prediction if
    // the
    // loop predictor is found to be beneficial.
    loop_predictor_.get_prediction(br_pc, &prediction_info.loop);
    if (loop_predictor_beneficial_.get() >= 0 && prediction_info.loop.valid) {
      prediction_info.tage_or_loop_prediction = prediction_info.loop.prediction;
    }
  }

  if (!CONFIG::USE_SC) {
    prediction_info.final_prediction = prediction_info.tage_or_loop_prediction;
  } else {
    statistical_corrector_.get_prediction(
        br_pc, prediction_info.tage, prediction_info.tage_or_loop_prediction,
        &prediction_info.sc);
    prediction_info.final_prediction = prediction_info.sc.prediction;
  }
  // std::cerr << " prediction=" << prediction_info.final_prediction << "\n";
  return prediction_info.final_prediction;
}

template <class CONFIG>
void Tage_SC_L<CONFIG>::commit_state(int64_t branch_id, uint64_t br_pc,
                                     Branch_Type br_type, bool resolve_dir) {
  if (!br_type.is_conditional) {
    return;
  }
  auto& prediction_info = prediction_info_buffer_[branch_id];
  if (CONFIG::USE_SC) {
    statistical_corrector_.commit_state(
        br_pc, resolve_dir, prediction_info.tage, prediction_info.sc,
        prediction_info.tage_or_loop_prediction);
  }

  if (CONFIG::USE_LOOP_PREDICTOR) {
    if (prediction_info.loop.valid) {
      if (prediction_info.final_prediction != prediction_info.loop.prediction) {
        loop_predictor_beneficial_.update(resolve_dir ==
                                          prediction_info.loop.prediction);
      }
    }
    loop_predictor_.commit_state(
        br_pc, resolve_dir, prediction_info.loop,
        prediction_info.final_prediction != resolve_dir,
        prediction_info.tage.prediction);
    loop_predictor_.commit_state_at_retire(
        br_pc, resolve_dir, prediction_info.loop,
        prediction_info.final_prediction != resolve_dir,
        prediction_info.tage.prediction);
  }

  tage_.commit_state(br_pc, resolve_dir, prediction_info.tage,
                     prediction_info.final_prediction);
}

template <class CONFIG>
void Tage_SC_L<CONFIG>::flush_branch(int64_t branch_id) {
  // std::cerr << "flush_branch_and_repair_state(" << branch_id << ", 0x"
  //           << std::hex << br_pc << std::dec << ", " << resolve_dir << ")\n";
  // First iterate over all flushed branches from youngest to oldest and
  // call local recovery functions.
  for (int64_t id = prediction_info_buffer_.back_id(); id >= branch_id; --id) {
    auto& prediction_info = prediction_info_buffer_[id];
    tage_.local_recover_speculative_state(prediction_info.tage);
    if (CONFIG::USE_LOOP_PREDICTOR) {
      loop_predictor_.local_recover_speculative_state(prediction_info.loop);
    }
    if (CONFIG::USE_SC) {
      statistical_corrector_.local_recover_speculative_state(
          prediction_info.br_pc, prediction_info.sc);
    }
  }
  
  auto& prediction_info = prediction_info_buffer_[branch_id];
  prediction_info_buffer_.deallocate_and_after(branch_id);

  // Now call global recovery functions.
  tage_.global_recover_speculative_state(prediction_info.tage);
  if (CONFIG::USE_LOOP_PREDICTOR) {
    loop_predictor_.global_recover_speculative_state(prediction_info.loop);
  }
  if (CONFIG::USE_SC) {
    statistical_corrector_.global_recover_speculative_state(prediction_info.sc);
  }

  random_number_gen_.seed_ = prediction_info.rng_seed;

 /* tage_.commit_state_at_retire(prediction_info.tage);
  if (CONFIG::USE_SC) {
    statistical_corrector_.commit_state_at_retire();
  }
  prediction_info_buffer_.deallocate_front(branch_id);*/
}

template <class CONFIG>
void Tage_SC_L<CONFIG>::flush_branch_and_repair_state(int64_t branch_id,
                                                      uint64_t br_pc,
                                                      Branch_Type br_type,
                                                      bool resolve_dir,
                                                      uint64_t br_target) {
  // std::cerr << "flush_branch_and_repair_state(" << branch_id << ", 0x"
  //           << std::hex << br_pc << std::dec << ", " << resolve_dir << ")\n";
  // First iterate over all flushed branches from youngest to oldest and
  // call local recovery functions.
  for (int64_t id = prediction_info_buffer_.back_id(); id >= branch_id; --id) {
    auto& prediction_info = prediction_info_buffer_[id];
    tage_.local_recover_speculative_state(prediction_info.tage);
    if (CONFIG::USE_LOOP_PREDICTOR) {
      loop_predictor_.local_recover_speculative_state(prediction_info.loop);
    }
    if (CONFIG::USE_SC) {
      statistical_corrector_.local_recover_speculative_state(
          prediction_info.br_pc, prediction_info.sc);
    }
  }
  prediction_info_buffer_.deallocate_after(branch_id);

  // Now call global recovery functions.
  auto& prediction_info = prediction_info_buffer_[branch_id];
  tage_.global_recover_speculative_state(prediction_info.tage);
  if (CONFIG::USE_LOOP_PREDICTOR) {
    loop_predictor_.global_recover_speculative_state(prediction_info.loop);
  }
  if (CONFIG::USE_SC) {
    statistical_corrector_.global_recover_speculative_state(prediction_info.sc);
  }

  random_number_gen_.seed_ = prediction_info.rng_seed;

  // Finally, update the speculative histories again using the resolved
  // direction of the branch.
  tage_.update_speculative_state(br_pc, br_target, br_type, resolve_dir,
                                 &prediction_info.tage);
  if (CONFIG::USE_LOOP_PREDICTOR) {
    loop_predictor_.update_speculative_state(prediction_info.loop);
  }
  if (CONFIG::USE_SC) {
    statistical_corrector_.update_speculative_state(
        br_pc, resolve_dir, br_target, br_type, &prediction_info.sc);
  }
}

template <class CONFIG>
void Tage_SC_L<CONFIG>::commit_state_at_retire(int64_t branch_id,
                                               uint64_t br_pc,
                                               Branch_Type br_type,
                                               bool resolve_dir,
                                               uint64_t br_target) {
  auto& prediction_info = prediction_info_buffer_[branch_id];
  // if (CONFIG::USE_LOOP_PREDICTOR) {
  //  loop_predictor_.commit_state_at_retire(
  //      br_pc, resolve_dir, prediction_info.loop,
  //      prediction_info.final_prediction != resolve_dir,
  //      prediction_info.tage.prediction);
  //}
  tage_.commit_state_at_retire(prediction_info.tage);
  if (CONFIG::USE_SC) {
    statistical_corrector_.commit_state_at_retire();
  }
  prediction_info_buffer_.deallocate_front(branch_id);
}

template <class CONFIG>
void Tage_SC_L<CONFIG>::update_speculative_state(int64_t branch_id,
                                                 uint64_t br_pc,
                                                 Branch_Type br_type,
                                                 bool branch_dir,
                                                 uint64_t br_target) {
  auto& prediction_info = prediction_info_buffer_[branch_id];
  prediction_info.rng_seed = random_number_gen_.seed_;
  tage_.update_speculative_state(br_pc, br_target, br_type, branch_dir,
                                 &prediction_info.tage);
  if (CONFIG::USE_LOOP_PREDICTOR) {
    loop_predictor_.update_speculative_state(prediction_info.loop);
  }
  if (CONFIG::USE_SC) {
    statistical_corrector_.update_speculative_state(
        br_pc, branch_dir, br_target, br_type, &prediction_info.sc);
  }
  prediction_info.br_pc = br_pc;
}

}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_TAGESCL_HPP_


class TAGE_EMILIO: public BPredUnit
{
  private:
    tagescl::Tage_SC_L<tagescl::CONFIG_64KB> tage;

  protected:
    virtual bool predict(ThreadID tid, Addr branch_pc, bool cond_branch,
                         void* &b);

    struct TageEmilioBranchInfo
    {
        int id;
        Addr pc;
        tagescl::Branch_Type br_type;
        TageEmilioBranchInfo()
        {}
    };

  public:

    TAGE_EMILIO(const TAGE_EMILIOParams &params);

    // Base class methods.
    bool lookup(ThreadID tid, Addr pc, void* &bp_history) override;
    void updateHistories(ThreadID tid, Addr pc, bool uncond, bool taken,
                         Addr target,  void * &bp_history) override;
    void update(ThreadID tid, Addr pc, bool taken,
                void * &bp_history, bool squashed,
                const StaticInstPtr & inst, Addr target) override;
    virtual void squash(ThreadID tid, void * &bp_history) override;
};

} // namespace branch_prediction
} // namespace gem5

#endif // __CPU_PRED_TAGE_EMILIO_HH__
