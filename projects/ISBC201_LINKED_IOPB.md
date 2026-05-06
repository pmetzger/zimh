# iSBC 201 Linked IOPB Support

The iSBC 201 controller code currently executes only the IOPB whose
address is written to the controller start port. The hardware manual
describes linked IOPB behavior using the channel word successor, wait,
and branch-on-wait bits, the block number byte, and the next-IOPB
address fields.

The unused-variable warning on the block number exposed that the linked
IOPB fields are fetched but not implemented. Do not treat the block
number as ordinary dead state if linked IOPB support is later added.
Correct support should be manual-backed and tested for:

- successor-bit chaining to the next IOPB address
- branch-on-wait behavior
- intermediate linked-operation interrupts and result type 01
- block-number reporting for linked-operation errors
