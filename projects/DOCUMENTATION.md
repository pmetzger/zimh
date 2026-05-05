# Documentation Fixes

Add documentation comments for all public APIs in their corresponding
header files and as header comments in front of their implementation
in the `.c` file. API contract information should be included,
especially if it is not obvious.

Important APIs should also be covered in the developer documentation,
especially when they have non-obvious lifetime, ownership, wrapping,
threading, or error-handling behavior.

Consider the possibility of creating Unix man pages for the simulator
binaries.
