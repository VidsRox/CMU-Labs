# CS:APP PerfLab - Optimized Kernels

## Results

| Kernel | Mean Speedup | Method |
|--------|-------------|--------|
| rotate | ~18x | 8×8 blocking |
| smooth | ~61x | Four-region split, no conditionals |

Verified correct at ODD_DIM=96 via `./driver -g`.

### Full benchmark output

**Rotate:**
| Version | 64 | 128 | 256 | 512 | 1024 | Mean |
|---|---|---|---|---|---|---|
| naive | 1.8 | 2.5 | 4.7 | 9.1 | 8.2 | - |
| 2x unroll | 1.9 | 2.6 | 7.6 | 10.3 | 8.3 | 8.8x |
| 4x unroll | 1.8 | 2.5 | 4.7 | 9.5 | 8.4 | 10.0x |
| 8x unroll | 2.0 | 2.6 | 5.8 | 9.2 | 7.8 | 9.5x |
| blocked 8×8 | 1.8 | 1.9 | 3.3 | 2.4 | 3.1 | **18.2x** |

**Smooth:**
| Version | 32 | 64 | 128 | 256 | 512 | Mean |
|---|---|---|---|---|---|---|
| naive | 39.6 | 39.4 | 39.4 | 41.3 | 44.3 | 17.3x |
| four-region split | 11.6 | 11.7 | 12.1 | 11.9 | 11.9 | 59.7x |
| prefix sum | 27.1 | 26.6 | 31.6 | 35.5 | 36.7 | 22.6x |

## What was optimized

### rotate
Naive rotate has stride-1 src reads (fast) but stride-N dst writes (catastrophic). As j increases by 1, the destination row decreases by 1, jumping 3072 bytes between consecutive writes for a 512×512 image. Every write lands in a different cache line - one miss paid, one pixel used out of ten available.

Fix: 8×8 blocking. Each tile touches exactly 8 destination rows repeatedly as i advances across the tile. The working set stays bounded in L1/L2 regardless of total image size, converting guaranteed cold misses into cache hits within the tile.

Block sizes 8 and 16 both perform well. Block size 32 degrades at N=1024 because a 32×32 tile of both src and dst (~12KB each) pushes against L1 d-cache capacity.

Loop unrolling (2x, 4x, 8x) was measured and confirmed to provide no meaningful speedup in isolation - the bottleneck is cache misses on dst writes, not loop overhead. Unrolling only matters after blocking is in place.

### smooth
Naive smooth calls `avg()` once per pixel. `avg()` uses `min()`/`max()` boundary clamps on every call, even for interior pixels where the window is always a full 3×3 and the answer is always divide by 9. The compiler cannot eliminate these branches because it cannot prove at compile time that any given pixel is interior.

Fix: eliminate `avg()` entirely, split into four explicit regions with hardcoded neighbor counts and no conditionals anywhere. Interior pixels (rows 1 to dim-2, columns 1 to dim-2) cover ~99% of the image for large N and get a tight branch-free loop with direct accumulation into local variables.

Regions handled: 4 corners (4 neighbors, hardcoded), top/bottom edges (6 neighbors, loop over j), left/right edges (6 neighbors, loop over i), interior (9 neighbors, nested loop with no conditionals).

### smooth - prefix sum experiment
As a follow-up, a prefix sum (2D summed-area table) based smooth was implemented and benchmarked. The idea: precompute three `(dim+1)×(dim+1)` integral image tables - one per channel - then answer each pixel's neighbourhood sum with four array lookups and the inclusion-exclusion formula, collapsing the corner/edge/interior case split into a single uniform loop.

Result: 22.6x mean speedup - **2.7x slower than the four-region split and barely better than naive**. Two reasons:

- **Precomputation overhead**: three full `calloc` passes over arrays larger than the image, paid upfront before any pixel is computed.
- **Cache pressure**: the three extra `(dim+1)²` arrays grow with image size, increasingly spilling out of cache. CPE degrades visibly with dim (27.1 → 36.7) while the four-region split stays flat (11.6 → 11.3), which is the cache effect in isolation.

The prefix sum approach wins for large, variable-size windows where it replaces O(window²) work with O(1). For a fixed 3×3 window with only 9 additions per pixel, the arithmetic savings (nine additions reduced to four lookups) are too small to justify the memory overhead. The four-region split, which eliminates branching and function call overhead without introducing new data structures, captures the real bottleneck here.

## Building and running

```bash
make driver
./driver          # test all registered versions
./driver -g       # autograder mode - tests only rotate() and smooth()
```

## Process

The lab mechanics themselves weren't particularly hard - once the mental model was in place, writing the code was straightforward. The bulk of the work was building that model: understanding why strided writes cause misses, what blocking actually changes about memory access order, and why eliminating `avg()` matters more than inlining it. This was done through Socratic back-and-forth with Claude - working through the hardware reasoning concretely before touching `kernels.c`, so that each optimization was understood rather than cargo-culted.

The prefix sum experiment came out of studying the Range Sum Query 2D problem (LeetCode 304) immediately after PerfLab. The smooth function's neighbourhood sum is structurally identical to a 2D range query, which made testing the prefix sum approach a natural extension. The benchmark confirmed the theoretical prediction: prefix sums are the right tool when window size is large or variable, not for a fixed 3×3 kernel.

## Files

Only `kernels.c` was modified. All other files are original lab infrastructure.