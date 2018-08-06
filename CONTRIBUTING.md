#Contributing to Libpull

🚀 First of all, thank you for taking the time to contribute! 🚀

## What should I know before I get started?

If you want to contribute to Libpull you should have understand the
basics of the library. To do it a good approach is to read the
documentation and follow the tutorial to test it on a real device.

## Contributing

If you already did it, then you are ready to contribute it. 
The kind of contribution may be diveded by the contribution
goal.

### Increase portability

The libpull library aims to be a very portable library with a
freestanding core and some platform specific modules.

#### Support a new RTOS.

If you want to integrate a new RTOS we ask you to check
the following points:

- the OS is well known, open source and still maintained;
- you are able to maintain and provide support for a certain period;
- the OS has a minimum number of platforms on which is supported;

If the following checks passes, you can open an issue and discuss
together the integration of the new RTOS.

#### Support a new MCU.

If you want to provide support for a new MCU you should check the
following points:

- you are able to maintain and provide support for a certain period;
- the drivers to interact with the MCU (such as writing memory, etc)
are open source;
- the MCU is still in production.

If the following checks passes, you can open an issue to discuss
the integation of the new MCU. If you already have an implementation
that follows the logic of the library you can may want to open directly
a pull request.

### Reporting bugs.

Please open an issue indicating all the steps
to the reproduce the bug/vulnerability. If you
already have a solution please feel free to open
a pull request, where it will be also easier to
discuss the improvements.

### Improve documentation.

If you found an error or want to improve some documentation
pages plase open a pull request with the fix. To check that
the documentation still builds please follow the steps described
in `build/documentation/README.md`.

## Styleguides

### Git commit messages

For the commit messages we use the
[conventional commits](https://conventionalcommits.org)
standardized approach.
This allows us to have a structured message for each commit
that can be used then to easily generate the changelog for
each new release.

A conventional commit message is composed in the following way:

    <type>[optional scope]: <description>

where type is one of the following prefix:

- feat: introducing a new feature;
- fix: fixing a bug/vulnerability;
- docs: adding/improving documentation;
- style: fixing indentation/code style;
- refactor: refactoring code;
- perf: a code change to improve performance;
- test: adding/fixing unit tests;

Since this process can be tediuos while writing commits we
suggest the [cz-cli tool](https://github.com/commitizen/cz-cli)
that helps in writing commits conformant to the conventional
commits standards.

The optional scope can be used to specify where the type is applied.
For example, in case we are adding a new test for the memory module
we could write a commit message such as:

    test(memory): add a test to detect a failure;

When writing the description considering the following rules:

- Use the present tense ("Fix bug" and not "Fixed bug")
- Use the imperative mood ("Teach library to..." and not "Teaches library to...")

### C code

All the C code must be formatted with [clang-format](https://clang.llvm.org/docs/ClangFormat.html). When submitting PR please avoid reformatting the code until the changes has been approved to increase readibility of the diff.

