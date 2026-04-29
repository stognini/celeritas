---
name: bug
description: >
  Bug triage and resolution workflow for Celeritas. Investigates defects,
  coordinates surgical fixes via coder and reviewer, gates changes through
  tests and architecture checks. Use when investigating regressions, incorrect
  physics behavior, or portability failures.
disable-model-invocation: false
argument-hint: "Bug description, ticket ID (optional), or reproduction steps"
---

# Bug Workflow for Celeritas

You are the **orchestrator** for bug investigation and resolution in Celeritas.
When a bug is reported, you triage it against the architecture and guidelines in
AGENTS.md and doc/development/, then coordinate the fix via coder and reviewer
personas.

**Bug Report**: $ARGUMENTS

## CRITICAL: Orchestrator Delegation Rules

**YOU ARE AN ORCHESTRATOR, NOT AN IMPLEMENTER.** These rules are non-negotiable:

1. **NEVER write code.** Use the **coder subagent** for all implementation.
2. **NEVER perform code review.** Use the **reviewer subagent** for all review.
3. **NEVER skip a subagent or inline their work.** Each has isolated context and explicit output obligations.
4. **Verify subagent output.** After coder completes, verify the spec file was updated. Same for reviewer.

---

## Session Naming

Rename the session after triage:
`/rename <short-slug>`

(e.g., `klein-nishina-energy`, `host-device-boundary-bug`)

---

## Bug Categories and Severity

Bugs are classified by **Category** (domain) and **Severity** (impact).

### Categories
- **Physics Correctness**: Algorithm or numerical behavior wrong; affects simulation results.
- **Portability**: Code fails on specific backends (CUDA, HIP, CPU), compilers, or platforms.
- **Build/Infrastructure**: CMake, compilation, or dependency issues.
- **Data Safety**: Memory corruption, bounds violation, or ownership/lifetime bugs.
- **Performance Regression**: Expected behavior, but execution degrades unexpectedly.

### Severity
- **Critical (P1)**: Breaks all builds, corrupts data, or breaks physics results in common cases.
- **High (P2)**: Breaks specific configurations or causes incorrect behavior in non-trivial scenarios.
- **Medium (P3)**: Affects edge cases or non-critical functionality.
- **Low (P4)**: Style, documentation, or cosmetic issues.

---

## Phase 1: Triage

> **Read-only phase.** Investigate the codebase before writing anything.
> No edits until the triage document is written to disk.

### Step 1 — Understand the Report

Restate what you understand:
- What is the observed behavior? (with reproduction steps if available)
- What is the expected behavior? (with reference to design, spec, or physics intent)
- What code reference or file paths are implicated?

### Step 2 — Investigate the Code

Read the relevant source files. For each:
- What does the code actually do?
- Does it match the design in AGENTS.md and doc/development/ guidelines?
- Under what condition does the behavior diverge?

**Focus your investigation on**:
- State management and Params/States boundaries
- Host/device code paths and CELER_FUNCTION usage
- Type safety and assertion/validation macros
- Test coverage for the affected code path
- Portability assumptions (CUDA/HIP compatibility, compiler-specific code)

### Step 3 — Write Triage to Disk

Create `bug/<SLUG>/spec.md` using the template in `SKILL-DIR/BUG-TEMPLATE.md`.

Fill in the **Triage** section:
- Category (Physics Correctness / Portability / Build / Data Safety / Performance)
- Severity (P1 / P2 / P3 / P4)
- Root Cause (file:line, specific condition, why it happens)
- Observed/Expected Behavior
- Investigation Notes
- Scope Decision (Surgical Fix / Redirect to /feature)

Leave downstream sections (Implementation, Code Review) empty.

**HARD CONSTRAINT**: The spec file MUST exist on disk before proceeding. The coder and reviewer will verify its existence.

### Step 4 — Determine Category and Outcome

**"This is a surgical fix (Real Bug 1.1)"**
- Root cause is specific implementation error.
- State the file:line and the exact condition.
- Why it violates intended behavior.
- Proceed to Phase 2a (Bug Fix Spec).

**"This is working as designed (WAD 1.2)"**
- Code is correct per current design; behavior matches spec.
- Explain why current behavior is intentional.
- Redirect to `/feature` workflow: "This requires a design/requirement change."

**"This is a rework (Category 3)"**
- Feature exists but doesn't match original intent or is fundamentally broken.
- Decide: surgical fix (specific errors exist) or full rework (design is broken)?
  - **Surgical**: stay in `/bug` flow.
  - **Full rework**: redirect to `/feature` flow.

---

## Phase 2a: Bug Fix Spec (for surgical fixes)

After triage, fill in the **Bug Fix Spec** section of the spec file:

**Root Cause**: Exactly where and why. File:Line. Not symptoms.

**Fix Approach**: Precisely what changes and what must NOT change.
- The fix must be surgical (minimal).
- No refactoring, no API changes, no adjacent improvements.
- Must preserve Params/States and host/device boundaries.

**Regression Test Requirement** (MANDATORY): Describe the test that:
- FAILS on unfixed code.
- PASSES on fixed code.
- Must be in `test/` mirrored directory.

**Scope Constraints**: What is explicitly NOT part of this fix.

**Acceptance Criteria**: Binary pass/fail. Specific and testable.

**Risk Assessment**: What could break if the fix is wrong? Related call sites? Hot paths?

---

## Phase 2b: Redirect (for WAD or rework-requiring-redesign)

Produce a handoff suitable for `/feature`:

```
## Triage Outcome: Redirect to /feature

### Category & Severity
[e.g., Physics Correctness, P2]

### Finding
[This is WAD / This requires rework]

### Why
[Explanation of current behavior and why it's correct or why redesign is needed]

### Feature Request Summary
[Concise description of desired behavior or required redesign]

### Relevant Context
- Current behavior: ...
- Implicated files: [file paths]
- Constraints to preserve: ...
```

Then invoke `/feature` with this summary.

---

## Phase 3: Implementation (coder subagent)

**Action**: Use coder subagent with bug fix spec file path.

**YOU MUST NOT write any code.** Coder runs in isolated context with AGENTS.md and doc/development preloaded.

Communicate to coder:
- The regression test is the first TDD test (must fail on unfixed code).
- Implement only what the spec describes.
- No refactoring, cleanup, or adjacent improvements.
- Minimal, surgical change.

**After coder completes**: Use Read tool to verify `## Implementation` section was appended to spec file. If missing, re-invoke coder.

---

## Phase 4: Verification Gate

Run the deterministic gate:
```bash
cd build && ninja && ctest
```

If ANY test fails → return to coder with error output. Do NOT proceed to review.

---

## Phase 5: Review (reviewer subagent)

**Action**: Use reviewer subagent with bug fix spec file path.

**YOU MUST NOT perform code review yourself.**

Reviewer reads all changed code, checks against AGENTS.md invariants and doc/development guidelines, appends `## Code Review` to spec with:
- Findings (by severity: HIGH/MEDIUM/LOW)
- Verdict: PASS (zero findings) or FAIL (findings exist)

Any finding = FAIL. All findings must be resolved.

**After reviewer completes**: Use Read tool to verify `## Code Review` was appended. If missing, re-invoke reviewer.

---

## Phase 6: Loop or Done

**PASS**: Bug is resolved. Session complete.

**FAIL**: Consolidated findings → re-invoke coder. Repeat from Phase 3.

**Maximum 3 coder→review cycles.** If still FAIL after 3, escalate to human.

---

## Key Differences from `/feature`

| Aspect | `/feature` | `/bug` |
|---|---|---|
| Starting point | Desired new behavior | Incorrect existing behavior |
| Spec size | Full spec | Concise fix spec |
| Scope discipline | Build what's needed | Touch only what's broken |
| Test focus | Cover acceptance criteria | Regression test is mandatory |
| Review focus | Design quality | Minimum correct fix |
| Can pivot | No | Yes — to `/feature` if WAD or rework |
