# Server Directory Overview

## `./idl`
- Generates table-related source files from IDL definitions and supporting metadata.

## `./node`
- Contains the server process entry target.

## `./makeproto`
- Generates `create_db.sql` from `db.proto`.
- Generates protocol XML files from `net.proto` and associates message structures with protocol ids.

## `./rebuild`
- Legacy maintenance utility target.
- Native CMake unity build now generates combined sources in the build directory instead of checked-in `ub_*.cpp` files.
