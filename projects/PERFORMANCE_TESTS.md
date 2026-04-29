# Performance Tests

Add an automated performance test system that can be run periodically to
detect regressions and measure improvements.

The system should run a stable set of representative simulator workloads,
record timing and throughput metrics, and compare results against recent
baselines. It should report meaningful changes without treating normal host
noise as a failure.

## Work

- Choose representative workloads for core simulators and I/O-heavy paths.
- Make each workload deterministic enough for repeated measurement.
- Capture runtime, throughput, and any simulator-specific counters that help
  explain changes.
- Store baselines by platform, compiler, build type, and relevant feature
  settings.
- Report statistically meaningful regressions and improvements.
- Make the suite easy to run locally and suitable for scheduled CI.

## Acceptance Criteria

- A periodic performance run can compare current results against a baseline.
- Reports identify likely regressions, improvements, and noisy measurements.
- The suite is documented well enough to add new workloads over time.
