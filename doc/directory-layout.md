# Directory Layout

## Goal

This document describes the repository layout after the current directory cleanup.
The structure now reflects real physical ownership instead of placeholder
aggregation directories.

## Top Level

```text
client/     client-side code and scripts
doc/        architecture and module notes
public/     shared source code reused by runtime, tools, and tests
server/     server runtime targets, local tools, and tests
cmake/      shared build helpers
```

## Public Code

```text
public/cpp/
  actor/        actor framework, runtime contracts, generated glue, gameplay logic
  net/          transport and connection layers
  tools/        general infrastructure utilities
  third_party/  vendored headers and source
```

Rules:

- Keep reusable code in `public/cpp`.
- Do not mix build-only helper code into `public/cpp`.
- Keep generated code clearly separated from handwritten logic.

## Actor Layout

```text
public/cpp/actor/
  actor_base/
    core/       actor base types and common runtime abstractions
    db/         actor DB adapters and DB client helpers
    dispatch/   message envelope and dispatch registration
    id/         nguid and routing address helpers
    nsp/        NSP read/write/server helpers
    runtime/    scheduler and worker-thread support
    script/     script runtime integration
    timer/      actor timer support

  actor_logic/
    access/     login, gateway, session, csv, node-facing actors
    example/    example and tutorial actors
    forward/    c2g / g2c forwarding registration
    misc/       fallback bucket for newly generated uncategorized actors
    ops/        admin, log, robot, test and operation-facing actors
    player/     role, create, brief, keyvalue and player-facing actors
    social/     family, friends, chat, mail, notice, ranklist
    world/      world events and activity management

  generated/
    auto/       generated actor glue and script/protocol bindings
    pb/         generated protobuf source

  protocol/     shared actor protocol and routing contracts
  tab/          table/config mappings and runtime lookup helpers
```

## Server Layout

```text
server/
  runtime/
    libnglcore/
      core/
        bootstrap/  startup and node bootstrap code
    node/
  tools/
    rebuild/
    makeproto/
    idl/
  tests/
```

## Migration Notes

The current cleanup completed these changes:

- `server/` now uses real `runtime / tools / tests` ownership roots.
- `server/runtime/libnglcore/core/bootstrap/` now contains startup assembly code.
- `public/cpp/actor/actor_base/` is now split by runtime responsibility.
- `public/cpp/actor/actor_logic/` keeps the original flat actor-module layout.
- generated actor code now lives under `public/cpp/actor/generated/`.
- tool generators were updated so future output keeps the flat actor-module layout.

## Remaining Risks

- Build verification has not been rerun in this turn.
