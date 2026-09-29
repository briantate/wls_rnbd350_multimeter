---
name: bme280-driver-generation
description: Generate the BME280 temperature-only driver for the Production Temperature Controller using retrieval-augmented generation. Use when implementing or refactoring the bme280_driver.{h,c} files. Retrieves the BME280 datasheet sections for chip-id, calibration, ctrl_meas, status, force-mode, data registers, and compensation formula before generating; cites every hardware-specific decision in retrieval_log.md; refuses to fabricate register addresses, calibration math, or timing values.
---

# BME280 Driver Generation (Retrieval-Augmented)

This is the Lab 4 adaptation of the `rag-driver-generation` starter
skill. It is specific to the Production Temperature Controller's
BME280 driver. The contract is fixed: temperature only, force mode,
compatible with the scaffolding header from Lab 3.

## Input Contract

Required:

- A built ChromaDB index containing the BME280 datasheet and the
  STM32L4 HAL I2C reference. Default path: `vector-db/`,
  collection `bme280-driver`.
- The header file `firmware/scaffolding/include/bme280/bme280.h`.
  The implementation must match the signatures declared there.
- The HAL surface the driver calls: `hal_i2c.h`. The driver must not
  call ST's HAL directly; it goes through `hal_i2c_read` and
  `hal_i2c_write`.

Stop and ask if any of these is missing.

## Output Contract

Write to the working directory's `driver/` folder:

```
driver/
├── bme280_driver.h            Final header (matches Lab 3 contract)
├── bme280_driver.c            Implementation
├── retrieval_log.md           Citation log (see format below)
├── test_bme280_driver.c       Host-runnable test file
└── CMakeLists.txt             Build glue (compiles against host HAL)
```

The header contract is fixed: do not change function names, types,
or status enum values declared in `bme280.h`.

The retrieval log uses one section per decision:

```
### Decision: <one-line description>
- Code reference: <file>:<symbol>
- Retrieved chunk: <source>::chunk-<index>
- Quoted text: "<verbatim excerpt>"
- Grounding: <one or two sentences explaining why this excerpt
  supports this decision>
```

## Process

### 1. Confirm the contract

Read `firmware/scaffolding/include/bme280/bme280.h`. Note the function
signatures, the status enum, and the configuration struct. Do not
proceed if the header is missing.

### 2. Issue the BME280 retrievals (in order)

For each of the queries below, retrieve the top 5 chunks from the
index. Save the chunk IDs; you will cite them in the retrieval log.

| # | Query |
|---|-------|
| 1 | "BME280 chip_id read power-on-reset identification" |
| 2 | "BME280 calibration coefficients dig_T1 dig_T2 dig_T3 register addresses byte order signedness" |
| 3 | "BME280 ctrl_meas register oversampling and mode bitfield layout" |
| 4 | "BME280 status register measuring bit polling" |
| 5 | "BME280 force mode trigger one-shot measurement" |
| 6 | "BME280 temperature data registers raw 20-bit ADC msb lsb xlsb" |
| 7 | "BME280 temperature compensation formula t_fine integer arithmetic" |
| 8 | "BME280 7-bit I2C address SDO pin selection" |

### 3. Issue the HAL retrievals

| # | Query |
|---|-------|
| 9  | "STM32 HAL I2C 8-bit shifted device address convention" |
| 10 | "HAL_I2C_Mem_Read parameters memory address size return values" |

### 4. Reconcile retrievals

Read every retrieved chunk before generating any code. If two
chunks contradict each other, the datasheet wins for hardware facts;
the HAL reference wins for HAL conventions.

### 5. Generate the driver

Write `bme280_driver.h` matching the scaffolding contract verbatim, plus the
implementation in `bme280_driver.c`. Every line that depends on a
retrieved fact carries a comment of the form
`/* [retrieval-log: <decision-id>] */`.

Apply the project's `coding-standards` skill throughout: header
guards in `TEMPCTL_BME280_DRIVER_H_` form, four-space indent,
braces on the same line, Doxygen comments on public functions, no
direct register access.

### 6. Generate the retrieval log

Write `retrieval_log.md`. Every hardware-specific decision goes in.
At minimum, expect one entry for: each register address, the
calibration register block layout, the ctrl_meas bitfield layout,
the status register semantics, the compensation formula, the I2C
address, and ST's 8-bit shifted-address convention.

### 7. Generate tests

Write `test_bme280_driver.c` with at least:

- `test_bme280_init_validates_chip_id` --- chip-id mismatch returns
  `BME280_STATUS_BAD_CHIP_ID`.
- `test_bme280_init_loads_calibration` --- after init, the cached
  calibration values match a known fixture.
- `test_bme280_read_temperature_compensation` --- compensation
  arithmetic produces the documented reference output (datasheet
  Appendix A example: raw 0x7E0044, calib values from datasheet
  example, expected ~25.08 C). Cite the appendix.
- `test_bme280_read_temperature_propagates_bus_error` --- a
  forced HAL error causes the read to return `BME280_STATUS_BUS_ERROR`.

### 8. Verify the build

From `driver/`:

```
cmake -B build -DBUILD_FOR_HOST=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The driver compiles against the host HAL stubs and the four tests
pass.

### 9. Verify the citations

Open `retrieval_log.md` and re-query each cited chunk:

```
python scripts/query_index.py \
    --persist vector-db --top 1 \
    --query "<query that should retrieve the cited chunk>"
```

Each cited chunk must appear as a top result for at least one query.
If a citation does not match a retrievable chunk, the citation is
broken; remove it and re-run the relevant retrieval.

## Anti-patterns

- **Generating register addresses from memory.** The BME280 chip-id
  register (0xD0) happens to match Bosch's pattern, so an
  unconstrained AI will guess it correctly. The calibration block,
  ctrl_meas, and the temperature data registers are not as obvious.
  Always retrieve.
- **Fabricating the temperature compensation formula.** This is the
  highest-impact retrieval target. The formula in the datasheet uses
  specific shifts and integer widths; an AI generating from memory
  will produce arithmetic that compiles and silently outputs values
  off by 30 degrees C or more.
- **Polling the status.measuring bit on Renode.** Renode's BME280
  model returns ready immediately. Code that polls until the bit
  rises will hang. The driver should check the bit but treat the
  ready-immediately case as success, not as an error.
- **Mixing 7-bit and 8-bit I2C addresses.** The datasheet gives
  0x76 (7-bit). ST's HAL expects 0xEC (8-bit shifted). The HAL
  abstraction in this project hides the shift inside `hal_i2c_*`
  but the driver must know which form it is passing.
- **Implementing pressure or humidity reads.** Out of scope.
- **Citing without quoting.** Every retrieval-log entry must
  include a verbatim excerpt of the chunk.
