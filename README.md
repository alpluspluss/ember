# ember

ember is a compiler infrastructure project I decided to develop. The goal of this project is to create both
frontend and backend for my language, "ash", as well as utility tools and the standard library.

## Status

There isn't much about ember yet as it is a *very new* project and will likely take a substantial
amount of time and effort into creating it. Therefore, please do not expect all the components in this project to be
completed.

- Aarch64 backend covered around 70% of AArch64 instructions with extensions.

As of the time I am writing this, I am designing the language semantics and syntax in which can be read
[here](https://github.com/alpluspluss/ember/tree/main/docs/ash-man.md) in order to understand design decisions on the language.
As for the code optimization and object file
generation, [sparkle](https://github.com/alpluspluss/ember/tree/main/sparkle)
, alongside its subsystems (will) handles all of these operations.

You can check the status of each sub-project in their respective directory, `dir/STATUS.md`.