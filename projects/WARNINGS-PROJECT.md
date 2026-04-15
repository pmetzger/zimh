# GCC/Clang Warning Recommendations

This project is a large, older C codebase. The warning policy should be
staged rather than maximally strict by default.

## Recommended Default Warnings

For normal developer builds, enable:

- `-Wall`
- `-Wextra`
- `-Wformat=2`
- `-Wundef`
- `-Wimplicit-fallthrough`
- `-Wstrict-prototypes`
- `-Wmissing-prototypes`
- `-Wold-style-definition`

These warnings tend to catch real bugs and API mistakes without creating
too much noise.

## Good Next-Tier Warnings

After the tree is reasonably clean, consider enabling:

- `-Wpointer-arith`
- `-Wcast-qual`
- `-Wwrite-strings`
- `-Wshadow`

These are useful, but they are more likely to produce cleanup churn in
older code.

## Warnings Not Recommended Globally Yet

Do not enable these globally without a deliberate cleanup pass:

- `-Wconversion`
- `-Wsign-conversion`
- `-Wcast-align`
- `-pedantic`
- `-Wstrict-overflow`
- `-Wdouble-promotion`

They are likely to be too noisy for this repository as a general build
default.

## Warnings As Errors

Do not make warnings fatal for ordinary developer builds.

Instead, use:

- an optional strict build with `-Werror`
- or a dedicated CI configuration that enables `-Werror`

This keeps local builds usable while still allowing stricter enforcement
when needed.

## Suggested Policy

1. Default builds:
   - `-Wall -Wextra -Wformat=2 -Wundef -Wimplicit-fallthrough`
   - `-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition`
2. Optional strict build:
   - add `-Werror`
3. Optional audit build:
   - add `-Wshadow`, `-Wcast-qual`, and `-Wwrite-strings`
