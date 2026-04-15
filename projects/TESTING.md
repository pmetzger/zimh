# Legacy code -> fully-tested codebase: change protocol for agents
*(Inspired by Michael Feathers' "Working Effectively with Legacy Code",
plus modern testing techniques.)*

## Non-negotiable rule
**Never change production code without first constructing an automated
test.**

- If you know the intended behavior: write a test that would fail
  without your change and pass with it.
- If you do not know the intended behavior: write a test that documents
  the current behavior first, then make the smallest change and
  update/extend tests to reflect the intended behavior.

This is the opposite of "edit code and then poke at it". The test suite
is the safety net that makes refactoring, redesign, and language
translation safe.

---

## The core loop (run this for every single change)
1. **Define the smallest change.**
   - One bug fix, one feature slice, one refactor step. Keep it tiny so
     failures are easy to diagnose.

2. **Choose the closest test boundary that can run fast and
   deterministically.**
   - Prefer calling a function/class directly.
   - If the code is deeply tangled, start at the nearest stable
     boundary above it (public API, CLI command, HTTP handler, job
     entrypoint) and write a coarse "characterize current behavior"
     test there first. That test becomes the safety net while you
     create finer-grained tests.

3. **Write the test first.**
   - If adding/changing behavior: make the test assert the new behavior
     and confirm it fails before code changes.
   - If refactoring or you're unsure what is correct: write a "current
     behavior" test that records what the code does today (outputs,
     returned values, emitted events, database writes via a fake, etc.).
     These tests are documentation of reality, not moral truth.

4. **If you cannot test the change point, create a "seam" before
   changing behavior.**
   A seam is a spot where you can change what happens by substituting
   something (a dependency, implementation, or collaborator) **without
   rewriting the code at the call site**.
   - Typical seams in modern code:
     - Constructor/function parameters (pass a fake instead of the real
       dependency).
     - Interfaces/protocols (swap implementation in tests).
     - Overridable factory/getter method (override in a test subclass).
     - Module-level indirection (replace a dependency via import
       injection / dependency container in tests).
   - Goal: **make the code runnable in a test harness** and allow
     substitution of slow/flaky side effects.

5. **Break dependencies only enough to get tests running.**
   - Keep dependency-breaking edits small and reversible.
   - After each small edit: run tests.

6. **Make the minimal production change.**
   - Then run the new test(s) and the full relevant suite.
   - If green: refactor in tiny steps, keeping tests green after each
     step.

7. **Expand tests until you have confidence.**
   - Add edge cases and assertions where failures would be expensive.
   - Prefer tests that "sense" the behavior you care about (clear
     assertions) and stay stable over time.

---

## Fast way to get a foothold: "construction test"
When a class/function is hard to test, don't guess. **Create a test
that only attempts to instantiate/call it.**

- The compiler/runtime failure tells you exactly what dependencies are
  blocking testability.
- Then you apply one dependency-breaking move at a time until the
  construction test can run, and you replace it with real behavior
  tests.

---

## Adding new tested code safely
Use these techniques when the legacy code is hard to touch and you need
to move forward without "hacking inside" untested code.

### Sprout method
**Motivation:** Add behavior while minimizing edits to fragile code.
**Technique:** Put new logic in a new helper method/function that you
develop by writing tests first, then add a single call from the legacy
method.

- Use when the new behavior can be cleanly separated as "compute X" or
  "transform Y".
- The legacy method becomes orchestration; the sprouted method becomes
  test-driven, clean logic.

### Sprout class/module
**Motivation:** Sometimes you cannot even construct the legacy class in
tests soon enough.
**Technique:** Create a new class/module that contains the new
behavior, develop it under tests, and call it from the legacy code with
the smallest possible integration.

- Treat it as a staging area: later, when the legacy area is
  test-covered, you can merge/reshape responsibilities.

### Wrap method
**Motivation:** Add behavior to an existing call without intertwining
logic.
**Technique:** Rename the old method, create a new method with the
original name/signature, and have it call:

- the new behavior (before/after), then
- the old renamed method.

This introduces a seam and keeps the old code intact.

### Wrap class
**Motivation:** Avoid adding more responsibilities to an already-too-
large class, or apply behavior across many callers.
**Technique:** Create a wrapper object that implements the same
interface and delegates to the wrapped instance while adding behavior
(decorator-style).

- Useful for logging, caching, authorization, retries, instrumentation,
  or policy changes.

---

## Dependency-breaking toolkit
Prefer the least invasive move that works. Use these to create seams and
make tests possible. Pick one, apply it minimally, then write tests.

- **Externalize hidden construction**
  - If a constructor creates network/db/filesystem objects internally,
    add an alternate constructor (or parameter) that accepts the
    dependency.
  - Keep the original constructor for production, delegating to the
    parameterized one.

- **Introduce an interface/protocol and use a fake**
  - Replace concrete dependencies with an interface.
  - In tests, implement a small fake that returns controlled data and
    records calls.

- **Replace global/static/singleton usage**
  - Wrap it behind an injected dependency or an overridable method.
  - Especially important for time, randomness, environment variables,
    global config, and service locators.

- **Separate side effects from decisions**
  - Move pure logic into testable functions (no I/O).
  - Keep I/O in thin adapters that have a small number of integration
    tests.

- **Use "good enough" test doubles**
  - Fakes/stubs are fine even if they're not "production-quality"
    objects.
  - Optimize for clarity and determinism; keep test code readable and
    maintainable.

---

## Scaling from "some tests" to "fully covered"
1. **Start where change happens and where failures hurt.**
   - High churn files, bug hotspots, complex branching logic, and core
     business flows first.

2. **Adopt a "method-use rule" for the whole codebase.**
   - Before calling, modifying, or relying on a method that lacks
     tests: write tests for it first (even if they only document current
     behavior).
   - Over time this turns the test suite into the living spec of what
     is safe to depend on.

3. **Turn coarse tests into fine tests.**
   - Begin with boundary characterization tests to get a safety net
     quickly.
   - Then push tests inward by introducing seams and replacing real
     dependencies with fakes.
   - The end state is lots of fast tests for logic + a small number of
     slower integration tests for adapters.

4. **Use coverage as a map, not a goal by itself.**
   - Use coverage reports to find untested risk areas and to confirm you
     didn't miss newly-added code.
   - Prefer "changed-lines coverage": new/edited lines must be covered
     by new tests.

5. **Refactor only under test protection.**
   - Once tests cover an area, you can safely: extract functions,
     rename, split modules, remove duplication, introduce interfaces,
     and simplify control flow.

---

## Modern techniques that accelerate legacy coverage
Use these when they speed up capturing behavior or increasing
confidence:

- **Snapshot ("approval") tests for complex outputs**
  - Great for HTML, JSON, logs, reports, large object graphs.
  - Use stable serialization and normalize nondeterminism (timestamps,
    random IDs).

- **Property-based testing for invariants**
  - Encode rules like "sorting is idempotent", "parse(serialize(x))
    preserves meaning", "total is never negative".
  - This finds edge cases legacy code often mishandles.

- **Mutation testing (selectively)**
  - Use on critical logic to verify tests would actually detect wrong
    behavior (not just execute lines).

- **Record/replay at boundaries**
  - For unstable external systems, record real interactions once and
    replay deterministically in tests.

---

## Making refactors and language translations safe
Treat the test suite as the contract:

- First, create characterization tests at stable boundaries to pin down
  existing behavior.
- Then introduce an abstraction (interface/module boundary) so both old
  and new implementations can exist.
- Run the same tests against both implementations (differential
  testing).
- Switch traffic/calls gradually, keeping tests as the gate.

---

## Agent operating rules
- Do not modify production code until at least one new automated test
  exists for the intended change.
- If code is not testable, prioritize creating a seam and breaking
  dependencies with minimal edits, protected by the nearest boundary
  characterization test.
- Keep edits small; run tests frequently; never proceed with failing
  tests.
- Prefer adding new tested code (sprout/wrap) over modifying untested
  legacy code directly.
