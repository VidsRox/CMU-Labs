# CS:APP PerfLab - Optimized Kernels

## Results

| Kernel | Mean Speedup | Method |
|--------|-------------|--------|
| rotate | ~20x | 8×8 blocking |
| smooth | ~60x | Four-region split, no conditionals |

Verified correct at ODD_DIM=96 via `./driver -g`.

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

## Building and running

```bash
make driver
./driver          # test all registered versions
./driver -g       # autograder mode - tests only rotate() and smooth()
```

## Process

The lab mechanics themselves weren't particularly hard - once the mental model was in place, writing the code was straightforward. The bulk of the work was building that model: understanding why strided writes cause misses, what blocking actually changes about memory access order, and why eliminating `avg()` matters more than inlining it. This was done through Socratic back-and-forth with Claude - working through the hardware reasoning concretely before touching `kernels.c`, so that each optimization was understood rather than cargo-culted.

## Files

Only `kernels.c` was modified. All other files are original lab infrastructure.