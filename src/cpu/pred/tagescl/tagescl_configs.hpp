/* Copyright 2020 HPS/SAFARI Research Groups
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

#ifndef SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_
#define SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_

namespace tagescl {

struct CONFIG_8KB {
  static constexpr bool USE_LOOP_PREDICTOR = true;
  static constexpr bool USE_SC = true;
  static constexpr int CONFIDENCE_COUNTER_WIDTH = 7;

  struct TAGE {
    static constexpr int MIN_HISTORY_SIZE = 4;
    static constexpr int MAX_HISTORY_SIZE = 1000;
    static constexpr int NUM_HISTORIES = 18;
    static constexpr int PATH_HISTORY_WIDTH = 27;
    static constexpr int FIRST_LONG_HISTORY_TABLE = 11;
    static constexpr int FIRST_2WAY_TABLE = 9;
    static constexpr int LAST_2WAY_TABLE = 22;
    static constexpr int SHORT_HISTORY_TAG_BITS = 8;
    static constexpr int LONG_HISTORY_TAG_BITS = 12;
    static constexpr int PRED_COUNTER_WIDTH = 3;
    static constexpr int USEFUL_BITS = 2;
    static constexpr int LOG_ENTRIES_PER_BANK = 7;
    static constexpr int SHORT_HISTORY_NUM_BANKS = 9;
    static constexpr int LONG_HISTORY_NUM_BANKS = 17;
    static constexpr int EXTRA_ENTRIES_TO_ALLOCATE = 1;
    static constexpr int TICKS_UNTIL_USEFUL_SHIFT = 1024;
    static constexpr int ALT_SELECTOR_LOG_TABLE_SIZE = 4;
    static constexpr int ALT_SELECTOR_ENTRY_WIDTH = 5;
    static constexpr int BIMODAL_HYSTERESIS_SHIFT = 2;
    static constexpr int BIMODAL_LOG_TABLES_SIZE = 12;
  };

  struct LOOP {
    static constexpr int LOG_NUM_ENTRIES = 3;
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
    static constexpr int LOG_BIAS_ENTRIES = 7;

    // Global history GEHL — active, reduced for 8KB
    static constexpr int LOG_SIZE_GLOBAL_HISTORY_GEHL = 7;
    struct GLOBAL_HISTORY_GEHL_HISTORIES {
      static constexpr int arr[] = {6, 3};
    };

    // Path GEHL — REQUIRED by statistical_corrector.hpp (always instantiated).
    // Not used in 8KB design, so minimize to near-zero budget.
    static constexpr int LOG_SIZE_PATH_GEHL = 1;
    struct PATH_GEHL_HISTORIES {
      static constexpr int arr[] = {1};
    };

    // Local history — 1st only
    static constexpr bool USE_LOCAL_HISTORY = true;
    static constexpr int FIRST_LOCAL_HISTORY_LOG_TABLE_SIZE = 6;
    static constexpr int FIRST_LOCAL_HISTORY_SHIFT = 2;
    static constexpr int LOG_SIZE_FIRST_LOCAL_GEHL = 7;
    struct FIRST_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {6, 3};
    };

    // 2nd and 3rd local histories disabled via real guards in the code
    static constexpr bool USE_SECOND_LOCAL_HISTORY = false;
    static constexpr int SECOND_LOCAL_HISTORY_LOG_TABLE_SIZE = 1; // required for member instantiation
    static constexpr int SECOND_LOCAL_HISTORY_SHIFT = 1;
    static constexpr int LOG_SIZE_SECOND_LOCAL_GEHL = 1;
    struct SECOND_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {1};
    };

    static constexpr bool USE_THIRD_LOCAL_HISTORY = false;
    static constexpr int THIRD_LOCAL_HISTORY_LOG_TABLE_SIZE = 1; // required for member instantiation
    static constexpr int THIRD_LOCAL_HISTORY_SHIFT = 1;
    static constexpr int LOG_SIZE_THIRD_LOCAL_GEHL = 1;
    struct THIRD_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {1};
    };

    // IMLI — first GEHL only (active)
    static constexpr bool USE_IMLI = true;
    static constexpr int IMLI_COUNTER_WIDTH = 8;
    static constexpr int IMLI_TABLE_SIZE = 1 << IMLI_COUNTER_WIDTH;
    static constexpr int log_size_first_imli_gehl = 7;
    struct FIRST_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {IMLI_COUNTER_WIDTH};
    };

    // Second IMLI GEHL — REQUIRED by statistical_corrector.hpp (always instantiated).
    // Not used in 8KB design, so minimize to near-zero budget.
    static constexpr int LOG_SIZE_SECOND_IMLI_GEHL = 1;
    struct SECOND_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {1};
    };

    static constexpr int PRECISION = 6;
    static constexpr int SC_PATH_HISTORY_WIDTH = 27;
  };
};

constexpr int CONFIG_8KB::SC::GLOBAL_HISTORY_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::PATH_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::FIRST_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::SECOND_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::THIRD_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::FIRST_IMLI_GEHL_HISTORIES::arr[];
constexpr int CONFIG_8KB::SC::SECOND_IMLI_GEHL_HISTORIES::arr[];

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

struct CONFIG_128KB {
  // static constexpr bool PIPELINE_SUPPORT = true;
  static constexpr bool USE_LOOP_PREDICTOR = true;
  static constexpr bool USE_SC = true;
  // Confidence counter width unchanged — this is a per-entry bit width,
  // not a table size; widening it would change predictor semantics.
  static constexpr int CONFIDENCE_COUNTER_WIDTH = 7;

  struct TAGE {
    // History geometry unchanged — these define the geometric series of
    // history lengths, not storage directly.
    static constexpr int MIN_HISTORY_SIZE = 6;
    static constexpr int MAX_HISTORY_SIZE = 3000;
    static constexpr int NUM_HISTORIES = 18;
    static constexpr int PATH_HISTORY_WIDTH = 27;

    // Table boundary indices unchanged — bank geometry is preserved.
    static constexpr int FIRST_LONG_HISTORY_TABLE = 13;
    static constexpr int FIRST_2WAY_TABLE = 9;
    static constexpr int LAST_2WAY_TABLE = 22;

    // Tag widths unchanged — widening tags would change collision semantics,
    // not simply scale storage proportionally.
    static constexpr int SHORT_HISTORY_TAG_BITS = 8;
    static constexpr int LONG_HISTORY_TAG_BITS = 12;

    // Counter and useful-bit widths unchanged — semantic parameters.
    static constexpr int PRED_COUNTER_WIDTH = 3;
    static constexpr int USEFUL_BITS = 1;

    // KEY SCALING LEVER: +1 doubles all tagged table entries.
    // 64KB used 10 (1K entries/bank); 128KB uses 11 (2K entries/bank).
    // Total tagged storage: 30 banks * 2K * (3+8/12+1) bits ~ 2x of 64KB.
    static constexpr int LOG_ENTRIES_PER_BANK = 11;

    // Bank counts unchanged — preserving the short/long history geometry.
    static constexpr int SHORT_HISTORY_NUM_BANKS = 10;
    static constexpr int LONG_HISTORY_NUM_BANKS = 20;

    static constexpr int EXTRA_ENTRIES_TO_ALLOCATE = 1;
    static constexpr int TICKS_UNTIL_USEFUL_SHIFT = 1024;

    // Alt-selector table unchanged — very small, not a meaningful storage
    // contributor (2^4 * 5 bits = 80 bits).
    static constexpr int ALT_SELECTOR_LOG_TABLE_SIZE = 4;
    static constexpr int ALT_SELECTOR_ENTRY_WIDTH = 5;

    static constexpr int BIMODAL_HYSTERESIS_SHIFT = 2;
    // +1 doubles the bimodal base table: 2^13 -> 2^14 entries.
    static constexpr int BIMODAL_LOG_TABLES_SIZE = 14;
  };

  struct LOOP {
    // +1 doubles loop predictor entries: 2^5 -> 2^6 = 64 entries (4-way).
    static constexpr int LOG_NUM_ENTRIES = 6;
    // Iteration counter and tag widths unchanged — semantic parameters.
    static constexpr int ITERATION_COUNTER_WIDTH = 10;
    static constexpr int TAG_BITS = 10;
    static constexpr int CONFIDENCE_THRESHOLD = 15;
  };

  struct SC {
    // Threshold counter widths unchanged — these are per-counter bit widths,
    // not table sizes.
    static constexpr int UPDATE_THRESHOLD_WIDTH = 12;
    static constexpr int PERPC_UPDATE_THRESHOLD_WIDTH = 8;
    static constexpr int INITIAL_UPDATE_THRESHOLD = 35 << 3;

    static constexpr bool USE_VARIABLE_THRESHOLD = true;
    // +1 doubles the per-PC threshold table: 2^6 -> 2^7 = 128 entries.
    static constexpr int LOG_SIZE_PERPC_THRESHOLD_TABLE =
        USE_VARIABLE_THRESHOLD ? 7 : 0;
    // Derived as before: half of LOG_SIZE_PERPC_THRESHOLD_TABLE.
    static constexpr int LOG_SIZE_VARIABLE_THRESHOLD_TABLE =
        LOG_SIZE_PERPC_THRESHOLD_TABLE / 2;
    // Threshold counter widths unchanged — semantic parameters.
    static constexpr int VARIABLE_THRESHOLD_WIDTH = 6;
    static constexpr int INITIAL_VARIABLE_THRESHOLD = 7;
    static constexpr int INITIAL_VARIABLE_THRESHOLD_FOR_BIAS = 4;

    // +1 doubles the bias table: 2^8 -> 2^9 = 512 entries.
    static constexpr int LOG_BIAS_ENTRIES = 9;

    // Global history GEHL: +1 doubles table size (2^10 -> 2^11 = 2K entries).
    // History lengths unchanged — these are history taps, not storage.
    static constexpr int LOG_SIZE_GLOBAL_HISTORY_GEHL = 11;
    struct GLOBAL_HISTORY_GEHL_HISTORIES {
      static constexpr int arr[] = {40, 24, 10};
    };

    // Path GEHL: +1 doubles table size (2^9 -> 2^10 = 1K entries).
    static constexpr int LOG_SIZE_PATH_GEHL = 10;
    struct PATH_GEHL_HISTORIES {
      static constexpr int arr[] = {25, 16, 9};
    };

    // --- Local history tables ---
    static constexpr bool USE_LOCAL_HISTORY = true;
    // +1 doubles first local history table: 2^8 -> 2^9 = 512 entries.
    static constexpr int FIRST_LOCAL_HISTORY_LOG_TABLE_SIZE = 9;
    static constexpr int FIRST_LOCAL_HISTORY_SHIFT = 2;
    // +1 doubles first local GEHL: 2^10 -> 2^11 = 2K entries.
    static constexpr int LOG_SIZE_FIRST_LOCAL_GEHL = 11;
    struct FIRST_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {11, 6, 3};
    };

    static constexpr bool USE_SECOND_LOCAL_HISTORY = true;
    // +1 doubles second local history table: 2^4 -> 2^5 = 32 entries.
    static constexpr int SECOND_LOCAL_HISTORY_LOG_TABLE_SIZE = 5;
    static constexpr int SECOND_LOCAL_HISTORY_SHIFT = 5;
    // +1 doubles second local GEHL: 2^9 -> 2^10 = 1K entries.
    static constexpr int LOG_SIZE_SECOND_LOCAL_GEHL = 10;
    struct SECOND_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {16, 11, 6};
    };

    static constexpr bool USE_THIRD_LOCAL_HISTORY = true;
    // +1 doubles third local history table: 2^4 -> 2^5 = 32 entries.
    static constexpr int THIRD_LOCAL_HISTORY_LOG_TABLE_SIZE = 5;
    static constexpr int THIRD_LOCAL_HISTORY_SHIFT = 10;
    // +1 doubles third local GEHL: 2^10 -> 2^11 = 2K entries.
    static constexpr int LOG_SIZE_THIRD_LOCAL_GEHL = 11;
    struct THIRD_LOCAL_GEHL_HISTORIES {
      static constexpr int arr[] = {9, 4};
    };

    // --- IMLI ---
    static constexpr bool USE_IMLI = true;
    // +1 doubles IMLI table: 2^8 -> 2^9 = 512 entries.
    // IMLI_TABLE_SIZE is derived, so it scales automatically.
    static constexpr int IMLI_COUNTER_WIDTH = 9;
    static constexpr int IMLI_TABLE_SIZE = 1 << IMLI_COUNTER_WIDTH;
    // +1 doubles first IMLI GEHL: 2^8 -> 2^9 = 512 entries.
    static constexpr int log_size_first_imli_gehl = 9;
    struct FIRST_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {IMLI_COUNTER_WIDTH};
    };
    // +1 doubles second IMLI GEHL: 2^9 -> 2^10 = 1K entries.
    static constexpr int LOG_SIZE_SECOND_IMLI_GEHL = 10;
    struct SECOND_IMLI_GEHL_HISTORIES {
      static constexpr int arr[] = {10, 4};
    };

    // Precision and path history width unchanged — semantic parameters.
    static constexpr int PRECISION = 6;
    static constexpr int SC_PATH_HISTORY_WIDTH = 27;
  };
};

// Out-of-class constexpr array definitions (required pre-C++17)
constexpr int CONFIG_128KB::SC::GLOBAL_HISTORY_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::PATH_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::FIRST_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::SECOND_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::THIRD_LOCAL_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::FIRST_IMLI_GEHL_HISTORIES::arr[];
constexpr int CONFIG_128KB::SC::SECOND_IMLI_GEHL_HISTORIES::arr[];


}  // namespace tagescl

#endif  // SPEC_TAGE_SC_L_TAGESCL_CONFIGS_HPP_
