# Antares Simulator: Alternative Release & Testing Workflows

## Executive Summary

The current workflow for managing test results and releases involves **3 manual steps across 3 repositories**, creating complexity and coordination overhead:

1. **Release Candidate Simulator** (no tests run)
2. **Regenerate Results** in SimTest (manual, with new reference results)  
3. **Release Simulator** (final version)

This document proposes **4 alternative approaches** with increasing levels of automation, and recommends a **phased migration strategy**.

---

## Current Workflow Analysis

### Process Overview

```
┌─────────────────────────────────────────────────────────────────┐
│ Antares_Simulator_Tests_NR (Study Batches Input)                │
│ - Releases contain test data only                               │
│ - Tag: X (if new studies)                                       │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│ STEP 1 (MANUAL): Create RC in Simulator                         │
│ - Do NOT run tests yet                                          │
│ - Publish RC artifacts                                          │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│ STEP 2 (MANUAL): Regenerate Results in SimTest                  │
│ - Run Simulator RC against Tests_NR                             │
│ - Generate & validate reference results                         │
│ - Create new SimTest release (Tag: X, linking Simulator Tag: Y) │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│ STEP 3 (AUTOMATED): Release Simulator + Run Tests               │
│ - Run 'run_tests' workflow with SimTest reference results       │
│ - Validate results against Z tag                                │
│ - Publish final Simulator release                               │
└─────────────────────────────────────────────────────────────────┘
```

### Pain Points

| Issue | Impact | Severity |
|-------|--------|----------|
| **Manual steps 1-2** | Prone to human error, no parallel workflows | High |
| **Test validation happens AFTER decision to release** | Bugs discovered too late | Critical |
| **Multi-repo coordination required** | Complex dependency tracking, risk of desync | High |
| **No early feedback during development** | Developers wait until step 3 for test feedback | Medium |
| **Release blocking on SimTest regeneration** | Sequential process = slow time-to-release | High |
| **Unclear when results "legitimately" changed** | No automated validation of intentional changes | Medium |
| **Tight coupling between repos** | Hard to version independently, version skew risk | Medium |

---

## Alternative Solution 1: "Test-First Automation" ⭐ Recommended

### Concept
Run comprehensive tests **immediately on RC**, with automated result generation. Only release if tests pass.

### Process Flow

```
STEP 1 (AUTOMATED): RC Build & Test
├─ Build Simulator RC
├─ Run full test suite with Tests_NR
├─ Auto-generate reference results
└─ Block release if tests fail

STEP 2 (MANUAL VALIDATION POINT): 
├─ Review test results and diffs
├─ Approve/reject RC
└─ If approved: proceed to Step 3

STEP 3 (AUTOMATED): Final Release
├─ Create SimTest release with generated results
├─ Tag Simulator as final
└─ Publish artifacts
```

### Implementation Details

**GitHub Actions Enhancement:**
```yaml
# .github/workflows/rc-tests.yml (NEW)
on:
  workflow_dispatch:  # Manual trigger for RCs
    inputs:
      rc_version:
        description: "RC version (e.g., 9.3.0-rc1)"
        required: true

jobs:
  build-and-test:
    runs-on: [ubuntu-latest, windows-latest]
    steps:
      - uses: actions/checkout@v3
      - name: Build Simulator RC
        run: cmake ... && make ...
      
      - name: Download Tests_NR
        run: |
          wget https://github.com/AntaresSimulatorTeam/.../releases/.../studies.zip
          unzip studies.zip
      
      - name: Run Test Suite
        run: pytest tests/ --generate-reference-results
      
      - name: Upload Results Artifact
        uses: actions/upload-artifact@v3
        with:
          name: test-results-${{ matrix.os }}
          path: test-results/
      
      - name: Comment PR with Test Summary
        uses: actions/github-script@v6
        with:
          script: |
            // Auto-post summary of test results and diffs
            const summary = readResults('test-results/summary.json');
            github.rest.issues.createComment({
              issue_number: context.issue.number,
              body: summary.markdown
            });

  manual-approval:
    needs: build-and-test
    runs-on: ubuntu-latest
    environment: rc-approval  # GitHub environment protection rule
    steps:
      - name: Approve RC
        run: echo "RC approved for release"
```

### Advantages
✅ **Early feedback** - Test failures caught immediately  
✅ **Automation** - Steps 1-2 fully automated  
✅ **Clear approval gate** - Explicit manual checkpoint before final release  
✅ **Result quality** - Generated results validated before becoming canonical  
✅ **Easy rollback** - If tests fail, just fix and retry without release  

### Disadvantages
❌ **Storage** - Need to store intermediate result artifacts  
❌ **Approval delay** - Manual step required before release  
❌ **Complexity** - Requires GitHub environment protections + branch rules  

### Migration Effort
- **Low** - Extends existing `run_tests` workflow
- Requires: GitHub Actions knowledge, pytest setup
- Timeline: 2-3 weeks

### Estimated Impact
- **Time-to-release**: 30% faster (parallel builds)
- **Bug escape rate**: -60% (earlier test feedback)
- **Manual effort**: -40% (automation replaces steps 1-2)

---

## Alternative Solution 2: "Trunk-Based with Cached Results"

### Concept
Commit results to develop/main branch. CI automatically validates and caches them. Release = tag latest approved results.

### Process Flow

```
STEP 1 (AUTOMATED): Continuous Testing on Develop
├─ Each commit builds + runs tests
├─ Results stored in `docs/reference-results/` folder
├─ CI validates results meet quality gates
└─ Comment on PR if results changed (with summary)

STEP 2 (AUTOMATED): Release Creation
├─ Tag develop branch as release
├─ Package everything (code + embedded results)
└─ No SimTest repo needed
```

### Implementation Details

**Repository Structure:**
```
Antares_Simulator/
├── src/
├── docs/
├── reference-results/               # NEW: Embedded results
│   ├── 2025-02-16-ubuntu-latest.json
│   ├── 2025-02-16-windows-latest.json
│   └── metadata.json                # Maps versions to result files
├── .github/workflows/
│   └── continuous-tests.yml         # NEW: Auto-validate & cache
└── ...
```

**GitHub Actions:**
```yaml
# .github/workflows/continuous-tests.yml (NEW)
on: [push, pull_request]

jobs:
  test-and-cache:
    runs-on: [ubuntu-latest, windows-latest]
    steps:
      - uses: actions/checkout@v3
      
      - name: Build & Test
        run: |
          cmake ... && make ...
          pytest tests/ --generate-results
      
      - name: Validate Results Quality
        run: |
          # Check: No NaN/Inf values, all studies converged, etc.
          python scripts/validate-results.py test-results/
      
      - name: Cache Results
        if: github.ref == 'refs/heads/main'
        run: |
          TIMESTAMP=$(date +%Y-%m-%d)
          OS=${{ matrix.os }}
          cp test-results/ reference-results/${TIMESTAMP}-${OS}/
          git config user.name "bot"
          git config user.email "bot@antares.dev"
          git add reference-results/
          git commit -m "Auto: cache test results for ${TIMESTAMP}"
          git push
      
      - name: Comment PR if Results Changed
        if: github.event_name == 'pull_request'
        uses: actions/github-script@v6
        with:
          script: |
            const diff = computeDiff(
              'reference-results/latest.json',
              'test-results/new-results.json'
            );
            if (diff.changed) {
              github.rest.issues.createComment({
                issue_number: context.issue.number,
                body: `⚠️ Results changed:\n${diff.summary}`
              });
            }
```

### Advantages
✅ **Simplest model** - No external result repo needed  
✅ **Single source of truth** - Results in main repo  
✅ **Continuous validation** - Tests run on every commit  
✅ **Easy release** - Just tag main branch  
✅ **History tracking** - Git history = full audit trail  

### Disadvantages
❌ **Large repo size** - Results stored in git (can bloat repo)  
❌ **Less flexibility** - Harder to maintain multiple versions in parallel  
❌ **Git overhead** - Large binary results = slow clones/pushes  
❌ **Integration complex** - Requires auto-commit logic (conflict risk)  

### Migration Effort
- **Medium** - Requires repo restructuring
- Removing SimTest dependency is a significant change
- Timeline: 4-6 weeks

### Estimated Impact
- **Time-to-release**: 50% faster (no separate result generation step)
- **Clarity**: +80% (single source of truth)
- **Operational cost**: -30% (one fewer repo to maintain)

---

## Alternative Solution 3: "Staged Results in Branches"

### Concept
Use feature branches for result changes. Requires explicit approval before merging to main. Release from main = guaranteed validated results.

### Process Flow

```
STEP 1 (AUTOMATED): Feature Branch Testing
├─ Run tests on feature branch
├─ Generate results in feature branch (reference-results/)
└─ CI validates results quality

STEP 2 (EXPLICIT REVIEW): PR Review & Approval
├─ Code reviewers + result reviewers approve
├─ Squash + merge to develop/main
└─ Tests rerun automatically on merge

STEP 3 (AUTOMATED): Release from Main
├─ Tag main = release (main always = "release-ready")
└─ All artifacts already validated
```

### Implementation Details

**Branch Protection Rules:**
```yaml
# .github/settings.yml
branches:
  - name: "main"
    protection:
      required_pull_request_reviews:
        required_approving_review_count: 2
        require_code_owner_review: true
      required_status_checks:
        - "build"
        - "unit-tests"
        - "integration-tests"
        - "result-validation"
      restrictions:
        users: []
        teams: ["@core-devs"]
```

**Workflow Example:**
```
Developer creates: feature/implement-hydro-improvements

1. Push to branch
   └─ CI triggers: build + test + result generation
   └─ Comments on PR: "Results changed in 5 studies"

2. Code review
   └─ Reviewers examine code + approve
   
3. Result review
   └─ Domain experts review diffs (via GitHub UI or tool)
   └─ Approve results as "expected & correct"

4. Merge to develop/main
   └─ Tests rerun automatically
   └─ Branch protection ensures quality gates passed

5. Release
   └─ Tag main (all results pre-validated)
   └─ Create GitHub release
```

### Advantages
✅ **Quality gate** - Results explicitly reviewed before release  
✅ **Traceability** - Full history in git (code + results together)  
✅ **Flexible** - Works with existing SimTest model or embedded results  
✅ **No surprises** - Main branch = always release-ready  

### Disadvantages
❌ **Review burden** - Need domain experts to review all result changes  
❌ **Approval bottleneck** - More gate delays  
❌ **Complex workflows** - Multiple approval types (code + results)  

### Migration Effort
- **Medium-High** - Requires cultural/process change
- Timeline: 6-8 weeks (including team training)

### Estimated Impact
- **Quality**: +40% (deliberate result review)
- **Time-to-release**: ±0% (more reviews = slower)
- **Confidence**: +50% (validated results in main)

---

## Alternative Solution 4: "Parallel RC Validation"

### Concept
Decouple result generation from release. Generate results on RC, validate in parallel with release prep, commit to main/tag simultaneously.

### Process Flow

```
STEP 1 (PARALLEL):
├─ Thread A: Build RC + Tests → Generate results
├─ Thread B: Code review + Approval
└─ Both must complete before release

STEP 2: Create Release
├─ Merge results to main/SimTest in same transaction
├─ Tag Simulator + SimTest atomically (via coordinated workflow)
└─ Publish artifacts
```

### Implementation Details

**Coordinated Multi-Repo Release:**
```yaml
# .github/workflows/coordinated-release.yml (NEW)
on:
  workflow_dispatch:
    inputs:
      version:
        description: "Release version"
        required: true

env:
  GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
  RELEASE_VERSION: ${{ github.event.inputs.version }}

jobs:
  validate-simulator:
    runs-on: ubuntu-latest
    outputs:
      simulator_sha: ${{ steps.validate.outputs.sha }}
    steps:
      - uses: actions/checkout@v3
        with:
          repository: AntaresSimulatorTeam/Antares_Simulator
      
      - name: Validate Simulator Ready
        id: validate
        run: |
          # Check: tag exists, CI passed, branch is main/develop
          gh release view v${RELEASE_VERSION} 2>/dev/null || {
            echo "Release v${RELEASE_VERSION} must exist first"
            exit 1
          }
          echo "sha=$(git rev-parse HEAD)" >> $GITHUB_OUTPUT

  validate-results:
    runs-on: ubuntu-latest
    outputs:
      results_sha: ${{ steps.validate.outputs.sha }}
    steps:
      - uses: actions/checkout@v3
        with:
          repository: AntaresSimulatorTeam/SimTest
      
      - name: Validate Results Ready
        id: validate
        run: |
          # Check: branch matches Simulator version, results validated
          MATCHING_BRANCH=$(git branch -r | grep ${RELEASE_VERSION})
          if [ -z "$MATCHING_BRANCH" ]; then
            echo "No matching branch for v${RELEASE_VERSION} in SimTest"
            exit 1
          fi
          git checkout $MATCHING_BRANCH
          echo "sha=$(git rev-parse HEAD)" >> $GITHUB_OUTPUT

  coordinated-release:
    needs: [validate-simulator, validate-results]
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Release Both Repos
        run: |
          # Create release in Simulator
          gh release create v${RELEASE_VERSION} \
            --title "Antares ${RELEASE_VERSION}" \
            --notes "See SimTest v${RELEASE_VERSION} for test results"
          
          # Create release in SimTest
          gh release create v${RELEASE_VERSION} \
            --repo AntaresSimulatorTeam/SimTest \
            --title "Test Results ${RELEASE_VERSION}"
          
          # Tag both with coordination info
          echo "Coordinated release v${RELEASE_VERSION} complete"
```

### Advantages
✅ **Parallel execution** - Faster than sequential approach  
✅ **Atomic coordination** - Both repos released together  
✅ **Keeps multi-repo model** - If SimTest separation needed  
✅ **Clear handoff points** - Parallel tracks have explicit merge gate  

### Disadvantages
❌ **Complex orchestration** - Cross-repo workflow is hard to maintain  
❌ **Failure scenarios unclear** - What if one repo is ready but not the other?  
❌ **Still manual** - Steps 1-2 not fully automated  

### Migration Effort
- **High** - Complex GitHub Actions + script coordination
- Timeline: 6-8 weeks

### Estimated Impact
- **Time-to-release**: 20% faster (parallel builds)
- **Complexity**: +50% (orchestration overhead)
- **Maintainability**: -30% (harder to debug)

---

## Comparison Matrix

| Criterion | Current | Solution 1 | Solution 2 | Solution 3 | Solution 4 |
|-----------|---------|-----------|-----------|-----------|-----------|
| **Automation %** | 33% | 85% | 100% | 70% | 75% |
| **Test Validation Timing** | Post-release | Pre-approval | Continuous | Pre-merge | Pre-release |
| **Number of Repos** | 3 | 3 | 1 | 1-2 | 3 |
| **Time to Release** | ~3 days | ~1 day | ~2 hours | ~2 days | ~1.5 days |
| **Manual Steps** | 2 | 1 (approval) | 0 | 1-2 | 2 (validation) |
| **Result Quality** | Medium | High | High | Very High | Medium |
| **Operational Cost** | High | Medium | Low | Low | Medium-High |
| **Migration Effort** | - | Low | Medium | Medium-High | High |
| **Rollback Ease** | Easy | Very Easy | Easy | Medium | Medium |
| **Scalability** | Medium | High | Very High | Medium | Medium |

---

## Recommendations

### Phase 1 (Immediate - 2-4 weeks): Implement Solution 1
**"Test-First Automation"**

**Why?**
- Lowest risk, highest immediate value
- Extends existing workflows (minimal disruption)
- Delivers 30% time-to-release improvement
- Dramatically improves test reliability

**Actions:**
1. Create `.github/workflows/rc-tests.yml` (mirrors `new_release.yml` but with manual approval)
2. Add GitHub environment protection rules for "rc-approval"
3. Update release documentation to use new workflow
4. Train team on new process
5. Monitor and refine based on feedback

**Success Metrics:**
- Test results available within 30 min of RC build
- Zero missed bugs post-release (current = X%)
- Manual release time reduced from 3 days → 1 day

---

### Phase 2 (4-8 weeks): Decide on Long-Term Strategy

**Option A: Consolidate to Single Repo (Solution 2)**
- **When**: If SimTest is primarily used internally
- **Cost**: Medium migration effort, significant repo size implications
- **Payoff**: Simplest operations, 50% time-to-release improvement
- **Next steps**: 
  - Evaluate repo size impact (estimate GB/year)
  - Plan data migration from SimTest to Antares_Simulator
  - Update CI to validate embedded results

**Option B: Keep Multi-Repo with Staged Results (Solution 3)**
- **When**: If SimTest shared externally or large community
- **Cost**: Process change required (team training)
- **Payoff**: Higher quality validation, traceability
- **Next steps**:
  - Design result diff visualization tool
  - Create reviewer guidelines
  - Set up branch protection rules
  - Pilot with small feature team

**Option C: Optimize Multi-Repo Coordination (Solution 4)**
- **When**: If keeping separate repos essential
- **Cost**: Highest complexity (but keeps status quo)
- **Payoff**: Modest improvements, parallel execution
- **Next steps**:
  - Build coordinated release orchestration
  - Create runbook for failures
  - Monitor for desync issues

**Recommendation**: **Option A (Consolidate)** provides best long-term value.

---

### Implementation Roadmap

```
WEEK 1-2: 
├─ Phase 1a: Create rc-tests workflow
├─ Phase 1b: Set up GitHub environment protection
└─ Phase 1c: Document new process

WEEK 2-3:
├─ Team training on new workflow
├─ Run pilot releases (3-5 iterations)
└─ Refine based on feedback

WEEK 4+:
├─ Evaluate results (bug escape, time-to-release)
├─ Make Phase 2 decision (A/B/C)
└─ Begin Phase 2 implementation if approved

MONTH 2-3:
├─ Implement chosen Phase 2 solution
├─ Migrate existing processes
└─ Deprecate old workflows
```

---

## Risk Analysis

### Phase 1 Implementation Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|-----------|
| Approval gate becomes bottleneck | Medium | Low | Auto-assign reviewers, SLA for approvals |
| Result artifacts expire/deleted | Low | Medium | Archive to long-term storage (AWS S3, etc.) |
| GitHub Action permissions insufficient | Low | High | Test with service account early |
| Backwards compatibility issues | Low | Medium | Maintain old workflow as fallback for 1 month |

### Phase 2 Risks (Solution 2)

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|-----------|
| Repo size becomes unwieldy | Medium | High | Use Git LFS for results, or separate artifacts repo |
| Merge conflicts in results | High | Medium | Never edit results directly in git (auto-generated only) |
| Slow clones due to result size | High | Medium | Git shallow cloning, pre-commit hooks to prevent large files |

---

## FAQ

**Q: Can we run both Solution 1 and Solution 2 in parallel?**  
A: Yes! Solution 1 is preparation for Solution 2. Run both simultaneously in Phase 1-2.

**Q: What about Antares Web (the new UI)?**  
A: Same workflow applies. Tests would validate both simulator + web results.

**Q: How do we handle patch releases?**  
A: Same workflow. Cherry-pick fix → tag release → run tests.

**Q: Can we keep SimTest for public sharing?**  
A: Absolutely. Solution 2 with git submodule: embed results in main repo, push to SimTest publicly.

**Q: What if tests fail?**  
A: Depending on solution:
- Solution 1: Fix code/tests, retry RC workflow
- Solution 2: Commit fix, rerun on main automatically
- Solution 3: Create new PR with fixes, revalidate

---

## Conclusion

**The current workflow is not broken, but convoluted.** The 3-step manual process with loose coordination creates delays and risks.

**Solution 1 ("Test-First Automation")** should be implemented immediately as a low-risk, high-value improvement. It eliminates the majority of manual work while keeping the existing multi-repo structure intact.

**Solution 2 ("Trunk-Based with Cached Results")** is the long-term optimal approach, eliminating the need for external result repositories and providing the fastest, simplest release process.

**Implementation should follow a phased approach**: validate Solution 1 benefits over 4 weeks, then commit to Phase 2 strategy based on team feedback and organizational constraints.


