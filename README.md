# TLAC — TLA+ Trace Validation Library for C

TLAC is a C library for instrumenting imperative programs and validating their
execution traces against a TLA+ specification using TLC. The project implements
a trace-based verification workflow:

1. A C program is instrumented using TLAC.
2. The program produces execution traces in NDJSON format.
3. A corresponding TLA+ Trace Specification checks the trace.
4. TLC validates that the trace conforms to the original specification.

This enables practical conformance checking between implementation and formal
model.

## Build

```sh
nix develop . -c zsh
make
```

## Run


