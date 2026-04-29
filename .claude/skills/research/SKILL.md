---
name: research
description: 'Research with full traceability across web and workspace sources. Use for evidence gathering, web/file search, comparative analysis, and documented findings with exact queries, URLs, files, tools, and claim-to-source mapping.'
argument-hint: 'Research question, scope, and constraints/timebox'
user-invocable: true
---

# Research Skill

## Purpose
Use this skill when the user asks for web/file research, comparisons, evidence gathering,
or "find out" tasks. The output must be reproducible and auditable.

This skill prioritizes:
- Traceability: every claim links to a concrete source touchpoint.
- Reproducibility: another person can repeat the same search steps.
- Separation of summary vs deep analysis.

## Required Deliverables
For any research task, produce both:
1. A concise executive summary for sharing with stakeholders.
2. A full research report that includes complete search trace logs.

Use the template at:
- `.claude/skills/research/RESEARCH_RESULT_TEMPLATE.md`

## Required Traceability Fields
The report must include all of the following, even if a section is empty:
- Date/time (ISO 8601 with timezone)
- Author
- Model used
- Task prompt and scope
- Search engines used (name + query strings)
- URLs fetched (exact URL + why visited)
- Workspace files searched/opened (exact path + why opened)
- Commands/tools used (tool name + parameters summary)
- Inclusion/exclusion rationale
- Evidence-to-claim mapping
- Known gaps and confidence

## Workflow
1. Clarify objective
- Restate the research question, ask clarification questions, and define success criteria.
- **Never assume**; ask for clarification if there's any ambiguity.

2. Plan search strategy
- Create query variants before searching.
- Prefer broad-to-narrow sequence.

3. Execute and log in real time
- Log each search query exactly as executed.
- Log each fetched URL and each opened file path.
- Add one-line intent for every source touch.

4. Analyze evidence
- Extract relevant facts and contradictions.
- Separate observations from inferences.

5. Synthesize output
- Write executive summary first.
- Write detailed analysis with citations to trace entries.

6. Validate trace completeness
- Verify all claims have source linkage.
- Verify empty sections explicitly say "None".

## Quality Bar
- Do not present uncited conclusions as facts.
- Quote exact user queries and exact URLs where possible.
- Clearly mark assumptions.
- If a source appears low quality or outdated, state why.

## Suggested Citation Style Inside Report
- Use source IDs like `W1`, `W2` (web) and `F1`, `F2` (files).
- Reference IDs inline for claims, e.g. "X increased in 2025 [W3]".

## Minimal Example Trace Entry
```text
ID: W2
Type: web
Engine: DuckDuckGo
Query: "celeritas geant4 integration architecture"
URL: https://example.org/page
Timestamp: 2026-04-28T15:22:09-07:00
Reason opened: Identify authoritative architecture overview
Key notes: Mentions Action/Executor/Interactor layering
```

## When to Stop Research
Stop when one of these is true:
- The question is fully answered with converging evidence.
- Additional sources are repetitive and add no new signal.
- Timebox is reached; report residual uncertainty.
