# libpEpAdapter


## Build Configuration

The build configuration file is called `local.conf`.
Use the file `local.conf.example` as a template.

```bash
cp local.conf.example local.conf
```

Then, tweak it to your needs.

## Make Targets
The default make target is `src`.

### Build
`make src` - Builds the complete library

### Test
`make test` - Builds all tests

### Install
`make install` - Installs the library into your $PREFIX   
`make uninstall` - Uninstalls the library from your $PREFIX

### Clean
`make clean`



