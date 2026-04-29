# Bug: <NAME>

**Date**: YYYY-MM-DD
**Ticket**: <TICKET-ID>
**Status**: Triage | Fix In Progress | Fix Complete | Verified

## Triage

**Category**: Physics Correctness | Portability | Build/Infrastructure | Data Safety | Performance Regression
**Severity**: P1 (Critical) | P2 (High) | P3 (Medium) | P4 (Low)
**Root Cause**: <file:line, specific condition, why it happens>
**Scope Decision**: Surgical Fix | Redirect to /feature

### Observed Behavior

<What the code does wrong. Include reproduction steps if available.>

### Expected Behavior

<What the code should do, with reference to AGENTS.md, doc/development/, or physics intent.>

### Investigation Notes

<What code was read, what patterns were checked, specific conditions that trigger the bug.>

---

## Bug Fix Spec

### Root Cause

<File:Line. Exact logic error. Why it happens.>

### Fix Approach

<Precisely what changes and what must NOT change. Surgical fix only. Preserve Params/States and host/device boundaries.>

### Regression Test Requirement

Describe the test that:
- **Fails** on unfixed code
- **Passes** on fixed code
- Location: `test/` mirrored directory

### Scope Constraints

<What is explicitly NOT part of this fix.>

### Acceptance Criteria

1. <Specific, testable, binary pass/fail>
2. ...

### Risk Assessment

<What could break if the fix is wrong? Related call sites? Hot paths?>

---

<!-- Sections below are appended by downstream personas. Do not fill during triage phase. -->

## Implementation

<!-- Appended by coder: summary of changes, test results -->

## Code Review

<!-- Appended by reviewer: findings (HIGH/MEDIUM/LOW), verdict (PASS/FAIL) -->
