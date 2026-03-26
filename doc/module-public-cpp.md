# public/cpp Shared Layer

`public/cpp` is the highest-reuse C++ source area in the repository. Runtime
code, tests, and local tools all depend on it directly.

## Directory Roles

- `actor`
  Actor framework, runtime contracts, generated glue, and gameplay logic.
- `net`
  Transport and session layers, including TCP, UDP, WebSocket, and KCP support.
- `tools`
  Shared infrastructure such as logging, config, serialization, database
  helpers, timing, and command-line support.
- `third_party`
  Vendored third-party source and headers.

## Actor Layout

`public/cpp/actor` is now split into these responsibility layers:

- `actor_base`
  Base actor infrastructure.
  - `core`: actor base types and lifecycle primitives
  - `db`: DB actor adapters and DB client helpers
  - `dispatch`: message envelopes and dispatch tables
  - `id`: `nguid` and routing helpers
  - `nsp`: NSP read/write/server helpers
  - `runtime`: scheduler and worker-thread support
  - `script`: script runtime integration
  - `timer`: actor timer support

- `actor_logic`
  Gameplay and domain actors.
  - `access`: login, gateway, csv, and node-facing actors
  - `example`: sample and tutorial actors
  - `forward`: c2g and g2c forward registration
  - `misc`: fallback location for newly generated uncategorized actors
  - `ops`: gm, log, robot, and test-facing actors
  - `player`: role, create, brief, and keyvalue actors
  - `social`: family, friends, chat, mail, notice, and ranklist actors
  - `world`: world events and activity management

- `generated`
  Generated code only.
  - `auto`: actor glue, protocol bindings, script bindings
  - `pb`: protobuf output

- `protocol`
  Shared actor protocol and routing contracts.

- `tab`
  Table/config mappings and runtime lookup helpers.

## Placement Rules

Keep code under `actor` when it is tightly coupled to actor lifecycle,
routing, protocol contracts, or actor-owned domain data.

Do not place purely generic string/container utilities, standalone network
wrappers, or build-only helper logic under `actor`.

## Maintenance Notes

- New `actor_base` code should go to the correct sublayer instead of returning
  to a flat root layout.
- New `actor_logic` modules should pick a domain category before adding files.
- Generated code should stay under `actor/generated` and should not be mixed
  with handwritten logic.